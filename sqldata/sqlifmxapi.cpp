
// SqlIfmxApi Informix ODBC API
// Copyright (c) 2012 SQLines. All rights reserved

#include "sqlifmxapi.h"
#include "str.h"
#include "os.h"

// Constructor
SqlIfmxApi::SqlIfmxApi()
{
	_henv = SQL_NULL_HANDLE;
	_hdbc = SQL_NULL_HANDLE;
	_hstmt_cursor = SQL_NULL_HANDLE;

	_cursor_fetched = 0;

	_dll = NULL;

	_SQLAllocHandle = NULL;
	_SQLBindCol = NULL;
	_SQLDescribeCol = NULL;
	_SQLDisconnect = NULL;
	_SQLDriverConnect = NULL;
	_SQLExecDirect = NULL;
	_SQLFetch = NULL;
	_SQLFreeHandle = NULL;
	_SQLGetData = NULL;
	_SQLGetDiagRec = NULL;
	_SQLNumResultCols = NULL;
	_SQLSetConnectAttr = NULL;
	_SQLSetEnvAttr = NULL;
	_SQLSetStmtAttr = NULL;
}

SqlIfmxApi::~SqlIfmxApi()
{
	Deallocate();
}

// Initialize API
int SqlIfmxApi::Init()
{
#if defined(WIN32) || defined(_WIN64)

	// Try to load the library by default path
	_dll = LoadLibraryEx(IFMX_DLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	// DLL load failed
	if(_dll == NULL)
	{
		std::list<std::string> paths;

		// Try to find Informix installation paths
		FindInformixPaths(paths);

		for(std::list<std::string>::iterator i = paths.begin(); i != paths.end() ; i++)
		{
			// For Informix 11.7 path already includes terminating \ and DLL is located in BIN
			std::string path = (*i) + "bin\\";
			path += IFMX_DLL;
			
			// Try to open DLL
			_dll = LoadLibraryEx(path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	
			if(_dll != NULL)
				break;
		}		
	}
#endif

	// Get functions
	if(_dll != NULL)
	{
		// Save the full path of the loaded driver
		Os::GetModuleFileName(_dll, _loaded_driver, 1024);

		_SQLAllocHandle = (SQLAllocHandleFunc)Os::GetProcAddress(_dll, "SQLAllocHandle");
		_SQLBindCol = (SQLBindColFunc)Os::GetProcAddress(_dll, "SQLBindCol");
		_SQLDescribeCol = (SQLDescribeColFunc)Os::GetProcAddress(_dll, "SQLDescribeCol");
		_SQLDisconnect = (SQLDisconnectFunc)Os::GetProcAddress(_dll, "SQLDisconnect");
		_SQLDriverConnect = (SQLDriverConnectFunc)Os::GetProcAddress(_dll, "SQLDriverConnect");
		_SQLExecDirect = (SQLExecDirectFunc)Os::GetProcAddress(_dll, "SQLExecDirect");
		_SQLFetch = (SQLFetchFunc)Os::GetProcAddress(_dll, "SQLFetch");
		_SQLFreeHandle = (SQLFreeHandleFunc)Os::GetProcAddress(_dll, "SQLFreeHandle");
		_SQLGetData = (SQLGetDataFunc)Os::GetProcAddress(_dll, "SQLGetData");
		_SQLGetDiagRec = (SQLGetDiagRecFunc)Os::GetProcAddress(_dll, "SQLGetDiagRec");
		_SQLNumResultCols = (SQLNumResultColsFunc)Os::GetProcAddress(_dll, "SQLNumResultCols");
		_SQLSetConnectAttr = (SQLSetConnectAttrFunc)Os::GetProcAddress(_dll, "SQLSetConnectAttr");
		_SQLSetEnvAttr = (SQLSetEnvAttrFunc)Os::GetProcAddress(_dll, "SQLSetEnvAttr");
		_SQLSetStmtAttr = (SQLSetStmtAttrFunc)Os::GetProcAddress(_dll, "SQLSetStmtAttr");

		if(_SQLAllocHandle == NULL || _SQLBindCol == NULL || _SQLDescribeCol == NULL || 
			_SQLDisconnect == NULL || _SQLDriverConnect == NULL || _SQLExecDirect == NULL || 
			_SQLFetch == NULL || _SQLFreeHandle == NULL || _SQLGetData == NULL || _SQLGetDiagRec == NULL || 
			_SQLSetConnectAttr == NULL || _SQLSetEnvAttr == NULL || _SQLSetStmtAttr == NULL)
			return -1;
	}
	else
	{
		Os::GetLastErrorText(IFMX_DLL_LOAD_ERROR, _native_error_text, 1024);
		return -1;
	}

	return 0;
}

// Set the connection string in the API object
void SqlIfmxApi::SetConnectionString(const char *conn)
{
	if(conn == NULL)
		return;

	std::string db;

	SplitConnectionString(conn, _user, _pwd, db);

	const char *start = db.c_str();

	// Find , that denotes the database name
	const char *comma = strchr(start, ',');

	// Define server and database name
	if(comma != NULL)
	{
		_server.assign(start, (size_t)(comma - start));
		_db = comma + 1;
	}
	else
		_server = start;
}

// Connect to the database
int SqlIfmxApi::Connect(size_t *time_spent)
{
	if(_connected == true)
		return 0;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;

	// Allocate environment handle
	int rc = _SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_henv);

	// Set ODBC 3 version
	if(rc == SQL_SUCCESS)
		rc = _SQLSetEnvAttr(_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

	// Allocate connection handle
	rc = _SQLAllocHandle(SQL_HANDLE_DBC, _henv, &_hdbc);

	// Build connection string
	std::string conn = "Server=";
	conn += _server;
	conn += ";Database=";
	conn += _db;
	conn += ";Uid=";
	conn += _user;
	conn += ";Pwd=";
	conn += _pwd;
	conn += ";";

	// Informix ODBC ignores CLIENT_LOCALE set in SETNET32 and environment variables
	const char *client_locale = NULL;

	if(_parameters != NULL)
		client_locale = _parameters->Get("-informix_client_locale");

	if(client_locale != NULL && *client_locale != '\x0')
	{
		conn += "Client_Locale=";
		conn += client_locale;
		conn += ";";

		// DB_LOCALE must be also set, for example if database is en_US.57372 (UTF-8), and only
		// CLIENT_LOCATE is set to this value, still got Locale mismatch error on connection
		conn += "Db_Locale=";
		conn += client_locale;
		conn += ";";
	}
	// Informix crashes on conversion errors, so set default database locale that is set on 
	// Informix servers (both Windows and Unix)
	else
		conn += "Client_Locale=en_US.819;Db_Locale=en_US.819;";

	SQLCHAR full_conn[1024];
	SQLSMALLINT full_conn_out_len;
	
	rc = _SQLDriverConnect(_hdbc, NULL, (SQLCHAR*)conn.c_str(), SQL_NTS, full_conn, 1024, &full_conn_out_len, SQL_DRIVER_NOPROMPT);
	
	if(rc == SQL_ERROR)
	{
		SetError(SQL_HANDLE_DBC, _hdbc);
		return rc;
	}

	_connected = true;

	// Set UNCOMMITTED READ mode to prevent locks 
	// Will fail if transactions are not available in the database (256: Transaction not available)
	_SQLSetConnectAttr(_hdbc, SQL_ATTR_TXN_ISOLATION, (SQLPOINTER)SQL_TXN_READ_UNCOMMITTED, 0);

	// Identify the version of the connected database
	SetVersion();

	// Initialize session
	InitSession();

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return rc;
}

// Initialize session
int SqlIfmxApi::InitSession()
{
	const char *value = NULL;

	if(_parameters != NULL)
		value = _parameters->Get("-fetch_lob_as_varchar");

	if(value != NULL && *value != '\x0' && 
		(_stricmp(value, "yes") == 0 || _stricmp(value, "true") == 0))
		_cursor_fetch_lob_as_varchar = true;

	return 0;
}

// Disconnect from the database
void SqlIfmxApi::Disconnect() 
{
	if(_connected == false)
		return;

	_SQLDisconnect(_hdbc);
	_SQLFreeHandle(SQL_HANDLE_DBC, _hdbc);
	
	_hdbc = SQL_NULL_HANDLE;
	_connected = false;
}

// Free connection and environment handles 
void SqlIfmxApi::Deallocate()
{
	Disconnect();

	if(_henv != SQL_NULL_HANDLE)
	{
		_SQLFreeHandle(SQL_HANDLE_ENV, _henv);
		_henv = SQL_NULL_HANDLE;
	}
}

// Get row count for the specified object
int SqlIfmxApi::GetRowCount(const char *object, int *count, size_t *time_spent)
{
	if(object == NULL)
		return -1;

	std::string query = "SELECT COUNT(*) FROM ";
	query += object;

	// Execute the query
	int rc = ExecuteScalar(query.c_str(), count, time_spent);

	return rc;
}

// Execute the statement and get scalar result
int SqlIfmxApi::ExecuteScalar(const char *query, int *result, size_t *time_spent)
{
	SQLHANDLE stmt;
	int q_result = 0;

	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = _SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = _SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);

	// Error raised
	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, stmt);
		_SQLFreeHandle(SQL_HANDLE_STMT, stmt);

		return rc;
	}

	// Bind the result
	rc = _SQLBindCol(stmt, 1, SQL_C_SLONG, &q_result, 4, NULL);

	// Fetch the result
	rc = _SQLFetch(stmt);

	// Set the query result
	if(result)
		*result = q_result;

	_SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Execute the statement
