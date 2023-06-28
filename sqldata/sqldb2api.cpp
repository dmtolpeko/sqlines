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

// SqlDb2Api DB2 ODBC API

#include <stdlib.h>
#include "sqldb2api.h"
#include "str.h"
#include "os.h"

// Oracle OCI, Sybase ASE, MySQL C definitions
#include <oci.h>
#include <ctpublic.h>
#include <mysql.h>

// Constructor
SqlDb2Api::SqlDb2Api()
{
	_henv = SQL_NULL_HANDLE;
	_hdbc = SQL_NULL_HANDLE;

	_hstmt_cursor = SQL_NULL_HANDLE;
	_hstmt_insert = SQL_NULL_HANDLE;

	_cursor_fetched = 0;
	_rows_processed = 0;

	_dll = NULL;

	_SQLAllocHandle = NULL;
	_SQLBindCol = NULL;
	_SQLBindParameter = NULL; 
	_SQLConnect = NULL;
	_SQLDescribeCol = NULL;
	_SQLDisconnect = NULL;
	_SQLDriverConnect = NULL;
	_SQLEndTran = NULL;
	_SQLExecDirect = NULL;
	_SQLExecute = NULL;
	_SQLFetch = NULL;
	_SQLFreeHandle = NULL;
	_SQLGetDiagRec = NULL;
	_SQLNumResultCols = NULL;
	_SQLPrepare = NULL;
	_SQLSetEnvAttr = NULL;
	_SQLSetConnectAttr = NULL;
	_SQLSetStmtAttr = NULL;
}

SqlDb2Api::~SqlDb2Api()
{
	Deallocate();
}

