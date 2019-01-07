
// SqlIfmxApi Informix ODBC API
// Copyright (c) 2012 SQLines. All rights reserved

#ifndef sqlines_sqlifmxapi_h
#define sqlines_sqlifmxapi_h

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h> 
#include "sqlapibase.h"
#include "sqldb.h"

// Informix ODBC DLL (looks like this name is stable and even mentioned in 2003 documents)
#if defined(WIN32) || defined(_WIN64)
#define IFMX_DLL				"iclit09b.dll"
#else
#define IFMX_DLL				"iclit09b.so"
#endif

#define IFMX_DLL_LOAD_ERROR	"Loading Informix library " IFMX_DLL ": " 

// ODBC API Functions
typedef SQLRETURN (SQL_API *SQLAllocHandleFunc)(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE *OutputHandle);
typedef SQLRETURN (SQL_API *SQLBindColFunc)(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN *StrLen_or_Ind);
typedef SQLRETURN (SQL_API *SQLDescribeColFunc)(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLCHAR *ColumnName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLength, SQLSMALLINT *DataType, SQLULEN *ColumnSize, SQLSMALLINT *DecimalDigits, SQLSMALLINT *Nullable);
typedef SQLRETURN (SQL_API *SQLDisconnectFunc)(SQLHDBC ConnectionHandle);
typedef SQLRETURN (SQL_API *SQLDriverConnectFunc)(SQLHDBC hdbc, SQLHWND hwnd, SQLCHAR *szConnStrIn, SQLSMALLINT cchConnStrIn, SQLCHAR *szConnStrOut, SQLSMALLINT cchConnStrOutMax, SQLSMALLINT *pcchConnStrOut, SQLUSMALLINT fDriverCompletion);
typedef SQLRETURN (SQL_API *SQLExecDirectFunc)(SQLHSTMT StatementHandle, SQLCHAR* StatementText, SQLINTEGER TextLength);
typedef SQLRETURN (SQL_API *SQLFetchFunc)(SQLHSTMT StatementHandle);
typedef SQLRETURN (SQL_API *SQLFreeHandleFunc)(SQLSMALLINT HandleType, SQLHANDLE Handle);
typedef SQLRETURN (SQL_API *SQLGetDataFunc)(SQLHSTMT StatementHandle, SQLUSMALLINT Col_or_Param_Num, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr);
typedef SQLRETURN (SQL_API *SQLGetDiagRecFunc)(SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT RecNumber, SQLCHAR *Sqlstate, SQLINTEGER *NativeError, SQLCHAR* MessageText, SQLSMALLINT BufferLength, SQLSMALLINT *TextLength);
typedef SQLRETURN (SQL_API *SQLNumResultColsFunc)(SQLHSTMT StatementHandle, SQLSMALLINT *ColumnCount);
typedef SQLRETURN (SQL_API *SQLSetConnectAttrFunc)(SQLHDBC ConnectionHandle, SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength);
typedef SQLRETURN (SQL_API *SQLSetEnvAttrFunc)(SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength);
typedef SQLRETURN (SQL_API *SQLSetStmtAttrFunc)(SQLHSTMT StatementHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength);

class SqlIfmxApi : public SqlApiBase
{
	// Environment and connection handles
	SQLHENV _henv;
	SQLHDBC _hdbc;
	SQLHANDLE _hstmt_cursor;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _server;
	std::string _db;

	// Attribute to store last number of fetched rows (SQL_ATTR_ROWS_FETCHED_PTR)
	int _cursor_fetched;

	// Informix ODBC DDL
#if defined(WIN32) || defined(_WIN64)
	HMODULE _dll;
#else
	void* _dll;
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

public:
	SqlIfmxApi();
	~SqlIfmxApi();

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
	virtual int GetRowCount(const char *object, long *count, size_t *time_spent);

	// Execute the statement and get scalar result
	virtual int ExecuteScalar(const char *query, long *result, size_t *time_spent);

	// Execute the statement
	virtual int ExecuteNonQuery(const char *query, size_t *time_spent);

	// Open cursor and allocate buffers
	virtual int OpenCursor(const char *query, long buffer_rows, long buffer_memory, long *col_count, long *allocated_array_rows, 
		long *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query = false,
        std::list<SqlDataTypeMap> *dtmap = NULL);

	// Fetch next portion of data to allocate buffers
	virtual int Fetch(long *rows_fetched, size_t *time_spent);

	// Close the cursor and deallocate buffers
	virtual int CloseCursor();

	// Initialize the bulk copy from one database into another
	virtual int InitBulkTransfer(const char *table, long col_count, long allocated_array_rows, SqlCol *s_cols, SqlCol **t_cols);

	// Transfer rows between databases
	virtual int TransferRows(SqlCol *s_cols, long rows_fetched, long *rows_written, size_t *bytes_written,
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
	virtual int GetLobLength(long row, long column, long *length);
	// Get LOB content
	virtual int GetLobContent(long row, long column, void *data, long length, long *len_ind);
	// Get partial LOB content
	virtual int GetLobPart(long row, long column, void *data, long length, long *len_ind);
	
	// Get the list of available tables
	virtual int GetAvailableTables(std::string &select, std::string &exclude, std::list<std::string> &tables);

	// Read schema information
	virtual int ReadSchema(const char *select, const char *exclude, bool read_cns = true, bool read_idx = true);

	// Read specific catalog information
	virtual int ReadConstraintTable(const char *schema, const char *constraint, std::string &table);
	virtual int ReadConstraintColumns(const char *schema, const char *table, const char *constraint, std::string &cols);

	// Get a list of columns for specified primary or unique key
	virtual int GetKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &output, std::list<std::string> * = NULL);
	// Get a list of columns for specified foreign key
	virtual int GetForeignKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &fcols, std::list<std::string> &pcols, std::string &ptable); 
	
	// Get a list of columns for specified index
	virtual int GetIndexColumns(SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts);

    // Get data type name by code
    void GetDataTypeName(SQLSMALLINT native_dt, std::string &name);
        
	// Get database subtype
	virtual short GetType() { return SQLDATA_INFORMIX; }
	virtual short GetSubType() { return -1; }

private:
	int InitSession();

	// Metadata reading functions
	int ReadTableColumns(std::string &condition);
	int ReadIdentityColumns(std::string &condition);
	int ReadDefaults(std::string &condition);
	int ReadTableConstraints(std::string &condition);
	int ReadCheckConstraints(std::string &condition);
	int ReadReferences(std::string &condition);
	int ReadIndexes(std::string &condition);

	void SetCheckConstraint(int cnsid, std::string &condition);

	// Filter system tables
	void AddSystemTablesFilter(const char *alias, std::string &query);

	// Set column default value when reading the catalog
	void SetDefault(SqlColMeta &col, char type, char *value, long len);

	// Set version of the connected database
	void SetVersion();

	// Find Informix installation paths
	void FindInformixPaths(std::list<std::string> &paths);

	// Write LOB data using BCP API
	int WriteLob(SqlCol *s_cols, int row, int *lob_bytes);

	// Set error code and message for the last API call
	void SetError(SQLSMALLINT handle_type, SQLHANDLE handle);
};

#endif // sqlines_sqlifmxapi_h
