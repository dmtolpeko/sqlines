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

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#include <process.h>
#endif

#include <stdio.h>
#include <string>
#include <list>
#include "sqldb.h"
#include "sqlociapi.h"
#include "sqlsncapi.h"
#include "sqlmysqlapi.h"
#include "sqlpgapi.h"
#include "sqlctapi.h"
#include "sqlasaapi.h"
#include "sqlifmxapi.h"
#include "sqldb2api.h"
#include "sqlodbcapi.h"
#include "sqlstdapi.h"
#include "str.h"
#include "os.h"

// Constructor
SqlDb::SqlDb()
{
	source_type = 0;
	target_type = 0;
	source_subtype = 0;
	target_subtype = 0;

	_callback = NULL;
	_callback_object = NULL;
	_callback_rate = 3100;

	_metaSqlDb = this;
	_parameters = NULL;
	_log = NULL;
	_column_map = NULL;
	_datatype_map = NULL;
	_tsel_exp_map = NULL;
	_tsel_exp_all = NULL;
	_twhere_cond_map = NULL;

	_session_id = 0;

	_trace_diff_data = false;
	_validation_not_equal_max_rows = -1;
	_validation_datetime_fraction = -1;
	_mysql_validation_collate = NULL;
}

// Destructor
SqlDb::~SqlDb()
{
	delete _source_ca.db_api;
	delete _target_ca.db_api;
}

// Set database configuration
int SqlDb::Init(int db_types, const char *source_conn, const char *target_conn, int *s_rc_inout, int *t_rc_inout)
{
	SqlApiBase *s_db_api = NULL, *t_db_api = NULL;

	int s_rc = 0;
	int t_rc = 0;

	// Set static initialization results
	if(s_rc_inout != NULL)
		s_rc = *s_rc_inout;

	if(t_rc_inout != NULL)
		t_rc = *t_rc_inout;

	// Create database API object for source database
	if(s_rc == 0)
		s_db_api = CreateDatabaseApi(source_conn, &source_type);

	// Create database API object for target database
	if(t_rc == 0 && db_types != SQLDB_SOURCE_ONLY)
		t_db_api = CreateDatabaseApi(target_conn, &target_type);
	
	// Initialize the source database API (do not exit in case of error, allow initializing the target API)
	if(s_db_api != NULL)
	{
		s_db_api->SetTargetApiProvider(t_db_api);
		s_rc = InitCa(&_source_ca, s_db_api);
	
		if(s_rc_inout != NULL)
			*s_rc_inout = s_rc;
	}
    else
    {
		if(s_rc == 0)
			strcpy(_source_ca.native_error_text, "Source database type is unknown");

        s_rc = -1;

        if(s_rc_inout != NULL)
           *s_rc_inout = -1;
     }

	// Initialize the source database API
	if(t_db_api != NULL)
	{
		t_db_api->SetSourceApiProvider(s_db_api);
		t_rc = InitCa(&_target_ca, t_db_api);

		if(t_rc_inout != NULL)
			*t_rc_inout = t_rc;
	}
    else
    {
		if(t_rc == 0)
			strcpy(_target_ca.native_error_text, "Target database type is unknown");

        t_rc = -1;

        if(t_rc_inout != NULL)
            *t_rc_inout = -1;
    }

	if(s_rc == -1 || t_rc == -1)
		return -1;

	return 0;
}

// Perform static initialization of the API libraries
int SqlDb::InitStatic(int db_types, const char *source_conn, const char *target_conn, int *s_rc_out, int *t_rc_out)
{
	std::string error_text;

	int s_rc = InitStaticApi(source_conn, error_text);

	if(s_rc == -1)
	{
		_source_ca.cmd_rc = (short)s_rc;
		strcpy(_source_ca.native_error_text, error_text.c_str());
	}

	if(s_rc_out != NULL)
		*s_rc_out = s_rc;

	// Source database is only initialized
	if(db_types == SQLDB_SOURCE_ONLY)
		return s_rc;
	
	error_text.clear();
	
	int t_rc = InitStaticApi(target_conn, error_text);

	if(t_rc == -1)
	{
		_target_ca.cmd_rc = (short)t_rc;
		strcpy(_target_ca.native_error_text, error_text.c_str());
	}

	if(t_rc_out != NULL)
		*t_rc_out = t_rc;

	if(s_rc == -1 || t_rc == -1)
		return -1;

	return 0;
}

// Initialize database API once per process
int SqlDb::InitStaticApi(const char *conn, std::string &error)
{
	if(conn == NULL)
		return -1;

	int rc = 0;

	const char *cur = Str::SkipSpaces(conn);

	// Initialize MySQL C API
	if(_strnicmp(cur, "mysql", 5) == 0 || _strnicmp(cur, "mariadb", 7) == 0)
	{
		SqlMysqlApi mysqlApi;
		mysqlApi.SetParameters(_parameters);
		mysqlApi.SetAppLog(_log);

		const char *conn = strchr(cur, ',');

		if(conn != NULL)
		{
			conn = Str::SkipSpaces(conn + 1);
			mysqlApi.SetConnectionString(conn);
		}

		if(_strnicmp(cur, "mariadb", 7) == 0)
			mysqlApi.SetSubType(SQLDATA_SUBTYPE_MARIADB);

		rc = mysqlApi.InitStatic();

		if(rc == -1)
			error = mysqlApi.GetNativeErrorText();
	}

	// If API does not support static initialization 0 must be returned
	return rc;
}

// Create database API object for source database
SqlApiBase* SqlDb::CreateDatabaseApi(const char *conn, short *type)
{
	if(conn == NULL)
		return NULL;

	SqlApiBase *db_api = NULL;

	const char *cur = Str::SkipSpaces(conn);

	// Check for Oracle OCI
	if(_strnicmp(cur, "oracle", 6) == 0)
	{
		db_api = new SqlOciApi();
		cur += 6;

		if(type != NULL)
			*type = SQLDATA_ORACLE;
	}
	else
	// Check for SQL Server Native Client
	if(_strnicmp(cur, "sql", 3) == 0)
	{
		db_api = new SqlSncApi();
		cur += 3;

		if(type != NULL)
			*type = SQLDATA_SQL_SERVER;
	}
	else
	// Check for MySQL C API
	if(_strnicmp(cur, "mysql", 5) == 0)
	{
		db_api = new SqlMysqlApi();
		cur += 5;

		if(type != NULL)
			*type = SQLDATA_MYSQL;
	}
    else
	// Check for MariaDB C API
	if(_strnicmp(cur, "mariadb", 7) == 0)
	{
		db_api = new SqlMysqlApi();
		db_api->SetSubType(SQLDATA_SUBTYPE_MARIADB);

		cur += 7;

		if(type != NULL)
			*type = SQLDATA_MYSQL;
	}
	else
	// Check for PostgreSQL libpq C library
	if(_strnicmp(cur, "pg", 2) == 0)
	{
		db_api = new SqlPgApi();
		cur += 2;

		if(type != NULL)
			*type = SQLDATA_POSTGRESQL;
	}
	else
	// Check for Sybase Client library
	if(_strnicmp(cur, "sybase", 6) == 0)
	{
		db_api = new SqlCtApi();
		cur += 6;

		if(type != NULL)
			*type = SQLDATA_SYBASE;
	}
#if defined(WIN32) || defined(_WIN64)
	else
	// Sybase SQL Anywhere ODBC driver
	if(_strnicmp(cur, "asa", 3) == 0)
	{
		db_api = new SqlAsaApi();
		cur += 3;

		if(type != NULL)
			*type = SQLDATA_ASA;
	}
#endif
	else
	// Check for Informix library
	if(_strnicmp(cur, "informix", 8) == 0)
	{
		db_api = new SqlIfmxApi();
		cur += 8;

		if(type != NULL)
			*type = SQLDATA_INFORMIX;
	}
	else
	// Check for DB2 library
	if(_strnicmp(cur, "db2", 3) == 0)
	{
		db_api = new SqlDb2Api();
		cur += 3;

		if(type != NULL)
			*type = SQLDATA_DB2;
	}
#if defined(WIN32) || defined(_WIN64)
	else
	// Check for ODBC
	if(_strnicmp(cur, "odbc", 4) == 0)
	{
		db_api = new SqlOdbcApi();
		cur += 4;

		if(type != NULL)
			*type = SQLDATA_ODBC;
	}
#endif
	else
	// Check for Standard Output
	if(_strnicmp(cur, "stdout", 6) == 0)
	{
		db_api = new SqlStdApi();
		cur += 6;

		if(type != NULL)
			*type = SQLDATA_STDOUT;
	}
	
	cur = Str::SkipSpaces(cur);

	// Check driver type information
	if(*cur == ':')
	{
		cur++;

		db_api->SetDriverType(cur);

		cur = Str::SkipUntil(cur, ',');
	}

	cur = Str::SkipSpaces(cur);

	// Skip comma after database type
	if(*cur == ',')
		cur++;

	cur = Str::SkipSpaces(cur);

	// Set the connection string and parameters in the API object
	if(db_api != NULL)
	{
		db_api->SetConnectionString(cur);
		db_api->SetParameters(_parameters);
		db_api->SetAppLog(_log);
	}

	return db_api;
}

// Initialize database API
int SqlDb::InitCa(SqlDbThreadCa *ca, SqlApiBase *db_api)
{
	if(db_api == NULL)
		return -1;

	ca->sqlDb = this;
	ca->db_api = db_api;

	// Initialize the database API library
	int rc = ca->db_api->Init();

	if(rc == -1)
	{
		ca->cmd_rc = (short)rc;
		strcpy(ca->native_error_text, ca->db_api->GetNativeErrorText());

		return rc;
	}

	// Create synchronization objects 
#if defined(WIN32) || defined(_WIN64)
	ca->_wait_event = CreateEvent(NULL, FALSE,  FALSE, NULL);
	ca->_completed_event = CreateEvent(NULL, FALSE,  FALSE, NULL);
#else
	Os::CreateEvent(&ca->_wait_event);
	Os::CreateEvent(&ca->_completed_event);
#endif

	// Start a thread to handle requests for the database
#if defined(WIN32) || defined(_WIN64)
	_beginthreadex(NULL, 0, &SqlDb::StartWorkerS, ca, 0, NULL);
#else
	pthread_t thread;
	pthread_create(&thread, NULL, &SqlDb::StartWorkerS, ca);
#endif
	return 0;
}

// Connect to databases
int SqlDb::Connect(int db_types, SqlDataReply &reply, int *s_rc_in, int *t_rc_in)
{
	size_t start = Os::GetTickCount();

	int rc = 0;
	int s_rc = 0;
	int t_rc = 0;

	// Set initialization results
	if(s_rc_in != NULL)
		s_rc = *s_rc_in;

	if(t_rc_in != NULL)
		t_rc = *t_rc_in;

	// Connect to both databases (both were initialized successfully)
	if(db_types == SQLDB_BOTH && s_rc == 0 && t_rc == 0)
	{
		rc = Execute(SQLDATA_CMD_CONNECT);
	}
	else
	// Connect to source only
	if(db_types == SQLDB_SOURCE_ONLY || (db_types == SQLDB_BOTH && s_rc == 0))
	{
		if(_source_ca.db_api != NULL)
		{
			s_rc = _source_ca.db_api->Connect(&_source_ca._time_spent);

			if(s_rc == -1)
				_source_ca.error = _source_ca.db_api->GetError();
		}
	}
	else
	// Connect to target only
	if(db_types == SQLDB_SOURCE_ONLY || (db_types == SQLDB_BOTH && t_rc == 0))
	{
		if(_target_ca.db_api != NULL)
		{
			t_rc = _target_ca.db_api->Connect(&_target_ca._time_spent);

			if(t_rc == -1)
				_target_ca.error = _target_ca.db_api->GetError();
		}
	}

	if(s_rc == -1 || t_rc == -1)
		rc = -1;

	source_subtype = GetSubType(SQLDB_SOURCE_ONLY);
	target_subtype = GetSubType(SQLDB_TARGET_ONLY);

	// Set results
	reply._cmd_subtype = SQLDATA_CMD_CONNECT;
	reply.rc = rc;

	reply._s_rc = _source_ca.cmd_rc;
	reply._t_rc = _target_ca.cmd_rc;

	// Versions
	if(_source_ca.db_api != NULL)
		strcpy(reply._s_name, _source_ca.db_api->GetVersion().c_str());
	
	if(_target_ca.db_api != NULL)
		strcpy(reply._t_name, _target_ca.db_api->GetVersion().c_str());

	reply.s_error = _source_ca.error;
	reply.t_error = _target_ca.error;

	reply._time_spent = GetTickCount() - start;
	reply._s_time_spent = _source_ca._time_spent;
	reply._t_time_spent = _target_ca._time_spent;

	if(reply.s_error == -1 && _source_ca.db_api != NULL)
		strcpy(reply.s_native_error_text, _source_ca.db_api->GetNativeErrorText());

	if(reply.t_error == -1 && _target_ca.db_api != NULL)
		strcpy(reply.t_native_error_text, _target_ca.db_api->GetNativeErrorText());

	return rc;
}

// Test connection
int SqlDb::TestConnection(std::string &conn, std::string &error, std::string &loaded_path, 
									std::list<std::string> &search_paths, size_t *time_spent)
{
	size_t start = Os::GetTickCount();

	// Create database API
	SqlApiBase *api = CreateDatabaseApi(conn.c_str(), NULL);

	if(api == NULL)
	{
		error = "Unknown database API";
		return -1;
	}
	
	// Initialize the database API library for process
	int rc = InitStaticApi(conn.c_str(), error);

	if(rc == 0)
	{
		// Initialize the database API library
		rc = api->Init();

		api->GetDriverPaths(search_paths);
		loaded_path = api->GetLoadedDriver();

		if(rc == -1)
		{
			error = api->GetNativeErrorText();
			delete api;

			if(time_spent != NULL)
				*time_spent = Os::GetTickCount() - start;

			return -1;
		}
	}

	// Try to connect
	if(rc == 0)
	{
		rc = api->Connect(NULL);

		if(rc == -1)
			error = api->GetNativeErrorText();

		api->Disconnect();
		api->Deallocate();
	}

	delete api;

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return rc;
}

// Get the list of available tables
int SqlDb::GetAvailableTables(int db_types, std::string &table_template, std::string &exclude, 
									std::list<std::string> &tables)
{
	if(_source_ca.db_api == NULL)
		return -1;

	int rc = 0;

	if(db_types == SQLDB_SOURCE_ONLY)
		rc = _source_ca.db_api->GetAvailableTables(table_template, exclude, tables);

	return rc;
}

// Read schema information
int SqlDb::ReadSchema(int db_types, std::string &select, std::string &exclude, bool read_cns, bool read_idx)
{
	int rc = 0;
	
	// Read source and target schemas
	if(db_types == SQLDB_BOTH && _source_ca.db_api != NULL && _target_ca.db_api != NULL)
	{
		if(target_type == SQLDATA_ORACLE)
		{
			_source_ca._void1 = (void*)select.c_str();
			_target_ca._void1 = (void*)select.c_str();

			_source_ca._void2 = (void*)exclude.c_str();
			_target_ca._void2 = (void*)exclude.c_str();

			rc = Execute(SQLDATA_CMD_READ_SCHEMA, SQLDATA_CMD_READ_SCHEMA_FOR_TRANSFER_TO);
		}
		else
			rc = _source_ca.db_api->ReadSchema(select.c_str(), exclude.c_str(), read_cns, read_idx);
	}
	else
	// Read the source schema only
	if(db_types == SQLDB_SOURCE_ONLY && _source_ca.db_api != NULL)
		rc = _source_ca.db_api->ReadSchema(select.c_str(), exclude.c_str(), read_cns, read_idx);

	return rc;
}

