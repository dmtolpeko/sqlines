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

// SqlMyqlApi MySQL C API

#ifndef sqlines_sqlmysqlapi_h
#define sqlines_sqlmysqlapi_h

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <mysql.h>
#include <oci.h>
#include <string>
#include "sqlapibase.h"
#include "sqldb.h"

#if defined(WIN32) || defined(_WIN64)
#define MYSQL_C_DLL						"libmysql.dll"
#define MARIADB_C_DLL					"libmariadb.dll"
#else
#define MYSQL_C_DLL						"libmysqlclient.so"
#define MARIADB_C_DLL					"libmariadb.so"
#endif

#define MARIADB_DLL_LOAD_ERROR			"Loading MariaDB C API " MARIADB_C_DLL ": " 
#define MARIADB_64BIT_DLL_LOAD_ERROR	"Loading MariaDB C API " MARIADB_C_DLL ": 64-bit library loaded, use 32-bit client or 64-bit version of SQLines Data"

#define MYSQL_DLL_LOAD_ERROR			"Loading MySQL C API " MYSQL_C_DLL ": " 
#define MYSQL_64BIT_DLL_LOAD_ERROR		"Loading MySQL C API " MYSQL_C_DLL ": 64-bit library loaded, use 32-bit client or 64-bit version of SQLines Data"

#define MYSQL_C_FUNC_LOAD_ERROR			"Library " MYSQL_C_DLL " loaded, but API functions cannot be found"
#define MARIADB_C_FUNC_LOAD_ERROR		"Library " MARIADB_C_DLL " loaded, but API functions cannot be found"

// MySQL C API functions
typedef MYSQL* (STDCALL *mysql_initFunc)(MYSQL *mysql);
typedef void (STDCALL *mysql_closeFunc)(MYSQL *sock);
typedef unsigned int (STDCALL *mysql_errnoFunc)(MYSQL *mysql);
typedef const char* (STDCALL *mysql_errorFunc)(MYSQL *mysql);
typedef MYSQL_FIELD* (STDCALL *mysql_fetch_fieldsFunc)(MYSQL_RES *res);
typedef MYSQL_ROW (STDCALL *mysql_fetch_rowFunc)(MYSQL_RES *result);
typedef unsigned long* (STDCALL *mysql_fetch_lengthsFunc)(MYSQL_RES *result);
typedef void (STDCALL *mysql_free_resultFunc)(MYSQL_RES *result);
typedef unsigned int (STDCALL *mysql_num_fieldsFunc)(MYSQL_RES *res);
typedef MYSQL* (STDCALL *mysql_real_connectFunc)(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);
typedef int (STDCALL *mysql_queryFunc)(MYSQL *mysql, const char *stmt_str);
typedef int (STDCALL *mysql_optionsFunc)(MYSQL *mysql, enum mysql_option option, const void *arg);
typedef int (STDCALL *mysql_set_character_setFunc)(MYSQL *mysql, const char *csname);
typedef void (*mysql_set_local_infile_handlerFunc)(MYSQL *mysql, int (*local_infile_init)(void **, const char *, void *), int (*local_infile_read)(void *, char *, unsigned int), void (*local_infile_end)(void *), int (*local_infile_error)(void *, char*, unsigned int), void *userdata);
typedef my_bool (STDCALL *mysql_thread_initFunc)();
typedef MYSQL_RES* (STDCALL *mysql_use_resultFunc)(MYSQL *mysql);

class SqlMysqlApi : public SqlApiBase
{
	// MySQL environment handle
	MYSQL _mysql;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _server;
	int _port;
	std::string _db;

	MYSQL_RES *_cursor_result;

