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

// SQLData Manager

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#include <process.h>
#endif

#include <stdio.h>
#include <algorithm>
#include "sqldata.h"
#include "str.h"
#include "sqlparserexp.h"
#include "os.h"
#include "file.h"

// Constructor
SqlData::SqlData()
{
	_source_type = 0;
	_target_type = 0;

	source_subtype = 0;
	target_subtype = 0;

	_command = 0;
	_command_options = 0;

	_tables = NULL;
	_queries = NULL;

	_max_sessions = 4;

	_worker_exe = SQLDATA_WORKER_EXE;
	_workers = -1;
	_local_workers = -1;

#if defined(WIN32) || defined(_WIN64)
	_completed_event = NULL;

	// Critical sections 
	InitializeCriticalSection(&_task_queue_critical_section);
	InitializeCriticalSection(&_worker_critical_section);
#else
	pthread_mutex_init(&_task_queue_critical_section, NULL);
	pthread_mutex_init(&_worker_critical_section, NULL);
#endif

	_all_workers = 0;
	_running_workers = 0; 

	_s_total_rows = 0;
	_t_total_rows = 0;
	_s_total_bytes = 0;
	_t_total_bytes = 0;

	_parameters = NULL;
	_log = NULL;
	_trace = false;

	_migrate_tables = true;
	_migrate_data = true;
	_migrate_constraints = true;
	_migrate_indexes = true;

	_sqlParser = CreateParserObject();

	_callback = NULL;
	_callback_object = NULL;
	_callback_rate = 3100;
}

// Run the data transfer or validation
int SqlData::Run()
{
	if(_tables == NULL && _queries == NULL)
		return 1;

	_tables_in_progress.clear();
	_meta_in_progress.clear();

	_s_total_rows = 0;
	_t_total_rows = 0;
	_s_total_bytes = 0;
	_t_total_bytes = 0;

#if defined(WIN32) || defined(_WIN64)
	// Create an event to wait for completion of all tasks
	_completed_event = CreateEvent(NULL, FALSE, FALSE, NULL); 
#else
	Os::CreateEvent(&_completed_event);
#endif

	size_t start = GetTickCount();

	// Start local in-process workers
	StartLocalWorkers();

#if defined(WIN32) || defined(_WIN64)
	// Wait until all tasks are completed
	WaitForSingleObject(_completed_event, INFINITE);
#else
	Os::WaitForEvent(&_completed_event);
#endif

	// Notify on operation completion
	if(_callback)
	{
		SqlDataReply reply;
		reply._cmd = (short)_command;
		reply._cmd_subtype = SQLDATA_CMD_ALL_COMPLETE;

		reply._s_int1 = _s_total_rows;
		reply._t_int1 = _t_total_rows;
		reply._s_bigint1 = _s_total_bytes;
		reply._t_bigint1 = _t_total_bytes;

		reply._time_spent = GetTickCount() - start;
		reply._int1 = (int)reply._time_spent;
		
		_callback(_callback_object, &reply);
	}

	return 0;
}

// Specify source and target database connection strings
void SqlData::SetConnectionStrings(std::string &source, std::string &target, short *s_type, short *t_type)
{
	_source_conn = source;
	_target_conn = target;

	// Define the source and target types
	_source_type = DefineDbType(source.c_str());
	_target_type = DefineDbType(target.c_str());

	if(s_type != NULL)
		*s_type = _source_type;

	if(t_type != NULL)
		*t_type = _target_type;

	// Set SqlParser types
	if(_sqlParser != NULL)
	{
		short p_source = DefineSqlParserType(_source_type);
		short p_target = DefineSqlParserType(_target_type);

		SetParserTypes(_sqlParser, p_source, p_target);
	}
}

// Define the source and target types
short SqlData::DefineDbType(const char *conn)
{
	if(conn == NULL)
		return -1;

	if(_strnicmp(conn, "oracle", 6) == 0)
		return SQLDATA_ORACLE;
	else
	if(_strnicmp(conn, "sql", 3) == 0)
		return SQLDATA_SQL_SERVER;
	else
	if(_strnicmp(conn, "mysql", 5) == 0)
		return SQLDATA_MYSQL;
    else
	if(_strnicmp(conn, "mariadb", 7) == 0)
		return SQLDATA_MYSQL;
	else
	if(_strnicmp(conn, "pg", 2) == 0)
		return SQLDATA_POSTGRESQL;
	else
	if(_strnicmp(conn, "sybase", 6) == 0)
		return SQLDATA_SYBASE;
	else
	if(_strnicmp(conn, "informix", 8) == 0)
		return SQLDATA_INFORMIX;
	else
	if(_strnicmp(conn, "db2", 3) == 0)
		return SQLDATA_DB2;
	else
	if(_strnicmp(conn, "asa", 3) == 0)
		return SQLDATA_ASA;
	else
	if(_strnicmp(conn, "odbc", 4) == 0)
		return SQLDATA_ODBC;
	else
	if(_strnicmp(conn, "stdout", 6) == 0)
		return SQLDATA_STDOUT;

	return -1;
}

// Get SqlParser type from SqlData database type 
short SqlData::DefineSqlParserType(short type)
{
	short rt = 0;

	if(type == SQLDATA_SQL_SERVER)
		rt = SQL_SQL_SERVER;
	else
	if(type == SQLDATA_ORACLE)
		rt = SQL_ORACLE;
	else
	if(type == SQLDATA_DB2)
		rt = SQL_DB2;
	else
	if(type == SQLDATA_MYSQL)
		rt = SQL_MYSQL;
	else
	if(type == SQLDATA_POSTGRESQL)
		rt = SQL_POSTGRESQL;
	else
	if(type == SQLDATA_SYBASE)
		rt = SQL_SYBASE;
	else
	if(type == SQLDATA_INFORMIX)
		rt = SQL_INFORMIX;

	return rt;
}

// Perform local connection for metadata read
int SqlData::ConnectMetaDb(SqlDataReply &reply, int db_types)
{
	int s_rc = 0;
	int t_rc = 0;

	// Initialize local (in-process) database interface
	int rc = InitLocal(reply, db_types, &s_rc, &t_rc);

	if(rc == -1)
	{
		GetErrors(reply);

		// Exist only if both source and target initialized with errors, otherwise try to connect 
		// to one of the databases (at least test connection)
		if(s_rc == -1 && t_rc == -1)
			return -1;
	}

	// Connect to source and target databases through in-process interface
	rc = ConnectLocal(reply, db_types, &s_rc, &t_rc);

	return rc;
}

// Get the list of available tables
int SqlData::GetAvailableTables(std::string &select, std::string &exclude, std::list<std::string> &tables)
{
	int rc = _db.GetAvailableTables(SQLDB_SOURCE_ONLY, select, exclude, tables);

	return rc;
}

// Test connection to the database
int SqlData::TestConnection(std::string &conn, std::string &error, std::string &loaded_path, 
									std::list<std::string> &search_paths, size_t *time_spent)
{
	SqlDb db;
	db.SetParameters(_parameters);
	db.SetAppLog(_log);

	return db.TestConnection(conn, error, loaded_path, search_paths, time_spent);
}