// Read non-table objects
int SqlDb::ReadObjects(int db_types, std::string &select, std::string &exclude)
{
	int rc = 0;
	
	// Read the source schema only
	if(db_types == SQLDB_SOURCE_ONLY && _source_ca.db_api != NULL)
		rc = _source_ca.db_api->ReadObjects(select.c_str(), exclude.c_str());

	return rc;
}

// Read table name by constraint name
int SqlDb::ReadConstraintTable(int /*db_types*/, const char *schema, const char *constraint, std::string &table)
{
	if(_source_ca.db_api == NULL)
		return -1;

	int rc = _source_ca.db_api->ReadConstraintTable(schema, constraint, table);

	return rc;
}

// Read columns of the specified constraint name
int SqlDb::ReadConstraintColumns(int /*db_types*/, const char *schema, const char *table, const char *constraint, std::string &cols)
{
	if(_source_ca.db_api == NULL)
		return -1;

	int rc = _source_ca.db_api->ReadConstraintColumns(schema, table, constraint, cols);

	return rc;
}

// Check if identity column defined for the table
bool SqlDb::IsIdentityDefined(int /*db_types*/, std::string &schema, std::string &table)
{
	if(_source_ca.db_api == NULL)
		return false;

	return _source_ca.db_api->IsIdentityDefined(schema, table);
}

// Read columns for the specified PRIMARY and UNIQUE key constraint
int SqlDb::GetKeyConstraintColumns(int /*db_types*/, SqlConstraints &cns, std::list<std::string> &key_cols)
{
	if(_source_ca.db_api == NULL)
		return -1;

	int rc = _source_ca.db_api->GetKeyConstraintColumns(cns, key_cols);

	return rc;
}

// Get columns for the specified FOREIGN key constraint
int SqlDb::GetForeignKeyConstraintColumns(int /*db_types*/, SqlConstraints &cns, std::list<std::string> &fcols,
											std::list<std::string> &pcols, std::string &ptable)
{
	if(_source_ca.db_api == NULL)
		return -1;

	int rc = _source_ca.db_api->GetForeignKeyConstraintColumns(cns, fcols, pcols, ptable);

	return rc;
}

// Get column list for the specified index
int SqlDb::GetIndexColumns(int /*db_types*/, SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts)
{
	if(_source_ca.db_api == NULL)
		return -1;

	return _source_ca.db_api->GetIndexColumns(idx, idx_cols, idx_sorts);
}

// Get information on table columns
std::list<SqlColMeta>* SqlDb::GetTableColumns(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetTableColumns();
}

// Get information on table constraints
std::list<SqlConstraints>* SqlDb::GetTableConstraints(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetTableConstraints();
}

// Get information about constraint columns
std::list<SqlConsColumns>* SqlDb::GetConstraintColumns(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetConstraintColumns();
}

std::list<SqlComments>* SqlDb::GetTableComments(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetTableComments();
}

std::list<SqlIndexes>* SqlDb::GetTableIndexes(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetTableIndexes();
}

std::list<SqlIndColumns>* SqlDb::GetIndexColumns(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetIndexColumns();
}

std::list<SqlIndExp>* SqlDb::GetIndexExpressions(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetIndexExpressions();
}

// Get sequences
std::list<SqlSequences>* SqlDb::GetSequences(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetSequences();
}

// Get stored procedures
std::list<SqlObjMeta>* SqlDb::GetProcedures(int /*db_type*/)
{
	if(_source_ca.db_api == NULL)
		return NULL;

	return _source_ca.db_api->GetProcedures();
}

// Transfer table rows
int SqlDb::TransferRows(SqlDataReply &reply, int options, bool create_tables, bool data)
{
	if(_source_ca.db_api == NULL || _target_ca.db_api == NULL)
		return -1;

	// Notify that a table was selected for processing
	if(_callback != NULL)
	{
		reply._cmd_subtype = SQLDATA_CMD_STARTED;
		_callback(_callback_object, &reply);
	}

	size_t start = GetTickCount(), now = start, prev_update = start;

	std::string select, t_select;

	// SELECT query
	if(reply.s_sql_l.empty())
	{
		if(_metaSqlDb != NULL)
			_metaSqlDb->BuildQuery(select, t_select, reply._s_name, reply._t_name, true);
	}
	else
		select = reply.s_sql_l;

	size_t col_count = 0, allocated_array_rows = 0;
	int rows_fetched = 0, rows_written = 0;
	int all_rows_read = 0, all_rows_written = 0;
	
	size_t bytes_written = 0;
	__int64 all_bytes_written = 0;

	size_t time_read = 0, all_time_read = 0;
	size_t time_write = 0, all_time_write = 0;
	
	SqlCol *s_cols = NULL, *s_cols_copy = NULL, *cur_cols = NULL, *t_cols = NULL;

	bool no_more_data = false;
	size_t buffer_rows = 0;

	// Fetch only one row to get cursor definition when no data transferred
	if(!data)
		buffer_rows = 1;

	// Open cursor allocating 10M buffer
	int rc = _source_ca.db_api->OpenCursor(select.c_str(), buffer_rows, 1024*1024*32, &col_count, &allocated_array_rows, 
		&rows_fetched, &s_cols, &time_read, false, _datatype_map);

	if(rc != -1)
	{
		all_rows_read += rows_fetched;
		all_time_read += time_read;
	}

	// Notify on opening cursor
	if(_callback != NULL)
	{
		reply._cmd_subtype = SQLDATA_CMD_OPEN_CURSOR;
		reply.rc = rc;
		reply._s_int1 = all_rows_read;
		reply._s_int2 = (int)time_read;

		// There can be warnings (rc = 1) such as truncation error, get exact message
		if(rc != 0)
		{
			reply.s_error = _source_ca.db_api->GetError();
			strcpy(reply.s_native_error_text, _source_ca.db_api->GetNativeErrorText());
		}

		// In case of error, callback will be called on completion
		if(rc != -1)
			_callback(_callback_object, &reply);
	}

	if(rc == -1)
		return -1;

	// Check if we can read and write in parallel for this table
	bool parallel_read_write = _source_ca.db_api->CanParallelReadWrite();

	// Check whether target bound the source buffers, so the same buffers must be passed to TransferRows
	bool data_bound = _target_ca.db_api->IsDataBufferBound();

	// Oracle OCI returns 100, ODBC 0 when rows are less than allocated array
	if(rc == 100 || rows_fetched < allocated_array_rows)
		no_more_data = true;

	cur_cols = s_cols;

	// Allocate a copy of column buffer if data were not fetched in one iteration
	if(no_more_data == false && parallel_read_write == true)
	{
		CopyColumns(s_cols, &s_cols_copy, col_count, allocated_array_rows);
		cur_cols = s_cols_copy;
	}

	bool ddl_error = false;

	// Check whether we need to drop, create or truncate the table 
	if(create_tables && options != 0)
	{
		rc = PrepareTransfer(s_cols, reply._s_name, reply._t_name, col_count, options, reply);

		if(rc == -1)
		{
			reply._cmd_subtype = SQLDATA_CMD_DUMP_DDL;
			reply.rc = rc;
			reply.data = s_cols;
			reply._int1 = (int)col_count;

			if(_callback != NULL)
				_callback(_callback_object, &reply);

			reply.data = NULL;
			reply._int1 = 0;

			// Notify on transfer completion with DDL error (call back will be called in the caller)
			reply._cmd_subtype = SQLDATA_CMD_COMPLETE_WITH_DDL_ERROR;
			reply.rc = rc;

			ddl_error = true;
		}
	}

	bool bulk_init = false;

	// Initialize the bulk insert in the target database
	if(data && rc != -1 && rows_fetched != 0)
	{
		rc = _target_ca.db_api->InitBulkTransfer(reply._t_name, col_count, allocated_array_rows, cur_cols, &t_cols);
		bulk_init = true;
	}

	while(data && rc != -1)
	{
		// Only one fetch, or the last fetch
		if(no_more_data == true)
		{
			// Sybase CT-lib and ODBC return NO DATA after all rows fetched so check for number of rows
			if(rows_fetched != 0)
			{
				// Copy buffer if it is non-single fetch and source data buffer is bound
				if(s_cols_copy != NULL && data_bound == true)
					CopyColumnData(s_cols, s_cols_copy, col_count, rows_fetched);
				// Do not copy, use source buffers
				else
					cur_cols = s_cols;

				// Insert last portion of rows
				rc = _target_ca.db_api->TransferRows(cur_cols, rows_fetched, &rows_written, &bytes_written,
						&time_write);

				if(rc == 0)
				{
					all_rows_written += rows_written;
					all_bytes_written += bytes_written;
					all_time_write += time_write;
				}
			}

			break;
		}
		else
		// Parallel read/write is not allowed
		if(parallel_read_write == false)
		{
			// Insert the current row
			rc = _target_ca.db_api->TransferRows(cur_cols, rows_fetched, &rows_written, &bytes_written,
							&time_write);

			if(rc == -1)
				break;

			// Get next row
			rc = _source_ca.db_api->Fetch(&rows_fetched, &time_read);
		}
		// Use concurrent threads
		else
		{
			// Copy data
			CopyColumnData(s_cols, s_cols_copy, col_count, rows_fetched);
			cur_cols = s_cols_copy;

			// Prepare insert command
			_target_ca._int2 = rows_fetched; 
			_target_ca._void1 = cur_cols; 

			// Fetch the next set of data, and insert the current set in 2 concurrent threads
			rc = Execute(SQLDATA_CMD_FETCH_NEXT, SQLDATA_CMD_INSERT_NEXT);

			if(rc == -1)
				break;

			rows_fetched = _source_ca._int1;
			time_read = (size_t)_source_ca._int4;
			rows_written = _target_ca._int1;
			bytes_written = (size_t)_target_ca._int3;
			time_write = (size_t)_target_ca._int4;
		}

		all_rows_read += rows_fetched;
		all_time_read += time_read;
		all_rows_written += rows_written;
		all_bytes_written += bytes_written;
		all_time_write += time_write;

		now = GetTickCount();

		// Notify on transfer in progress every callback_rate milliseconds
		if(_callback != NULL && (now - prev_update >= _callback_rate))
		{
			reply._cmd_subtype = SQLDATA_CMD_IN_PROGRESS;
			reply.rc = 0;
			reply._int1 = (int)(now - start);
			reply._s_int1 = all_rows_read;
			reply._s_int2 = (int)all_time_read;
			reply._t_int1 = (int)all_rows_written;
			reply._t_int2 = (int)all_time_write;
			reply._t_bigint1 = all_bytes_written;

			_callback(_callback_object, &reply);

			prev_update = now;
		}

		// it was the last fetch
		if(rc == 100 || rows_fetched < allocated_array_rows)
			no_more_data = true;
	}

	// Complete transfer
	if(bulk_init == true)
	{
		int close_rc = _target_ca.db_api->CloseBulkTransfer();

		if(close_rc == -1)
		{
			rc = -1;
			reply._t_rc = -1;
		}
	}

	// Close the source database cursor
	_source_ca.db_api->CloseCursor();

	// Delete the copy buffer
	if(s_cols_copy != NULL)
	{
		for(int i = 0; i < col_count; i++)
		{
			delete [] s_cols_copy[i]._data;
			delete [] s_cols_copy[i]._ind2;
			delete [] s_cols_copy[i]._len_ind2;
		}

		delete [] s_cols_copy;
	}

	// Notify on transfer completion (call back will be called in the caller)
	if(data && ddl_error == false)
	{
		reply._cmd_subtype = SQLDATA_CMD_COMPLETE;
		reply.rc = rc;
		reply._int1 = (int)(GetTickCount() - start);
		reply._s_int1 = all_rows_read;
		reply._s_int2 = (int)all_time_read;
		reply._t_int1 = all_rows_written;
		reply._t_int2 = (int)all_time_write;
		reply._t_bigint1 = all_bytes_written;

		// Set error information in case of a failure
		if(rc == -1)
		{
			reply.s_error = _source_ca.db_api->GetError();
			reply.t_error = _target_ca.db_api->GetError();

			strcpy(reply.s_native_error_text, _source_ca.db_api->GetNativeErrorText());
			strcpy(reply.t_native_error_text, _target_ca.db_api->GetNativeErrorText());
		}
	}

	// Notify that the data transfer was skipped
	if(!data)
	{
		reply._cmd_subtype = SQLDATA_CMD_SKIPPED;
		reply.rc = 0;
	}

	return (rc == 100) ? 0 : rc;
}

