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

#if defined(WIN32) || defined(WIN64)
#include <conio.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include <stdio.h>
#include "sqldatacmd.h"
#include "str.h"
#include "sqldb.h"
#include "os.h"
#include "file.h"

// Constructor
SqlDataCmd::SqlDataCmd()
{
	_command = 0;
	_command_options = 0;
	_command_start = 0;

	_source_type = 0;
	_target_type = 0;

	_concurrent_sessions = 4;
	_workers = 0;
	_local_workers = 1;

	_total_tables = 0;
	_failed_tables = 0;
	_total_objects = 0;
	_total_object_lines = 0;
	_total_object_bytes = 0;

	_total_target_ddl = 0;
	_failed_target_ddl = 0;

	_tables_equal = 0;
	_tables_diff = 0;

	_exe = NULL;

	_ui_callback = NULL;
	_ui_callback_object = NULL;

	_transfer_table_num = 1;
	_transfer_obj_num = 1;
	_validate_table_num = 1;
	_assess_table_num = 1;

	_td_stdout = false;
}

// Run the data transfer or validation
int SqlDataCmd::Run()
{
	_command_start = Os::GetTickCount();
	
	_total_tables = 0;
	_failed_tables = 0;
	
	_total_target_ddl = 0;
	_failed_target_ddl = 0;

	_tables_equal = 0;
	_tables_diff = 0;

	// Start writing new log files
	_log.Reset();
	_ddl_log.Reset();
	_failed_ddl_log.Reset();
	_failed_tables_log.Reset();

	_failed_tables_list.clear();
	_warnings_tables_list.clear();
	_tables_diff_list.clear();

	// Read and validate parameters
	int rc = SetParameters();

	// Command line or parameters error
	if(rc == -1)
		return rc;

	PrintCurrentTimestamp();
	_log.LogFile("\nCurrent directory: %s", Os::CurrentWorkingDirectory());
	_log.LogFile("\nConfiguration file: %s", _parameters.GetConfigFile());

	// Check status for configuration file reading
	if(!_parameters.IsCfgReadOk())
		_log.LogFile("\n\nError reading the configuration file: %s", _parameters.GetCfgReadError());

	// Connect to the database to read metadata
	rc = Connect();

	if(rc == -1)
	{
		SqlDataReply reply;
		reply._cmd = _command;
		reply._cmd_subtype = SQLDATA_CMD_ALL_COMPLETE;
		_sqlData.GetErrors(reply);
		
		// Notify UI on error completion
		if(_ui_callback != NULL)
			_ui_callback(_ui_callback_object, &reply);

		return -1;
	}

	// Read the database catalog to define objects and metadata
	rc = ReadMetadata();

	// No tables selected
	if(_total_tables == 0 && _total_objects == 0)
		return 0;

	int workers = (_command != SQLDATA_CMD_TRANSFER_OBJ) ? _sqlData.SetConcurrentSessions(_concurrent_sessions, _total_tables) : 1;
	
	if(_command == SQLDATA_CMD_TRANSFER)
	{
		// Remove foreign keys for selected tables if DROP or TRUNCATE options are set
		rc = DropReferences();

		_log.Log("\n\nTransferring database ");
		_transfer_table_num = 1;
	}
	else
	if(_command == SQLDATA_CMD_TRANSFER_OBJ)
	{
		_log.Log("\n\nTransferring database objects ");
		_transfer_obj_num = 1;
	}
	else
	if(_command == SQLDATA_CMD_VALIDATE)
	{
		if(_command_options == SQLDATA_OPT_ROWCOUNT)
			_log.Log("\n\nValidating table row count ");
		else
		if(_command_options == SQLDATA_OPT_ROWS)
			_log.Log("\n\nValidating table data ");
	
		_validate_table_num = 1;
	}
	else
	if(_command == SQLDATA_CMD_ASSESS)
	{
		_log.Log("\n\nRunning assessment ");
		_assess_table_num = 1;
	}

	_log.Log("(%d %s):\n", workers, (workers == 1) ? "session" : "concurrent sessions");

	// Process tables or objects
	if(rc != -1)
		rc = _sqlData.Run();

	PrintCurrentTimestamp();

	printf("\n");

	return rc;
}

// Callback (already called from a critical section)
void SqlDataCmd::Callback(SqlDataReply *reply)
{
	if(reply == NULL || reply->_cmd_subtype == SQLDATA_CMD_SKIPPED)
		return;

	if(reply->_cmd == SQLDATA_CMD_TRANSFER)
		CallbackTransfer(reply);
	else
	if(reply->_cmd == SQLDATA_CMD_VALIDATE)
	{
		if(_command_options == SQLDATA_OPT_ROWCOUNT)
			CallbackValidationRowCount(reply);
		else
		if(_command_options == SQLDATA_OPT_ROWS)
			CallbackValidationRows(reply);
	}
	else
	if(reply->_cmd == SQLDATA_CMD_ASSESS)
		CallbackAssess(reply);

	// Update UI module
	if(_ui_callback != NULL)
		_ui_callback(_ui_callback_object, reply);
}