// Create queues to transfer schema metadata
int SqlData::CreateMetadataQueues(std::string &select, std::string &exclude)
{
	_meta_tasks.clear();

	int db_types = (_command != SQLDATA_CMD_ASSESS) ? SQLDB_BOTH : SQLDB_SOURCE_ONLY;

	// Read source and target schemas
	int rc = _db.ReadSchema(db_types, select, exclude, _migrate_constraints, _migrate_indexes);

	// Do not perform any DDL tasks if validation or assessment is performed, or load to existing or truncate is set
	if(_command == SQLDATA_CMD_VALIDATE || _command == SQLDATA_CMD_ASSESS ||
		(_command == SQLDATA_CMD_TRANSFER &&
			(_command_options == SQLDATA_OPT_NONE || _command_options == SQLDATA_OPT_TRUNCATE)))
		return rc;

	std::list<SqlColMeta> *table_columns = _db.GetTableColumns(SQLDB_SOURCE_ONLY);

	if(table_columns != NULL)
	{
		// Find defaults and identity columns
		for(std::list<SqlColMeta>::iterator i = table_columns->begin(); i != table_columns->end(); i++)
		{
			// Column has the DEFAULT clause
			if((*i).no_default == false)
				CreateMetadataTaskForColumnDefault(*i);
			
			// Identity, auto-increment, serial columns
			if((*i).identity == true)
			{
				// For Oracle create sequence and trigger, for PostgreSQL sequence and default
				if(_target_type == SQLDATA_ORACLE || _target_type == SQLDATA_POSTGRESQL)
					GetIdentityMetaTask((*i)); 
			}
		}
	}

	std::list<SqlConstraints> *table_constraints = _db.GetTableConstraints(SQLDB_SOURCE_ONLY);
	std::list<SqlConsColumns> *table_cons_columns = _db.GetConstraintColumns(SQLDB_SOURCE_ONLY);

	if(table_constraints != NULL)
	{
		for(std::list<SqlConstraints>::iterator i = table_constraints->begin(); i != table_constraints->end(); i++)
		{
			SqlMetaTask task;

			std::string schema = (*i).schema;
			std::string table = (*i).table;

			task.s_name = schema;
			task.s_name += '.';
			task.s_name += table;

			MapObjectName(schema, table, task.t_name);

			task.statement = "ALTER TABLE ";
			task.statement += task.t_name;
			task.statement += " ADD";

			std::string clause;
			std::string ptable;

			// Select CHECK constraints
			if((*i).type == 'C' && (*i).condition != NULL)
			{
				task.type = SQLDATA_CMD_ADD_CHECK_CONSTRAINT;

				// Add if( to force a boolean condition conversion - without closing ')'
				std::string condition_source("if(");
				condition_source += (*i).condition;

				std::string condition_target;

				// Convert condition to the target syntax seperately (not entire ALTER TABLE statement)
				// to avoid unintended changes in identifiers
				ConvertSql(condition_source, condition_target);	

				clause += " CHECK (";

				// Remove "if("
				if(condition_target.size() > 3)
					clause += condition_target.c_str() + 3;
				else
					clause += condition_target;

				clause += ")";
			}
			else
			// PRIMARY KEY and UNIQUE constraints
			if((*i).type == 'P' || (*i).type == 'U')
			{
				if((*i).type == 'P')
				{
					// For target MySQL, MariaDB a primary key is defined in CREATE TABLE
					// if the table has an identity column, so skip PK in ALTER TABLE
					if(_target_type == SQLDATA_MYSQL && IsIdentityDefined(schema, table))
						continue;

					task.type = SQLDATA_CMD_ADD_PRIMARY_KEY;
					clause += " PRIMARY KEY (";
				}
				else
				{
					task.type = SQLDATA_CMD_ADD_UNIQUE_KEY;
					clause += " UNIQUE (";
				}

				std::string key_cols;

				// Get columns
				GetKeyConstraintColumns(task, (*i), key_cols);

				clause += key_cols;
				clause += ")";
			}
			else
			// FOREIGN KEY constraint
			if((*i).type == 'R' && _source_type == SQLDATA_ORACLE)
			{
				task.type = SQLDATA_CMD_ADD_FOREIGN_KEY;
				clause += " FOREIGN KEY (";

				std::string fcols;
				std::string pcols;

				// Find constraint columns
				for(std::list<SqlConsColumns>::iterator k = table_cons_columns->begin(); k != table_cons_columns->end(); k++)
				{
					// Compare schema, table and constraint names (columns are already ordered)
					if((*k).schema != NULL && strcmp((*k).schema, (*i).schema) == 0 &&
						(*k).table != NULL && strcmp((*k).table, (*i).table) == 0 &&
						(*k).constraint != NULL && strcmp((*k).constraint, (*i).constraint) == 0)
					{
						if(fcols.empty() == false)
							fcols += ", ";

						if(_target_type == SQL_SQL_SERVER)
							fcols += '[';

						fcols += (*k).column;

						if(_target_type == SQL_SQL_SERVER)
							fcols += ']';
					}

					// Get primary or unique key columns 
					if((*k).schema != NULL && strcmp((*k).schema, (*i).r_schema) == 0 &&
						(*k).constraint != NULL && strcmp((*k).constraint, (*i).r_constraint) == 0)
					{
						if(pcols.empty() == false)
							pcols += ", ";

						if(_target_type == SQL_SQL_SERVER)
							pcols += '[';

						pcols += (*k).column;

						if(_target_type == SQL_SQL_SERVER)
							pcols += ']';

						ptable = (*k).table;
					}
				}

				// If primary key table not selected, its columns will not be found, so read them from the database
				if(ptable.empty() == true)
				{
					_db.ReadConstraintTable(SQLDB_SOURCE_ONLY, (*i).r_schema, (*i).r_constraint, ptable);
					_db.ReadConstraintColumns(SQLDB_SOURCE_ONLY, (*i).r_schema, NULL, (*i).r_constraint, pcols);
				}

				clause += fcols;
				clause += ") REFERENCES ";
				clause += ptable;
				clause += " (";
				clause += pcols;
				clause += ")";
			}
			else
			// FOREIGN KEY constraint
			if((*i).type == 'R' && _source_type != SQLDATA_ORACLE)
			{
				task.type = SQLDATA_CMD_ADD_FOREIGN_KEY;
				clause += " FOREIGN KEY (";

				std::string fcols;
				std::string pcols;
				std::string ptable_t;

				// Get foreign key columns
				GetForeignKeyConstraintColumns(task, (*i), fcols, pcols, ptable);
				
				// Map referenced table name
				MapObjectName(ptable, ptable_t);

				clause += fcols;
				clause += ") REFERENCES ";
				clause += ptable_t;
				clause += " (";
				clause += pcols;
				clause += ")";

				// ON UPDATE clause
				if((*i).fk_on_update != 0)
				{
					clause += " ON UPDATE ";
					clause += GetForeignKeyAction((*i).fk_on_update);
				}

				// ON DELETE clause
				if((*i).fk_on_delete != 0)
				{
					clause += " ON DELETE ";
					clause += GetForeignKeyAction((*i).fk_on_delete);
				}
			}

			std::string cns_name;

			// Get the target constraint name
			MapConstraintName((*i).constraint, cns_name, (*i).type, task.s_name, ptable);

			if(cns_name.empty() == false)
			{
				task.statement += " CONSTRAINT ";
				task.statement += cns_name;
			}
			
			task.statement += clause;
			_meta_tasks.push_back(task);
		}
	}

	std::list<SqlComments> *table_comments = _db.GetTableComments(SQLDB_SOURCE_ONLY);

	if(table_comments != NULL && _target_type != SQLDATA_MYSQL)
	{
		for(std::list<SqlComments>::iterator i = table_comments->begin(); i != table_comments->end(); i++)
		{
			SqlMetaTask task;

			std::string s_schema = (*i).schema;
			std::string s_table = (*i).table;

			std::string t_schema;
			std::string t_table;

			MapObjectName(s_schema, s_table, t_schema, t_table);

			task.s_name = s_schema;
			task.s_name += '.';
			task.s_name += s_table;

			task.statement = "EXECUTE sp_addextendedproperty 'Comment', '";
			task.statement += (*i).comment;
			task.statement += "', 'user', ";
			task.statement += t_schema;
			task.statement += ", 'table', ";
			task.statement += t_table;

			task.type = SQLDATA_CMD_ADD_COMMENT;

			// It is a column comment
			if((*i).type == 'C')
			{
				task.statement += ", 'column', ";
				task.statement += (*i).column;
			}

			_meta_tasks.push_back(task);
		}
	}

	// Indexes
	std::list<SqlIndexes> *table_indexes = _db.GetTableIndexes(SQLDB_SOURCE_ONLY);

	if(table_indexes != NULL)
	{
		for(std::list<SqlIndexes>::iterator i = table_indexes->begin(); i != table_indexes->end(); i++)
		{
			SqlMetaTask task;

			std::string schema = (*i).t_schema;
			std::string table = (*i).t_name;

			task.s_name = schema;
			task.s_name += '.';
			task.s_name += table;

			MapObjectName(schema, table, task.t_name);

			task.type = SQLDATA_CMD_CREATE_INDEX;

			task.statement = "CREATE ";

			if((*i).unique == true)
				task.statement += "UNIQUE ";

			task.statement += "INDEX ";

			// Get target index name
			std::string t_index;
			ConvertIdentifier((*i).index, t_index);

			task.statement += t_index;
			task.statement += " ON ";
			task.statement += task.t_name;
			task.statement += " (";

			task.t_o_name = t_index;

			std::string idx_cols;

			// Get index columns
            GetIndexColumns((*i), idx_cols);

			task.statement += idx_cols;
			task.statement += ")";

			_meta_tasks.push_back(task);
		}
	}

	// Sequences
	std::list<SqlSequences> *sequences = _db.GetSequences(SQLDB_SOURCE_ONLY);

	if(sequences != NULL && _target_type != SQLDATA_MYSQL)
	{
		for(std::list<SqlSequences>::iterator i = sequences->begin(); i != sequences->end(); i++)
		{
			SqlMetaTask task;

			std::string schema = (*i).schema;
			std::string name = (*i).name;

			task.s_name = schema;
			task.s_name += '.';
			task.s_name += name;

			MapObjectName(schema, name, task.t_name);

			task.t_o_name = task.t_name;

			task.type = SQLDATA_CMD_CREATE_SEQUENCE;

			task.statement = "CREATE SEQUENCE ";
			task.statement += task.t_name;

			// Use last number as the start value
			task.statement += " START WITH ";
			task.statement += (*i).cur_str;

			task.statement += " INCREMENT BY ";
			task.statement += (*i).inc_str;
			
			_meta_tasks.push_back(task);
		}
	}

	return rc;
}

