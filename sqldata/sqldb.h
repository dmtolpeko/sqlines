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

// SqlDb Database Access

#ifndef sqlines_sqldb_h
#define sqlines_sqldb_h

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
#include <pthread.h>
#define __int64 long long
#endif

// ODBC definitions
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include "sqlapibase.h"
#include "../sqlcommon/parameters.h"
#include "../sqlcommon/applog.h"
#include "../sqlcommon/os.h"

// Databases against which execute operation
#define SQLDB_BOTH							1
#define SQLDB_SOURCE_ONLY					2
#define SQLDB_TARGET_ONLY					3

// Database types
#define SQLDATA_SQL_SERVER					1
#define SQLDATA_ORACLE						2
#define SQLDATA_ODBC						3
#define SQLDATA_MYSQL						4
#define SQLDATA_POSTGRESQL					5
#define SQLDATA_SYBASE						6
#define SQLDATA_INFORMIX					7
#define SQLDATA_DB2							8
#define SQLDATA_ASA							9
#define SQLDATA_STDOUT						10

// Database subtypes
#define SQLDATA_SUBTYPE_MARIADB				1

// High level commands to perform
#define SQLDATA_CMD_TRANSFER				0x01
#define SQLDATA_CMD_VALIDATE				0x02
#define SQLDATA_CMD_ASSESS					0x04

// Command/reply type
#define SQLDATA_CMD_CONNECT								1
#define SQLDATA_CMD_STARTED								2
#define SQLDATA_CMD_COMPLETE							3
#define SQLDATA_CMD_COMPLETE_WITH_DDL_ERROR				4
#define SQLDATA_CMD_ALL_COMPLETE						5
#define SQLDATA_CMD_IN_PROGRESS							6
#define SQLDATA_CMD_OPEN_CURSOR							7
#define SQLDATA_CMD_TRUNCATE							8
#define SQLDATA_CMD_DROP								9
#define SQLDATA_CMD_CREATE								10
#define SQLDATA_CMD_VALIDATE_TABLE						11
#define SQLDATA_CMD_ADD_DEFAULT							12
#define SQLDATA_CMD_ADD_CHECK_CONSTRAINT				13
#define SQLDATA_CMD_ADD_COMMENT							14
#define SQLDATA_CMD_ADD_PRIMARY_KEY						15
#define SQLDATA_CMD_ADD_UNIQUE_KEY						16
#define SQLDATA_CMD_ADD_FOREIGN_KEY						17
#define SQLDATA_CMD_CREATE_INDEX						18
#define SQLDATA_CMD_DROP_SEQUENCE						19
#define SQLDATA_CMD_CREATE_SEQUENCE						20
#define SQLDATA_CMD_CREATE_TRIGGER						21
#define SQLDATA_CMD_VALIDATE_ROW_COUNT					22
#define SQLDATA_CMD_FETCH_NEXT							23
#define SQLDATA_CMD_INSERT_NEXT							24
#define SQLDATA_CMD_NUMBER_OF_TABLES					25
#define SQLDATA_CMD_DUMP_DDL							26
#define SQLDATA_CMD_CLOSE_CURSOR						27
#define SQLDATA_CMD_READ_SCHEMA							28
#define SQLDATA_CMD_READ_SCHEMA_FOR_TRANSFER_TO			29	
#define SQLDATA_CMD_DROP_FOREIGN_KEY					30
#define SQLDATA_CMD_SKIPPED								31
#define SQLDATA_CMD_NO_MORE_TASKS						100

// Transfer command options
#define SQLDATA_OPT_NONE					0x00
#define SQLDATA_OPT_CREATE					0x01
#define SQLDATA_OPT_DROP					0x02
#define SQLDATA_OPT_TRUNCATE				0x04

// Validate command options
#define SQLDATA_OPT_ROWCOUNT				0x01
#define SQLDATA_OPT_ROWS					0x02

#define TRACE_DIFF_DMP(data, len) { if(_trace_valdiff_data) File::Append(_valdiff_file.c_str(), data, len); } 

class SqlDb;
struct SqlDataReply;

typedef void (*SqlDataCallbackFunc)(void *, SqlDataReply *);

// Database communication area
struct SqlDbThreadCa
{
	SqlDb *sqlDb;
	SqlApiBase *db_api;