// Callback function for transfer task
void SqlDataCmd::CallbackTransfer(SqlDataReply *reply)
{
	if(reply == NULL)
		return;

	char time_fmt[21];

	// Object name
	if(reply->_cmd_subtype != SQLDATA_CMD_COMPLETE_WITH_DDL_ERROR && 
			reply->_cmd_subtype != SQLDATA_CMD_DROP_FOREIGN_KEY && 
			reply->_cmd_subtype != SQLDATA_CMD_ALL_COMPLETE && reply->_cmd_subtype != SQLDATA_CMD_DUMP_DDL)
		_log.Log("\n  %s - ", reply->_s_name);

	if(reply->_cmd_subtype == SQLDATA_CMD_STARTED)
	{
		_log.Log("Started (%d of %d, session %d)", _transfer_table_num, _total_tables, reply->session_id);
		_transfer_table_num++;
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_OPEN_CURSOR)
	{
		Str::FormatTime((size_t)reply->_s_int2, time_fmt);

		_log.Log("Open cursor (");

		if(reply->rc != -1)
		{
			_log.Log("%d row%s read, %s, session %d)", reply->_s_int1, SUFFIX(reply->_s_int1), time_fmt, reply->session_id);
		
			// Warning can be returned, for example in case data truncation
			if(reply->rc == 1)
			{
				_log.Log("\n    Warning: %s", reply->s_native_error_text);
				AddWarningsTable(reply, reply->s_native_error_text);
			}
		}
		else
		{
			_log.Log("Failed, %s, session %d)\n    %s", time_fmt, reply->session_id, reply->s_native_error_text);
			AddFailedTable(reply, reply->s_native_error_text);
		}
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_CREATE)
	{
		Str::FormatTime((size_t)reply->_int1, time_fmt);

		_log.Log("Create target table (");

		if(reply->rc == -1)
		{
			_log.Log("Failed, %s, session %d)\n    %s", time_fmt, reply->session_id, reply->t_native_error_text);
			_failed_target_ddl++;
		}
		else
			_log.Log("%s, session %d)", time_fmt, reply->session_id);

		_total_target_ddl++;

		LogSql(reply, time_fmt);
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_DROP || reply->_cmd_subtype == SQLDATA_CMD_TRUNCATE ||
		reply->_cmd_subtype == SQLDATA_CMD_DROP_SEQUENCE)
	{
		Str::FormatTime((size_t)reply->_int1, time_fmt);

		if(reply->_cmd_subtype == SQLDATA_CMD_DROP)
			_log.Log("Drop target table (");
		else 
		if(reply->_cmd_subtype == SQLDATA_CMD_TRUNCATE)
			_log.Log("Truncate target table (");
		else 
		if(reply->_cmd_subtype == SQLDATA_CMD_DROP_SEQUENCE)
			_log.Log("Drop sequence %s (", reply->t_o_name);

		// Check whether the table does not exist
		if(reply->rc == -1)
		{
			if(reply->t_error == SQL_DBAPI_NOT_EXISTS_ERROR)
				_log.Log("not exists, %s, session %d)", time_fmt, reply->session_id);
			else
			{
				_log.Log("Failed, %s, session %d)\n    %s", time_fmt, reply->session_id, reply->t_native_error_text);
				_failed_target_ddl++;
			}
		}
		else
			_log.Log("%s, session %d)", time_fmt, reply->session_id);

		_total_target_ddl++;
		LogSql(reply, time_fmt);
	}
	else
	// Dropping foreign keys before the transfer
	if(reply->_cmd_subtype == SQLDATA_CMD_DROP_FOREIGN_KEY)
	{
		Str::FormatTime(reply->_time_spent, time_fmt);
		_log.Log("\n  %s (%d key%s, %s)", reply->_t_name, reply->_int1, SUFFIX(reply->_int1), time_fmt);
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_IN_PROGRESS || reply->_cmd_subtype == SQLDATA_CMD_COMPLETE)
	{
		double r_speed_rows = 0, w_speed_rows = 0, w_speed_bytes = 0;

		// Calculate number of rows read per unit of time
		if(reply->_s_int1 !=0 && reply->_s_int2 != 0)
			r_speed_rows = ((double)reply->_s_int1)/((double)reply->_s_int2) * 1000.0;

		if(reply->_t_int2 != 0)
		{
			if(reply->_t_int1 != 0)
				w_speed_rows = ((double)reply->_t_int1)/((double)reply->_t_int2)*1000.0;

			if(reply->_t_bigint1 != 0)
				w_speed_bytes = ((double)reply->_t_bigint1)/((double)reply->_t_int2)*1000.0;
		}

		char w_bytes_fmt[21], w_speed_bytes_fmt[21];
		char tr_time_fmt[21], r_time_fmt[21], w_time_fmt[21];

		// Format numbers
		Str::FormatByteSize((double)reply->_t_bigint1, w_bytes_fmt);
		Str::FormatByteSize(w_speed_bytes, w_speed_bytes_fmt);

		// Format time
		Str::FormatTime((size_t)reply->_int1, tr_time_fmt);
		Str::FormatTime((size_t)reply->_s_int2, r_time_fmt);
		Str::FormatTime((size_t)reply->_t_int2, w_time_fmt);

		if(reply->_cmd_subtype == SQLDATA_CMD_IN_PROGRESS)
			_log.Log("In progress (session %d)", reply->session_id);
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_COMPLETE)
		{
			if(reply->rc == 0)
				_log.Log("Data transfer complete (session %d)", reply->session_id);
			else
			if(reply->rc == -1)
			{
				std::string error;

				if(reply->t_error == SQL_DBAPI_NOT_EXISTS_ERROR)
					error = "target table does not exist";
				else
				if(reply->s_error == -1)
				{
					error = reply->s_native_error_text;
					if(reply->t_error == -1)
					{
						error += "\n\t";
						error += reply->t_native_error_text;
					}
				}
				else
				if(reply->t_error == -1)
					error = reply->t_native_error_text;

				_log.Log("Data transfer failed\n\t%s", error.c_str());
				AddFailedTable(reply, error.c_str());
			}
		}

		_log.Log("\n     Rows read:     %d (%.0lf rows/sec)", reply->_s_int1, r_speed_rows);
		_log.Log("\n     Rows written:  %d (%.0lf rows/sec, %s, %s/sec)", 
			reply->_t_int1, w_speed_rows, w_bytes_fmt, w_speed_bytes_fmt);
		_log.Log("\n     Transfer time: %s (%s read, %s write)", tr_time_fmt, r_time_fmt, w_time_fmt);

		// Warning can be returned, for example in case data truncation
		if(reply->rc == 1)
		{
			_log.Log("\n    Warning: %s", reply->s_native_error_text);
			AddWarningsTable(reply, reply->s_native_error_text);
		}

		// Save information about the tables with different number of read and written rows
		if(reply->_cmd_subtype == SQLDATA_CMD_COMPLETE && reply->rc == 0 && reply->_s_int1 != reply->_t_int1)
		{
			char row_fmt[13];

			std::string table_diff = reply->_s_name;
			table_diff += " (";
			table_diff += Str::IntToString(reply->_s_int1, row_fmt);
			table_diff += " rows read, ";
			table_diff += Str::IntToString(reply->_t_int1, row_fmt);
			table_diff += " rows written, ";
			table_diff += Str::IntToString(abs(reply->_s_int1 - reply->_t_int1), row_fmt);
			table_diff += " rows difference)";

			_tables_read_write_diff_list.push_back(table_diff);
		}
	}
	else
	// Error during DROP or CREATE TABLE etc.
	if(reply->_cmd_subtype == SQLDATA_CMD_COMPLETE_WITH_DDL_ERROR)
	{
		AddFailedTable(reply, reply->t_native_error_text);
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_ADD_DEFAULT || reply->_cmd_subtype == SQLDATA_CMD_ADD_CHECK_CONSTRAINT ||
		reply->_cmd_subtype == SQLDATA_CMD_ADD_PRIMARY_KEY || reply->_cmd_subtype == SQLDATA_CMD_ADD_UNIQUE_KEY ||
		reply->_cmd_subtype == SQLDATA_CMD_ADD_FOREIGN_KEY || reply->_cmd_subtype == SQLDATA_CMD_ADD_COMMENT ||
		reply->_cmd_subtype == SQLDATA_CMD_CREATE_INDEX || reply->_cmd_subtype == SQLDATA_CMD_CREATE_SEQUENCE ||
		reply->_cmd_subtype == SQLDATA_CMD_CREATE_TRIGGER)
	{
		Str::FormatTime(reply->_t_time_spent, time_fmt);

		if(reply->_cmd_subtype == SQLDATA_CMD_ADD_DEFAULT)
			_log.Log("Add DEFAULT clause (");
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_ADD_CHECK_CONSTRAINT)
			_log.Log("Add CHECK constraint (");
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_ADD_PRIMARY_KEY)
			_log.Log("Add PRIMARY KEY constraint (");
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_ADD_UNIQUE_KEY)
			_log.Log("Add UNIQUE constraint (");
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_ADD_FOREIGN_KEY)
			_log.Log("Add FOREIGN KEY constraint (");
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_ADD_COMMENT)
			_log.Log("Add comment (");
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_CREATE_INDEX)
			_log.Log("Create index %s (", reply->t_o_name);
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_CREATE_SEQUENCE)
			_log.Log("Create sequence (");
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_CREATE_TRIGGER)
			_log.Log("Create trigger (");

		if(reply->rc != -1)
			_log.Log("%s, session %d)", time_fmt, reply->session_id);
		else
		{
			_log.Log("Failed, %s, session %d)\n\t%s", time_fmt, reply->session_id, reply->t_native_error_text);
			_failed_target_ddl++;
		}

		_total_target_ddl++;
		LogSql(reply, time_fmt);
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_ALL_COMPLETE)
	{
		int read_write_diff_count = (int)_tables_read_write_diff_list.size();

		// Output information about tables with warnings
		if(_warnings_tables_list.empty() == false)
		{
			_log.Log("\n\nWarnings:\n\n");

			int k = 1;
			for(std::list<std::string>::iterator i = _warnings_tables_list.begin(); 
					i != _warnings_tables_list.end(); i++, k++)
				_log.Log("%5d. %s\n", k, i->c_str());
		}

		// Output information about failed tables
		if(_failed_tables_list.empty() == false)
		{
			_log.Log("\n\nFailed tables (%d table%s):\n\n", _failed_tables, SUFFIX(_failed_tables));

			int k = 1;
			for(std::list<std::string>::iterator i = _failed_tables_list.begin(); 
					i != _failed_tables_list.end(); i++, k++)
				_log.Log("%5d. %s\n", k, i->c_str());

			for(std::list<std::string>::iterator i = _failed_tables_names_list.begin(); 
					i != _failed_tables_names_list.end(); i++)
				_failed_tables_log.Log("%s\n", i->c_str());
		}

		// Output information about tables with different number of read and written rows
		if(_tables_read_write_diff_list.empty() == false)
		{
			_log.Log("\n\nRead/write row count differences (%d table%s):\n\n", read_write_diff_count, 
				SUFFIX(read_write_diff_count));

			int k = 1;
			for(std::list<std::string>::iterator i = _tables_read_write_diff_list.begin(); 
					i != _tables_read_write_diff_list.end(); i++, k++)
				_log.Log("%4d. %s\n", k, i->c_str());
		}

		if(_failed_tables_list.empty() == false || _tables_read_write_diff_list.empty() == false)
			_log.Log("\nPlease contact us at support@sqlines.com for any assistance.\n");

		double speed_rows = 0, speed_bytes = 0;

		if(reply->_int1 != 0)
		{
			speed_rows = ((double)reply->_t_int1)/((double)reply->_int1)*1000.0;
			speed_bytes = ((double)reply->_t_bigint1)/((double)reply->_int1)*1000.0;
		}

		char bytes_fmt[21], speed_bytes_fmt[21];

		// Format numbers
		Str::FormatByteSize((double)reply->_t_bigint1, bytes_fmt);
		Str::FormatByteSize(speed_bytes, speed_bytes_fmt);

		Str::FormatTime((size_t)reply->_int1, time_fmt);

		if(_failed_tables_list.empty() == true && _tables_read_write_diff_list.empty() == true)
			_log.Log("\n");

		_log.Log("\nSummary:\n");
		_log.Log("\n  Tables:        %d (%d Ok, %d failed)", _total_tables, _total_tables - _failed_tables, _failed_tables);
		_log.Log("\n  Target DDL:    %d (%d Ok, %d failed)", _total_target_ddl, _total_target_ddl - _failed_target_ddl, _failed_target_ddl);
		_log.Log("\n  Rows read:     %d", reply->_s_int1);
		_log.Log("\n  Rows written:  %d",reply->_t_int1);

		if(reply->_s_int1 != reply->_t_int1)
			_log.Log(" (%d row difference)", abs(reply->_s_int1 - reply->_t_int1));

		_log.Log("\n  Transfer time: %s (%.0lf rows/sec, %s, %s/sec)", time_fmt, speed_rows, bytes_fmt, speed_bytes_fmt);
	
		_log.Log("\n\nLogs:\n");
		_log.Log("\n  Execution log:             %s", _logname.c_str());
		_log.Log("\n  DDL SQL statements:        %s", SQLDATA_DDL_LOGFILE);
		
		if(_failed_target_ddl > 0)
			_log.Log("\n  Failed DDL SQL statements: %s", SQLDATA_FAILED_DDL_LOGFILE);
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_DUMP_DDL)
	{
		SqlCol *cols = (SqlCol*)reply->data;

		_log.LogFile("\n\n  %s columns:", reply->_s_name);

		// Dump column metadata
		for(int i = 0; i < reply->_int1; i++)
		{
			_log.LogFile("\n   %2d. %s, data type %d, length %d, precision %d, scale %d", 
				i+1, cols[i]._name, cols[i]._native_dt, cols[i]._len, cols[i]._precision, cols[i]._scale);
		}
	}
}

