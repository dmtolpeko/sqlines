/** 
 * Copyright (c) 2016 SQLines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// SqlSncApi SQL Server Native Client API

#include <string.h>
#include <string>
#include "str.h"
#include "sqlsncapi.h"
#include "os.h"

// MySQL C API definitions
#include <mysql.h>

// Constructor
SqlSncApi::SqlSncApi()
{
	_henv = SQL_NULL_HANDLE;
	_hdbc = SQL_NULL_HANDLE;
	_hstmt_cursor = SQL_NULL_HANDLE;

	_trusted = false;

	_cursor_fetched = 0;

	_bcp_cols_count = 0;
	_bcp_cols = NULL;
	_bcp_lob_exists = false;
	_bcp_codepage = -2; // -1 is used by BCPFILECP_RAW

	_error = 0;
	_error_text[0] = '\x0';

	_native_error = 0;
	_native_error_text[0] = '\x0';

	_dll = NULL;
	_dll_odbc = NULL;

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

	_bcp_batch = NULL;
	_bcp_bind = NULL;
	_bcp_control = NULL;
	_bcp_done = NULL;
	_bcp_init = NULL;
	_bcp_moretext = NULL;
	_bcp_sendrow = NULL;
}

SqlSncApi::~SqlSncApi()
{
	// When other ODBC drivers are used crash can happen on exit when not disconnected and not deallocated
	Deallocate();
}

// Initialize API
int SqlSncApi::Init()
{
	std::string dll;

#if defined(WIN32) || defined(_WIN64)
	// Define which SQL Server Native Client driver and DLL to use
	DefineDriver(_driver, dll);

	_dll_odbc = LoadLibraryEx(SQLSRV_ODBC_DLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	_dll = LoadLibraryEx(dll.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
	_dll_odbc = Os::LoadLibrary(SQLSERV_DLL);
	_dll = _dll_odbc;

	if(_dll_odbc == NULL || _dll == NULL)
	{
		char *error = Os::LoadLibraryError();
		if(error != NULL)
			strcpy(_native_error_text, error);
	}
#endif
		
	// Get functions
	if(_dll_odbc != NULL)
	{
		_SQLAllocHandle = (SQLAllocHandleFunc)Os::GetProcAddress(_dll_odbc, "SQLAllocHandle");
		_SQLBindCol = (SQLBindColFunc)Os::GetProcAddress(_dll_odbc, "SQLBindCol");
		_SQLDescribeCol = (SQLDescribeColFunc)Os::GetProcAddress(_dll_odbc, "SQLDescribeCol");
		_SQLDisconnect = (SQLDisconnectFunc)Os::GetProcAddress(_dll_odbc, "SQLDisconnect");
		_SQLDriverConnect = (SQLDriverConnectFunc)Os::GetProcAddress(_dll_odbc, "SQLDriverConnect");
		_SQLExecDirect = (SQLExecDirectFunc)Os::GetProcAddress(_dll_odbc, "SQLExecDirect");
		_SQLFetch = (SQLFetchFunc)Os::GetProcAddress(_dll_odbc, "SQLFetch");
		_SQLFreeHandle = (SQLFreeHandleFunc)Os::GetProcAddress(_dll_odbc, "SQLFreeHandle");
		_SQLGetData = (SQLGetDataFunc)Os::GetProcAddress(_dll_odbc, "SQLGetData");
		_SQLGetDiagRec = (SQLGetDiagRecFunc)Os::GetProcAddress(_dll_odbc, "SQLGetDiagRec");
		_SQLNumResultCols = (SQLNumResultColsFunc)Os::GetProcAddress(_dll_odbc, "SQLNumResultCols");
		_SQLSetConnectAttr = (SQLSetConnectAttrFunc)Os::GetProcAddress(_dll_odbc, "SQLSetConnectAttr");
		_SQLSetEnvAttr = (SQLSetEnvAttrFunc)Os::GetProcAddress(_dll_odbc, "SQLSetEnvAttr");
		_SQLSetStmtAttr = (SQLSetStmtAttrFunc)Os::GetProcAddress(_dll_odbc, "SQLSetStmtAttr");

		if(_dll != NULL)
		{
			// Save the full path of the loaded driver
			Os::GetModuleFileName(_dll, _loaded_driver, 1024);

			_bcp_batch = (bcp_batchFunc)Os::GetProcAddress(_dll, "bcp_batch");
			_bcp_bind = (bcp_bindFunc)Os::GetProcAddress(_dll, "bcp_bind");
			_bcp_control = (bcp_controlFunc)Os::GetProcAddress(_dll, "bcp_control");
			_bcp_done = (bcp_doneFunc)Os::GetProcAddress(_dll, "bcp_done");
			_bcp_init = (bcp_initFunc)Os::GetProcAddress(_dll, "bcp_initA");
			_bcp_moretext = (bcp_moretextFunc)Os::GetProcAddress(_dll, "bcp_moretext");
			_bcp_sendrow = (bcp_sendrowFunc)Os::GetProcAddress(_dll, "bcp_sendrow");
		}

		if(_SQLAllocHandle == NULL || _SQLBindCol == NULL || _SQLDescribeCol == NULL || 
			_SQLDisconnect == NULL || _SQLDriverConnect == NULL || _SQLExecDirect == NULL || 
			_SQLFetch == NULL || _SQLFreeHandle == NULL || _SQLGetData == NULL || _SQLGetDiagRec == NULL || 
			_SQLSetConnectAttr == NULL || _SQLSetEnvAttr == NULL || _SQLSetStmtAttr == NULL ||
			_bcp_batch == NULL || _bcp_bind == NULL || _bcp_control == NULL  || _bcp_init == NULL || 
			_bcp_moretext == NULL || _bcp_sendrow == NULL)
			return -1;
	}
	else
	{
		Os::GetLastErrorText(SQLSRV_DLL_LOAD_ERROR, _native_error_text, 1024);
		return -1;
	}

	// Initialize options
	if(_parameters != NULL)
		_bcp_codepage = _parameters->GetInt("-bcp_codepage", -2);   // -1 is used by BCPFILECP_RAW

	return 0;
}

// Set the connection string in the API object
void SqlSncApi::SetConnectionString(const char *conn)
{
	SplitConnectionString(conn, _user, _pwd, _server, _db, _port);

	// Check for trusted connection
	_trusted = (_stricmp(_user.c_str(), "trusted") == 0) ? true : false;

	return;
}

// Connect to the database
int SqlSncApi::Connect(size_t *time_spent)
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

	// Enable bulk copy API 
	rc = _SQLSetConnectAttr(_hdbc, SQL_COPT_SS_BCP, (SQLPOINTER)SQL_BCP_ON, SQL_IS_INTEGER);

	// Set the size of network packets (rc = 1 with message Package Size changed)
	rc = _SQLSetConnectAttr(_hdbc, SQL_ATTR_PACKET_SIZE, (SQLPOINTER)(1024*128), 0);

	std::string conn = _driver;

	// Build connection string, add server
	if(!_server.empty())
	{
		conn += "Server=";
		conn += _server;

		if(!_port.empty())
		{
			conn += ",";
			conn += _port;
		}

		conn += ";";
	}

	// Add server
	if(!_db.empty())
	{
		conn += "Database=";
		conn += _db;
		conn += ";";
	}

	// Add login information
	if(_trusted == false)
	{
		conn += "UID=";
		conn += _user;

		conn += ";PWD=";
		conn += _pwd;
		conn += ";";
	}
	else
		conn += "Trusted_Connection=yes;";

	// Connection string must not contain spaces between parameters, =, values etc.
	rc = _SQLDriverConnect(_hdbc, NULL, (SQLCHAR*)conn.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

	if(rc == SQL_ERROR || rc < 0)
		SetError(SQL_HANDLE_DBC, _hdbc);
	else
		_connected = true;

	// Identify the version of the connected database
	SetVersion();
	ReadReservedWords();

    // Call after version numbers are set
	InitSession();

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return rc;
}

// Disconnect from the database
void SqlSncApi::Disconnect() 
{
	if(_connected == false)
		return;

	_SQLDisconnect(_hdbc);
	_SQLFreeHandle(SQL_HANDLE_DBC, _hdbc);
	
	_hdbc = SQL_NULL_HANDLE;
	_connected = false;
}

// Free connection and environment handles 
void SqlSncApi::Deallocate()
{
	Disconnect();

	if(_henv != SQL_NULL_HANDLE)
	{
		_SQLFreeHandle(SQL_HANDLE_ENV, _henv);
		_henv = SQL_NULL_HANDLE;
	}
}

// Get row count for the specified object
int SqlSncApi::GetRowCount(const char *object, int *count, size_t *time_spent)
{
	if(object == NULL)
		return -1;

	std::string query = "SELECT COUNT(*) FROM ";
	query += object;
	query += " WITH (NOLOCK)";

	// Execute the query
	int rc = ExecuteScalar(query.c_str(), count, time_spent);

	return rc;
}

// Execute the statement and get scalar result
int SqlSncApi::ExecuteScalar(const char *query, int *result, size_t *time_spent)
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
int SqlSncApi::ExecuteNonQuery(const char *query, size_t *time_spent)
{
	if(query == NULL)
		return -1;

	TRACE_P("SQL Server ExecuteNonQuery() Entered:\n%s", query);

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

	TRACE_P("SQL Server ExecuteNonQuery() Left, retcode %d", rc);

	return rc;
}

// Open cursor and allocate buffers
int SqlSncApi::OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query, std::list<SqlDataTypeMap> * /*dtmap*/)
{
	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = _SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &_hstmt_cursor);

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
	rc = _SQLNumResultCols(_hstmt_cursor, (SQLSMALLINT*)&_cursor_cols_count);

	if(_cursor_cols_count > 0)
		_cursor_cols = new SqlCol[_cursor_cols_count];

	size_t row_size = 0;

	_cursor_lob_exists = false;

	// Get column information
	for(size_t i = 0; i < _cursor_cols_count; i++)
	{
		SQLSMALLINT native_dt;
		SQLULEN column_size;
		SQLSMALLINT decimal_digits;
		SQLSMALLINT nullable;

		rc = _SQLDescribeCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLCHAR*)_cursor_cols[i]._name, 256, NULL, &native_dt, &column_size, &decimal_digits, &nullable); 

		_cursor_cols[i]._native_dt = native_dt;
		_cursor_cols[i]._len = column_size;

		// VARCHAR(MAX), NVARCHAR(MAX) has 0 length
		if((native_dt == SQL_VARCHAR || native_dt == SQL_WVARCHAR || native_dt == SQL_VARBINARY) && 
			column_size == 0)
		{
            // Do not bind LOB column, read LOB value in chunks   
            if(_lob_bind_buffer == 0)
            {
                _cursor_cols[i]._len = 65536 * 16;
			    _cursor_cols[i]._lob = true;
			    _cursor_lob_exists = true;
            }
            // Bind LOB as usual column
            else
                _cursor_cols[i]._len = (size_t)_lob_bind_buffer;
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
		// TEXT, NTEXT and IMAGE (size is 2G)
		if(native_dt == SQL_LONGVARCHAR || native_dt == SQL_LONGVARBINARY || native_dt == SQL_WLONGVARCHAR)
		{
            // Do not bind LOB column, read LOB value in chunks   
            if(_lob_bind_buffer == 0)
            {
                // Change 2G column size to estimated average size (chunk size of 3 MB)
                _cursor_cols[i]._len = 65536 * 16 * 3;
			    _cursor_cols[i]._lob = true;
			    _cursor_lob_exists = true;
            }
            // Bind LOB as usual column
            else
                _cursor_cols[i]._len = (size_t)_lob_bind_buffer;
		}
		else
		// XML
		if(native_dt == SQL_SS_XML)
		{
			_cursor_cols[i]._len = 65536 * 16 * 3;
			_cursor_cols[i]._lob = true;
			_cursor_lob_exists = true;
		}

		if(nullable == SQL_NULLABLE)
			_cursor_cols[i]._nullable = true;
		else
			_cursor_cols[i]._nullable = false;

		row_size += _cursor_cols[i]._len;
	}

	_cursor_allocated_rows = 1;

	if(row_size == 0)
		row_size = 1;

	// Define how many rows fetch at once
	if(buffer_rows > 0)
	{
		// Row length can be large for bound LOB buffers so inforce hard limit of buffer size
		size_t buffer_rows_max = (100*1024*1024)/row_size;

		if(buffer_rows < buffer_rows_max)
			_cursor_allocated_rows = buffer_rows;
		else
			_cursor_allocated_rows = buffer_rows_max;		
	}
	else
	if(buffer_memory > 0)
	{
		size_t rows = buffer_memory/row_size;
		_cursor_allocated_rows = rows > 0 ? rows : 1;
	}	

	if(_cursor_lob_exists == true)
		_cursor_allocated_rows = 1;

	// Allocate buffers to each column
	for(size_t i = 0; i < _cursor_cols_count; i++)
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
            // MySQL does not support UCS2 in LOAD DATA INFILE, let's convert to ASCII; also convert to ASCII for target Oracle
            if(_target_api_provider != NULL && 
				(_target_api_provider->GetType() == SQLDATA_MYSQL || _target_api_provider->GetType() == SQLDATA_ORACLE))
            {
                _cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			    _cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			    _cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
            }
            else
            {
                _cursor_cols[i]._native_fetch_dt = SQL_C_WCHAR;
			    _cursor_cols[i]._fetch_len = _cursor_cols[i]._len * sizeof(wchar_t) + 2 /* U+0000 */;   
			    _cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
            }
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
		// Data type NCHAR or NVARCHAR
		if(_cursor_cols[i]._native_dt == SQL_WCHAR || _cursor_cols[i]._native_dt == SQL_WVARCHAR)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_WCHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len * sizeof(wchar_t) + 2 /* U+0000 */;   
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}		
		else
		// INT
		if(_cursor_cols[i]._native_dt == SQL_INTEGER)
		{
			// Check whether target supports bind to INT or string has to be used
			if((_target_api_provider != NULL && _target_api_provider->IsIntegerBulkBindSupported() == true) ||
                catalog_query == true)
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
			if((_target_api_provider != NULL && _target_api_provider->IsSmallintBulkBindSupported() == true) ||
                catalog_query == true)
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
		// DECIMAL and NUMERIC
		if(_cursor_cols[i]._native_dt == SQL_DECIMAL || _cursor_cols[i]._native_dt == SQL_NUMERIC)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			// Include sign, decimal point and terminating zero
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + _cursor_cols[i]._scale + 3;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// FLOAT
		if(_cursor_cols[i]._native_dt == SQL_FLOAT)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			// Driver returns column len 53 for FLOAT without parameters
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// REAL
		if(_cursor_cols[i]._native_dt == SQL_REAL)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			// Driver returns column len 24 for REAL
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
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
			// Check whether target supports bind to TIMESTAMP or string has to be used
			if(_target_api_provider != NULL && _target_api_provider->IsTimestampBulkBindSupported() == true)
			{
				_cursor_cols[i]._native_fetch_dt = SQL_TYPE_TIMESTAMP;
				_cursor_cols[i]._fetch_len = sizeof(SQL_TIMESTAMP_STRUCT);
			}
			else
			{
                _cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

                // SMALLDATETIME len is 16 (YYYY-MM-DD HH24:MI with spaces), but bind to 16 causes
                // truncation errors, add :SS (seconds always zero)
                if (_cursor_cols[i]._len == 16)
                    _cursor_cols[i]._fetch_len = 19 + 1;
                else
				    _cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;	
			}

			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// TIME
		if(_cursor_cols[i]._native_dt == SQL_SS_TIME2)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;	

			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// DATETIMEOFFSET (-155)
		if(_cursor_cols[i]._native_dt == SQL_SS_TIMESTAMPOFFSET)
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
		// BINARY and VARBINARY 
		if(_cursor_cols[i]._native_dt == SQL_BINARY || _cursor_cols[i]._native_dt == SQL_VARBINARY)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_BINARY;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// TEXT and IMAGE
		if(_cursor_cols[i]._native_dt == SQL_LONGVARCHAR || _cursor_cols[i]._native_dt == SQL_LONGVARBINARY)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			if(_cursor_cols[i]._native_dt == SQL_LONGVARBINARY)
				_cursor_cols[i]._native_fetch_dt = SQL_C_BINARY;

			// Will not bound if fecthed as LOB, but still allocate the space for the chunk
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len];
		}
		else
		// XML
		if(_cursor_cols[i]._native_dt == SQL_SS_XML)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len];
		}
		else
		// UNIQUEIDENTIFIER
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
				rc = _SQLBindCol(_hstmt_cursor, SQLUSMALLINT(i + 1), (SQLSMALLINT)_cursor_cols[i]._native_fetch_dt, _cursor_cols[i]._data, 
					(SQLLEN)_cursor_cols[i]._fetch_len, (SQLLEN*)_cursor_cols[i].ind);

				if(rc == -1)
                {
					SetError(SQL_HANDLE_STMT, _hstmt_cursor);

                    _SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_cursor);
		            _hstmt_cursor = SQL_NULL_HANDLE;

		            return -1;
			    }
            }
		}
	}

	// Prepare array fetch
	rc = _SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)_cursor_allocated_rows, 0); 
	rc = _SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROWS_FETCHED_PTR, &_cursor_fetched, 0);

	// Perform initial fetch, return SQL_SUCCESS even if there are less rows than array size
	rc = Fetch(NULL, NULL);

    if(rc == -1)
    {
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);

        _SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_cursor);
	    _hstmt_cursor = SQL_NULL_HANDLE;

	    return -1;
	}

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
int SqlSncApi::Fetch(int *rows_fetched, size_t *time_spent) 
{
	if(_cursor_allocated_rows <= 0)
		return -1;

	size_t start = GetTickCount();

	// Fetch the data
	int rc = _SQLFetch(_hstmt_cursor);

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
    else if (rc == SQL_SUCCESS_WITH_INFO)
        SetError(SQL_HANDLE_STMT, _hstmt_cursor);

	if(rows_fetched != NULL)
		*rows_fetched = _cursor_fetched;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;
	
	return rc;
}