// Initialize API
int SqlDb2Api::Init()
{
#if defined(WIN32) || defined(_WIN64)

	// Force UTF-8 codepage at the client side. By default, if DB2CODEPAGE environment variable is not set, DB2 uses regional settings
	if(_target_api_provider != NULL && _target_api_provider->IsTargetUtf8LoadSupported())
		_putenv("DB2CODEPAGE=1208");

	// Try to load the library by default path
	_dll = LoadLibraryEx(DB2_DLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	// DLL load failed
	if(_dll == NULL)
	{
		std::list<std::string> paths;

		// Try to find DB2 installation paths
		FindDb2Paths(paths);

		for(std::list<std::string>::iterator i = paths.begin(); i != paths.end() ; i++)
		{
			// For DB2 10.1 and 11 path already includes terminating \ and DLL is located in BIN
			std::string path = (*i) + "BIN\\";
			path += DB2_DLL;
			
			// Try to open DLL
			_dll = LoadLibraryEx(path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	
			if(_dll != NULL)
				break;
		}		
	}
#else
	_dll = Os::LoadLibrary(DB2_DLL);
#endif

	// Get functions
	if(_dll != NULL)
	{
		_SQLAllocHandle = (SQLAllocHandleFunc)Os::GetProcAddress(_dll, "SQLAllocHandle");
		_SQLBindCol = (SQLBindColFunc)Os::GetProcAddress(_dll, "SQLBindCol");
		_SQLBindParameter = (SQLBindParameterFunc)Os::GetProcAddress(_dll, "SQLBindParameter");
		_SQLConnect = (SQLConnectFunc)Os::GetProcAddress(_dll, "SQLConnect");
		_SQLDescribeCol = (SQLDescribeColFunc)Os::GetProcAddress(_dll, "SQLDescribeCol");
		_SQLDisconnect = (SQLDisconnectFunc)Os::GetProcAddress(_dll, "SQLDisconnect");
		_SQLDriverConnect = (SQLDriverConnectFunc)Os::GetProcAddress(_dll, "SQLDriverConnect");
		_SQLEndTran = (SQLEndTranFunc)Os::GetProcAddress(_dll, "SQLEndTran");
		_SQLExecDirect = (SQLExecDirectFunc)Os::GetProcAddress(_dll, "SQLExecDirect");
		_SQLExecute = (SQLExecuteFunc)Os::GetProcAddress(_dll, "SQLExecute");
		_SQLFetch = (SQLFetchFunc)Os::GetProcAddress(_dll, "SQLFetch");
		_SQLFreeHandle = (SQLFreeHandleFunc)Os::GetProcAddress(_dll, "SQLFreeHandle");
		_SQLGetDiagRec = (SQLGetDiagRecFunc)Os::GetProcAddress(_dll, "SQLGetDiagRec");
		_SQLNumResultCols = (SQLNumResultColsFunc)Os::GetProcAddress(_dll, "SQLNumResultCols");
		_SQLPrepare = (SQLPrepareFunc)Os::GetProcAddress(_dll, "SQLPrepare");
		_SQLSetEnvAttr = (SQLSetEnvAttrFunc)Os::GetProcAddress(_dll, "SQLSetEnvAttr");
		_SQLSetConnectAttr = (SQLSetConnectAttrFunc)Os::GetProcAddress(_dll, "SQLSetConnectAttr");
		_SQLSetStmtAttr = (SQLSetStmtAttrFunc)Os::GetProcAddress(_dll, "SQLSetStmtAttr");

		if(_SQLAllocHandle == NULL || _SQLBindCol == NULL || _SQLBindParameter == NULL || 
			_SQLConnect == NULL || _SQLDescribeCol == NULL || _SQLDisconnect == NULL || 
			_SQLDriverConnect == NULL || _SQLEndTran == NULL || _SQLExecDirect == NULL || 
			_SQLExecute == NULL || _SQLFetch == NULL || _SQLFreeHandle == NULL || _SQLGetDiagRec == NULL || 
			_SQLPrepare == NULL || _SQLSetEnvAttr == NULL || _SQLSetConnectAttr == NULL || 
			_SQLSetStmtAttr == NULL)
			return -1;
	}
	else
	{
		Os::GetLastErrorText(DB2_DLL_LOAD_ERROR_PREFIX, _native_error_text, 1024);

		if(*_native_error_text == '\x0')
			strcpy(_native_error_text, DB2_DLL_LOAD_ERROR);

		return -1;
	}

	return 0;
}

// Set the connection string in the API object
void SqlDb2Api::SetConnectionString(const char *conn)
{
	if(conn == NULL)
		return;

	SplitConnectionString(conn, _user, _pwd, _db);
}

// Connect to the database
int SqlDb2Api::Connect(size_t *time_spent)
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
	std::string conn = "DSN=";
	conn += _db;
	conn += ";Uid=";
	conn += _user;
	conn += ";Pwd=";
	conn += _pwd;
	conn += ";";

	// Force DB2 to use . as DECIMAL separator no matter what locale settings are. 
	// Otherwise SELECT, but not INSERT (!), returns comma if separator is comma
	conn += "patch2=15;";

	SQLCHAR full_conn[1024];
	SQLSMALLINT full_conn_out_len;
	
	rc = _SQLDriverConnect(_hdbc, NULL, (SQLCHAR*)conn.c_str(), SQL_NTS, full_conn, 1024, &full_conn_out_len, SQL_DRIVER_NOPROMPT);
	
	if(rc == SQL_ERROR)
	{
		SetError(SQL_HANDLE_DBC, _hdbc);

		if(time_spent != NULL)
			*time_spent = Os::GetTickCount() - start;

		return -1;
	}

	_connected = true;

	InitSession();

	// Set version of the connected database
	SetVersion();

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return rc;
}

// Disconnect from the database
void SqlDb2Api::Disconnect() 
{
	if(_connected == false)
		return;

	_SQLDisconnect(_hdbc);
	_SQLFreeHandle(SQL_HANDLE_DBC, _hdbc);
	
	_hdbc = SQL_NULL_HANDLE;
	_connected = false;
}

// Free connection and environment handles 
void SqlDb2Api::Deallocate()
{
	Disconnect();

	if(_henv != SQL_NULL_HANDLE)
	{
		_SQLFreeHandle(SQL_HANDLE_ENV, _henv);
		_henv = SQL_NULL_HANDLE;
	}
}

// Get row count for the specified object
int SqlDb2Api::GetRowCount(const char *object, int *count, size_t *time_spent)
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
int SqlDb2Api::ExecuteScalar(const char *query, int *result, size_t *time_spent)
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
int SqlDb2Api::ExecuteNonQuery(const char *query, size_t *time_spent)
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
int SqlDb2Api::OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool /*catalog_query*/, std::list<SqlDataTypeMap> * /*dtmap*/)
{
	TRACE("DB2 API OpenCursor() Entered");
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
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		SQLSMALLINT native_dt;
		// On 64-bit platform SQLULEN column_size is 64-bit, but DB2 does sets upper 4 bytes to 0 for some reason, we so must initialize
		SQLULEN column_size = 0;
		SQLSMALLINT decimal_digits;
		SQLSMALLINT nullable;

		rc = _SQLDescribeCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLCHAR*)_cursor_cols[i]._name, 256, NULL, &native_dt, &column_size,
					&decimal_digits, &nullable);
      
		_cursor_cols[i]._native_dt = native_dt;
		_cursor_cols[i]._len = column_size;

		// For DECIMAL/NUMERIC column size includes precision only
		if(native_dt == SQL_DECIMAL || native_dt == SQL_NUMERIC)
		{
			_cursor_cols[i]._precision = (int)column_size;
			_cursor_cols[i]._scale = decimal_digits;
		}
		else
		// XML (size is 0 returned for DB2 11)
		if(native_dt == -370)
		{
			_cursor_cols[i]._len = 1048576;
		}
		else
		{
		    // For TIMESTAMP, decimal_digits contain fractional precision 
			_cursor_cols[i]._scale = decimal_digits;
		}

		if(nullable == SQL_NULLABLE)
			_cursor_cols[i]._nullable = true;
		else
			_cursor_cols[i]._nullable = false;

		row_size += _cursor_cols[i]._len;
	}

	_cursor_allocated_rows = 1;

	if(row_size == 0)
        row_size = 10000;

	// Define how many rows fetch at once
	if(buffer_rows > 0)
		_cursor_allocated_rows = buffer_rows;
	else
	if(buffer_memory > 0)
	{
		size_t rows = buffer_memory/row_size;
		_cursor_allocated_rows = rows > 0 ? rows : 1;
	}	

	// Check if max batch size in rows is limited
	if (_batch_max_rows > 0 && _cursor_allocated_rows > _batch_max_rows)
		_cursor_allocated_rows = _batch_max_rows;

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
		// LONG VARCHAR, len is 32700
		if(_cursor_cols[i]._native_dt == -1)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// Data type GRAPHIC with code -95 (fixed-length UTF-16) or VARGRAPHIC with code -96 (variable-length UTF-16)
		if(_cursor_cols[i]._native_dt == -95 || _cursor_cols[i]._native_dt == -96)
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
				_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
				_cursor_cols[i]._fetch_len = 6;	
			}

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
		// TIMESTAMP, len is 26
		if(_cursor_cols[i]._native_dt == SQL_TYPE_TIMESTAMP)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_TYPE_TIMESTAMP;
			_cursor_cols[i]._fetch_len = sizeof(SQL_TIMESTAMP_STRUCT);
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// TIME 
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
		// FLOAT (SQL_REAL)
		if(_cursor_cols[i]._native_dt == SQL_REAL)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			// Driver returns 7 as len for FLOAT, it is too short for string representations
			_cursor_cols[i]._fetch_len = 65 + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// CLOB with code -99 and BLOB with code -98
		if(_cursor_cols[i]._native_dt == -99 || _cursor_cols[i]._native_dt == -98)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;

			if(_cursor_cols[i]._native_dt == -98)
				_cursor_cols[i]._native_fetch_dt = SQL_C_BINARY;

			// DB2 allows specifying any length for BLOB, so it not necessary to large and we can fetch as a regular type in arrays
			if(_cursor_cols[i]._len <= 1048576)
			{
				_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
				_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
			}
		}
		else
		// XML with code -370
		if(_cursor_cols[i]._native_dt == -370)
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
			// DB2 64-bit sets indicator only for lowest 4 bytes
			memset(_cursor_cols[i].ind, 0, _cursor_allocated_rows * sizeof(size_t));

			rc = _SQLBindCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLSMALLINT)_cursor_cols[i]._native_fetch_dt, _cursor_cols[i]._data, (SQLLEN)_cursor_cols[i]._fetch_len,
								(SQLLEN*)_cursor_cols[i].ind);

			if(rc == -1)
				SetError(SQL_HANDLE_STMT, _hstmt_cursor);
		}
	}

	// Prepare array fetch
	rc = _SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)_cursor_allocated_rows, 0); 
	rc = _SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROWS_FETCHED_PTR, &_cursor_fetched, 0);

	// Perform initial fetch, return SQL_SUCCESS even if there are less rows than array size
	rc = _SQLFetch(_hstmt_cursor);

	if(rc == -1 || rc == 1)
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);

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

	TRACE("DB2 API OpenCursor() Left");
	return rc;
}