// Create tasks queue to transfer non-table objects
int SqlData::CreateObjectQueue(std::string &select, std::string &exclude, int *total_obj, int *total_lines, int *total_bytes)
{
	size_t obj_cnt  = 0;
	size_t obj_lines  = 0;
	size_t obj_bytes  = 0;

	_obj_meta_tasks.clear();

	// Read source objects
	int rc = _db.ReadObjects(SQLDB_SOURCE_ONLY, select, exclude);

	// Stored procedures
	std::list<SqlObjMeta> *procedures = _db.GetProcedures(SQLDB_SOURCE_ONLY);

	if(procedures != NULL)
	{
		obj_cnt += procedures->size();

		for(std::list<SqlObjMeta>::iterator i = procedures->begin(); i != procedures->end(); i++)
		{
			SqlObjMetaTask task;
			task.source = &(*i);

			_obj_meta_tasks.push_back(task);

			size_t bytes = 0;
			size_t lines = 0;

			Str::GetSizeAndLines((*i).text, &bytes, &lines);

			obj_bytes += bytes;
			obj_lines += lines;			
		}
	}

	if(total_obj != NULL)
		*total_obj = obj_cnt;

	if(total_lines != NULL)
		*total_lines = obj_lines;

	if(total_bytes != NULL)
		*total_bytes = obj_bytes;

	return rc;
}

// Metadata task for column default
void SqlData::CreateMetadataTaskForColumnDefault(SqlColMeta &col)
{
	if(col.no_default == true || col.default_value == NULL)
		return; 

	SqlMetaTask task;
	task.type = SQLDATA_CMD_ADD_DEFAULT;

	std::string schema = col.schema;
	std::string table = col.table;

	task.s_name = schema;
	task.s_name += '.';
	task.s_name += table;
	
	MapObjectName(schema, table, task.t_name);	

	task.columns = col.column;
	std::string exp = col.default_value;

	// Convert DEFAULT value (as expression) to the target syntax
	ConvertSql(exp, task.conditions);

	// Generate ALTER TABLE 
	task.statement = "ALTER TABLE ";
	task.statement += task.t_name;
	
	// in SQL Server 'ADD DEFAULT value FOR col'
	if(_target_type == SQL_SQL_SERVER)
	{
		task.statement += " ADD DEFAULT ";

        // Convert Sybase ASA autoupdate TIMESTAMP to GETDATE()
        if(_strnicmp(task.conditions.c_str(), "timestamp", 9) == 0)
            task.statement += "GETDATE()";
        else
		    task.statement += task.conditions;
		
        task.statement += " FOR ";
		task.statement += col.column;

		_meta_tasks.push_back(task);
	}
	else
	// in Oracle 'MODIFY (col DEFAULT value)'
	if(_target_type == SQL_ORACLE)
	{
		task.statement += " MODIFY (";
		task.statement += col.column;
		task.statement += " DEFAULT ";
		task.statement += task.conditions;
		task.statement += ")";
	
		_meta_tasks.push_back(task);
	}
	else
	// in PostgreSQL 'ALTER col SET DEFAULT value'
	if(_target_type == SQL_POSTGRESQL)
	{
		task.statement += " ALTER ";
		task.statement += col.column;
		task.statement += " SET DEFAULT ";
		task.statement += task.conditions;
	
		_meta_tasks.push_back(task);
	}
}

// Drop foreign keys defined on selected tables in the target database
int SqlData::DropReferences(int *all_keys, size_t *time_spent)
{
	if(_tables == NULL)
		return -1;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;
	int all = 0;
	
	for(std::list<std::string>::iterator i = _tables->begin(); i != _tables->end(); i++)
	{
		std::string t_table;
		int keys = 0;
		size_t time_s = 0;

		// Get the target name
		MapObjectName((*i), t_table);

		_db.DropReferences(SQLDB_TARGET_ONLY, t_table.c_str(), &time_s, &keys);

		// Foreign key exist on this table
		if(keys > 0)
		{
			SqlDataReply reply;
			reply._cmd = _command;
			reply._cmd_subtype = SQLDATA_CMD_DROP_FOREIGN_KEY;
			reply._time_spent = time_s;
			reply._int1 = keys;

			strcpy(reply._s_name, (*i).c_str());
			strcpy(reply._t_name, t_table.c_str());
		
			if(_callback != NULL)
				_callback(_callback_object, &reply);
		}

		all += keys;
	}

	if(all_keys != NULL)
		*all_keys = all;

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return 0;
}

// Initialize local (in-process) database interface
int SqlData::InitLocal(SqlDataReply & /*reply*/, int db_types, int *s_rc, int *t_rc)
{
	TRACE("SqlData InitLocal() Entered");

	_db.SetParameters(_parameters);
	_db.SetAppLog(_log);
	_db.SetSessionId(1);

	_db.SetColumnMapping(&_column_map);
    _db.SetDataTypeMapping(&_datatype_map);
	_db.SetTableSelectExpressions(&_tsel_exp_map);
	_db.SetTableSelectExpressionsAll(&_tsel_exp_all);
	_db.SetTableWhereConditions(&_twhere_cond_map);

	// Perform static initialization of the API libraries
	int rc = _db.InitStatic(db_types, _source_conn.c_str(), _target_conn.c_str(), s_rc, t_rc);

	// Initialize source and target database interfaces (continue connecting even if static init failed for one of the database, helps test connections)
	rc = _db.Init(db_types, _source_conn.c_str(), _target_conn.c_str(), s_rc, t_rc);

	TRACE("SqlData InitLocal() Left");
	return rc;
}

// Connect to source and target databases through in-process interface
int SqlData::ConnectLocal(SqlDataReply &reply, int db_types, int *s_rc, int *t_rc)
{
	// Connect to databases
	int rc = _db.Connect(db_types, reply, s_rc, t_rc);

	// Define database subtypes after connection
	if(rc != -1)
	{
		source_subtype = _db.GetSubType(SQLDB_SOURCE_ONLY);
		target_subtype = _db.GetSubType(SQLDB_TARGET_ONLY);

		SetSchemaMapping();
	}

	return rc;
}

// Set schema name mapping
void SqlData::SetSchemaMapping()
{
	const char *cur = Str::SkipSpaces(_schema_map_str.c_str());

	_schema_map.clear();

	while(*cur)
	{
		std::string s, t;

		// Get source schema name
		while(*cur && (*cur != ':' && *cur != ',' && *cur != ' '))
		{
			s += *cur;
			cur++;
		}

		cur = Str::SkipSpaces(cur);

		if(*cur == ':')
		{
			// If source is Oracle, convert schema name to upper case
			if(_source_type == SQLDATA_ORACLE || source_subtype == SQLDATA_ORACLE)
				std::transform(s.begin(), s.end(), s.begin(), ::toupper);

			cur++;

			cur = Str::SkipSpaces(cur);

			// Get target schema name
			while(*cur && (*cur != ',' && *cur != ' '))
			{
				t += *cur;
				cur++;
			}
		}
		// -smap=schema means -smap=*:schema
		else
		{
			t = s;
			s = "*";
		}

		_schema_map.insert(std::pair<std::string, std::string>(s,t));

		cur = Str::SkipSpaces(cur);

		// Skip mapping delimiter
		if(*cur == ',')
			cur++;

		cur = Str::SkipSpaces(cur);
	}
}