// Callback function for assessment task
void SqlDataCmd::CallbackAssess(SqlDataReply *reply)
{
	if(reply == NULL)
		return;

	char time_fmt[21];

	// Object name
	if(reply->_cmd_subtype != SQLDATA_CMD_ALL_COMPLETE)
		_log.Log("\n  %s - ", reply->_s_name);

	if(reply->_cmd_subtype == SQLDATA_CMD_STARTED)
	{
		_log.Log("Started (%d of %d, session %d)", _assess_table_num, _total_tables, reply->session_id);
		_assess_table_num++;
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_OPEN_CURSOR)
	{
		Str::FormatTime((size_t)reply->_s_int2, time_fmt);

		_log.Log("Open cursor (");

		if(reply->rc != -1)
		{
			_log.Log("%d row%s read, %s, session %d)", reply->_s_int1, SUFFIX(reply->_s_int1), time_fmt, reply->session_id);
		
			// Warning can be returned, for example in case data truncation
			if(reply->rc == 1)
			{
				_log.Log("\n    Warning: %s", reply->s_native_error_text);
				AddWarningsTable(reply, reply->s_native_error_text);
			}
		}
		else
		{
			_log.Log("Failed, %s, session %d)\n    %s", time_fmt, reply->session_id, reply->s_native_error_text);
			AddFailedTable(reply, reply->s_native_error_text);
		}
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_IN_PROGRESS || reply->_cmd_subtype == SQLDATA_CMD_COMPLETE)
	{
		double r_speed_rows = 0, r_speed_bytes = 0;

		// Calculate number of rows ans bytes read per unit of time
		if(reply->_s_int1 !=0 && reply->_s_int2 != 0)
		{
			r_speed_rows = ((double)reply->_s_int1)/((double)reply->_s_int2) * 1000.0;

			if(reply->_s_bigint1 != 0)
				r_speed_bytes = ((double)reply->_s_bigint1)/((double)reply->_s_int2)*1000.0;
		}

		char r_bytes_fmt[21], r_speed_bytes_fmt[21];
		char tr_time_fmt[21], r_time_fmt[21];

		// Format numbers
		Str::FormatByteSize((double)reply->_s_bigint1, r_bytes_fmt);
		Str::FormatByteSize(r_speed_bytes, r_speed_bytes_fmt);

		// Format time
		Str::FormatTime((size_t)reply->_int1, tr_time_fmt);
		Str::FormatTime((size_t)reply->_s_int2, r_time_fmt);

		if(reply->_cmd_subtype == SQLDATA_CMD_IN_PROGRESS)
			_log.Log("In progress (session %d)", reply->session_id);
		else
		if(reply->_cmd_subtype == SQLDATA_CMD_COMPLETE)
		{
			if(reply->rc == 0)
				_log.Log("Assessment complete (session %d)", reply->session_id);
			else
			if(reply->rc == -1)
			{
				_log.Log("Data assessment failed\n\t%s", reply->s_native_error_text);
				AddFailedTable(reply, reply->s_native_error_text);
			}
		}

		_log.Log("\n     Rows read:       %d (%.0lf rows/sec, %s, %s/sec)", 
			reply->_s_int1, r_speed_rows, r_bytes_fmt, r_speed_bytes_fmt);
		_log.Log("\n     Assessment time: %s (%s read)", tr_time_fmt, r_time_fmt);

		// Warning can be returned, for example in case data truncation
		if(reply->rc == 1)
		{
			_log.Log("\n    Warning: %s", reply->s_native_error_text);
			AddWarningsTable(reply, reply->s_native_error_text);
		}
	}
	else
	if(reply->_cmd_subtype == SQLDATA_CMD_ALL_COMPLETE)
	{
		// Output information about tables with warnings
		if(_warnings_tables_list.empty() == false)
		{
			_log.Log("\n\nWarnings:\n\n");

			int k = 1;
			for(std::list<std::string>::iterator i = _warnings_tables_list.begin(); 
					i != _warnings_tables_list.end(); i++, k++)
				_log.Log("%5d. %s\n", k, i->c_str());
		}

		// Output information about failed tables
		if(_failed_tables_list.empty() == false)
		{
			_log.Log("\n\nFailed tables (%d table%s):\n\n", _failed_tables, SUFFIX(_failed_tables));

			int k = 1;
			for(std::list<std::string>::iterator i = _failed_tables_list.begin(); 
					i != _failed_tables_list.end(); i++, k++)
				_log.Log("%5d. %s\n", k, i->c_str());

			_log.Log("\nPlease contact us at support@sqlines.com for any assistance.\n");
		}

		double speed_rows = 0, speed_bytes = 0;

		if(reply->_int1 != 0)
		{
			speed_rows = ((double)reply->_s_int1)/((double)reply->_int1)*1000.0;
			speed_bytes = ((double)reply->_s_bigint1)/((double)reply->_int1)*1000.0;
		}

		char bytes_fmt[21], speed_bytes_fmt[21];

		// Format numbers
		Str::FormatByteSize((double)reply->_s_bigint1, bytes_fmt);
		Str::FormatByteSize(speed_bytes, speed_bytes_fmt);

		Str::FormatTime((size_t)reply->_int1, time_fmt);

		if(_failed_tables_list.empty() == true && _tables_read_write_diff_list.empty() == true)
			_log.Log("\n");

		_log.Log("\nSummary:\n");
		_log.Log("\n  Tables:          %d (%d Ok, %d failed)", _total_tables, _total_tables - _failed_tables, _failed_tables);
		_log.Log("\n  Rows read:       %d", reply->_s_int1);

		_log.Log("\n  Assessment time: %s (%.0lf rows/sec, %s, %s/sec)", time_fmt, speed_rows, bytes_fmt, speed_bytes_fmt);
	
		_log.Log("\n\nLogs:\n");
		_log.Log("\n  Execution log:             %s", _logname.c_str());
	}
}