// Fetch next portion of data to allocate buffers
int SqlDb2Api::Fetch(int *rows_fetched, size_t *time_spent) 
{
	if(_cursor_allocated_rows <= 0)
		return -1;

	TRACE("DB2 API Fetch() Entered");

	size_t start = GetTickCount();

	// Fetch the data
	int rc = _SQLFetch(_hstmt_cursor);

	if(rows_fetched != NULL)
		*rows_fetched = _cursor_fetched;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;
	
	TRACE("DB2 API Fetch() Left");
	return rc;
}

// Close the cursor and deallocate buffers
int SqlDb2Api::CloseCursor()
{
	TRACE("DB2 API CloseCursor() Entered");

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

	TRACE("DB2 API CloseCursor() Left");
	return rc;
}

// Initialize the bulk copy from one database into another
int SqlDb2Api::InitBulkTransfer(const char *table, size_t col_count, size_t allocated_array_rows, SqlCol *s_cols, SqlCol ** /*t_cols*/)
{
	// Set AUTOCOMMIT OFF
	int rc = _SQLSetConnectAttr(_hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0);

	// Allocate a statement handle
	rc = _SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &_hstmt_insert);

	std::string insert = "INSERT INTO ";
	insert += table;
	insert += " VALUES (";

	// Add parameter markers
	for(int i = 0; i < col_count; i++)
		insert += (i > 0) ? ", ?" : "?";

	insert += ")";

	rc = _SQLPrepare(_hstmt_insert, (SQLCHAR*)insert.c_str(), SQL_NTS);

	if(rc == -1)
	{
		SetError(SQL_HANDLE_STMT, _hstmt_insert);
		_SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_insert); 

		_hstmt_insert = SQL_NULL_HANDLE;

		return -1;
	}

	// Specify to bind array by columns
	rc = _SQLSetStmtAttr(_hstmt_insert, SQL_ATTR_PARAM_BIND_TYPE, SQL_PARAM_BIND_BY_COLUMN, 0);

	// Specify the array size
	rc = _SQLSetStmtAttr(_hstmt_insert, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)allocated_array_rows, 0);

	// Number of processed rows
	rc = _SQLSetStmtAttr(_hstmt_insert, SQL_ATTR_PARAMS_PROCESSED_PTR, &_rows_processed, 0);

	_ins_cols_count = col_count;
	_ins_allocated_rows = allocated_array_rows;
	_ins_cols = new SqlCol[col_count];

	// Bind columns
	for(int i = 0; i < col_count; i++)
	{
		// Check whether the source buffer bound
		if(s_cols[i]._data == NULL)
			continue;

		strcpy(_ins_cols[i]._name, s_cols[i]._name);
		
		// SQL Server CHAR, Informix CHAR
		if(
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX)
				&& s_cols[i]._native_fetch_dt == SQL_C_CHAR) ||
			// Any MySQL type bound to string
			_source_api_type == SQLDATA_MYSQL)
		{
			// Bind to source buffer directly
			rc = _SQLBindParameter(_hstmt_insert, (SQLUSMALLINT)(i + 1), SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 
				s_cols[i]._len, 0, s_cols[i]._data, (SQLLEN)s_cols[i]._fetch_len, (SQLLEN*)s_cols[i].ind);
 		}
		else
		// Oracle CHAR and VARCHAR2
		if((_source_api_type == SQLDATA_ORACLE && s_cols[i]._native_fetch_dt == SQLT_STR) ||
			// Sybase CHAR
			(_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_CHAR_TYPE))
		{
			// Allocate space for NULL/length indicators
			_ins_cols[i].ind = new size_t[allocated_array_rows];

			// Bind data buffer directly
			rc = _SQLBindParameter(_hstmt_insert, (SQLUSMALLINT)(i + 1), SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 
					s_cols[i]._len, 0, s_cols[i]._data, (SQLLEN)s_cols[i]._fetch_len, (SQLLEN*)_ins_cols[i].ind);
		}		
		else
		// SQL Server, Informix INT
		if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX)
					&& s_cols[i]._native_dt == SQL_INTEGER) 
		{
			// Bind to source buffer directly
			rc = _SQLBindParameter(_hstmt_insert, (SQLUSMALLINT)(i + 1), SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 
				sizeof(int), 0, s_cols[i]._data, sizeof(int), (SQLLEN*)s_cols[i].ind);
		}
		else
		// Sybase INT
		if(_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_INT_TYPE)
		{
			_ins_cols[i].ind = new size_t[allocated_array_rows];

			// Bind data buffer directly
			rc = _SQLBindParameter(_hstmt_insert, (SQLUSMALLINT)(i + 1), SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 
					sizeof(int), 0, s_cols[i]._data, sizeof(int), (SQLLEN*)_ins_cols[i].ind);
		}
		else
		// SQL Server, Informix SMALLINT
		if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX)
					&& s_cols[i]._native_dt == SQL_SMALLINT) 
		{
			// Bind to source buffer directly
			rc = _SQLBindParameter(_hstmt_insert, (SQLUSMALLINT)(i + 1), SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 
				sizeof(short), 0, s_cols[i]._data, sizeof(short), (SQLLEN*)s_cols[i].ind);
		}
		else
		// Sybase SMALLINT
		if(_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_SMALLINT_TYPE)
		{
			_ins_cols[i].ind = new size_t[allocated_array_rows];

			// Bind data buffer directly
			rc = _SQLBindParameter(_hstmt_insert, (SQLUSMALLINT)(i + 1), SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 
					sizeof(short), 0, s_cols[i]._data, sizeof(short), (SQLLEN*)_ins_cols[i].ind);
		}
	}

	return rc;
}

