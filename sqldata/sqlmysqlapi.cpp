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

// SqlMysqlApi MySQL C API

#if defined(WIN32) || defined(_WIN64)
#include <process.h>
#endif

#include <stdio.h>
#include "sqlmysqlapi.h"
#include "str.h"
#include "os.h"

// Required to access ODBC and CT-Lib constants
#include <sql.h>
#include <sqlext.h>
#include <ctpublic.h>

#if defined(WIN32) || defined(WIN64)
HMODULE SqlMysqlApi::_dll = NULL;
#else
void* SqlMysqlApi::_dll = NULL;
#endif

mysql_initFunc SqlMysqlApi::_mysql_init = NULL;
mysql_closeFunc SqlMysqlApi::_mysql_close = NULL;
mysql_errnoFunc SqlMysqlApi::_mysql_errno = NULL;
mysql_errorFunc SqlMysqlApi::_mysql_error = NULL;
mysql_fetch_fieldsFunc SqlMysqlApi::_mysql_fetch_fields = NULL;
mysql_fetch_rowFunc SqlMysqlApi::_mysql_fetch_row = NULL;
mysql_fetch_lengthsFunc SqlMysqlApi::_mysql_fetch_lengths = NULL;
mysql_free_resultFunc SqlMysqlApi::_mysql_free_result = NULL;
mysql_num_fieldsFunc SqlMysqlApi::_mysql_num_fields = NULL;
mysql_optionsFunc SqlMysqlApi::_mysql_options = NULL;
mysql_real_connectFunc SqlMysqlApi::_mysql_real_connect = NULL;
mysql_queryFunc SqlMysqlApi::_mysql_query = NULL;
mysql_set_character_setFunc SqlMysqlApi::_mysql_set_character_set = NULL;
mysql_set_local_infile_handlerFunc SqlMysqlApi::_mysql_set_local_infile_handler = NULL;
mysql_thread_initFunc SqlMysqlApi::_mysql_thread_init = NULL;
mysql_use_resultFunc SqlMysqlApi::_mysql_use_result = NULL;

// Constructor
SqlMysqlApi::SqlMysqlApi()
{
	_cursor_result = NULL;

	_ldi_rc = 0;
	_ldi_terminated = 0;
	_ldi_cols_count = 0;
	_ldi_rows_count = 0;
	_ldi_cols = NULL;

	_ldi_current_row = 0;
	_ldi_current_col = 0;
	_ldi_current_col_len = 0;
	_ldi_write_newline = false;
	_ldi_lob_data = NULL;
	_ldi_lob_size = 0;

	_ldi_bytes = 0;
	_ldi_bytes_all = 0;

	_port = 0;

#if defined(WIN32) || defined(WIN64)
	_wait_event = NULL;
	_completed_event = NULL;
#endif
}

SqlMysqlApi::~SqlMysqlApi()
{
	Deallocate();
}

// Initialize API for process (MySQL C library can be compiled as non thread-safe)
int SqlMysqlApi::InitStatic()
{
	if(_static_init == true)
		return 0;

	TRACE("MySQL/C InitStatic() Entered");

	const char *mysql_default_lib = MYSQL_C_DLL; 
	const char *mysql_load_error = MYSQL_DLL_LOAD_ERROR;
	const char *mysql_64bit_load_error = MYSQL_64BIT_DLL_LOAD_ERROR; 
	const char *mysql_c_func_load_error = MYSQL_C_FUNC_LOAD_ERROR; 

	const char *mysql_lib = NULL; 

	if(_subtype == SQLDATA_SUBTYPE_MARIADB)
	{
		TRACE("MySQL/C InitStatic() for MariaDB");

		mysql_default_lib = MARIADB_C_DLL;
		mysql_load_error = MARIADB_DLL_LOAD_ERROR;
		mysql_64bit_load_error = MARIADB_64BIT_DLL_LOAD_ERROR; 
		mysql_c_func_load_error = MARIADB_C_FUNC_LOAD_ERROR; 
		
		if(_parameters != NULL)
			mysql_lib = _parameters->Get("-mariadb_lib");
	}

	if(mysql_lib != NULL)
	{
		TRACE_P("MySQL/C InitStatic() library: %s", mysql_lib);
		_dll = Os::LoadLibrary(mysql_lib);
	}
	else
	{
		// Try to load the library by default path
		TRACE_P("MySQL/C InitStatic() default library: %s", mysql_default_lib);
		_dll = Os::LoadLibrary(mysql_default_lib);
	}

	if(_dll == NULL)
	{
		Os::GetLastErrorText(mysql_load_error, _native_error_text, 1024);
		TRACE_P("MySQL/C InitStatic() error: %s", _native_error_text);
	}

	// DLL load failed (do not search if library path is set)
	if(_dll == NULL && mysql_lib == NULL)
	{
#if defined(WIN32) || defined(_WIN64)

		// No error set if DLL is 64-bit and current sqldata build is 32-bit
		bool sf = Os::Is64Bit(mysql_default_lib);

		if(sf == true)
			strcpy(_native_error_text, mysql_64bit_load_error);

		// Try to find MySQL installation paths
		// Note that search is performed for MySQL C Connector even for MariaDB
		FindMysqlPaths();

		for(std::list<std::string>::iterator i = _driver_paths.begin(); i != _driver_paths.end() ; i++)
		{
			std::string loc = (*i);
			loc += MYSQL_C_DLL;

			// Try to open DLL
			_dll = LoadLibrary(loc.c_str());

			if(_dll != NULL)
				break;

			// Set error for the current search item 
			Os::GetLastErrorText(MYSQL_DLL_LOAD_ERROR, _native_error_text, 1024);

			// No error set if DLL is 64-bit
			sf = Os::Is64Bit(MYSQL_C_DLL);

			if(sf == true)
				strcpy(_native_error_text, MYSQL_64BIT_DLL_LOAD_ERROR);
		}		
#else
		char *error = Os::LoadLibraryError();
		if(error != NULL)
			strcpy(_native_error_text, error);
#endif
	}

	// Get functions
	if(_dll != NULL)
	{
		// Save the full path of the loaded driver
		Os::GetModuleFileName(_dll, _loaded_driver, 1024);

		_mysql_init = (mysql_initFunc)Os::GetProcAddress(_dll, "mysql_init");
		_mysql_close = (mysql_closeFunc)Os::GetProcAddress(_dll, "mysql_close");
		_mysql_errno = (mysql_errnoFunc)Os::GetProcAddress(_dll, "mysql_errno");
		_mysql_error = (mysql_errorFunc)Os::GetProcAddress(_dll, "mysql_error");
		_mysql_fetch_fields = (mysql_fetch_fieldsFunc)Os::GetProcAddress(_dll, "mysql_fetch_fields");
		_mysql_fetch_row = (mysql_fetch_rowFunc)Os::GetProcAddress(_dll, "mysql_fetch_row");
		_mysql_fetch_lengths = (mysql_fetch_lengthsFunc)Os::GetProcAddress(_dll, "mysql_fetch_lengths");
		_mysql_free_result = (mysql_free_resultFunc)Os::GetProcAddress(_dll, "mysql_free_result");
		_mysql_num_fields = (mysql_num_fieldsFunc)Os::GetProcAddress(_dll, "mysql_num_fields");
		_mysql_options = (mysql_optionsFunc)Os::GetProcAddress(_dll, "mysql_options");
		_mysql_real_connect = (mysql_real_connectFunc)Os::GetProcAddress(_dll, "mysql_real_connect");
		_mysql_query = (mysql_queryFunc)Os::GetProcAddress(_dll, "mysql_query");
		_mysql_set_character_set = (mysql_set_character_setFunc)Os::GetProcAddress(_dll, "mysql_set_character_set");
		_mysql_set_local_infile_handler = (mysql_set_local_infile_handlerFunc)Os::GetProcAddress(_dll, "mysql_set_local_infile_handler");
		_mysql_thread_init = (mysql_thread_initFunc)Os::GetProcAddress(_dll, "mysql_thread_init");
		_mysql_use_result = (mysql_use_resultFunc)Os::GetProcAddress(_dll, "mysql_use_result");

		// mysql_set_character_set is available since 5.0.7, check for NULL when use

		if(_mysql_init == NULL || _mysql_close == NULL || _mysql_errno == NULL || _mysql_error == NULL || 
			_mysql_fetch_fields == NULL || _mysql_fetch_row == NULL || _mysql_fetch_lengths == NULL || 
			_mysql_free_result == NULL || _mysql_num_fields == NULL || _mysql_options == NULL || _mysql_real_connect == NULL || 
			_mysql_query == NULL || _mysql_set_local_infile_handler == NULL || 
			_mysql_thread_init == NULL || _mysql_use_result == NULL)
		{
			strcpy(_native_error_text, MYSQL_C_FUNC_LOAD_ERROR);
			return -1;
		}
	}
	else
		// Error message already set
		return -1;

	// Initialize the MySQL library 
	// mysql_library_init is not exported DLL function it is define for mysql_server_init
	// but by calling mysql_init we force mysql_library_init() call to initialize the library for the process
	// Must be called before threads are launched, otherwise the application crashes on some systems
	if(_mysql_init != NULL)
	{
		MYSQL *mysql = _mysql_init(&_mysql);

		if(mysql == NULL)
			return -1;

		// Initialize global options that take effect only for new connections
		InitGlobalOptions();

		// Did not work for MariaDB 
		//_mysql_options(&_mysql, MYSQL_OPT_LOCAL_INFILE, 0);
	}

	_static_init = true;
	TRACE("MySQL/C InitStatic() Left");

	return 0;
}