// Set table name mapping
void SqlData::SetTableMappingFromFile(std::string &file)
{
	// Mapping file size
	int size = File::GetFileSize(file.c_str());

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';
	char *cur = input;

	// Parse mapping file
	while(*cur)
	{
		std::string source;
		std::string target;

		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		// Get the source table name until ,
		while(*cur && *cur != ',')
		{
			source += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(source);

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

		// Get the target table name until new line
		while(*cur && *cur != '\r' && *cur != '\n')
		{
			target += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(target);

		_table_map.insert(std::pair<std::string, std::string>(source, target));
	}
}

// Set column name mapping
void SqlData::SetColumnMappingFromFile(std::string &file)
{
	// Mapping file size
	int size = File::GetFileSize(file.c_str());

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';

	char *cur = input;

	SqlColMap col;

	// Parse mapping file
	while(*cur)
	{
		col.schema.clear();
		col.table.clear();
		col.name.clear();
		col.t_name.clear();
		col.t_type.clear();

		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		std::string object;

		// Get schema.object until ,
		while(*cur && *cur != ',')
		{
			object += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(object);

		SqlApiBase::SplitQualifiedName(object.c_str(), col.schema, col.table);

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

		// Get source column name until ,
		while(*cur && *cur != ',')
		{
			col.name += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(col.name);

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

		// Get target column name until , or newline/tab
		while(*cur && *cur != ',' && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			col.t_name += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(col.t_name);

		if(*cur == ',')
			cur++;
		// Data type is optional
		else
		{
			_column_map.push_back(col);
			continue;
		}

		cur = Str::SkipSpaces(cur);

		// Get the target name until new line
		while(*cur && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			col.t_type += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(col.t_type);

		_column_map.push_back(col);
	}
}

// Set constraint name mapping
void SqlData::SetConstraintMappingFromFile(std::string &file)
{
	// Mapping file size
	int size = File::GetFileSize(file.c_str());

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';

	char *cur = input;

	// Parse mapping file
	while(*cur)
	{
		std::string table;
		std::string source_cns;
		std::string target_cns;

		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		// Get schema.table until ,
		while(*cur && *cur != ',')
		{
			table += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(table);

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

		// Get source constraint name until ,
		while(*cur && *cur != ',')
		{
			source_cns += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(source_cns);

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

		// Get target constraint name until , or newline or tab
		while(*cur && *cur != ',' && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			target_cns += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(target_cns);

		_cns_map.insert(std::pair<std::string, std::string>(table + ',' + source_cns, target_cns));
	}
}

// Set global data type mapping
void SqlData::SetDataTypeMappingFromFile(std::string &file)
{
	// Mapping file size
	int size = File::GetFileSize(file.c_str());

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';

	char *cur = input;

	SqlDataTypeMap dt;

	// Parse mapping file
	while(*cur)
	{
		dt.name.clear();
		dt.t_name.clear();
		
        dt.len_min = -1;
        dt.len_max = -1;
        dt.scale_min = -1;
        dt.scale_max = -1;

		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		// Get the source data type name until , or (
		while(*cur && *cur != ',' && *cur != '(')
		{
			dt.name += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(dt.name);

        // Length range
        if(*cur == '(')
        {
            cur++;
            cur = Str::SkipSpaces(cur);

            std::string len;

            if(*cur != '*')
            {
                // Get the minimal length until - ) or ,
		        while(*cur && *cur != '-' && *cur != ',' && *cur != ')')
		        {
			        len += *cur;
			        cur++;
		        }

                int num = -1;
                sscanf(len.c_str(), "%d", &num);

                dt.len_min = num;
                dt.len_max = num;
            }
            else
                cur++;

            cur = Str::SkipSpaces(cur);
            
            // Scale is also specified
            if(*cur == ',')
            {
                cur++;
                cur = Str::SkipSpaces(cur);

                std::string scale;

                if(*cur != '*')
                {
                    // Get the minimal scale until - or )
		            while(*cur && *cur != '-' && *cur != ')')
		            {
			            scale += *cur;
			            cur++;
		            }

                    Str::TrimTrailingSpaces(scale);

                    int num = -1;
                    sscanf(scale.c_str(), "%d", &num);

                    dt.scale_min = num;
                    dt.scale_max = num;
                }
                else
                    cur++;   
            }

            cur = Str::SkipSpaces(cur);
                
            if(*cur == ')')
                cur++;
        }

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

		// Get the target data type until new line
		while(*cur && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			dt.t_name += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(dt.t_name);

		_datatype_map.push_back(dt);
	}
}

// Set table select expressions from file
void SqlData::SetTableSelectExpressionsFromFile(std::string &file)
{
	// Configuration file with select expressions
	int size = File::GetFileSize(file.c_str());

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';
	char *cur = input;

	// Parse mapping file
	while(*cur)
	{
		std::string table;
		std::string selexp;

		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		// Get the source table name until , 
		while(*cur && *cur != ',')
		{
			table += *cur;
			cur++;
		}

		if(*cur != ',')
			break;

		Str::TrimTrailingSpaces(table);
		cur = Str::SkipSpaces(++cur);

		// Get the select expression until ; or end of file
		while(*cur && *cur != ';')
		{
			selexp += *cur;
			cur++;
		}

		if(*cur == ';')
			cur++;

		Str::TrimTrailingSpaces(selexp);
		_tsel_exp_map.insert(std::pair<std::string, std::string>(table, selexp));
	}
}

// Set table select expressions for all tables from file
void SqlData::SetTableSelectExpressionsAllFromFile(std::string &file)
{
	// Configuration file with select expressions
	int size = File::GetFileSize(file.c_str());

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';
	char *cur = input;

	// Parse file
	if(*cur)
	{
		cur = Str::SkipComments(cur);

		// Get the select expression until ; or end of file
		while(*cur && *cur != ';')
		{
			_tsel_exp_all += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(_tsel_exp_all);
	}
}

// Set table WHERE conditions from file
void SqlData::SetTableWhereConditionsFromFile(std::string &file)
{
	// Configuration file with WHERE conditions
	int size = File::GetFileSize(file.c_str());

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';
	char *cur = input;

	// Parse mapping file
	while(*cur)
	{
		std::string table;
		std::string where_cond;

		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		// Get the source table name until , 
		while(*cur && *cur != ',')
		{
			table += *cur;
			cur++;
		}

		if(*cur != ',')
			break;

		Str::TrimTrailingSpaces(table);
		cur = Str::SkipSpaces(++cur);

		// Get the select expression until ; or end of file
		while(*cur && *cur != ';')
		{
			where_cond += *cur;
			cur++;
		}

		if(*cur == ';')
			cur++;

		Str::TrimTrailingSpaces(where_cond);
		_twhere_cond_map.insert(std::pair<std::string, std::string>(table, where_cond));
	}
}

#if defined(WIN32) || defined(_WIN64)
// Start communication sessions with worker processes
unsigned int __stdcall SqlData::StartSessionsS(void *object)
{
	SqlData *sqlData = (SqlData*)object;

	if(sqlData == NULL)
		return (unsigned int)-1;

	return (unsigned int)sqlData->StartSessions();
}

// Start communication sessions with worker processes
int SqlData::StartSessions()
{
	// Create the first instance of the communication pipe
	HANDLE hp = CreateNamedPipe(SQLDATA_NAMED_PIPE, PIPE_ACCESS_DUPLEX, 
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 8192, 8192, 0, NULL);

	// Launch worker processes
	if(_workers > 0)
	{
		for(int i = 0; i < _max_sessions; i++)
			_spawnl(_P_NOWAIT, _worker_exe.c_str(), _worker_exe.c_str(), NULL);
	}

	bool more = true;

	// Connection loop
	while(more)
	{
		// Wait for client connection
		BOOL cn = ConnectNamedPipe(hp, NULL);

		bool connected = (cn == TRUE) ? true : false;

		// A connection error
		if(cn == FALSE)
		{
			size_t err = GetLastError();

			// if ERROR_PIPE_CONNECTED is returned, it is normal (client connected before ConnectNamedPipe called)
			if(err == ERROR_PIPE_CONNECTED)
				connected = true;
		}

		// If connection failed, for example, ERROR_NO_DATA means that client closed pipe on its end, reopen
		if(connected == false)
			CloseHandle(hp);

		// A client connected, start a worker thread
		if(connected == true)
		{
			SqlDataPipe *dp = new SqlDataPipe();
			dp->sd = this;
			dp->hp = hp;

			_beginthreadex(NULL, 0, &SqlData::StartWorkerS, dp, 0, NULL);
		}

		// Create another instance for further connections
		hp = CreateNamedPipe(SQLDATA_NAMED_PIPE, PIPE_ACCESS_DUPLEX, 
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 8192, 8192, 0, NULL);
	}

	return 0;
}
#endif

// Start local in-process workers
int SqlData::StartLocalWorkers()
{
	// Thread parameters (will be removed by thread) 
	void **data = new void*[2];
	data[0] = this;
	data[1] = &_db;

	int local = 0;

	// Define the number of in-process workers
	if(_local_workers == -1 || _local_workers > _max_sessions)
		local = _max_sessions;
	else
		local = _local_workers;

	// Start a local worker thread using existing database interface
	if(local >= 1)
	{
#if defined(WIN32) || defined(_WIN64)
		_beginthreadex(NULL, 0, &SqlData::StartLocalWorkerS, data, 0, NULL);
#else
		pthread_t thread;
		pthread_create(&thread, NULL, &SqlData::StartLocalWorkerS, data);
#endif
		_all_workers++;
	}

	// Start additional worker threads
	for(int i = 1; i < local; i++)
	{
		SqlDb *sqlDb = new SqlDb();

		// Thread parameters will be removed by thread
		data = new void*[2];
		data[0] = this;
		data[1] = sqlDb;

		sqlDb->SetMetaDb(&_db);
		sqlDb->SetParameters(_parameters);
		sqlDb->SetAppLog(_log);
		sqlDb->SetSessionId(i + 1);

		sqlDb->SetColumnMapping(&_column_map);
        sqlDb->SetDataTypeMapping(&_datatype_map);
		sqlDb->SetTableSelectExpressions(&_tsel_exp_map);
		sqlDb->SetTableSelectExpressionsAll(&_tsel_exp_all);
		sqlDb->SetTableWhereConditions(&_twhere_cond_map);

		SqlDataReply reply;

		int db_types = (_command != SQLDATA_CMD_ASSESS) ? SQLDB_BOTH : SQLDB_SOURCE_ONLY;
		
		// Connect to the databases
		sqlDb->Init(db_types, _source_conn.c_str(), _target_conn.c_str(), NULL, NULL);
		sqlDb->Connect(db_types, reply, NULL, NULL);

		sqlDb->SetCallbackRate(_callback_rate);
		sqlDb->SetCallback(this, CallbackS);

		data[1] = sqlDb;

#if defined(WIN32) || defined(_WIN64)
		_beginthreadex(NULL, 0, &SqlData::StartLocalWorkerS, data, 0, NULL);
#else
		pthread_t thread;
		pthread_create(&thread, NULL, &SqlData::StartLocalWorkerS, data);
#endif
		_all_workers++;
	}
	return 0;
}

// Start local in-process worker
#if defined(WIN32) || defined(_WIN64)
unsigned int __stdcall SqlData::StartLocalWorkerS(void *dt)
{
	if(dt == NULL)
		return (unsigned int)-1;
#else
void* SqlData::StartLocalWorkerS(void *dt)
{
	if(dt == NULL)
		return NULL;
#endif

	void **data = (void**)dt;

	SqlData *sqlData = (SqlData*)data[0];
	SqlDb *sqlDb = (SqlDb*)data[1];

	int rc = -1;

	if(sqlData != NULL && sqlDb != NULL)
	{
		// Use critical section to increment the number of workers
		Os::EnterCriticalSection(&sqlData->_worker_critical_section);

		sqlData->_running_workers++;

		Os::LeaveCriticalSection(&sqlData->_worker_critical_section);
	
		// Run the worker
		rc = sqlData->StartLocalWorker(sqlDb);
	}

	// Delete database API object if they do not belong to manager internal object
	if(sqlDb != &sqlData->_db)
		delete sqlDb;

	delete [] (char**)dt; 

#if defined(WIN32) || defined(_WIN64)
	return (unsigned int)rc;
#else
	return NULL;
#endif
}

int SqlData::StartLocalWorker(SqlDb *sqlDb)
{
	if(sqlDb == NULL)
		return -1;

	SqlDataReply reply;
	reply._cmd = _command;
	reply.session_id = sqlDb->GetSessionId();

	bool more = true;

	// Process tasks in a loop
	while(more)
	{
		// Get next table for processing
		std::string s_table = GetNextTask();
		std::string t_table;

		// No more tables for processing
		if(s_table.empty())
		{
			if(!GetNextQueryTask(reply))
			{
				more = false;
				break;
			}
		}
		else
		{
			// Get the target name
			MapObjectName(s_table, t_table);

			strcpy(reply._s_name, s_table.c_str());
			strcpy(reply._t_name, t_table.c_str());
		}

		// Execute transfer command
		if(_command == SQLDATA_CMD_TRANSFER)
		{
			if(_migrate_tables || _migrate_data)
				reply.rc = sqlDb->TransferRows(reply, _command_options, _migrate_tables, _migrate_data);
			else
			{
				reply.rc = 0;
				reply._cmd_subtype = SQLDATA_CMD_SKIPPED;
			}
		}
		else
		// Execute validate row cound command
		if(_command == SQLDATA_CMD_VALIDATE)
		{
			// Row count validation
			if(_command_options == SQLDATA_OPT_ROWCOUNT)
				reply.rc = sqlDb->ValidateRowCount(reply);
			else
			// Data validation
			if(_command_options == SQLDATA_OPT_ROWS)
				reply.rc = sqlDb->ValidateRows(reply);
		}
		else
		// Execute assessment command
		if(_command == SQLDATA_CMD_ASSESS)
		{
			reply.rc = sqlDb->AssessRows(reply);
		}

		// Notify that the table processing (data transfer) completed
		NotifyTableCompletion(s_table);

		// Notify on completion
		Callback(&reply);
	}

	*reply._s_name = '\x0';
	*reply._t_name = '\x0';

	more = true;

	// Process metadata and schema tasks in a loop
	while(more)
	{
		SqlMetaTask task;

		// Get next metadata task for processing
		bool exists = GetNextMetaTask(task);

		// No more tasks
		if(!exists)
		{
			more = false;
			break;
		}

		strcpy(reply._s_name, task.s_name.c_str());
		strcpy(reply._t_name, task.t_name.c_str());

		strcpy(reply.t_o_name, task.t_o_name.c_str());

		reply._cmd_subtype = (short)task.type;
		reply.t_sql = task.statement.c_str();

		// Execute a DDL command
		if(task.type == SQLDATA_CMD_ADD_DEFAULT || task.type == SQLDATA_CMD_ADD_PRIMARY_KEY || 
			task.type == SQLDATA_CMD_ADD_UNIQUE_KEY || task.type == SQLDATA_CMD_ADD_FOREIGN_KEY || 
			task.type == SQLDATA_CMD_ADD_COMMENT || task.type == SQLDATA_CMD_CREATE_INDEX || 
			task.type == SQLDATA_CMD_CREATE_TRIGGER)
		{
			reply.rc = sqlDb->ExecuteNonQuery(SQLDB_TARGET_ONLY, reply, task.statement.c_str());
		}
		else
		// Add a CHECK constraint (expression conversion was already performed)
		if(task.type == SQLDATA_CMD_ADD_CHECK_CONSTRAINT)
		{
			reply.rc = sqlDb->ExecuteNonQuery(SQLDB_TARGET_ONLY, reply, task.statement.c_str());
		}
		else
		if(task.type == SQLDATA_CMD_CREATE_SEQUENCE)
		{
			reply.rc = sqlDb->CreateSequence(SQLDB_TARGET_ONLY, reply, task.statement.c_str(), task.t_o_name.c_str());
		}

		// Notify that the meta task completed
		NotifyMetaCompletion(task);
		
		// Notify on completion
		Callback(&reply);
	}
	
	NotifyWorkerExit();

	return 0;
}

// Start a communication with a worker process
#if defined(WIN32) || defined(_WIN64)
unsigned int __stdcall SqlData::StartWorkerS(void *object)
{
	SqlDataPipe *sp = (SqlDataPipe*)object;

	if(sp == NULL)
		return (unsigned)-1;

	int rc = sp->sd->StartWorker(sp->hp);

	delete sp;

	return (unsigned int)rc;
}
#endif

#if defined(WIN32) || defined(_WIN64)
// Start a communication with a worker process
int SqlData::StartWorker(HANDLE hp)
{
	SqlDataCommand cmd;
	SqlDataReply reply;

	DWORD written, read;
	BOOL rcb;

	// Send te connection command with source and target connection strings
	cmd._cmd = SQLDATA_CMD_CONNECT;
	strcpy(cmd._s_name, _source_conn.c_str());
	strcpy(cmd._t_name, _target_conn.c_str());

	// Send the command to the client
	rcb = WriteFile(hp, &cmd, sizeof(SqlDataCommand), &written, NULL);  
	// Wait for a reply from the worker process
	rcb = ReadFile(hp, &reply, sizeof(SqlDataReply), &read, NULL);

	bool more = true;

	// Process tasks in a loop
	while(more)
	{
		// Get next table for processing
		std::string table = GetNextTask();

		// No more tables for processing
		if(table.empty() == true)
		{
			more = false;
			break;
		}

		strncpy(cmd._s_name, table.c_str(), 256);

		// Send the command to the client
		rcb = WriteFile(hp, &cmd, sizeof(SqlDataCommand), &written, NULL);  

		// Wait for a reply from the worker process
		rcb = ReadFile(hp, &reply, sizeof(SqlDataReply), &read, NULL);
	}

	cmd._cmd = SQLDATA_CMD_NO_MORE_TASKS;	

	// Notify the worker that it can finish
	rcb = WriteFile(hp, &cmd, sizeof(SqlDataCommand), &written, NULL);  

	FlushFileBuffers(hp); 
	DisconnectNamedPipe(hp); 
	CloseHandle(hp); 

	return 0;
}
#endif

// Notify that the table processing (data transfer) completed
void SqlData::NotifyTableCompletion(std::string table)
{
	Os::EnterCriticalSection(&_task_queue_critical_section);

	_tables_in_progress.remove(table);

	Os::LeaveCriticalSection(&_task_queue_critical_section);
}

// Notify that the meta data task completed
void SqlData::NotifyMetaCompletion(SqlMetaTask &task)
{
	Os::EnterCriticalSection(&_task_queue_critical_section);

	// Find the task in the list
	for(std::list<SqlMetaTask>::iterator i = _meta_in_progress.begin(); i != _meta_in_progress.end(); i++)
	{
		if((*i).type == task.type && (*i).statement == task.statement)
		{
			_meta_in_progress.erase(i);
			break;
		}
	}

	Os::LeaveCriticalSection(&_task_queue_critical_section);
}

// Worker thread is terminating work
void SqlData::NotifyWorkerExit()
{
	Os::EnterCriticalSection(&_worker_critical_section);

	// Decrement the number of workers
	_running_workers--;

	if(_running_workers == 0)
	{
#if defined(WIN32) || defined(_WIN64)
		SetEvent(_completed_event);
#else
		Os::SetEvent(&_completed_event);
#endif
	}

	Os::LeaveCriticalSection(&_worker_critical_section);
}

// Get next table for processing
std::string SqlData::GetNextTask()
{
	Os::EnterCriticalSection(&_task_queue_critical_section);

	std::string table;

	// Get the next table from the queue
	if(_tables != NULL && _tables->empty() == false)
	{
		table = _tables->front();
		_tables->pop_front();

		_tables_in_progress.push_back(table);
	}

	Os::LeaveCriticalSection(&_task_queue_critical_section);
	return table;
}

// Get next query for processing
bool SqlData::GetNextQueryTask(SqlDataReply &reply)
{
	bool exists = false;

	Os::EnterCriticalSection(&_task_queue_critical_section);

	// Get the next query from the queue
	if(_queries != NULL && _queries->empty() == false)
	{
		std::map<std::string, std::string>::iterator i = _queries->begin();
		if(i != _queries->end())
		{
			const std::string &table = (*i).first;

			strcpy(reply._s_name, table.c_str());
			strcpy(reply._t_name, table.c_str());
			reply.s_sql_l = (*i).second;

			_tables_in_progress.push_back(table);
			_queries->erase(i);

			exists = true;
		}
	}

	Os::LeaveCriticalSection(&_task_queue_critical_section);
	return exists;
}

// Get next metadata/schema task for processing
bool SqlData::GetNextMetaTask(SqlMetaTask &task)
{
	SqlMetaTask first_task;

	bool exists = false;
	bool first = true;

	Os::EnterCriticalSection(&_task_queue_critical_section);

	// Get the next task from the queue
	while(_meta_tasks.empty() == false)
	{
		task = _meta_tasks.front();
		_meta_tasks.pop_front();

		// Check whether data of the table already transferred
		bool data_complete = IsDataTransferred(task.s_name);

		bool pk_created = true;

		// For foreign key task check if the primary key was already created
		if(task.type == SQLDATA_CMD_ADD_FOREIGN_KEY && data_complete == true)
			pk_created = IsPrimaryKeyCreated(task);

		// For trigger task, check that all sequences already created
		if(task.type == SQLDATA_CMD_CREATE_TRIGGER && data_complete == true && pk_created == true)
			pk_created = IsSequencesCreated();

		// Due to locks, do not perform DDL operations on the same object in parallel
		bool ddl_exists = IsDdlRunning(task.s_name);

		// We cannot execute this task now
		if(data_complete == false || pk_created == false || ddl_exists)
		{
			// Save the first task to prevent loops
			if(first == true)
			{
				// Return task back to the list
				_meta_tasks.push_back(task);

				// Save pointers
				first_task.type = task.type;
				first_task.s_name = task.s_name;
				first_task.statement = task.statement;

				first = false;
			}
			else
			{
				// Make sure we did not select the first task again (loop)
				if(first_task.type == task.type && first_task.s_name == task.s_name &&
					first_task.statement == task.statement)
				{
					// Temporary leave the critical section to allow other threads to do work 
					Os::LeaveCriticalSection(&_task_queue_critical_section);
					Os::Sleep(1);
					Os::EnterCriticalSection(&_task_queue_critical_section);
				}

				// Return task back to the list
				_meta_tasks.push_back(task);
			}

			continue;
		}

		exists = true;
		break;
	}

	_meta_in_progress.push_back(task);

	Os::LeaveCriticalSection(&_task_queue_critical_section);
	return exists;
}

// Convert SQL statement
void SqlData::ConvertSql(std::string &in, std::string &out)
{
	if(_sqlParser == NULL)
		return;

	Os::EnterCriticalSection(&_task_queue_critical_section);

	const char *output = NULL;
	int out_size = 0;

	// Convert the file
	int rc = ::ConvertSql(_sqlParser, in.c_str(), (int)in.size(), &output, &out_size, NULL);

	if(rc == 0)
		out = output;

	FreeOutput(output);

	Os::LeaveCriticalSection(&_task_queue_critical_section);
}

// Check whether data of the table already transferred
bool SqlData::IsDataTransferred(std::string &table)
{
	// Check tables that are in progress now
	std::list<std::string>::iterator i = std::find(_tables_in_progress.begin(), _tables_in_progress.end(), table);

	if(i != _tables_in_progress.end())
		return false;

	// Check tables that are not selected yet
	if(_tables != NULL)
	{
		i = std::find(_tables->begin(), _tables->end(), table);

		if(i != _tables->end())
			return false;
	}

	return true;
}

// For foreign key task check if the primary key was already created (already in critical session)
bool SqlData::IsPrimaryKeyCreated(SqlMetaTask &task)
{
	if(task.type != SQLDATA_CMD_ADD_FOREIGN_KEY)
		return false;

	// First check currently executed metadata tasks
	for(std::list<SqlMetaTask>::iterator i = _meta_in_progress.begin(); i != _meta_in_progress.end(); i++)
	{
		// Check any primary key
		if((*i).type == SQLDATA_CMD_ADD_PRIMARY_KEY)
			return false;
	}

	// Check not selected tasks
	for(std::list<SqlMetaTask>::iterator i = _meta_tasks.begin(); i != _meta_tasks.end(); i++)
	{
		// Check any primary key
		if((*i).type == SQLDATA_CMD_ADD_PRIMARY_KEY)
			return false;
	}

	return true;
}

// Check is a DDL task is running for the specified object (already in critical session)
bool SqlData::IsDdlRunning(std::string &table)
{
	// Check currently executed metadata tasks
	for(std::list<SqlMetaTask>::iterator i = _meta_in_progress.begin(); i != _meta_in_progress.end(); i++)
	{
		// Compare table name
		if(_stricmp(table.c_str(), (*i).s_name.c_str()) == 0)
			return true;
	}

	return false;
}

// For trigger task, check that all sequences already created (already in critical session)
bool SqlData::IsSequencesCreated()
{
	// First check currently executed metadata tasks
	for(std::list<SqlMetaTask>::iterator i = _meta_in_progress.begin(); i != _meta_in_progress.end(); i++)
	{
		// Check any create sequence
		if((*i).type == SQLDATA_CMD_CREATE_SEQUENCE)
			return false;
	}

	// Check not selected tasks
	for(std::list<SqlMetaTask>::iterator i = _meta_tasks.begin(); i != _meta_tasks.end(); i++)
	{
		// Check any create sequence
		if((*i).type == SQLDATA_CMD_CREATE_SEQUENCE)
			return false;
	}

	return true;
}

// Get the target name
void SqlData::MapObjectName(std::string &source, std::string &target)
{
	// Check if the mapping for this table name is specified
	std::map<std::string, std::string>::iterator i = _table_map.find(source);

	if(i != _table_map.end())
	{
		target = i->second;
		return;
	}

	if(_schema_map.empty() == true)
	{
		target = source;
		return;
	}

	std::string s_schema, s_table;

	const char *cur = source.c_str();

	// Get source schema name
	while(*cur && *cur != '.')
	{
		s_schema += *cur;
		cur++; 
	}

	if(*cur == '.')
		cur++;

	// Get source table name
	while(*cur)
	{
		s_table += *cur;
		cur++; 
	}

	if(s_table.empty())
	{
		s_table = s_schema;
		s_schema.clear();
	}

	MapObjectName(s_schema, s_table, target, false);
}

void SqlData::MapObjectName(std::string &s_schema, std::string &s_table, std::string &t_name, bool tmap)
{
	std::string t_schema;
	std::string t_table;
	
	// Check if the mapping for this table name is specified
	if(tmap)
	{
		std::string source;

		if(!s_schema.empty())
			source = s_schema + '.';

		source += s_table;

		std::map<std::string, std::string>::iterator i = _table_map.find(source);

		if(i != _table_map.end())
		{
			t_name = i->second;
			return;
		}
	}
	
	MapObjectName(s_schema, s_table, t_schema, t_table);

	if(t_schema.empty() == false)
	{
		t_name = t_schema;
		t_name += '.';
		t_name += t_table;
	}
	else
		t_name = t_table;
}

void SqlData::MapObjectName(std::string &s_schema, std::string &s_table, std::string &t_schema, std::string &t_table)
{
	// Map schema name
	if(s_schema.empty() == false)
	{
		std::string s_schema2 = s_schema;

		// Schema mapping is defined
		if(_schema_map.empty() == false)
		{
			// If source is Oracle, convert schema name to upper case
			if(_source_type == SQLDATA_ORACLE || source_subtype == SQLDATA_ORACLE)
				std::transform(s_schema2.begin(), s_schema2.end(), s_schema2.begin(), ::toupper);

			// Find the mapped name for this schema
			std::map<std::string, std::string>::iterator i = _schema_map.find(s_schema2);

			// If not found, check if there is a mapping for all schemas
			if(i == _schema_map.end())
			{
				i = _schema_map.find("*");

				// Found
				if(i != _schema_map.end())
					t_schema = i->second;
				else
					t_schema = s_schema;
			}
			else
				// Found
				t_schema = i->second;
		}
		else
			t_schema = s_schema;
	}

	// Map table name
	if(s_table.empty() == false)
	{
		if(_target_type == SQL_SQL_SERVER && _source_type != SQL_SQL_SERVER)
		{
			t_table = "[";
			if(s_table.at(0) == '"' && s_table.length() > 2)
			{
				t_table += s_table.substr(1, s_table.length() - 2);
			}
			else
				t_table += s_table;

			t_table += "]";
		}
        else
        // In SQL Server, name can be enclosed with []
        if(_source_type == SQL_SQL_SERVER && _target_type != SQL_SQL_SERVER)
		{
			if(s_table.at(0) == '[' && s_table.length() > 2)
            {
                char enc = '"';

                if(_target_type == SQL_MYSQL)
                    enc = '`';
                
                t_table = enc;
                t_table += s_table.substr(1, s_table.length() - 2);
                t_table += enc;
            } 
			else
				t_table += s_table;
		}
		else
			t_table = s_table;
	}
}

// Map the constraint name
void SqlData::MapConstraintName(const char *source, std::string &target, char type, std::string &table, std::string &p_table) 
{
	if(source == NULL)
		return;

	// Check if the mapping for this table constraint name is specified
	std::map<std::string, std::string>::iterator i = _cns_map.find(std::string(table) + ',' + std::string(source));

	if(i != _cns_map.end())
	{
		target = i->second;
		return;
	}

	size_t len = strlen(source);

	// Do not set the constraint name if it is system-generated
	if((strncmp(source, "SYS_C0", 6) == 0) || (strncmp(source, "ASA", 3) == 0 && len <= 8))
		return;

	// Foreign key constraint can be equal to PK table name (by default in ASA), while
	// in SQL Server constraints, tables must be unique in the database
	if(type == 'R' && _target_type == SQL_SQL_SERVER)
	{
		const char *tab = Str::SkipUntil(p_table.c_str(), '.');

		if(*tab != '\x0')
			tab++;
		else
			tab = p_table.c_str();

		if(_stricmp(source, tab) == 0)
			return;
	}

	std::string name = source;

	// SQL Server constraint names must be unique within the database
	if(_source_type != SQL_SQL_SERVER && _target_type == SQL_SQL_SERVER)
	{
		// Check if this name already used
		std::map<std::string, int>::iterator i = _constraint_ref.find(source);

		if(i != _constraint_ref.end())
		{
			// Increment the counter in map
			(*i).second++;

			char num_str[11]; 
			sprintf(num_str, "%d", (*i).second);

			name += num_str;
		}
		else 
			_constraint_ref[source] = 1;
	}
		
	if(_source_type != SQL_SQL_SERVER && _target_type == SQL_SQL_SERVER)
		target += '[';

	target += name;

	if(_source_type != SQL_SQL_SERVER && _target_type == SQL_SQL_SERVER)
		target += ']';
}

// Get the target column name and data type
void SqlData::MapColumn(const char *s_table, std::string &s_name, std::string &t_name, std::string &t_type)
{
    _db.MapColumn(s_table, s_name.c_str(), t_name, t_type);
}

// Define foreign key ON UPDATE/ON DELETE action
const char* SqlData::GetForeignKeyAction(char action)
{
	const char *act = NULL;

	// RESTRICT in Sybase ASA
	if(action == FK_ACTION_RESTRICT)
	{
		if(_target_type == SQL_SQL_SERVER)
			act = "NO ACTION";
		else 
			act = "RESTRICT";
	}
	else
	// CASCADE
	if(action == FK_ACTION_CASCADE)
		act = "CASCADE";
	else
	// SET NULL
	if(action == FK_ACTION_SET_NULL)
		act = "SET NULL";
	else
	// SET DEFAULT
	if(action == FK_ACTION_SET_DEFAULT)
		act = "SET DEFAULT";

	return act;
}

// Convert identifier between databases 
void SqlData::ConvertIdentifier(const char *in, std::string &out)
{
	if(in == NULL)
		return;

	const char *cur = in;
	char quoted = '\x0';

	// Informix index can start with a space following by a digit, replace space with a alphabetic char
	if(*cur == ' ')
	{
		out = 'n';
		cur++;
	}
	else
	// In Sybase ASA index name can start with an digit
	if(*cur >= '0' && *cur <= '9')
	{
		out = '"';
		quoted = '"';
	}

	while(*cur != '\x0')
	{
		out += *cur;
		cur++;
	}

	if(quoted != '\x0')
		out += quoted;

    // In Oracle maximum length of the identifier is 30 characters
    if(_target_type == SQLDATA_ORACLE && out.length() >= 30)
    {
        std::string trimmed;
				
		Str::TrimToAbbr(out, trimmed, 30);
		out = trimmed;
    }
}

void SqlData::SetCallback(void *object, SqlDataCallbackFunc func) 
{ 
	_callback_object = object, 
	_callback = func; 

	// Set the callback to the local worker too
	_db.SetCallback(this, CallbackS);

	_db.SetCallbackRate(_callback_rate);
}

// Callbacks
void SqlData::Callback(SqlDataReply *reply)
{
	if(reply == NULL)
		return;

	Os::EnterCriticalSection(&_worker_critical_section);

	if(reply->_cmd_subtype == SQLDATA_CMD_COMPLETE)
	{
		_s_total_rows += reply->_s_int1;
		_t_total_rows += reply->_t_int1;
		_s_total_bytes += reply->_s_bigint1;
		_t_total_bytes += reply->_t_bigint1;
	}

	if(_callback)
		_callback(_callback_object, reply);

	Os::LeaveCriticalSection(&_worker_critical_section);
}

void SqlData::CallbackS(void *object, SqlDataReply *reply)
{
	if(object == NULL)
		return;

	SqlData *sqlData = (SqlData*)object;

	sqlData->Callback(reply);
}

// Set the number of concurrent sessions
int SqlData::SetConcurrentSessions(int max_sessions, int table_count)
{
	// Max sessions cannot exceed the number of tables (do not count metadata tasks as they typicaly wait data load completion)
	_max_sessions = (max_sessions <= table_count) ? max_sessions : table_count;

	return _max_sessions;
}

// Check if identity column defined for the table
bool SqlData::IsIdentityDefined(std::string &schema, std::string &table)
{
	return _db.IsIdentityDefined(SQLDB_SOURCE_ONLY, schema, table);
}

// Get primary or unique key columns
void SqlData::GetKeyConstraintColumns(SqlMetaTask &task, SqlConstraints &cns, std::string &cols)
{
	std::list<std::string> lcols;
	_db.GetKeyConstraintColumns(SQLDB_SOURCE_ONLY, cns, lcols);

	int c = 0;

	for(std::list<std::string>::iterator i = lcols.begin(); i != lcols.end(); i++)
	{
		if(c > 0)
			cols += ", ";

		std::string column;
		std::string type;

		_db.MapColumn(task.s_name.c_str(), (*i).c_str(), column, type);

		cols += column;
		c++;
	}
}

// Get foreign key columns
void SqlData::GetForeignKeyConstraintColumns(SqlMetaTask &task, SqlConstraints &cns, std::string &fcols, std::string &pcols, std::string &ptable)
{
	std::list<std::string> lfcols;
	std::list<std::string> lpcols;

	_db.GetForeignKeyConstraintColumns(SQLDB_SOURCE_ONLY, cns, lfcols, lpcols, ptable);

	int c = 0;

	for(std::list<std::string>::iterator i = lfcols.begin(); i != lfcols.end(); i++)
	{
		if(c > 0)
			fcols += ", ";

		std::string column;
		std::string type;

		_db.MapColumn(task.s_name.c_str(), (*i).c_str(), column, type);

		fcols += column;
		c++;
	}

	c = 0;

	for(std::list<std::string>::iterator i = lpcols.begin(); i != lpcols.end(); i++)
	{
		if(c > 0)
			pcols += ", ";

		std::string column;
		std::string type;

		_db.MapColumn(ptable.c_str(), (*i).c_str(), column, type);

		pcols += column;
		c++;
	}
}

// Get index columns
void SqlData::GetIndexColumns(SqlIndexes &idx, std::string &cols)
{
	std::list<std::string> lcols;
	std::list<std::string> lsorts;
    _db.GetIndexColumns(SQLDB_SOURCE_ONLY, idx, lcols, lsorts);

	int c = 0;
	std::list<std::string>::iterator s = lsorts.begin();
	for(std::list<std::string>::iterator i = lcols.begin(); i != lcols.end(); i++, s++)
	{
		if(c > 0)
			cols += ", ";

        std::string t_name;
		std::string t_type;

		// Get the target column name
        MapColumn(idx.t_name, (*i), t_name, t_type);
		cols += t_name;

		// Sort direction
		if(!(*s).empty())
		{
			cols += " ";
			cols += (*s);
		}

		c++;
	}       
}

// Generate statement to emulate identity in Oracle (sequence and trigger) and PostgreSQL (sequence and default)
void SqlData::GetIdentityMetaTask(SqlColMeta &col)
{
	if(col.identity == false)
		return;

	SqlMetaTask task;	

	std::string schema = col.schema;
	std::string table = col.table;

	task.s_name = schema;
	task.s_name += '.';
	task.s_name += table;

	MapObjectName(schema, table, task.t_name);
		
	// Sequence name
	task.t_o_name = task.t_name + "_seq";

	// Drop the sequence first
	/*task.type = SQLDATA_CMD_DROP_SEQUENCE;	
	task.statement = "DROP SEQUENCE ";
	task.statement += seq;

	_meta_tasks.push_back(task); */

	char start_str[11]; 
	char inc_str[11]; 

	int start = (col.id_next > 0) ? col.id_next : col.id_start;

	sprintf(start_str, "%d", start);
	sprintf(inc_str, "%d", col.id_inc);

	// Create sequence
	task.type = SQLDATA_CMD_CREATE_SEQUENCE;	
	task.statement = "CREATE SEQUENCE ";
	task.statement += task.t_o_name;
	task.statement += " START WITH ";
	task.statement += start_str;
	task.statement += " INCREMENT BY ";
	task.statement += inc_str;

	_meta_tasks.push_back(task);

	std::string column;
	std::string type;

	_db.MapColumn(task.s_name.c_str(), col.column, column, type);
	
	// Create trigger for Oracle
	if(_target_type == SQLDATA_ORACLE)
	{
		task.type = SQLDATA_CMD_CREATE_TRIGGER;
		task.statement = "CREATE OR REPLACE TRIGGER ";
		task.statement += task.t_o_name;
		task.statement += "_tr\n BEFORE INSERT ON ";
		task.statement += task.t_name;
		task.statement += " FOR EACH ROW\n";
		task.statement += " WHEN (NEW.";
		task.statement += column;
		task.statement += " IS NULL OR NEW.";
		task.statement += column;
		task.statement += " = 0)\n";
		task.statement += "BEGIN\n";
		task.statement += " SELECT ";
		task.statement += task.t_o_name;
		task.statement += ".NEXTVAL INTO :NEW.";
		task.statement += column;
		task.statement += " FROM dual;\n";
		task.statement += "END;\n";
	}
	else
	// Add DEFAULT for PostgreSQL
	if(_target_type == SQLDATA_POSTGRESQL)
	{
		task.type = SQLDATA_CMD_ADD_DEFAULT;
		task.statement = "ALTER TABLE ";
		task.statement += task.t_name;
		task.statement += " ALTER COLUMN ";
		task.statement += column;
		task.statement += " SET DEFAULT nextval ('";
		task.statement += task.t_o_name;
		task.statement += "')";
	}
   
	_meta_tasks.push_back(task);
}

// Set parameters
void SqlData::SetParameters(Parameters *p) 
{ 
	_parameters = p; 

	if(_parameters->GetTrue("-trace") != NULL)
		_trace = true;

	if(_parameters->GetFalse("-ddl_tables") != NULL)
		_migrate_tables = false;

	if(_parameters->GetFalse("-data") != NULL)
		_migrate_data = false;

	if(_parameters->GetFalse("-constraints") != NULL)
		_migrate_constraints = false;

	if(_parameters->GetFalse("-indexes") != NULL)
		_migrate_indexes = false;
}

// Get errors on local DB interface
void SqlData::GetErrors(SqlDataReply &reply)
{
	_db.GetErrors(reply);
}