int SqlIfmxApi::ExecuteNonQuery(const char *query, size_t *time_spent)
{
	SQLHANDLE stmt;

	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = _SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = _SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);

	// Error raised
	if(rc == -1)
		SetError(SQL_HANDLE_STMT, stmt);

	_SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Open cursor and allocate buffers
int SqlIfmxApi::OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query, std::list<SqlDataTypeMap> *dtmap)
{
	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = _SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &_hstmt_cursor);

	if(rc == -1)
		return -1;

	// Execute the query
	rc = _SQLExecDirect(_hstmt_cursor, (SQLCHAR*)query, SQL_NTS);

	// Error raised
	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);
		_SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_cursor);

		_hstmt_cursor = SQL_NULL_HANDLE;

		return -1;
	}

	// Define the number of columns
	_SQLNumResultCols(_hstmt_cursor, (SQLSMALLINT*)&_cursor_cols_count);

	// Informix can return 0 column if the fetch of the previous table failed with character conversion
	// error (Informix return NO_DATA after an error in one of rows of the previous table)
	if(_cursor_cols_count == 0)
	{
		_SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_cursor);
		_hstmt_cursor = SQL_NULL_HANDLE;

		return -1;
	}

	if(_cursor_cols_count > 0)
		_cursor_cols = new SqlCol[_cursor_cols_count];

	int row_size = 0;

	_cursor_lob_exists = false;

	// Get column information
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		SQLSMALLINT native_dt;
		SQLULEN column_size;
		SQLSMALLINT decimal_digits;
		SQLSMALLINT nullable;

		rc = _SQLDescribeCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLCHAR*)_cursor_cols[i]._name, 256, NULL, &native_dt, &column_size,
					&decimal_digits, &nullable);
      
		// SQL_INTEGER returned for SERIAL
		_cursor_cols[i]._native_dt = native_dt;
		
		// For INTEGER column size is 10, for SMALLINT 5, for DECIMAL contains only p (s is in decimal digits)
		_cursor_cols[i]._len = column_size;

		// For DATETIME save fractional precision (column size already includes length with precision)
		if(native_dt == SQL_TYPE_TIMESTAMP)
			_cursor_cols[i]._scale = decimal_digits;
		else
		// For DECIMAL/NUMERIC column size includes precision only
		if(native_dt == SQL_DECIMAL || native_dt == SQL_NUMERIC)
		{
			_cursor_cols[i]._precision = (int)column_size;
			_cursor_cols[i]._scale = decimal_digits;

			// If DECIMAL created without parameters, it created as floating point with p=16, s=255, convert it to DOUBLE
			if(decimal_digits == 255)
				_cursor_cols[i]._native_dt = SQL_DOUBLE;
		}
		else
		// TEXT (SQL_LONGVARCHAR), CLOB (-103), BYTE (SQL_LONGVARBINARY) and BLOB (-102)
		if(native_dt == SQL_LONGVARCHAR || native_dt == -103 || native_dt == SQL_LONGVARBINARY ||
			native_dt == -102)
		{
			// Fecthed as LOB
			if(_cursor_fetch_lob_as_varchar == false)
			{
				// Change 2G column size to estimated average size (chunk size)
				_cursor_cols[i]._len = 65536 * 2;

				_cursor_cols[i]._lob = true;
				_cursor_lob_exists = true;
			}
			// Fetched as VARCHAR, max length is 32K, SUCCESS_WITH_INFO is returned if column has a large value
			// and error "Data truncated" is set, but length indicator will contain the total size (!) of LOB, 
			// not fetched size
			else
				_cursor_cols[i]._len = 32700;
		}

		if(nullable == SQL_NULLABLE)
			_cursor_cols[i]._nullable = true;
		else
			_cursor_cols[i]._nullable = false;

        // Map the data type
        if(dtmap != NULL)
        {
            GetDataTypeName(native_dt, _cursor_cols[i]._datatype_name);
            MapDataType(&_cursor_cols[i], dtmap);
        }

		row_size += (int)_cursor_cols[i]._len;
	}

	_cursor_allocated_rows = 1;

	// Define how many rows fetch at once
	if(buffer_rows > 0)
		_cursor_allocated_rows = buffer_rows;
	else
	if(buffer_memory > 0)
	{
		size_t rows = (size_t)buffer_memory/row_size;
		_cursor_allocated_rows = rows > 0 ? rows : 1;

		// On some systems unexpected behaviour (long waits or hang) when buffer is too large (100K rows i.e)
		if(_cursor_allocated_rows > 10000)
			_cursor_allocated_rows = 10000;
	}	

	if(_cursor_lob_exists == true)
		_cursor_allocated_rows = 1;

	// Allocate buffers to each column
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		// Data type CHAR or VARCHAR
		if(_cursor_cols[i]._native_dt == SQL_CHAR || _cursor_cols[i]._native_dt == SQL_VARCHAR)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// BIGINT or SQL_INFX_BIGINT (-114)
		if(_cursor_cols[i]._native_dt == SQL_BIGINT || _cursor_cols[i]._native_dt == -114)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = 21;	

			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// INT
		if(_cursor_cols[i]._native_dt == SQL_INTEGER)
		{
			// Check whether target supports bind to INT or string has to be used
			if((_target_api_provider != NULL && _target_api_provider->IsIntegerBulkBindSupported() == true)
				|| catalog_query == true)
			{
				_cursor_cols[i]._native_fetch_dt = SQL_C_LONG;
				_cursor_cols[i]._fetch_len = sizeof(int);				
			}
			else
			{
				_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
				_cursor_cols[i]._fetch_len = 11;	
			}

			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// SMALLINT
		if(_cursor_cols[i]._native_dt == SQL_SMALLINT)
		{
			// Check whether target supports bind to SMALLINT or string has to be used
			if((_target_api_provider != NULL && _target_api_provider->IsSmallintBulkBindSupported() == true) 
				|| catalog_query == true)
			{
				_cursor_cols[i]._native_fetch_dt = SQL_C_SHORT;
				_cursor_cols[i]._fetch_len = sizeof(short);
			}
			else
			{
                // sign, 5 digits and 'x0'
				_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
				_cursor_cols[i]._fetch_len = 7;	
			}

			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// BIT data type (_len is 1)
		if(_cursor_cols[i]._native_dt == SQL_BIT)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// DATE 
		if(_cursor_cols[i]._native_dt == SQL_TYPE_DATE)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// DATETIME
		if(_cursor_cols[i]._native_dt == SQL_TYPE_TIMESTAMP)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_TYPE_TIMESTAMP;
			_cursor_cols[i]._fetch_len = sizeof(SQL_TIMESTAMP_STRUCT);
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// DECIMAL and NUMERIC
		if(_cursor_cols[i]._native_dt == SQL_DECIMAL || _cursor_cols[i]._native_dt == SQL_NUMERIC)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			// Include sign, decimal point and terminating zero
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + _cursor_cols[i]._scale + 3;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// FLOAT (SQL_DOUBLE), SMALLFLOAT (SQL_REAL), REAL (SQL_REAL) and DOUBLE (SQL_DOUBLE)
		if(_cursor_cols[i]._native_dt == SQL_REAL || _cursor_cols[i]._native_dt == SQL_DOUBLE)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			// Driver returns 15 as len for FLOAT, it is too short for string representations
			_cursor_cols[i]._fetch_len = 65 + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// TEXT (SQL_LONGVARCHAR), CLOB (-103), BYTE (SQL_LONGVARBINARY) and BLOB (-102)
		if(_cursor_cols[i]._native_dt == SQL_LONGVARCHAR || _cursor_cols[i]._native_dt == -103 || 
			_cursor_cols[i]._native_dt == SQL_LONGVARBINARY || _cursor_cols[i]._native_dt == -102)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			if(_cursor_cols[i]._native_dt == SQL_LONGVARBINARY || _cursor_cols[i]._native_dt == -102)
				_cursor_cols[i]._native_fetch_dt = SQL_C_BINARY;

			// Fecthed as LOB
			if(_cursor_fetch_lob_as_varchar == false)
			{
				// Will not bound if fecthed as LOB, but still allocate the space for the chunk
				_cursor_cols[i]._fetch_len = _cursor_cols[i]._len;
				_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len];
			}
			// Fetched as VARCHAR
			else
			{
				_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
				_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
			}
		}

		// Bind the data to array
		if(_cursor_cols[i]._data != NULL)
		{
			// Allocate indicators
			_cursor_cols[i].ind = new size_t[_cursor_allocated_rows];

			// Do not bind LOB column
			if(_cursor_cols[i]._lob == false)
			{
				rc = _SQLBindCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLSMALLINT)_cursor_cols[i]._native_fetch_dt, _cursor_cols[i]._data, 
							(SQLLEN)_cursor_cols[i]._fetch_len, (SQLLEN*)_cursor_cols[i].ind);

				if(rc == -1)
					SetError(SQL_HANDLE_STMT, _hstmt_cursor);
			}
		}
	}

	// Prepare array fetch
	rc = _SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)_cursor_allocated_rows, 0); 
	rc = _SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROWS_FETCHED_PTR, &_cursor_fetched, 0);

	// Perform initial fetch, returns SQL_SUCCESS even if there are less rows than array size
	rc = Fetch(NULL, NULL);

	if(col_count != NULL)
		*col_count = _cursor_cols_count; 

	if(allocated_array_rows != NULL)
		*allocated_array_rows = _cursor_allocated_rows;

	if(rows_fetched != NULL)
		*rows_fetched = _cursor_fetched;
	
	if(cols != NULL)
		*cols = _cursor_cols;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Fetch next portion of data to allocate buffers
