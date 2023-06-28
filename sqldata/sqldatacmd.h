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

// SQLData Command Line 

#ifndef sqldata_sqldatacmd_h
#define sqldata_sqldatacmd_h

#include <string>
#include <queue>
#include "sqldata.h"
#include "applog.h"
#include "parameters.h"
#include "license.h"

#define SD_OPTION					"-sd"		// Source database connection string
#define TD_OPTION					"-td"		// Target database connection string
#define T_OPTION					"-t"		// List of objects
#define TF_OPTION					"-tf"		// A file with list of objects
#define TEXCL_OPTION				"-texcl"	// List of objects to exclude
#define QF_OPTION					"-qf"		// A file with SQL SELECT queries
#define CMD_OPTION					"-cmd"		// Command to perform
#define SMAP_OPTION					"-smap"		// Schema name mapping
#define TMAPF_OPTION				"-tmapf"	// Table name mapping
#define CMAPF_OPTION				"-cmapf"	// Column name and data type mapping
#define CNSMAPF_OPTION				"-cnsmapf"	// Table constraint name mapping
#define DTMAPF_OPTION				"-dtmapf"	// Global data type mapping
#define TSELF_OPTION				"-tself"	// Table select expressions
#define TSELALLF_OPTION				"-tselallf"	// Table select expressions for all tables
#define TWHEREF_OPTION				"-twheref"	// Table WHERE conditions
#define TOPT_OPTION					"-topt"		// Transfer command options
#define VOPT_OPTION					"-vopt"		// Validate command options
#define OUT_OPTION					"-out"		// Output directory
#define LOG_OPTION					"-log"		// Log option
#define TRACE_OPTION				"-trace"	// Trace option
#define SS_OPTION					"-ss"		// Number of concurrent sessions option
#define WORKERS_OPTION				"-wrk"		// Number of separate worker processes (undocumented)
#define LOCAL_WORKERS_OPTION		"-lwrk"		// Number of worker threads (undocumented)

#define SUFFIX(int_value)				((int_value == 1) ? "" : "s")
#define SUFFIX2(int_value, str1, str2)	((int_value == 1) ? str1 : str2)

// Default mapping files
#define SQLDATA_TMAP_FILE				"sqlines_tmap.txt"
#define SQLDATA_CMAP_FILE				"sqlines_cmap.txt"
#define SQLDATA_CNSMAP_FILE				"sqlines_cnsmap.txt"
#define SQLDATA_DTMAP_FILE				"sqlines_dtmap.txt"
#define SQLDATA_TSEL_FILE				"sqlines_tsel.txt"
#define SQLDATA_TSEL_ALL_FILE			"sqlines_tsel_all.txt"
#define SQLDATA_TWHERE_FILE				"sqlines_twhere.txt"

// Default log, trace and configuration files
#define SQLDATA_LOGFILE					"sqldata.log"
#define SQLDATA_TRACEFILE				"sqldata.trc"
#define SQLDATA_CONFIGFILE				"sqldata.cfg"

// SQL statement logs
#define SQLDATA_DDL_LOGFILE				"sqldata_ddl.sql"
#define SQLDATA_FAILED_DDL_LOGFILE		"sqldata_failed.sql"
#define SQLDATA_FAILED_TABLES_LOGFILE	"sqldata_failed_tables.txt"

class SqlDataCmd
{
	// Options
	std::string _sd;
	std::string _td;
	std::string _t;
	std::string _tf;
	std::string _texcl;
	std::string _qf;
	std::string _cmd;
	std::string _smap;
	std::string _tmapf;
	std::string _cmapf;
	std::string _cnsmapf;
    std::string _dtmapf;
    std::string _tself;
	std::string _tselallf;
	std::string _twheref;
	std::string _topt;
	std::string _vopt;
	std::string _out;
	std::string _logname;
	std::string _ss;

	// Target is stdout
	bool _td_stdout;

	// Number of worker processes and threads
	short _concurrent_sessions;
	short _workers;
	short _local_workers;

	// SQLData manager
	SqlData _sqlData;

	// Source and target database types
	short _source_type;
	short _target_type;