// Initialize API for thread
int SqlMysqlApi::Init()
{
	if(_static_init == false)
		return -1;

#if defined(WIN32) || defined(WIN64)
	// Create synchronization objects for LOAD DATA INFILE command
	_wait_event = CreateEvent(NULL, FALSE,  FALSE, NULL);
	_completed_event = CreateEvent(NULL, FALSE,  FALSE, NULL);
#else
	Os::CreateEvent(&_wait_event);
	Os::CreateEvent(&_completed_event);
#endif

	return 0;
}

// Set the connection string in the API object
void SqlMysqlApi::SetConnectionString(const char *conn)
{
	if(conn == NULL)
		return;

	std::string db;

	SplitConnectionString(conn, _user, _pwd, db);

	const char *start = db.c_str();

	// Find : and , that denote the server port and the database name
	const char *semi = strchr(start, ':');
	const char *comma = strchr(start, ',');

	const char *end = (semi != NULL) ? semi :comma;

	// Define server name
	if(end != NULL)
		_server.assign(start, (size_t)(end - start));
	else
		_server = start;

	// Define port
	if(semi != NULL)
	{
		std::string port;

		if(comma != NULL && comma > semi)
			port.assign(semi + 1, (size_t)(comma - semi - 1));
		else
			port = semi + 1;

		sscanf(port.c_str(), "%d", &_port);
	}

	if(comma != NULL)
		_db = Str::SkipSpaces(comma + 1);
}

// Connect to the database
int SqlMysqlApi::Connect(size_t *time_spent)
{
	// Check if already connected
	if(_connected == true)
		return 0;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;

	// Connect is called in a new thread so we must call mysql_init again but now for the thread (calls mysql_thread_init)
	// If not mysqk_init not called in each thread working with MySQL, the application crashes
	MYSQL *mysql = _mysql_init(&_mysql);

	if(mysql == NULL)
		return -1;

	mysql = _mysql_real_connect(&_mysql, _server.c_str(), _user.c_str(), _pwd.c_str(),
		_db.c_str(), (unsigned int)_port, NULL, CLIENT_LOCAL_FILES);

	if(mysql == NULL)
	{
		SetError();

		if(time_spent != NULL)
			*time_spent = Os::GetTickCount() - start;

		return -1;
	}

	// Set callbacks for LOAD DATA INFILE command
	_mysql_set_local_infile_handler(&_mysql, local_infile_initS, local_infile_readS, local_infile_endS,
		local_infile_errorS, this);

	// Set version of the connected database
	SetVersion();

	// Call after version numbers are set
	InitSession();

	_connected = true;

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return 0;
}

// Initialize global options that take effect only for new connections
int SqlMysqlApi::InitGlobalOptions()
{
	if(_parameters == NULL)
		return -1;

	int rc = 0;
	const char *max_allowed_packet = NULL;

	// MariaDB options
	if(_subtype == SQLDATA_SUBTYPE_MARIADB)
		max_allowed_packet = _parameters->Get("-mariadb_max_allowed_packet");

	// Set options in dedicated connection before all other connections
	if(max_allowed_packet != NULL)
	{
		rc = Connect(NULL);

		if(rc != -1)
		{
			rc = ExecuteNonQuery(std::string("SET GLOBAL max_allowed_packet=").append(max_allowed_packet).c_str(), NULL);
			Disconnect();
		}
	}

	return rc; 
}