// Get partial LOB content
int SqlSncApi::GetLobPart(size_t /*row*/, size_t column, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	SQLLEN ind = 0;
	
	int rc = _SQLGetData(_hstmt_cursor, (SQLUSMALLINT)(column + 1), (SQLSMALLINT)_cursor_cols[column]._native_fetch_dt, 
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
	if(_cursor_cols[column]._native_dt == SQL_LONGVARCHAR || _cursor_cols[column]._native_dt == SQL_VARCHAR)
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
	if(_cursor_cols[column]._native_dt == SQL_WLONGVARCHAR || _cursor_cols[column]._native_dt == SQL_WVARCHAR)
	{
		if(ind >= (SQLLEN)_cursor_cols[column]._fetch_len)
			_cursor_cols[column].ind[0] = _cursor_cols[column]._fetch_len - 1;
		// For last chunk u0000 not included (!) to ind (tested on ASA 16)
		else
			_cursor_cols[column].ind[0] = (size_t)ind;
	}
	else
	if(_cursor_cols[column]._native_dt == SQL_LONGVARBINARY || _cursor_cols[column]._native_dt == SQL_VARBINARY)
	{
		if(ind >= (SQLLEN)_cursor_cols[column]._fetch_len)
			_cursor_cols[column].ind[0] = _cursor_cols[column]._fetch_len;
		else
			_cursor_cols[column].ind[0] = (size_t)ind;
	}
	
	return rc;
}

// Close the cursor and deallocate buffers
int SqlSncApi::CloseCursor()
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
int SqlSncApi::InitBulkTransfer(const char *table, size_t col_count, size_t allocated_array_rows, SqlCol *s_cols, SqlCol **t_cols)
{
	if(table == NULL || s_cols == NULL || col_count <= 0)
		return -1;

	_ins_allocated_rows = allocated_array_rows;
	_ins_all_rows_inserted = 0;
	
	int rc = _bcp_init(_hdbc, table, NULL, NULL, DB_IN);

	if(rc == FAIL)
	{
		SetError(SQL_HANDLE_DBC, _hdbc);
		return -1;
	}

	// Lock table
	rc = _bcp_control(_hdbc, BCPHINTS, (void*)"TABLOCK");

	// Use IDENTITY values from the source table
	rc = _bcp_control(_hdbc, BCPKEEPIDENTITY, (void*)TRUE);

	// Set the BCP codepage defined by the user. 
	// By default Windows locale defines input data codepage that can cause problems when Windows and table locale does not match
	// For some reason specifying BCPFILECP_RAW (-1) still causes conversion and cannot be used as default
	if(_bcp_codepage != -2)
	{
		rc = _bcp_control(_hdbc, BCPFILECP, (void*)_bcp_codepage);

		if(rc == FAIL)
		{	
			SetError(SQL_HANDLE_DBC, _hdbc);
			return -1;
		}
	}

	_bcp_cols_count = col_count;
	_bcp_cols = new SqlCol[col_count];

	_bcp_lob_exists = false;

	// Map each column and its properties
	for(int i = 0; i < col_count; i++)
	{
		// Check whether the source buffer bound
		if(s_cols[i]._data == NULL)
			continue;

		strcpy(_bcp_cols[i]._name, s_cols[i]._name);

		_bcp_cols[i]._len = s_cols[i]._fetch_len;
		_bcp_cols[i]._lob = s_cols[i]._lob;

		// Sybase ASE data types fetched as strings
		if((_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_CHAR_TYPE && s_cols[i]._fetch_len <= 8000) ||
			// All MySQL data types except BLOB are fetched as strings
			(_source_api_type == SQLDATA_MYSQL && s_cols[i]._native_fetch_dt != MYSQL_TYPE_BLOB))
		{
			_bcp_cols[i]._native_dt = SQLCHARACTER;
		}
		else
		// SQL Server, Sybase ASA NCHAR, NVARCHAR less than 8000 bytes (4000 characters), DB2 GRAPHIC and VARGRAPHIC
		if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_ASA) && 
			s_cols[i]._native_fetch_dt == SQL_C_WCHAR && 
			s_cols[i]._fetch_len <= 8000 && s_cols[i]._lob == false)
		{
			_bcp_cols[i]._native_dt = SQLNCHAR;
		}
		else
		// Oracle VARCHAR2, TIMESTAMP or NUMBER/NUMBER(p,s) where s != 0 mapped to string
		if((_source_api_type == SQLDATA_ORACLE && s_cols[i]._native_fetch_dt == SQLT_STR) ||
			// SQLServer, DB2, Informix, Sybase ASA, ODBC VARCHAR
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || 
			  _source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA || 
				_source_api_type == SQLDATA_ODBC) && s_cols[i]._native_fetch_dt == SQL_C_CHAR &&
					s_cols[i]._fetch_len <= 8000 &&
					s_cols[i]._lob == false))
		{
			_bcp_cols[i]._native_dt = SQLCHARACTER;
		}
		else
		// Map Oracle NUMBER fetched as INT to INT code for SQL Server
		if((_source_api_type == SQLDATA_ORACLE && s_cols[i]._native_fetch_dt == SQLT_INT) ||
			// Sybase ASE INT
			(_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_INT_TYPE) ||
			// Informix, Sybase ASA, ODBC INTEGER
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX || 
			   _source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC) 
				&& s_cols[i]._native_fetch_dt == SQL_C_LONG))
		{
			_bcp_cols[i]._native_dt = SQLINT4;
		}
		else
		// SMALLINT
		if((_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_SMALLINT_TYPE) ||
			// Informix, Sybase ASA, ODBC SMALLINT
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX || 
				_source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC) && 
				s_cols[i]._native_fetch_dt == SQL_C_SHORT))
		{
			_bcp_cols[i]._native_dt = SQLINT2;
		}
		else
		// Map Oracle DATE to SQLCHARACTER 19 bytes (yyyy-mm-dd hh:mi:ss)
		if(_source_api_type == SQLDATA_ORACLE && s_cols[i]._native_fetch_dt == SQLT_DAT)
		{
			_bcp_cols[i]._native_dt = SQLCHARACTER;

			// Allocate also space for terminating 0 
			_bcp_cols[i]._len = 19 + 1;
		}
		else
		// TIMESTAMP fetched as SQL_TIMESTAMP_STRUCT to SQLCHARACTER 26 bytes (yyyy-mm-dd hh:mi:ss.ffffff)
		if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || 
			_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA || 
			_source_api_type == SQLDATA_ODBC) && s_cols[i]._native_fetch_dt == SQL_C_TYPE_TIMESTAMP)
		{
			_bcp_cols[i]._native_dt = SQLCHARACTER;

			// Allocate also space for terminating 0 
			_bcp_cols[i]._len = 26 + 1;
		}
		else
		// Map Oracle RAW to VARBINARY for SQL Server
		if(_source_api_type == SQLDATA_ORACLE && s_cols[i]._native_fetch_dt == SQLT_BIN)
		{
			_bcp_cols[i]._native_dt = SQLVARBINARY;
		}
		else
		// ODBC FLOAT
		if(_source_api_type == SQLDATA_ODBC && s_cols[i]._native_fetch_dt == SQL_C_FLOAT)
		{
			_bcp_cols[i]._native_dt = SQLFLT4;
		}
		else
		// Map BLOB and CLOB to use bcp_moretext
		if((_source_api_type == SQLDATA_ORACLE && 
				(s_cols[i]._native_dt == SQLT_BLOB || s_cols[i]._native_dt == SQLT_CLOB)) ||
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX || 
			   _source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC) && 
					(s_cols[i]._native_dt == SQL_LONGVARBINARY || s_cols[i]._native_dt == SQL_LONGVARCHAR ||
						s_cols[i]._native_dt == SQL_WLONGVARCHAR)) ||
			// Sybase ASE TEXT
			(_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_CHAR_TYPE && s_cols[i]._fetch_len > 8000) ||
			// SQL Server VARCHAR(max) and NVARCHAR(max), DB2 CLOB, XML
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2) && 
				(s_cols[i]._native_fetch_dt == SQL_C_CHAR || s_cols[i]._native_fetch_dt == SQL_C_WCHAR) &&
			s_cols[i]._fetch_len > 8000) ||
			// SQL Server VARBINARY(max), DB2 BLOB
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2) && s_cols[i]._native_fetch_dt == SQL_C_BINARY &&
				s_cols[i]._fetch_len > 8000) ||
			// Sybase ASA CHAR/VARCHAR, NCHAR/NVARCHAR greater than 8000 bytes
			(_source_api_type == SQLDATA_ASA && 
			(s_cols[i]._native_fetch_dt == SQL_C_CHAR || s_cols[i]._native_fetch_dt == SQL_C_WCHAR) && 
			s_cols[i]._fetch_len > 8000) ||
			// MySQL BLOB
			(_source_api_type == SQLDATA_MYSQL && s_cols[i]._native_fetch_dt == MYSQL_TYPE_BLOB))
		{
			bool binary = false;
			_bcp_cols[i]._native_dt = SQLVARCHAR;

			if((_source_api_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_BLOB) ||
				((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX || 
				  _source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC) && 
				  s_cols[i]._native_dt == SQL_LONGVARBINARY) ||
				((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2) && s_cols[i]._native_fetch_dt == SQL_C_BINARY) ||
				(_source_api_type == SQLDATA_MYSQL && s_cols[i]._native_fetch_dt == MYSQL_TYPE_BLOB))
			{
				binary = true;
				_bcp_cols[i]._native_dt = SQLVARBINARY;
			}
			else
			// Unicode type
			if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX || 
				_source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC) && 
				(s_cols[i]._native_dt == SQL_WLONGVARCHAR || s_cols[i]._native_dt == SQL_WCHAR || 
					s_cols[i]._native_dt == SQL_WVARCHAR))
				_bcp_cols[i]._native_dt = SQLNVARCHAR;
			else
			// XML type
			if(_source_api_type == SQLDATA_DB2 && s_cols[i]._native_dt == -370)
				_bcp_cols[i]._native_dt = SQLXML;

			_bcp_cols[i]._len = 0;

			// Allocate space for indicators to store source LOB values
			_bcp_cols[i]._data = new char[4]; 

			_bcp_lob_exists = true;

			// Force LOB for character fields exceeding 8000 bytes
			_bcp_cols[i]._lob = true;

			// Bind the data buffer to NULL
			rc = _bcp_bind(_hdbc, NULL, 0,  SQL_VARLEN_DATA, NULL, 0, _bcp_cols[i]._native_dt, i + 1);

			if(rc == FAIL)
			{
				SetError(SQL_HANDLE_DBC, _hdbc);
				return -1;
			}
		}

		// Allocating space for 1 item in buffer and the null/length indicator that goes before data
		if(_bcp_cols[i]._len > 0)
		{
			_bcp_cols[i]._data = new char[_bcp_cols[i]._len + 4];

			// Bind the data buffer
			rc = _bcp_bind(_hdbc, (LPCBYTE)_bcp_cols[i]._data, 4, (DBINT)_bcp_cols[i]._len, NULL, 0, _bcp_cols[i]._native_dt, i + 1);

			if(rc == FAIL)
			{
				SetError(SQL_HANDLE_DBC, _hdbc);
				return -1;
			}
		}
	}

	if(t_cols != NULL)
		*t_cols = _bcp_cols;

	// BCP functions return 1 on success, 0 on failure
	return (rc == 1) ? 0 : -1;
}