// Callback function for row count validation task
void SqlDataCmd::CallbackValidationRowCount(SqlDataReply *reply)
{
	if(reply == NULL)
		return;

	// Object name
	if(reply->_cmd_subtype != SQLDATA_CMD_ALL_COMPLETE)
		_log.Log("\n  %s - ", reply->_s_name);

	if(reply->_cmd_subtype == SQLDATA_CMD_STARTED)
	{
		_log.Log("Started (%d of %d, session %d)", _validate_table_num, _total_tables, reply->session_id);
		_validate_table_num++;
	}
	else
	// A single table validated
	if(reply->_cmd_subtype == SQLDATA_CMD_COMPLETE)
	{
		std::string table_diff;

		// Operation result
		if(reply->_s_rc == -1 || reply->_t_rc == -1)
			_log.Log("Failed");
		else
		// There is a difference in row count
		if(reply->_s_int1 != reply->_t_int1)
		{
			char row_fmt[13];

			_log.Log("Not Equal");
			_tables_diff++;

			table_diff = reply->_s_name;
			table_diff += " (";
			table_diff += Str::IntToString(reply->_s_int1, row_fmt);
			table_diff += " rows in source, ";
			table_diff += Str::IntToString(reply->_t_int1, row_fmt);
			table_diff += " rows in target)";

			_tables_diff_list.push_back(table_diff);
		}
		// The same row count
		else
		{
			_log.Log("Ok");
			_tables_equal++;
		}

		char time_fmt[21], s_time_fmt[21], t_time_fmt[21];

		// Format time
		Str::FormatTime(reply->_time_spent, time_fmt);
		Str::FormatTime(reply->_s_time_spent, s_time_fmt);
		Str::FormatTime(reply->_t_time_spent, t_time_fmt);

		_log.Log(" (%s, session %d)\n", time_fmt, reply->session_id);

		_log.Log("    Source: ");

		if(reply->_s_rc == -1)
		{
			if(reply->s_error == SQL_DBAPI_NOT_EXISTS_ERROR)
				_log.Log("Table does not exist");
			else
				_log.Log("Error");
		}
		else
			_log.Log("%d row%s", reply->_s_int1, SUFFIX(reply->_s_int1));

		_log.Log(" (%s)\n", s_time_fmt);

		_log.Log("    Target: ");

		if(reply->_t_rc == -1)
		{
			if(reply->t_error == SQL_DBAPI_NOT_EXISTS_ERROR)
				_log.Log("Table does not exist");
			else
				_log.Log("Error");
		}
		else
			_log.Log("%d row%s", reply->_t_int1, SUFFIX(reply->_t_int1));

		_log.Log(" (%s)", t_time_fmt);
	}
	else
	// All tables validated
	if(reply->_cmd_subtype == SQLDATA_CMD_ALL_COMPLETE)
	{
		if(_tables_diff == 0)
			_log.Log("\n\nThere are no row count differences.\n");
		else
		{
			_log.Log("\n\nRow count differences (%d table%s):\n\n", _tables_diff, SUFFIX(_tables_diff));

			// Output table information including row count
			int k = 1;
			for(std::list<std::string>::iterator i = _tables_diff_list.begin(); i != _tables_diff_list.end(); i++, k++)
				_log.Log("%5d. %s\n", k, i->c_str());
		}

		char time_fmt[21];

		// Format validation time
		Str::FormatTime(reply->_time_spent, time_fmt);
	
		_log.Log("\nSummary:\n\n");

		_log.Log("  Total number of tables:         %d\n", _total_tables);
		_log.Log("    With the same row count:      %d\n", _tables_equal);
		_log.Log("    With the different row count: %d\n", _tables_diff);
		_log.Log("  Total number of rows:           %d in source, %d in target\n", reply->_s_int1, reply->_t_int1);
		_log.Log("    Row count difference:         %d\n", abs(reply->_s_int1 - reply->_t_int1));
		_log.Log("  Validation time:                %s\n", time_fmt);
	}
}

