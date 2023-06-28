
// SqlAsaApi Sybase SQL Anywhere API
// Copyright (c) 2012 SQLines. All rights reserved

#include <string.h>
#include <string>
#include "str.h"
#include "sqlasaapi.h"
#include "os.h"

// MySQL C API definitions
#include <mysql.h>

// Constructor
SqlAsaApi::SqlAsaApi()
{
	_henv = SQL_NULL_HANDLE;
	_hdbc = SQL_NULL_HANDLE;
	_hstmt_cursor = SQL_NULL_HANDLE;

	_cursor_fetched = 0;

	_error = 0;
	_error_text[0] = '\x0';

	_native_error = 0;
	_native_error_text[0] = '\x0';

	_char_as_wchar = false;
}

SqlAsaApi::~SqlAsaApi()
{
	// When other ODBC drivers are used crash can happen on exit when not disconnected and not deallocated
	Deallocate();
}

// Initialize API
int SqlAsaApi::Init()
{
	// No driver load or initialization is required as ODBC Manager must be used
	return 0;
}

// Set the connection string in the API object
void SqlAsaApi::SetConnectionString(const char *conn)
{
	if(conn == NULL)
		return;

	SplitConnectionString(conn, _user, _pwd, _dsn);
}

// Connect to the database
int SqlAsaApi::Connect(size_t *time_spent)
{
	if(_connected == true)
		return 0;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;

	std::string conn;

	// Allocate environment handle
	int rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_henv);

	// Set ODBC 3 version
	if(rc == SQL_SUCCESS)
		rc = SQLSetEnvAttr(_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

	// Allocate connection handle
	rc = SQLAllocHandle(SQL_HANDLE_DBC, _henv, &_hdbc);

	// Build connection string, add server
	if(_dsn.empty() == false)
	{
		conn += "DSN=";
		conn += _dsn;
		conn += ";";
	}

	// Add login information
	if(_user.empty() == false)
	{
		conn += "UID=";
		conn += _user;
		conn += ";";
	}

	if(_pwd.empty() == false)
	{
		conn += "PWD=";
		conn += _pwd;
		conn += ";";
	}

	rc = SQLDriverConnect(_hdbc, NULL, (SQLCHAR*)conn.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

	if(rc == SQL_ERROR)
	{
		SetError(SQL_HANDLE_DBC, _hdbc);
		return -1;
	}

	// Initialize session
	InitSession();

	// Set session attributes
	rc = SetSession();

	// Identify the version of the connected database
	SetVersion();

	_connected = true;

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return rc;
}

// Disconnect from the database
void SqlAsaApi::Disconnect() 
{
	if(_connected == false)
		return;

	SQLDisconnect(_hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, _hdbc);
	
	_hdbc = SQL_NULL_HANDLE;
	_connected = false;
}

// Free connection and environment handles 
void SqlAsaApi::Deallocate()
{
	Disconnect();

	if(_henv != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, _henv);
		_henv = SQL_NULL_HANDLE;
	}
}

