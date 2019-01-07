/** 
 * Copyright (c) 2017 SQLines
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

// SqlPgApi PostgreSQL libpq C Library

#ifndef sqlines_sqlpgapi_h
#define sqlines_sqlpgapi_h

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <libpq-fe.h>
#include <oci.h>
#include <string>
#include "sqlapibase.h"
#include "sqldb.h"

#if defined(WIN32) || defined(_WIN64)
#define LIBPQ_C_DLL				"libpq.dll"
#else
#define LIBPQ_C_DLL				"libpq.so.5"
#endif

#define LIBPQ_DLL_LOAD_ERROR	"Error loading PostgreSQL " LIBPQ_C_DLL ": " 

#define LIBPQ_COPY_DATA_BUFFER_LEN			(1024*1024)

// libpq C library functions
typedef void (*PQclearFunc)(PGresult *res);
typedef char* (*PQerrorMessageFunc)(const PGconn *conn);
typedef PGresult* (*PQexecFunc)(PGconn *conn, const char *query);
typedef int (*PQfmodFunc)(const PGresult *res, int column_number);
typedef char* (*PQfnameFunc)(const PGresult *res, int column_number);
typedef int (*PQfsizeFunc)(const PGresult *res, int column_number);
typedef Oid (*PQftypeFunc)(const PGresult *res, int column_number);
typedef int (*PQgetisnullFunc)(const PGresult *res, int row_number, int column_number);
typedef int (*PQgetlengthFunc)(const PGresult *res, int row_number, int column_number);
typedef PGresult* (*PQgetResultFunc)(PGconn *conn);
typedef char* (*PQgetvalueFunc)(const PGresult *res, int row_number, int column_number);
typedef int (*PQnfieldsFunc)(const PGresult *res);
typedef int (*PQntuplesFunc)(const PGresult *res);
typedef int (*PQputCopyDataFunc)(PGconn *conn, const char *buffer, int nbytes);
typedef int (*PQputCopyEndFunc)(PGconn *conn, const char *errormsg);
typedef char* (*PQresultErrorMessageFunc)(const PGresult *res);
typedef ExecStatusType (*PQresultStatusFunc)(const PGresult *res);
typedef int	(*PQsetClientEncodingFunc)(PGconn *conn, const char *encoding);
typedef PGconn* (*PQsetdbLoginFunc)(const char *pghost, const char *pgport, const char *pgoptions, const char *pgtty, const char *dbName, const char *login, const char *pwd);
typedef ConnStatusType (*PQstatusFunc)(const PGconn *conn);

// PostgreSQL built-in data types (SELECT oid, typname FROM pg_type system table)
#define PG_DT_VARCHAR		1043	
#define PG_DT_BPCHAR		1042    // Blank padded CHAR
#define PG_DT_INT2			21		// SMALLINT	
#define PG_DT_INT4			23
#define PG_DT_NUMERIC		1700	// NUMERIC and DECIMAL
#define PG_DT_DATE			1082
#define PG_DT_FLOAT			701

class SqlPgApi : public SqlApiBase
{
	// PostgreSQL connection
	PGconn *_conn;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _server;
	std::string _port;
	std::string _db;

	// Open cursor result
	PGresult *_cursor_result;
	// Number of rows in the result set
	int _cursor_rows_count;
	// Number of rows already fetched to buffers
	int _cursor_rows_fetched;

	// Number of columns for COPY command
	long _copy_cols_count;
	// Buffer for COPY data
	char *_copy_data;

	// PostgreSQL libpq C library DDL
#if defined(WIN32) || defined(_WIN64)
	HMODULE _dll;
#else
	void* _dll;
#endif

	// libpq C library functions
	PQclearFunc _PQclear;
	PQerrorMessageFunc _PQerrorMessage;
	PQexecFunc _PQexec;
	PQfmodFunc _PQfmod;
	PQfnameFunc _PQfname;
	PQfsizeFunc _PQfsize;
	PQftypeFunc _PQftype;
	PQgetisnullFunc _PQgetisnull;
	PQgetlengthFunc _PQgetlength;
	PQgetResultFunc _PQgetResult;
	PQgetvalueFunc _PQgetvalue;
	PQnfieldsFunc _PQnfields;
	PQntuplesFunc _PQntuples;
	PQputCopyDataFunc _PQputCopyData;
	PQputCopyEndFunc _PQputCopyEnd;
	PQresultErrorMessageFunc _PQresultErrorMessage;
	PQresultStatusFunc _PQresultStatus;
	PQsetClientEncodingFunc _PQsetClientEncoding;
	PQsetdbLoginFunc _PQsetdbLogin;
	PQstatusFunc _PQstatus;
	
public:
	SqlPgApi();
	~SqlPgApi();

	// Initialize API
	virtual int Init();

	// Set additional information about the driver type
	virtual void SetDriverType(const char * /*info*/) {}

	// Set the connection string in the API object
	virtual void SetConnectionString(const char *conn);

	// Connect to the database
	virtual int Connect(size_t *time_spent);

	// Set version of the connected database
	void SetVersion();
	// Initialize session
	void InitSession();

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
	virtual bool CanParallelReadWrite() { return true; }

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
	virtual int GetLobPart(long /*row*/, long /*column*/, void * /*data*/, long /*length*/, long * /*len_ind*/) { return -1; }
	
	// Get the list of available tables
	virtual int GetAvailableTables(std::string &select, std::string &exclude, std::list<std::string> &tables);

	// Read schema information
	virtual int ReadSchema(const char *select, const char *exclude, bool read_cns = true, bool read_idx = true);

	// Read specific catalog information
	virtual int ReadConstraintTable(const char *schema, const char *constraint, std::string &table);
	virtual int ReadConstraintColumns(const char *schema, const char *table, const char *constraint, std::string &cols);

	// Build a condition to select objects from the catalog
	virtual void GetCatalogSelectionCriteria(std::string &selection_template, std::string &output);

	// Get fatures supported by the provider
	virtual bool IsIntegerBulkBindSupported() { return false; }
	virtual bool IsSmallintBulkBindSupported() { return false; }

	// Get database type and subtype
	virtual short GetType() { return SQLDATA_POSTGRESQL; }
	virtual short GetSubType() { return 0; }

private:
	// Free connection and environment handles 
	void Deallocate();

	// Find PostgreSQL installation paths
	void FindPgPaths(std::list<std::string> &paths);

	// Write LOB data using BCP API
	int WriteLob(SqlCol *s_cols, int row, int *lob_bytes);

	// Set error code and message for the last API call
	void SetError();
};

#endif // sqlines_sqlpgapi_h