int SqlIfmxApi::Fetch(int *rows_fetched, size_t *time_spent) 
{
	if(_cursor_allocated_rows <= 0)
		return -1;

	size_t start = GetTickCount();

	// Fetch the data
	int rc = _SQLFetch(_hstmt_cursor);

	// SQL_SUCCESS_WITH_INFO is set when data truncated, and error message "Data truncated"
	if(rc != 0 && rc != 100)
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);

	// Read not bound LOB data (first chunk)
	if((rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) && _cursor_lob_exists == true)
	{
		for(size_t i = 0; i < _cursor_cols_count; i++)
		{
			if(_cursor_cols[i]._lob == false)
				continue;

			// Get the first LOB chunk
			rc = GetLobPart(0, i, NULL, 0, NULL);

			// SQLGetData crashes on next call for this table if SQLGetData for the previous row failed
			if(rc == -1)
				break;
		}
	}

	if(rows_fetched != NULL)
		*rows_fetched = _cursor_fetched;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;
	
	return rc;
}

// Close the cursor and deallocate buffers
int SqlIfmxApi::CloseCursor()
{
	// Close the statement handle
	int rc = _SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_cursor);

	_hstmt_cursor = SQL_NULL_HANDLE;

	if(_cursor_cols == NULL)
		return 0;

	// Delete allocated buffers
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		delete [] _cursor_cols[i]._data;
		delete [] _cursor_cols[i].ind;
	}

	delete [] _cursor_cols;

	_cursor_cols = NULL;
	_cursor_cols_count = 0;
	_cursor_allocated_rows = 0;

	return rc;
}