	// Command type
	short _cmd; 
	// Command return code (0 success, -1 error)
	short cmd_rc;

	// Object name (table name i.e) on which the command is executed
	const char *_name;

	// Command in/out data (depends on the command type)
	long _int1;
	long _int2;
	long _int3;
	long _int4;
	void *_void1;
	void *_void2;

	// Time spent on last operation
	size_t _time_spent;

	// Database API error information
	int error;
	char native_error_text[1024];

	// Events to wait for a new task and notify about the completion
#if defined(WIN32) || defined(_WIN64)
	HANDLE _wait_event;
	HANDLE _completed_event; 
#else
	Event _wait_event;
	Event _completed_event; 
#endif

	SqlDbThreadCa()
	{ 
		sqlDb = NULL; db_api = NULL; _cmd = 0; cmd_rc = 0; _name = NULL; 
		_int1 = 0; _int2 = 0; _int3 = 0; _int4 = 0;
		_void1 = NULL; _void2 = NULL;
		_time_spent = 0;
		
		error = 0; 
		*native_error_text = '\x0';
	}
};

// SQLData Command packet
struct SqlDataCommand
{
	// Command to performe
	short _cmd;

	// Source and target object names on which operation was performed
	char _s_name[1024];
	char _t_name[1024];

	SqlDataCommand() 
	{ 
		_cmd = 0; *_s_name = '\x0'; *_t_name = '\x0'; 
	}
};

// SQLData Reply packet
struct SqlDataReply
{
	// High-level command performed (transfer, validate or assess)
	short _cmd;
	// Command subtype
	short _cmd_subtype;

	// Session id
	int session_id;

	// Return code of operation (-1 failed, 0 succeeded, 1 still in progress)
	int rc;

	// Return code for source and target database
	int _s_rc;
	int _t_rc;

	// Time spent on last operation (all, source and target)
	size_t _time_spent;
	size_t _s_time_spent;
	size_t _t_time_spent;

	// Source and target object names on which operation was performed
	char _s_name[1024];
	char _t_name[1024];

	// For tables, name of the index, sequence i.e for which operation is performed
	char t_o_name[1024];

	// Command result data (depends on the command type)
	long _int1;
	long _int2;
	long _int3;
	long _s_int1;
	long _s_int2;
	long _t_int1;
	long _t_int2;
	__int64 _s_bigint1;
	__int64 _t_bigint1;

	// Database error code for source and target database
	int s_error;
	int t_error;

	// Database native error text
	char s_native_error_text[1024];
	char t_native_error_text[1024];

	// SQL statements 
	const char *s_sql;
	std::string s_sql_l;
	const char *t_sql;
	std::string t_sql_l;

	// Pointer and array to data blocks specified for command
	void *data;
	char data2[1024];

	SqlDataReply() 
	{ 
		_cmd = 0; _cmd_subtype = 0; session_id = 0; rc = 0; _s_rc = 0; _t_rc = 0; 
		*_s_name = '\x0'; *_t_name = '\x0'; *t_o_name = '\x0';
		_int1 = 0; _int2 = 0; _int3 = 0; 
		_s_int1 = 0; _s_int2 = 0; 
		_t_int1 = 0; _t_int2 = 0; 
		_s_bigint1 = 0; _t_bigint1 = 0; 

		_time_spent = 0; _s_time_spent = 0; _t_time_spent = 0;

		s_error = 0; t_error = 0; 
		*s_native_error_text = '\x0'; *t_native_error_text = '\x0';
		s_sql = NULL; t_sql = NULL; data = NULL; *data2 = '\x0';
	}
};

class SqlDb
{
	// Source and target database types
	short source_type;
	short target_type;

	// Source and target database subtypes (when API is ODBC)
	short source_subtype;
	short target_subtype;

	// Communication area with source and target database threads
	SqlDbThreadCa _source_ca;
	SqlDbThreadCa _target_ca;

	// Callback function for notifications
	SqlDataCallbackFunc _callback;
	void *_callback_object;

	// How often "In Progress" notifications are sent
	size_t _callback_rate;

	// SqlDb object that read database schema
	SqlDb *_metaSqlDb;

	// Column name and data type mapping
	std::list<SqlColMap> *_column_map;
    std::list<SqlDataTypeMap> *_datatype_map;

