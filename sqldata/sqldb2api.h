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

// SqlDb2Api IBM DB2 ODBC API

#ifndef sqlines_sqldb2api_h
#define sqlines_sqldb2api_h

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h> 
#include "sqlapibase.h"
#include "sqldb.h"

// IBM DB2 CLI/ODBC DLL
#if defined(_WIN64)
#define DB2_DLL				"db2cli64.dll"
#elif defined(WIN32)
#define DB2_DLL				"db2cli.dll"
#else
#define DB2_DLL				"libdb2.so"
#endif

#define DB2_DLL_LOAD_ERROR	        "Loading IBM DB2 CLI " DB2_DLL 
#define DB2_DLL_LOAD_ERROR_PREFIX	DB2_DLL_LOAD_ERROR ":" 

// ODBC API Functions
typedef SQLRETURN (SQL_API *SQLAllocHandleFunc)(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE *OutputHandle);
typedef SQLRETURN (SQL_API *SQLBindColFunc)(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN *StrLen_or_Ind);
typedef SQLRETURN (SQL_API *SQLBindParameterFunc)(SQLHSTMT hstmt, SQLUSMALLINT ipar, SQLSMALLINT fParamType, SQLSMALLINT fCType, SQLSMALLINT fSqlType, SQLULEN cbColDef, SQLSMALLINT ibScale, SQLPOINTER rgbValue, SQLLEN cbValueMax, SQLLEN *pcbValue);
typedef SQLRETURN (SQL_API *SQLConnectFunc)(SQLHDBC ConnectionHandle, SQLCHAR *ServerName, SQLSMALLINT NameLength1, SQLCHAR *UserName, SQLSMALLINT NameLength2, SQLCHAR *Authentication, SQLSMALLINT NameLength3);
typedef SQLRETURN (SQL_API *SQLDescribeColFunc)(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLCHAR *ColumnName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLength, SQLSMALLINT *DataType, SQLULEN *ColumnSize, SQLSMALLINT *DecimalDigits, SQLSMALLINT *Nullable);
typedef SQLRETURN (SQL_API *SQLDisconnectFunc)(SQLHDBC ConnectionHandle);
typedef SQLRETURN (SQL_API *SQLDriverConnectFunc)(SQLHDBC hdbc, SQLHWND hwnd, SQLCHAR *szConnStrIn, SQLSMALLINT cchConnStrIn, SQLCHAR *szConnStrOut, SQLSMALLINT cchConnStrOutMax, SQLSMALLINT *pcchConnStrOut, SQLUSMALLINT fDriverCompletion);
typedef SQLRETURN (SQL_API *SQLEndTranFunc)(SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT CompletionType);
typedef SQLRETURN (SQL_API *SQLExecDirectFunc)(SQLHSTMT StatementHandle, SQLCHAR* StatementText, SQLINTEGER TextLength);
typedef SQLRETURN (SQL_API *SQLExecuteFunc)(SQLHSTMT StatementHandle);
typedef SQLRETURN (SQL_API *SQLFetchFunc)(SQLHSTMT StatementHandle);
typedef SQLRETURN (SQL_API *SQLFreeHandleFunc)(SQLSMALLINT HandleType, SQLHANDLE Handle);
typedef SQLRETURN (SQL_API *SQLGetDiagRecFunc)(SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT RecNumber, SQLCHAR *Sqlstate, SQLINTEGER *NativeError, SQLCHAR* MessageText, SQLSMALLINT BufferLength, SQLSMALLINT *TextLength);
typedef SQLRETURN (SQL_API *SQLNumResultColsFunc)(SQLHSTMT StatementHandle, SQLSMALLINT *ColumnCount);
typedef SQLRETURN (SQL_API *SQLPrepareFunc)(SQLHSTMT StatementHandle, SQLCHAR* StatementText, SQLINTEGER TextLength);
typedef SQLRETURN (SQL_API *SQLSetEnvAttrFunc)(SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength);
typedef SQLRETURN (SQL_API *SQLSetConnectAttrFunc)(SQLHDBC ConnectionHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength);
typedef SQLRETURN (SQL_API *SQLSetStmtAttrFunc)(SQLHSTMT StatementHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength);

class SqlDb2Api : public SqlApiBase
{
	// Environment and connection handles
	SQLHENV _henv;
	SQLHDBC _hdbc;
	SQLHANDLE _hstmt_cursor;
	SQLHANDLE _hstmt_insert;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _db;

	// Attribute to store last number of fetched rows (SQL_ATTR_ROWS_FETCHED_PTR)
	int _cursor_fetched;
	// Number of processed rows in array (SQL_ATTR_PARAMS_PROCESSED_PTR attribute)
	int _rows_processed;

	// DB2 ODBC DDL
#if defined(WIN32) || defined(_WIN64)
	HMODULE _dll;
#else
	void* _dll;
#endif