// Initialize the bulk copy from one database into another
int SqlIfmxApi::InitBulkTransfer(const char * /*table*/, size_t /*col_count*/, size_t /*allocated_array_rows*/, SqlCol * /*s_cols*/, SqlCol ** /*t_cols*/)
{
	return -1;
}

// Transfer rows between databases
int SqlIfmxApi::TransferRows(SqlCol * /*s_cols*/, int /*rows_fetched*/, int * /*rows_written*/, size_t * /*bytes_written*/,
							size_t * /*time_spent*/)
{
	return -1;
}

// Write LOB data using BCP API
int SqlIfmxApi::WriteLob(SqlCol * /*s_cols*/, int /*row*/, int * /*lob_bytes*/)
{
	return -1;
}

// Complete bulk transfer
int SqlIfmxApi::CloseBulkTransfer()
{
	return -1;
}

// Specifies whether API allows to parallel reading from this API and write to another API
bool SqlIfmxApi::CanParallelReadWrite()
{
	if(_cursor_lob_exists == true)
		return false;

	return true;
}

// Drop the table
int SqlIfmxApi::DropTable(const char* /*table*/, size_t * /*time_spent*/, std::string & /*drop_stmt*/)
{
	return -1;
}

// Remove foreign key constraints referencing to the parent table
int SqlIfmxApi::DropReferences(const char* /*table*/, size_t * /*time_spent*/)
{
	return -1;
}

// Get the length of LOB column in the open cursor
int SqlIfmxApi::GetLobLength(size_t /*row*/, size_t /*column*/, size_t * /*length*/)
{
	return -1;
}

// Get LOB content
int SqlIfmxApi::GetLobContent(size_t /*row*/, size_t /*column*/, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	return -1;
}

// Get partial LOB content
int SqlIfmxApi::GetLobPart(size_t /*row*/, size_t column, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	SQLLEN ind = 0;

	int rc = _SQLGetData(_hstmt_cursor, (SQLUSMALLINT)(column + 1), (SQLSMALLINT)_cursor_cols[column]._native_fetch_dt, 
					_cursor_cols[column]._data, (SQLLEN)_cursor_cols[column]._fetch_len, &ind);

	_cursor_cols[column]._lob_fetch_status = rc;

	// If SQLGetData returns -1, for example for error 21005 (Inexact character conversion)
	// next call to SQLGetData crashes (!), so the processing of this table must be stopped 
	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);
		return rc;
	}

	// NULL value in LOB
	if(ind == -1)
		_cursor_cols[column].ind[0] = (size_t)-1;
	else
	// TEXT and CLOB (-103)
	if(_cursor_cols[column]._native_dt == SQL_LONGVARCHAR || _cursor_cols[column]._native_dt == -103)
	{
		// When buffer is fully filled, the last byte in the buffer is x0
		if(ind >= (SQLLEN)_cursor_cols[column]._fetch_len)
			_cursor_cols[column].ind[0] = _cursor_cols[column]._fetch_len - 1;
		// For last chunk x0 not included (!) to ind 
		else
			_cursor_cols[column].ind[0] = (size_t)ind;
	}
	else
	// When BLOB value larger then the buffer Informix returns full LOB size in ind
	if(_cursor_cols[column]._native_dt == SQL_LONGVARBINARY || _cursor_cols[column]._native_dt == -102)
	{
		// BLOB value part not terminated by 0x
		if(ind >= (SQLLEN)_cursor_cols[column]._fetch_len)
			_cursor_cols[column].ind[0] = _cursor_cols[column]._fetch_len;
		else
			_cursor_cols[column].ind[0] = (size_t)ind;
	}
	
	return rc;
}

// Get the list of available tables
int SqlIfmxApi::GetAvailableTables(std::string &select, std::string &exclude, std::list<std::string> &tables)
{
	std::string condition;

	// Get a condition to select objects from the catalog 
	// * means all user tables, *.* means all tables accessible by the user
	GetSelectionCriteria(select.c_str(), exclude.c_str(), "owner", "tabname", condition, _user.c_str(), false);

	// Build the query
	std::string query = "SELECT owner, tabname FROM systables WHERE tabtype='T' ";

	AddSystemTablesFilter("", query);

	// Add filter
	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}

	SQLHANDLE stmt;

	// Allocate a statement handle
	int rc = _SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = _SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

	// Error raised
	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, stmt);
		_SQLFreeHandle(SQL_HANDLE_STMT, stmt); 

		return -1;
	}

	char owner[1024], name[1024];
	SQLLEN cbOwner = 0, cbName = 0;

	*owner = '\x0';
	*name = '\x0';

	rc = _SQLBindCol(stmt, 1, SQL_C_CHAR, (SQLCHAR*)owner, 1024, &cbOwner);
	rc = _SQLBindCol(stmt, 2, SQL_C_CHAR, (SQLCHAR*)name, 1024, &cbName);

    // Fetch tables
	while(rc == 0)
	{
		rc = _SQLFetch(stmt);

		if(rc != SQL_SUCCESS)
			break;

		// Informix returns owner name padded with spaces
		Str::TrimTrailingSpaces(owner);

		std::string tab = owner;
		tab += ".";
		tab += name;

		tables.push_back(tab);
	}

	_SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return (rc == SQL_NO_DATA ) ? 0 : rc;
}