// Callback function for table data validation task
void SqlDataCmd::CallbackValidationRows(SqlDataReply *reply)
{
	if(reply == NULL)
		return;

	// Object name
	if(reply->_cmd_subtype != SQLDATA_CMD_ALL_COMPLETE)
		_log.Log("\n  %s - ", reply->_s_name);

	if(reply->_cmd_subtype == SQLDATA_CMD_STARTED)
	{
		_log.Log("Started (%d of %d, session %d)", _validate_table_num, _total_tables, reply->session_id);
		_validate_table_num++;
	}
	else
	// A single table validated
	if(reply->_cmd_subtype == SQLDATA_CMD_COMPLETE)
	{
		std::string table_diff;

		// Operation result
		if(reply->_s_rc == -1 || reply->_t_rc == -1)
		{
			_log.Log("Failed");
			_failed_tables++;
		}
		else
		// There is a difference in row count or different data in rows
		if(reply->_s_int1 != reply->_t_int1 || reply->_int1 != 0)
		{
			_log.Log("Not Equal");
			_tables_diff++;

			table_diff = reply->_s_name;
			table_diff += " (";
			table_diff += reply->data2;
			table_diff += ")";

			_tables_diff_list.push_back(table_diff);
		}
		// The same row count
		else
		{
			_log.Log("Ok");
			_tables_equal++;
		}

		char time_fmt[21], s_time_fmt[21], t_time_fmt[21];

		// Format time
		Str::FormatTime(reply->_time_spent, time_fmt);
		Str::FormatTime(reply->_s_time_spent, s_time_fmt);
		Str::FormatTime(reply->_t_time_spent, t_time_fmt);

		_log.Log(" (%s, session %d)\n", time_fmt, reply->session_id);

		double s_speed_rows = 0, t_speed_rows = 0;

		// Calculate number of rows read per unit of time
		if(reply->_s_int1 != 0 && reply->_s_time_spent != 0)
			s_speed_rows = ((double)reply->_s_int1)/((double)reply->_s_time_spent) * 1000.0;

		if(reply->_t_int1 != 0 && reply->_t_time_spent != 0)
			t_speed_rows = ((double)reply->_t_int1)/((double)reply->_t_time_spent) * 1000.0;

		double s_speed_bytes = 0, t_speed_bytes = 0;

		if(reply->_s_bigint1 != 0 && reply->_s_time_spent != 0)
			s_speed_bytes = ((double)reply->_s_bigint1)/((double)reply->_s_time_spent)*1000.0;

		if(reply->_t_bigint1 != 0 && reply->_t_time_spent != 0)
			t_speed_bytes = ((double)reply->_t_bigint1)/((double)reply->_t_time_spent)*1000.0;

		char s_bytes_fmt[21], t_bytes_fmt[21];
		char s_speed_bytes_fmt[21], t_speed_bytes_fmt[21];

		// Format numbers
		Str::FormatByteSize((double)reply->_s_bigint1, s_bytes_fmt);
		Str::FormatByteSize((double)reply->_t_bigint1, t_bytes_fmt);
		Str::FormatByteSize(s_speed_bytes, s_speed_bytes_fmt);
		Str::FormatByteSize(t_speed_bytes, t_speed_bytes_fmt);

		_log.Log("    Source:    ");

		if(reply->_s_rc == -1)
		{
			if(reply->s_error == SQL_DBAPI_NOT_EXISTS_ERROR)
				_log.Log("Table does not exist");
			else
				_log.Log("Error: %s", reply->s_native_error_text);
		}
		else
			_log.Log("%d row%s", reply->_s_int1, SUFFIX(reply->_s_int1));

		_log.Log(" (%s read", s_time_fmt);

		if(reply->_s_int1 > 0)
			_log.Log(", %.0lf rows/sec, %s, %s/sec)\n", s_speed_rows, s_bytes_fmt, s_speed_bytes_fmt);
		else
			_log.Log(")\n");

		_log.Log("    Target:    ");

		if(reply->_t_rc == -1)
		{
			if(reply->t_error == SQL_DBAPI_NOT_EXISTS_ERROR)
				_log.Log("Table does not exist");
			else
				_log.Log("Error: %s", reply->t_native_error_text);
		}
		else
			_log.Log("%d row%s", reply->_t_int1, SUFFIX(reply->_t_int1));

		_log.Log(" (%s read", t_time_fmt);

		if(reply->_t_int1 > 0)
			_log.Log(", %.0lf rows/sec, %s, %s/sec)", t_speed_rows, t_bytes_fmt, t_speed_bytes_fmt);
		else
			_log.Log(")");

		// Show differences
		if(reply->_int1 != 0)
			_log.Log("\n    Not Equal: %d row%s, %d of %d column%s (%s)", reply->_int1, SUFFIX(reply->_int1),
						reply->_int2, reply->_int3, SUFFIX(reply->_int3), reply->data2);  

		_log.Log("\n      Source query: %s", reply->s_sql_l.c_str()); 
		_log.Log("\n      Target query: %s", reply->t_sql_l.c_str()); 
	}
	else
	// All tables validated
	if(reply->_cmd_subtype == SQLDATA_CMD_ALL_COMPLETE)
	{
		char time_fmt[21];

		// Format validation time
		Str::FormatTime(Os::GetTickCount() - _command_start, time_fmt);

		if(_tables_diff > 0)
		{
			_log.Log("\n\nFound differences (%d table%s):\n", _tables_diff, SUFFIX(_tables_diff));

			for(std::list<std::string>::iterator i = _tables_diff_list.begin(); i != _tables_diff_list.end(); i++)
				_log.Log("\n  %s", i->c_str());
		}

		_log.Log("\n\nSummary:\n\n");

		_log.Log("  Tables:           %d (%d compared, %d failed)\n", _total_tables, _total_tables - _failed_tables, _failed_tables);
		_log.Log("  Equal tables:     %d\n", _tables_equal);
		_log.Log("  Different tables: %d\n", _tables_diff);
		_log.Log("  Validation time:  %s\n", time_fmt);
	}
}

void SqlDataCmd::CallbackS(void *object, SqlDataReply *reply)
{
	if(object == NULL)
		return;

	SqlDataCmd *cmd = (SqlDataCmd*)object;

	cmd->Callback(reply);
}

// Connect to the database to read metadata
int SqlDataCmd::Connect()
{
	SqlDataReply reply;

	int rc = -1;

	if(_command == SQLDATA_CMD_TRANSFER || _command == SQLDATA_CMD_TRANSFER_OBJ || _command == SQLDATA_CMD_VALIDATE)
	{
		_log.Log("\n\nConnecting to databases");

		// Connect to databases to read metadata
		rc = _sqlData.ConnectMetaDb(reply, SQLDB_BOTH);
	}
	// In assessment mode connect to the source database only
	else
	if(_command == SQLDATA_CMD_ASSESS)
	{
		_log.Log("\n\nConnecting to database");

		// Connect to databases to read metadata
		rc = _sqlData.ConnectMetaDb(reply, SQLDB_SOURCE_ONLY);
	}

	char time_fmt[21];
	Str::FormatTime(reply._time_spent, time_fmt);

	_log.Log(" (%s)\n  ", time_fmt);
	LogConnection(_source_type, reply._s_rc, reply._s_name, reply.s_native_error_text, reply._s_time_spent);

	if(_command != SQLDATA_CMD_ASSESS)
	{
		_log.Log("\n  ");
		LogConnection(_target_type, reply._t_rc, reply._t_name, reply.t_native_error_text, reply._t_time_spent);
	}

	return rc;
}

// Test a connection to the database
int SqlDataCmd::TestConnection(std::string &conn, std::string &error, std::string &loaded_path, 
									std::list<std::string> &search_paths, size_t *time_spent)
{
	_sqlData.SetParameters(&_parameters);

	int rc = _sqlData.TestConnection(conn, error, loaded_path, search_paths, time_spent);

	return rc;
}

// Log connection information
void SqlDataCmd::LogConnection(int type, int rc, const char *version, const char *error, size_t time_spent)
{
	// Oracle version includes product name
	if(type == SQLDATA_ORACLE && (version == NULL || *version == '\x0'))
		_log.Log("Oracle ");
	else
	// SQL Server version includes full product name
	if(type == SQLDATA_SQL_SERVER && (version == NULL || *version == '\x0'))
		_log.Log("SQL Server ");
	else
	if(type == SQLDATA_MYSQL)
		_log.Log("MySQL ");
	else
	// PostgreSQL version includes product name
	if(type == SQLDATA_POSTGRESQL && (version == NULL || *version == '\x0'))
		_log.Log("PostgreSQL ");
	else
	if(type == SQLDATA_SYBASE)
		_log.Log("Sybase ASE ");
	else
	// Informix version includes database name
	if(type == SQLDATA_INFORMIX && (version == NULL || *version == '\x0'))
		_log.Log("Informix ");
	else
	// DB2 version includes full information
	if(type == SQLDATA_DB2 && (version == NULL || *version == '\x0'))
		_log.Log("IBM DB2 ");
	else
	if(type == SQLDATA_ASA)
		_log.Log("Sybase SQL Anywhere ");
	else
	if(type == SQLDATA_STDOUT)
		_log.Log("Standard output ");
    else
    if(type == -1)
        _log.Log("Unknown database ");

	if(version != NULL && *version != '\x0')
		_log.Log("%s ", version);

	if(error != NULL && *error != '\x0')
		_log.Log("- Error: %s ", error);

	char time_fmt[21];
	Str::FormatTime(time_spent, time_fmt);

	const char *status = (rc >= 0) ? "Ok" : "Failed";

	_log.Log("(%s, %s)", status, time_fmt);
}