// Assess table rows
int SqlDb::AssessRows(SqlDataReply &reply)
{
	if(_source_ca.db_api == NULL)
		return -1;

	// Notify that a table was selected for processing
	if(_callback != NULL)
	{
		reply._cmd_subtype = SQLDATA_CMD_STARTED;
		_callback(_callback_object, &reply);
	}

	size_t start = GetTickCount(), now = start, prev_update = start;

	std::string select, t_select;

	// SELECT query
	if(_metaSqlDb != NULL)
		_metaSqlDb->BuildQuery(select, t_select, reply._s_name, reply._t_name, true);

	//select = "select well_te from afmss.well_remarks where well_rmks_id=100827";

	size_t col_count = 0, allocated_array_rows = 0;
	int rows_fetched = 0;
	int all_rows_read = 0;
	__int64 all_bytes_read = 0;
	
	size_t time_read = 0, all_time_read = 0;
	
	SqlCol *s_cols = NULL;

	bool no_more_data = false;

	// Open cursor allocating 10M buffer
	int rc = _source_ca.db_api->OpenCursor(select.c_str(), 0, 1024*1024*10, &col_count, &allocated_array_rows, 
		&rows_fetched, &s_cols, &time_read);

	if(rc != -1)
	{
		all_rows_read += rows_fetched;
		all_time_read += time_read;
	}

	// Notify on opening cursor
	if(_callback != NULL)
	{
		reply._cmd_subtype = SQLDATA_CMD_OPEN_CURSOR;
		reply.rc = rc;
		reply._s_int1 = all_rows_read;
		reply._s_int2 = (int)time_read;

		// There can be warnings (rc = 1) such as truncation error, get exact message
		if(rc != 0)
		{
			reply.s_error = _source_ca.db_api->GetError();
			strcpy(reply.s_native_error_text, _source_ca.db_api->GetNativeErrorText());
		}

		// In case of error, callback will be called on completion
		if(rc != -1)
			_callback(_callback_object, &reply);
	}

	if(rc == -1)
		return -1;

	// Oracle OCI returns 100, ODBC 0 when rows are less than allocated array
	if(rc == 100 || rows_fetched < allocated_array_rows)
		no_more_data = true;

	while(rc != -1)
	{
		// Prepare buffers and calculate data size
		for(int i = 0; i < rows_fetched; i++)
		{
			for(int k = 0; k < col_count; k++)
			{
				if(source_type == SQLDATA_DB2 || source_type == SQLDATA_SQL_SERVER || 
					source_type == SQLDATA_INFORMIX || source_type == SQLDATA_MYSQL ||
					source_type == SQLDATA_ASA)
				{
					if(s_cols[k].ind != NULL && s_cols[k].ind[i] != -1)
					{
						// Source indicator can contain value larger than fetch size in case if data truncated
						// warning risen (Informix when LOB fecthed as VARCHAR)
						if(s_cols[k].ind[i] > s_cols[k]._fetch_len)
							all_bytes_read += s_cols[k]._fetch_len;
						else
							all_bytes_read += s_cols[k].ind[i];
					}
				}
			}
		}

		if(no_more_data == true)
			break;

		// Get next rows
		rc = _source_ca.db_api->Fetch(&rows_fetched, &time_read);

		all_rows_read += rows_fetched;
		all_time_read += time_read;

		now = GetTickCount();

		// Notify on assessment in progress every callback_rate milliseconds
		if(_callback != NULL && (now - prev_update >= _callback_rate))
		{
			reply._cmd_subtype = SQLDATA_CMD_IN_PROGRESS;
			reply.rc = 0;
			reply._int1 = (int)(now - start);
			reply._s_int1 = all_rows_read;
			reply._s_int2 = (int)all_time_read;
			reply._s_bigint1 = all_bytes_read;

			_callback(_callback_object, &reply);

			prev_update = now;
		}

		// Last fetch in ODBC-like interface, no new data in buffer
		if(rc == 100 && rows_fetched == 0)
			break;

		// it was the last fetch
		if(rc == 100 || rows_fetched < allocated_array_rows)
			no_more_data = true;
	}

	// Close the source database cursor
	_source_ca.db_api->CloseCursor();

	reply._cmd_subtype = SQLDATA_CMD_COMPLETE;
	reply.rc = rc;
	reply._int1 = (int)(GetTickCount() - start);
	reply._s_int1 = all_rows_read;
	reply._s_int2 = (int)all_time_read;
	reply._s_bigint1 = all_bytes_read;

	// Set error information in case of a failure
	if(rc == -1)
	{
		reply.s_error = _source_ca.db_api->GetError();
		strcpy(reply.s_native_error_text, _source_ca.db_api->GetNativeErrorText());
	}

	return (rc == 100) ? 0 : rc;
}

// Check whether we need to drop, create or truncate the table 
int SqlDb::PrepareTransfer(SqlCol *s_cols, const char *s_table, const char *t_table, size_t col_count, int options, SqlDataReply &reply)
{
	if(s_table == NULL || t_table == NULL || _target_ca.db_api == NULL || s_cols == NULL)
		return -1;

	int rc = 0;
	size_t time_spent = 0;
	
	// Check for necessity to DROP existing table
	if(options & SQLDATA_OPT_DROP)
	{
		std::string drop_stmt;

		rc = _target_ca.db_api->DropTable(t_table, &time_spent, drop_stmt);

		if(_callback != NULL)
		{
			reply._cmd_subtype = SQLDATA_CMD_DROP;
			reply.rc = rc;
			reply._int1 = (int)time_spent;
			reply.t_sql = drop_stmt.c_str();

			if(rc != 0)
			{
				reply.t_error = _target_ca.db_api->GetError();
				strcpy(reply.t_native_error_text, _target_ca.db_api->GetNativeErrorText());
			}

			_callback(_callback_object, &reply);
		}

		// If failed with the error other than table does not exist, then exit
		if(rc == -1 && reply.t_error != SQL_DBAPI_NOT_EXISTS_ERROR)
			return rc;
	}

	// Check truncate command
	if(options & SQLDATA_OPT_TRUNCATE)
	{
		std::string command = "TRUNCATE TABLE ";
		command += t_table;

		rc = _target_ca.db_api->ExecuteNonQuery(command.c_str(), &time_spent);

		if(_callback != NULL)
		{
			reply._cmd_subtype = SQLDATA_CMD_TRUNCATE;
			reply.rc = rc;
			reply._int1 = (int)time_spent;
			reply.t_sql = command.c_str();

			if(rc != 0)
			{
				reply.t_error = _target_ca.db_api->GetError();
				strcpy(reply.t_native_error_text, _target_ca.db_api->GetNativeErrorText());
			}

			_callback(_callback_object, &reply);
		}
	}

	// Check for necessity to create table
	if(options & SQLDATA_OPT_CREATE)
	{
		std::string command;

		// Generate SQL CREATE TABLE statement
		GenerateCreateTable(s_cols, s_table, t_table, (int)col_count, command);

		rc = _target_ca.db_api->ExecuteNonQuery(command.c_str(), &time_spent);

		if(_callback != NULL)
		{
			reply._cmd_subtype = SQLDATA_CMD_CREATE;
			reply.rc = rc;
			reply._int1 = (int)time_spent;
			reply.t_sql = command.c_str();

			if(rc != 0)
			{
				reply.t_error = _target_ca.db_api->GetError();
				strcpy(reply.t_native_error_text, _target_ca.db_api->GetNativeErrorText());
			}

			_callback(_callback_object, &reply);
		}
	}

	return rc;
}