// Read schema information
int SqlIfmxApi::ReadSchema(const char *select, const char *exclude, bool read_cns, bool read_idx)
{
	std::string condition;

	ClearSchema();

	// Get a condition to select objects from the catalog
	// User can see multiple schemas in the database
	// * means all user tables, *.* means all tables accessible by the user
	GetSelectionCriteria(select, exclude, "t.owner", "t.tabname", condition, _user.c_str(), false);

	int rc = ReadTableColumns(condition);
	rc = ReadIdentityColumns(condition);	
	rc = ReadDefaults(condition);	

	if(read_cns)
	{
		rc = ReadTableConstraints(condition);
		rc = ReadCheckConstraints(condition);
		rc = ReadReferences(condition);
	}

	if(read_idx)
		rc = ReadIndexes(condition);

	return rc;
}

// Read information about table columns
int SqlIfmxApi::ReadTableColumns(std::string &condition)
{
	std::string query = "SELECT t.owner, tabname, colname, c.colno, c.tabid, c.coltype";
	query += " FROM syscolumns c, systables t WHERE c.tabid = t.tabid AND t.tabid >= 100";

	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}
	
	query += " ORDER BY c.tabid, c.colno";
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlColMeta col_meta;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, 
		&time_read, true);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			SQLLEN len;

			col_meta.schema = NULL;
			col_meta.table = NULL;
			col_meta.column = NULL;
			col_meta.num = 0;
			col_meta.tabid = 0;
			col_meta.data_type_code = 0;
			
			len = (SQLLEN)cols[0].ind[i];

			// Schema
			if(len != -1)
			{
				col_meta.schema = new char[(size_t)len + 1];

				strncpy(col_meta.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				col_meta.schema[len] = '\x0';

				// Schema name is right-padded with spaces
				Str::TrimTrailingSpaces(col_meta.schema);
			}

			len = (SQLLEN)cols[1].ind[i];

			// Table
			if(len != -1)
			{
				col_meta.table = new char[(size_t)len + 1];

				strncpy(col_meta.table, cols[1]._data + cols[1]._fetch_len * i, (size_t)len);
				col_meta.table[len] = '\x0';
			}

			len = (SQLLEN)cols[2].ind[i];

			// Column
			if(len != -1)
			{
				col_meta.column = new char[(size_t)len + 1];

				strncpy(col_meta.column, cols[2]._data + cols[2]._fetch_len * i, (size_t)len);
				col_meta.column[len] = '\x0';
			}
			
			len = (SQLLEN)cols[3].ind[i];

			// Column number (SMALLINT)
			if(len == 2)
				col_meta.num = *((short*)(cols[3]._data + cols[3]._fetch_len * i));

			len = (SQLLEN)cols[4].ind[i];

			// Table ID (INTEGER)
			if(len == 4)
				col_meta.tabid = *((int*)(cols[4]._data + cols[4]._fetch_len * i));

			len = (SQLLEN)cols[5].ind[i];

			// Data type code (SMALLINT)
			if(len == 2)
				col_meta.data_type_code = *((short*)(cols[5]._data + cols[5]._fetch_len * i));
			
			_table_columns.push_back(col_meta);
		}

		rc = Fetch(&rows_fetched, &time_read);

		// No more rows
		if(rc == 100)
			break;
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	col_meta.schema = NULL; 
	col_meta.table = NULL; 
	col_meta.column = NULL; 
	col_meta.default_value = NULL;

	CloseCursor();

	return 0;
}

// Read information about identity columns
int SqlIfmxApi::ReadIdentityColumns(std::string &condition)
{
	// If SERIAL is NOT NULL, code is 262 (NOT NULL added automatically by DB-Access)
	std::string query = "SELECT p.serialv, c.colno, c.tabid";
	query += " FROM syscolumns c, systables t, sysmaster:sysptnhdr p WHERE ";
	query += " c.tabid = t.tabid AND p.partnum = t.partnum AND";
	query += " c.coltype IN (6, 262) AND t.tabid >= 100";

	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, 
		&time_read, true);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			int serialv = 0;
			int colno = 0;
			int tabid = 0;
			
			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Serialv (INTEGER)
			if(len == 4)
				serialv = *((int*)(cols[0]._data + cols[0]._fetch_len * i));
			
			len = (SQLLEN)cols[1].ind[i];

			// Column number (SMALLINT)
			if(len == 2)
				colno = *((short*)(cols[1]._data + cols[1]._fetch_len * i));

			len = (SQLLEN)cols[2].ind[i];

			// Table ID (INTEGER)
			if(len == 4)
				tabid = *((int*)(cols[2]._data + cols[2]._fetch_len * i));

			// Find column and set identity parameters
			for(std::list<SqlColMeta>::iterator m = _table_columns.begin(); m != _table_columns.end(); m++)
			{
				if((*m).tabid == tabid && (*m).num == colno)
				{
					(*m).identity = true;
					(*m).id_start = 1;
					(*m).id_inc = 1;
					(*m).id_next = serialv;

					break;
				}
			}
		}

		rc = Fetch(&rows_fetched, &time_read);

		// No more rows
		if(rc == 100)
			break;
	}

	CloseCursor();

	return 0;
}

