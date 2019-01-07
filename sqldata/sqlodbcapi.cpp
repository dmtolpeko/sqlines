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

// SqlOdbcApi ODBC API

#include "sqlodbcapi.h"
#include "sqlociapi.h"
#include "../sqlcommon/str.h"
#include "../sqlcommon/os.h"

// Constructor
SqlOdbcApi::SqlOdbcApi()
{
	_db_type = 0;

	_henv = SQL_NULL_HANDLE;
	_hdbc = SQL_NULL_HANDLE;
	_hstmt_cursor = SQL_NULL_HANDLE;

	_cursor_fetched = 0;

	_sqlGetData_features = 0;
}

SqlOdbcApi::~SqlOdbcApi()
{
	// Note that some drivers Oracle ODBC i.e. cause application crash on exit when not deallocated
	Deallocate();
}

// Initialize API
int SqlOdbcApi::Init()
{
	// No driver load is required as ODBC manager is used
	return 0;
}

// Free connection and environment handles (When other ODBC drivers are used crash can happen on exit when not disconnected and not deallocated)
void SqlOdbcApi::Deallocate()
{
	int rc = 0;

	if(_connected == true)
	{
		rc = SQLDisconnect(_hdbc);
		_connected = false;
	}

	if(_hdbc != SQL_NULL_HANDLE)
	{
		rc = SQLFreeHandle(SQL_HANDLE_DBC, _hdbc);
		_hdbc = SQL_NULL_HANDLE;
	}

	if(_henv != SQL_NULL_HANDLE)
	{
		rc = SQLFreeHandle(SQL_HANDLE_ENV, _henv);
		_henv = SQL_NULL_HANDLE;
	}
}

// Set additional information about the driver type
void SqlOdbcApi::SetDriverType(const char *info)
{
	if(info == NULL)
		return;

	const char *end = Str::SkipUntil(info, ',');

	// Check for Oracle ODBC
	if(_strnicmp(info, "oracle in", 9) == 0)
	{
		_db_type = SQLDATA_ORACLE;		
		_driver.assign(info, (size_t)(end - info)); 
	}
}

// Set the connection string in the API object
void SqlOdbcApi::SetConnectionString(const char *conn)
{
	if(conn == NULL)
		return;

	// Oracle connection
	if(_db_type == SQLDATA_ORACLE)
	{
		SetOracleConnectionString(conn);
	}
	
	return;
}

// Set the connection string to connect to Oracle
void SqlOdbcApi::SetOracleConnectionString(const char *conn)
{
	if(conn == NULL)
		return;

	SplitConnectionString(conn, _user, _pwd, _db);

	_conn = "Dbq=";
	_conn += _db;
}

// Connect to the database
int SqlOdbcApi::Connect(size_t * /*time_spent*/)
{
	// Allocate environment handle
	int rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_henv);

	// Set ODBC 3 version
	if(rc == SQL_SUCCESS)
		SQLSetEnvAttr(_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

	// Allocate connection handle
	rc = SQLAllocHandle(SQL_HANDLE_DBC, _henv, &_hdbc);

	// Build connection string
	std::string conn = "Driver={";
	conn += _driver;
	conn += "};";
	conn += _conn;
	conn += ";Uid=";
	conn += _user;
	conn += ";Pwd=";
	conn += _pwd;
	conn += ";";

	SQLCHAR full_conn[1024];
	SQLSMALLINT full_conn_out_len;
	
	rc = SQLDriverConnect(_hdbc, NULL, reinterpret_cast<SQLCHAR*>(const_cast<char*>(conn.c_str())), SQL_NTS, full_conn, 1024, &full_conn_out_len, SQL_DRIVER_NOPROMPT);
	
	if(rc == SQL_ERROR)
		SetError(SQL_HANDLE_DBC, _hdbc);
	else
		_connected = true;

	if(rc == SQL_ERROR)
		SQLGetInfo(_hdbc, SQL_GETDATA_EXTENSIONS, &_sqlGetData_features, 0, NULL);     

	return rc;
}