// Initialize session by setting options
int SqlMysqlApi::InitSession()
{
	if(_parameters == NULL)
		return -1;

	int rc = 0;

	const char *value = _parameters->Get("-mysql_set_character_set");

	// mysql_set_character_set is available since 5.0.7
	if(Str::IsSet(value) == true && _mysql_set_character_set != NULL)
	{
		rc = _mysql_set_character_set(&_mysql, value);
		TRACE_P("MySQL/C InitSession() set character set: %s", value);
	}

	value = _parameters->Get("-mysql_set_foreign_key_checks");

	if(Str::IsSet(value) == true)
	{
		std::string stmt = "SET FOREIGN_KEY_CHECKS=";
		stmt += value;

		rc = ExecuteNonQuery(stmt.c_str(), NULL);
	}

	// Set INNODB_STATS_ON_METADATA=0 to significantly speed up access to INFORMATION_SCHEMA views
	// (about 5-10 times faster from 5-10 sec to 0.5-1 sec). But this requires SUPER privilege
	// Available since 5.1.17
	if(IsVersionEqualOrHigher(5, 1, 17) == true)
		rc = ExecuteNonQuery("SET GLOBAL INNODB_STATS_ON_METADATA=0", NULL);

	// Binary logging for replication. This requires SUPER privilege!
	if(_subtype == SQLDATA_SUBTYPE_MARIADB)
	{
		value = _parameters->Get("-mariadb_set_sql_log_bin");

		if(Str::IsSet(value))
			rc = ExecuteNonQuery(std::string("SET sql_log_bin=").append(value).c_str(), NULL);
	}
	else
		rc = ExecuteNonQuery("SET sql_log_bin=0", NULL);

	// SET UNIQUE_CHECKS option for MariaDB
	if(_subtype == SQLDATA_SUBTYPE_MARIADB)
	{
		value = _parameters->Get("-mariadb_set_unique_checks");

		if(Str::IsSet(value))
			rc = ExecuteNonQuery(std::string("SET unique_checks=").append(value).c_str(), NULL);
	}

	// If AUTOCOMMIT is set to 0 in the database or client side, LOAD DATA INFILE requires a COMMIT statement 
	// to put data to the table, so we force AUTOCOMMIT to 1 for connection
	rc = ExecuteNonQuery("SET autocommit=1", NULL);

	// Reset initialization error text so it will not be later associated with the data transfer
	*_native_error_text = '\x0';

	return rc;
}

// Disconnect from the database
void SqlMysqlApi::Disconnect()
{
	if(_connected == false)
		return;

	_mysql_close(&_mysql);
	_connected = false;
}

// Deallocate the driver
void SqlMysqlApi::Deallocate() 
{
	Disconnect();
}

// Get row count for the specified object
int SqlMysqlApi::GetRowCount(const char *object, int *count, size_t *time_spent)
{
	if(object == NULL)
		return -1;

	std::string query = "SELECT COUNT(*) FROM ";
	query += object;

	// Execute the query
	int rc = ExecuteScalar(query.c_str(), count, time_spent);

	return rc;
}

// Execute the statement and get scalar result
int SqlMysqlApi::ExecuteScalar(const char *query, int *result, size_t *time_spent)
{
	if(query == NULL || result == NULL)
		return -1;

	size_t start = Os::GetTickCount();

	// Execute the query
	int rc = _mysql_query(&_mysql, query);

	// Error raised
	if(rc != 0)
	{
		SetError();
		return -1;
	}

	MYSQL_RES *res = _mysql_use_result(&_mysql);

	if(result == NULL)
		return -1;

	bool exists = false;

	// Fetch the first row
	MYSQL_ROW row = _mysql_fetch_row(res);

	if(row != NULL && row[0] != NULL)
	{
		sscanf(row[0], "%d", result);
		exists = true;
	}

	_mysql_free_result(res);

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return (exists == true) ? 0 : -1;
}

// Execute the statement
int SqlMysqlApi::ExecuteNonQuery(const char *query, size_t *time_spent)
{
	size_t start = Os::GetTickCount();

	// Execute the query
	int rc = _mysql_query(&_mysql, query);

	// Error raised
	if(rc != 0)
		SetError();

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	// mysql_query returns 1 on failure
	return (rc == 0) ? 0 : -1;
}

// Open cursor and allocate buffers
int SqlMysqlApi::OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool /*catalog_query*/, std::list<SqlDataTypeMap> * /*dtmap*/)
{
	if(query == NULL)
		return -1;

	size_t start = Os::GetTickCount();

	// Execute the query
	int rc = _mysql_query(&_mysql, query);

	// Error raised
	if(rc != 0)
	{
		SetError();
		return -1;
	}

	_cursor_result = _mysql_use_result(&_mysql);

	if(_cursor_result == NULL)
		return -1;

	// Define the number of columns
	_cursor_cols_count = _mysql_num_fields(_cursor_result);

	if(_cursor_cols_count > 0)
		_cursor_cols = new SqlCol[_cursor_cols_count];

	size_t row_size = 0;

	MYSQL_FIELD *fields = _mysql_fetch_fields(_cursor_result);

	_cursor_lob_exists = false;

	// Get column information
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		// Copy column name
		strcpy(_cursor_cols[i]._name, fields[i].name);

		// Native data type
		_cursor_cols[i]._native_dt = fields[i].type;

		// Column length in characters (max_length is 0), 11 returned for INT
		// len 0 returned for BINARY(0) (MYSQL_TYPE_STRING = 254)
		// len -1 returned for LONGTEXT (MYSQL_TYPE_BLOB = 252)
		_cursor_cols[i]._len = fields[i].length;

		_cursor_cols[i]._binary = (fields[i].flags & BINARY_FLAG) ? true : false;

		// DECIMAL or NUMERIC
		if(_cursor_cols[i]._native_dt == MYSQL_TYPE_NEWDECIMAL || _cursor_cols[i]._native_dt == MYSQL_TYPE_DECIMAL)
		{
			_cursor_cols[i]._scale = (int)fields[i].decimals;

			// If decimals is 0 length contain precision + 1, if <> 0, length contains precision + scale 
			if(fields[i].decimals == 0)
				_cursor_cols[i]._precision = (int)_cursor_cols[i]._len - 1;
			else
				_cursor_cols[i]._precision = (int)(_cursor_cols[i]._len - _cursor_cols[i]._scale);
		}
		else
		// MYSQL_TYPE_BLOB returned for BLOB, TEXT, LONGTEXT (MySQL 5.5); BINARY_FLAG is set for BLOBs
		if(_cursor_cols[i]._native_dt == MYSQL_TYPE_BLOB)
		{
			_cursor_cols[i]._lob = true;
			_cursor_lob_exists = true;
		}

		// len is -1 for LONGTEXT, do not allocate space in the buffer
		// Note 4294967295 returned on 64-bit system, and it is not equal -1
		if(_cursor_cols[i]._len != -1 && _cursor_cols[i]._len < 1000000000)
			row_size += _cursor_cols[i]._len;

		// NOT NULL attribute
		_cursor_cols[i]._nullable = (fields[i].flags & NOT_NULL_FLAG) ? false : true;
	}

	_cursor_allocated_rows = 1;

	// Define how many rows fetch at once
	if(buffer_rows > 0)
		_cursor_allocated_rows = buffer_rows;
	else
	if(buffer_memory > 0)
	{
		// Avoid 0 rows size when only CHAR(0), BINARY(0) or LONGTEXT columns in table
		if(row_size == 0)
			row_size++;

		size_t rows = buffer_memory/row_size;
		_cursor_allocated_rows = rows > 0 ? rows : 1;
	}	

	if(_cursor_lob_exists == true)
		_cursor_allocated_rows = 1;

	// Allocate buffers to each column
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		_cursor_cols[i]._native_fetch_dt = _cursor_cols[i]._native_dt;
		
		// Do not allocate space for LONGTEXT, LONGBLOB columns, but allocate for BLOB and TEXT (65,535 bytes)
		if(_cursor_cols[i]._len != -1 && _cursor_cols[i]._len < 1000000000)
		{
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}

		_cursor_cols[i].ind = new size_t[_cursor_allocated_rows];
	}

	int fetched = 0;

	// Fetch initial set of data
	rc = Fetch(&fetched, NULL);

	if(col_count != NULL)
		*col_count = _cursor_cols_count; 

	if(allocated_array_rows != NULL)
		*allocated_array_rows = _cursor_allocated_rows;

	if(rows_fetched != NULL)
		*rows_fetched = fetched;
	
	if(cols != NULL)
		*cols = _cursor_cols;

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return 0;
}