// Read information about column default values
int SqlIfmxApi::ReadDefaults(std::string &condition)
{
	std::string query = "SELECT d.tabid, d.colno, d.type, d.default";
	query += " FROM sysdefaults d, systables t WHERE d.tabid = t.tabid AND t.tabid >= 100 ";
	
	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, 
		&time_read, true); 

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			int tabid = 0;
			int colno = 0;
			char type = '\x0';

			char *default_v = NULL;
			size_t default_len = 0; 
			
			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Table ID (INTEGER)
			if(len == 4)
				tabid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			len = (SQLLEN)cols[1].ind[i];

			// Column number (SMALLINT)
			if(len == 2)
				colno = *((short*)(cols[1]._data + cols[1]._fetch_len * i));

			len = (SQLLEN)cols[2].ind[i];

			// Default type CHAR(1)
			if(len == 1)
				type = *(cols[2]._data + cols[2]._fetch_len * i);

			// Default value
			if(cols[3].ind[i] != -1)
			{
				default_v = cols[3]._data + cols[3]._fetch_len * i;
				default_len = (size_t)cols[3].ind[i];
			}
			
			// Find the column and set the default
			for(std::list<SqlColMeta>::iterator m = _table_columns.begin(); m != _table_columns.end(); m++)
			{
				if((*m).tabid == tabid && (*m).num == colno)
				{
					SetDefault((*m), type, default_v, default_len);
					break;
				}
			}
		}

		rc = Fetch(&rows_fetched, &time_read);

		// No more rows
		if(rc == 100)
			break;
	}

	CloseCursor();

	return 0;
}

// Set column default value when reading the catalog
void SqlIfmxApi::SetDefault(SqlColMeta &col, char type, char *value, size_t len)
{
	// String (non-quoted) or number literal
	if(type == 'L')
	{
		int dc = col.data_type_code;

		// CHAR - 0, CHAR NOT NULL - 256
		if(dc == 0 || dc == 256)
		{
			// Space for single quotes
			col.default_value = new char[len + 2 + 1];

			col.default_value[0] = '\'';
			strncpy(col.default_value + 1, value, len);

			// Prepare for trim
			col.default_value[len + 1] = '\x0';

			Str::TrimTrailingSpaces(col.default_value);

			size_t l = strlen(col.default_value);

			// Append close quote and terminate
			col.default_value[l] = '\'';
			col.default_value[l + 1] = '\x0';

			col.default_type = type;
			col.no_default = false;
		}
		else
		// A number SMALLINT - 1, INTEGER - 2, FLOAT - 3, SMALLFLOAT - 4, DECIMAL - 5 (if NOT NULL +256)
		if((dc >= 1 && dc <= 5) || (dc >= 257 && dc <= 261))
		{
			const char *cur = value;
			int i = 0;

			// Default value contains binary representation, a space and then ASCII value of the number
			while(i < len)
			{
				if(cur[i] == ' ')
				{
					i++;
					break;
				}

				i++;
			}

			col.default_value = (char*)Str::GetCopy(cur + i, len - i);
			col.default_value[len - i] = '\x0';

			// Default value contains trailing spaces
			Str::TrimTrailingSpaces(col.default_value);

			col.default_type = type;
			col.no_default = false;
		}
	}
}

// Read information about table constraints
int SqlIfmxApi::ReadTableConstraints(std::string &condition)
{
	std::string query = "SELECT t.owner, tabname, constrname, constrtype, c.tabid, c.idxname, c.constrid";
	query += " FROM sysconstraints c, systables t WHERE ";
	query += " c.tabid = t.tabid AND constrtype in ('P', 'U', 'R', 'C') ";

	AddSystemTablesFilter("t.", query);

	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlConstraints col_cns;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			// Get constraint type
			col_cns.type = (cols[3].ind[i] != -1) ? *(cols[3]._data + cols[3]._fetch_len * i) : '\x0';

			col_cns.condition = NULL;

			// Schema
			if(cols[0].ind[i] != -1)
			{
				col_cns.schema = new char[(size_t)cols[0].ind[i] + 1];

				strncpy(col_cns.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0].ind[i]);
				col_cns.schema[cols[0].ind[i]] = '\x0';

				// Schema name is right-padded with spaces
				Str::TrimTrailingSpaces(col_cns.schema);
			}
			else
				col_cns.schema = NULL;

			// Table
			if(cols[1].ind[i] != -1)
			{
				col_cns.table = new char[(size_t)cols[1].ind[i] + 1];

				strncpy(col_cns.table, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1].ind[i]);
				col_cns.table[cols[1].ind[i]] = '\x0';
			}
			else
				col_cns.table = NULL;

			// Constraint name
			if(cols[2].ind[i] != -1)
			{
				col_cns.constraint = new char[(size_t)cols[2].ind[i] + 1];

				strncpy(col_cns.constraint, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2].ind[i]);
				col_cns.constraint[cols[2].ind[i]] = '\x0';
			}
			else
				col_cns.constraint = NULL;

			SQLLEN len = (SQLLEN)cols[4].ind[i];

			// Table ID (INTEGER)
			if(len == 4)
				col_cns.tabid = *((int*)(cols[4]._data + cols[4]._fetch_len * i));

			len = (SQLLEN)cols[5].ind[i];

			// Index name
			if(len != -1)
			{
				col_cns.idxname = new char[(size_t)len + 1];

				strncpy(col_cns.idxname, cols[5]._data + cols[5]._fetch_len * i, (size_t)len);
				col_cns.idxname[len] = '\x0';
			}
			else
				col_cns.idxname = NULL;

			len = (SQLLEN)cols[6].ind[i];

			// Constraint ID (INTEGER)
			if(len == 4)
				col_cns.cnsid = *((int*)(cols[6]._data + cols[6]._fetch_len * i));

			_table_constraints.push_back(col_cns);
		}

		rc = Fetch(&rows_fetched, &time_read);

		// No more rows
		if(rc == 100)
			break;
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	col_cns.schema = NULL; 
	col_cns.table = NULL; 
	col_cns.constraint = NULL; 
	col_cns.condition = NULL; 
	col_cns.r_schema = NULL; 
	col_cns.r_constraint = NULL;
	col_cns.idxname = NULL;

	CloseCursor();

	return 0;
}