// Get row count for the specified object
int SqlOdbcApi::GetRowCount(const char *object, long *count, size_t *time_spent)
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
int SqlOdbcApi::ExecuteScalar(const char *query, long *result, size_t *time_spent)
{
	SQLHANDLE stmt;
	int q_result = 0;

	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = SQLExecDirect(stmt, reinterpret_cast<SQLCHAR*>(const_cast<char*>(query)), SQL_NTS);

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
int SqlOdbcApi::ExecuteNonQuery(const char *query, size_t *time_spent)
{
	SQLHANDLE stmt;

	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = SQLExecDirect(stmt, reinterpret_cast<SQLCHAR*>(const_cast<char*>(query)), SQL_NTS);

	// Error raised
	if(rc == -1)
		SetError(SQL_HANDLE_STMT, stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Open cursor and allocate buffers
int SqlOdbcApi::OpenCursor(const char *query, long buffer_rows, long buffer_memory, long *col_count, long *allocated_array_rows, 
		long *rows_fetched, SqlCol **cols, size_t *time_spent, bool /*catalog_query*/, std::list<SqlDataTypeMap> * /*dtmap*/)
{
	size_t start = GetTickCount();

	// Allocate a statement handle
	int rc = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &_hstmt_cursor);

	// Execute the query
	rc = SQLExecDirect(_hstmt_cursor, reinterpret_cast<SQLCHAR*>(const_cast<char*>(query)), SQL_NTS);

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

		rc = SQLDescribeCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLCHAR*)_cursor_cols[i]._name, 256, NULL, &native_dt, &column_size,
					&decimal_digits, &nullable);
      
		_cursor_cols[i]._native_dt = native_dt;
		_cursor_cols[i]._len = column_size;

		// Save precision and scale for decimal type (returned for Oracle NUMBER(p,s)
		if(native_dt ==	SQL_DECIMAL)
		{
			_cursor_cols[i]._precision = (int)column_size;
			_cursor_cols[i]._scale = decimal_digits;
		}
		else
		// For LOB columns, size 2G is returned, reduce the row buffer
		if(native_dt ==	SQL_LONGVARCHAR || native_dt ==	SQL_LONGVARBINARY)
		{
			_cursor_cols[i]._len = 8000;
			_cursor_lob_exists = true;
		}

		if(nullable == SQL_NULLABLE)
			_cursor_cols[i]._nullable = true;
		else
			_cursor_cols[i]._nullable = false;

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
		// Oracle NUMBER(p,0) p <=9 represented as DECIMAL
		if(_db_type == SQLDATA_ORACLE && _cursor_cols[i]._native_dt == SQL_DECIMAL && _cursor_cols[i]._precision <= 9 &&
			_cursor_cols[i]._scale == 0)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_LONG;
			_cursor_cols[i]._fetch_len = sizeof(int);
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// DECIMAL (also Oracle NUMERIC(p,s)
		if(_cursor_cols[i]._native_dt == SQL_DECIMAL)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_CHAR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// Date and time (including Oracle DATE)
		if(_cursor_cols[i]._native_dt == SQL_TYPE_TIMESTAMP)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_TYPE_TIMESTAMP;
			_cursor_cols[i]._fetch_len = sizeof(SQL_TIMESTAMP_STRUCT);
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// ODBC FLOAT
		if(_cursor_cols[i]._native_dt == SQL_FLOAT)
		{
			_cursor_cols[i]._native_fetch_dt = SQL_C_FLOAT;
			_cursor_cols[i]._fetch_len = sizeof(float);
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}	
		else
		// Character or binary LOB, do not bind
		if(_cursor_cols[i]._native_dt == SQL_LONGVARCHAR || _cursor_cols[i]._native_dt == SQL_LONGVARBINARY)
		{
			_cursor_cols[i]._native_fetch_dt = _cursor_cols[i]._native_dt;
			_cursor_cols[i]._fetch_len = 0;
			_cursor_cols[i]._data = NULL;
		}	
		
		// Bind the data to array
		if(_cursor_cols[i]._data != NULL)
		{
			// Allocate indicators
			_cursor_cols[i].ind = new long[_cursor_allocated_rows];

			rc = SQLBindCol(_hstmt_cursor, (SQLUSMALLINT)(i + 1), (SQLSMALLINT)_cursor_cols[i]._native_fetch_dt, _cursor_cols[i]._data, (SQLLEN)_cursor_cols[i]._fetch_len,
								(SQLLEN*)_cursor_cols[i].ind);

			if(rc == -1)
				SetError(SQL_HANDLE_STMT, _hstmt_cursor);
		}
	}

	// Prepare array fetch
	rc = SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)_cursor_allocated_rows, 0); 
	rc = SQLSetStmtAttr(_hstmt_cursor, SQL_ATTR_ROWS_FETCHED_PTR, &_cursor_fetched, 0);

	// Perform initial fetch, return SQL_SUCCESS even if there are less rows than array size
	rc = SQLFetchScroll(_hstmt_cursor, SQL_FETCH_NEXT, 0);

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
int SqlOdbcApi::Fetch(long *rows_fetched, size_t *time_spent) 
{
	if(_cursor_allocated_rows <= 0)
		return -1;

	size_t start = GetTickCount();

	// Fetch the data
	int rc = SQLFetchScroll(_hstmt_cursor, SQL_FETCH_NEXT, 0);

	if(rows_fetched != NULL)
		*rows_fetched = _cursor_fetched;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;
	
	return rc;
}