	// Table select expressions and WHERE conditions
	std::map<std::string, std::string> *_tsel_exp_map;
	std::string *_tsel_exp_all;
	std::map<std::string, std::string> *_twhere_cond_map;

	Parameters *_parameters;
	AppLog *_log;

	// Validation options	
	int _validation_not_equal_max_rows;
	int _validation_datetime_fraction;

	const char *_mysql_validation_collate;
	
	bool _trace_diff_data;
	AppLog _trace_diff;
	
	// Session number of the interface
	int _session_id;

public:
	SqlDb();
	~SqlDb();

	// Perform static initialization of the API libraries
	int InitStatic(int db_types, const char *source_conn, const char *target_conn, int *s_rc, int *t_rc);
	// Set database configuration
	int Init(int db_types, const char *source_conn, const char *target_conn, int *s_rc, int *t_rc);

	// Connect to databases
	int Connect(int db_types, SqlDataReply &reply, int *s_rc, int *t_rc);

	// Test connection
	int TestConnection(std::string &conn, std::string &error, std::string &loaded_path, 
									std::list<std::string> &search_paths, size_t *time_spent);

	// Set object that read full database schema
	void SetMetaDb(SqlDb *meta) { _metaSqlDb = meta; }
	// Set column name and data type mapping
	void SetColumnMapping(std::list<SqlColMap> *cmap) { _column_map = cmap; }
    void SetDataTypeMapping(std::list<SqlDataTypeMap> *dtmap) { _datatype_map = dtmap; }
	void SetTableSelectExpressions(std::map<std::string, std::string> *tselmap) { _tsel_exp_map = tselmap; }
	void SetTableSelectExpressionsAll(std::string *tselall) { _tsel_exp_all = tselall; }
	void SetTableWhereConditions(std::map<std::string, std::string> *twheremap) { _twhere_cond_map = twheremap; }

	// Get the list of available tables
	int GetAvailableTables(int db_types, std::string &table_template, std::string &exclude, 
									std::list<std::string> &tables);

	// Read schema information
	int ReadSchema(int db_types, std::string &select, std::string &exclude, bool read_cns = true, bool read_idx = true);

	// Transfer table rows
	int TransferRows(SqlDataReply &reply, int options, bool create_tables, bool data);

	// Validate table row count
	int ValidateRowCount(SqlDataReply &reply);
	// Validate data in rows
	int ValidateRows(SqlDataReply &reply);
	int ValidateCompareRows(SqlCol *s_cols, SqlCol *t_cols, long s_col_count, long *s_bytes, long t_col_count, long s_rows, long t_rows, long *t_bytes, long running_rows, long running_not_equal_rows);

	// Compare string representations of numbers .5 and 0.50
	bool ValidateCompareNumbers(SqlCol *s_col, const char *s_string, int s_len, SqlCol *t_col, const char *t_string, int t_len, int *equal);
	// Compare datetime values fetched as strings
	bool ValidateCompareDatetimes(SqlCol *s_col, const char *s_string, int s_len, SqlCol *t_col, const char *t_string, int t_len, int *equal);
	// Compare strings
	bool ValidateCompareStrings(SqlCol *s_col, const char *s_string, int s_len, SqlCol *t_col, const char *t_string, int t_len, int *equal);
	// Compare LOB values
	bool ValidateCompareLobs(SqlCol *s_col, int s_len, SqlCol *t_col, int t_len, int *equal);
	// Dump differences in column
	void ValidateDiffDump(SqlCol *s_col, SqlCol *t_col, long row, long s_len, long t_len, char *s_string, char *t_string);
	void ValidateDiffDumpValue(long len, char *str);

	// Assess table rows
	int AssessRows(SqlDataReply &reply);

	// Get the length of column data in the buffer
	int GetColumnDataLen(SqlCol *cols, int row, int column, int db_type, SqlApiBase *db_api);
	// Get column data from the buffer
	int GetColumnData(SqlCol *cols, int row, int column, int db_type, SqlApiBase *db_api, char **str, bool *int_set, int *int_v, SQL_TIMESTAMP_STRUCT **ts, char **ora_date);

	// Build transfer and data validation query
	int BuildQuery(std::string &s_query, std::string &t_query, const char *s_table, const char *t_table, bool transfer);
	int BuildQueryAddOrder(std::string &s_query, std::string &s_schema, std::string &s_object, std::string &t_query, std::string &t_schema, std::string &t_object);

