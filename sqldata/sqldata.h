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

// SQLData

#ifndef sqldata_sqldata_h
#define sqldata_sqldata_h

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <string>
#include <list>
#include <queue>
#include <map>
#include "sqldb.h"
#include "parameters.h"
#include "applog.h"

#define SQLDATA_NAME  		        "SQLines Data"	
#define SQLDATA_VERSION_NUMBER		"3.3.31"	

#if defined(_WIN64)
#define SQLDATA_VERSION				SQLDATA_NAME ## " " ## SQLDATA_VERSION_NUMBER ##" x64"	
#elif defined(WIN32)
#define SQLDATA_VERSION				SQLDATA_NAME ## " " ## SQLDATA_VERSION_NUMBER
#elif defined(__x86_64__)
#define SQLDATA_VERSION				SQLDATA_NAME " " SQLDATA_VERSION_NUMBER " x86_64 Linux"
#else
#define SQLDATA_VERSION				SQLDATA_NAME " " SQLDATA_VERSION_NUMBER " Linux"
#endif

#define SQLDATA_DESC				"Database Migration Tool"
#define SQLDATA_COPYRIGHT			"Copyright (c) 2020 SQLines. All Rights Reserved."

#define SQLDATA_DEFAULT_SESSIONS	4

// SQLData named pipe
#define SQLDATA_NAMED_PIPE			"\\\\.\\pipe\\SqlData"

// SQLData worker process executable
#define SQLDATA_WORKER_EXE			"sqldataworker.exe"

struct SqlMetaTask
{
	int type;
	
	std::string s_name;
	std::string t_name;

	// For tables, the name of index, sequence, constraint depending on operation
	std::string t_o_name;

	std::string columns;
	std::string conditions;
	std::string statement;

	SqlMetaTask() {	type = 0; }
};

struct SqlObjMetaTask
{
	SqlObjMeta *source;

	SqlObjMetaTask() {	source = NULL; }
};

class SqlData
{
	// Source and target connection strings
	std::string _source_conn;
	std::string _target_conn;

	// Source and target database types
	short _source_type;
	short _target_type;

	// Source and target database subtypes (when API is ODBC)
	short source_subtype;
	short target_subtype;

	// Current command to perform
	short _command;
	// Command options
	int _command_options;

	// List of tables
	std::list<std::string> *_tables;
	// List of queries
	std::map<std::string, std::string> *_queries;
	// Metadata transfer queue
	std::list<SqlMetaTask> _meta_tasks;
	std::list<SqlObjMetaTask> _obj_meta_tasks;

	// Tables that are being processed now (data transfer)
	std::list<std::string> _tables_in_progress;
	// Metadata tasks in process
	std::list<SqlMetaTask> _meta_in_progress;

	// Worker process
	std::string _worker_exe;

	// Number of workers processes and threads to start
	short _workers;
	short _local_workers;

	// Number of concurrent sessions
	int _max_sessions;

	// Schema name map
	std::map<std::string, std::string> _schema_map;
	std::string _schema_map_str;

	// Table, column name, data type and constraint mapping
	std::map<std::string, std::string> _table_map;
	std::list<SqlColMap> _column_map;
    std::list<SqlDataTypeMap> _datatype_map;
	std::map<std::string, std::string> _cns_map;

	// Table select expressions and WHERE conditions
	std::map<std::string, std::string> _tsel_exp_map;
	std::string _tsel_exp_all;
	std::map<std::string, std::string> _twhere_cond_map;

	std::map<std::string, int> _constraint_ref;

	// General DDL and data options
	bool _migrate_tables;
	bool _migrate_data;
	bool _migrate_constraints;
	bool _migrate_indexes;

#if defined(WIN32) || defined(_WIN64)
	// Event to wait until all tasks are completed
	HANDLE _completed_event;
#else
	Event _completed_event;
#endif

	// Initial number of workers
	int _all_workers;
	// Number of running workers
	int _running_workers;

	// Total number of rows
	int _s_total_rows;
	int _t_total_rows;

	// Total number of bytes
	__int64 _s_total_bytes;
	__int64 _t_total_bytes;

#if defined(WIN32) || defined(_WIN64)
	// Critical sections
	CRITICAL_SECTION _task_queue_critical_section;
	CRITICAL_SECTION _worker_critical_section;
#else
	pthread_mutex_t _task_queue_critical_section;
	pthread_mutex_t _worker_critical_section;
#endif

	// Local in-process database interface (to retrieve metadata and process tasks)
	SqlDb _db;

	// Callback function for notifications
	SqlDataCallbackFunc _callback;
	void *_callback_object;

	// How often "In Progress" notifications are sent
	int _callback_rate;

	Parameters *_parameters;
	AppLog *_log;
	bool _trace;

	// SQL parser object
	void *_sqlParser;

public:
	SqlData();

	// Specify source and target database connection strings
	void SetConnectionStrings(std::string &source, std::string &target, short *s_type, short *t_type);

	// Perform local connection for metadata read
	int ConnectMetaDb(SqlDataReply &reply, int db_types);

	// Get the list of available tables
	int GetAvailableTables(std::string &table_template, std::string &exclude, std::list<std::string> &tables);

	// Set tables or queries for processing
	void SetTables(std::list<std::string> *tables) { _tables = tables; }
	void SetQueries(std::map<std::string, std::string> *queries) { _queries = queries; }

	// Run the data transfer or validation
	int Run();	

	// Test connection
	int TestConnection(std::string &conn, std::string &error, std::string &loaded_path, std::list<std::string> &search_paths, size_t *time_spent);

	// Create queues to transfer schema metadata
	int CreateMetadataQueues(std::string &select, std::string &exclude);
	void CreateMetadataTaskForColumnDefault(SqlColMeta &col);