// Read the database catalog to define objects and metadata
int SqlDataCmd::ReadMetadata()
{
	size_t start = GetTickCount();

	// Set callback function
	_sqlData.SetCallback((void*)this, SqlDataCmd::CallbackS);

	std::list<std::string> avail_tables;
	int rc = -1; 

	if(!_t.empty() || !_tf.empty())
	{
		_log.Log("\n\nReading the database schema");

		// Get list of tables available in the source database
		if(!_t.empty())
		{
			if(_command != SQLDATA_CMD_TRANSFER_OBJ)
				rc = _sqlData.GetAvailableTables(_t, _texcl, avail_tables); 
			else
				// For non-table objects source code will be read later
				rc = 0;
		}
		else
		// Get list from file
		if(!_tf.empty())
			rc = GetObjectsFromFile(avail_tables);
	}
	else
	// Get queries from a file
	if(!_qf.empty())
	{
		_log.Log("\n\nDefining queries to transfer data");
		rc = GetQueriesFromFile(_qf, _queries);
	}

	if(rc == -1)
	{
		SqlDataReply reply;
		reply._cmd = _command;
		reply._cmd_subtype = SQLDATA_CMD_ALL_COMPLETE;
		_sqlData.GetErrors(reply);

		if(reply._s_rc == -1 && *reply.s_native_error_text)
			_log.Log("\n\t%s", reply.s_native_error_text);

		if(reply._t_rc == -1 && *reply.t_native_error_text)
			_log.Log("\n\t%s", reply.t_native_error_text);
		
		// Notify UI on error completion
		if(_ui_callback != NULL)
			_ui_callback(_ui_callback_object, &reply);

		return -1;
	}

	_total_tables = (int)avail_tables.size();

	// Queries from file
	if(!_qf.empty())
	{
		_total_tables = (int)_queries.size();
		_sqlData.SetQueries(&_queries);
		_log.Log(" (%d %s)", _total_tables, SUFFIX2(_total_tables, "query", "queries"));
	}
	else
	// Tables
	if(_command != SQLDATA_CMD_TRANSFER_OBJ)
	{
		// Add all tables to the list
		for(std::list<std::string>::iterator i = avail_tables.begin(); i != avail_tables.end(); i++)
			_tables.push_back(*i);

		_sqlData.SetTables(&_tables);

		// Create queues to transfer schema metadata
		std::string meta_filter = _t;

		if(_t.empty())
		{
			int c = 0;
			for(std::list<std::string>::iterator i = avail_tables.begin(); i != avail_tables.end(); i++)
			{
				if(c > 0)
					meta_filter += ",";

				meta_filter += (*i);
				c++;
			}
		}
			
		_sqlData.CreateMetadataQueues(meta_filter, _texcl);

		char time_fmt[21];
		Str::FormatTime(GetTickCount() - start, time_fmt);

		_log.Log(" (%d table%s, %s)", _total_tables, SUFFIX(_total_tables), time_fmt);

		// Notify UI about the number of tables
		if(_ui_callback != NULL)
		{
			SqlDataReply reply;
			reply._cmd = _command;
			reply._cmd_subtype = SQLDATA_CMD_NUMBER_OF_TABLES;
			reply._int1 = _total_tables;

			_ui_callback(_ui_callback_object, &reply);
		}
	}
	else
	// Non-table objects
	if(_command == SQLDATA_CMD_TRANSFER_OBJ)
	{
		_sqlData.CreateObjectQueue(_t, _texcl, &_total_objects, &_total_object_lines, &_total_object_bytes);

		char time_fmt[21];
		Str::FormatTime(GetTickCount() - start, time_fmt);

		char bytes_fmt[21];
		Str::FormatByteSize((double)_total_object_bytes, bytes_fmt);

		_log.Log(" (%d object%s, %d line%s, %s, %s)", _total_objects, SUFFIX(_total_objects), _total_object_lines, SUFFIX(_total_object_lines), 
			bytes_fmt, time_fmt);
	}
	
	return 0;
}

// Remove foreign keys for selected tables 
int SqlDataCmd::DropReferences()
{
	if(_target_type != SQLDATA_ORACLE)
		return 0;

	int rc = 0;

	if(_command == SQLDATA_CMD_TRANSFER && 
		(_command_options != SQLDATA_OPT_DROP || _command_options != SQLDATA_OPT_TRUNCATE))
	{
		int keys = 0;
		size_t time_spent = 0;

		_log.Log("\nDropping foreign keys on selected tables in target");

		rc = _sqlData.DropReferences(&keys, &time_spent);

		char time_fmt[21];
		Str::FormatTime(time_spent, time_fmt);

		_log.Log(" (%d key%s, %s)", keys, SUFFIX(keys), time_fmt);
	}

	return rc;
}

// Define command line options
int SqlDataCmd::DefineOptions(int argc, char **argv)
{
	if(argv != NULL)
		_exe = argv[0];

	_parameters.SetConfigFile(SQLDATA_CONFIGFILE);

	int rc = _parameters.Load(argc, argv);

	return rc;
}

int SqlDataCmd::DefineOptions(const char *options)
{
	_parameters.SetConfigFile(SQLDATA_CONFIGFILE);

	int rc = _parameters.LoadStr(options);
		
	return rc;
}

// Get list of objects to process from a file
int SqlDataCmd::GetObjectsFromFile(std::list<std::string> &tables)
{
	if(_tf.empty())
		return -1;

	// Configuration file size
	int size = File::GetFileSize(_tf.c_str());

	if(size == -1)
		return -1;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(_tf.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return -1;
	}

	input[size] = '\x0';

	char *cur = input;

	// Process input
	while(*cur)
	{
		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		std::string name;

		// Get the table name until new line
		while(*cur && *cur != '\r' && *cur != '\n')
		{
			name += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(name);

		// Add the table
		tables.push_back(name);
	}

	delete input;

	return 0;
}

// Get list of queries to process from a file
int SqlDataCmd::GetQueriesFromFile(std::string &file, std::map<std::string, std::string> &queries)
{
	// Configuration file with WHERE conditions
	int size = File::GetFileSize(file.c_str());

	if(size == -1)
		return -1; 
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file.c_str(), input, (unsigned int)size) == -1)
	{
		delete input;
		return -1;
	}

	input[size] = '\x0';
	char *cur = input;

	// Parse mapping file
	while(*cur)
	{
		std::string table;
		std::string query;

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
			query += *cur;
			cur++;
		}

		if(*cur == ';')
			cur++;

		Str::TrimTrailingSpaces(query);
		queries.insert(std::pair<std::string, std::string>(table, query));
	}

	return 0;
}