// Read CHECK constraint conditions 
int SqlIfmxApi::ReadCheckConstraints(std::string & /*condition*/)
{
	// Read all CHECK constraints, filter will be applied by constrid
	std::string query = "SELECT constrid, seqno, checktext";
	query += " FROM syschecks WHERE type = 'T'"; 
	query += " ORDER BY constrid, seqno"; 
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	int prev_cnsid = -1;

	// Text can span multiple rows
	std::string checktext;

	while(rc >= 0)
	{		
		for(int i = 0; i < rows_fetched; i++)
		{
			int cnsid = -1;
			int seqno = -1;

			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Constraint ID (INTEGER)
			if(len == 4)
				cnsid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			// Save previous constraint and reset CHECK text
			if(prev_cnsid != -1 && cnsid != prev_cnsid)
			{
				SetCheckConstraint(prev_cnsid, checktext);
				checktext.clear();
			}

			prev_cnsid = cnsid;

			len = (SQLLEN)cols[1].ind[i];

			// Line number of condition (SMALLINT)
			if(len == 2)
				seqno = *((short*)(cols[1]._data + cols[1]._fetch_len * i));

			len = (SQLLEN)cols[2].ind[i];

			// CHECK condition
			if(len != -1)
				checktext.append(cols[2]._data + cols[2]._fetch_len * i, (size_t)len);
		}

		rc = Fetch(&rows_fetched, &time_read);

		// No more rows
		if(rc == 100)
		{
			SetCheckConstraint(prev_cnsid, checktext);
			break;
		}
	}

	CloseCursor();

	return 0;
}

// Save CHECK constraint condition
void SqlIfmxApi::SetCheckConstraint(int cnsid, std::string &condition)
{
	if(condition.empty() == true)
		return;

	// Find CHECK constraint and set condition
	for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
	{
		if((*i).cnsid == cnsid)
		{
			char *check = new char[condition.size() + 1];
			strcpy(check, condition.c_str());
				
			(*i).condition = check;
			break;
		}
	}
}

// Read references between foreign key and primary key constraints 
int SqlIfmxApi::ReadReferences(std::string & /*condition*/)
{
	std::string query = "SELECT constrid, primary FROM sysreferences";
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, 
		&time_read, true);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			int cnsid = 0;
			int r_cnsid = 0;

			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Foreign key constraint ID (INTEGER)
			if(len == 4)
				cnsid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			len = (SQLLEN)cols[1].ind[i];

			// Primary key constraint ID (INTEGER)
			if(len == 4)
				r_cnsid = *((int*)(cols[1]._data + cols[1]._fetch_len * i));

			// Find foreign key constraint and set primary key
			for(std::list<SqlConstraints>::iterator m = _table_constraints.begin(); m != _table_constraints.end(); m++)
			{
				if((*m).cnsid == cnsid)
				{
					(*m).r_cnsid = r_cnsid;
					break;
				}
			}
		}

		rc = Fetch(&rows_fetched, &time_read);

		// No more rows
		if(rc == 100)
			break;
	}

	CloseCursor();

	return 0;
}

// Read information about indexes and index columns
int SqlIfmxApi::ReadIndexes(std::string &condition)
{
	// Note that i.owner for indexes created for FK is 'informix' even if table is not in informix schema
	std::string query = "SELECT i.owner, i.idxname, t.owner, t.tabname, i.tabid, i.idxtype, ";
	query += "part1, part2, part3, part4, part5, part6, part7, part8, "; 
	query += "part9, part10, part11, part12, part13, part14, part15, part16 "; 
	query += "FROM sysindexes i, systables t WHERE i.tabid = t.tabid ";

	AddSystemTablesFilter("t.", query);

	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlIndexes idx;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, 
		&time_read, true);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			SQLLEN len;

			idx.schema = NULL;
			idx.index = NULL;
			idx.t_schema = NULL;
			idx.t_name = NULL;
			idx.tabid = 0;
			
			len = (SQLLEN)cols[0].ind[i];

			// Index owner
			if(len != -1)
			{
				idx.schema = new char[(size_t)len + 1];

				strncpy(idx.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				idx.schema[len] = '\x0';

				// Schema name is right-padded with spaces
				Str::TrimTrailingSpaces(idx.schema);
			}

			len = (SQLLEN)cols[1].ind[i];

			// Index name
			if(len != -1)
			{
				idx.index = new char[(size_t)len + 1];

				strncpy(idx.index, cols[1]._data + cols[1]._fetch_len * i, (size_t)len);
				idx.index[len] = '\x0';
			}

			len = (SQLLEN)cols[2].ind[i];

			// Table schema
			if(len != -1)
			{
				idx.t_schema = new char[(size_t)len + 1];

				strncpy(idx.t_schema, cols[2]._data + cols[2]._fetch_len * i, (size_t)len);
				idx.t_schema[len] = '\x0';

				// Schema name is right-padded with spaces
				Str::TrimTrailingSpaces(idx.t_schema);
			}

			len = (SQLLEN)cols[3].ind[i];

			// Table name
			if(len != -1)
			{
				idx.t_name = new char[(size_t)len + 1];

				strncpy(idx.t_name, cols[3]._data + cols[3]._fetch_len * i, (size_t)len);
				idx.t_name[len] = '\x0';
			}

			len = (SQLLEN)cols[4].ind[i];

			// Table ID - INTEGER
			if(len == 4)
				idx.tabid = *((int*)(cols[4]._data + cols[4]._fetch_len * i));

			len = (SQLLEN)cols[5].ind[i];

			// Index type - CHAR(1)
			if(len == 1)
			{
				char type = *(cols[5]._data + cols[5]._fetch_len * i);

				if(type == 'U')
					idx.unique = true;
				else
					idx.unique = false;
			}

			bool pk_index = false;
            bool uq_index = false;

			// Check if this index was created by a key constraint
			for(std::list<SqlConstraints>::iterator m = _table_constraints.begin(); m != _table_constraints.end(); m++)
			{
				if((*m).tabid == idx.tabid && ((*m).type == 'P' || (*m).type == 'U'))
				{
					if((*m).idxname != NULL && strcmp((*m).idxname, idx.index) == 0)
					{
                        if((*m).type == 'P')
						    pk_index = true;
                        else
                            uq_index = true;

						break;
					}
				}
			}

            // Define index columns
			for(int k = 0; k < 16; k++)
			{
				len = (SQLLEN)cols[6+k].ind[i];

				short colno = *((short*)(cols[6+k]._data + cols[6+k]._fetch_len * i));

				// No more columns in the index
				if(colno == 0)
					break;

				SqlIndColumns idx_col;

				idx_col.schema = (char*)Str::GetCopy(idx.schema);
				idx_col.index = (char*)Str::GetCopy(idx.index);
				idx_col.tabid = idx.tabid;

				// Find column name
				for(std::list<SqlColMeta>::iterator m = _table_columns.begin(); m != _table_columns.end(); m++)
				{
					if((*m).tabid == idx_col.tabid && (*m).num == colno)
					{
						idx_col.column = (char*)Str::GetCopy((*m).column);

                        // Force NOT NULL for primary key columns
                        if(pk_index)
                        {
                            (*m).nullable = false;
                            (*m).pk_column = true;
                        }

   						break;
					}
				}

				_table_ind_columns.push_back(idx_col);

				// Pointers now belong to the list
				idx_col.schema = NULL;
				idx_col.index = NULL;
				idx_col.column = NULL;
			}

			// Add only indexes not generated for PRIMARY and UNIQUE keys
			if(pk_index == false && uq_index == false)
				_table_indexes.push_back(idx);
			else
			{
				delete idx.schema; 
				delete idx.index; 
				delete idx.t_schema; 
				delete idx.t_name;
			}
		}

		rc = Fetch(&rows_fetched, &time_read);

		// No more rows
		if(rc == 100)
			break;
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	idx.schema = NULL; 
	idx.index = NULL; 
	idx.t_schema = NULL; 
	idx.t_name = NULL; 

	CloseCursor();

	return 0;
}