// Transfer rows between databases
int SqlDb2Api::TransferRows(SqlCol *s_cols, int rows_fetched, int *rows_written, size_t *bytes_written,
							size_t *time_spent)
{
	size_t start = GetTickCount();
	int bytes = 0;
	int rc = 0;

	if(rows_fetched <= 0)
		return -1;

	// Reset the array size for last portion
	if(rows_fetched != _ins_allocated_rows)
		rc = _SQLSetStmtAttr(_hstmt_insert, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)rows_fetched, 0);

	// Prepare buffers and calculate data size
	for(int i = 0; i < rows_fetched; i++)
	{
		for(int k = 0; k < _ins_cols_count; k++)
		{
			if(_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX ||
				_source_api_type == SQLDATA_MYSQL)
			{
				// Calculate size for non-NULL values
				if(s_cols[k].ind != NULL && s_cols[k].ind[i] != -1)
					bytes += (int)s_cols[k].ind[i];
			}
			else
			if(_source_api_type == SQLDATA_ORACLE)
			{
				// Copy indicators and calculate size for non-NULL values
				if(s_cols[k]._ind2[i] != -1)
				{
					_ins_cols[k].ind[i] = (size_t)s_cols[k]._len_ind2[i];
					bytes += s_cols[k]._len_ind2[i];
				}
				else
					_ins_cols[k].ind[i] = (size_t)-1;
			}
			else
			if(_source_api_type == SQLDATA_SYBASE)
			{
				// Copy indicators and calculate size for non-NULL values
				if(s_cols[k]._ind2[i] != -1)
				{
					_ins_cols[k].ind[i] = (size_t)s_cols[k]._len_ind4[i];
					bytes += s_cols[k]._len_ind4[i];
				}
				else
					_ins_cols[k].ind[i] = (size_t)-1;
			}
		}
	}

	// Insert rows
	rc = _SQLExecute(_hstmt_insert);

	// Commit the batch
	rc = _SQLEndTran(SQL_HANDLE_DBC, _hdbc, SQL_COMMIT);

	if(rows_written != NULL)
		*rows_written = _rows_processed;

	if(bytes_written != NULL)
		*bytes_written = (size_t)bytes;

	if(time_spent)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Write LOB data using BCP API