	// Command and options to perform
	short _command;
	int _command_options;

	// Command start time
	size_t _command_start;

	int _transfer_table_num;
	int _transfer_obj_num;
	int _validate_table_num;
	int _assess_table_num;

	// Command line and configuration parameters
	Parameters _parameters;
	// Logger
	AppLog _log;

	// SQL Statement logs
	AppLog _ddl_log;
	AppLog _failed_ddl_log;
	AppLog _failed_tables_log;

	// List of tables
	std::list<std::string> _tables;
	// List of queries
	std::map<std::string, std::string> _queries;

	// Total number of tables
	int _total_tables;
	// Tables for which the transfer or validation failed
	int _failed_tables;

	// Total number of non-table objects
	int _total_objects;
	int _total_object_lines;
	int _total_object_bytes;

	// Total number of DDL statement executed for target database
	int _total_target_ddl;
	int _failed_target_ddl;

	// Tables with equal and different row count or data
	int _tables_equal;
	int _tables_diff;

	// List of failed tables
	std::list<std::string> _failed_tables_list;
	std::list<std::string> _failed_tables_names_list;
	// List of tables with warnings
	std::list<std::string> _warnings_tables_list;
	// List of tables with different number of read and written rows (includes table name and row count information)
	std::list<std::string> _tables_read_write_diff_list;
	// List of tables with different row count (includes table name and row count information)
	std::list<std::string> _tables_diff_list;

	// Current executable file
	const char *_exe;

	// Callbacks called by SqlDataCmd to UI modules
	SqlDataCallbackFunc _ui_callback;
	void *_ui_callback_object;

	// License information
	License _license;

public:
	SqlDataCmd();

	// Define command line options
	int DefineOptions(int argc, char **argv);
	int DefineOptions(const char *options);

	// Run the operations
	int Run();	

	// Test a connection to the database
	int TestConnection(std::string &conn, std::string &error, std::string &loaded_path, std::list<std::string> &search_paths, size_t *time_spent);

	// Callbacks (Already called from a critical section)
	void Callback(SqlDataReply *reply);
	static void CallbackS(void *object, SqlDataReply *reply);

	// Set console output function
	void SetConsoleFunc(void *object, AppLogConsoleFunc console) { _log.SetConsoleFunc(object, console); }

	void SetUiCallback(void *o, SqlDataCallbackFunc f) { _ui_callback = f; _ui_callback_object = o; }

	bool IsStdOut() { return _td_stdout; }

	// Set the license information
	void SetLicense(const char *exe) { _license.Set(exe); }
	License *GetLicense() { return &_license; }

private:
	// Connect to the database to read metadata
	int Connect();
	// Read the database catalog to define objects and metadata
	int ReadMetadata();
	// Remove foreign keys for selected tables 
	int DropReferences();

	// Read and validate parameters
	int SetParameters();

	// Get list of objects to process from a file
	int GetObjectsFromFile(std::list<std::string> &tables);
	// Get list of queries to process from a file
	int GetQueriesFromFile(std::string &file, std::map<std::string, std::string> &queries);

	// Log executed target DDL statements
    void LogSql(SqlDataReply *reply, const char *time_str);

	// Log connection information
	void LogConnection(int type, int rc, const char *version, const char *error, size_t time_spent);

	// Define what command to perform
	void DefineCommand();
	// Define command options
	void DefineCommandOptions();

	// Output how to use the tool if /? or incorrect parameters are specified
	void PrintHowToUse();

	// Output the current date and time
	void PrintCurrentTimestamp();
	void PrintCurrentTimestamp(AppLog &log, const char *prefix);

	// Callback functions
	void CallbackTransfer(SqlDataReply *reply);
	void CallbackValidationRowCount(SqlDataReply *reply);
	void CallbackValidationRows(SqlDataReply *reply);
	void CallbackAssess(SqlDataReply *reply);

	void AddFailedTable(SqlDataReply *reply, const char *error);
	void AddWarningsTable(SqlDataReply *reply, const char *error);
};

#endif // sqldata_sqldatacmd_h