// Fetch next portion of data to allocate buffers
int SqlMysqlApi::Fetch(int *rows_fetched, size_t *time_spent) 
{
	MYSQL_ROW row = NULL;
	int fetched = 0;

	size_t start = Os::GetTickCount();

	// Fill the buffer
	for(int i = 0; i < _cursor_allocated_rows; i++)
	{
		// Fetch the next row
		row =  _mysql_fetch_row(_cursor_result);

		if(row == NULL)
			break;

		unsigned long *lengths = (unsigned long*)_mysql_fetch_lengths(_cursor_result);

		// Copy column values and set indicators
		for(int k = 0; k < _cursor_cols_count; k++)
		{
			// Check for NULL value
			if(row[k] == NULL)
			{
				_cursor_cols[k].ind[i] = (size_t)-1;
				continue;
			}
			else
				_cursor_cols[k].ind[i] = (size_t)lengths[k];

			char *data = NULL;
			
			// Column data
			if(_cursor_cols[k]._data != NULL)
				data = _cursor_cols[k]._data + _cursor_cols[k]._fetch_len * i;

			bool copy = true;

			// Check for zero date 0000-00-00
			if(row[k] != NULL && _cursor_cols[k]._native_dt == MYSQL_TYPE_DATE)
			{
				// Change to NULL
				if(strcmp(row[k], "0000-00-00") == 0)
				{
					_cursor_cols[k].ind[i] = (size_t)-1;
					copy = false;
				}
			}
			else
			// Check for zero datetime 0000-00-00 00:00:00
			if(row[k] != NULL && _cursor_cols[k]._native_dt == MYSQL_TYPE_DATETIME)
			{
				// Change to NULL
				if(strcmp(row[k], "0000-00-00 00:00:00") == 0)
				{
					_cursor_cols[k].ind[i] = (size_t)-1;
					copy = false;
				}
			}

			// Copy data (LONGBLOB and LONGTEXT are not copied if they transferred as LOBs to the target)
			if(data != NULL && copy == true)
				memcpy(data, row[k], lengths[k]);
		}

		fetched++;
	}

	if(rows_fetched != NULL)
		*rows_fetched = fetched;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return 0;
}

// Close the cursor and deallocate buffers
int SqlMysqlApi::CloseCursor()
{
	if(_cursor_result != NULL)
		_mysql_free_result(_cursor_result);

	_cursor_result = NULL;

	if(_cursor_cols == NULL)
		return 0;

	// Delete allocated buffers
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		delete [] _cursor_cols[i]._data;
		delete [] _cursor_cols[i].ind;
	}

	delete [] _cursor_cols;

	_cursor_cols = NULL;
	_cursor_cols_count = 0;
	_cursor_allocated_rows = 0;

	return 0;
}

// Initialize the bulk copy from one database into another
int SqlMysqlApi::InitBulkTransfer(const char *table, size_t col_count, size_t /*allocated_array_rows*/, SqlCol * /*s_cols*/, SqlCol ** /*t_cols*/)
{
	TRACE("MySQL/C InitBulkTransfer() Entered");

	// Column metadata and data passed directly to TransferRows
	_ldi_cols = NULL;

	_ldi_cols_count = col_count;

	_load_command = "LOAD DATA LOCAL INFILE 'sqldata.in_memory' IGNORE INTO TABLE "; 
	_load_command += table;

	TRACE(_load_command.c_str());
	TRACE_DMP_INIT(table);

	// Counters must be reset to clear data for the previous table
	_ldi_rc = 0;
	_ldi_terminated = 0;
	_ldi_current_row = 0;
	_ldi_current_col = 0;
	_ldi_current_col_len = 0;
	_ldi_write_newline = false;
	_ldi_lob_data = NULL;
	_ldi_lob_size = 0;

	_ldi_bytes = 0;
	_ldi_bytes_all = 0;

#if defined(WIN32) || defined(WIN64)
	ResetEvent(_wait_event);
	ResetEvent(_completed_event);

	// Run blocking LOAD DATA INFILE command in a separate thread
	_beginthreadex(NULL, 0, StartLoadDataInfileS, this, 0, NULL);

	// Wait until the thread started
	WaitForSingleObject(_completed_event, INFINITE);
#else
	Os::ResetEvent(&_wait_event);
	Os::ResetEvent(&_completed_event);

	pthread_t thread;
	pthread_create(&thread, NULL, StartLoadDataInfileS, this);

	Os::WaitForEvent(&_completed_event);
#endif

	TRACE("MySQL/C InitBulkTransfer() Left");

	// Return code typically 0 as init function waits for LDI thread to start only, but in case cases init can catch LDI errors
	return _ldi_rc;
}

// Transfer rows between databases
int SqlMysqlApi::TransferRows(SqlCol *s_cols, int rows_fetched, int *rows_written, size_t *bytes_written,
							size_t *time_spent)
{
	if(rows_fetched == 0)
		return 0;

	TRACE("MySQL/C TransferRows() Entered");
	size_t start = Os::GetTickCount();

	_ldi_rows_count = rows_fetched;
	_ldi_cols = s_cols;

	_ldi_current_row = 0;
	_ldi_current_col = 0;
	_ldi_current_col_len = 0;
	_ldi_write_newline = false;

	_ldi_bytes = 0;

	// Notify that the new portion of data is available
#if defined(WIN32) || defined(_WIN64)
	SetEvent(_wait_event);

	// Wait until it processed
	WaitForSingleObject(_completed_event, INFINITE);
#else
	Os::SetEvent(&_wait_event);
	Os::WaitForEvent(&_completed_event);
#endif

	if(time_spent)
		*time_spent = GetTickCount() - start;

	if(rows_written != NULL)
		*rows_written = rows_fetched;

	if(bytes_written != NULL)
		*bytes_written = (size_t)_ldi_bytes;

	if(time_spent)
		*time_spent = GetTickCount() - start;

	TRACE("MySQL/C TransferRows() Left");

	// Return code should remain 0 until bloking LDI call terminates
	return _ldi_rc;
}