	// Create tasks queue to transfer non-table objects
	int CreateObjectQueue(std::string &select, std::string &exclude, int *total_obj, int *total_lines, int *total_bytes);

	// Set schema name mapping
	void SetSchemaMapping(std::string &map) { _schema_map_str = map; }
	void SetSchemaMapping();

	// Set table name mapping
	void SetTableMappingFromFile(std::string &file);
	// Set column name mapping
	void SetColumnMappingFromFile(std::string &file);
	// Set constraint name mapping
	void SetConstraintMappingFromFile(std::string &file);
    // Set global data type mapping
    void SetDataTypeMappingFromFile(std::string &file);
	// Set table select expressions from file
	void SetTableSelectExpressionsFromFile(std::string &file);
	void SetTableSelectExpressionsAllFromFile(std::string &file);
	// Set table WHERE conditions from file
	void SetTableWhereConditionsFromFile(std::string &file);

	// Set worker process
	void SetWorkerProcess(std::string worker_exe) { _worker_exe = worker_exe; }

	// Set the number of worker processes
	void SetWorkers(short workers) { _workers = workers; }
	// Set the number of worker threads
	void SetLocalWorkers(short workers) { _local_workers = workers; }

	void SetCallback(void *object, SqlDataCallbackFunc func);
	void SetCallbackRate(int rate) { _callback_rate = rate; }

	// Set the number of concurrent sessions
	int SetConcurrentSessions(int max_sessions, int table_count);

	// Get errors on local DB interface
	void GetErrors(SqlDataReply &reply);

	void SetCommand(short c) { _command = c; }
	void SetCommandOptions(int c) { _command_options = c; }
	void SetParameters(Parameters *p);
	void SetAppLog(AppLog *l) { _log = l; } 

	// Drop references to the selected tables in the target database
	int DropReferences(int *all_keys, size_t *time_spent);

	// Callbacks
	void Callback(SqlDataReply *reply);
	static void CallbackS(void *object, SqlDataReply *reply);

private:
	// Define the source and target types
	short DefineDbType(const char *conn);
	short DefineSqlParserType(short type);

	// Initialize local (in-process) database interface
	int InitLocal(SqlDataReply &reply, int db_types, int *s_rc, int *t_rc);

	// Connect to source and target databases through in-process interface
	int ConnectLocal(SqlDataReply &reply, int db_types, int *s_rc, int *t_rc);

	// Start local in-process workers
	int StartLocalWorkers();
	int StartLocalWorker(SqlDb *sqlDb);
#if defined(WIN32) || defined(_WIN64)
	static unsigned int __stdcall StartLocalWorkerS(void *data);
#else
	static void* StartLocalWorkerS(void *data);
#endif

#if defined(WIN32) || defined(_WIN64)
	// Start communication sessions with worker processes
	int StartSessions();
	static unsigned int __stdcall StartSessionsS(void *object);
#endif

	// Start a communication with a worker process
#if defined(WIN32) || defined(_WIN64)
	int StartWorker(HANDLE hp);
	static unsigned int __stdcall StartWorkerS(void *object); 
#endif
	
	// Get next table for processing
	std::string GetNextTask();
	// Get next query for processing
	bool GetNextQueryTask(SqlDataReply &reply);
	// Get next metadata/schema task for processing
	bool GetNextMetaTask(SqlMetaTask &task);

	// Convert SQL statement
	void ConvertSql(std::string &in, std::string &out);

	// Notify that the table processing (data transfer) completed
	void NotifyTableCompletion(std::string table);
	// Notify that the meta data task completed
	void NotifyMetaCompletion(SqlMetaTask &task);

	// Worker thread is terminating work
	void NotifyWorkerExit();

	// Check whether data of the table already transferred
	bool IsDataTransferred(std::string &table);
	// For foreign key task check if the primary key was already created
	bool IsPrimaryKeyCreated(SqlMetaTask &task);
	// Check is a DDL task is running for the specified object
	bool IsDdlRunning(std::string &table);
	// For trigger task, check that all sequences already created
	bool IsSequencesCreated();
	// Check if identity column defined for the table
	bool IsIdentityDefined(std::string &schema, std::string &table);

	// Get the target name
	void MapObjectName(std::string &source, std::string &target);
	void MapObjectName(std::string &s_schema, std::string &s_table, std::string &t_name, bool tmap = true);
	void MapObjectName(std::string &s_schema, std::string &s_table, std::string &t_schema, std::string &t_table);
	void MapConstraintName(const char *source, std::string &target, char type, std::string &table, std::string &p_table);
    void MapColumn(const char *s_table, std::string &s_name, std::string &t_name, std::string &t_type);

	// Convert identifier between databases 
	void ConvertIdentifier(const char *in, std::string &out);

	// Generate statement to emulate identity in Oracle (create sequence and trigger), sequence and default in PostgreSQL
	void GetIdentityMetaTask(SqlColMeta &col); 

	// Define foreign key ON UPDATE/ON DELETE action
	const char* GetForeignKeyAction(char action);

	// Get primary or unique key columns
	void GetKeyConstraintColumns(SqlMetaTask &task, SqlConstraints &cns, std::string &output);
	// Get foreign key columns
	void GetForeignKeyConstraintColumns(SqlMetaTask &task, SqlConstraints &cns, std::string &fcols, std::string &pcols, std::string &ptable);
    // Get index columns
	void GetIndexColumns(SqlIndexes &idx, std::string &output);
};

struct SqlDataPipe
{
	SqlData *sd;

#if defined(WIN32) || defined(WIN64)
	HANDLE hp;
#endif
};

#endif // sqldata_sqldata_h