// Get row count for the specified object
int SqlAsaApi::GetRowCount(const char *object, int *count, size_t *time_spent)
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
int SqlAsaApi::ExecuteScalar(const char *query, int *result, size_t *time_spent)
{
	SQLHANDLE stmt;
	int q_result = 0;

	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);

	// Error raised
	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, stmt);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);

		return rc;
	}

	// Bind the result
	rc = SQLBindCol(stmt, 1, SQL_C_SLONG, &q_result, 4, NULL);

	// Fetch the result
	rc = SQLFetch(stmt);

	// Set the query result
	if(result)
		*result = q_result;

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Execute the statement
int SqlAsaApi::ExecuteNonQuery(const char *query, size_t *time_spent)
{
	SQLHANDLE stmt;

	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = SQLExecDirect(stmt, (SQLCHAR*)query, SQL_NTS);

	// Error raised
	if(rc == -1)
		SetError(SQL_HANDLE_STMT, stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Open cursor and allocate buffers
int SqlAsaApi::OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query, std::list<SqlDataTypeMap> *dtmap)
{
	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &_hstmt_cursor);

	// Execute the query
	rc = SQLExecDirect(_hstmt_cursor, (SQLCHAR*)query, SQL_NTS);

	// Error raised
	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);
		SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_cursor);

		_hstmt_cursor = SQL_NULL_HANDLE;

		return -1;
	}

	// Define the number of columns
	rc = SQLNumResultCols(_hstmt_cursor, (SQLSMALLINT*)&_cursor_cols_count);

	if(_cursor_cols_count > 0)
		_cursor_cols = new SqlCol[_cursor_cols_count];

	size_t row_size = 0;

	_cursor_lob_exists = false;

	// Get column information
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		SQLSMALLINT native_dt;
		SQLULEN column_size;
		SQLSMALLINT decimal_digits;
		SQLSMALLINT nullable;

		rc = SQLDescribeCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLCHAR*)_cursor_cols[i]._name, 256, NULL, &native_dt, 
					&column_size, &decimal_digits, &nullable);
      
		_cursor_cols[i]._native_dt = native_dt;
		_cursor_cols[i]._len = column_size;

		// All char data is set to be extracted as wide char (UTF-16/UCS-2)
		if((native_dt == SQL_CHAR || native_dt == SQL_VARCHAR) && !catalog_query && _char_as_wchar)
		{
			_cursor_cols[i]._native_dt = (native_dt == SQL_CHAR) ? SQL_WCHAR : SQL_WVARCHAR;
		}
		else
		// For DECIMAL/NUMERIC column size includes precision only
		if(native_dt == SQL_DECIMAL || native_dt == SQL_NUMERIC)
		{
			// Precision is 19 for MONEY, and 10 for SMALLMONEY
			_cursor_cols[i]._precision = (int)column_size;

			// Scale is 4 for MONEY and SMALLMONEY
			_cursor_cols[i]._scale = decimal_digits;
		}
		else
		// LONG VARCHAR and SQL_LONGVARBINARY (size is 2,000,000,000 returned for ASA 9)
		if(native_dt == SQL_LONGVARCHAR || native_dt == SQL_LONGVARBINARY || native_dt == SQL_WLONGVARCHAR)
		{
			// Fecthed as LOB (CHECK constraints and DEFAULT values defined as LONG VARCHAR in catalog)
			if(_cursor_fetch_lob_as_varchar == false && catalog_query == false)
			{
				// Change 2G column size to estimated average size (chunk size of 3 MB)
				_cursor_cols[i]._len = 65536 * 16 * 3;

				_cursor_cols[i]._lob = true;
				_cursor_lob_exists = true;
			}
			else
				_cursor_cols[i]._len = 32700;
		}
		else
		// TIME data type (fractional precision is always 6, no option to specify other value)
		if(native_dt == SQL_TYPE_TIME)
		{
			// Set fractions to 6 as ODBC does not support fractions in TIME
			_cursor_cols[i]._scale = 6;
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

		row_size += _cursor_cols[i]._len;
	}

	_cursor_allocated_rows = 1;

	// Define how many rows fetch at once
	if(buffer_rows > 0)
		_cursor_allocated_rows = buffer_rows;
	else
	if(buffer_memory > 0)
	{
		size_t rows = buffer_memory/row_size;
		_cursor_allocated_rows = rows > 0 ? rows : 1;

		// Sybase ASA 9 returns invalid value when fetch array is 100K i.e
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
		// Data type NCHAR or NVARCHAR
		if(_cursor_cols[i]._native_dt == SQL_WCHAR || _cursor_cols[i]._native_dt == SQL_WVARCHAR)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_WCHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len * sizeof(wchar_t) + 2 /* U+0000 */;   
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// BIGINT 
		if(_cursor_cols[i]._native_dt == SQL_BIGINT)
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
			if(_target_api_provider != NULL && _target_api_provider->IsIntegerBulkBindSupported() == true)
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
			if(_target_api_provider != NULL && _target_api_provider->IsSmallintBulkBindSupported() == true)
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
		// TINYINT (_len is 3)
		if(_cursor_cols[i]._native_dt == SQL_TINYINT)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
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
		// FLOAT (SQL_REAL)
		if(_cursor_cols[i]._native_dt == SQL_REAL)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			// Driver returns 7 as len for FLOAT, it is too short for string representations
			_cursor_cols[i]._fetch_len = 65 + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// DOUBLE 
		if(_cursor_cols[i]._native_dt == SQL_DOUBLE)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			// Driver returns 15 as len for DOUBLE, it is too short for string representations
			_cursor_cols[i]._fetch_len = 65 + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// DATE (len is 10)
		if(_cursor_cols[i]._native_dt == SQL_TYPE_DATE)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// TIMESTAMP
		if(_cursor_cols[i]._native_dt == SQL_TYPE_TIMESTAMP)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_TYPE_TIMESTAMP;
			_cursor_cols[i]._fetch_len = sizeof(SQL_TIMESTAMP_STRUCT);
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// TIME (len is 6, bind to string as ODBC TIME_STRUCT does not support fractions)
		if(_cursor_cols[i]._native_dt == SQL_TYPE_TIME)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = 15 + 1;
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
		// BINARY is variable-length in ASA
		if(_cursor_cols[i]._native_dt == SQL_BINARY)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_BINARY;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// LONG VARCHAR and LONG BINARY
		if(_cursor_cols[i]._native_dt == SQL_LONGVARCHAR || _cursor_cols[i]._native_dt == SQL_LONGVARBINARY)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			if(_cursor_cols[i]._native_dt == SQL_LONGVARBINARY)
				_cursor_cols[i]._native_fetch_dt = SQL_C_BINARY;

			// Fecthed as LOB
			if(_cursor_fetch_lob_as_varchar == false && catalog_query == false)
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
		else
		// LONG NVARCHAR
		if(_cursor_cols[i]._native_dt == SQL_WLONGVARCHAR)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_WCHAR;

			// Fecthed as LOB
			if(_cursor_fetch_lob_as_varchar == false && catalog_query == false)
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
		else
		// UNIQUEIDENTIFIER data type (_len is 36)
		if(_cursor_cols[i]._native_dt == SQL_GUID)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		
		// Bind the data to array
		if(_cursor_cols[i]._data != NULL)
		{
			// Allocate indicators
			_cursor_cols[i].ind = new size_t[_cursor_allocated_rows];

			if(_cursor_cols[i]._lob == false)
			{
				rc = SQLBindCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLSMALLINT)_cursor_cols[i]._native_fetch_dt, _cursor_cols[i]._data, 
								(SQLLEN)_cursor_cols[i]._fetch_len,	(SQLLEN*)_cursor_cols[i].ind);

				if(rc == -1)
					SetError(SQL_HANDLE_STMT, _hstmt_cursor);
			}
		}
	}

	// Prepare array fetch (ASA 9 returns invalid value when array size is 100,000
	rc = SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)_cursor_allocated_rows, 0); 

	if(rc == -1)
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);

	rc = SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROWS_FETCHED_PTR, &_cursor_fetched, 0);

	// Perform initial fetch, return SQL_SUCCESS even if there are less rows than array size
	rc = Fetch(NULL, NULL);

	if(col_count != NULL)
		*col_count = _cursor_cols_count; 

	if(allocated_array_rows != NULL)
		*allocated_array_rows = _cursor_allocated_rows;

	if(rows_fetched != NULL)
		*rows_fetched = (int)_cursor_fetched;
	
	if(cols != NULL)
		*cols = _cursor_cols;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Fetch next portion of data to allocate buffers
int SqlAsaApi::Fetch(int *rows_fetched, size_t *time_spent) 
{
	if(_cursor_allocated_rows <= 0)
		return -1;

	size_t start = GetTickCount();

	// Fetch the data
	int rc = SQLFetch(_hstmt_cursor);

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

			if(rc == -1)
				break;
		}
	}

	if(rows_fetched != NULL)
		*rows_fetched = (int)_cursor_fetched;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;
	
	return rc;
}

// Specifies whether API allows to parallel reading from this API and write to another API
bool SqlAsaApi::CanParallelReadWrite()
{
	if(_cursor_lob_exists == true)
		return false;

	return true;
}

// Get partial LOB content
int SqlAsaApi::GetLobPart(size_t /*row*/, size_t column, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	SQLLEN ind = 0;
	
	int rc = SQLGetData(_hstmt_cursor, (SQLUSMALLINT)(column + 1), (SQLSMALLINT)_cursor_cols[column]._native_fetch_dt, 
					_cursor_cols[column]._data, (SQLLEN)_cursor_cols[column]._fetch_len, &ind);

	_cursor_cols[column]._lob_fetch_status = rc;

	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);
		return rc;
	}

	// NULL value in LOB
	if(ind == -1)
		_cursor_cols[column].ind[0] = (size_t)-1;
	else
	// LONG VARCHAR
	if(_cursor_cols[column]._native_dt == SQL_LONGVARCHAR)
	{
		// ind contains the full length if the buffer is smaller than LOB (tested on ASA 9)
		if(ind >= (SQLLEN)_cursor_cols[column]._fetch_len)
			_cursor_cols[column].ind[0] = _cursor_cols[column]._fetch_len - 1;
		// For last chunk x0 not included (!) to ind (tested on ASA 9)
		else
			_cursor_cols[column].ind[0] = (size_t)ind;
	}
	else
	// LONG NVARCHAR
	if(_cursor_cols[column]._native_dt == SQL_WLONGVARCHAR)
	{
		if(ind >= (SQLLEN)_cursor_cols[column]._fetch_len)
			_cursor_cols[column].ind[0] = _cursor_cols[column]._fetch_len - 1;
		// For last chunk u0000 not included (!) to ind (tested on ASA 16)
		else
			_cursor_cols[column].ind[0] = (size_t)ind;
	}
	else
	if(_cursor_cols[column]._native_dt == SQL_LONGVARBINARY)
	{
		if(ind >= (SQLLEN)_cursor_cols[column]._fetch_len)
			_cursor_cols[column].ind[0] = _cursor_cols[column]._fetch_len;
		else
			_cursor_cols[column].ind[0] = (size_t)ind;
	}
	
	return rc;
}