	// Return code of blocking LOAD DATA INFILE command 
	int _ldi_rc;
	// Flag whether LOAD DATA INFILE command terminated
	int _ldi_terminated;
	// Number of columns for LOAD DATA INFILE command
	size_t _ldi_cols_count;
	// Number of rows in the current portion of data
	int _ldi_rows_count;
	// Column describtions and data for bulk copy
	SqlCol *_ldi_cols;
	// Current processed row, column and item in the portion of data
	size_t _ldi_current_row;
	size_t _ldi_current_col;
	int _ldi_current_col_len;
	bool _ldi_write_newline;
	// Buffer for the current LOB value
	char *_ldi_lob_data;
	size_t _ldi_lob_size;

	// Bytes written during last transfer iteration (last batch)
	int _ldi_bytes;
	// Bytes written since the beginning of table load
	size_t _ldi_bytes_all;

	// Current LOAD DATA INFILE command
	std::string _load_command;

#if defined(WIN32) || defined(_WIN64)
	// Event to wait for a new portion of data for LOAD DATA INFILE command
	HANDLE _wait_event;
	// Event to notify that the portion of data processed
	HANDLE _completed_event;
#else
	Event _wait_event;
	Event _completed_event;
#endif

	// MySQL C API DDL
#if defined(WIN32) || defined(_WIN64)
	static HMODULE _dll;
#else
	static void* _dll;
#endif

	// MySQL C API functions
	static mysql_initFunc _mysql_init;
	static mysql_closeFunc _mysql_close;
	static mysql_errnoFunc _mysql_errno;
	static mysql_errorFunc _mysql_error;
	static mysql_fetch_fieldsFunc _mysql_fetch_fields;
	static mysql_fetch_rowFunc _mysql_fetch_row;
	static mysql_fetch_lengthsFunc _mysql_fetch_lengths;
	static mysql_free_resultFunc _mysql_free_result;
	static mysql_num_fieldsFunc _mysql_num_fields;
	static mysql_optionsFunc _mysql_options;
	static mysql_real_connectFunc _mysql_real_connect;
	static mysql_queryFunc _mysql_query;
	static mysql_set_character_setFunc _mysql_set_character_set;
	static mysql_set_local_infile_handlerFunc _mysql_set_local_infile_handler;
	static mysql_thread_initFunc _mysql_thread_init;
	static mysql_use_resultFunc _mysql_use_result;

public:
	SqlMysqlApi();
	~SqlMysqlApi();

	// Initialize API for process
	int InitStatic();
	// Initialize API for thread
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

	// Get fatures supported by the provider
	virtual bool IsIntegerBulkBindSupported() { return false; }
	virtual bool IsSmallintBulkBindSupported() { return false; }
	virtual bool IsTimestampBulkBindSupported() { return false; }

	// Whether target database loads in UTF8
	virtual bool IsTargetUtf8LoadSupported() { return true; }

	// Get database type and subtype
	virtual short GetType() { return SQLDATA_MYSQL; }
	virtual short GetSubType() { return 0; }

	// Execute SHOW WARNINGS and log results
	void ShowWarnings(const char *prefix); 

private:
	// Find MySQL installation paths
	void FindMysqlPaths();

	// Initialize global options that take effect only for new connections
	int InitGlobalOptions();

	// Initialize session by setting options
	int InitSession();

	// Set version of the connected database
	void SetVersion();

	// Write LOB data using BCP API
	int WriteLob(SqlCol *s_cols, int row, int *lob_bytes);

	// LOAD DATA INFILE callbacks
	int local_infile_read(char *buf, unsigned int buf_len);
	void local_infile_end(void *ptr);
	int local_infile_error(void *ptr, char *error_msg, unsigned int error_msg_len);

	static int local_infile_initS(void **ptr, const char *filename, void *userdata);
	static int local_infile_readS(void *ptr, char *buf, unsigned int buf_len);
	static void local_infile_endS(void *ptr);
	static int local_infile_errorS(void *ptr, char *error_msg, unsigned int error_msg_len);

#if defined(WIN32) || defined(_WIN64)
	static unsigned int __stdcall StartLoadDataInfileS(void *object);
#else
	static void* StartLoadDataInfileS(void *object);
#endif

	// Set error code and message for the last API call
	void SetError();
};

#endif // sqlines_sqlmysqlapi_h