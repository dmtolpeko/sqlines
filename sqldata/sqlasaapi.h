
// SqlAsaApi Sybase SQL Anywhere API
// Copyright (c) 2012 SQLines. All rights reserved

#ifndef sqlines_sqlasaapi_h
#define sqlines_sqlasaapi_h

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h> 
#include <oci.h>
#include <ctpublic.h>
#include "sqlapibase.h"
#include "sqldb.h"

class SqlAsaApi : public SqlApiBase
{
	// Environment and connection handles
	SQLHENV _henv;
	SQLHDBC _hdbc;
	SQLHANDLE _hstmt_cursor;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _dsn;

	// Attribute to store last number of fetched rows (SQL_ATTR_ROWS_FETCHED_PTR)
	SQLULEN _cursor_fetched;

	// Extract all character data as 2-byte Unicode (UTF-16/UCS-2)
	bool _char_as_wchar;

public:
	SqlAsaApi();
	~SqlAsaApi();

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
	// Get a list of columns for specified foreign key
	virtual int GetForeignKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &fcols, std::list<std::string> &pcols, std::string &ptable);

	// Get a list of columns for specified index
	virtual int GetIndexColumns(SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts);

	// Get database type and subtype 
	virtual short GetType() { return SQLDATA_ASA; }
	virtual short GetSubType() { return 0; }

    // Get data type name by code
    void GetDataTypeName(SQLSMALLINT native_dt, std::string &name);

	// Check whether identifier must be quoted
	bool IsQuoteRequired(const char *name);

private:
	int InitSession();
	// Set session attributes
	int SetSession();
	// Set version of the connected database
	void SetVersion();

	// Metadata reading functions
	int ReadTableColumns(std::string &condition);
	int ReadTableConstraints(std::string &condition);
	int ReadIndexes(std::string &condition);
	int ReadIndexColumns(std::string &condition);
	int ReadReferences(std::string &condition);
	int ReadForeignKeyColumns(std::string &condition);
	int ReadForeignKeyActions();
	int ReadCheckConstraints(std::string &condition);
	int ReadReservedWords();

	void SetPrimaryKeyColumn(SqlColMeta &col_meta);
	void SetUniqueConstraintColumn(SqlIndColumns &idx_col);
	void SetDefault(SqlColMeta &col_meta, const char *value, int len);
	void SetIdentity(SqlColMeta &col_meta, int id_max);

	// Set error code and message for the last API call
	void SetError(SQLSMALLINT handle_type, SQLHANDLE handle);
};

#endif // sqlines_sqlasaapi