// Close the cursor and deallocate buffers
int SqlAsaApi::CloseCursor()
{
	// Close the statement handle
	int rc = SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_cursor);

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
int SqlAsaApi::InitBulkTransfer(const char * /*table*/, size_t /*col_count*/, size_t /*allocated_array_rows*/, SqlCol * /*s_cols*/, SqlCol ** /*t_cols*/)
{
	return -1;
}

// Transfer rows between databases
int SqlAsaApi::TransferRows(SqlCol * /*s_cols*/, int /*rows_fetched*/, int * /*rows_written*/, size_t * /*bytes_written*/,
							size_t * /*time_spent*/)
{
	return -1;
}

// Complete bulk transfer
int SqlAsaApi::CloseBulkTransfer()
{
	return -1;
}

// Drop the table
int SqlAsaApi::DropTable(const char* table, size_t *time_spent, std::string &drop_stmt)
{
	size_t time_spent1 = 0, time_spent2 = 0;

	int	rc = DropReferences(table, &time_spent1);

	drop_stmt = "DROP TABLE ";
	drop_stmt += table;

	rc = ExecuteNonQuery(drop_stmt.c_str(), &time_spent2);

	if(time_spent)
		*time_spent = time_spent1 + time_spent2;
	
	return rc;
}

// Remove foreign key constraints referencing to the parent table
int SqlAsaApi::DropReferences(const char* table, size_t *time_spent)
{
	std::string schema;
	std::string name;

	SplitQualifiedName(table, schema, name);

	std::string drop = "BEGIN "\
					   "DECLARE @stmt VARCHAR(300);"\
					   "DECLARE cur CURSOR FOR SELECT 'ALTER TABLE ' + OBJECT_SCHEMA_NAME(parent_object_id) "\
					   " + '.' + OBJECT_NAME(parent_object_id) + ' DROP CONSTRAINT ' + name " \
					   "FROM sys.foreign_keys WHERE ";
					   
	if(schema.empty() == false)
	{
		drop += "OBJECT_SCHEMA_NAME(referenced_object_id) = '";
		drop += schema;
		drop += "' AND ";
	}

	drop += "OBJECT_NAME(referenced_object_id) = '";
	drop += Str::UnquoteIdent(name);
	drop += "';";
 
    drop += "OPEN cur;"\
			"FETCH cur INTO @stmt;"\
			"WHILE @@FETCH_STATUS = 0 "\
			"BEGIN "\
			" EXEC (@stmt);"\
			" FETCH cur INTO @stmt; "\
			"END "\
			"CLOSE cur; "\
			"DEALLOCATE cur; "\
			"END";

	int rc = ExecuteNonQuery(drop.c_str(), time_spent);

	return rc;
}

// Get the length of LOB column in the open cursor
int SqlAsaApi::GetLobLength(size_t /*row*/, size_t /*column*/, size_t * /*length*/)
{
	return -1;
}

// Get LOB content
int SqlAsaApi::GetLobContent(size_t /*row*/, size_t /*column*/, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	return -1;
}

// Get the list of available tables
int SqlAsaApi::GetAvailableTables(std::string &select, std::string &exclude, std::list<std::string> &tables)
{
	std::string condition;

	// Get a condition to select objects from the catalog 
	// * means all user tables, *.* means all tables accessible by the user
	GetSelectionCriteria(select.c_str(), exclude.c_str(), "USER_NAME(creator)", "table_name", condition, _user.c_str(), false);

	// Tested on ASA 9
	std::string query = "SELECT USER_NAME(creator), table_name FROM systable WHERE table_type = 'BASE'";
	
	// Add filter
	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}

	// Exclude system and Sybase specific schemas if *.* condition is set or
	// if default user (user name set in ODBC) and * is specified
	if(strcmp(select.c_str(), "*.*") == 0 || (strcmp(select.c_str(), "*") == 0 && _user.empty()))
		query += " AND USER_NAME(creator) NOT IN ('SYS','dbo','rs_systabgroup')";

	SQLHANDLE stmt;

	// Allocate a statement handle
	int rc = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

	// Error raised
	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, stmt);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt); 

		return -1;
	}

	char owner[1024], name[1024];
	SQLLEN cbOwner = 0, cbName = 0;

	*owner = '\x0';
	*name = '\x0';

	rc = SQLBindCol(stmt, 1, SQL_C_CHAR, (SQLCHAR*)owner, 1024, &cbOwner);
	rc = SQLBindCol(stmt, 2, SQL_C_CHAR, (SQLCHAR*)name, 1024, &cbName);

    // Fetch tables
	while(rc == 0)
	{
		rc = SQLFetch(stmt);

		if(rc != SQL_SUCCESS)
			break;

		std::string tab = owner;
		tab += ".";

		if(IsQuoteRequired(name))
		{
			tab += '"';
			tab += name;
			tab += '"';
		}
		else
			tab += name;

		tables.push_back(tab);
	}

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return (rc == SQL_NO_DATA ) ? 0 : rc;
}

// Read schema information
int SqlAsaApi::ReadSchema(const char *select, const char *exclude, bool read_cns, bool read_idx)
{
	std::string condition;

	ClearSchema();

	// Get a condition to select objects from the catalog 
	// * means all user tables, *.* means all tables accessible by the user
	GetSelectionCriteria(select, exclude, "USER_NAME(t.creator)", "t.table_name", condition, _user.c_str(), false);

	// Exclude system and Sybase specific schemas if *.* condition is set
	if(select != NULL && strcmp(select, "*.*") == 0 && condition.empty())
		condition += " USER_NAME(t.creator) NOT IN ('SYS','dbo','rs_systabgroup')";

	// Read constraints first as primary key columns defined in ReadTableColumns
	int rc = ReadTableConstraints(condition);

	rc = ReadTableColumns(condition);

	if(read_idx)
	{
		rc = ReadIndexes(condition);
		rc = ReadIndexColumns(condition);
	}

	if(read_cns)
	{
		rc = ReadReferences(condition);
		rc = ReadForeignKeyColumns(condition);
		rc = ReadForeignKeyActions();
		rc = ReadCheckConstraints(condition);
	}

	ReadReservedWords();

	// Reset initialization error text so it will not be later associated with the data transfer
	*_native_error_text = '\x0';
	_error = 0;

	return rc;
}