// Generate SQL CREATE TABLE statement
int SqlDb::GenerateCreateTable(SqlCol *s_cols, const char *s_table, const char *t_table, int col_count, std::string &sql)
{
	if(s_table == NULL || t_table == NULL || s_cols == NULL)
		return -1;

	sql = "CREATE TABLE ";
	sql += t_table; 
	sql += "\n(\n   "; 

	char int1[12], int2[12];

	for(int i = 0; i < col_count; i++)
	{
		if(i > 0)
			sql += ",\n   ";

		std::string colname;
		std::string coltype;

		// Get the target column name
		MapColumn(s_table, s_cols[i]._name, colname, coltype);

		sql += colname;
		sql += ' ';

		strcpy(s_cols[i]._t_name, colname.c_str());

        // Set data type from global data type mapping
        if(coltype.empty() == true && s_cols[i]._t_datatype_name.empty() == false)
            coltype = s_cols[i]._t_datatype_name;

		std::string identity_clause;

		// Get IDENTITY clause to be added to column definition, it may affect the target data type conversion
		GetInlineIdentityClause(s_table, s_cols[i]._name, identity_clause);

		// Data type is set by the column name mapping
		if(coltype.empty() == false)
			sql += coltype;
		else
		// Oracle VARCHAR2 (SQLT_CHR) ODBC SQL_VARCHAR 
		if((source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_CHR) ||
			// Sybase ASE CHAR and VARCHAR (CS_CHAR_TYPE for VARCHAR <= 255, and CS_LONGCHAR_TYPE for VARCHAR < 32K)
			(source_type == SQLDATA_SYBASE && (s_cols[i]._native_dt == CS_CHAR_TYPE || s_cols[i]._native_dt == CS_LONGCHAR_TYPE)) ||			
			// MySQL VARCHAR
		   (source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_VAR_STRING) ||
			// SQL Server, DB2, Informix, Sybase ASA VARCHAR
		   ((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || 
				source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA || source_type == SQLDATA_ODBC) 
					&& s_cols[i]._native_dt == SQL_VARCHAR))
		{
			Str::IntToString((int)s_cols[i]._len, int1);

			// VARCHAR2 or CLOB in Oracle
			if(target_type == SQLDATA_SQL_SERVER)
			{
				if(s_cols[i]._len <= 8000)
				{
					sql += "VARCHAR(";
					sql += int1;
					sql += ")";
				}
				else
					sql += "VARCHAR(max)";
			}
			else
			// VARCHAR2 or CLOB in Oracle
			if(target_type == SQLDATA_ORACLE)
			{
				if(s_cols[i]._len <= 4000)
				{
					sql += "VARCHAR2(";
					sql += int1;
					sql += " CHAR)";
				}
				else
					sql += "CLOB";
			}
            else
			if(target_type == SQLDATA_MYSQL)
			{
                // Can be SQL Server VARCHAR(max), so check the length
                if(s_cols[i]._len <= 8000)
                {
                    sql += "VARCHAR(";
				    sql += int1;
				    sql += ")";
                }
                else
                    sql += "LONGTEXT";
    		}
			else
			if(target_type == SQLDATA_POSTGRESQL)
			{
				sql += "TEXT";
			}
			else
			{
				sql += "VARCHAR(";
				sql += int1;
				sql += ")";
			}
		}
		else
		// Oracle CHAR
		if((source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_AFC) ||
			// SQL Server CHAR
			(source_type == SQLDATA_SQL_SERVER && s_cols[i]._native_dt == SQL_CHAR) ||
			// Informix CHAR
			(source_type == SQLDATA_INFORMIX && s_cols[i]._native_dt == SQL_CHAR) ||
			// DB2 CHAR
			(source_type == SQLDATA_DB2 && s_cols[i]._native_dt == SQL_CHAR) ||
			// ODBC CHAR
			(source_type == SQLDATA_ODBC && s_cols[i]._native_dt == SQL_CHAR) ||
			// MySQL CHAR
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_STRING))
		{
			int len = (int)s_cols[i]._len;
            Str::IntToString(len, int1);

			// MySQL allows zero length for CHAR columns
			if(len == 0 && target_type != SQLDATA_MYSQL)
				len = 1;

            // MySQL allows max size 255 for CHAR
            if(target_type == SQLDATA_MYSQL)
			{
                if(s_cols[i]._len <= 255)
                {
                    sql += "CHAR(";
				    sql += int1;
				    sql += ")";
                }
                else
                    sql += "TEXT";
    		}
			else
			// SQL Server allows max size 8000 for CHAR (while Informix allows 32,767)
            if(target_type == SQLDATA_SQL_SERVER)
			{
                if(s_cols[i]._len <= 8000)
                {
                    sql += "CHAR(";
				    sql += int1;
				    sql += ")";
                }
                else
                    sql += "VARCHAR(max)";
    		}
            else
            {
                sql += "CHAR(";
			    sql += int1;

			    // Add CHAR length semantic for Oracle as many databases specify size in characters,
			    // while BYTE is default in Oracle
			    if(target_type == SQLDATA_ORACLE)
				    sql += " CHAR";

			    sql += ")";
            }
		}
		else
		// SQL Server, Sybase ASA NCHAR
		if(((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_ASA) && 
			s_cols[i]._native_dt == SQL_WCHAR) ||
			// DB2 GRAPHIC with code -95
			(source_type == SQLDATA_DB2 && s_cols[i]._native_dt == -95))
		{
			// Length in characters for source Sybase ASA
			int len = (int)s_cols[i]._len;
			Str::IntToString((int)s_cols[i]._len, int1);

			// NCHAR in SQL Server
			if(target_type == SQLDATA_SQL_SERVER)
			{
				if(len <= 4000)
				{
					sql += "NCHAR(";
					sql += int1;
					sql += ")";
				}
				else
					sql += "NVARCHAR(max)";
			}
            else
            // MySQL allows max size 255 for NCHAR
            if(target_type == SQLDATA_MYSQL)
			{
                if(s_cols[i]._len <= 255)
                {
                    sql += "NCHAR(";
				    sql += int1;
				    sql += ")";
                }
                else
                    sql += "TEXT";
    		}
			else
			if(target_type == SQLDATA_POSTGRESQL)
			{
				sql += "TEXT";
			}
			else
			{
				sql += "NCHAR(";
				sql += int1;
				sql += ")";
			}
		}
		else
		// SQL Server, Sybase ASA NVARCHAR
		if(((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_ASA) && 
			s_cols[i]._native_dt == SQL_WVARCHAR) ||
			// DB2 VARGRAPHIC with code -96
			(source_type == SQLDATA_DB2 && s_cols[i]._native_dt == -96))
		{
			// Length in characters for source Sybase ASA
			int len = (int)s_cols[i]._len;
			Str::IntToString((int)s_cols[i]._len, int1);

			if(target_type == SQLDATA_ORACLE)
			{
				sql += "NVARCHAR2(";
				sql += int1;
				sql += ")";
			}
			else
			// NVARCHAR in SQL Server
			if(target_type == SQLDATA_SQL_SERVER)
			{
				if(len <= 4000)
				{
					sql += "NVARCHAR(";
					sql += int1;
					sql += ")";
				}
				else
					sql += "NVARCHAR(max)";
			}
            else
			if(target_type == SQLDATA_MYSQL)
			{
                // Can be SQL Server NVARCHAR(max), so check the length
                if(s_cols[i]._len <= 4000)
                {
                    sql += "NVARCHAR(";
				    sql += int1;
				    sql += ")";
                }
                else
                    sql += "LONGTEXT";
    		}
			else
			if(target_type == SQLDATA_POSTGRESQL)
			{
				sql += "TEXT";
			}
			else
			{
				sql += "NVARCHAR(";
				sql += int1;
				sql += ")";
			}
		}
		else
		// Oracle NUMBER as INTEGER 
		if((source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_NUM && s_cols[i]._precision != 0 && 
				s_cols[i]._precision < 10 && s_cols[i]._scale == 0) ||
			// Oracle NUMBER as INTEGER through ODBC
		    (source_type == SQLDATA_ODBC && source_subtype == SQLDATA_ORACLE && s_cols[i]._native_dt == SQL_DECIMAL &&
				s_cols[i]._precision < 10 && s_cols[i]._scale == 0) ||
			// SQL Server, DB2, Informix, Sybase ASA INTEGER
			((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || 
				source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA) 
							&& s_cols[i]._native_dt == SQL_INTEGER) ||
			// Sybase ASE INT
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_INT_TYPE) ||
			// MySQL INT
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_LONG))

		{
			if(target_type != SQLDATA_ORACLE)
				sql += "INTEGER";
			else
				sql += "NUMBER(10)";
		}
		else
		// SQL Server, DB2, Informix, Sybase ASA BIGINT (SQL_BIGINT -5)
		if(((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || 
				source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA) && 
				s_cols[i]._native_dt == SQL_BIGINT) ||
			// MySQL BIGINT
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_LONGLONG) ||
			// Sybase ASE BIGINT
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_BIGINT_TYPE) ||
			// Informix SQL_INFX_BIGINT (-114)
			(source_type == SQLDATA_INFORMIX && s_cols[i]._native_dt == -114))
		{
			if(target_type == SQLDATA_ORACLE)
				sql += "NUMBER(19)";
			else
				sql += "BIGINT";
		}
		else
		// SMALLINT
		if((source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_SMALLINT_TYPE) ||
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_SHORT) ||
			((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || 
				source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA) 
					&& s_cols[i]._native_dt == SQL_SMALLINT))
		{
			if(target_type != SQLDATA_ORACLE)
				sql += "SMALLINT";
			else
				sql += "NUMBER(5)";
		}
		else
		// SQL Server, MySQL, Sybase ASA TINYINT 
		if(((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_ASA) && 
				s_cols[i]._native_dt == SQL_TINYINT) ||
			// Sybase TINYINT
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_TINYINT_TYPE) ||			
			// MySQL TINYINT
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_TINY))
		{
			if(target_type == SQLDATA_ORACLE)
				sql += "NUMBER(3)";
			else
			if(target_type == SQLDATA_SQL_SERVER || target_type == SQLDATA_MYSQL)
				sql += "TINYINT";
			else
				sql += "INTEGER";
		}
		else
		// Oracle NUMBER without parameters
		if(source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_NUM && 
			((s_cols[i]._precision == 0 && s_cols[i]._scale == 129) || s_cols[i]._precision >= 38)) 
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "FLOAT";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "NUMBER";
			else
			if(target_type == SQLDATA_MYSQL)
				sql += "DOUBLE";
			else
				sql += "DOUBLE PRECISION";
		}
		else
		// MySQL DOUBLE
		if((source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_DOUBLE) ||
			// SQL Server, DB2, ODBC FLOAT
			((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || source_type == SQLDATA_ODBC) && 
				s_cols[i]._native_dt == SQL_FLOAT))
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "FLOAT";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "NUMBER";
			else
			if(target_type == SQLDATA_POSTGRESQL)
				sql += "DOUBLE PRECISION";
			else
				sql += "DOUBLE";
		}
		else
		// SQL Server, DB2, ODBC DOUBLE
		if((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || source_type == SQLDATA_ODBC) && 
				s_cols[i]._native_dt == SQL_DOUBLE)
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "FLOAT";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "NUMBER";
			else
			if(target_type == SQLDATA_POSTGRESQL)
				sql += "DOUBLE PRECISION";
			else
				sql += "DOUBLE";
		}
		else
		// Informix single-precision floating point number SMALLFLOAT, REAL
		if((source_type == SQLDATA_INFORMIX && s_cols[i]._native_dt == SQL_REAL) ||
			// DB2 FLOAT, SQL Server REAL, Sybase ASA single-precision FLOAT
			((source_type == SQLDATA_DB2 || source_type == SQLDATA_SQL_SERVER || 
				source_type == SQLDATA_ASA) && 
				s_cols[i]._native_dt == SQL_REAL))
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "FLOAT";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "NUMBER";
			else
				sql += "FLOAT";
		}
		else
		// Informix double-precision floating point number FLOAT, DOUBLE PRECISION
		// Sybase ASA DOUBLE
		if((source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA) && 
			s_cols[i]._native_dt == SQL_DOUBLE)
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "FLOAT";
			else
		    if(target_type == SQLDATA_ORACLE)
				sql += "NUMBER";
			else
				sql += "DOUBLE PRECISION";
		}
		else
		// MySQL FLOAT (4-byte) 
		if((source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_FLOAT) ||
			// Sybase ASE FLOAT
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_FLOAT_TYPE))
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "REAL";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "NUMBER";
			else
				sql += "FLOAT";
		}
		else
		// DB2 DECFLOAT(16 | 34)
		if(source_type == SQLDATA_DB2 && s_cols[i]._native_dt == -360)
		{
			sql += "DOUBLE";
		}
		else
		// Oracle NUMBER as DECIMAL
		if((source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_NUM ||
			source_type == SQLDATA_ODBC && source_subtype == SQLDATA_ORACLE && s_cols[i]._native_dt == SQL_DECIMAL) &&
			((s_cols[i]._precision >= 10 && s_cols[i]._precision <= 38) || s_cols[i]._scale != 0) ||
			// Sybase ASE NUMERIC and DECIMAL
			(source_type == SQLDATA_SYBASE && (s_cols[i]._native_dt == CS_NUMERIC_TYPE || s_cols[i]._native_dt == CS_DECIMAL_TYPE)) ||
			// SQL Server, DB2, Informix, Sybase ASA DECIMAL/NUMERIC
			((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || 
				source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA) && 
				(s_cols[i]._native_dt == SQL_DECIMAL || s_cols[i]._native_dt == SQL_NUMERIC)) ||
			// MySQL DECIMAL or NUMERIC
			(source_type == SQLDATA_MYSQL && 
				(s_cols[i]._native_dt == MYSQL_TYPE_NEWDECIMAL || s_cols[i]._native_dt == MYSQL_TYPE_DECIMAL)))
		{
			// Sybase ASE allows NUMERIC/DECIMAL(x,0) for identity while MySQL/MariaDB allows only integers
			if(source_type == SQLDATA_SYBASE && target_type == SQLDATA_MYSQL && s_cols[i]._scale == 0 && !identity_clause.empty())
			{
				if(s_cols[i]._precision < 9)
					sql += "INTEGER";
				else
					sql += "BIGINTEGER";
			}
			else
			{
				if(target_type == SQLDATA_ORACLE)
					sql += "NUMBER(";
				else
					sql += "DECIMAL(";
			
				sql += Str::IntToString(s_cols[i]._precision, int1);
				sql += ",";
				sql += Str::IntToString(s_cols[i]._scale, int2);
				sql += ")";
			}
		}
		else
		// Sybase ASE MONEY
		if(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_MONEY_TYPE)
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "MONEY";
			else
			{
				sql += "DECIMAL(";
			
				sql += Str::IntToString(s_cols[i]._precision, int1);
				sql += ",";
				sql += Str::IntToString(s_cols[i]._scale, int2);
				sql += ")";
			}
		}
		else
		// Sybase ASE SMALLMONEY
		if(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_MONEY4_TYPE)
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "SMALLMONEY";
			else
			{
				sql += "DECIMAL(";
			
				sql += Str::IntToString(s_cols[i]._precision, int1);
				sql += ",";
				sql += Str::IntToString(s_cols[i]._scale, int2);
				sql += ")";
			}
		}
		else
		// Oracle DATE 
		if((source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_DAT) ||
			// MySQL DATETIME
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_DATETIME))
		{
			if(target_type == SQLDATA_DB2 || target_type == SQLDATA_POSTGRESQL)
				sql += "TIMESTAMP";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "DATE";
			else
				sql += "DATETIME";
		}
		else
		// Oracle TIMESTAMP (SQLT_TIMESTAMP) 
		if((source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_TIMESTAMP) ||
			// Sybase ASE DATETIME
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_DATETIME_TYPE) || 
			// SQL Server, DB2, Informix, Sybase ASA, ODBC DATETIME
		   ((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || 
				source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA || 
				source_type == SQLDATA_ODBC) && 
					s_cols[i]._native_dt == SQL_TYPE_TIMESTAMP))
		{
			char fraction[11]; 

			// Define fractional seconds precision
			sprintf(fraction, "%d", s_cols[i]._scale);

			if(target_type == SQLDATA_SQL_SERVER)
				sql += "DATETIME2";
			else
			if(target_type == SQLDATA_MYSQL)
			{
				sql += "DATETIME(";

				// In DB2 fraction can be up to 12, in some other databases up to 9
				// while MySQL and MariaDB support up to 6
				if(s_cols[i]._scale <= 6)
					sql += fraction;
				else
					sql += "6";
				sql += ')';
			}
			else
			// Oracle TIMESTAMP
			if(target_type == SQLDATA_ORACLE)
			{
				sql += "TIMESTAMP(";
				sql += fraction;
				sql += ')';
			}
			else
				sql += "TIMESTAMP";
		}
		else
		// Oracle TIMESTAMP WITH TIME ZONE 
		if(source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_TIMESTAMP_TZ)
		{
			char fraction[11]; 

			// Define fractional seconds precision
			sprintf(fraction, "%d", s_cols[i]._scale);

			if(target_type == SQLDATA_SQL_SERVER)
			{
				sql += "DATETIMEOFFSET(";
				sql += fraction;
				sql += ')';
			}
			else
			if(target_type == SQLDATA_ORACLE)
			{
				sql += "TIMESTAMP(";
				sql += fraction;
				sql += ')';
				sql += " WITH TIME ZONE";
			}
			else
			if(target_type == SQLDATA_MYSQL)
				sql += "DATETIME";
		}
		else
		// ODBC, SQL Server, DB2, Informix, Sybase ASA TIME
		if(((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || 
			source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA ||
			source_type == SQLDATA_ODBC) && s_cols[i]._native_dt == SQL_TYPE_TIME) ||
			(source_type == SQLDATA_SQL_SERVER && s_cols[i]._native_dt == SQL_SS_TIME2) ||
			// Sybase ASE TIME
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_TIME_TYPE))
		{
			char fraction[11]; 

			// Define fractional seconds precision
			sprintf(fraction, "%d", s_cols[i]._scale);

			if(target_type == SQLDATA_SQL_SERVER)
			{
				sql += "TIME(";
				sql += fraction;
				sql += ')';
			}
			else
			// Oracle TIMESTAMP
			if(target_type == SQLDATA_ORACLE)
			{
				sql += "TIMESTAMP(";
				sql += fraction;
				sql += ')';
			}
			else
				sql += "TIME";
		}
		else
		// Oracle INTERVAL DAY TO SECOND 
		if(source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_INTERVAL_DS)
		{
			char fraction[11]; 

			// Define fractional seconds precision
			sprintf(fraction, "%d", s_cols[i]._scale);

			if(target_type == SQLDATA_SQL_SERVER || target_type == SQLDATA_MYSQL)
				sql += "VARCHAR(30)";
			else
			if(target_type == SQLDATA_ORACLE)
			{
				sql += "INTERVAL DAY TO SECOND(";
				sql += fraction;
				sql += ')';
			}
		}
		else
		// SQL Server, DB2, Informix, Sybase ASA DATE 
		if(((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || 
				source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA) && 
				s_cols[i]._native_dt == SQL_TYPE_DATE) ||
			// MySQL DATE
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_DATE) ||
			// Sybase ASE DATE
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_DATE_TYPE))
		{
			sql += "DATE";
		}
		else
		// Sybase ASE SMALLDATETIME
		if(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_DATETIME4_TYPE)
		{
			if(target_type == SQLDATA_MYSQL)
				sql += "DATETIME";
			else
				sql += "DATETIME";
		}
        else
		// SQL Server BINARY
		if((source_type == SQLDATA_SQL_SERVER && s_cols[i]._native_dt == SQL_BINARY) ||
			// Sybase ASE BINARY
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_BINARY_TYPE)) 
		{
			if(target_type == SQLDATA_ORACLE)
			{
				sql += "RAW(";
				sql += Str::IntToString((int)s_cols[i]._len, int1);
				sql += ")";
			}
			else
			if(target_type == SQLDATA_MYSQL)
			{
				sql += "BINARY(";
				sql += Str::IntToString((int)s_cols[i]._len, int1);
				sql += ")";
			}
		}
		else
		// Oracle RAW
		if((source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_BIN) ||
			// SQL Server VARBINARY, DB2 VARCHAR FOR BIT DATA
			((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2) 
				&& s_cols[i]._native_dt == SQL_VARBINARY &&
			 s_cols[i]._lob == false) || 
			// Sybase ASA BINARY is variable-length (!) data type			
			(source_type == SQLDATA_ASA && s_cols[i]._native_dt == SQL_BINARY))
		{
           	int len = (int)s_cols[i]._len;
			Str::IntToString(len, int1);

            // VARBINARY(n | MAX) in SQL Server
			if(target_type == SQLDATA_SQL_SERVER)
			{
                sql += "VARBINARY(";

				if(len <= 8000)
					sql += int1;
                else
                    sql += "MAX";

				sql += ")";
			}
            else
            if(target_type == SQLDATA_MYSQL)
			{
                // Can be SQL Server VARBINARY(max) fetched as non-LOB, so check the length
                if(s_cols[i]._len <= 8000)
                {
                    sql += "VARBINARY(";
				    sql += int1;
				    sql += ")";
                }
                else
                    sql += "LONGBLOB";
    		}
            else
			if(target_type == SQLDATA_POSTGRESQL)
			{
				sql += "BYTEA";
			}
			else
			{
				sql += "VARBINARY(";
				sql += int1;
				sql += ")";
			}
		}
		else
		// Oracle CLOB, LONG
		if((source_type == SQLDATA_ORACLE && 
			(s_cols[i]._native_dt == SQLT_CLOB || s_cols[i]._native_dt == SQLT_LNG)) ||
			// Sybase ASE TEXT
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_TEXT_TYPE) ||
			// Informix TEXT, Sybase ASA LONG VARCHAR
			((source_type == SQLDATA_INFORMIX || source_type == SQLDATA_ASA || source_type == SQLDATA_ODBC) 
				&& s_cols[i]._native_dt == SQL_LONGVARCHAR) ||
			// DB2 CLOB with code -99, LONG VARCHAR with code -1
			(source_type == SQLDATA_DB2 && (s_cols[i]._native_dt == -99 || s_cols[i]._native_dt == -1)) ||
			// Informix CLOB with code -103
			(source_type == SQLDATA_INFORMIX && s_cols[i]._native_dt == -103) ||
			// MySQL CLOB
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_BLOB && s_cols[i]._binary == false))
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "VARCHAR(MAX)";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "CLOB";
			else
			if(target_type == SQLDATA_MYSQL)
				sql += "LONGTEXT";
			else
			if(target_type == SQLDATA_POSTGRESQL)
				sql += "TEXT";
		}
		else
		// SQL Server NTEXT, Sybase ASA LONG NVARCHAR
		if(((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_INFORMIX || 
			source_type == SQLDATA_ASA || source_type == SQLDATA_ODBC) 
			&& s_cols[i]._native_dt == SQL_WLONGVARCHAR) ||
			// Sybase ASE UNITEXT
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_UNITEXT_TYPE))
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "NVARCHAR(MAX)";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "NCLOB";
			else
			if(target_type == SQLDATA_MYSQL)
				sql += "LONGTEXT";
			else
			if(target_type == SQLDATA_POSTGRESQL)
				sql += "TEXT";
		}
		else
		// Oracle BLOB, ODBC BLOB
		if((source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_BLOB) ||
			// SQL Server IMAGE, Informix BYTE, Sybase ASA LONG BINARY
			((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_DB2 || source_type == SQLDATA_INFORMIX || 
				source_type == SQLDATA_ASA || source_type == SQLDATA_ODBC)
				&& s_cols[i]._native_dt == SQL_LONGVARBINARY) ||
		    // SQL Server VARBINARY(max)
			(source_type == SQLDATA_SQL_SERVER && s_cols[i]._native_dt == SQL_VARBINARY &&
			 s_cols[i]._lob == true) ||
			// DB2 BLOB with code -98
			(source_type == SQLDATA_DB2 && s_cols[i]._native_dt == -98) ||
			// Informix BLOB with code -102
			(source_type == SQLDATA_INFORMIX && s_cols[i]._native_dt == -102) ||
			// MySQL BLOB
			(source_type == SQLDATA_MYSQL && s_cols[i]._native_dt == MYSQL_TYPE_BLOB && s_cols[i]._binary == true))
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "VARBINARY(MAX)";
			else
			if(target_type == SQLDATA_ORACLE)
				sql += "BLOB";
			else
			if(target_type == SQLDATA_MYSQL)
				sql += "LONGBLOB";
			else
			if(target_type == SQLDATA_POSTGRESQL)
				sql += "BYTEA";
		}
		else
		// SQL Server XML
		if((source_type == SQLDATA_SQL_SERVER && s_cols[i]._native_dt == SQL_SS_XML) ||
		  // DB2 XML with code -370
		  (source_type == SQLDATA_DB2 && s_cols[i]._native_dt == -370))
		{
			if(target_type == SQLDATA_ORACLE)
				sql += "XMLTYPE";
            else
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "XML";
            else
            if(target_type == SQLDATA_MYSQL)
                sql += "LONGTEXT";
			else
			if(target_type == SQLDATA_POSTGRESQL)
				sql += "TEXT";
			else
				sql += "XML";
		}
		else
		// SQL Server, Sybase ASA BIT, Informix BOOLEAN (SQL_BIT -7)
		if(((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_INFORMIX || 
			source_type == SQLDATA_ASA) && 
			s_cols[i]._native_dt == SQL_BIT) ||
			// Sybase ASE BIT
			(source_type == SQLDATA_SYBASE && s_cols[i]._native_dt == CS_BIT_TYPE))
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "BIT";
			else
				sql += "CHAR(1)";
		}
		else
		// Sybase ASA UNIQUEIDENTIFIER (SQL_GUID -11)
		if((source_type == SQLDATA_SQL_SERVER || source_type == SQLDATA_ASA) && 
			s_cols[i]._native_dt == SQL_GUID)
		{
			if(target_type == SQLDATA_SQL_SERVER)
				sql += "UNIQUEIDENTIFIER";
			else
				sql += "CHAR(36)";
		}
		else
		// Oracle ROWID 
		if(source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_RDD)
		{
			if(target_type != SQLDATA_ORACLE)
				sql += "CHAR(18)";
			else
				sql += "ROWID";
		}
		else
		// Oracle object type
		if(source_type == SQLDATA_ORACLE && s_cols[i]._native_dt == SQLT_NTY)
		{
			Str::IntToString((int)s_cols[i]._len, int1);
			
			sql += "VARCHAR(";
			sql += int1;
			sql += ")";
		}

		std::string default_clause;

		// Get DEFAULT if it is a literal (MySQL i.e. requires full column specification in ALTER TABLE)
		GetDefaultClause(s_table, s_cols[i]._name, default_clause);

		if(source_type == SQLDATA_DB2 && target_type == SQLDATA_MYSQL && default_clause.empty() == false)
		{
			sql += " DEFAULT ";
			sql += default_clause;
		}

		// Check NOT NULL attribute
		if(s_cols[i]._nullable == false || IsPrimaryKeyColumn(s_table, s_cols[i]._name))
			sql += " NOT NULL";

		if(identity_clause.empty() == false)
		{
			sql += " ";
			sql += identity_clause;
		}

		// For MySQL add column comment as a column property
		if(target_type == SQLDATA_MYSQL)
		{
			std::string comment;
			
			_source_ca.db_api->GetComment(s_table, s_cols[i]._name, comment);

			// A comment found
			if(comment.empty() == false)
			{
				std::string quoted_comment;

				// Single quotes in comment must be duplicated
				Str::Quote(comment, quoted_comment);

				sql += " COMMENT ";
				sql += quoted_comment;
			}
		}
	}

	sql += "\n)"; 

	// For MySQL add table comment, character set etc. as a part of CREATE TABLE statement
	if(target_type == SQLDATA_MYSQL)
		AddMySQLTableOptions(s_table, sql);

	return 0;
}