// Write LOB data 
int SqlMysqlApi::WriteLob(SqlCol * /*s_cols*/, int /*row*/, int * /*lob_bytes*/)
{
	return -1;
}

// LOAD DATA INFILE callbacks
int SqlMysqlApi::local_infile_read(char *buf, unsigned int buf_len)
{
	TRACE_P("MySQL/C LOAD DATA INFILE Read callback() Entered, buffer size is %d bytes", buf_len);

	// Wait until a new portion of data is available (_ldi_cols is NULL when first callback is called before TransferRows called)
	if(_ldi_cols == NULL || (_ldi_current_row == 0 && _ldi_current_col == 0 && _ldi_current_col_len == 0))
	{
		TRACE("MySQL/C LOAD DATA INFILE Read callback() Waiting for data");
#if defined(WIN32) || defined(_WIN64)
		WaitForSingleObject(_wait_event, INFINITE);
#else
		Os::WaitForEvent(&_wait_event);
#endif
		TRACE("MySQL/C LOAD DATA INFILE Read callback() Data arrived");
	}

	// EOF condition
	if(_ldi_current_row == -1)
	{
		TRACE("MySQL/C LOAD DATA INFILE Read callback() Left with EOF condition");
		// Completion event will be set in the thread executing LOAD DATA INFILE 
		return 0;
	}

	char *cur = buf;
	unsigned int remain_len = buf_len;

	// Check whether newline was not written for the previous row
	if(_ldi_write_newline == true)
	{
		*cur = '\n';
		cur++;
		remain_len--;

		_ldi_bytes++;
		_ldi_bytes_all++;

		_ldi_write_newline = false;
	}
	
	// Copy rows
	for(size_t i = _ldi_current_row; i < _ldi_rows_count; i++, _ldi_current_row++)
	{
		// Copy column data
		for(size_t k = _ldi_current_col; k < _ldi_cols_count; k++, _ldi_current_col++)
		{
			// Add column delimiter if we are not in the middle of column
			if(k > 0 && _ldi_current_col_len == 0)
			{
				if(remain_len >= 1)
				{
					*cur = '\t';
					cur++;
					remain_len--;

					_ldi_bytes++;
					_ldi_bytes_all++;
				}
				else
				{
					TRACE_P("MySQL/C LOAD DATA INFILE Read callback() Left - middle of batch, %d bytes chunk, %d bytes all", (int)(cur - buf), _ldi_bytes_all);
					TRACE_DMP(buf, (unsigned int)(cur - buf));
					return (int)(cur - buf);
				}
			}
			
			int len = -1;

			// Check whether column is null
			if(_source_api_type == SQLDATA_ORACLE && _ldi_cols[k]._ind2 != NULL)
			{
				if(_ldi_cols[k]._ind2[i] != -1)
				{
					// LOB column
					if(_ldi_cols[k]._native_fetch_dt == SQLT_BLOB || _ldi_cols[k]._native_fetch_dt == SQLT_CLOB)
					{
						// Get the size and read LOB value when just switched to new column
						if(_ldi_current_col_len == 0)
						{
							// Get the LOB size in bytes for BLOB, in characters for CLOB
							int lob_rc = _source_api_provider->GetLobLength(i, k, &_ldi_lob_size);

							// Probably empty LOB
							if(lob_rc != -1)
								len = (int)_ldi_lob_size;

							if(lob_rc != -1 && _ldi_lob_size > 0)
							{
								size_t alloc_size = 0;
								int read_size = 0;

								_ldi_lob_data = _source_api_provider->GetLobBuffer(i, k, _ldi_lob_size, &alloc_size);

								// Get LOB content
								lob_rc = _source_api_provider->GetLobContent(i, k, _ldi_lob_data, alloc_size, &read_size);

								if(lob_rc == 0)
								{
									// Now set the size in bytes for both CLOB and BLOB
									_ldi_lob_size = (size_t)read_size;
									len = read_size;
								}
								// Error reading LOB
								else
								{
									_source_api_provider->FreeLobBuffer(_ldi_lob_data);
									_ldi_lob_data = NULL;
									_ldi_lob_size = 0;
								}
							}
						}
						else
							len = (int)_ldi_lob_size;
					}
					// Not a LOB column
					else
						len = _ldi_cols[k]._len_ind2[i];
				}
			}
			else
			// Sybase ASE
			if(_source_api_type == SQLDATA_SYBASE && _ldi_cols[k]._ind2 != NULL)
			{
				if(_ldi_cols[k]._ind2[i] != -1)
					len = _ldi_cols[k]._len_ind4[i];
			}
			else
			// ODBC indicator contains either NULL or length
			if((_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_DB2 ||
				_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_ODBC) 
				&& _ldi_cols[k].ind != NULL)
			{
				len = (int)_ldi_cols[k].ind[i];

#if defined(_WIN64)
				// DB2 11 64-bit CLI driver still writes indicators to 4-byte array
				if(_source_api_type == SQLDATA_DB2 /*&& _ldi_cols[k].ind[0] & 0xFFFFFFFF00000000*/)
					len = ((int*)(_ldi_cols[k].ind))[i];
#endif
				// DB2 supports TIMESTAMP(12), it cause Data truncation warning, so trim it
				if(_source_api_type == SQLDATA_DB2 && _ldi_cols[k]._native_dt == SQL_TYPE_TIMESTAMP && len > 26)
					len = 26;
			}
			
			bool no_space = false;

			// Write NULL value
			if(len == -1)
			{
				if(remain_len >= 2)
				{
					cur[0] = '\\'; cur[1] = 'N';
					cur += 2;
					remain_len -= 2;

					_ldi_bytes += 2;
					_ldi_bytes_all += 2;
				}
				else 
					no_space = true;
			}
			else
			// Oracle CHAR, VARCHAR2, CLOB and BLOB
			if((_source_api_type == SQLDATA_ORACLE && (_ldi_cols[k]._native_fetch_dt == SQLT_STR ||
					_ldi_cols[k]._native_fetch_dt == SQLT_BLOB || _ldi_cols[k]._native_fetch_dt == SQLT_CLOB ||
                    _ldi_cols[k]._native_fetch_dt == SQLT_BIN || _ldi_cols[k]._native_fetch_dt == SQLT_LNG)) ||
				// Sybase CHAR
				(_source_api_type == SQLDATA_SYBASE && _ldi_cols[k]._native_fetch_dt == CS_CHAR_TYPE) ||
				// ODBC CHAR
				((_source_api_type == SQLDATA_ODBC || _source_api_type == SQLDATA_INFORMIX || 
				 _source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_SQL_SERVER) && 
							_ldi_cols[k]._native_fetch_dt == SQL_C_CHAR) ||
				// DB2 BINARY
				((_source_api_type == SQLDATA_ODBC || _source_api_type == SQLDATA_DB2) && 
					_ldi_cols[k]._native_fetch_dt == SQL_C_BINARY))
			{
				// Copy data 
				for(int m = _ldi_current_col_len; m < len; m++)
				{
					char c = 0;

					// Not LOB data
					if(_ldi_lob_data == 0)
						c = (_ldi_cols[k]._data + _ldi_cols[k]._fetch_len * i)[m];
					else
						c = _ldi_lob_data[m];

					// Duplicate escape \ character
					if(c == '\\')
					{
						if(remain_len >= 2)
						{
							cur[0] = c;
							cur[1] = c;
						
							cur += 2;
							remain_len -= 2;
							_ldi_bytes += 2;
							_ldi_bytes_all += 2;

							// Only one char of source data read
							_ldi_current_col_len++;
						}
						else
						{
							no_space = true;						
							break;
						}
					}
					// Newline and tab must be escaped
					else
					if(c == '\n' || c == '\t')
					{
						if(remain_len >= 2)
						{
							cur[0] = '\\';
							cur[1] = (c == '\n') ? 'n' : 't';
						
							cur += 2;
							remain_len -= 2;
							_ldi_bytes += 2;
							_ldi_bytes_all += 2;

							// Only one char of source data read
							_ldi_current_col_len++;
						}
						else
						{
							no_space = true;						
							break;
						}
					}
					else
					if(remain_len >= 1)
					{
						*cur = c;
						cur++;
						remain_len--;

						_ldi_bytes++;
						_ldi_bytes_all++;
						_ldi_current_col_len++;
					}
					else
					{
						no_space = true;
						break;
					}
				}

				// All column data were written
				if(no_space == false)
				{
					_ldi_current_col_len = 0;

					if(_ldi_lob_data != NULL)
					{
						_source_api_provider->FreeLobBuffer(_ldi_lob_data);
						_ldi_lob_data = NULL;
						_ldi_lob_size = 0;
					}
				}
			}
			else
			// Oracle DATE fetched as 7 byte binary sequence
			if(_source_api_type == SQLDATA_ORACLE && _ldi_cols[k]._native_fetch_dt == SQLT_DAT)
			{
				if(remain_len >= 19)
				{
					// Unsigned required for proper conversion to int
					unsigned char *data = (unsigned char*)(_ldi_cols[k]._data + _ldi_cols[k]._fetch_len * i);

					int c = ((int)data[0]) - 100;
					int y = ((int)data[1]) - 100;
					int m = (int)data[2];
					int d = (int)data[3];
					int h = ((int)data[4]) - 1;
					int mi = ((int)data[5]) - 1;
					int s = ((int)data[6]) - 1;

					// Get string representation
					Str::Dt2Ch(c, cur);
					Str::Dt2Ch(y, cur + 2);
					cur[4] = '-';
					Str::Dt2Ch(m, cur + 5);
					cur[7] = '-';
					Str::Dt2Ch(d, cur + 8);
					cur[10] = ' ';
					Str::Dt2Ch(h, cur + 11);
					cur[13] = ':';
					Str::Dt2Ch(mi, cur + 14);
					cur[16] = ':';
					Str::Dt2Ch(s, cur + 17);

					cur += 19;
					remain_len -= 19;

					_ldi_bytes += 19;
					_ldi_bytes_all += 19;
				}
				else
					no_space = true;
			}
			else
			// ODBC TIMESTAMP fetched as SQL_TIMESTAMP_STRUCT
			if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || 
				_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA || 
				_source_api_type == SQLDATA_ODBC) && _ldi_cols[k]._native_fetch_dt == SQL_C_TYPE_TIMESTAMP)
			{
				if(remain_len >= 26)
				{
					size_t offset = sizeof(SQL_TIMESTAMP_STRUCT) * i;

					// Informix returns '1200-01-01 11:11:00.0' for 11:11 (HOUR TO MINUTE)
					SQL_TIMESTAMP_STRUCT *ts = (SQL_TIMESTAMP_STRUCT*)(_ldi_cols[k]._data + offset);

					long fraction = (long)ts->fraction;

					// In ODBC, fraction is stored in nanoseconds, but now we support only microseconds
					fraction = fraction/1000;

					// Convert SQL_TIMESTAMP_STRUCT to string
					Str::SqlTs2Str((short)ts->year, (short)ts->month, (short)ts->day, (short)ts->hour, (short)ts->minute, (short)ts->second, fraction, cur);

					cur += 26;
					remain_len -= 26;

					_ldi_bytes += 26;
					_ldi_bytes_all += 26;
				}
				else
					no_space = true;
			}

			// There is no space to write the column value
			if(no_space)
			{
				// If it is non-first column and 0 bytes were written, remove column delimiter
				if(k > 0 && _ldi_current_col_len == 0)
				{
					cur--;
					remain_len++;
					_ldi_bytes--;
					_ldi_bytes_all--;
				}

				TRACE_P("MySQL/C LOAD DATA INFILE Read callback() Left - middle of batch, %d bytes chunk, %d bytes all", (int)(cur - buf), _ldi_bytes_all);
				TRACE_DMP(buf, (unsigned int)(cur - buf));
				return (int)(cur - buf);
			}
		}

		_ldi_current_col = 0;

		// Add row delimiter (no need to write \r for Windows)
		if(remain_len >= 1)
		{
			*cur = '\n';
			cur++;
			remain_len--;

			_ldi_bytes++;
			_ldi_bytes_all++;
		}
		else
		{
			_ldi_write_newline = true;

			_ldi_current_row++;
			_ldi_current_col = 0;
			_ldi_current_col_len = 0;

			TRACE_P("MySQL/C LOAD DATA INFILE Read callback() Left - middle of batch, %d bytes chunk, %d bytes all", (int)(cur - buf), _ldi_bytes_all);
			TRACE_DMP(buf, (unsigned int)(cur - buf));
			return (int)(cur - buf);
		}
	}

	_ldi_current_row = 0;
	_ldi_current_col = 0;
	_ldi_current_col_len = 0;

	// Notify that the new portion of data processed