// Get table name by constraint name
int SqlIfmxApi::ReadConstraintTable(const char * /*schema*/, const char * /*constraint*/, std::string & /*table*/)
{
	return -1;
}

// Read information about constraint columns
int SqlIfmxApi::ReadConstraintColumns(const char * /*schema*/, const char * /*table*/, const char * /*constraint*/, std::string & /*cols*/)
{
	return -1;
}

// Filter system tables
void SqlIfmxApi::AddSystemTablesFilter(const char *alias, std::string &query)
{
	// Systems views are in 'informix' schema and start with 'sys%', but user objects also be in this schema and
	// start with 'sys%', but their tabid starts with 100
	query += "AND ";
	query += alias;
	query += "tabid >= 100 ";

	// Exclude DataBlade system tables (they have tabid > 100)
	query += "AND NOT (";
	query += alias;
	query += "owner = 'informix' AND ";
	query += alias;
	query += "tabname IN ('sysblderrorlog', 'sysbldregistered')) ";
}

// Get a list of columns for specified primary or unique key
int SqlIfmxApi::GetKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &output, std::list<std::string> *)
{
	bool found = false;

	// Find index columns
	for(std::list<SqlIndColumns>::iterator i = _table_ind_columns.begin(); i != _table_ind_columns.end(); i++)
	{
		if((*i).tabid != cns.tabid)
		{
			// Not found yet
			if(found == false)
				continue;
			// Next index
			else
				break;
		}

		if((*i).index != NULL && cns.idxname != NULL && strcmp((*i).index, cns.idxname) == 0)
		{
			output.push_back((*i).column);
			found = true;
		}
	}
				
	return 0;
}

// Get a list of columns for specified foreign key
int SqlIfmxApi::GetForeignKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &fcols, std::list<std::string> &pcols, 
												std::string &ptable)
{
	// In Informix, each foreign key creates an index, so we can find columns the same way as for primary key
	GetKeyConstraintColumns(cns, fcols);

	// Find constraint for primary key
	for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
	{
		if((*i).cnsid == cns.r_cnsid)
		{
			GetKeyConstraintColumns((*i), pcols);
			
			ptable = (*i).schema;
			ptable += ".";
			ptable += (*i).table;
			
			break;
		}
	}

	return 0;
}

// Get a list of columns for specified index
int SqlIfmxApi::GetIndexColumns(SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts)
{
    int c = 0;

	// Find index columns
	for(std::list<SqlIndColumns>::iterator i = _table_ind_columns.begin(); i != _table_ind_columns.end(); i++)
	{
		if((*i).tabid != idx.tabid)
		{
			// Not found yet
			if(c == 0)
				continue;
			// Next index
			else
				break;
		}

		// Table is the same, so just compare index names
		if((*i).index != NULL && idx.index != NULL && strcmp((*i).index, idx.index) == 0)
		{
			idx_cols.push_back((*i).column);
			idx_sorts.push_back("");
			c++;
		}
	}

	return 0;
}

// Get data type name by code
void SqlIfmxApi::GetDataTypeName(SQLSMALLINT native_dt, std::string &name)
{
    if(native_dt == SQL_CHAR)
        name = "CHAR";
    else if(native_dt == SQL_DECIMAL)
        name = "DECIMAL";
    else if (native_dt == SQL_NUMERIC)
        name = "NUMERIC";
    else if (native_dt == SQL_TYPE_DATE)
        name = "DATE";    
}

// Set version of the connected database
void SqlIfmxApi::SetVersion()
{
	// Tested on Informix 11.7 (Includes product name - IBM Informix Dynamic Server Version 11.70.TC5DE) 
	// T means 32-bit Windows, U - 32-bit Unix, F - 64-bit Windows/Unix
	const char *query = "SELECT dbinfo('version', 'full') FROM systables WHERE tabid=1";

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor 
	int rc = OpenCursor(query, 1, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	// Get the value
	if(rc >= 0 && cols != NULL && cols[0].ind != NULL)
	{
		SQLLEN len = (SQLLEN)cols[0].ind[0];

		if(len != -1 && len < 127)
			_version.assign(cols[0]._data, (size_t)len);
	}

	CloseCursor();
}

// Find Informix installation paths
void SqlIfmxApi::FindInformixPaths(std::list<std::string> &paths)
{
#if defined(WIN32) || defined(_WIN64)
	HKEY hKey;

	// At least for Informix 11.7 server installation
	int rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Informix\\Environment", 0, KEY_READ, &hKey);

	if(rc != ERROR_SUCCESS)
		return;

	char location[1024];
	int value_size = 1024;

	// For 11.7 "INFORMIXDIR" parameter contains installation directory WITH terminating '\'
	rc = RegQueryValueEx(hKey, "INFORMIXDIR", NULL, NULL, (LPBYTE)location, (LPDWORD)&value_size); 

	if(rc == ERROR_SUCCESS)
		paths.push_back(std::string(location));

	RegCloseKey(hKey);

#endif
}

// Set error code and message for the last API call
void SqlIfmxApi::SetError(SQLSMALLINT handle_type, SQLHANDLE handle)
{
	SQLCHAR sqlstate[6]; *sqlstate = '\x0';

	// Get native error information
	_SQLGetDiagRec(handle_type, handle, 1, sqlstate, (SQLINTEGER*)&_native_error, (SQLCHAR*)_native_error_text, 1024, NULL);

	_error = SQL_DBAPI_UNKNOWN_ERROR;
	*_error_text = '\x0';
}