// Read and validate parameters
int SqlDataCmd::SetParameters()
{
	// Get -out option
	char *value = _parameters.Get(OUT_OPTION);

	if(value != NULL)
	{
		_out = value;
		File::CreateDirectories(value);
	}
	else
		_out.clear();

	// Get -log option
	value = _parameters.Get(LOG_OPTION);

	if(value != NULL)
		_logname = value;
	else
		_logname = SQLDATA_LOGFILE;

	value = _parameters.Get(TD_OPTION);

	// Check for data outout to stdout
	if(value != NULL && _strnicmp(value, "stdout", 6) == 0)
	{
		_td_stdout = true;
		_log.SetUseStdErr(true);
	}

	// Set the directory and name for log and trace files
	_log.SetLogfile(_logname.c_str(), _out.c_str());
	_log.SetTracefile(SQLDATA_TRACEFILE, _out.c_str());

	_log.Log("\n%s - %s.\n%s", SQLDATA_VERSION, SQLDATA_DESC, SQLDATA_COPYRIGHT);

	// Show the license message if required
	if(_license.IsLicenseCheckRequired())
	{
		if(_license.IsEmpty())
			_log.Log("\n\nThe product is FOR EVALUATION USE ONLY.");
		else
			_log.Log("\n\nThe product is licensed to %s.", _license.GetName().c_str());
	}

	if(_parameters.Get(TRACE_OPTION) != NULL)
		_log.Trace(SQLDATA_VERSION);

	// SQL statement logs
	_ddl_log.SetLogfile(SQLDATA_DDL_LOGFILE, _out.c_str());
	_failed_ddl_log.SetLogfile(SQLDATA_FAILED_DDL_LOGFILE, _out.c_str());
	_failed_tables_log.SetLogfile(SQLDATA_FAILED_TABLES_LOGFILE, _out.c_str());;

	// Get -sd option
	value = _parameters.Get(SD_OPTION);

	if(value != NULL)
		_sd = value;
	else
		_sd.clear();

	// Get -td option
	value = _parameters.Get(TD_OPTION);

	if(value != NULL)
		_td = value;
	else
		_td.clear();

	// Get -t option
	value = _parameters.Get(T_OPTION);

	if(value != NULL)
		_t = value;
	else
		_t.clear();

	// Get -tf option
	value = _parameters.Get(TF_OPTION);

	if(value != NULL)
		_tf = value;
	else
		_tf.clear();

	// Get -texcl option
	value = _parameters.Get(TEXCL_OPTION);

	if(value != NULL)
		_texcl = value;
	else
		_texcl.clear();

	// Get -qf option
	value = _parameters.Get(QF_OPTION);

	if(value != NULL)
		_qf = value;
	else
		_qf.clear();

	// Get -cmd option
	value = _parameters.Get(CMD_OPTION);

	if(value != NULL)
		_cmd = value;
	else
		_cmd.clear();

	// Get -smap option
	value = _parameters.Get(SMAP_OPTION);

	if(value != NULL)
		_smap = value;
	else
		_smap.clear();

	// Get -tmapf option
	value = _parameters.Get(TMAPF_OPTION);

	if(value != NULL)
		_tmapf = value;
	else
		_tmapf = SQLDATA_TMAP_FILE;

	// Get -cmapf option
	value = _parameters.Get(CMAPF_OPTION);

	if(value != NULL)
		_cmapf = value;
	else
		_cmapf = SQLDATA_CMAP_FILE;

	// Get -cnsmapf option
	value = _parameters.Get(CNSMAPF_OPTION);

	if(value != NULL)
		_cnsmapf = value;
	else
		_cnsmapf = SQLDATA_CNSMAP_FILE;

    // Get -dtmapf option
	value = _parameters.Get(DTMAPF_OPTION);

	if(value != NULL)
		_dtmapf = value;
	else
		_dtmapf = SQLDATA_DTMAP_FILE;

    // Get -tself option
	value = _parameters.Get(TSELF_OPTION);

	if(value != NULL)
		_tself = value;
	else
		_tself = SQLDATA_TSEL_FILE;

    // Get -tselallf option
	value = _parameters.Get(TSELALLF_OPTION);

	if(value != NULL)
		_tselallf = value;
	else
		_tselallf = SQLDATA_TSEL_ALL_FILE;

	// Get -twheref option
	value = _parameters.Get(TWHEREF_OPTION);

	if(value != NULL)
		_twheref = value;
	else
		_twheref = SQLDATA_TWHERE_FILE;

	// Get -topt option
	value = _parameters.Get(TOPT_OPTION);

	if(value != NULL)
		_topt = value;
	else
		_topt.clear();

	// Get -vopt option
	value = _parameters.Get(VOPT_OPTION);

	if(value != NULL)
		_vopt = value;
	else
		_vopt.clear();

	// Get -ss option
	value = _parameters.Get(SS_OPTION);

	if(value != NULL)
		sscanf(value, "%hd", &_concurrent_sessions);
	else
		_concurrent_sessions = 4;
	
	// Get -wrk option
	value = _parameters.Get(WORKERS_OPTION);

	if(value != NULL)
	{
		sscanf(value, "%hd", &_workers);
		_sqlData.SetWorkers(_workers);
	}

	// Get -lwrk option
	value = _parameters.Get(LOCAL_WORKERS_OPTION);

	if(value != NULL)
	{
		sscanf(value, "%hd", &_local_workers);
		_sqlData.SetLocalWorkers(_local_workers);
	}

	// Some mandatory parameters must be set, and if not print how to use and exist
	if(_sd.empty() || _td.empty() || _parameters.Get(HELP_PARAMETER) || (_t.empty() && _tf.empty() && _qf.empty()))
	{
		_log.Log("\n\nCommand line error:\n");

		if(_sd.empty() == true)
			_log.Log("\n   -sd option is not set");

		if(_td.empty() == true)
			_log.Log("\n   -td option is not set");

		if(_t.empty() && _tf.empty() && _qf.empty())
			_log.Log("\n   -t, -tf or -qf option is not set");

		PrintHowToUse();
		return -1;
	}

	// Define what commands to perform
	DefineCommand();

	// Define command options
	DefineCommandOptions();

	// Specify connection strings for source and target databases
	_sqlData.SetConnectionStrings(_sd, _td, &_source_type, &_target_type);

	// Set mappings
	_sqlData.SetSchemaMapping(_smap);
	_sqlData.SetTableMappingFromFile(_tmapf);
	_sqlData.SetColumnMappingFromFile(_cmapf);
	_sqlData.SetConstraintMappingFromFile(_cnsmapf);
    _sqlData.SetDataTypeMappingFromFile(_dtmapf);
	_sqlData.SetTableSelectExpressionsFromFile(_tself);
	_sqlData.SetTableSelectExpressionsAllFromFile(_tselallf);
	_sqlData.SetTableWhereConditionsFromFile(_twheref);

	_sqlData.SetCommand(_command);
	_sqlData.SetCommandOptions(_command_options);
	_sqlData.SetParameters(&_parameters);
	_sqlData.SetAppLog(&_log);

	return 0;
}