// Transfer rows between databases
int SqlSncApi::TransferRows(SqlCol *s_cols, int rows_fetched, int *rows_written, size_t *bytes_written,
							size_t *time_spent)
{
	if(s_cols == NULL || _bcp_cols == NULL || _bcp_cols_count <= 0 || rows_fetched < 0)
		return -1;

	if(rows_fetched == 0)
		return 0;

	int rc = 0;
	size_t bytes = 0;

	size_t start = GetTickCount();

	// Copy rows
	for(size_t i = 0; i < rows_fetched; i++)
	{
		// Copy column data
		for(size_t k = 0; k < _bcp_cols_count; k++)
		{
			// BCP indicator is 4-byte on both 32-bit and 64-bit platforms
			SQLINTEGER *ind = (SQLINTEGER*)_bcp_cols[k]._data;

			if(ind == NULL)
				continue;

			*ind = SQL_NULL_DATA;

			// Write NULL/length indicator to target
			if(_source_api_type == SQLDATA_ORACLE && s_cols[k]._ind2 != NULL)
			{
				if(s_cols[k]._ind2[i] != -1)
					*ind = s_cols[k]._len_ind2[i];
			}
			else
			// Sybase ASE
			if(_source_api_type == SQLDATA_SYBASE && s_cols[k]._ind2 != NULL)
			{
				if(s_cols[k]._ind2[i] != -1)
					*ind = s_cols[k]._len_ind4[i];
			}
			else
			// ODBC indicator contains either NULL or length
			if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX || 
				_source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_MYSQL || 
				_source_api_type == SQLDATA_ASA || 
				_source_api_type == SQLDATA_ODBC) 
				&& s_cols[k].ind != NULL)
			{
				*ind = (SQLINTEGER)s_cols[k].ind[i];

#if defined(WIN64)
				// DB2 11 64-bit CLI driver still writes indicators to 4-byte array
				if(_source_api_type == SQLDATA_DB2 && s_cols[k].ind[0] & 0xFFFFFFFF00000000)
					*ind = (SQLINTEGER)((int*)(s_cols[k].ind))[i];
#endif
			}

			if(*ind == SQL_NULL_DATA)
				continue;

			// For non-LOB column calculate size
			if((_source_api_type == SQLDATA_ORACLE && s_cols[k]._native_dt != SQLT_BLOB && s_cols[k]._native_dt != SQLT_CLOB) ||
			   (_source_api_type == SQLDATA_SYBASE && s_cols[k]._native_dt != CS_TEXT_TYPE) || 
				_source_api_type == SQLDATA_INFORMIX ||
				_source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_MYSQL || 
				_source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC ||
				_source_api_type == SQLDATA_SQL_SERVER)
				bytes += *ind;

			// Oracle VARCHAR2, RAW
			if((_source_api_type == SQLDATA_ORACLE && 
				(s_cols[k]._native_fetch_dt == SQLT_STR || s_cols[k]._native_fetch_dt == SQLT_BIN)) ||
				// Sybase CHAR
				(_source_api_type == SQLDATA_SYBASE && s_cols[k]._native_fetch_dt == CS_CHAR_TYPE && s_cols[k]._fetch_len <= 8000) ||
				// MySQL all data types except BLOB
				(_source_api_type == SQLDATA_MYSQL && s_cols[k]._native_fetch_dt != MYSQL_TYPE_BLOB) ||
				// Informix, Sybase ASA, ODBC CHAR/VARCHAR and other data types fetched as string
				((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 ||
				   _source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC) && 
				   s_cols[k]._native_fetch_dt == SQL_C_CHAR && s_cols[k]._fetch_len <= 8000 && 
				   s_cols[k]._lob == false) ||
				// SQL Server, Sybase ASA NCHAR, NVARCHAR, DB2 GRAPHIC and VARGRAPHIC
				((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_ASA) && 
					s_cols[k]._native_fetch_dt == SQL_C_WCHAR &&
					s_cols[k]._fetch_len <= 8000 && s_cols[k]._lob == false) ||
				// Informix, Sybase ASA BINARY non-LOB
				((_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA) && 
					s_cols[k]._native_fetch_dt == SQL_C_BINARY &&
					s_cols[k]._lob == false))
			{
				memcpy(_bcp_cols[k]._data + 4, s_cols[k]._data + s_cols[k]._fetch_len * i, (size_t)*ind);
			}
			else
			// Oracle NUMBER fetched as INT 
			if((_source_api_type == SQLDATA_ORACLE && s_cols[k]._native_fetch_dt == SQLT_INT) ||
				// Sybase ASE INT
				(_source_api_type == SQLDATA_SYBASE && s_cols[k]._native_fetch_dt == CS_INT_TYPE) ||
				// DB2, Informix, Sybase ASA, ODBC INTEGER
				((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || 
				    _source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA || 
					_source_api_type == SQLDATA_ODBC) && 
					s_cols[k]._native_fetch_dt == SQL_C_LONG))
			{
				int *s_data = (int*)s_cols[k]._data;
				int *t_data = (int*)_bcp_cols[k]._data;

				// Write column value after indicator
				t_data[1] = s_data[i];
			}
			else
			// Sybase ASE SMALLINT
			if((_source_api_type == SQLDATA_SYBASE && s_cols[k]._native_fetch_dt == CS_SMALLINT_TYPE) ||
				// DB2, Informix, Sybase ASA, ODBC SMALLINT
				((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || 
					_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA || 
					_source_api_type == SQLDATA_ODBC) && 
					s_cols[k]._native_fetch_dt == SQL_C_SHORT))
			{
				short *s_data = (short*)s_cols[k]._data;
				short *t_data = (short*)_bcp_cols[k]._data;

				// Write column value after 4-byte indicator
				t_data[2] = s_data[i];
			}
			else
			// ODBC FLOAT
			if(_source_api_type == SQLDATA_ODBC && s_cols[k]._native_fetch_dt == SQL_C_FLOAT)
			{
				float *s_data = (float*)s_cols[k]._data;
				float *t_data = (float*)_bcp_cols[k]._data;

				// Write column value after indicator
				t_data[1] = s_data[i];
			}
			else
			// Oracle DATE fetched as 7 byte binary sequence
			if(_source_api_type == SQLDATA_ORACLE && s_cols[k]._native_fetch_dt == SQLT_DAT)
			{
				*ind = 19;

				// Unsigned required for proper conversion to int
				unsigned char *data = (unsigned char*)(s_cols[k]._data + s_cols[k]._fetch_len * i);

				int c = ((int)data[0]) - 100;
				int y = ((int)data[1]) - 100;
				int m = (int)data[2];
				int d = (int)data[3];
				int h = ((int)data[4]) - 1;
				int mi = ((int)data[5]) - 1;
				int s = ((int)data[6]) - 1;

				// Get string representation
				Str::Dt2Ch(c, _bcp_cols[k]._data + 4);
				Str::Dt2Ch(y, _bcp_cols[k]._data + 6);
				_bcp_cols[k]._data[8] = '-';
				Str::Dt2Ch(m, _bcp_cols[k]._data + 9);
				_bcp_cols[k]._data[11] = '-';
				Str::Dt2Ch(d, _bcp_cols[k]._data + 12);
				_bcp_cols[k]._data[14] = ' ';
				Str::Dt2Ch(h, _bcp_cols[k]._data + 15);
				_bcp_cols[k]._data[17] = ':';
				Str::Dt2Ch(mi, _bcp_cols[k]._data + 18);
				_bcp_cols[k]._data[20] = ':';
				Str::Dt2Ch(s, _bcp_cols[k]._data + 21);
			}
			else
			// ODBC TIMESTAMP fetched as SQL_TIMESTAMP_STRUCT
			if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || 
				_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA || 
				_source_api_type == SQLDATA_ODBC) && s_cols[k]._native_fetch_dt == SQL_C_TYPE_TIMESTAMP)
			{
				*ind = 26;

				size_t offset = sizeof(SQL_TIMESTAMP_STRUCT) * i;

				// Informix returns '1200-01-01 11:11:00.0' for 11:11 (HOUR TO MINUTE)
				SQL_TIMESTAMP_STRUCT *ts = (SQL_TIMESTAMP_STRUCT*)(s_cols[k]._data + offset);

				long fraction = (long)ts->fraction;

				// In ODBC, fraction is stored in nanoseconds, but now we support only microseconds
				fraction = fraction/1000;

				// Convert SQL_TIMESTAMP_STRUCT to string
				Str::SqlTs2Str((short)ts->year, (short)ts->month, (short)ts->day, (short)ts->hour, (short)ts->minute, (short)ts->second, fraction, 
					_bcp_cols[k]._data + 4);
			}
		}

		rc = _bcp_sendrow(_hdbc);

		if(rc == FAIL)
		{
			SetError(SQL_HANDLE_DBC, _hdbc);
			break;
		}

		_ins_all_rows_inserted++;

		// Send LOB data
		if(_bcp_lob_exists == true && _source_api_provider != NULL)
		{
			size_t lob_bytes = 0;

			rc = WriteLob(s_cols, i, &lob_bytes);

			if(rc != FAIL)
				bytes += lob_bytes;
		}
	}

	int written = 0;

	if(rc != FAIL)
	{
		// Commit rows
		if(_ins_allocated_rows > 1)
			written = _bcp_batch(_hdbc);
		else
		// Commit table with LOB every 100 rows
		if(_ins_all_rows_inserted % 100 == 0)
		{
			/*int committed */ _bcp_batch(_hdbc);

			// Rows were already sent before and counter incremented
			written = 1;
		}
		// 1 row was sent
		else
			written = 1;

		if(written == -1)
			SetError(SQL_HANDLE_DBC, _hdbc);
	}

	if(rows_written != NULL)
		*rows_written = written;

	if(bytes_written != NULL)
		*bytes_written = bytes;

	if(time_spent)
		*time_spent = GetTickCount() - start;

	// BCP Batch function return the number of saved rows, -1 on failure
	return (written >= 0 && rc != FAIL) ? 0 : -1;
}