// Add table options when target is MySQL
void SqlDb::AddMySQLTableOptions(const char *s_table, std::string &sql)
{
	// Add table comment as a part of CREATE TABLE statement
	std::string comment;
	_source_ca.db_api->GetComment(s_table, NULL, comment);

	// A comment found
	if(comment.empty() == false)
	{
		std::string quoted_comment;
		// Single quotes in comment must be duplicated
		Str::Quote(comment, quoted_comment);

		sql += "\n  COMMENT ";
		sql += quoted_comment;
	}

	// Character set
	const char *value = _parameters->Get("-mysql_set_character_set");

	if(Str::IsSet(value))
	{
		sql += "\n  CHARACTER SET ";
		sql += value;
	}

	// Collation
	value = _parameters->Get("-mysql_set_collation");

	if(Str::IsSet(value))
	{
		sql += "\n  COLLATE ";
		sql += value;
	}
}

// Get DEFAULT if it is a literal (MySQL i.e. requires full column specification in ALTER TABLE)
int	SqlDb::GetDefaultClause(const char *s_table, char *column, std::string &default_clause)
{
	if(s_table == NULL || column == NULL)
		return -1;

	if(target_type != SQLDATA_MYSQL)
		return 0;

	std::string def_clause;

	if(_metaSqlDb != NULL)
		_metaSqlDb->_source_ca.db_api->GetColumnDefault(s_table, column, def_clause);

	// MySQL allows only string literals, numbers and some functions
	if(def_clause.empty() == false)
	{
		const char *d = def_clause.c_str();

		if(*d == '\'' || (*d >= '0' && *d <= '9') || !_stricmp(d, "CURRENT_TIMESTAMP"))
			default_clause = def_clause;
		else 
		if(!_stricmp(d, "CURRENT TIMESTAMP"))
			default_clause = "CURRENT_TIMESTAMP";
	}

	return 0;
}

// Get IDENTITY clause to be added to column definition
int SqlDb::GetInlineIdentityClause(const char *s_table, char *column, std::string &identity_clause)
{
	if(s_table == NULL || column == NULL)
		return -1;

	if(target_type != SQLDATA_SQL_SERVER && target_type != SQLDATA_MYSQL)
		return 0;

	std::list<SqlColMeta> *table_columns = _metaSqlDb->GetTableColumns(SQLDB_SOURCE_ONLY);

	if(table_columns == NULL)
		return 0;

	std::string schema;
	std::string table;

	SqlApiBase::SplitQualifiedName(s_table, schema, table);

	// Find mapping record for this table and column
	for(std::list<SqlColMeta>::iterator i = table_columns->begin(); i != table_columns->end(); i++)
	{
		// Not an identity column
		if((*i).identity == false)
			continue;

		char *col = (*i).column;
		char *tab = (*i).table;
		char *sch = (*i).schema;

		if(col == NULL || tab == NULL || sch == NULL)
			continue;

		if(_stricmp(col, column) == 0 && _stricmp(tab, table.c_str()) == 0 &&
			_stricmp(sch, schema.c_str()) == 0)
		{
			char int1[21];

			if(target_type == SQLDATA_SQL_SERVER)
			{
				identity_clause += "IDENTITY(";
				identity_clause += Str::IntToString((*i).id_next, int1);
				identity_clause += ",";
				identity_clause += Str::IntToString((*i).id_inc, int1);
				identity_clause += ")";
			}
			else
			// MySQL and MariaDB require auto_increment to be a key
			if(target_type == SQLDATA_MYSQL)
				identity_clause += "AUTO_INCREMENT PRIMARY KEY";
				
			break;
		}
	}
		
	return 0;
}

// Check if a column belongs to primary key
bool SqlDb::IsPrimaryKeyColumn(const char *s_table, const char *column)
{
    if(s_table == NULL || column == NULL)
        return false;

	std::list<SqlColMeta> *table_columns = _metaSqlDb->GetTableColumns(SQLDB_SOURCE_ONLY);

	if(table_columns == NULL)
		return 0;

	std::string schema;
	std::string table;

	SqlApiBase::SplitQualifiedName(s_table, schema, table);

	// Find mapping record for this table and column
	for(std::list<SqlColMeta>::iterator i = table_columns->begin(); i != table_columns->end(); i++)
	{
		// Not a primary key column
        if((*i).pk_column == false)
			continue;

		char *col = (*i).column;
		char *tab = (*i).table;
		char *sch = (*i).schema;

		if(col == NULL || tab == NULL || sch == NULL)
			continue;

		if(_stricmp(col, column) == 0 && _stricmp(tab, table.c_str()) == 0 &&
			_stricmp(sch, schema.c_str()) == 0)
            return (*i).pk_column;
	}

    return false;
}

// Get the target column name and type
void SqlDb::MapColumn(const char *s_table, const char *s_name, std::string &t_name, std::string &t_type)
{
	if(s_table == NULL || s_name == NULL)
		return;

	bool name_mapped = false;

	// Check is column name mapping is set
	if(_column_map != NULL && _column_map->size() > 0)
	{
		std::string schema;
		std::string table;

		SqlApiBase::SplitQualifiedName(s_table, schema, table);

		// Find mapping record for this table and column
		for(std::list<SqlColMap>::iterator i = _column_map->begin(); i != _column_map->end(); i++)
		{
			if(_stricmp((*i).schema.c_str(), schema.c_str()) == 0 && 
				_stricmp((*i).table.c_str(), table.c_str()) == 0 && _stricmp((*i).name.c_str(), s_name) == 0)
			{
				// Target column name is optional
				if((*i).t_name.empty() == false)
				{
					t_name = (*i).t_name;
					name_mapped = true;
				}

				// Target data type is optional
				t_type = (*i).t_type;			

				break;
			}
		}
	}

	if(name_mapped)
		return;

	// Enclose column names to prevent reserved word conflicts
	if(target_type == SQLDATA_SQL_SERVER)
	{
		t_name += '[';
		t_name += s_name;
		t_name += "]";
	}
	else
	if(target_type == SQLDATA_ORACLE)
	{
		// Check for a reserved word in the target database
		bool resword = _target_ca.db_api->IsReservedWord(s_name);

		// Check for space (Sybase ASE, ASA i.e. return unqouted identifier when space inside)
		bool space = strchr(s_name, ' ') != NULL ? true : false;

		if(resword || space)
			t_name += '"';

		t_name += s_name;

		if(resword || space)
			t_name += '"';

		// The length must not exceed 30 characters
		if(resword == false)
		{
			size_t len = t_name.size();

			if(len > 30)
			{
				std::string trimmed;
				
				Str::TrimToAbbr(t_name, trimmed, 30);
				t_name = trimmed;
			}
		}
	}
	else
	if(target_type == SQLDATA_POSTGRESQL)
	{
		if(IsSpecialIdentifier(s_name) == true)
		{
			t_name += '"';
			t_name += s_name;
			t_name += "\"";
		}
		else
			t_name += s_name;
	}
	else
	if(target_type == SQLDATA_MYSQL)
	{
		t_name += '`';
		t_name += s_name;
		t_name += "`";
	}
	else
		t_name += s_name;
}

// Check is the identifier contains a special character such as a space, dot etc.
bool SqlDb::IsSpecialIdentifier(const char *name)
{
	if(name == NULL)
		return false;

	if(strchr(name, ' ') || strchr(name, '.'))
		return true;

	return false;
}

// Copy column definitions and buffers
int SqlDb::CopyColumns(SqlCol *cols, SqlCol **cols_copy, size_t col_count, size_t allocated_array_rows)
{
	if(cols == NULL || cols_copy == NULL || col_count <= 0 || allocated_array_rows <= 0)
		return -1;

	SqlCol *cols_c = new SqlCol[col_count];
		
	for(int i = 0; i < col_count; i++)
	{
		// Copy column metadata
		cols_c[i] = cols[i];

		// Allocate copy buffers
		cols_c[i]._data = new char[cols_c[i]._fetch_len * allocated_array_rows];

		if(cols[i]._ind2 != NULL)
			cols_c[i]._ind2 = new short[allocated_array_rows];

		if(cols[i].ind != NULL)
			cols_c[i].ind = new size_t[allocated_array_rows];

		if(cols[i]._len_ind2 != NULL)
			cols_c[i]._len_ind2 = new short[allocated_array_rows];

		if(cols[i]._len_ind4 != NULL)
			cols_c[i]._len_ind4 = new int[allocated_array_rows];
	}

	*cols_copy = cols_c;

	return 0;
}

// Copy data
int	SqlDb::CopyColumnData(SqlCol *s_cols, SqlCol *t_cols, size_t col_count, int rows_fetched)
{
	if(s_cols == NULL || t_cols == NULL || col_count <= 0 || rows_fetched <= 0)
		return -1;

	for(int i = 0; i < col_count; i++)
	{
		// Copy data
		memcpy(t_cols[i]._data, s_cols[i]._data, s_cols[i]._fetch_len * rows_fetched);

		if(s_cols[i]._ind2 != NULL)
			memcpy(t_cols[i]._ind2, s_cols[i]._ind2, sizeof(short) * rows_fetched);

		if(s_cols[i].ind != NULL)
			memcpy(t_cols[i].ind, s_cols[i].ind, sizeof(size_t) * rows_fetched);

		if(s_cols[i]._len_ind2 != NULL)
			memcpy(t_cols[i]._len_ind2, s_cols[i]._len_ind2, sizeof(short) * rows_fetched);

		if(s_cols[i]._len_ind4 != NULL)
			memcpy(t_cols[i]._len_ind4, s_cols[i]._len_ind4, sizeof(int) * rows_fetched);
	}

	return 0;
}

// Validate table row count
int SqlDb::ValidateRowCount(SqlDataReply &reply)
{
	if(_source_ca.db_api == NULL || _target_ca.db_api == NULL)
		return -1;

	// Notify that a table was selected for processing
	if(_callback != NULL)
	{
		reply._cmd_subtype = SQLDATA_CMD_STARTED;
		_callback(_callback_object, &reply);
	}

	_source_ca._name = reply._s_name;
	_target_ca._name = reply._t_name;

	size_t start = GetTickCount();

	// Execute the command
	int rc = Execute(SQLDATA_CMD_VALIDATE_ROW_COUNT);

	// Set results
	reply._cmd_subtype = SQLDATA_CMD_COMPLETE;
	reply.rc = rc;
	reply._s_rc = _source_ca.cmd_rc;
	reply._t_rc = _target_ca.cmd_rc;

	reply._s_int1 = _source_ca._int1;
	reply._t_int1 = _target_ca._int1;

	reply._time_spent = GetTickCount() - start;
	reply._s_time_spent = _source_ca._time_spent;
	reply._t_time_spent = _target_ca._time_spent;

	reply.s_error = _source_ca.error;
	reply.t_error = _target_ca.error;

	return rc;
};

