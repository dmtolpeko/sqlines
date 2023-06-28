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

#ifndef sqlines_sqlsncapi_h
#define sqlines_sqlsncapi_h

#define _SQLNCLI_ODBC_

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h> 
#include <sqlncli.h>
#include <oci.h>
#include <ctpublic.h>
#include "sqlapibase.h"
#include "sqldb.h"

// BCP functions require ODBC Manager handles to use, not SQL Server Native Client handles
// Underlying SQL Server Native Client functions can be successfully used to query and retrieve data except BCP extension

#define SQLSRV_ODBC_DLL			"odbc32.dll"

#define SQLSERV_DLL             "libsqlncli.so"

#define SQLSRV_DLL_LOAD_ERROR	"Loading SQL Server library:"

// ODBC API Functions
typedef SQLRETURN (SQL_API *SQLAllocHandleFunc)(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE *OutputHandle);
typedef SQLRETURN (SQL_API *SQLBindColFunc)(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN *StrLen_or_Ind);
typedef SQLRETURN (SQL_API *SQLDescribeColFunc)(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLCHAR *ColumnName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLength, SQLSMALLINT *DataType, SQLULEN *ColumnSize, SQLSMALLINT *DecimalDigits, SQLSMALLINT *Nullable);
typedef SQLRETURN (SQL_API *SQLDisconnectFunc)(SQLHDBC ConnectionHandle);
typedef SQLRETURN (SQL_API *SQLDriverConnectFunc)(SQLHDBC hdbc, SQLHWND hwnd, SQLCHAR* szConnStrIn, SQLSMALLINT cchConnStrIn, SQLCHAR* szConnStrOut, SQLSMALLINT cchConnStrOutMax, SQLSMALLINT* pcchConnStrOut, SQLUSMALLINT fDriverCompletion);
typedef SQLRETURN (SQL_API *SQLExecDirectFunc)(SQLHSTMT StatementHandle, SQLCHAR* StatementText, SQLINTEGER TextLength);
typedef SQLRETURN (SQL_API *SQLFetchFunc)(SQLHSTMT StatementHandle);
typedef SQLRETURN (SQL_API *SQLFreeHandleFunc)(SQLSMALLINT HandleType, SQLHANDLE Handle);
typedef SQLRETURN (SQL_API *SQLGetDataFunc)(SQLHSTMT StatementHandle, SQLUSMALLINT Col_or_Param_Num, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr);
typedef SQLRETURN (SQL_API *SQLGetDiagRecFunc)(SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT RecNumber, SQLCHAR *Sqlstate, SQLINTEGER *NativeError, SQLCHAR* MessageText, SQLSMALLINT BufferLength, SQLSMALLINT *TextLength);
typedef SQLRETURN (SQL_API *SQLNumResultColsFunc)(SQLHSTMT StatementHandle, SQLSMALLINT *ColumnCount);
typedef SQLRETURN (SQL_API *SQLSetConnectAttrFunc)(SQLHDBC ConnectionHandle, SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength);
typedef SQLRETURN (SQL_API *SQLSetEnvAttrFunc)(SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength);
typedef SQLRETURN (SQL_API *SQLSetStmtAttrFunc)(SQLHSTMT StatementHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength);

// Bulk copy API functions
typedef DBINT (SQL_API *bcp_batchFunc)(HDBC);
typedef RETCODE (SQL_API *bcp_bindFunc)(HDBC, LPCBYTE, INT, DBINT, LPCBYTE, INT, INT, INT);
typedef RETCODE (SQL_API *bcp_controlFunc)(HDBC, INT, void*);
typedef DBINT (SQL_API *bcp_doneFunc)(HDBC);
typedef RETCODE (SQL_API *bcp_initFunc)(HDBC, const char*, const char*, const char*, INT);
typedef RETCODE (SQL_API *bcp_moretextFunc)(HDBC, DBINT, LPCBYTE);
typedef RETCODE (SQL_API *bcp_sendrowFunc)(HDBC);