	// Execute the statement that does not return any result
	int ExecuteNonQuery(int db_types, SqlDataReply &reply, const char *query);

	// Create sequence object
	int CreateSequence(int db_types, SqlDataReply &reply, const char *query, const char *seq_name);

	// Drop foreign keys to the 
	int DropReferences(int db_types, const char *table, size_t *time_spent, int *keys);

	// Read specific catalog information
	int ReadConstraintTable(int db_types, const char *schema, const char *constraint, std::string &table);
	int ReadConstraintColumns(int db_types, const char *schema, const char *table, const char *constraint, std::string &cols);
	int GetKeyConstraintColumns(int db_types, SqlConstraints &cns, std::list<std::string> &key_cols);
	int GetForeignKeyConstraintColumns(int db_types, SqlConstraints &cns, std::list<std::string> &fcols, std::list<std::string> &pcols, std::string &ptable);
	int GetIndexColumns(int db_types, SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts);

	// Get database type for ODBC API
	short GetSubType(short db_types);

	// Get metadata information
	std::list<SqlColMeta>* GetTableColumns(int db_type);
	std::list<SqlConstraints>* GetTableConstraints(int db_type);
	std::list<SqlConsColumns>* GetConstraintColumns(int db_type);
	std::list<SqlComments>* GetTableComments(int db_type);
	std::list<SqlIndexes>* GetTableIndexes(int db_type);
	std::list<SqlIndColumns>* GetIndexColumns(int db_type);
	std::list<SqlIndExp>* GetIndexExpressions(int db_type);
	std::list<SqlSequences>* GetSequences(int db_type);

    // Get the target column name and data type
	void MapColumn(const char *s_table, const char *s_name, std::string &t_name, std::string &t_type);

	void SetParameters(Parameters *p);
	void SetAppLog(AppLog *l) { _log = l; }
	void SetSessionId(int id) { _session_id = id; }

	void SetCallback(void *object, SqlDataCallbackFunc func) { _callback_object = object, _callback = func; }
	void SetCallbackRate(int rate) { _callback_rate = rate; }

	// Get errors on the DB interface
	void GetErrors(SqlDataReply &reply);

	int GetSessionId() { return _session_id; }

private:
	// Check whether we need to drop, create or truncate the table 
	int PrepareTransfer(SqlCol *s_cols, const char *s_table, const char *t_table, size_t col_count, int options, SqlDataReply &reply);

	bool IsSpecialIdentifier(const char *s_name);

	// Generate SQL CREATE TABLE statement
	int GenerateCreateTable(SqlCol *s_cols, const char *s_table, const char *t_table, int col_count, std::string &sql);
	void AddMySQLTableOptions(const char *s_table, std::string &sql);
	
	// Get DEFAULT if it is a literal (MySQL i.e. requires full column specification in ALTER TABLE)
	int	GetDefaultClause(const char *s_table, char *column, std::string &default_clause);
	// Get IDENTITY clause to be added to column definition
	int GetInlineIdentityClause(const char *s_table, char *column, std::string &identity_clause);

    // Check if a column belongs to primary key
    bool IsPrimaryKeyColumn(const char *s_table, const char *column);

	// Execute a command by database API threads
	int Execute(int cmd);
	// Execute one command for source, another for target in concurrent threads and wait
	int Execute(int cmd1, int cmd2);

	// Initialize database API once per process
	int InitStaticApi(const char *conn, std::string &error);
	// Create database API object for source database
	SqlApiBase* CreateDatabaseApi(const char *conn, short *type);

	// Initialize database API
	int InitCa(SqlDbThreadCa *ca, SqlApiBase *db_api);

	// Copy column definitions and buffers
	int CopyColumns(SqlCol *cols, SqlCol **cols_copy, long col_count, long allocated_array_rows);
	// Copy data
	int	CopyColumnData(SqlCol *s_cols, SqlCol *t_cols, long col_count, long rows_fetched);

#if defined(WIN32) || defined(WIN64)
	// Start a worker thread
	static unsigned int __stdcall StartWorkerS(void *v_ca);
#else
	static void* StartWorkerS(void *v_ca);
#endif
	int StartWorker(SqlDbThreadCa *ca);
};

#endif // sqlines_sqldb_h