// Write LOB data using BCP API
int SqlSncApi::WriteLob(SqlCol *s_cols, size_t row, size_t *lob_bytes)
{
	if(s_cols == NULL || _bcp_lob_exists == false || _source_api_provider == NULL)
		return -1;

	int rc = 0;
	size_t len = 0;
	size_t row_len = 0;
	char *data = NULL;

	for(size_t k = 0; k < _bcp_cols_count; k++)
	{
		if(s_cols[k]._lob == false && _bcp_cols[k]._lob == false)
			continue;

		if(_source_api_type == SQLDATA_ORACLE)
		{
			// LOB is NULL
			if(s_cols[k]._ind2[row] == -1)
			{
				rc = _bcp_moretext(_hdbc, SQL_NULL_DATA, NULL);
				continue;
			}
		
			// LOB contains data
			rc = _source_api_provider->GetLobLength(row, k, (size_t*)&len);

			if(rc == -1)
				break;

			// LOB is empty
			if(len == 0)
			{
				rc = _bcp_moretext(_hdbc, 0, (LPCBYTE)"");
				continue;
			}

			data = new char[(size_t)len];

			// Get LOB content
			rc = _source_api_provider->GetLobContent(row, k, data, (size_t)len, NULL);

			// Write LOB
			if(rc != -1)
			{
				rc = _bcp_moretext(_hdbc, (DBINT)len, (LPCBYTE)data);
				row_len += len;
			}
		}
		else
		// Sybase ASE
		if(_source_api_type == SQLDATA_SYBASE)
		{
			// LOB is NULL
			if(s_cols[k]._ind2[row] == -1)
			{
				rc = _bcp_moretext(_hdbc, SQL_NULL_DATA, NULL);
				continue;
			}

			len = (size_t)s_cols[k]._len_ind4[row];

			// LOB is empty
			if(len == 0)
			{
				rc = _bcp_moretext(_hdbc, 0, (LPCBYTE)"");
				continue;
			}

			// Write the full LOB value
			rc = _bcp_moretext(_hdbc, (DBINT)len, (LPCBYTE)s_cols[k]._data + s_cols[k]._fetch_len * row);
			row_len += len;
		}
		else
		if(_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_INFORMIX || 
			_source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC)
		{
			// First chunk already read, BCP indicator is 4-byte on both 32-bit and 64-bit platforms
			if(s_cols[k].ind != NULL)
			{
				len = s_cols[k].ind[row];

#if defined(WIN64)
				// DB2 11 64-bit CLI driver still writes indicators to 4-byte array
				if(_source_api_type == SQLDATA_DB2 && s_cols[k].ind[0] & 0xFFFFFFFF00000000)
					len = (size_t)((SQLINTEGER*)(s_cols[k].ind))[row];
#endif
			}
			
			// LOB is NULL
			if(len == -1)
			{
				rc = _bcp_moretext(_hdbc, SQL_NULL_DATA, NULL);
				continue;
			}
			else
			// LOB is empty
			if(len == 0)
			{
				rc = _bcp_moretext(_hdbc, 0, (LPCBYTE)"");

				if(rc == FAIL)
					SetError(SQL_HANDLE_DBC, _hdbc);

				continue;
			}

			bool more = true;

			// Get LOB in chunks
			while(more)
			{
				// Write the current chunk
				rc = _bcp_moretext(_hdbc, (DBINT)len, (LPCBYTE)(s_cols[k]._data + s_cols[k]._fetch_len * row));
								
				if(rc == FAIL)
					SetError(SQL_HANDLE_DBC, _hdbc);

				row_len += len;

				// No more chunks
				if(s_cols[k]._lob_fetch_status != SQL_SUCCESS_WITH_INFO)
				{
					more = false;
					break;
				}

				// Read next chunk
				int read_rc = _source_api_provider->GetLobPart(0, k, NULL, 0, NULL);

				if(read_rc == -1)
					break;

				len = s_cols[k].ind[row];
			}
		}
		else
		if(_source_api_type == SQLDATA_MYSQL)
		{
			// Full value already read
			if(s_cols[k].ind != NULL)
				len = s_cols[k].ind[row];
			
			// LOB is NULL
			if(len == -1)
			{
				rc = _bcp_moretext(_hdbc, SQL_NULL_DATA, NULL);
				continue;
			}
			else
			// LOB is empty
			if(len == 0)
			{
				rc = _bcp_moretext(_hdbc, 0, (LPCBYTE)"");

				if(rc == FAIL)
					SetError(SQL_HANDLE_DBC, _hdbc);

				continue;
			}

			// Write the full LOB value
			rc = _bcp_moretext(_hdbc, (DBINT)len, (LPCBYTE)s_cols[k]._data);
			row_len += len;
		}

		// End the current LOB
		rc = _bcp_moretext(_hdbc, 0, NULL);

		if(rc == FAIL)
			SetError(SQL_HANDLE_DBC, _hdbc);

		delete data;
		data = NULL;
	}

	if(rc != FAIL && lob_bytes != NULL)
		*lob_bytes = row_len;

	return rc;
}

