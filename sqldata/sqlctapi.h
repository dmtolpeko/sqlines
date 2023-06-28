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

// SqlCtApi Sybase CTLIB Library

#ifndef sqlines_sqlctapi_h
#define sqlines_sqlctapi_h

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <string>
#include <ctpublic.h>
#include "sqlapibase.h"
#include "sqldb.h"

// Sybase ASE 16 and 15 DLLs
#if defined(_WIN64)
#define CTLIB_DLL				"libsybct64.dll"
#define CSLIB_DLL				"libsybcs64.dll"
#elif defined(WIN32)
#define CTLIB_DLL				"libsybct.dll"
#define CSLIB_DLL				"libsybcs.dll"
#else
#define CTLIB_DLL				"libsybct64.so"
#define CSLIB_DLL				"libsybcs64.so"
#endif

// Sybase ASE 12.5 DLLs
#define CTLIB_DLL2				"libct.dll"
#define CSLIB_DLL2				"libcs.dll"

#define CTLIB_DLL_LOAD_ERROR	"Error loading Sybase Client Library: " 

// libpq C library functions
typedef CS_RETCODE (CS_PUBLIC *cs_ctx_allocFunc)(CS_INT version, CS_CONTEXT **outptr);
typedef CS_RETCODE (CS_PUBLIC *cs_configFunc)(CS_CONTEXT *context, CS_INT action, CS_INT property, CS_VOID *buf, CS_INT buflen, CS_INT *outlen);
typedef CS_RETCODE (CS_PUBLIC *cs_ctx_dropFunc)(CS_CONTEXT *context);
typedef CS_RETCODE (CS_PUBLIC *cs_dt_infoFunc)(	CS_CONTEXT *context, CS_INT action, CS_LOCALE *locale, CS_INT type, CS_INT item, CS_VOID *buffer, CS_INT buflen, CS_INT *outlen);
typedef CS_RETCODE (CS_PUBLIC *cs_loc_allocFunc)(CS_CONTEXT *context, CS_LOCALE **loc_pointer);
typedef CS_RETCODE (CS_PUBLIC *cs_localeFunc)(CS_CONTEXT *context, CS_INT action, CS_LOCALE *locale, CS_INT type, CS_CHAR *buffer, CS_INT buflen, CS_INT *outlen);
typedef CS_RETCODE (CS_PUBLIC *ct_bindFunc)(CS_COMMAND *cmd, CS_INT item, CS_DATAFMT *datafmt, CS_VOID *buf, CS_INT *outputlen, CS_SMALLINT *indicator);
typedef CS_RETCODE (CS_PUBLIC *ct_cancelFunc)(CS_CONNECTION *connection, CS_COMMAND *cmd, CS_INT type);
typedef CS_RETCODE (CS_PUBLIC *ct_commandFunc)(CS_COMMAND *cmd, CS_INT type, CS_CHAR *buf, CS_INT buflen, CS_INT option);
typedef CS_RETCODE (CS_PUBLIC *ct_con_allocFunc)(CS_CONTEXT *context, CS_CONNECTION **connection);
typedef CS_RETCODE (CS_PUBLIC *ct_con_dropFunc)(CS_CONNECTION *connection);
typedef CS_RETCODE (CS_PUBLIC *ct_con_propsFunc)(CS_CONNECTION *connection, CS_INT action, CS_INT prop, CS_VOID *buf, CS_INT buflen, CS_INT *outlen);
typedef CS_RETCODE (CS_PUBLIC *ct_connectFunc)(CS_CONNECTION *connection, CS_CHAR *server_name, CS_INT snamelen);
typedef CS_RETCODE (CS_PUBLIC *ct_closeFunc)(CS_CONNECTION *connection, CS_INT option);
typedef CS_RETCODE (CS_PUBLIC *ct_cmd_allocFunc)(CS_CONNECTION *connection,	CS_COMMAND **cmdptr);
typedef CS_RETCODE (CS_PUBLIC *ct_cmd_dropFunc)(CS_COMMAND *cmd);
typedef CS_RETCODE (CS_PUBLIC *ct_describeFunc)(CS_COMMAND *cmd, CS_INT item, CS_DATAFMT *datafmt);
typedef CS_RETCODE (CS_PUBLIC *ct_diagFunc)(CS_CONNECTION *connection, CS_INT operation, CS_INT type, CS_INT idx, CS_VOID *buffer);
typedef CS_RETCODE (CS_PUBLIC *ct_exitFunc)(CS_CONTEXT *context, CS_INT option);
typedef CS_RETCODE (CS_PUBLIC *ct_fetchFunc)(CS_COMMAND *cmd, CS_INT type, CS_INT offset, CS_INT option, CS_INT *count);
typedef CS_RETCODE (CS_PUBLIC *ct_initFunc)(CS_CONTEXT *context, CS_INT version);
typedef CS_RETCODE (CS_PUBLIC *ct_res_infoFunc)(CS_COMMAND *cmd, CS_INT operation, CS_VOID *buf, CS_INT buflen, CS_INT *outlen);
typedef CS_RETCODE (CS_PUBLIC *ct_resultsFunc)(CS_COMMAND *cmd, CS_INT *result_type);
typedef CS_RETCODE (CS_PUBLIC *ct_sendFunc)(CS_COMMAND *cmd);