int SqlDb2Api::WriteLob(SqlCol * /*s_cols*/, int /*row*/, int * /*lob_bytes*/)
{
	return -1;
}

// Complete bulk transfer
int SqlDb2Api::CloseBulkTransfer()
{
	// Delete allocated buffers
	for(int i = 0; i < _ins_cols_count; i++)
	{
		// Delete indicators
		delete [] _ins_cols[i].ind;
	}

	delete [] _ins_cols;

	_ins_cols = NULL;
	_ins_cols_count = 0;
	_ins_allocated_rows = 0;

	// Close statement
	_SQLFreeHandle(SQL_HANDLE_STMT, _hstmt_insert); 

	_hstmt_insert = SQL_NULL_HANDLE;

	return 0;
}

// Specifies whether API allows to parallel reading from this API and write to another API
bool SqlDb2Api::CanParallelReadWrite()
{
	if(_cursor_lob_exists == true)
		return false;

	return true;
}

// Drop the table
int SqlDb2Api::DropTable(const char* table, size_t *time_spent, std::string &drop_stmt)
{
	drop_stmt = "DROP TABLE ";
	drop_stmt += table;

	int rc = ExecuteNonQuery(drop_stmt.c_str(), time_spent);

	return rc;
}

// Remove foreign key constraints referencing to the parent table
int SqlDb2Api::DropReferences(const char*  /*table*/, size_t * /*time_spent*/)
{
	return -1;
}