// Complete bulk transfer
int SqlSncApi::CloseBulkTransfer()
{
	if(_bcp_done == NULL)
		return -1;

	// Commit table with LOB every 100 rows
	if(_ins_allocated_rows == 1)
		_bcp_batch(_hdbc);

	int rows = _bcp_done(_hdbc);

	int rc = (rows >= 0) ? 0 : -1;

	if(_bcp_cols == NULL)
		return rc;

	// Delete allocated buffers
	for(int i = 0; i < _bcp_cols_count; i++)
	{
		// Delete data only, indicators are not used
		delete [] _bcp_cols[i]._data;
	}

	delete [] _bcp_cols;

	_bcp_cols = NULL;
	_bcp_cols_count = 0;

	return rc;
}

// Drop the table
int SqlSncApi::DropTable(const char* table, size_t *time_spent, std::string &drop_stmt)
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
int SqlSncApi::DropReferences(const char* table, size_t *time_spent)
{
	std::string schema;
	std::string name;

	SplitQualifiedName(table, schema, name);

	// NOLOCK is used to prevent "[Microsoft][SQL Server Native Client 11.0][SQL Server]Transaction (Process ID 54) was deadlocked on lock resources with 
	// another process and has been chosen as the deadlock victim. Rerun the transaction."
	std::string drop = "BEGIN "\
					   "DECLARE @stmt VARCHAR(300);"\
					   "DECLARE cur CURSOR FOR SELECT 'ALTER TABLE [' + OBJECT_SCHEMA_NAME(parent_object_id) "\
					   " + '].[' + OBJECT_NAME(parent_object_id) + '] DROP CONSTRAINT ' + name " \
					   "FROM sys.foreign_keys WITH (NOLOCK) WHERE ";
					   
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
int SqlSncApi::GetLobLength(size_t /*row*/, size_t /*column*/, size_t * /*length*/)
{
	return -1;
}

// Get LOB content
int SqlSncApi::GetLobContent(size_t /*row*/, size_t /*column*/, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	return -1;
}

// Get the list of available tables
int SqlSncApi::GetAvailableTables(std::string &table_template, std::string & /*exclude*/, std::list<std::string> &tables)
{
	std::string condition, condition2;

	// Get a condition to select objects from the catalog
	GetSelectionCriteria(table_template.c_str(), "SCHEMA_NAME(uid)", "name", condition, NULL, false);
    GetSelectionCriteria(table_template.c_str(), "USER_NAME(uid)", "name", condition2, NULL, false);

	// Build the query (sys.objects is available since 2005, sysobject is available in 2000-2012 but
	// will be removed
	// SCHEMA_NAME() is not available in SQL Server 2000
	// USER_NAME() is available in all versions but does not return schema name (!), so it returns NULL
	// for a table in a schema other than dbo
	std::string query = "SELECT SCHEMA_NAME(uid), name FROM sysobjects WHERE type = 'U'";
    std::string query2 = "SELECT USER_NAME(uid), name FROM sysobjects WHERE type = 'U'";
	
	// Add filter
	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;

        query2 += " AND ";
		query2 += condition2;
	}

	SQLHANDLE stmt;

	// Allocate a statement handle
	int rc = _SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = _SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    // The first query may fail (SQL Server 2000 i.e)
	if(rc < 0)
		rc = _SQLExecDirect(stmt, (SQLCHAR*)query2.c_str(), SQL_NTS);

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

		std::string tab = owner;
		tab += ".";

		if(IsQuoteRequired(name) || IsReservedWord(name))
		{
			tab += '[';
			tab += name;
			tab += ']';
		}
		else
			tab += name;

		tables.push_back(tab);
	}

	_SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return (rc == SQL_NO_DATA ) ? 0 : rc;
}