class SqlCtApi : public SqlApiBase
{
	// Sybase environment and connection
	CS_CONTEXT *_context;
	CS_CONNECTION *_connection;
	CS_LOCALE *_locale;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _server;
	std::string _port;
	std::string _db;

	// Open cursor command
	CS_COMMAND *_cursor_cmd;
	int _cursor_last_fetch_rc;

	// Sybase CT-Lib library DDL
#if defined(WIN32) || defined(_WIN64)
	HMODULE _ct_dll;
	HMODULE _cs_dll;
#else
	void* _ct_dll;
	void* _cs_dll;
#endif

	// Client library functions
	cs_ctx_allocFunc _cs_ctx_alloc;
	cs_configFunc _cs_config;
	cs_ctx_dropFunc _cs_ctx_drop;
	cs_dt_infoFunc _cs_dt_info;
	cs_loc_allocFunc _cs_loc_alloc;
	cs_localeFunc _cs_locale;
	ct_bindFunc _ct_bind;
	ct_cancelFunc _ct_cancel;
	ct_commandFunc _ct_command;
	ct_con_allocFunc _ct_con_alloc;
	ct_con_dropFunc _ct_con_drop;
	ct_con_propsFunc _ct_con_props;
	ct_connectFunc _ct_connect;
	ct_closeFunc _ct_close;
	ct_cmd_allocFunc _ct_cmd_alloc;
	ct_cmd_dropFunc _ct_cmd_drop;
	ct_describeFunc _ct_describe;
	ct_diagFunc _ct_diag;
	ct_exitFunc _ct_exit;
	ct_fetchFunc _ct_fetch;
	ct_initFunc _ct_init;
	ct_res_infoFunc _ct_res_info;
	ct_resultsFunc _ct_results;
	ct_sendFunc _ct_send;
	
public:
	SqlCtApi();
	~SqlCtApi();

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
	virtual int GetLobPart(size_t /*row*/, size_t /*column*/, void * /*data*/, size_t /*length*/, int * /*len_ind*/) { return -1; }
	
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
	
	// Build a condition to select objects from the catalog
	virtual void GetCatalogSelectionCriteria(std::string &selection_template, std::string &output);

	// Set version of the connected database
	void SetVersion();

	// Get database type and subtype
	virtual short GetType() { return SQLDATA_SYBASE; }
	virtual short GetSubType() { return 0; }

private:
	// Metadata reading functions
	int ReadTableColumns(std::string &condition);
	int ReadIndexes(std::string &condition);
	int ReadReferences(std::string &condition);

	// Find Sybase installation paths
	void FindSybasePaths(std::list<std::string> &paths);

	// Write LOB data using BCP API
	int WriteLob(SqlCol *s_cols, int row, int *lob_bytes);

	// Get length or NULL
	SQLLEN GetLen(SqlCol *col, int offset);

	// Set error code and message for the last API call
	void SetError();
};

#endif // sqlines_sqlctapi_h