// Validate data in rows
int SqlDb::ValidateRows(SqlDataReply &reply)
{
	if(_metaSqlDb == NULL)
		return -1;

	// Notify that a table was selected for processing
	if(_callback != NULL)
	{
		reply._cmd_subtype = SQLDATA_CMD_STARTED;
		_callback(_callback_object, &reply);
	}

	size_t start = Os::GetTickCount();

	std::string s_select, t_select;

	// SELECT queries with ORDER BY
	_metaSqlDb->BuildQuery(s_select, t_select, reply._s_name, reply._t_name, false);

	_source_ca._void1 = (void*)s_select.c_str();
	_target_ca._void1 = (void*)t_select.c_str();

	// Out SqlCols
	_source_ca._void2 = NULL;
	_target_ca._void2 = NULL;

	// Row and column counts
	_source_ca._int1 = 0;
	_source_ca._int2 = 0;
	_source_ca._int3 = 0;

	_target_ca._int1 = 0;
	_target_ca._int2 = 0;
	_target_ca._int3 = 0;

	// The same number of rows must be fetched
	_source_ca._int4 = 10000;
	_target_ca._int4 = 10000;

	int all_not_equal_rows = 0;
	int not_equal_rows = 0;

	// Open cursors
	int rc = Execute(SQLDATA_CMD_OPEN_CURSOR);

	SqlCol *s_cols = (SqlCol*)_source_ca._void2;
	SqlCol *t_cols = (SqlCol*)_target_ca._void2;

	int s_col_count = _source_ca._int1;
	int t_col_count = _target_ca._int1;

	int s_alloc_rows = _source_ca._int2;
	int t_alloc_rows = _target_ca._int2;

	int s_fetched_rows = _source_ca._int3;
	int t_fetched_rows = _target_ca._int3;

	int s_all_fetched_rows = 0;
	int t_all_fetched_rows = 0;

	__int64 s_all_bytes = 0;
	__int64 t_all_bytes = 0;

	int s_time_spent = (int)_source_ca._time_spent;
	int t_time_spent = (int)_target_ca._time_spent;

	bool more = true;

	// Set trace file name containing possible differences (it will be created only if any difference found)
	if(_trace_diff_data)
		_trace_diff.SetLogfile(std::string(reply._s_name).append("_diff.txt").c_str(), NULL);

	while(more)
	{
		if(rc == -1)
		{
			more = false;
			break;
		}

		int s_bytes = 0, t_bytes = 0;

		// Compare content in the buffers
		not_equal_rows = ValidateCompareRows(s_cols, t_cols, s_col_count, &s_bytes, t_col_count, s_fetched_rows, 
			t_fetched_rows, &t_bytes, s_all_fetched_rows, all_not_equal_rows);

		s_all_fetched_rows += s_fetched_rows;
		t_all_fetched_rows += t_fetched_rows;

		s_all_bytes += s_bytes;
		t_all_bytes += t_bytes;

		if(not_equal_rows != -1)
			all_not_equal_rows += not_equal_rows;

		// Exit if the number of fetched rows is less than allocated rows, or we reached the number of errors
		if((s_fetched_rows < s_alloc_rows || t_fetched_rows < t_alloc_rows) || 
			(_validation_not_equal_max_rows != -1 && all_not_equal_rows >= _validation_not_equal_max_rows))
		{
			more = false;
			break;
		}

		// Fetch the next set of data in 2 concurrent threads
		rc = Execute(SQLDATA_CMD_FETCH_NEXT);

		s_fetched_rows = _source_ca._int1;
		t_fetched_rows = _target_ca._int1;

		s_time_spent += _source_ca._int4;
		t_time_spent += _target_ca._int4;

		// No more row, 0 can be returned if total rows divisible buffer size (10K, 100K, 10M etc.)
		if(s_fetched_rows == 0 || t_fetched_rows == 0)
		{
			more = false;
			break;
		}
	}

	int diff_cols = 0;
	std::string diff_cols_list;

	// Before closing cursors calculate the number of different columns
	for(int i = 0; i < s_col_count; i++)
	{
		if(s_cols != NULL && s_cols[i]._diff_rows > 0)
		{
			if(diff_cols > 0)
				diff_cols_list += ", ";

			diff_cols_list += s_cols[i]._name;
			diff_cols++;
		}
	}

	// Close cursors
	_source_ca.db_api->CloseCursor();
	_target_ca.db_api->CloseCursor();

	// Set results
	reply._cmd_subtype = SQLDATA_CMD_COMPLETE;
	reply.rc = rc;

	reply._s_rc = _source_ca.cmd_rc;
	reply._t_rc = _target_ca.cmd_rc;

	reply.s_error = _source_ca.error;
	reply.t_error = _target_ca.error;

	reply._int1 = all_not_equal_rows;
	reply._int2 = diff_cols;
	reply._int3 = s_col_count;
	reply._s_int1 = s_all_fetched_rows;
	reply._t_int1 = t_all_fetched_rows;

	if(diff_cols > 0)
	{
		strncpy(reply.data2, diff_cols_list.c_str(), 1023);
		reply.data2[1023] = '\x0';
	}

	reply.s_sql_l = s_select;
	reply.t_sql_l = t_select;

	reply._s_bigint1 = s_all_bytes;
	reply._t_bigint1 = t_all_bytes;

	reply._time_spent = Os::GetTickCount() - start;
	reply._s_time_spent = (size_t)s_time_spent;
	reply._t_time_spent = (size_t)t_time_spent;

	if(reply.s_error == -1)
		strcpy(reply.s_native_error_text, _source_ca.db_api->GetNativeErrorText());

	if(reply.t_error == -1)
		strcpy(reply.t_native_error_text, _target_ca.db_api->GetNativeErrorText());

	return rc;
}

// Compare data in rows
int SqlDb::ValidateCompareRows(SqlCol *s_cols, SqlCol *t_cols, int s_col_count, int *s_bytes, int t_col_count, 
									int s_rows, int t_rows, int *t_bytes, int running_rows, int running_not_equal_rows)
{
	if(s_cols == NULL || t_cols == NULL || s_col_count == 0 || t_col_count == 0)
		return -1;

	int not_equal_rows = 0;
	int sb = 0, tb = 0;

	char ora_date_str[19];

	for(int i = 0; i < s_rows; i++)
	{
		// No corresponding target row
		if(i >= t_rows)
			break;

		bool equal_row = true;

		for(int k = 0; k < s_col_count; k++)
		{
			// No corresponding target column
			if(k >= t_col_count)
				break;

			// Length/NULL indicators
			int s_len = GetColumnDataLen(s_cols, i, k, source_type, _source_ca.db_api);
			int t_len = GetColumnDataLen(t_cols, i, k, target_type, _target_ca.db_api);

			char *s_string = NULL, *t_string = NULL;
			char *s_ora_date = NULL, *t_ora_date = NULL;
			
			bool s_int_set = false, t_int_set = false;
			int s_int = 0, t_int = 0;

			SQL_TIMESTAMP_STRUCT *s_ts_struct = NULL, *t_ts_struct = NULL;
	
			// Both NULL values
			if(s_len == -1 && t_len == -1)
				continue;
			else
			// Oracle NULL and non-Oracle empty string
			if((source_type == SQLDATA_ORACLE && s_len == -1 && target_type != SQLDATA_ORACLE && t_len == 0) ||
				(source_type != SQLDATA_ORACLE && s_len == 0 && target_type == SQLDATA_ORACLE && t_len == -1))
				continue;
			else
			// One is NULL, another not NULL
			if((s_len == -1 && t_len != -1) || (s_len != -1 && t_len == -1))
			{
				equal_row = false;
				s_cols[k]._diff_rows++;

				if(_trace_diff_data)
					ValidateDiffDump(&s_cols[k], &t_cols[k], i + running_rows, s_len, t_len, s_string, t_string);

				continue;
			}

			// Both are empty (non-NULL) values
			if(s_len == 0 && t_len == 0)
				continue;

			if(s_len != -1)
				sb += s_len;

			if(t_len != -1)
				tb += t_len;

			int equal = -1;

			// If LOBs perform their comparison first
			bool lob = ValidateCompareLobs(&s_cols[k], s_len, &t_cols[k], t_len, &equal);

			// Get the source and target values
			if(!lob)
			{
				GetColumnData(s_cols, i, k, source_type, _source_ca.db_api, &s_string, &s_int_set, &s_int, &s_ts_struct, &s_ora_date);
				GetColumnData(t_cols, i, k, target_type, _target_ca.db_api, &t_string, &t_int_set, &t_int, &t_ts_struct, &t_ora_date);
			}

			// Compare 2 string values
			if(s_string != NULL && t_string != NULL && !lob)
			{
				// Compare string representations of numbers .5 and 0.50
				bool num = ValidateCompareNumbers(&s_cols[k], s_string, s_len, &t_cols[k], t_string, t_len, &equal);
				bool datetime = false;

				if(!num)
					datetime = ValidateCompareDatetimes(&s_cols[k], s_string, s_len, &t_cols[k], t_string, t_len, &equal);

				if(!num && !datetime)
				{
					if(s_len == t_len)
						equal = memcmp(s_string, t_string, (size_t)s_len);
					else
						ValidateCompareStrings(&s_cols[k], s_string, s_len, &t_cols[k], t_string, t_len, &equal);
				}
			}
			else
			// Compare string and integer
			if(((s_int_set && t_string != NULL) || (s_string != NULL && t_int_set)) && !lob)
			{
				char *str = (s_string != NULL) ? s_string : t_string;
				int int_v = s_int_set ? s_int : t_int;

				int int_v2 = atoi(str);

				equal = (int_v == int_v2) ? 0 : -1; 
			}
			else
			// Compare ODBC SQL_TIMESTAMP_STRUCT and string
			if(((s_ts_struct != NULL && t_string != NULL) || (t_ts_struct != NULL && s_string != NULL))  && !lob)
			{
				SQL_TIMESTAMP_STRUCT *ts = (s_ts_struct != NULL) ? s_ts_struct : t_ts_struct;
				char *str = (s_string != NULL) ? s_string : t_string;

				// String can contain date only or date and time
				int str_len = (s_string != NULL) ? s_len : t_len;

				char ts_str[27]; 

				// Convert SQL_TIMESTAMP_STRUCT to string
				Str::SqlTs2Str((short)ts->year, (short)ts->month, (short)ts->day, (short)ts->hour, (short)ts->minute, (short)ts->second, (long)ts->fraction, ts_str);
			
				equal = memcmp(str, ts_str, (size_t)str_len);
			}
			else
			// Compare Oracle DATE and string
			if(((s_ora_date != NULL && t_string != NULL) || (t_ora_date != NULL && s_string != NULL))  && !lob)
			{
				char *ora_date = (s_ora_date != NULL) ? s_ora_date : t_ora_date;
				char *str = (s_string != NULL) ? s_string : t_string;

				// String can contain date only or date and time
				// int str_len = (s_string != NULL) ? s_len : t_len;

				// Convert 7-byte packed Oracle DATE to string (non-null terminated, exactly 19 characters)
				Str::OraDate2Str((unsigned char*)ora_date, ora_date_str);

				// Target SQL Server i.e. may have trailing .000 in string, so let's use Oracle len
				equal = memcmp(ora_date_str, str, 19);
			}
			
			// Not equal
			if(equal != 0)
			{
				equal_row = false;
				s_cols[k]._diff_rows++;

				if(_trace_diff_data)
					ValidateDiffDump(&s_cols[k], &t_cols[k], i + running_rows, s_len, t_len, s_string, t_string);
			}
		}

		if(equal_row == false)
		{
			not_equal_rows++;

			// Check for not equal rows max limit
			if(_validation_not_equal_max_rows != -1 && (not_equal_rows + running_not_equal_rows) >= _validation_not_equal_max_rows)
				break;
		}
	}

	if(s_bytes != NULL)
		*s_bytes = sb;

	if(t_bytes != NULL)
		*t_bytes = tb;

	return not_equal_rows;
}

// Compare string representations of numbers .5 and 0.50
bool SqlDb::ValidateCompareNumbers(SqlCol *s_col, const char *s_string, int s_len, SqlCol *t_col, 
										const char *t_string, int t_len, int *equal)
{
	if(s_col == NULL || s_string == NULL || t_col == NULL || t_string == NULL)
		return false;

	bool num = false;
	bool comp = false;

	// Check is the source data type is a number fetched as string
	if((source_type == SQLDATA_ORACLE && s_col->_native_dt == SQLT_NUM && s_col->_scale != 129 /*floating*/) || 
		(source_type == SQLDATA_SYBASE && s_col->_native_fetch_dt == CS_CHAR_TYPE &&
			(s_col->_native_dt == CS_NUMERIC_TYPE || s_col->_native_dt == CS_DECIMAL_TYPE || 
			 s_col->_native_dt == CS_MONEY_TYPE || s_col->_native_dt == CS_MONEY4_TYPE)) ||
		(source_type == SQLDATA_INFORMIX && s_col->_native_fetch_dt == SQL_C_CHAR &&
			(s_col->_native_dt == SQL_DECIMAL || s_col->_native_dt == SQL_NUMERIC ||
			 s_col->_native_dt == SQL_REAL || s_col->_native_dt == SQL_DOUBLE)))
	{
		// Compare numbers
		comp = Str::CompareNumberAsString(s_string, s_len, t_string, t_len);
		num = true;
	}
	else
	// Compare floating point numbers that can be different due to rounding issues
	// Sybase ASE can return 6.7000000000000002 for value 6.7
	if((source_type == SQLDATA_ORACLE && s_col->_native_dt == SQLT_NUM && s_col->_scale == 129 /*floating*/) ||		
		(source_type == SQLDATA_SYBASE && s_col->_native_fetch_dt == CS_CHAR_TYPE &&
		(s_col->_native_dt == CS_REAL_TYPE || s_col->_native_dt == CS_FLOAT_TYPE))) 
	{
		double d1 = 0.0, d2 = 0.0;
		char f1[11], f2[11];

		// Get double values
		sprintf(f1, "%%%dlf", s_len);
		sprintf(f2, "%%%dlf", t_len);
		sscanf(s_string, f1, &d1);
		sscanf(t_string, f2, &d2);

		if(d1 == d2)
		{
			comp = true;
			num = true;
		}
	}

	if(equal != NULL)
		*equal = comp ? 0 : -1;

	return num;
}

// Compare datetime values featched as strings
bool SqlDb::ValidateCompareDatetimes(SqlCol *s_col, const char *s_string, int s_len, SqlCol *t_col, 
										const char *t_string, int t_len, int *equal)
{
	if(s_col == NULL || s_string == NULL || t_col == NULL || t_string == NULL)
		return false;

	bool datetime = false;
	int comp = -1;

	// Oracle TIMESTAMP compared with MySQL DATETIME without fraction
	if((source_type == SQLDATA_ORACLE && s_col->_native_dt == SQLT_TIMESTAMP) ||
		(source_type == SQLDATA_SYBASE && (s_col->_native_dt == CS_DATETIME_TYPE || s_col->_native_dt == CS_DATETIME4_TYPE || s_col->_native_dt == CS_DATE_TYPE))
		&& (target_type == SQLDATA_MYSQL))
	{
		int s_len_new = s_len;
		int t_len_new = t_len;

		// All datetime data types are fetched with 26 length from Sybase ASE, so we need to correct the length
		if(source_type == SQLDATA_SYBASE)
		{
			// Sybase ASE DATE that fetched with 00:00:00.000000 time part
			if(s_col->_native_dt == CS_DATE_TYPE)
				s_len_new = 10;
			else
			// Sybase ASE SMALLDATETIME contains minutes only, seconds always 00
			if(s_col->_native_dt == CS_DATETIME4_TYPE)
				s_len_new = 19;
		}

		// Number of fractional digits to compare
		if(_validation_datetime_fraction != -1)
		{
			if(s_len_new > 20 + _validation_datetime_fraction)
				s_len_new = 20 + _validation_datetime_fraction;

			if(t_len_new > 19 + _validation_datetime_fraction)
				t_len_new = 20 + _validation_datetime_fraction;
		}
			
		// If length are not equal allow only difference in number of 0s in fractional part, so consider .0 and .000 equal
		if(s_len_new != t_len_new)
		{
			int l_len = s_len_new;
			int m_len = t_len_new;
			const char *m_string = t_string;
				
			if(s_len_new > t_len_new) 
			{
				l_len = t_len_new;
				m_len = s_len_new;
				m_string = s_string;
			}

			comp = memcmp(s_string, t_string, (size_t)l_len);	

			// Common part is equal
			if(!comp)
			{
				// Larger part must contain 0s
				for(int i = l_len; i < m_len; i++)
				{
					if(m_string[i] != '0')
					{
						comp = -1;
						break;
					}
				}
			}
		}
		else
			comp = memcmp(s_string, t_string, (size_t)s_len_new);				

		datetime = true;
	}

	if(equal != NULL)
		*equal = comp;

	return datetime;
}