#if defined(WIN32) || defined(_WIN64)
	SetEvent(_completed_event);
#else
	Os::SetEvent(&_completed_event);
#endif

	TRACE_P("MySQL/C LOAD DATA INFILE Read callback() Left - Batch fully loaded, %d bytes last chunk, %d bytes batch, %d bytes all", 
		(int)(cur - buf), _ldi_bytes, _ldi_bytes_all);
	TRACE_DMP(buf, (unsigned int)(cur - buf));
	return (int)(cur - buf);
}

void SqlMysqlApi::local_infile_end(void * /*ptr*/)
{
	TRACE("MySQL/C LOAD DATA INFILE - End callback()");
	return;
}

int SqlMysqlApi::local_infile_error(void * /*ptr*/, char * /*error_msg*/, unsigned int /*error_msg_len*/)
{
	TRACE("MySQL/C LOAD DATA INFILE - Error callback()");
	return 0;
}

int SqlMysqlApi::local_infile_initS(void **ptr, const char * /*filename*/, void *userdata)
{
	TRACE_S(((SqlMysqlApi*)userdata), "MySQL/C LOAD DATA INFILE Init callback()");

	// Pass pointer to the API object
	*ptr = userdata;
	return 0;
}

int SqlMysqlApi::local_infile_readS(void *ptr, char *buf, unsigned int buf_len)
{
	if(ptr == NULL)
		return -1;

	SqlMysqlApi *mysqlApi = (SqlMysqlApi*)ptr;
	
	return mysqlApi->local_infile_read(buf, buf_len);
}