// Read schema information
int SqlSncApi::ReadSchema(const char *select, const char * /*exclude*/, bool /*read_cns*/, bool read_idx)
{
    ClearSchema();

	if(read_idx)
	{
		ReadIndexes(select);
		ReadIndexColumns(select);
	}

	return 0;
}

// Read information about indexes 
int SqlSncApi::ReadIndexes(const char *select)
{
    std::string condition;

	// Get a filter for selected tables
	GetSelectionCriteria(select, "USER_NAME(o.uid)", "o.name", condition, NULL, false);

    // Indexes except PK and UNIQUE constraint indexes, tested on SQL Server 2008, 2000 
	std::string query = "SELECT NULL, i.name, USER_NAME(o.uid), o.name, o.id, CAST(i.indid AS INT),"; 
    query += " CASE INDEXPROPERTY(i.id,i.name,'IsUnique') WHEN 1 THEN 'Y' ELSE 'N' END";
    query += " FROM sysindexes i, sysobjects o";
    query += " WHERE i.id = o.id AND i.name NOT IN (SELECT name FROM sysobjects where parent_obj = i.id and xtype in ('PK','UQ'))";
	
    // SQL Server 2000 return indexes on system tables
    query += " AND o.name NOT IN ('syscolumns','syscomments','sysdepends','sysfiles','sysfiles1',";
    query += "'sysforeignkeys','sysfulltextcatalogs','sysfulltextnotify','sysindexes','sysindexkeys',";
    query += "'sysmembers','sysobjects','syspermissions','sysproperties','sysprotects','sysreferences',";
    query += "'systypes','sysusers')";

    // skip indid=0, this is not index, just a record for table itself (heap index?), index name is NULL
    // in SQL Server in this case; index name = table name in SQL Server 2000
    query += " AND indid > 0";

    // SQL Server 2008 returns indexes on system objects in sys schema, skip them as well
    query += " AND USER_NAME(o.uid) <> 'sys'";

	// Add filter
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
                std::string tab;
                tab = QuoteName(cols[3]._data + cols[3]._fetch_len * i, (size_t)len);

                idx.t_name = new char[tab.length() + 1];
                strcpy(idx.t_name, tab.c_str());
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
int SqlSncApi::ReadIndexColumns(const char *select)
{
    std::string condition;

	// Get a filter for selected tables
	GetSelectionCriteria(select, "USER_NAME(o.uid)", "o.name", condition, NULL, false);

    // Index columns, tested on SQL Server 2008, 2000 
    std::string query = "SELECT i.id, CAST(i.indid AS INT), c.name";
    query += " FROM sysindexkeys i, sysobjects o, syscolumns c";
    query += " WHERE i.id = o.id and o.id = c.id and i.colid = c.colid";

	// Add filter
	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;       
	}

    // Sort by column order in the index
    query += " ORDER by i.id, i.indid, i.keyno";
	
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
            idx_col.asc = true;
			
			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Table ID - INTEGER
			if(len == 4)
				idx_col.tabid = *((int*)(cols[0]._data + cols[0]._fetch_len * i));

			len = (SQLLEN)cols[1].ind[i];

			// Index ID - INTEGER
			if(len == 4)
				idx_col.idxid = *((int*)(cols[1]._data + cols[1]._fetch_len * i));

			len = (SQLLEN)cols[2].ind[i];

 			// Column name
			if(len != -1)
			{
                idx_col.column = new char[(size_t)len + 1];

				strncpy(idx_col.column, cols[2]._data + cols[2]._fetch_len * i, (size_t)len);
				idx_col.column[len] = '\x0';
			}

            _table_ind_columns.push_back(idx_col);
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

// Get a list of columns for specified index
int SqlSncApi::GetIndexColumns(SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts)
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
int SqlSncApi::ReadConstraintTable(const char * /*schema*/, const char * /*constraint*/, std::string & /*table*/)
{
	return -1;
}

// Read information about constraint columns
int SqlSncApi::ReadConstraintColumns(const char * /*schema*/, const char * /*table*/, const char * /*constraint*/, std::string & /*cols*/)
{
	return -1;
}

// Build a condition to select objects from the catalog
void SqlSncApi::GetCatalogSelectionCriteria(std::string & /*selection_template*/, std::string & /*output*/)
{
}

// Quote name with [] if required
std::string SqlSncApi::QuoteName(const char *name, size_t len)
{
    std::string n(name, len);

    if(IsQuoteRequired(n) || IsReservedWord(n))
        return '[' + n + ']';

    return n;
}

// Check whether identifier must be quoted
bool SqlSncApi::IsQuoteRequired(const char *name)
{
	if(name == NULL)
		return false;

	// Starts with a digit
	if(*name >= '0' && *name <= '9')
		return true;

    const char *cur = name;

    // Contains a special character
    while(*cur != '\x0')
    {
        if(strchr(g_spec_symbols, *cur) != NULL)
            return true;

        cur++;
    }

	return false;
}

// Fill reserved words
int SqlSncApi::ReadReservedWords()
{
	// Reserved words not allowed in identifiers
	_reserved_words_ddl.push_back("transaction");   
	return 0;
}

// Define which SQL Server Native Client driver to use
void SqlSncApi::DefineDriver(std::string &driver, std::string &dll)
{
#if defined(WIN32) || defined(_WIN64)
	HKEY hKey;

	// Keys: SQL Server, SQL Server Native Client 10.0, SQL Server Native Client 11.0
	int rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\ODBC\\ODBCINST.INI", 0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKey);

	if(rc != ERROR_SUCCESS)
		return;

	char key[1024];
	DWORD i = 0;

	bool sql2000 = false;
	bool sql2005 = false;
	bool sql2008 = false;
	bool sql2012 = false;

	bool more = true;

	// Enumerate through all keys
	while(more)
	{
		// Size modified with each call to RegEnumKeyEx
		int size = 1024;

		// Get next key
		rc = RegEnumKeyEx(hKey, i, key, (LPDWORD)&size, NULL, NULL, NULL, NULL);
 
		if(rc != ERROR_SUCCESS)
		{
			more = false;
			break;
		}

		// SQL Server 2000 driver
		if(strcmp(key, "SQL Server") == 0)
			sql2000 = true;
		else
		// SQL Server 2005 Native client
		if(strcmp(key, "SQL Native Client") == 0)
			sql2005 = true;
		else
		// SQL Server 2008 Native client
		if(strcmp(key, "SQL Server Native Client 10.0") == 0)
			sql2008 = true;
		else
		// SQL Server 2012, 2014 and 2016 Native client
		if(strcmp(key, "SQL Server Native Client 11.0") == 0)
			sql2012 = true;

		i++;
	}

	RegCloseKey(hKey);

	// Return the highest available version (this driver is also used for 2014 and 2016)
	if(sql2012)
	{
		driver = "Driver={SQL Server Native Client 11.0};";
		dll = "sqlncli11.dll";
	}
	else
	if(sql2008)
	{
		driver = "Driver={SQL Server Native Client 10.0};";
		dll = "sqlncli10.dll";
	}
	else
	if(sql2005)
	{
		driver = "Driver={SQL Native Client};";
		dll = "sqlncli.dll";
	}
	else
	if(sql2000)
	{
		driver = "Driver={SQL Server};";
		dll = "sqlsrv32.dll";
	}
    
#endif
}

// Set version of the connected database
void SqlSncApi::SetVersion()
{
	// Includes full product name
	// Microsoft SQL Server 2008 R2 (RTM) - 10.50.1617.0 (Intel X86) Apr 22 2011 11:57:00 Copyright (c) Microsoft Corporation Express Edition with Advanced Services on Windows NT 6.1 <X86> (Build 7600: )
	// Microsoft SQL Server 2014 (SP2) (KB3171021) - 12.0.5000.0 (X64) Jun 17 2016 19:14:09 Copyright (c) Microsoft Corporation	Express Edition (64-bit) on Windows NT 6.3 <X64> (Build 10586: )
	const char *query = "SELECT CAST(@@VERSION AS VARCHAR(300))";

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor 
	int rc = OpenCursor(query, 1, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	// Get the value
	if(rc >= 0 && cols != NULL && cols[0].ind != NULL && cols[0]._data != NULL)
	{
		SQLLEN len = (SQLLEN)cols[0].ind[0];

		// Skip 2nd and 3rd line containing release date and copyright
		if(len != -1)
		{
			const char *cur = cols[0]._data;

			int line = 1;

			for(int i = 0; i < len; i++)
			{
				if(cur[i] == '\n')
				{
					line++;
					continue;
				}

				if(line == 2 || line == 3 || cur[i] == '\r' || cur[i] == '\t')
					continue;

				_version += cur[i];
			}
		}
	}

	CloseCursor();
}

// Set error code and message for the last API call
void SqlSncApi::SetError(SQLSMALLINT handle_type, SQLHANDLE handle)
{
	SQLCHAR sqlstate[6]; *sqlstate = '\x0';

	_native_error_text[0] = '\x0';

	// Get native error information
	_SQLGetDiagRec(handle_type, handle, 1, sqlstate, (SQLINTEGER*)&_native_error, (SQLCHAR*)_native_error_text, 1024, NULL);

	_error = SQL_DBAPI_UNKNOWN_ERROR;
	*_error_text = '\x0';

	// Invalid object name '%.*ls' error
	if(_native_error == 208)
		_error = SQL_DBAPI_NOT_EXISTS_ERROR;
	else
	// Cannot find the object "%.*ls" because it does not exist or you do not have permissions. (Returned by TRUNCATE TABLE for non-existing table)
	if(_native_error == 4701)
		_error = SQL_DBAPI_NOT_EXISTS_ERROR;
	else
	// Cannot %S_MSG the %S_MSG '%.*ls', because it does not exist or you do not have permission (Returned by DROP TABLE).
	if(_native_error == 3701)
		_error = SQL_DBAPI_NOT_EXISTS_ERROR;
}