// Compare strings
bool SqlDb::ValidateCompareStrings(SqlCol *s_col, const char *s_string, int s_len, SqlCol *t_col, const char *t_string, int t_len, int *equal)
{
	if(s_col == NULL || s_string == NULL || t_col == NULL || t_string == NULL)
		return false;

	const char *max_s = s_string;
	int max_len = s_len;
	int min_len = t_len;

	if(s_len < t_len)
	{
		max_s = t_string;
		max_len = t_len;
		min_len = s_len;
	}

	// First compare data for matched size
	int comp = memcmp(s_string, t_string, (size_t)min_len);

	// Common part is equal
	if(comp == 0)
	{
		// Larger part must contain spaces only
		for(int i = min_len; i < max_len; i++)
		{
			if(max_s[i] != ' ')
			{
				comp = -1;
				break;
			}
		}
	}

	if(equal != NULL)
		*equal = comp;

	return true;
}

// Compare LOB values
bool SqlDb::ValidateCompareLobs(SqlCol *s_col, int s_len, SqlCol *t_col, int t_len, int *equal)
{
	if(s_col == NULL || t_col == NULL)
		return false;

	bool lob = false;
	int comp = -1;

	if(source_type == SQLDATA_ORACLE && (s_col->_native_dt == SQLT_CLOB || s_col->_native_dt == SQLT_BLOB))
	{
		if(s_len == t_len)
			comp = 0;

		lob = true;
	}

	if(lob && equal != NULL)
		*equal = comp;

	return lob;
}

// Dump differences in column
void SqlDb::ValidateDiffDump(SqlCol *s_col, SqlCol *t_col, int row, int s_len, int t_len, char *s_string, char *t_string)
{
	if(s_col == NULL || t_col == NULL)
		return;

	_trace_diff.LogFile("Row %d, Column %s (src native type code %d, tgt native type code %d)\n", row, s_col->_name, s_col->_native_dt, t_col->_native_dt);
	_trace_diff.LogFile("src len: %d; tgt len: %d;\n", s_len, t_len);

	_trace_diff.LogFile("src val: ");
	ValidateDiffDumpValue(s_len, s_string);
	_trace_diff.LogFile("\n");

	_trace_diff.LogFile("tgt val: ");
	ValidateDiffDumpValue(t_len, t_string);
	_trace_diff.LogFile("\n\n");
}

// Dump column value
void SqlDb::ValidateDiffDumpValue(int len, char *str)
{
	if(len != -1) 
	{
		if(str != NULL)
			_trace_diff.LogFile(str, len);
	}
	else 
		_trace_diff.LogFile("NULL");
}

// Get the length of column data in the buffer
int SqlDb::GetColumnDataLen(SqlCol *cols, int row, int column, int db_type, SqlApiBase *db_api)
{
	if(cols == NULL || db_api == NULL)
		return -1;

	size_t len = (size_t)-1;

	if(db_type == SQLDATA_ORACLE)
	{
		if(cols[column]._ind2 == NULL || cols[column]._len_ind2 == NULL)
			return -1;

		// Check for NULL
		if(cols[column]._ind2[row] == -1)
			len = (size_t)-1;
		else
		// Get length for the CLOB or BLOB
		if(cols[column]._native_dt == SQLT_CLOB || cols[column]._native_dt == SQLT_BLOB)
		{
			// LOB contains data
			int rc = db_api->GetLobLength((size_t)row, (size_t)column, &len);

			if(rc == -1)
				return -1;
		}
		// Get the length for non-NULL non-LOB value
		else
			len = (size_t)cols[column]._len_ind2[row];
	}
	else
	if(db_type == SQLDATA_SYBASE)
	{
		if(cols[column]._ind2 == NULL || cols[column]._len_ind4 == NULL)
			return -1;

		// Check for NULL
		if(cols[column]._ind2[row] == -1)
			len = (size_t)-1;
		else
			len = (size_t)cols[column]._len_ind4[row];
	}
	else
	if(db_type == SQLDATA_MYSQL)
	{
		if(cols[column].ind != NULL && cols[column].ind[row] != -1)
			len = cols[column].ind[row];
	}
	else
	if(db_type == SQLDATA_POSTGRESQL)
	{
		if(cols[column].ind != NULL && cols[column].ind[row] != -1)
			len = cols[column].ind[row];
	}
	else
	if(db_type == SQLDATA_SQL_SERVER || db_type == SQLDATA_INFORMIX)
	{
		if(cols[column].ind != NULL && cols[column].ind[row] != -1)
			len = cols[column].ind[row];
	}
	
	return (int)len;
}

// Get column data from the buffer
int SqlDb::GetColumnData(SqlCol *cols, int row, int column, int db_type, SqlApiBase *db_api, 
							char **str, bool *int_set, int *int_value, SQL_TIMESTAMP_STRUCT **ts_struct, 
							char **ora_date)
{
	if(cols == NULL || db_api == NULL)
		return -1;

	char *s = NULL;
	char *ora_d = NULL;

	int int_v = 0;
	bool int_s = false;

	SQL_TIMESTAMP_STRUCT *ts = NULL;

	if(db_type == SQLDATA_ORACLE)
	{
		if(cols[column]._native_fetch_dt == SQLT_STR || cols[column]._native_fetch_dt == SQLT_BIN)
			s = cols[column]._data + cols[column]._fetch_len * row;
		else
		if(cols[column]._native_fetch_dt == SQLT_INT)
		{
			int_v = *((int*)(cols[column]._data + cols[column]._fetch_len * row));
			int_s = true;
		}
		else
		if(cols[column]._native_fetch_dt == SQLT_DAT)
			ora_d = cols[column]._data + cols[column]._fetch_len * row;
	}
	else
	if(db_type == SQLDATA_SYBASE)
	{ 
		if(cols[column]._native_fetch_dt == CS_CHAR_TYPE) 
			s = cols[column]._data + cols[column]._fetch_len * row;
	}
	else
	if(db_type == SQLDATA_MYSQL)
	{
		// All columns are fetched as strings
		s = cols[column]._data + cols[column]._fetch_len * row;
	}
	else
	if(db_type == SQLDATA_POSTGRESQL)
	{
		s = cols[column]._data + cols[column]._fetch_len * row;
	}
	else
	if(db_type == SQLDATA_SQL_SERVER || db_type == SQLDATA_INFORMIX)
	{
		if(cols[column]._native_fetch_dt == SQL_C_CHAR || cols[column]._native_fetch_dt == SQL_C_BINARY)
			s = cols[column]._data + cols[column]._fetch_len * row;
		else
		if(cols[column]._native_fetch_dt == SQL_C_LONG)
		{
			int_v = *((int*)(cols[column]._data + cols[column]._fetch_len * row));
			int_s = true;
		}
		else
		if(cols[column]._native_fetch_dt == SQL_TYPE_TIMESTAMP)
			ts = (SQL_TIMESTAMP_STRUCT*)(cols[column]._data + cols[column]._fetch_len * row);
	}

	if(str != NULL && s != NULL)
		*str = s;

	if(int_value != NULL && int_set != NULL && int_s == true)
	{
		*int_value = int_v;
		*int_set = true;
	}

	if(ts_struct != NULL && ts != NULL)
		*ts_struct = ts;

	if(ora_date != NULL && ora_d != NULL)
		*ora_date = ora_d;

	return 0;
}

// Build transfer and data validation query
int SqlDb::BuildQuery(std::string &s_query, std::string &t_query, const char *s_table, const char *t_table,
							bool transfer)
{
	if(_source_ca.db_api == NULL)
		return -1;

	s_query = "SELECT ";
	t_query = "SELECT ";
	bool select_list = false;

	// Check if SELECT expression is defined for the table
	if(_tsel_exp_map != NULL )
	{
		std::map<std::string, std::string>::iterator i = _tsel_exp_map->find(s_table);

		if(i != _tsel_exp_map->end())
		{
			if(s_table != NULL)
			{
				s_query += i->second;
				s_query += " FROM ";
				s_query += s_table;
			}

			if(t_table != NULL)
			{
				t_query += i->second;
				t_query += " FROM ";
				t_query += t_table;
			}

			select_list = true;
		}
	}

	// Check if SELECT expression is defined for all tables
	if(!select_list && _tsel_exp_all != NULL && !_tsel_exp_all->empty())
	{
		if(s_table != NULL)
		{
			s_query += *_tsel_exp_all;
			s_query += " FROM ";
			s_query += s_table;
		}

		if(t_table != NULL)
		{
			t_query += *_tsel_exp_all;
			t_query += " FROM ";
			t_query += t_table;
		}

		select_list = true;
	}
	
	// Metadata is not supported
	if(!select_list && source_type != SQLDATA_ORACLE && source_type != SQLDATA_INFORMIX && source_type != SQLDATA_SYBASE
		&& source_type != SQLDATA_ASA)
	{
		if(s_table != NULL)
		{
			s_query += "* FROM ";
			s_query += s_table;
		}

		if(t_table != NULL)
		{
			t_query += "* FROM ";
			t_query += t_table;
		}

		select_list = true;
	}

	std::string s_schema, t_schema;
	std::string s_object, t_object;

	if(!select_list)
	{
		SqlApiBase::SplitQualifiedName(s_table, s_schema, s_object);
		SqlApiBase::SplitQualifiedName(t_table, t_schema, t_object);

		std::list<SqlColMeta> *table_columns = _source_ca.db_api->GetTableColumns();

		if(table_columns == NULL)
			return -1;

		int num = 0;

		// Find table columns (already ordered by column number)
		for(std::list<SqlColMeta>::iterator i = table_columns->begin(); i != table_columns->end(); i++)
		{
			char *s = (*i).schema;
			char *t = (*i).table;
			char *c = (*i).column;
			char *d = (*i).data_type;

			// Table found
			if(s != NULL && t != NULL && c != NULL && strcmp(s, s_schema.c_str()) == 0 && 
				strcmp(t, s_object.c_str()) == 0)
			{
				if(num > 0)
				{
					s_query += ", ";
					t_query += ", ";
				}

				bool column_added = false;

				if(d != NULL &&
				    // Oracle CHAR contains trailing spaces on retrieval 
					((source_type == SQLDATA_ORACLE && !_stricmp(d, "CHAR")) ||
					// Sybase ASE CHAR (UDT ID, TID) also contains trailing spaces
					(source_type == SQLDATA_SYBASE && 
						(!_stricmp(d, "CHAR") || !_stricmp(d, "ID") || !_stricmp(d, "TID")))))
				{
					// MySQL by default trims spaces in CHAR, but PAD_CHAR_TO_FULL_LENGTH can be set, so trim both 
					if(transfer == false && target_type == SQLDATA_MYSQL)
					{
						s_query += "RTRIM(";

						if(source_type == SQLDATA_ORACLE)
							s_query += '"';

						s_query += c;

						if(source_type == SQLDATA_ORACLE)
							s_query += '"';

						s_query += ") AS ";
						s_query += c;
					
						t_query += "RTRIM(";
						t_query += c;
						t_query += ") AS ";
						t_query += c;

						column_added = true;
					}
				}
				else
				// Oracle TIMESTAMP WITH TIME ZONE column
				if(source_type == SQLDATA_ORACLE && d != NULL && strstr(d, "WITH TIME ZONE") != NULL)
				{
					// Convert to UTC time zone for MySQL
					if(target_type == SQLDATA_MYSQL)
					{
						s_query += "SYS_EXTRACT_UTC(\"";
						s_query += c;
						s_query += "\") AS ";
						s_query += c;

						t_query += c;

						column_added = true;
					}
				}
				else
				// Oracle XMLTYPE column that fetched as object type in OCI
				if(source_type == SQLDATA_ORACLE && d != NULL && !_stricmp(d, "XMLTYPE"))
				{
					s_query += "XMLSERIALIZE(CONTENT \"";
					s_query += c;
					s_query += "\" AS CLOB) AS ";
					s_query += c;

					t_query += c;

					column_added = true;
				}
				else
				// Oracle stores quoted columns without " but in the same case as they specified in CREATE TABLE
				if(source_type == SQLDATA_ORACLE)
				{
					// Double quotes will not be returned when describing column name after open cursor
					s_query += "\"";
					s_query += c;
					s_query += "\"";	

					if(target_type == SQLDATA_SQL_SERVER)
					{
						t_query += '[';
						t_query += c;
						t_query += "]";
					}
					else
					if(target_type == SQLDATA_MYSQL)
					{
						t_query += '`';
						t_query += c;
						t_query += "`";
					}
					else
						t_query += c;

					column_added = true;
				}
				else
				// Sybase ASE stores columns without [] even if they are reserved words or contain spaces
				if(source_type == SQLDATA_SYBASE)
				{
					s_query += '[';
					s_query += c;
					s_query += ']';
				
					t_query += c;

					column_added = true;
				}
				else
				// Sybase ASA stores columns that are reserved words without any quotes in catalog
				if(source_type == SQLDATA_ASA && _source_ca.db_api->IsReservedWord(c))
				{
					s_query += '"';
					s_query += c;
					s_query += '"';
				
					t_query += c;

					column_added = true;
				}
				
				// Add column as is
				if(column_added == false)
				{
					if(source_type == SQLDATA_ORACLE)
						s_query += '"';
					
					s_query += c;
					
					if(source_type == SQLDATA_ORACLE)
						s_query += '"';
					
					t_query += c;
				}
			
				num++;
				continue;
			}
			else
			// Another table's columns started
			if(num > 0)
				break;
		} 

		if(num == 0)
		{
			s_query += " *";
			t_query += " *";
		}
		
		s_query += " FROM ";
		t_query += " FROM ";
	
		if(s_table != NULL)
			s_query += s_table;

		if(t_table != NULL)
			t_query += t_table;
	}

	// Check if WHERE condition is defined for the table
	if(_twhere_cond_map != NULL )
	{
		std::map<std::string, std::string>::iterator i = _twhere_cond_map->find(s_table);

		if(i != _twhere_cond_map->end())
		{
			if(s_table != NULL)
			{
				s_query += " WHERE ";
				s_query += i->second;
			}

			if(t_table != NULL)
			{
				t_query += " WHERE ";
				t_query += i->second;
			}
		}
	}

	// Add sort order for validation
	if(transfer == false)
		BuildQueryAddOrder(s_query, s_schema, s_object, t_query, t_schema, t_object);

	return 0;
}