// Read columns of the selected tables
int SqlAsaApi::ReadTableColumns(std::string &condition)
{
	// Tested on ASA 9 and 16
	std::string query = "SELECT USER_NAME(t.creator), t.table_name, c.column_name, c.column_id, c.table_id,";
	query += " c.pkey, c.\"default\", CASE WHEN c.max_identity < 2000000000 THEN CAST(c.max_identity AS INT) WHEN c.max_identity > 2000000000 THEN CAST(1000000 AS INT) ELSE CAST(0 AS INT) END";
	query += " FROM syscolumn c, systable t WHERE c.table_id = t.table_id";

	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}
	
	query += " ORDER BY c.table_id, c.column_id";
	
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
			col_meta.schema = NULL;
			col_meta.table = NULL;
			col_meta.column = NULL;
			col_meta.num = 0;
			col_meta.tabid = 0;
			col_meta.data_type_code = 0;
			col_meta.default_value = NULL;
			col_meta.no_default = true;
			col_meta.identity = false;
			
			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Schema
			if(len != -1)
			{
				col_meta.schema = new char[(size_t)len + 1];

				strncpy(col_meta.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				col_meta.schema[len] = '\x0';
			}

			len = (SQLLEN)cols[1].ind[i];

			// Table
			if(len != -1)
			{
				std::string tab;
				std::string name(cols[1]._data + cols[1]._fetch_len * i, 0, (size_t)len);
		
				if(IsQuoteRequired(name.c_str()))
				{
					tab = '"';
					tab += name;
					tab += '"';
					len += 2;
				}
				else
					tab = name;

				col_meta.table = new char[(size_t)len + 1];
				strncpy(col_meta.table, tab.c_str(), (size_t)len);
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

			// Column number (INTEGER)
			if(len == 4)
				col_meta.num = *((int*)(cols[3]._data + cols[3]._fetch_len * i));

			len = (SQLLEN)cols[4].ind[i];

			// Table ID (INTEGER)
			if(len == 4)
				col_meta.tabid = *((int*)(cols[4]._data + cols[4]._fetch_len * i));

			len = (SQLLEN)cols[5].ind[i];

			// Y if a primary key column (there is no any information on PK columns order if it distinguishes from column order)
			char pkey = (len != -1) ? *(cols[5]._data + cols[5]._fetch_len * i) : '\x0';

			if(pkey == 'Y')
				SetPrimaryKeyColumn(col_meta);

			len = (SQLLEN)cols[6].ind[i];

			// DEFAULT value
			if(len != -1)
				SetDefault(col_meta, cols[6]._data + cols[6]._fetch_len * i, (int)len); 

			len = (SQLLEN)cols[7].ind[i];

			// Max IDENTITY value or 0 if it is not identity column
			if(len != -1)
			{
				int id_max = *((int*)(cols[7]._data + cols[7]._fetch_len * i));

				if(id_max > 0)
					SetIdentity(col_meta, id_max);
			}

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

// Read information about table constraints
int SqlAsaApi::ReadTableConstraints(std::string &condition)
{
	// Tested on ASA 9 (type 'T' is for CHECK constraint)
	std::string query = "SELECT USER_NAME(t.creator), t.table_name, c.constraint_name, c.constraint_type,";
	query += " c.table_id, c.constraint_id, c.index_id, c.fkey_id";
	query += " FROM sysconstraint c, systable t WHERE ";
	query += " c.table_id = t.table_id AND c.constraint_type in ('P', 'U', 'F', 'T') ";

	// Query for ASA 10, 12 and 16 (join by object_id, but table_id is returned since it is used in syscolumn)
	std::string query2 = "SELECT USER_NAME(t.creator), t.table_name, c.constraint_name, c.constraint_type,";
	query2 += " cast(t.table_id as int), c.constraint_id, ";
	query2 += "(SELECT index_id FROM sysindex where c.ref_object_id = object_id), ";
	// fkey_id is stored as index_id in sysidx, not sysindex
	query2 += "(SELECT cast(index_id as smallint) FROM sysidx where c.ref_object_id = object_id) ";
	query2 += " FROM sysconstraint c, systable t WHERE ";
	query2 += " c.table_object_id = t.object_id AND c.constraint_type in ('P', 'U', 'F', 'T') ";

	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;

		query2 += " AND ";
		query2 += condition;
	}
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlConstraints col_cns;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	// The first query may fail
	if(rc < 0)
		rc = OpenCursor(query2.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			col_cns.schema = NULL;
			col_cns.table = NULL;
			col_cns.constraint = NULL;
			col_cns.type = '\x0';
			col_cns.tabid = 0;
			col_cns.cnsid = 0;
			col_cns.idxid = 0;
			col_cns.condition = NULL;
			col_cns.idxname = NULL;

			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Schema name
			if(len != -1)
			{
				col_cns.schema = new char[(size_t)len + 1];

				strncpy(col_cns.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				col_cns.schema[len] = '\x0';
			}

			len = (SQLLEN)cols[1].ind[i];
			
			// Table name
			if(len != -1)
			{
				std::string tab;
				std::string name(cols[1]._data + cols[1]._fetch_len * i, 0, (size_t)len);
		
				if(IsQuoteRequired(name.c_str()))
				{
					tab = '"';
					tab += name;
					tab += '"';
					len += 2;
				}
				else
					tab = name;

				col_cns.table = new char[(size_t)len + 1];
				strncpy(col_cns.table, tab.c_str(), (size_t)len);
				col_cns.table[len] = '\x0';
			}
			
			len = (SQLLEN)cols[2].ind[i];

			// Constraint name
			if(len != -1)
			{
				col_cns.constraint = new char[(size_t)len + 1];

				strncpy(col_cns.constraint, cols[2]._data + cols[2]._fetch_len * i, (size_t)len);
				col_cns.constraint[len] = '\x0';
			}

			len = (SQLLEN)cols[3].ind[i];

			// Constraint type
			if(len != -1)
			{
				char type = *(cols[3]._data + cols[3]._fetch_len * i);

				// Convert F to R for foreign key, T to C for CHECK constraint
				if(type == 'F')
					col_cns.type = 'R';
				else
				if(type == 'T')
					col_cns.type = 'C';
				else
					col_cns.type = type;
			}
			
			len = (SQLLEN)cols[4].ind[i];

			// Table ID (INTEGER)
			if(len == 4)
				col_cns.tabid = *((int*)(cols[4]._data + cols[4]._fetch_len * i));

			len = (SQLLEN)cols[5].ind[i];

			// Constraint ID (INTEGER)
			if(len == 4)
				col_cns.cnsid = *((int*)(cols[5]._data + cols[5]._fetch_len * i));

			len = (SQLLEN)cols[6].ind[i];

			// Index ID (INTEGER) for UNIQUE constraints
			if(len == 4)
				col_cns.idxid = *((int*)(cols[6]._data + cols[6]._fetch_len * i));

			len = (SQLLEN)cols[7].ind[i];

			// Foreign key ID (SMALLINT) for foreign keys
			if(len == 2)
				col_cns.fkid = *((short*)(cols[7]._data + cols[7]._fetch_len * i));

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

// Read information about indexes 
int SqlAsaApi::ReadIndexes(std::string &condition)
{
	// Tested on ASA 9, index_type is 'U' for UNIQUE constraints, 'Y' for unique indexes, 'N' for non-unique indexes
	std::string query = "SELECT NULL, i.index_name, USER_NAME(t.creator), t.table_name, t.table_id, i.index_id,";
	query += " i.\"unique\"";
	query += " FROM sysindex i, systable t WHERE ";
	query += " i.table_id = t.table_id AND i.\"unique\" <> 'U' ";

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
			idx.schema = NULL;
			idx.index = NULL;
			idx.t_schema = NULL;
			idx.t_name = NULL;
			idx.tabid = 0;
			idx.idxid = 0;
			
			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Index owner
			if(len != -1)
			{
				idx.schema = new char[(size_t)len + 1];

				strncpy(idx.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				idx.schema[len] = '\x0';
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

			// Index ID - INTEGER
			if(len == 4)
				idx.idxid = *((int*)(cols[5]._data + cols[5]._fetch_len * i));

			len = (SQLLEN)cols[6].ind[i];

			// Index type - CHAR(1)
			if(len == 1)
			{
				char type = *(cols[6]._data + cols[6]._fetch_len * i);

				if(type == 'Y')
					idx.unique = true;
				else
					idx.unique = false;
			}

			// sysindex does not includes indexes on primary keys; indexes on UNIQUE constraints were excluded in the query
			_table_indexes.push_back(idx);			
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

// Read information about index columns
int SqlAsaApi::ReadIndexColumns(std::string &condition)
{
	// Tested on ASA 9
	std::string query = "SELECT ic.table_id, ic.index_id, ic.column_id, ic.\"order\"";
	query += " FROM sysixcol ic, systable t WHERE ";
	query += " ic.table_id = t.table_id ";

	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}

	query += " ORDER BY ic.table_id, ic.index_id, ic.sequence";
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlIndColumns idx_col;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, 
		&time_read, true);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			idx_col.schema = NULL;
			idx_col.index = NULL;
			idx_col.column = NULL;
			idx_col.tabid = 0;
			idx_col.idxid = 0;
			
			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Table ID - INTEGER
			if(len == 4)
				idx_col.tabid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			len = (SQLLEN)cols[1].ind[i];

			// Index ID - INTEGER
			if(len == 4)
				idx_col.idxid = *((int*)(cols[1]._data + cols[1]._fetch_len * i));

			len = (SQLLEN)cols[2].ind[i];

			int column_id = 0;

			// Column ID - INTEGER
			if(len == 4)
				column_id = *((int*)(cols[2]._data + cols[2]._fetch_len * i));

			len = (SQLLEN)cols[3].ind[i];

			// Index type - CHAR(1)
			if(len == 1)
			{
				char type = *(cols[3]._data + cols[3]._fetch_len * i);

				if(type == 'A')
					idx_col.asc = true;
				else
					idx_col.asc = false;
			}

			// Find column name
			for(std::list<SqlColMeta>::iterator i = _table_columns.begin(); i != _table_columns.end(); i++)
			{
				if((*i).tabid == idx_col.tabid && (*i).num == column_id)
				{
					// Set column name
					idx_col.column = (char*)Str::GetCopy((*i).column);

					// Set a column for UNIQUE constraint
					SetUniqueConstraintColumn(idx_col);

					_table_ind_columns.push_back(idx_col);
					break;
				}
			}
		}

		rc = Fetch(&rows_fetched, &time_read);

		// No more rows
		if(rc == 100)
			break;
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	idx_col.schema = NULL;
	idx_col.index = NULL;
	idx_col.column = NULL; 

	CloseCursor();

	return 0;
}

// Read references between primary key and foreign key constraints
int SqlAsaApi::ReadReferences(std::string &condition)
{
	// Tested on ASA 9, 16
	std::string query = "SELECT fk.foreign_table_id, fk.foreign_key_id, fk.primary_table_id";
	query += " FROM sysforeignkey fk, systable t WHERE ";
	query += " fk.foreign_table_id = t.table_id ";

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
			int fk_tabid = 0;
			int fk_keyid = 0;
			int pk_tabid = 0;

			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Foreign key table ID - INTEGER
			if(len == 4)
				fk_tabid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			len = (SQLLEN)cols[1].ind[i];

			// Foreign key ID - SMALLINT
			if(len == 2)
				fk_keyid = *((short*)(cols[1]._data + cols[1]._fetch_len * i));
			else
			// INT in ASA 16
			if(len == 4)
				fk_keyid = *((int*)(cols[1]._data + cols[1]._fetch_len * i));			 

			len = (SQLLEN)cols[2].ind[i];

			// Primary key table ID - INTEGER
			if(len == 4)
				pk_tabid = *((int*)(cols[2]._data + cols[2]._fetch_len * i));

			int pk_cnsid = 0;

			// Find primary key constraint ID
			for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
			{
				if((*i).tabid == pk_tabid && (*i).type == 'P')
				{
					pk_cnsid = (*i).cnsid;
					break;
				}
			}

			// Now find foreign key constraint and set primary key constraint reference
			for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
			{
				if((*i).tabid == fk_tabid && (*i).fkid == fk_keyid && (*i).type == 'R')
				{
					(*i).r_cnsid = pk_cnsid;
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

// Read columns for foreign key constraints
int SqlAsaApi::ReadForeignKeyColumns(std::string &condition)
{
	// Tested on ASA 16
	std::string query = "SELECT fk.foreign_table_id, fk.foreign_key_id, fk.foreign_column_id";
	query += " FROM sysfkcol fk, systable t, sysidxcol i WHERE";
	query += " fk.foreign_table_id = t.table_id AND fk.foreign_table_id = i.table_id AND";
	query += " fk.foreign_key_id = i.index_id AND fk.foreign_column_id = i.column_id";

	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}

	query += " ORDER BY i.primary_column_id";
	
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
			int keyid = 0;
			int colid = 0;

			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Foreign key table ID - INTEGER
			if(len == 4)
				tabid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			len = (SQLLEN)cols[1].ind[i];

			// Foreign key ID - SMALLINT
			if(len == 2)
				keyid = *((short*)(cols[1]._data + cols[1]._fetch_len * i));
			else 
			// INT in ASA 16
			if(len == 4)
				keyid = *((int*)(cols[1]._data + cols[1]._fetch_len * i));

			len = (SQLLEN)cols[2].ind[i];

			// Foreign key column ID - INTEGER
			if(len == 4)
				colid = *((int*)(cols[2]._data + cols[2]._fetch_len * i));

			int cnsid = 0;

			// Find foreign key constraint ID
			for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
			{
				if((*i).tabid == tabid && (*i).fkid == keyid && (*i).type == 'R')
				{
					cnsid = (*i).cnsid;
					break;
				}
			}

			// Find foreign key column name
			for(std::list<SqlColMeta>::iterator i = _table_columns.begin(); i != _table_columns.end(); i++)
			{
				if((*i).tabid == tabid && (*i).num == colid)
				{
					SqlConsColumns col_cns;

					col_cns.cnsid = cnsid;
					col_cns.tabid = tabid;
					col_cns.column = (char*)Str::GetCopy((*i).column);

					_table_cons_columns.push_back(col_cns);

					// String belongs to list now
					col_cns.column = NULL;
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

// Read ON UPDATE and ON DELETE actions for foreign key constraints
int SqlAsaApi::ReadForeignKeyActions()
{
	// Tested on ASA 16 (does not return any rows for default RESTRICT action)
	std::string query = "SELECT foreign_table_id, foreign_key_id, event, referential_action";
	query += " FROM systrigger WHERE referential_action IN ('C', 'D', 'N', 'R')";
	
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
			int keyid = 0;
			char evnt = 0;
			char action = 0;

			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Foreign key table ID - INTEGER
			if(len == 4)
				tabid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			len = (SQLLEN)cols[1].ind[i];

			// Foreign key ID - SMALLINT
			if(len == 2)
				keyid = *((short*)(cols[1]._data + cols[1]._fetch_len * i));
			else 
			// INT in ASA 16
			if(len == 4)
				keyid = *((int*)(cols[1]._data + cols[1]._fetch_len * i));

			len = (SQLLEN)cols[2].ind[i];

			if(len == 1)
				evnt = *(cols[2]._data + cols[2]._fetch_len * i);

			len = (SQLLEN)cols[3].ind[i];

			if(len == 1)
				action = *(cols[3]._data + cols[3]._fetch_len * i);

			// Find foreign key constraint ID
			for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
			{
				if((*i).tabid == tabid && (*i).fkid == keyid && (*i).type == 'R')
				{
					char act = 0;

					if(action == 'C')
						act = FK_ACTION_CASCADE;
					else if(action == 'D')
						act = FK_ACTION_SET_DEFAULT;
					else if(action == 'N')
						act = FK_ACTION_SET_NULL;
					else if(action == 'R')
						act = FK_ACTION_RESTRICT;

					// UPDATE COLUMNS event
					if(evnt == 'C')
						(*i).fk_on_update = act;
					else
					// DELETE event
					if(evnt == 'D')
						(*i).fk_on_delete = act;

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

// Read CHECK constraint conditions
int SqlAsaApi::ReadCheckConstraints(std::string & /*condition*/)
{
	// Tested on ASA 9
	std::string query = "SELECT check_id, check_defn FROM syscheck";
	
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
			char *check = NULL;

			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// CHECK constraint ID - INTEGER
			if(len == 4)
				cnsid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			len = (SQLLEN)cols[1].ind[i];

			// CHECK constraint condition with CHECK keyword and ()
			if(len != -1)
			{
				char *cur = cols[1]._data + cols[1]._fetch_len * i;

				// Skip CHECK keyword (in lowercase)
				if(strncmp(cur, "check", 5) == 0)
				{
					cur += 5;
					len -= 5;
				}

				check = new char[(size_t)len + 1];

				strncpy(check, cur, (size_t)len);
				check[len] = '\x0';
			}

			// Find CHECK constraint and set condition
			for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
			{
				if((*i).cnsid == cnsid && (*i).type == 'C')
				{
					(*i).condition = check;
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

// Save information about a primary key column
void SqlAsaApi::SetPrimaryKeyColumn(SqlColMeta &col_meta)
{
	// Find the primary key constraint for this table
	for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
	{
		if((*i).tabid == col_meta.tabid && (*i).type == 'P')
		{
			SqlConsColumns col_cns;

			col_cns.cnsid = (*i).cnsid;
			col_cns.tabid = (*i).tabid;

			col_cns.schema = (char*)Str::GetCopy(col_meta.schema);
			col_cns.table = (char*)Str::GetCopy(col_meta.table);
			col_cns.column = (char*)Str::GetCopy(col_meta.column);

			_table_cons_columns.push_back(col_cns);

			// String belongs to list now
			col_cns.schema = NULL;
			col_cns.table = NULL;
			col_cns.column = NULL;

			break;
		}
	}
}

// Set a column for UNIQUE constraint
void SqlAsaApi::SetUniqueConstraintColumn(SqlIndColumns &idx_col)
{
	// Find an UNIQUE constraint with specific index ID
	for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
	{
		if((*i).type == 'U' && (*i).tabid == idx_col.tabid && (*i).idxid == idx_col.idxid)
		{
			SqlConsColumns col_cns;

			col_cns.cnsid = (*i).cnsid;
			col_cns.tabid = (*i).tabid;
			col_cns.column = (char*)Str::GetCopy(idx_col.column);

			_table_cons_columns.push_back(col_cns);

			// String belongs to list now
			col_cns.column = NULL;
			break;
		}
	}
}

// Set DEFAULT value for a column
void SqlAsaApi::SetDefault(SqlColMeta &col_meta, const char *value, int len)
{
	if(len <= 0 || value == NULL)
		return;

	// Skip some default values
	if((len == 13 && _strnicmp(value, "autoincrement", 13) == 0) ||
		(len >= 20 && _strnicmp(value, "global autoincrement", 20) == 0) ||
		(len >= 9 && _strnicmp(value, "publisher", 9) == 0) ||
		(len >= 17 && _strnicmp(value, "current publisher", 17) == 0))
		return;

	// String literals are already single-quoted
	col_meta.default_value = new char[(size_t)len + 1];

	strncpy(col_meta.default_value, value, (size_t)len);
	col_meta.default_value[len] = '\x0';

	col_meta.no_default = false;
}

// Set IDENTITY column
// Note that CHECKPOINT command maybe required to update max_identity in SYSTABCOL
void SqlAsaApi::SetIdentity(SqlColMeta &col_meta, int id_max)
{
	if(id_max == 0)
		return;

	col_meta.identity = true;
	col_meta.id_start = 1;
	col_meta.id_inc = 1;
	col_meta.id_next = id_max + 1;
}

// Get a list of columns for specified primary or unique key
int SqlAsaApi::GetKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &output, std::list<std::string> *)
{
	// Find constraint columns
	for(std::list<SqlConsColumns>::iterator i = _table_cons_columns.begin(); i != _table_cons_columns.end(); i++)
	{
		if((*i).tabid == cns.tabid && (*i).cnsid == cns.cnsid)
		{
			output.push_back((*i).column);
		}
	}
				
	return 0;
}

// Get a list of columns for specified foreign key
int SqlAsaApi::GetForeignKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &fcols, std::list<std::string> &pcols, std::string &ptable) 
{
	// Find foreign key and primary key columns
	for(std::list<SqlConsColumns>::iterator i = _table_cons_columns.begin(); i != _table_cons_columns.end(); i++)
	{
		// A foreign key column
		if((*i).cnsid == cns.cnsid) 
			fcols.push_back((*i).column);
		else
		// A primary key column
		if((*i).cnsid == cns.r_cnsid) 
		{
			// set primary key table
			if(ptable.empty() == true)
			{
				if((*i).schema != NULL)
				{
					ptable = (*i).schema;				
					ptable += ".";
				}
				
				if((*i).table != NULL)
					ptable += (*i).table;
			}

			pcols.push_back((*i).column);
		}
	}

	return 0;
}

// Get a list of columns for specified index
int SqlAsaApi::GetIndexColumns(SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts)
{
	// Find index columns
	for(std::list<SqlIndColumns>::iterator i = _table_ind_columns.begin(); i != _table_ind_columns.end(); i++)
	{
		if((*i).tabid == idx.tabid && (*i).idxid == idx.idxid)
		{
			idx_cols.push_back((*i).column);
			idx_sorts.push_back("");
		}
	}
				
	return 0;
}

// Get table name by constraint name
int SqlAsaApi::ReadConstraintTable(const char * /*schema*/, const char * /*constraint*/, std::string & /*table*/)
{
	return -1;
}

// Read information about constraint columns
int SqlAsaApi::ReadConstraintColumns(const char * /*schema*/, const char * /*table*/, const char * /*constraint*/, std::string & /*cols*/)
{
	return -1;
}

// Get data type name by code
void SqlAsaApi::GetDataTypeName(SQLSMALLINT native_dt, std::string &name)
{
    if(native_dt == SQL_DECIMAL)
        name = "DECIMAL";
    else if (native_dt == SQL_NUMERIC)
        name = "NUMERIC";
    else if (native_dt == SQL_TYPE_DATE)
        name = "DATE";    
}

// Check whether identifier must be quoted
bool SqlAsaApi::IsQuoteRequired(const char *name)
{
	if(name == NULL)
		return false;

	// Starts with a digit
	if(*name >= '0' && *name <= '9')
		return true;

	return false;
}

// Fill reserved words
int SqlAsaApi::ReadReservedWords()
{
	// Reserved words not allowed in identifiers
	_reserved_words_ddl.push_back("add");   
	_reserved_words_ddl.push_back("all");
	_reserved_words_ddl.push_back("alter");
	_reserved_words_ddl.push_back("and");
	_reserved_words_ddl.push_back("any");
	_reserved_words_ddl.push_back("as");
	_reserved_words_ddl.push_back("asc");
	_reserved_words_ddl.push_back("attach");
	_reserved_words_ddl.push_back("backup");
	_reserved_words_ddl.push_back("begin");
	_reserved_words_ddl.push_back("between");
	_reserved_words_ddl.push_back("bigint");
	_reserved_words_ddl.push_back("binary");
	_reserved_words_ddl.push_back("bit");
	_reserved_words_ddl.push_back("bottom");
	_reserved_words_ddl.push_back("break");
	_reserved_words_ddl.push_back("by");
	_reserved_words_ddl.push_back("call");
	_reserved_words_ddl.push_back("capability");
	_reserved_words_ddl.push_back("cascade");
	_reserved_words_ddl.push_back("case");
	_reserved_words_ddl.push_back("cast");
	_reserved_words_ddl.push_back("char");
	_reserved_words_ddl.push_back("char_convert");
	_reserved_words_ddl.push_back("character");
	_reserved_words_ddl.push_back("check");
	_reserved_words_ddl.push_back("checkpoint");
	_reserved_words_ddl.push_back("close");
	_reserved_words_ddl.push_back("comment");
	_reserved_words_ddl.push_back("commit");
	_reserved_words_ddl.push_back("compressed");
	_reserved_words_ddl.push_back("conflict");
	_reserved_words_ddl.push_back("connect");
	_reserved_words_ddl.push_back("constraint");
	_reserved_words_ddl.push_back("contains");
	_reserved_words_ddl.push_back("continue");
	_reserved_words_ddl.push_back("convert");
	_reserved_words_ddl.push_back("create");
	_reserved_words_ddl.push_back("cross");
	_reserved_words_ddl.push_back("cube");
	_reserved_words_ddl.push_back("current");
	_reserved_words_ddl.push_back("current_timestamp");
	_reserved_words_ddl.push_back("current_user");
	_reserved_words_ddl.push_back("cursor");
	_reserved_words_ddl.push_back("date");
	_reserved_words_ddl.push_back("datetimeoffset");
	_reserved_words_ddl.push_back("dbspace");
	_reserved_words_ddl.push_back("deallocate");
	_reserved_words_ddl.push_back("dec");
	_reserved_words_ddl.push_back("decimal");
	_reserved_words_ddl.push_back("declare");
	_reserved_words_ddl.push_back("default");
	_reserved_words_ddl.push_back("delete");
	_reserved_words_ddl.push_back("deleting");
	_reserved_words_ddl.push_back("desc");
	_reserved_words_ddl.push_back("detach");
	_reserved_words_ddl.push_back("distinct");
	_reserved_words_ddl.push_back("do");
	_reserved_words_ddl.push_back("double");
	_reserved_words_ddl.push_back("drop");
	_reserved_words_ddl.push_back("dynamic");
	_reserved_words_ddl.push_back("else");
	_reserved_words_ddl.push_back("elseif");
	_reserved_words_ddl.push_back("encrypted");
	_reserved_words_ddl.push_back("end");
	_reserved_words_ddl.push_back("endif");
	_reserved_words_ddl.push_back("escape");
	_reserved_words_ddl.push_back("except");
	_reserved_words_ddl.push_back("exception");
	_reserved_words_ddl.push_back("exec");
	_reserved_words_ddl.push_back("execute");
	_reserved_words_ddl.push_back("existing");
	_reserved_words_ddl.push_back("exists");
	_reserved_words_ddl.push_back("externlogin");
	_reserved_words_ddl.push_back("fetch");
	_reserved_words_ddl.push_back("first");
	_reserved_words_ddl.push_back("float");
	_reserved_words_ddl.push_back("for");
	_reserved_words_ddl.push_back("force");
	_reserved_words_ddl.push_back("foreign");
	_reserved_words_ddl.push_back("forward");
	_reserved_words_ddl.push_back("from");
	_reserved_words_ddl.push_back("full");
	_reserved_words_ddl.push_back("goto");
	_reserved_words_ddl.push_back("grant");
	_reserved_words_ddl.push_back("group");
	_reserved_words_ddl.push_back("having");
	_reserved_words_ddl.push_back("holdlock");
	_reserved_words_ddl.push_back("identified");
	_reserved_words_ddl.push_back("if");
	_reserved_words_ddl.push_back("in");
	_reserved_words_ddl.push_back("index");
	_reserved_words_ddl.push_back("inner");
	_reserved_words_ddl.push_back("inout");
	_reserved_words_ddl.push_back("insensitive");
	_reserved_words_ddl.push_back("insert");
	_reserved_words_ddl.push_back("inserting");
	_reserved_words_ddl.push_back("install");
	_reserved_words_ddl.push_back("instead");
	_reserved_words_ddl.push_back("int");
	_reserved_words_ddl.push_back("integer");
	_reserved_words_ddl.push_back("integrated");
	_reserved_words_ddl.push_back("intersect");
	_reserved_words_ddl.push_back("into");
	_reserved_words_ddl.push_back("is");
	_reserved_words_ddl.push_back("isolation");
	_reserved_words_ddl.push_back("join");
	_reserved_words_ddl.push_back("kerberos");
	_reserved_words_ddl.push_back("key");
	_reserved_words_ddl.push_back("lateral");
	_reserved_words_ddl.push_back("left");
	_reserved_words_ddl.push_back("like");
	_reserved_words_ddl.push_back("limit");
	_reserved_words_ddl.push_back("lock");
	_reserved_words_ddl.push_back("login");
	_reserved_words_ddl.push_back("long");
	_reserved_words_ddl.push_back("match");
	_reserved_words_ddl.push_back("membership");
	_reserved_words_ddl.push_back("merge");
	_reserved_words_ddl.push_back("message");
	_reserved_words_ddl.push_back("mode");
	_reserved_words_ddl.push_back("modify");
	_reserved_words_ddl.push_back("natural");
	_reserved_words_ddl.push_back("nchar");
	_reserved_words_ddl.push_back("new");
	_reserved_words_ddl.push_back("no");
	_reserved_words_ddl.push_back("noholdlock");
	_reserved_words_ddl.push_back("not");
	_reserved_words_ddl.push_back("notify");
	_reserved_words_ddl.push_back("null");
	_reserved_words_ddl.push_back("numeric");
	_reserved_words_ddl.push_back("nvarchar");
	_reserved_words_ddl.push_back("of");
	_reserved_words_ddl.push_back("off");
	_reserved_words_ddl.push_back("on");
	_reserved_words_ddl.push_back("open");
	_reserved_words_ddl.push_back("openstring");
	_reserved_words_ddl.push_back("openxml");
	_reserved_words_ddl.push_back("option");
	_reserved_words_ddl.push_back("options");
	_reserved_words_ddl.push_back("or");
	_reserved_words_ddl.push_back("order");
	_reserved_words_ddl.push_back("others");
	_reserved_words_ddl.push_back("out");
	_reserved_words_ddl.push_back("outer");
	_reserved_words_ddl.push_back("over");
	_reserved_words_ddl.push_back("passthrough");
	_reserved_words_ddl.push_back("precision");
	_reserved_words_ddl.push_back("prepare");
	_reserved_words_ddl.push_back("primary");
	_reserved_words_ddl.push_back("print");
	_reserved_words_ddl.push_back("privileges");
	_reserved_words_ddl.push_back("proc");
	_reserved_words_ddl.push_back("procedure");
	_reserved_words_ddl.push_back("publication");
	_reserved_words_ddl.push_back("raiserror");
	_reserved_words_ddl.push_back("readtext");
	_reserved_words_ddl.push_back("real");
	_reserved_words_ddl.push_back("reference");
	_reserved_words_ddl.push_back("references");
	_reserved_words_ddl.push_back("refresh");
	_reserved_words_ddl.push_back("release");
	_reserved_words_ddl.push_back("remote");
	_reserved_words_ddl.push_back("remove");
	_reserved_words_ddl.push_back("rename");
	_reserved_words_ddl.push_back("reorganize");
	_reserved_words_ddl.push_back("resource");
	_reserved_words_ddl.push_back("restore");
	_reserved_words_ddl.push_back("restrict");
	_reserved_words_ddl.push_back("return");
	_reserved_words_ddl.push_back("revoke");
	_reserved_words_ddl.push_back("right");
	_reserved_words_ddl.push_back("rollback");
	_reserved_words_ddl.push_back("rollup");
	_reserved_words_ddl.push_back("save");
	_reserved_words_ddl.push_back("savepoint");
	_reserved_words_ddl.push_back("scroll");
	_reserved_words_ddl.push_back("select");
	_reserved_words_ddl.push_back("sensitive");
	_reserved_words_ddl.push_back("session");
	_reserved_words_ddl.push_back("set");
	_reserved_words_ddl.push_back("setuser");
	_reserved_words_ddl.push_back("share");
	_reserved_words_ddl.push_back("smallint");
	_reserved_words_ddl.push_back("some");
	_reserved_words_ddl.push_back("spatial");
	_reserved_words_ddl.push_back("sqlcode");
	_reserved_words_ddl.push_back("sqlstate");
	_reserved_words_ddl.push_back("start");
	_reserved_words_ddl.push_back("stop");
	_reserved_words_ddl.push_back("subtrans");
	_reserved_words_ddl.push_back("subtransaction");
	_reserved_words_ddl.push_back("synchronize");
	_reserved_words_ddl.push_back("table");
	_reserved_words_ddl.push_back("temporary");
	_reserved_words_ddl.push_back("then");
	_reserved_words_ddl.push_back("time");
	_reserved_words_ddl.push_back("timestamp");
	_reserved_words_ddl.push_back("tinyint");
	_reserved_words_ddl.push_back("to");
	_reserved_words_ddl.push_back("top");
	_reserved_words_ddl.push_back("tran");
	_reserved_words_ddl.push_back("treat");
	_reserved_words_ddl.push_back("trigger");
	_reserved_words_ddl.push_back("truncate");
	_reserved_words_ddl.push_back("tsequal");
	_reserved_words_ddl.push_back("unbounded");
	_reserved_words_ddl.push_back("union");
	_reserved_words_ddl.push_back("unique");
	_reserved_words_ddl.push_back("uniqueidentifier");
	_reserved_words_ddl.push_back("unknown");
	_reserved_words_ddl.push_back("unsigned");
	_reserved_words_ddl.push_back("update");
	_reserved_words_ddl.push_back("updating");
	_reserved_words_ddl.push_back("user");
	_reserved_words_ddl.push_back("using");
	_reserved_words_ddl.push_back("validate");
	_reserved_words_ddl.push_back("values");
	_reserved_words_ddl.push_back("varbinary");
	_reserved_words_ddl.push_back("varbit");
	_reserved_words_ddl.push_back("varchar");
	_reserved_words_ddl.push_back("variable");
	_reserved_words_ddl.push_back("varying");
	_reserved_words_ddl.push_back("view");
	_reserved_words_ddl.push_back("wait");
	_reserved_words_ddl.push_back("waitfor");
	_reserved_words_ddl.push_back("when");
	_reserved_words_ddl.push_back("where");
	_reserved_words_ddl.push_back("while");
	_reserved_words_ddl.push_back("window");
	_reserved_words_ddl.push_back("with");
	_reserved_words_ddl.push_back("within");
	_reserved_words_ddl.push_back("work");
	_reserved_words_ddl.push_back("writetext");
	_reserved_words_ddl.push_back("xml");

	return 0;
}

// Initialize session
int SqlAsaApi::InitSession()
{
	if(_parameters == NULL)
		return -1;

	SqlApiBase::InitSession();

	if(_parameters->GetTrue("-sybase_asa_char_as_wchar") != NULL)
		_char_as_wchar = true;

	return 0;
}
// Set session attributes
int SqlAsaApi::SetSession()
{
	// Refresh metadata (identity values i.e.)
	const char *query = "CHECKPOINT";
	int rc = ExecuteNonQuery(query, NULL);

	return rc;
}

// Set version of the connected database
void SqlAsaApi::SetVersion()
{
	// Tested on ASA 9
	const char *query = "SELECT @@VERSION FROM dummy";

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor 
	int rc = OpenCursor(query, 1, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	// Get the value
	if(rc >= 0)
	{
		SQLLEN len = (SQLLEN)cols[0].ind[0];

		if(len != -1 && len < 127)
			_version.assign(cols[0]._data, (size_t)len);
	}

	CloseCursor();
}

// Set error code and message for the last API call
void SqlAsaApi::SetError(SQLSMALLINT handle_type, SQLHANDLE handle)
{
	SQLCHAR sqlstate[6]; *sqlstate = '\x0';

	// Get native error information
	SQLGetDiagRec(handle_type, handle, 1, sqlstate, (SQLINTEGER*)&_native_error, (SQLCHAR*)_native_error_text, 1024, NULL);

	_error = SQL_DBAPI_UNKNOWN_ERROR;
	*_error_text = '\x0';
}