void SqlMysqlApi::local_infile_endS(void *ptr)
{
	SqlMysqlApi *mysqlApi = (SqlMysqlApi*)ptr;

	if(mysqlApi != NULL)
		mysqlApi->local_infile_end(ptr);
}

int SqlMysqlApi::local_infile_errorS(void *ptr, char *error_msg, unsigned int error_msg_len)
{
	SqlMysqlApi *mysqlApi = (SqlMysqlApi*)ptr;

	if(mysqlApi != NULL)
		mysqlApi->local_infile_error(ptr, error_msg, error_msg_len);

	return 0;
}

#if defined(WIN32) || defined(WIN64)
unsigned int __stdcall SqlMysqlApi::StartLoadDataInfileS(void *object)
{
	if(object == NULL)
		return (unsigned int)-1;
#else
void* SqlMysqlApi::StartLoadDataInfileS(void *object)
{
	if(object == NULL)
		return NULL;
#endif
	SqlMysqlApi *mysqlApi = (SqlMysqlApi*)object;

	TRACE_S(mysqlApi, "MySQL/C StartLoadDataInfileS() Entered");

	// Notify InitBulkCopy that the thread started
#if defined(WIN32) || defined(WIN64)
	SetEvent(mysqlApi->_completed_event);
#else
	Os::SetEvent(&mysqlApi->_completed_event);
#endif

	mysqlApi->_ldi_terminated = 0;

	// Execute LOAD DATA INFILE command
	mysqlApi->_ldi_rc = mysqlApi->ExecuteNonQuery(mysqlApi->_load_command.c_str(), NULL);

	mysqlApi->_ldi_terminated = 1;

	// Release thread called CloseBulkTransfer, so this connection can be used by other functions
	// If the table does not exist ExecuteNonQuery returns immediately, so notify TransferRows
#if defined(WIN32) || defined(WIN64)
	SetEvent(mysqlApi->_completed_event);
#else
	Os::SetEvent(&mysqlApi->_completed_event);
#endif

	TRACE_S(mysqlApi, "MySQL/C StartLoadDataInfileS() Left");
#if defined(WIN32) || defined(WIN64)
	return (unsigned int)mysqlApi->_ldi_rc;
#else
	return NULL;
#endif
}

// Complete bulk transfer
int SqlMysqlApi::CloseBulkTransfer()
{
	TRACE("MySQL/C CloseBulkTransfer() Entered");
	_ldi_current_row = (size_t)-1;

	// Do not wait for complete event if LOAD DATA INFILE command already ended
	if(_ldi_terminated == 0)
	{
		// Notify that there are no more rows
#if defined(WIN32) || defined(WIN64)
		SetEvent(_wait_event);

		// Wait until it processed
		WaitForSingleObject(_completed_event, INFINITE);
#else
		Os::SetEvent(&_wait_event);
		Os::WaitForEvent(&_completed_event);
#endif
	}

	// Check warnings and errors
	ShowWarnings(_load_command.c_str());

	TRACE("MySQL/C CloseBulkTransfer() Left");
	return 0;
}

// Execute SHOW WARNINGS and log results
void SqlMysqlApi::ShowWarnings(const char *prefix)
{
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	SqlCol *cols = NULL;

	int rc = OpenCursor("SHOW WARNINGS", 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, NULL, true);

	if(rc >= 0 && rows_fetched > 0)
	{
		LOG_P("\n    SHOW WARNINGS: %s", prefix);

		for(int i = 0; i < rows_fetched; i++)
		{
			std::string warn = "\n      ";
			SQLLEN len = (SQLLEN)cols[0].ind[i];

			// Level
			if(len != -1)
			{
				warn += cols[0]._name;
				warn += ": ";
				warn.append(cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				warn += "; ";
			}

			len = (SQLLEN)cols[1].ind[i];

			// Code
			if(len != -1)
			{
				warn += cols[1]._name;
				warn += ": ";
				warn.append(cols[1]._data + cols[1]._fetch_len * i, (size_t)len);
				warn += "; ";
			}

			len = (SQLLEN)cols[2].ind[i];

			// Code
			if(len != -1)
			{
				warn += cols[2]._name;
				warn += ": ";
				warn.append(cols[2]._data + cols[2]._fetch_len * i, (size_t)len);
			}

			LOG(warn.c_str());
		}
	}

	CloseCursor();
}

// Drop the table
int SqlMysqlApi::DropTable(const char* table, size_t *time_spent, std::string &drop_stmt)
{
	size_t start = Os::GetTickCount();

	// Table may be referenced by other tables, so remove foreign keys 
	int rc = DropReferences(table, NULL);

	drop_stmt = "DROP TABLE IF EXISTS ";
	drop_stmt += table;

	rc = ExecuteNonQuery(drop_stmt.c_str(), NULL);

	if(time_spent)
		*time_spent = Os::GetTickCount() - start;

	return rc;
}	

// Remove foreign key constraints referencing to the parent table
int SqlMysqlApi::DropReferences(const char* table, size_t *time_spent)
{
	if(table == NULL)
		return -1;

	size_t start = Os::GetTickCount();

	std::string db;
	std::string object;

	// Table can contain the database name
	SplitQualifiedName(table, db, object);

	std::list<std::string> drop_fk;

	// Query to find foreign keys on the table (this query takes 5-8 sec on first call even if there are 3 rows in the whole table)
	
	// INNODB_STATS_ON_METADATA=0 is set to significantly speed up access to INFORMATION_SCHEMA views
	// (about 5-10 times faster from 5-10 sec to 0.5-1 sec). But this requires SUPER privilege and may be OFF
	
	std::string query = "SELECT constraint_name, table_name FROM information_schema.referential_constraints ";
	query += "WHERE referenced_table_name = '";
	query += object;
	query += "'";

	// Execute the query
	int rc = _mysql_query(&_mysql, query.c_str());

	// Error raised
	if(rc != 0)
	{
		SetError();
		return -1;
	}

	MYSQL_RES *res = _mysql_use_result(&_mysql);

	if(res == NULL)
		return -1;

	MYSQL_ROW row = NULL;
	bool more = true;

	while(more)
	{
		// Fetch the next row
		row = _mysql_fetch_row(res);

		if(row == NULL)
		{
			more = false;
			break;
		}

		std::string drop = "ALTER TABLE ";
		drop += row[1];
		drop += " DROP FOREIGN KEY ";
		drop += row[0];

		// Add drop foreign key statement
		drop_fk.push_back(drop);
	}

	_mysql_free_result(res);

	// Drop foreign key constraints
	for(std::list<std::string>::iterator i = drop_fk.begin(); i != drop_fk.end(); i++)
	{
		// If at least one FK drop fails the table cannot be dropped
		rc = ExecuteNonQuery((*i).c_str(), NULL);

		if(rc == -1)
			break;
	}

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	// mysql_query returns 1 on failure
	return (rc == 0) ? 0 : -1;
}

// Get the length of LOB column in the open cursor
int SqlMysqlApi::GetLobLength(size_t /*row*/, size_t /*column*/, size_t * /*length*/)
{
	return -1;
}

// Get LOB content
int SqlMysqlApi::GetLobContent(size_t /*row*/, size_t /*column*/, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	return -1;
}

// Get the list of available tables
int SqlMysqlApi::GetAvailableTables(std::string &table_template, std::string & /*exclude*/, 
									std::list<std::string> &tables)
{
	std::string condition;

	// Get a condition to select objects from the catalog
	// MySQL contains the database name in "table_schema" column
	GetSelectionCriteria(table_template.c_str(), "table_schema", "table_name", condition, _db.c_str(), false);

	// Build the query
	std::string query = "SELECT table_schema, table_name FROM information_schema.tables";
	query += " WHERE table_type='BASE TABLE'";
	query += " AND table_schema NOT IN ('information_schema', 'mysql', 'performance_schema')";
	
	// Add filter
	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}

	// Execute the query
	int rc = _mysql_query(&_mysql, query.c_str());

	// Error raised
	if(rc != 0)
	{
		SetError();
		return -1;
	}

	MYSQL_RES *res = _mysql_use_result(&_mysql);

	if(res == NULL)
		return -1;

	bool more = true;

	while(more)
	{
		// Fetch the row
		MYSQL_ROW row = _mysql_fetch_row(res);

		// We must fetch all rows to avoid "out of sync" errors
		if(row == NULL)
		{
			more = false;
			break;
		}

		std::string tab;
		
		// Schema name
		if(row[0] != NULL)
		{
			tab += row[0];
			tab += ".";
		}

		// Table name
		if(row[1] != NULL)
			tab += row[1];

		tables.push_back(tab);
	}

	_mysql_free_result(res);

	return 0;
}

// Read schema information
int SqlMysqlApi::ReadSchema(const char * /*select*/, const char * /*exclude*/, bool /*read_cns*/, bool /*read_idx*/)
{
	return -1;
}

// Get table name by constraint name
int SqlMysqlApi::ReadConstraintTable(const char * /*schema*/, const char * /*constraint*/, std::string & /*table*/)
{
	return -1;
}

// Read information about constraint columns
int SqlMysqlApi::ReadConstraintColumns(const char * /*schema*/, const char * /*table*/, const char * /*constraint*/, std::string & /*cols*/)
{
	return -1;
}

// Set version of the connected database
void SqlMysqlApi::SetVersion()
{
	// Note: INFORMATION_SCHEMA.GLOBAL_VARIABLES is available since 5.1 only

	// Returns 4 rows and 2 columns: Variable_name and Value
	// 5.5.15	MySQL Community Server (GPL)	x86		Win32
	const char *query = "SHOW VARIABLES LIKE 'VERSION%'";

	// Execute the query
	int rc = _mysql_query(&_mysql, query);

	// Error raised
	if(rc != 0)
	{
		SetError();
		return;
	}

	MYSQL_RES *res = _mysql_use_result(&_mysql);

	if(res == NULL)
		return;

	_version.clear();

	int i = 0;
	bool more = true;

	while(more)
	{
		// Fetch the row
		MYSQL_ROW row = _mysql_fetch_row(res);

		if(row == NULL || row[1] == NULL)
		{
			more = false;
			break;
		}

		if(i > 0)
			_version += " ";

		// row[1] contains 2nd column as a null-terminated string
		_version += row[1];

		// Set version numbers
		if(row[0] != NULL && strcmp(row[0], "version") == 0)
		{
			sscanf(row[1], "%d.%d.%d", &_version_major, &_version_minor, &_version_release); 
		}

		i++;
	}

	_mysql_free_result(res);
}

// Find MySQL installation paths
void SqlMysqlApi::FindMysqlPaths()
{
#if defined(WIN32) || defined(_WIN64)
	HKEY hKey, hSubkey;

	// Main key is "Software\\MySQL AB" for MySQL 5.5 and 5.6
	// MySQL Installer for MySQL 5.7 puts keys even for 64-bit version to "Software\\WOW6432Node\\MySQL AB"
	char *keys[] = { "Software\\MySQL AB", "Software\\WOW6432Node\\MySQL AB" };
	
	char key[1024];
	char location[1024];

	for(int k = 0; k < 2; k++)
	{
		int rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keys[k], 0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKey);

		if(rc != ERROR_SUCCESS)
			return;

		DWORD i = 0;
		bool more = true;

		// Enumerate through all keys
		while(more)
		{
			// Size modified with each call to RegEnumKeyEx
			int size = 1024;

			// Get next key
			rc = RegEnumKeyEx(hKey, i, key, (LPDWORD)&size, NULL, NULL, NULL, NULL);
 
			if(rc != ERROR_SUCCESS)
			{
				more = false;
				break;
			}

			i++;

			// Key is "MySQL Server 5.5" for version 5.5, 5.6 and 5.7
			if(strncmp(key, "MySQL Server", 12) != 0)
				continue;

			rc = RegOpenKeyEx(hKey, key, 0, KEY_READ, &hSubkey);

			if(rc != ERROR_SUCCESS)
				break;
			
			int value_size = 1024;

			rc = RegQueryValueEx(hSubkey, "Location", NULL, NULL, (LPBYTE)location, (LPDWORD)&value_size); 

			if(rc == ERROR_SUCCESS)
			{
				// For MySQL 5.5, 5.6 and 5.7 location includes terminating '\'
				std::string loc = location;
				loc += "lib\\";

				_driver_paths.push_back(loc);
			}
  
			RegCloseKey(hSubkey);	
		}

		RegCloseKey(hKey);
	}

#endif
}

// Set error code and message for the last API call
void SqlMysqlApi::SetError()
{
	// Get native error code
	_native_error = (int)_mysql_errno(&_mysql);
	
	// Get native error text
	strcpy(_native_error_text, _mysql_error(&_mysql));

	_error = SQL_DBAPI_UNKNOWN_ERROR;
	*_error_text = '\x0';
}