	// ODBC API functions
	SQLAllocHandleFunc _SQLAllocHandle;
	SQLBindColFunc _SQLBindCol;
	SQLBindParameterFunc _SQLBindParameter;
	SQLConnectFunc _SQLConnect;
	SQLDescribeColFunc _SQLDescribeCol;
	SQLDisconnectFunc _SQLDisconnect;
	SQLDriverConnectFunc _SQLDriverConnect;
	SQLEndTranFunc _SQLEndTran;
	SQLExecDirectFunc _SQLExecDirect;
	SQLExecuteFunc _SQLExecute;
	SQLFetchFunc _SQLFetch;
	SQLFreeHandleFunc _SQLFreeHandle;
	SQLGetDiagRecFunc _SQLGetDiagRec;
	SQLNumResultColsFunc _SQLNumResultCols;
	SQLPrepareFunc _SQLPrepare;
	SQLSetEnvAttrFunc _SQLSetEnvAttr;
	SQLSetConnectAttrFunc _SQLSetConnectAttr;
	SQLSetStmtAttrFunc _SQLSetStmtAttr;

public:
	SqlDb2Api();
	~SqlDb2Api();

	// Initialize API
	virtual int Init();

	// Set additional information about the driver type
	virtual void SetDriverType(const char * /*info*/) {}

	// Set the connection string in the API object
	virtual void SetConnectionString(const char *conn);

	// Connect to the database
	virtual int Connect(size_t *time_spent);

	// Disconnect from the database
	virtual void Disconnect();
	// Deallocate the driver
	virtual void Deallocate();

	// Get row count for the specified object
	virtual int GetRowCount(const char *object, int *count, size_t *time_spent);

	// Execute the statement and get scalar result
	virtual int ExecuteScalar(const char *query, int *result, size_t *time_spent);

	// Execute the statement
	virtual int ExecuteNonQuery(const char *query, size_t *time_spent);

	// Open cursor and allocate buffers
	virtual int OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query = false,
        std::list<SqlDataTypeMap> *dtmap = NULL);

	// Fetch next portion of data to allocate buffers
	virtual int Fetch(int *rows_fetched, size_t *time_spent);

	// Close the cursor and deallocate buffers
	virtual int CloseCursor();

	// Initialize the bulk copy from one database into another
	virtual int InitBulkTransfer(const char *table, size_t col_count, size_t allocated_array_rows, SqlCol *s_cols, SqlCol **t_cols);

	// Transfer rows between databases
	virtual int TransferRows(SqlCol *s_cols, int rows_fetched, int *rows_written, size_t *bytes_written,
		size_t *time_spent);

	// Specifies whether API allows to parallel reading from this API and write to another API
	virtual bool CanParallelReadWrite();

	// Complete bulk transfer
	virtual int CloseBulkTransfer();

	// Drop the table
	virtual int DropTable(const char* table, size_t *time_spent, std::string &drop_stmt);
	// Remove foreign key constraints referencing to the parent table
	virtual int DropReferences(const char* table, size_t *time_spent);

	// Get the length of LOB column in the open cursor
	virtual int GetLobLength(size_t row, size_t column, size_t *length);
	// Get LOB content
	virtual int GetLobContent(size_t row, size_t column, void *data, size_t length, int *len_ind);
	// Get partial LOB content
	virtual int GetLobPart(size_t row, size_t column, void *data, size_t length, int *len_ind);
	
	// Get the list of available tables
	virtual int GetAvailableTables(std::string &select, std::string &exclude, std::list<std::string> &tables);

	// Read schema information
	virtual int ReadSchema(const char *select, const char *exclude, bool read_cns = true, bool read_idx = true);

	// Read specific catalog information
	virtual int ReadConstraintTable(const char *schema, const char *constraint, std::string &table);
	virtual int ReadConstraintColumns(const char *schema, const char *table, const char *constraint, std::string &cols);

	// Get a list of columns for specified primary or unique key
	virtual int GetKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &output, std::list<std::string> * = NULL);

	// Build a condition to select objects from the catalog
	virtual void GetCatalogSelectionCriteria(std::string &selection_template, std::string &output);

	// Set version of the connected database
	void SetVersion();

	// Get database subtype
	virtual short GetType() { return SQLDATA_DB2; }
	virtual short GetSubType() { return -1; }

private:
	// Metadata reading functions
	int ReadTableConstraints(std::string &condition);
	int ReadConstraintColumns(std::string &selection);

	// Find DB2 installation paths
	void FindDb2Paths(std::list<std::string> &paths);

	// Write LOB data using BCP API
	int WriteLob(SqlCol *s_cols, int row, int *lob_bytes);

	// Set error code and message for the last API call
	void SetError(SQLSMALLINT handle_type, SQLHANDLE handle);

	// Get length or NULL indicator
	SQLLEN GetLen(SQLLEN *ind, int offset);
	// Check indicator for NULL (there are issues with DB2 64-bit driver)
	bool IsIndNull(SQLLEN ind);
};

#endif // sqlines_sqldb2api_h