// Get the length of LOB column in the open cursor
int SqlDb2Api::GetLobLength(size_t /*row*/, size_t /*column*/, size_t * /*length*/)
{
	return -1;
}

// Get LOB content
int SqlDb2Api::GetLobContent(size_t /*row*/, size_t /*column*/, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	return -1;
}

// Get partial LOB content
int SqlDb2Api::GetLobPart(size_t /*row*/, size_t /*column*/, void* /*data*/, size_t /*length*/, int* /*len_ind*/)
{
	/*int rc = SQLGetData(_hstmt_cursor, (SQLUSMALLINT)(column + 1), SQL_C_BINARY, data, (SQLLEN)length, (SQLLEN*)len_ind);

	if(rc == -1)
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);

	return rc;*/
	return -1;
}

// Get the list of available tables
int SqlDb2Api::GetAvailableTables(std::string &table_template, std::string & /*exclude*/, 
									std::list<std::string> &tables)
{
	std::string condition;
	std::string condition2;

	// Get a condition to select objects from the catalog
	GetSelectionCriteria(table_template.c_str(), "tabschema", "tabname", condition, _user.c_str(), true);
	GetSelectionCriteria(table_template.c_str(), "creator", "name", condition, _user.c_str(), true);

	// Build the query
	std::string query = "SELECT tabschema, tabname FROM syscat.tables WHERE type='T'";
	query += " AND tabschema NOT IN ('SYSIBM', 'SYSTOOLS')";

	// DB2 for z/OS
	std::string query2 = "SELECT creator, name FROM sysibm.systables WHERE type='T'";
	query2 += " AND creator NOT IN ('SYSIBM')";
 	
	// Add filter
	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;

		query2 += " AND ";
		query2 += condition;
	}

	SQLHANDLE stmt;

	// Allocate a statement handle
	int rc = _SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = _SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

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
		tab += name;

		tables.push_back(tab);
	}

	_SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return (rc == SQL_NO_DATA ) ? 0 : rc;
}

// Read schema information
int SqlDb2Api::ReadSchema(const char *select, const char *exclude, bool read_cns, bool /*read_idx*/)
{
	int rc = 0;
	std::string condition;
	ClearSchema();

	// Get a condition to select objects from the catalog 
	// * means all user tables, *.* means all tables accessible by the user
	GetSelectionCriteria(select, exclude, "t.tabschema", "t.tabname", condition, _user.c_str(), false);

	if(read_cns)
	{
		rc = ReadTableConstraints(condition);
		rc = ReadConstraintColumns(condition);
	}

	return rc;
}