// Log executed target DDL statements
void SqlDataCmd::LogSql(SqlDataReply *reply, const char *time_str)
{
	if(reply == NULL || reply->t_sql == NULL || time_str == NULL)
		return;

	static bool first_ddl = true;
	static bool first_failed = true;

	char stmt_term = ';';

	// Terminate Oracle PL/SQL statements with /
	if(_target_type == SQLDATA_ORACLE && reply->_cmd_subtype == SQLDATA_CMD_CREATE_TRIGGER)
		stmt_term = '/';
	
	if(first_ddl == true)
	{
		_ddl_log.LogFile("-- %s - %s.\n-- %s", SQLDATA_VERSION, SQLDATA_DESC, SQLDATA_COPYRIGHT);
		_ddl_log.LogFile("\n\n-- All DDL SQL statements executed for the target database");
		
		PrintCurrentTimestamp(_ddl_log, "-- ");

		first_ddl = false;
	}

	// Statement was executed successfully
	if(reply->rc == 0)
	{
		_ddl_log.LogFile("\n\n%s%c", reply->t_sql, stmt_term);
		_ddl_log.LogFile("\n\n-- Ok (%s)", time_str);
	}
	// SQL statement failed
	else
	{
		bool drop_not_existent = false;

		// Exclude DROP statements with not exist error
		if((reply->_cmd_subtype == SQLDATA_CMD_DROP || reply->_cmd_subtype == SQLDATA_CMD_DROP_SEQUENCE) && 
				reply->t_error == SQL_DBAPI_NOT_EXISTS_ERROR)
			drop_not_existent = true;
	
		if(first_failed == true && drop_not_existent == false)
		{
			_failed_ddl_log.LogFile("-- %s - %s.\n-- %s", SQLDATA_VERSION, SQLDATA_DESC, SQLDATA_COPYRIGHT);
			_failed_ddl_log.LogFile("\n\n-- Failed DDL SQL statements executed for the target database");
		
			PrintCurrentTimestamp(_failed_ddl_log, "-- ");

			first_failed = false;
		}

		_ddl_log.LogFile("\n\n%s%c", reply->t_sql, stmt_term);
		_ddl_log.LogFile("\n\n-- Failed (%s)", time_str);
		_ddl_log.LogFile("\n-- %s", reply->t_native_error_text);

		if(drop_not_existent == false)
		{
			_failed_ddl_log.LogFile("\n\n%s%c", reply->t_sql, stmt_term);
			_failed_ddl_log.LogFile("\n\n-- Failed (%s)", time_str);
			_failed_ddl_log.LogFile("\n-- %s", reply->t_native_error_text);
		}
	}
}

// Save information about failed table
void SqlDataCmd::AddFailedTable(SqlDataReply *reply, const char *error)
{
	if(reply == NULL)
		return;

	_failed_tables++;

	std::string failed_table = reply->_s_name;
				
	if(error != NULL && *error != '\x0')
	{
		failed_table += " - ";
		failed_table += error;
	}

	_failed_tables_list.push_back(failed_table);
	_failed_tables_names_list.push_back(reply->_s_name);
}

// Add information about tables completed with warnings
void SqlDataCmd::AddWarningsTable(SqlDataReply *reply, const char *error)
{
	if(reply == NULL || error == NULL)
		return;

	std::string warn_table = reply->_s_name;
				
	if(*error != '\x0')
	{
		warn_table += " - ";
		warn_table += error;
	}

	_warnings_tables_list.push_back(warn_table);
}

// Define what command to perform
void SqlDataCmd::DefineCommand()
{
	if(_cmd.empty() == true)
	{
		_command = SQLDATA_CMD_TRANSFER;
		return;
	}

	const char *cur = Str::SkipSpaces(_cmd.c_str());

	if(_strnicmp(cur, "transfer_obj", 12) == 0)
		_command = SQLDATA_CMD_TRANSFER_OBJ;
	else
	if(_strnicmp(cur, "transfer", 8) == 0)
		_command = SQLDATA_CMD_TRANSFER;
	else
	if(_strnicmp(cur, "validate", 8) == 0)
		_command = SQLDATA_CMD_VALIDATE;
	else
	if(_strnicmp(cur, "assess", 6) == 0)
		_command = SQLDATA_CMD_ASSESS;
	else
		_command = SQLDATA_CMD_TRANSFER;
}

// Define command options
void SqlDataCmd::DefineCommandOptions()
{
	_command_options = 0;

	// Transfer command is set
	if(_command == SQLDATA_CMD_TRANSFER)
	{
		if(_topt.empty() == false)
		{
			const char *cur = Str::SkipSpaces(_topt.c_str());

			while(*cur)
			{
				if(_strnicmp(cur, "create", 6) == 0)
				{
					_command_options |= SQLDATA_OPT_CREATE;
					cur += 6;
				}
				else
				if(_strnicmp(cur, "recreate", 8) == 0)
				{
					_command_options |= SQLDATA_OPT_DROP;
					_command_options |= SQLDATA_OPT_CREATE;
					cur += 8;
				}
				else
				if(_strnicmp(cur, "truncate", 8) == 0)
				{
					_command_options |= SQLDATA_OPT_TRUNCATE;
					cur += 8;
				}
				else
				if(_strnicmp(cur, "none", 4) == 0)
				{
					// Suppress all other options already set
					_command_options = SQLDATA_OPT_NONE;
					cur += 4;
				}
				else
					cur++;

				cur = Str::SkipSpaces(cur);

				// Skip comma
				if(*cur == ',')
					cur++;

				cur = Str::SkipSpaces(cur);
			}
		}
		else
		{
			_command_options |= SQLDATA_OPT_DROP;
			_command_options |= SQLDATA_OPT_CREATE;
		}
	}
	else
	// Validate command is set
	if(_command == SQLDATA_CMD_VALIDATE)
	{
		const char *cur = Str::SkipSpaces(_vopt.c_str());

		if(_strnicmp(cur, "rowcount", 8) == 0)
		{
			_command_options |= SQLDATA_OPT_ROWCOUNT;
			cur += 8;
		}
		else
		if(_strnicmp(cur, "rows", 4) == 0)
		{
			_command_options |= SQLDATA_OPT_ROWS;
			cur += 4;
		}
		else
			_command_options |= SQLDATA_OPT_ROWCOUNT;
	}
}

// Output how to use the tool if /? or incorrect parameters are specified
void SqlDataCmd::PrintHowToUse()
{
	printf("\n\nHow to use:");
	printf("\n\n    sqldata -option=value [...n]");

	printf("\n\nOptions:\n");
	printf("\n   -sd       - Source database connection string");
	printf("\n   -td       - Target database connection string");
	printf("\n   -t        - List of objects (wildcards *.* are allowed)");
	printf("\n   -tf       - File with a list of objects");
	printf("\n   -texcl    - Objects to exclude (wildcards *.* are allowed)");
	printf("\n   -qf       - Query file");
	printf("\n   -out      - Output directory (the current directory by default)");
	printf("\n   -log      - Log file (sqldata.log by default)");
	printf("\n   -?        - Print how to use");

	printf("\n\nExample:");
	printf("\n\nTransfer table cities from Oracle to SQL Server");
#if defined(WIN32) || defined(_WIN64)
	printf("\n\n   sqldata.exe -sd=oracle,scott/tiger@orcl -td=sql,trusted@srv1,hr -t=cities");
	printf("\n");
	printf("\nRun sqldata_w.exe to launch a GUI version of SQLData.\n");
	printf("\nPress any key to continue...\n");
	_getch();
#else
	printf("\n\n   ./sqldata -sd=oracle,scott/tiger@orcl -td=sql,trusted@srv1,hr -t=cities");
	printf("\n\n");
#endif
}

// Output the current date and time
void SqlDataCmd::PrintCurrentTimestamp()
{
	PrintCurrentTimestamp(_log, "");
}

void SqlDataCmd::PrintCurrentTimestamp(AppLog &log, const char *prefix)
{
	log.LogFile("\n\n%sCurrent timestamp: %s", prefix, Os::CurrentTimestamp());
}