// Add ORDER BY clause to data validation query
int SqlDb::BuildQueryAddOrder(std::string &s_query, std::string &s_schema, std::string &s_object, 
			std::string &t_query, std::string & /*t_schema*/, std::string & /*t_object*/)
{
	if(_metaSqlDb == NULL || _metaSqlDb->_source_ca.db_api == NULL || _metaSqlDb->_target_ca.db_api == NULL)
		return -1;

	SqlApiBase *db_api = _metaSqlDb->_source_ca.db_api;
	SqlApiBase *t_db_api = _metaSqlDb->_target_ca.db_api;

	std::list<std::string> s_order, s_order_sorts;
	std::list<std::string> s_order_types;

	bool pk_exists = false;

	std::list<SqlConstraints> *table_cns = db_api->GetTableConstraints();
	std::list<SqlConsColumns> *table_cns_cols = db_api->GetConstraintColumns();

	if(table_cns != NULL && table_cns_cols != NULL)
	{
		// Find a primary or unique key
		for(std::list<SqlConstraints>::iterator i = table_cns->begin(); i != table_cns->end(); i++)
		{
			if((*i).type != 'P' && (*i).type != 'U')
				continue;

			char *s = (*i).schema;
			char *t = (*i).table;
			char *c = (*i).constraint;

			if(s == NULL || t == NULL || c == NULL)
				continue;

			if(strcmp(t, s_object.c_str()) != 0 || strcmp(s, s_schema.c_str()) != 0)
					continue;
	
			// Now find the key columns
			db_api->GetKeyConstraintColumns((*i), s_order, &s_order_types);
			
			pk_exists = true;
			break;
		}
	} 

	bool unique_exists = false;

	std::list<SqlIndexes> *idx = db_api->GetTableIndexes();

	if(pk_exists == false && idx != NULL)
	{
		// Find the first unique index
		for(std::list<SqlIndexes>::iterator i = idx->begin(); i != idx->end(); i++)
		{
			if((*i).unique == false)
				continue;

			char *s = (*i).t_schema;
			char *t = (*i).t_name;
			
			if(s == NULL || t == NULL)
				continue;

			if(strcmp(t, s_object.c_str()) != 0 || strcmp(s, s_schema.c_str()) != 0)
				continue;
	
			// Get index columns
			db_api->GetIndexColumns((*i), s_order, s_order_sorts);

			unique_exists = true;
			break;
		}
	} 

	std::list<SqlColMeta> *table_cols = db_api->GetTableColumns();

	if(pk_exists == false && unique_exists == false && table_cols != NULL)
	{
		bool found = false;

		// Find table columns
		for(std::list<SqlColMeta>::iterator i = table_cols->begin(); i != table_cols->end(); i++)
		{
			char *s = (*i).schema;
			char *t = (*i).table;
			char *c = (*i).column;
			char *d = (*i).data_type;
			int dc = (*i).data_type_code;

			if(s == NULL || t == NULL || c == NULL)
				continue;

			if(strcmp(t, s_object.c_str()) != 0 || strcmp(s, s_schema.c_str()) != 0)
			{
				if(found)
					break;
				else
					continue;
			}

			// Data type as a string in Oracle, Sybase ASE
			if(d != NULL)
			{
				// A numeric column
				if(!_stricmp(d, "NUMBER") || !_stricmp(d, "INT") || !_stricmp(d, "SMALLINT") || !_stricmp(d, "BIGINT") ||
					!_stricmp(d, "NUMERIC") || !_stricmp(d, "DECIMAL") || !_stricmp(d, "FLOAT") || !_stricmp(d, "MONEY") || !_stricmp(d, "SMALLMONEY"))
				{
					s_order.push_back(c);
					s_order_types.push_back("Numeric");
				}
				else
				// A string column CHAR, VARCHAR or VARCHAR2
				if(!_stricmp(d, "CHAR") || !_stricmp(d, "VARCHAR"))
				{
					s_order.push_back(c);
					s_order_types.push_back("String");
				}
				else
				// A datetime column
				if(strstr(d, "TIMESTAMP") != NULL || !_stricmp(d, "DATE") || !_stricmp(d, "DATETIME"))
				{
					s_order.push_back(c);
					s_order_types.push_back("Datetime");
				}

				found = true;
			}
			else
			// Data type as a code in Informix
			if(source_type == SQLDATA_INFORMIX)
			{
				// A numeric column SMALLINT - 1, INTEGER - 2, FLOAT - 3, SMALLFLOAT - 4, DECIMAL - 5
				// SERIAL - 6 (if NOT NULL +256)
				if((dc >= 1 && dc <= 6) || (dc >= 257 && dc <= 262))
				{
					s_order.push_back(c);
					s_order_types.push_back("Numeric");
				}
				else
				// A string column CHAR - 0, CHAR NOT NULL - 256, VARCHAR - 13, VARCHAR NOT NULL 269
				if(dc == 0 || dc == 13 || dc == 256 || dc == 269)
				{
					s_order.push_back(c);
					s_order_types.push_back("String");
				}
				else
				// A datetime column DATE - 7
				if(dc == 7)
				{
					s_order.push_back(c);
					s_order_types.push_back("Datetime");
				}

				found = true;
			}
		}
	}

	if(s_order.empty() == false)
	{
		int c = 0;
		std::list<std::string>::iterator ti = s_order_types.begin();

		for(std::list<std::string>::iterator i = s_order.begin(); i != s_order.end(); i++)
		{
			std::string type;

			// Column type - String, Numeric or Datetime
			if(ti != s_order_types.end())
			{
				type = (*ti);
				ti++;
			}

			if(c == 0)
			{
				s_query += " ORDER BY ";
				t_query += " ORDER BY ";
			}
			else
			{
				s_query += ", ";
				t_query += ", ";
			}

			s_query += (*i);
			t_query += (*i);
		
			// In Oracle NULLs go last in ASC order, in MySQL and Informix first
			if(source_type == SQLDATA_ORACLE && target_type == SQLDATA_MYSQL) 
				s_query += " NULLS FIRST";
			else
			if((source_type == SQLDATA_MYSQL || source_type == SQLDATA_INFORMIX) && target_type == SQLDATA_ORACLE)
				t_query += " NULLS FIRST";
			else
			// MySQL/MariaDB uses collation defined in the table, for some reason it does not use COLLATION_CONNECTION session value
			// so we redefine COLLATE in ORDER BY for strings
			if(target_type == SQLDATA_MYSQL)
			{
				if(_mysql_validation_collate != NULL && type == "String")
					t_query.append(" COLLATE ").append(_mysql_validation_collate);
			}
			else
			if(target_type == SQLDATA_SQL_SERVER)
			{
				if(type == "String")
					t_query.append(" COLLATE Latin1_General_bin");
			}
			else
			// Informix and PostgreSQL have differences in string and NULL order
			if(source_type == SQLDATA_INFORMIX && target_type == SQLDATA_POSTGRESQL)
			{
				// Informix sorts strings in ASCII order - space, digit, A,B,a, b; 
				// while PostgreSQL space, digit, a, A, b, B when collation is not "C"
				if(type == "String")
				{
					// Collate "C" to use ASCII code sorting is available since PostgreSQL 9.1
					if(t_db_api->GetMajorVersion() >= 9 && t_db_api->GetMinorVersion() >= 1)
						t_query += " COLLATE \"C\" NULLS FIRST";
				}
				else
					t_query += " NULLS FIRST";
			}

			c++;
		}
	}
		
	return 0;
}

// Execute the statement that does not return any result
int SqlDb::ExecuteNonQuery(int db_types, SqlDataReply &reply, const char *query)
{
	if(db_types == SQLDB_TARGET_ONLY)
	{
		reply.rc = _target_ca.db_api->ExecuteNonQuery(query, &reply._t_time_spent);

		if(reply.rc == -1)
		{
			reply.t_error = _target_ca.db_api->GetError();
			strcpy(reply.t_native_error_text, _target_ca.db_api->GetNativeErrorText());
		}
	}

	return reply.rc;
}

// Create sequence object
int SqlDb::CreateSequence(int db_types, SqlDataReply &reply, const char *query, const char *seq_name)
{
	if(seq_name == NULL)
		return -1;

	SqlDataReply drop_reply;

	strcpy(reply.t_o_name, seq_name);
	drop_reply = reply;
	
	std::string drop_stmt = "DROP SEQUENCE ";

	// IF EXISTS supported by PostgreSQL
	if(target_type == SQLDATA_POSTGRESQL)
		drop_stmt += "IF EXISTS ";

	drop_stmt += seq_name;

	// Execute drop sequence
	int rc = ExecuteNonQuery(db_types, drop_reply, drop_stmt.c_str());
		
	if(_callback != NULL)
	{
		drop_reply._cmd_subtype = SQLDATA_CMD_DROP_SEQUENCE;
		drop_reply.t_sql = drop_stmt.c_str();

		_callback(_callback_object, &drop_reply);
	}

	// Create the sequence
	rc = ExecuteNonQuery(db_types, reply, query);

	return rc;
}

// Drop foreign keys to the 
int SqlDb::DropReferences(int db_types, const char *table, size_t *time_spent, int *keys)
{
	SqlApiBase *api = (db_types == SQLDB_SOURCE_ONLY) ? _source_ca.db_api : _target_ca.db_api;

	if(api == NULL)
		return -1;

	int rc = api->DropReferencesChild(table, time_spent, keys);

	return rc;
}

// Get database type for ODBC API
short SqlDb::GetSubType(short db_types)
{
	short subtype = 0;

	SqlApiBase *api = (db_types == SQLDB_SOURCE_ONLY) ? _source_ca.db_api : _target_ca.db_api;

	if(api != NULL)
		subtype = api->GetSubType();

	return subtype;
}

// Execute a command by database API threads
int SqlDb::Execute(int cmd)
{
	return Execute(cmd, cmd);
}

// Execute one command for source, another for target in concurrent threads and wait
int SqlDb::Execute(int cmd1, int cmd2)
{
	_source_ca._cmd = (short)cmd1;
	_target_ca._cmd = (short)cmd2;

	// Release the source and target threads to process the command
#if defined(WIN32) || defined(_WIN64)
	SetEvent(_source_ca._wait_event);
	SetEvent(_target_ca._wait_event);
#else
	Os::SetEvent(&_source_ca._wait_event);
	Os::SetEvent(&_target_ca._wait_event);
#endif
	// Wait for the commands completion
#if defined(WIN32) || defined(_WIN64)
	HANDLE handles[2] = { _source_ca._completed_event, _target_ca._completed_event };
	WaitForMultipleObjects(2, handles, TRUE, INFINITE);
#else
	Os::WaitForEvent(&_source_ca._completed_event);
	Os::WaitForEvent(&_target_ca._completed_event);
#endif

	if(_source_ca.cmd_rc == -1 || _target_ca.cmd_rc == -1)
		return -1;

	return 0;
}

// Start a worker thread
int SqlDb::StartWorker(SqlDbThreadCa *ca)
{
	if(ca == NULL)
		return -1;

	bool more = true;

	while(more)
	{
		size_t time_spent = 0;

		// Wait for a new task
#if defined(WIN32) || defined(_WIN64)
		WaitForSingleObject(ca->_wait_event, INFINITE);
#else
		Os::WaitForEvent(&ca->_wait_event);
#endif
		// Execute connection command
		if(ca->_cmd == SQLDATA_CMD_CONNECT)
		{
			ca->cmd_rc = (short)ca->db_api->Connect(&ca->_time_spent);
		}
		else 
		// Execute row count command
		if(ca->_cmd == SQLDATA_CMD_VALIDATE_ROW_COUNT)
		{
			ca->cmd_rc = (short)ca->db_api->GetRowCount(ca->_name, &ca->_int1, &ca->_time_spent);
		}
		else 
		// Open cursor for data validation
		if(ca->_cmd == SQLDATA_CMD_OPEN_CURSOR)
		{
			size_t col_count;
			size_t allocated_rows;

			// Exact or less number of rows must be fetched 
			ca->cmd_rc = (short)ca->db_api->OpenCursor((const char*)ca->_void1, (size_t)ca->_int4, 0, &col_count, &allocated_rows, 
					&ca->_int3, (SqlCol**)&ca->_void2, &ca->_time_spent);
			ca->_int1 = (int)col_count;
			ca->_int2 = (int)allocated_rows;
		}
		else
		// Fetch next portion of data
		if(ca->_cmd == SQLDATA_CMD_FETCH_NEXT)
		{
			ca->cmd_rc = (short)ca->db_api->Fetch(&ca->_int1, &time_spent);
			ca->_int4 = (int)time_spent;
		}
		else
		// Insert the data from buffer
		if(ca->_cmd == SQLDATA_CMD_INSERT_NEXT)
		{
			size_t bytes_written;
			ca->cmd_rc = (short)ca->db_api->TransferRows((SqlCol*)ca->_void1, ca->_int2, &ca->_int1, &bytes_written,
				&time_spent);
			ca->_int3 = (int)bytes_written;
			ca->_int4 = (int)time_spent;
		}
		else 
		// Close cursor for data validation
		if(ca->_cmd == SQLDATA_CMD_CLOSE_CURSOR)
		{
			ca->cmd_rc = (short)ca->db_api->CloseCursor();
		}
		else
		// Read the database schema
		if(ca->_cmd == SQLDATA_CMD_READ_SCHEMA)
		{
			ca->cmd_rc = (short)ca->db_api->ReadSchema((const char*)ca->_void1, (const char*)ca->_void2);
		}
		else
		// Read the schema of the database where transfer is performed to
		if(ca->_cmd == SQLDATA_CMD_READ_SCHEMA_FOR_TRANSFER_TO)
		{
			ca->cmd_rc = (short)ca->db_api->ReadSchemaForTransferTo((const char*)ca->_void1, (const char*)ca->_void2);
		}

		// Get database API error code
		if(ca->cmd_rc == -1)
		{
			ca->error = ca->db_api->GetError();
			strcpy(ca->native_error_text, ca->db_api->GetNativeErrorText());
		}

		// Specify that the task is completed
#if defined(WIN32) || defined(_WIN64)
		SetEvent(ca->_completed_event);
#else
		Os::SetEvent(&ca->_completed_event);
#endif
	}

	return 0;
}

// Start a worker thread
#if defined(WIN32) || defined(_WIN64)
unsigned int __stdcall SqlDb::StartWorkerS(void *v_ca)
#else
void* SqlDb::StartWorkerS(void *v_ca)
#endif
{
	SqlDbThreadCa *ca = (SqlDbThreadCa*)v_ca;

	if(ca == NULL || ca->sqlDb == NULL)
#if defined(WIN32) || defined(_WIN64)
		return (unsigned int)-1;
#else
		return NULL;
#endif

	int rc = ca->sqlDb->StartWorker(ca);

	delete ca->db_api;

#if defined(WIN32) || defined(_WIN64)
	return (unsigned int)rc;
#else
	return NULL;
#endif
}

// Set parameters
void SqlDb::SetParameters(Parameters *p) 
{ 
	_parameters = p; 

	if(_parameters->GetTrue("-trace_diff_data") != NULL)
		_trace_diff_data = true;

	_validation_not_equal_max_rows = _parameters->GetInt("-validation_not_equal_max_rows", -1);
	_validation_datetime_fraction = _parameters->GetInt("-validation_datetime_fraction", -1);
	_mysql_validation_collate = _parameters->Get("-mysql_validation_collate");
}

// Get errors on the DB interface
void SqlDb::GetErrors(SqlDataReply &reply)
{
	reply.rc = (_source_ca.cmd_rc == -1 || _target_ca.cmd_rc == -1) ? -1 : 0;

	reply._s_rc = _source_ca.cmd_rc;
	reply._t_rc = _target_ca.cmd_rc;

	reply.s_error = _source_ca.error;
	reply.t_error = _target_ca.error;

	strcpy(reply.s_native_error_text, _source_ca.native_error_text);
	strcpy(reply.t_native_error_text, _target_ca.native_error_text);
}