// Read information about table constraints
int SqlDb2Api::ReadTableConstraints(std::string &condition)
{
	// Tested on DB2 11 (type 'K' is for CHECK constraint)
	std::string query = "SELECT t.tabschema, t.tabname, t.constname, t.type";
	query += " FROM syscat.tabconst t WHERE t.type in ('P', 'U' /*, 'F', 'K'*/)";

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
			col_cns.schema = NULL;
			col_cns.table = NULL;
			col_cns.constraint = NULL;
			col_cns.type = '\x0';

			SQLLEN len = GetLen((SQLLEN*)cols[0].ind, i);

			// Schema name
			if(len != -1)
			{
				col_cns.schema = new char[(size_t)len + 1];

				strncpy(col_cns.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				col_cns.schema[len] = '\x0';
			}

			len = GetLen((SQLLEN*)cols[1].ind, i);
			
			// Table name
			if(len != -1)
			{
				col_cns.table = new char[(size_t)len + 1];

				strncpy(col_cns.table, cols[1]._data + cols[1]._fetch_len * i, (size_t)len);
				col_cns.table[len] = '\x0';
			}
			
			len = GetLen((SQLLEN*)cols[2].ind, i);

			// Constraint name
			if(len != -1)
			{
				col_cns.constraint = new char[(size_t)len + 1];

				strncpy(col_cns.constraint, cols[2]._data + cols[2]._fetch_len * i, (size_t)len);
				col_cns.constraint[len] = '\x0';
			}

			len = GetLen((SQLLEN*)cols[3].ind, i);

			// Constraint type
			if(len != -1)
			{
				char type = *(cols[3]._data + cols[3]._fetch_len * i);

				// Convert F to R for foreign key, K to C for CHECK constraint
				if(type == 'F')
					col_cns.type = 'R';
				else
				if(type == 'K')
					col_cns.type = 'C';
				else
					col_cns.type = type;
			}

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

// Read information about primary and unique key columns
int SqlDb2Api::ReadConstraintColumns(std::string &condition)
{
	// Tested on DB2 11 
	std::string query = "SELECT t.tabschema, t.tabname, t.constname, t.colname";
	query += " FROM syscat.keycoluse t";

	if(condition.empty() == false)
	{
		query += " WHERE ";
		query += condition;
	}
	
	query += " ORDER BY t.tabschema, t.tabname, t.constname, t.colseq";
		
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlConsColumns col_cns;

	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{

			col_cns.schema = NULL;
			col_cns.table = NULL;
			col_cns.constraint = NULL;
			col_cns.column = NULL;

			SQLLEN len = GetLen((SQLLEN*)cols[0].ind, i);

			// Schema name
			if(len != -1)
			{
				col_cns.schema = new char[(size_t)len + 1];

				strncpy(col_cns.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				col_cns.schema[len] = '\x0';
			}

			len = GetLen((SQLLEN*)cols[1].ind, i);
			
			// Table name
			if(len != -1)
			{
				col_cns.table = new char[(size_t)len + 1];

				strncpy(col_cns.table, cols[1]._data + cols[1]._fetch_len * i, (size_t)len);
				col_cns.table[len] = '\x0';
			}
			
			len = GetLen((SQLLEN*)cols[2].ind, i);

			// Constraint name
			if(len != -1)
			{
				col_cns.constraint = new char[(size_t)len + 1];

				strncpy(col_cns.constraint, cols[2]._data + cols[2]._fetch_len * i, (size_t)len);
				col_cns.constraint[len] = '\x0';
			}

			len = GetLen((SQLLEN*)cols[3].ind, i);

			// Column name
			if(len != -1)
			{
				col_cns.column = new char[(size_t)len + 1];

				strncpy(col_cns.column, cols[3]._data + cols[3]._fetch_len * i, (size_t)len);
				col_cns.column[len] = '\x0';
			}

			_table_cons_columns.push_back(col_cns);
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	col_cns.schema = NULL; 
	col_cns.table = NULL; 
	col_cns.constraint = NULL; 
	col_cns.column = NULL; 

	CloseCursor();
	return 0;
}

// Get a list of columns for specified primary or unique key
int SqlDb2Api::GetKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &output, std::list<std::string> *)
{
	bool found = false;

	if(cns.schema == NULL || cns.table == NULL || cns.constraint == NULL)
		return -1;

	for(std::list<SqlConsColumns>::iterator i = _table_cons_columns.begin(); i != _table_cons_columns.end(); i++)
	{
		char *s = (*i).schema;
		char *t = (*i).table;
		char *c = (*i).constraint;
		char *col = (*i).column;

		if(s == NULL || t == NULL || c == NULL || col == NULL)
			continue;

		// Compare schema, table and constraint names (columns are already ordered)
		if(strcmp(cns.constraint, c) == 0 && strcmp(cns.table, t) == 0 && strcmp(cns.schema, s) == 0)
		{
			output.push_back(col);
			found = true;
		}
		else
		if(found)
			break;
	}

	return 0;
}

// Get table name by constraint name
int SqlDb2Api::ReadConstraintTable(const char * /*schema*/, const char * /*constraint*/, std::string & /*table*/)
{
	return -1;
}

// Read information about constraint columns
int SqlDb2Api::ReadConstraintColumns(const char * /*schema*/, const char * /*table*/, const char * /*constraint*/, std::string & /*cols*/)
{
	return -1;
}

// Build a condition to select objects from the catalog
void SqlDb2Api::GetCatalogSelectionCriteria(std::string & /*selection_template*/, std::string & /*output*/)
{
}

// Set version of the connected database
void SqlDb2Api::SetVersion()
{
	// Tested on DB2 10.1, 11 LUW
	std::string query = "SELECT ii.service_level || ', ' || ii.inst_ptr_size || '-bit, ' || si.os_name || ' ' ";
	query += "|| si.os_version || ', ' || si.total_cpus || ' CPU', ";
	query += "	(SELECT pi.installed_prod FROM TABLE(SYSPROC.ENV_GET_PROD_INFO()) AS pi ";
	query += "	WHERE pi.installed_prod IN ('EXP', 'PE', 'WSE', 'WSUE', 'ESE'))";
	query += "FROM TABLE(SYSPROC.ENV_GET_INST_INFO()) AS ii, TABLE(SYSPROC.ENV_GET_SYS_INFO()) AS si";

	// For DB2 8.x
	std::string query2 = "SELECT versionnumber FROM SYSIBM.SYSVERSIONS";

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor 
	int rc = OpenCursor(query.c_str(), 1, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	// Get the value
	if(rc >= 0 && cols != NULL && cols[0].ind != NULL && cols[1].ind != NULL)
	{
		if(cols != NULL && cols[0].ind != NULL && cols[1].ind != NULL)
		{
			_version.assign("IBM DB2 ");

			SQLLEN len = (SQLLEN)cols[1].ind[0];
			char *data = cols[1]._data;

			// Check DB2 edition
			if(!IsIndNull(len) && data != NULL)
			{
				// Express Edition
				if(len == 3 && strncmp(data, "EXP", 3) == 0)
					_version += "Express Edition ";
				else
				// Personal Edition
				if(len == 2 && strncmp(data, "PE", 2) == 0)
					_version += "Personal Edition ";
				else
				// Workgroup Server Edition
				if((len == 3 && strncmp(data, "WSE", 3) == 0) || (len == 4 && strncmp(data, "WSUE", 4) == 0))
					_version += "Workgroup Server Edition ";
				else
				// Enterprise Server Edition
				if(len == 3 && strncmp(data, "ESE", 3) == 0)
					_version += "Enterprise Server Edition ";			
			}		

			len = (SQLLEN)cols[0].ind[0];
			data = cols[0]._data;

			// Version details
			if(len != -1 && data != NULL)
			{
				// If starts with "DB2 v10..." skip first 5 characters
				if(len > 5 && strncmp(data, "DB2 v", 5) == 0)
				{
					len -= 5;
					data += 5;
				}

				_version.append(data, (size_t)len);
			}
		}
	}
	else
	{
		rc = OpenCursor(query2.c_str(), 1, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

		if(rc >= 0 && cols != NULL && cols[0].ind)
		{
			_version.assign("IBM DB2 ");

			SQLLEN len = (SQLLEN)cols[0].ind[0];
			char *data = cols[0]._data;

			// Check DB2 version
			if(!IsIndNull(len) && data != NULL)
			{
				// Values: 8020700, 8020400, 9070100, 11010101
				if(len == 7)
				{
					_version += data[0];
					_version += '.';
					_version += data[2];
					_version += '.';
					_version += data[4];
				}
			}		
		}
	}

	CloseCursor();
	*_native_error_text = '\x0';
}

// Find DB2 installation paths
void SqlDb2Api::FindDb2Paths(std::list<std::string> &paths)
{
#if defined(WIN32) || defined(_WIN64)
	HKEY hKey;

	// At least for DB2 10 server installation
	int rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\IBM\\DB2", 0, KEY_READ, &hKey);

	if(rc != ERROR_SUCCESS)
		return;

	char location[1024];
	int value_size = 1024;

	// For 10 and 11 "DB2 Path Name" parameter contains installation directory WITH terminating '\'
	rc = RegQueryValueEx(hKey, "DB2 Path Name", NULL, NULL, (LPBYTE)location, (LPDWORD)&value_size); 

	if(rc == ERROR_SUCCESS)
		paths.push_back(std::string(location));

	RegCloseKey(hKey);

#endif
}

// Set error code and message for the last API call
void SqlDb2Api::SetError(SQLSMALLINT handle_type, SQLHANDLE handle)
{
	SQLCHAR sqlstate[6]; *sqlstate = '\x0';

	// Get native error information
	_SQLGetDiagRec(handle_type, handle, 1, sqlstate, (SQLINTEGER*)&_native_error, (SQLCHAR*)_native_error_text, 1024, NULL);

	_error = SQL_DBAPI_UNKNOWN_ERROR;
	*_error_text = '\x0';

	// Object not found error, SQL0204N, SQLSTATE 42704 (42S02)
	if(_native_error == -204)
		_error = SQL_DBAPI_NOT_EXISTS_ERROR;
}

// Get length or NULL indicator
SQLLEN SqlDb2Api::GetLen(SQLLEN *ind, int offset)
{
	if(ind == NULL)
		return -1;

#if defined(WIN64)
	// DB2 11 64-bit CLI driver still writes indicators to 4-byte array
	return (SQLLEN)((int*)ind)[offset];
#else
	return ind[offset];
#endif
}

// Check indicator for NULL (there are issues with DB2 64-bit driver)
bool SqlDb2Api::IsIndNull(SQLLEN ind)
{
#if defined(_WIN64)
	// DB2 64-bit driver set -1 to the lowest 4 bytes only
	if(ind == 0x00000000FFFFFFFF)
		return true;
#endif

	if(ind == -1)
		return true;

	return false;
}