// Close the cursor and deallocate buffers
int SqlOdbcApi::CloseCursor()
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
int SqlOdbcApi::InitBulkTransfer(const char * /*table*/, long /*col_count*/, long /*allocated_array_rows*/, SqlCol * /*s_cols*/, SqlCol ** /*t_cols*/)
{
	return -1;
}

// Transfer rows between databases
int SqlOdbcApi::TransferRows(SqlCol * /*s_cols*/, long /*rows_fetched*/, long * /*rows_written*/, size_t * /*bytes_written*/,
							size_t * /*time_spent*/)
{
	return -1;
}

// Write LOB data using BCP API
int SqlOdbcApi::WriteLob(SqlCol * /*s_cols*/, int /*row*/, int * /*lob_bytes*/)
{
	return -1;
}

// Complete bulk transfer
int SqlOdbcApi::CloseBulkTransfer()
{
	return -1;
}

// Specifies whether API allows to parallel reading from this API and write to another API
bool SqlOdbcApi::CanParallelReadWrite()
{
	if(_cursor_lob_exists == true)
		return false;

	return true;
}

// Drop the table
int SqlOdbcApi::DropTable(const char* /*table*/, size_t * /*time_spent*/, std::string & /*drop_stmt*/)
{
	return -1;
}

// Remove foreign key constraints referencing to the parent table
int SqlOdbcApi::DropReferences(const char* /*table*/, size_t * /*time_spent*/)
{
	return -1;
}

// Get the length of LOB column in the open cursor
int SqlOdbcApi::GetLobLength(long /*row*/, long /*column*/, long * /*length*/)
{
	return -1;
}

// Get LOB content
int SqlOdbcApi::GetLobContent(long /*row*/, long /*column*/, void * /*data*/, long /*length*/, long * /*len_ind*/)
{
	return -1;
}

// Get partial LOB content
int SqlOdbcApi::GetLobPart(long /*row*/, long column, void *data, long length, long *len_ind)
{
	SQLLEN ind;
	int rc = SQLGetData(_hstmt_cursor, (SQLUSMALLINT)(column + 1), SQL_C_BINARY, data, (SQLLEN)length, &ind);

	if(rc == -1)
		SetError(SQL_HANDLE_STMT, _hstmt_cursor);

	if(len_ind != NULL)
		*len_ind = (int)ind;

	return rc;
}

// Get the list of available tables
int SqlOdbcApi::GetAvailableTables(std::string &table_template, std::string & /*exclude*/, 
									std::list<std::string> &tables)
{
	std::string query;

	// Oracle connection
	if(_db_type == SQLDATA_ORACLE)
	{
		SqlOciApi::GetAllTablesQuery(table_template, query, _user.c_str());
	}

	SQLHANDLE stmt;

	// Allocate a statement handle
	int rc = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &stmt);

	// Execute the query
	rc = SQLExecDirect(stmt, reinterpret_cast<SQLCHAR*>(const_cast<char*>(query.c_str())), SQL_NTS);

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
		tab += name;

		tables.push_back(tab);
	}

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return (rc == SQL_NO_DATA ) ? 0 : rc;
}

// Read schema information
int SqlOdbcApi::ReadSchema(const char * /*select*/, const char * /*exclude*/, bool /*read_cns*/, bool /*read_idx*/)
{
	return -1;
}

// Get table name by constraint name
int SqlOdbcApi::ReadConstraintTable(const char * /*schema*/, const char * /*constraint*/, std::string & /*table*/)
{
	return -1;
}

// Read information about constraint columns
int SqlOdbcApi::ReadConstraintColumns(const char * /*schema*/, const char * /*table*/, const char * /*constraint*/, std::string & /*cols*/)
{
	return -1;
}

// Build a condition to select objects from the catalog
void SqlOdbcApi::GetCatalogSelectionCriteria(std::string & /*selection_template*/, std::string & /*output*/)
{
}

// Set error code and message for the last API call
void SqlOdbcApi::SetError(SQLSMALLINT handle_type, SQLHANDLE handle)
{
	SQLCHAR sqlstate[6]; *sqlstate = '\x0';

	// Get native error information
	SQLGetDiagRec(handle_type, handle, 1, sqlstate, (SQLINTEGER*)&_native_error, (SQLCHAR*)_native_error_text, 1024, NULL);

	_error = SQL_DBAPI_UNKNOWN_ERROR;
	*_error_text = '\x0';
}