class SqlSncApi : public SqlApiBase
{
	// Environment and connection handles
	SQLHENV _henv;
	SQLHDBC _hdbc;
	SQLHANDLE _hstmt_cursor;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _server;
	std::string _port;
	std::string _db;
	bool _trusted;

	std::string _driver;

	// Attribute to store last number of fetched rows (SQL_ATTR_ROWS_FETCHED_PTR)
	int _cursor_fetched;

	// Number of columns in bulk copy operation
	size_t _bcp_cols_count;
	// Column describtions and data for bulk copy
	SqlCol *_bcp_cols;
	// Exist LOB columns to be sent using bcp_moretext
	bool _bcp_lob_exists;

	// Codepage to use for BCP
	int _bcp_codepage;

	// SQL Server Native Client DDL (for Bulk Copy API)
#if defined(WIN32) || defined(_WIN64)
	HMODULE _dll;
	HMODULE _dll_odbc;
#else
	void* _dll;
	void* _dll_odbc;
#endif

	// ODBC API functions
	SQLAllocHandleFunc _SQLAllocHandle;
	SQLBindColFunc _SQLBindCol;
	SQLDescribeColFunc _SQLDescribeCol;
	SQLDisconnectFunc _SQLDisconnect;
	SQLDriverConnectFunc _SQLDriverConnect;
	SQLExecDirectFunc _SQLExecDirect;
	SQLFetchFunc _SQLFetch;
	SQLFreeHandleFunc _SQLFreeHandle;
	SQLGetDataFunc _SQLGetData;
	SQLGetDiagRecFunc _SQLGetDiagRec;
	SQLNumResultColsFunc _SQLNumResultCols;
	SQLSetConnectAttrFunc _SQLSetConnectAttr;
	SQLSetEnvAttrFunc _SQLSetEnvAttr;
	SQLSetStmtAttrFunc _SQLSetStmtAttr;

	// Bulk copy API functions
	bcp_batchFunc _bcp_batch;
	bcp_bindFunc _bcp_bind;
	bcp_controlFunc _bcp_control;
	bcp_doneFunc _bcp_done;
	bcp_initFunc _bcp_init;
	bcp_moretextFunc _bcp_moretext;
	bcp_sendrowFunc _bcp_sendrow;

public:
	SqlSncApi();
	~SqlSncApi();

	// Initialize API
	virtual int Init();

    // Initialize session by setting general options
    void InitSession() { SqlApiBase::InitSession(); }

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
	virtual bool CanParallelReadWrite() { return true; }

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

    virtual int GetIndexColumns(SqlIndexes & /*idx*/, std::list<std::string> & /*idx_cols*/, std::list<std::string> & /*idx_sorts*/);

	// Build a condition to select objects from the catalog
	virtual void GetCatalogSelectionCriteria(std::string &selection_template, std::string &output);

	// Get database type and subtype
	virtual short GetType() { return SQLDATA_SQL_SERVER; }
	virtual short GetSubType() { return 0; }

	// Check whether identifier must be quoted
	bool IsQuoteRequired(const char *name);
    bool IsQuoteRequired(std::string &name) { return IsQuoteRequired(name.c_str()); }

    // Quote name with [] if required
    std::string QuoteName(const char *name, size_t len); 

private:
	// Define the reserved words
	int ReadReservedWords();

	// Set version of the connected database
	void SetVersion();

    // Metadata reading functions
	int ReadIndexes(const char *select);
    int ReadIndexColumns(const char *select);

	// Define which SQL Server Native Client driver and DLL to use
	void DefineDriver(std::string &driver, std::string &dll);

	// Write LOB data using BCP API
	int WriteLob(SqlCol *s_cols, size_t row, size_t *lob_bytes);

	// Set error code and message for the last API call
	void SetError(SQLSMALLINT handle_type, SQLHANDLE handle);
};

#endif // sqlines_sqlsncapi
