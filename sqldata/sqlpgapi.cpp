
// SqlPgApi PostgreSQL libpq C API
// Copyright (c) 2012 SQLines. All rights reserved

#include <stdio.h>
#include "sqlpgapi.h"
#include "str.h"
#include "os.h"

// Required to access ODBC, CT-Lib, MySQL C constants
#include <sql.h>
#include <sqlext.h>
#include <ctpublic.h>
#include <mysql.h>

// Constructor
SqlPgApi::SqlPgApi()
{
	_cursor_result = NULL;
	_cursor_rows_count = 0;
	_cursor_rows_fetched = 0;

	_copy_cols_count = 0;
	_copy_data = NULL;

	_conn = NULL;
	_dll = NULL;

	_PQclear = NULL;
	_PQerrorMessage = NULL;
	_PQexec = NULL;
	_PQfmod = NULL;
	_PQfname = NULL;
	_PQfsize = NULL;
	_PQftype = NULL;
	_PQgetisnull = NULL;
	_PQgetlength = NULL;
	_PQgetResult = NULL;
	_PQgetvalue = NULL;
	_PQnfields = NULL;
	_PQntuples = NULL;
	_PQputCopyData = NULL;
	_PQputCopyEnd = NULL;
	_PQresultErrorMessage = NULL;
	_PQresultStatus = NULL;
	_PQsetClientEncoding = NULL;
	_PQsetdbLogin = NULL;
	_PQstatus = NULL;
}

SqlPgApi::~SqlPgApi()
{
}

// Initialize API
int SqlPgApi::Init()
{
	// Try to load the library by default path
	_dll = Os::LoadLibrary(LIBPQ_C_DLL);

	// DLL load failed
	if(_dll == NULL)
	{
#if defined(WIN32) || defined(_WIN64)

		std::list<std::string> paths;

		// Try to find PostgreSQL installation paths
		FindPgPaths(paths);

		for(std::list<std::string>::iterator i = paths.begin(); i != paths.end() ; i++)
		{
			std::string loc = (*i) + "\\bin\\";
			loc += LIBPQ_C_DLL;

			// Try to open DLL
			_dll = LoadLibraryEx(loc.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			if(_dll != NULL)
				break;
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
		_PQclear = (PQclearFunc)Os::GetProcAddress(_dll, "PQclear");
		_PQerrorMessage = (PQerrorMessageFunc)Os::GetProcAddress(_dll, "PQerrorMessage");
		_PQexec = (PQexecFunc)Os::GetProcAddress(_dll, "PQexec");
		_PQfmod = (PQfmodFunc)Os::GetProcAddress(_dll, "PQfmod");
		_PQfname = (PQfnameFunc)Os::GetProcAddress(_dll, "PQfname");
		_PQfsize = (PQfsizeFunc)Os::GetProcAddress(_dll, "PQfsize");
		_PQftype = (PQftypeFunc)Os::GetProcAddress(_dll, "PQftype");
		_PQgetisnull = (PQgetisnullFunc)Os::GetProcAddress(_dll, "PQgetisnull");
		_PQgetlength = (PQgetlengthFunc)Os::GetProcAddress(_dll, "PQgetlength");
		_PQgetResult = (PQgetResultFunc)Os::GetProcAddress(_dll, "PQgetResult");
		_PQgetvalue = (PQgetvalueFunc)Os::GetProcAddress(_dll, "PQgetvalue");
		_PQnfields = (PQnfieldsFunc)Os::GetProcAddress(_dll, "PQnfields");
		_PQntuples = (PQntuplesFunc)Os::GetProcAddress(_dll, "PQntuples");
		_PQputCopyData = (PQputCopyDataFunc)Os::GetProcAddress(_dll, "PQputCopyData");
		_PQputCopyEnd = (PQputCopyEndFunc)Os::GetProcAddress(_dll, "PQputCopyEnd");
		_PQresultErrorMessage = (PQresultErrorMessageFunc)Os::GetProcAddress(_dll, "PQresultErrorMessage");
		_PQresultStatus = (PQresultStatusFunc)Os::GetProcAddress(_dll, "PQresultStatus");
		_PQsetClientEncoding = (PQsetClientEncodingFunc)Os::GetProcAddress(_dll, "PQsetClientEncoding");
		_PQsetdbLogin = (PQsetdbLoginFunc)Os::GetProcAddress(_dll, "PQsetdbLogin");
		_PQstatus = (PQstatusFunc)Os::GetProcAddress(_dll, "PQstatus");

		if(_PQclear == NULL || _PQerrorMessage == NULL || _PQexec == NULL || _PQfmod == NULL ||
			_PQfname == NULL || _PQfsize == NULL || _PQftype == NULL || _PQgetisnull == NULL || 
			_PQgetlength == NULL || _PQgetResult == NULL || _PQgetvalue == NULL || _PQnfields == NULL || 
			_PQntuples == NULL || _PQputCopyData == NULL || _PQputCopyEnd == NULL || 
			_PQresultErrorMessage == NULL || _PQresultStatus == NULL || _PQsetClientEncoding == NULL || 
			_PQsetdbLogin == NULL || _PQstatus == NULL)
			return -1;
	}
	else
	{
		Os::GetLastErrorText(LIBPQ_DLL_LOAD_ERROR, _native_error_text, 1024);
		return -1;
	}

	return 0;
}

// Free connection and environment handles (When other ODBC drivers are used crash can happen on exit when not disconnected and not deallocated)
void SqlPgApi::Deallocate()
{
}

// Set the connection string in the API object
void SqlPgApi::SetConnectionString(const char *conn)
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
		if(comma != NULL && comma > semi)
			_port.assign(semi + 1, (size_t)(comma - semi - 1));
		else
			_port = semi + 1;
	}

	if(comma != NULL)
		_db = Str::SkipSpaces(comma + 1);
}

// Connect to the database
int SqlPgApi::Connect(size_t *time_spent)
{
	// Check if already connected
	if(_connected == true)
		return 0;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;

	const char *server = _server.empty() ? NULL : _server.c_str();
	const char *port = _port.empty() ? NULL : _port.c_str();
	const char *db = _db.empty() ? NULL : _db.c_str();
	const char *user = _user.empty() ? NULL : _user.c_str();
	const char *pwd = _pwd.empty() ? NULL : _pwd.c_str();

	_conn = _PQsetdbLogin(server, port, NULL, NULL, db, user, pwd);

	if(_PQstatus(_conn) != CONNECTION_OK)
	{
		SetError();

		if(time_spent != NULL)
			*time_spent = Os::GetTickCount() - start;

		return -1;
	}

	// Set version of the connected database
	SetVersion();

	// Initialize session
	InitSession();

	_connected = true;

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return 0;
}

// Get row count for the specified object
int SqlPgApi::GetRowCount(const char *object, int *count, size_t *time_spent)
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
int SqlPgApi::ExecuteScalar(const char *query, int *result, size_t *time_spent)
{
	if(query == NULL || result == NULL)
		return -1;

	size_t start = Os::GetTickCount();

	// Execute the statement
	PGresult *res = _PQexec(_conn, query);

	bool exists = false;

	// Get the value of the first column of the first row
	if(_PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		char *value = _PQgetvalue(res, 0, 0);
		sscanf(value, "%d", result);

		exists = true;
	}
	else
		SetError();

	_PQclear(res);

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	return (exists == true) ? 0 : -1;
}

// Execute the statement
int SqlPgApi::ExecuteNonQuery(const char *query, size_t *time_spent)
{
	if(query == NULL)
		return -1;

	size_t start = Os::GetTickCount();

	// Execute the query
	PGresult *result = _PQexec(_conn, query);

	bool error = false;

	int rc = _PQresultStatus(result);

	// Error raised
	if(rc != PGRES_COMMAND_OK)
	{
		SetError();
		error = true;
	}

	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;

	_PQclear(result);

	return (error == true) ? -1 : 0;
}

// Open cursor and allocate buffers
int SqlPgApi::OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool /*catalog_query*/, std::list<SqlDataTypeMap> * /*dtmap*/)
{
	if(query == NULL)
		return -1;

	_cursor_rows_count = 0;
	_cursor_rows_fetched = 0;

	size_t start = Os::GetTickCount();

	// Execute the query
	_cursor_result = _PQexec(_conn, query);

	// Error raised
	if(_PQresultStatus(_cursor_result) != PGRES_TUPLES_OK)
	{
		SetError();
		_PQclear(_cursor_result);
		_cursor_result = NULL;

		if(time_spent != NULL)
			*time_spent = Os::GetTickCount() - start;

		return -1;
	}

	// Get the number of columns
	_cursor_cols_count = (size_t)_PQnfields(_cursor_result);

	// Get the number of rows in the result set
	_cursor_rows_count = (size_t)_PQntuples(_cursor_result);

	if(_cursor_cols_count > 0)
		_cursor_cols = new SqlCol[_cursor_cols_count];

	size_t row_size = 0;

	// Get column information
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		// Get column name
		char *colname = _PQfname(_cursor_result, i);

		if(colname != NULL)
			strcpy(_cursor_cols[i]._name, colname);

		// Native data type
		_cursor_cols[i]._native_dt = (int)_PQftype(_cursor_result, i);

		// For CHAR, VARCHAR, NUMERIC/DECIMAL PQfsize returns -1
		int len = _PQfsize(_cursor_result, i);

		// 4 returned for INT, but string representation is used
		if(_cursor_cols[i]._native_dt == PG_DT_INT4)
			len = 11;
		else
		// 2 returned for SMALLINT, but 5 required for string 
		if(_cursor_cols[i]._native_dt == PG_DT_INT2)
			len = 5;
		else
		// 4 returned for DATE, but 10 required for string representation
		if(_cursor_cols[i]._native_dt == PG_DT_DATE)
			len = 10;
		else
		// 8 returned for FLOAT, not enough for string representation
		if(_cursor_cols[i]._native_dt ==  PG_DT_FLOAT)
			len = 21;

		// In this case we have to use PQfmod
		if(len == -1)
		{
			len = _PQfmod(_cursor_result, i);

			// For CHAR and VARCHAR size + 4 is returned
			if(_cursor_cols[i]._native_dt == PG_DT_BPCHAR || _cursor_cols[i]._native_dt == PG_DT_VARCHAR)
			{
				if(len > 4)
					len -= 4;
			}
			else
			// NUMERIC and DECIMAL; for (9,2) value is 0009 0006
			if(_cursor_cols[i]._native_dt == PG_DT_NUMERIC)
			{
				// Precision
				_cursor_cols[i]._precision = (len >> 16);
				// Scale
				_cursor_cols[i]._scale = (len & 0xFFFF) - 4;

				// Total number of characters 
				len = _cursor_cols[i]._precision + _cursor_cols[i]._scale + 2;				
			}
			else
			// PQfmod can return a large value; limit the length for unknown types
			if(len > 30)
				len = 30;
		}
            
		// Column length in bytes 
		_cursor_cols[i]._len = (size_t)len;

		/*
		// BLOB 
		if(_cursor_cols[i]._native_dt == MYSQL_TYPE_BLOB)
		{
			_cursor_cols[i]._lob = true;
			_cursor_lob_exists = true;
		}*/

		row_size += _cursor_cols[i]._len;

		// NOT NULL attribute
		//_cursor_cols[i]._nullable = (fields[i].flags & NOT_NULL_FLAG) ? false : true;
	}

	_cursor_allocated_rows = 1;

	// Define how many rows fetch at once
	if(buffer_rows > 0)
		_cursor_allocated_rows = buffer_rows;
	else
	if(buffer_memory > 0)
	{
		size_t rows = buffer_memory/row_size;
		_cursor_allocated_rows = rows > 0 ? rows : 1;
	}	

	// No need to allocate more than actual number of rows in the result set
	if(_cursor_allocated_rows > _cursor_rows_count)
	{
		_cursor_allocated_rows = (_cursor_rows_count != 0) ? _cursor_rows_count : 1;
	}

	// Allocate buffers to each column
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		_cursor_cols[i]._native_fetch_dt = _cursor_cols[i]._native_dt;
		_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;

		_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		_cursor_cols[i].ind = new size_t[_cursor_allocated_rows];
	}

	int fetched = 0;

	// Fetch initial set of data
	Fetch(&fetched, NULL);

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
int SqlPgApi::Fetch(int *rows_fetched, size_t *time_spent) 
{
	int fetched = 0;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;

	// Fill the buffer
	for(int i = 0; i < _cursor_allocated_rows; i++)
	{
		if(_cursor_rows_fetched >= _cursor_rows_count)
			break;
	
		// Copy column values and set indicators
		for(int k = 0; k < _cursor_cols_count; k++)
		{
			// Check for NULL value
			int null = _PQgetisnull(_cursor_result, _cursor_rows_fetched, k);

			if(null == 1)
			{
				_cursor_cols[k].ind[i] = (size_t)-1;
				continue;
			}

			// Get the actual data length in bytes
			size_t len = (size_t)_PQgetlength(_cursor_result, _cursor_rows_fetched, k);			
			
			if(len > _cursor_cols[k]._fetch_len)
				len = _cursor_cols[k]._fetch_len;
				
			_cursor_cols[k].ind[i] = len;

			// Get column data
			char *data = _PQgetvalue(_cursor_result, _cursor_rows_fetched, k);

			// Copy data
			memcpy(_cursor_cols[k]._data + _cursor_cols[k]._fetch_len * i, data, len);
		}

		_cursor_rows_fetched++;
		fetched++;
	}

	if(rows_fetched != NULL)
		*rows_fetched = fetched;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return 0;
}

// Close the cursor and deallocate buffers
int SqlPgApi::CloseCursor()
{
	if(_cursor_result != NULL)
		_PQclear(_cursor_result);

	_cursor_result = NULL;

	_cursor_rows_count = 0;
	_cursor_rows_fetched = 0;

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
int SqlPgApi::InitBulkTransfer(const char *table, size_t col_count, size_t /*allocated_array_rows*/, SqlCol * /*s_cols*/, SqlCol ** /*t_cols*/)
{
	std::string command = "COPY ";
	command += table;
	command += " FROM STDIN"; 

	_copy_cols_count = col_count;

	// Execute COPY command 
	PGresult* result = _PQexec(_conn, command.c_str());

	if(_PQresultStatus(result) != PGRES_COPY_IN)
	{
		_PQclear(result);
		return -1;
	}

	_copy_data = new char[LIBPQ_COPY_DATA_BUFFER_LEN];

	_PQclear(result);
		
	return 0;
}

// Transfer rows between databases
int SqlPgApi::TransferRows(SqlCol *s_cols, int rows_fetched, int *rows_written, size_t *bytes_written,
							size_t *time_spent)
{
	if(rows_fetched == 0)
		return 0;

	size_t start = Os::GetTickCount();

	int rc = 0;
	size_t bytes = 0;

	char *cur = _copy_data;
	int remain_len = LIBPQ_COPY_DATA_BUFFER_LEN;

	// Copy rows
	for(size_t i = 0; i < rows_fetched; i++)
	{
		// Copy column data
		for(size_t k = 0; k < _copy_cols_count; k++)
		{
			int len = -1;
			char *lob_data = NULL;

			// Check whether column is null
			if(_source_api_type == SQLDATA_ORACLE && s_cols[k]._ind2 != NULL)
			{
				if(s_cols[k]._ind2[i] != -1)
				{
					// Oracle DATE column will be written as string
					if(s_cols[k]._native_fetch_dt == SQLT_DAT)
						len = 19;
					else
					// LOB column
					if(s_cols[k]._native_fetch_dt == SQLT_BLOB || s_cols[k]._native_fetch_dt == SQLT_CLOB)
					{
						size_t lob_size = 0;

						// Get the LOB size in bytes for BLOB, in characters for CLOB
						int lob_rc = _source_api_provider->GetLobLength(i, k, &lob_size);

						// Probably empty LOB
						if(lob_rc != -1)
							len = (int)lob_size;

						if(lob_rc != -1 && lob_size > 0)
						{
							size_t alloc_size = 0;
							int read_size = 0;

							lob_data = _source_api_provider->GetLobBuffer(i, k, lob_size, &alloc_size);

							// Get LOB content
							lob_rc = _source_api_provider->GetLobContent(i, k, lob_data, alloc_size, &read_size);

							// Error reading LOB
							if(lob_rc != 0)
							{
								_source_api_provider->FreeLobBuffer(lob_data);
								lob_data = NULL;
								len = -1;								
							}
							else
								len = read_size;
						}						
					}
					else
						len = s_cols[k]._len_ind2[i];
				}
			}
			else
			// Sybase ASE
			if(_source_api_type == SQLDATA_SYBASE && s_cols[k]._ind2 != NULL)
			{
				if(s_cols[k]._ind2[i] != -1)
					len = s_cols[k]._len_ind4[i];
			}
			else
			// ODBC indicator contains either NULL or length
			if((_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_DB2 ||
				_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_MYSQL ||
				_source_api_type == SQLDATA_ODBC) 
				&& s_cols[k].ind != NULL)
			{
				len = (int)s_cols[k].ind[i];
			}
			
			// Check if we still have space to write column data, NULL value and delimiters
			if(remain_len < 5 || (len != -1 && remain_len < len + 3))
			{
				rc = _PQputCopyData(_conn, _copy_data, (int)(cur - _copy_data));

				cur = _copy_data;
				remain_len = LIBPQ_COPY_DATA_BUFFER_LEN;
			}

			// Add column delimiter
			if(k > 0)
			{
				*cur = '\t';
				cur++;
				remain_len--;

				bytes++;
			}

			// Write NULL value
			if(len == -1)
			{
				cur[0] = '\\'; cur[1] = 'N';
				cur += 2;
				remain_len -= 2;

				bytes += 2;
			}
			else
			// Oracle CHAR, VARCHAR2, LOBs 
			if((_source_api_type == SQLDATA_ORACLE && 
				(s_cols[k]._native_fetch_dt == SQLT_STR || s_cols[k]._native_fetch_dt == SQLT_BLOB || s_cols[k]._native_fetch_dt == SQLT_CLOB ||
                    s_cols[k]._native_fetch_dt == SQLT_BIN || s_cols[k]._native_fetch_dt == SQLT_LNG)) ||
				// Sybase CHAR, INT, SMALLINT
				(_source_api_type == SQLDATA_SYBASE && s_cols[k]._native_fetch_dt == CS_CHAR_TYPE) ||
				// ODBC CHAR, BINARY
				((_source_api_type == SQLDATA_ODBC || _source_api_type == SQLDATA_INFORMIX || 
				 _source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_SQL_SERVER) && 
				 (s_cols[k]._native_fetch_dt == SQL_C_CHAR || s_cols[k]._native_fetch_dt == SQL_C_BINARY ||
				   s_cols[k]._native_fetch_dt == SQL_C_WCHAR)) ||
				// MySQL CHAR
				(_source_api_type == SQLDATA_MYSQL && s_cols[k]._native_fetch_dt == MYSQL_TYPE_STRING))
			{

				// Copy data handling escape characaters
				for(int m = 0; m < len; m++)
				{
					char c = 0;

					if(lob_data == NULL)
						c = (s_cols[k]._data + s_cols[k]._fetch_len * i)[m];
					else
						c = lob_data[m];

					// Duplicate escape \ character
					if(c == '\\')
					{
						cur[0] = c;
						cur[1] = c;
						
						cur += 2;
						remain_len -= 2;
						bytes += 2;
					}
					else
					// Escape delimiter or new line in data
					if(c == '\t' || c == '\r' || c == '\n')
					{
						cur[0] = '\\';

						if(c == '\t')
							cur[1] = 't';
						if(c == '\r')
							cur[1] = 'r';
						if(c == '\n')
							cur[1] = 'n';

						cur += 2;
						remain_len -= 2;
						bytes += 2;
					}
					else
					// Zero byte must be escaped (can appear in binary data)
					if(c == '\x0')
					{
						// '\' must be itself escaped
						cur[0] = '\\';
						cur[1] = '\\';
						cur[2] = '0';
						cur[3] = '0';
						cur[4] = '0';

						cur += 5;
						remain_len -= 5;
						bytes += 5;
					}
					else
					{
						*cur = c;

						cur++;
						remain_len--;
						bytes++;
					}

					// Check if we still have space to write the next byte of column data (can explode due to escape sequences)
					if(m + 1 < len && remain_len < 5)
					{
						rc = _PQputCopyData(_conn, _copy_data, (int)(cur - _copy_data));

						cur = _copy_data;
						remain_len = LIBPQ_COPY_DATA_BUFFER_LEN;
					}
				}
			}
			else
			// Oracle DATE fetched as 7 byte binary sequence
			if(_source_api_type == SQLDATA_ORACLE && s_cols[k]._native_fetch_dt == SQLT_DAT)
			{
				// Unsigned required for proper conversion to int
				unsigned char *data = (unsigned char*)(s_cols[k]._data + s_cols[k]._fetch_len * i);

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

				bytes += 19;
			}
			else
			// ODBC TIMESTAMP fetched as SQL_TIMESTAMP_STRUCT
			if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX || 
				_source_api_type == SQLDATA_ASA || _source_api_type == SQLDATA_ODBC) && 
				s_cols[k]._native_fetch_dt == SQL_C_TYPE_TIMESTAMP)
			{
				size_t offset = sizeof(SQL_TIMESTAMP_STRUCT) * i;
				
				// Informix returns '1200-01-01 11:11:00.0' for 11:11 (HOUR TO MINUTE)
				SQL_TIMESTAMP_STRUCT *ts = (SQL_TIMESTAMP_STRUCT*)(s_cols[k]._data + offset);

				long fraction = (long)ts->fraction;

				// Sybase ASA 9 stores 6-digit fraction multiplied by 1000, i.e. 123456 stored as 123456000
				if(_source_api_type == SQLDATA_ASA && fraction % 1000 == 0)
					fraction = fraction/1000;

				// Convert SQL_TIMESTAMP_STRUCT to string (puts terminating 0)
				Str::SqlTs2Str((short)ts->year, (short)ts->month, (short)ts->day, (short)ts->hour, (short)ts->minute, (short)ts->second, fraction, cur);

				cur += 26;
				remain_len -= 26;

				bytes += 26;
			}

			if(lob_data != NULL)
				_source_api_provider->FreeLobBuffer(lob_data);
		}

		// Add row delimiter (no need to write \r for Windows)
		*cur = '\n';
		cur++;
		remain_len--;

		bytes++;
	}

	// Write last portion of data
	rc = _PQputCopyData(_conn, _copy_data, (int)(cur - _copy_data));

	if(time_spent)
		*time_spent = GetTickCount() - start;

	if(rows_written != NULL)
		*rows_written = rows_fetched;

	if(bytes_written != NULL)
		*bytes_written = bytes;

	if(time_spent)
		*time_spent = GetTickCount() - start;

	return 0;
}

// Write LOB data 
int SqlPgApi::WriteLob(SqlCol * /*s_cols*/, int /*row*/, int * /*lob_bytes*/)
{
	return -1;
}

// Complete bulk transfer
int SqlPgApi::CloseBulkTransfer()
{
	// End COPY command (returns 1 on success)
	int rc = (_PQputCopyEnd(_conn, NULL) == 1) ? 0 : -1;

	PGresult *res = _PQgetResult(_conn);

	// Get the command result
	ExecStatusType code = _PQresultStatus(res);

	if(code == PGRES_FATAL_ERROR)
	{
		char *error = _PQresultErrorMessage(res);
		strcpy(_native_error_text, error);

		_error = -1;
		rc = -1;
	}
	
	delete _copy_data;
	_copy_data = NULL;

	return rc;
}

// Drop the table
int SqlPgApi::DropTable(const char* table, size_t *time_spent, std::string &drop_stmt)
{
	drop_stmt = "DROP TABLE IF EXISTS ";
	drop_stmt += table;

	// Specify CASCADE to drop views and foreign key constraints
	drop_stmt += " CASCADE";

	int rc = ExecuteNonQuery(drop_stmt.c_str(), time_spent);

	return rc;
}	

// Remove foreign key constraints referencing to the parent table
int SqlPgApi::DropReferences(const char* /*table*/, size_t * /*time_spent*/)
{
	return -1;
}

// Get the length of LOB column in the open cursor
int SqlPgApi::GetLobLength(size_t /*row*/, size_t /*column*/, size_t * /*length*/)
{
	return -1;
}

// Get LOB content
int SqlPgApi::GetLobContent(size_t /*row*/, size_t /*column*/, void * /*data*/, size_t /*length*/, int * /*len_ind*/)
{
	return -1;
}

// Get the list of available tables
int SqlPgApi::GetAvailableTables(std::string & /*table_template*/, std::string & /*exclude*/, std::list<std::string> & /*tables*/)
{
	return -1;
}

// Read schema information
int SqlPgApi::ReadSchema(const char * /*select*/, const char * /*exclude*/, bool /*read_cns*/, bool /*read_idx*/)
{
	return -1;
}

// Get table name by constraint name
int SqlPgApi::ReadConstraintTable(const char * /*schema*/, const char * /*constraint*/, std::string & /*table*/)
{
	return -1;
}

// Read information about constraint columns
int SqlPgApi::ReadConstraintColumns(const char * /*schema*/, const char * /*table*/, const char * /*constraint*/, std::string & /*cols*/)
{
	return -1;
}

// Build a condition to select objects from the catalog
void SqlPgApi::GetCatalogSelectionCriteria(std::string & /*selection_template*/, std::string & /*output*/)
{
}

// Initialize session
void SqlPgApi::InitSession()
{
	int rc = 0;

	// Client encoding is UTF-8 by default while data in ASCII, read encoding values
	const char *client_encoding = NULL;

	if(_parameters != NULL)
		client_encoding = _parameters->Get("-pg_client_encoding");

	if(client_encoding != NULL)
		rc = _PQsetClientEncoding(_conn, client_encoding);
}

// Set version of the connected database
void SqlPgApi::SetVersion()
{
	// Values returned:
	// PostgreSQL 9.0.4, compiled by Visual C++ build 1500, 32-bit
	// PostgreSQL 9.2.4, compiled by Visual C++ build 1600, 32-bit
	const char *query = "SELECT version()";

	// Execute the statement
	PGresult *res = _PQexec(_conn, query);

	char *value = NULL;

	// Get the value of the first column of the first row
	if(_PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		value = _PQgetvalue(res, 0, 0);
		_version.assign(value);
	}

	// Define major, minor and release numbers
	if(value != NULL && strlen(value) > 11)
	{
		// Skip "PostgreSQL "
		sscanf(value + 11, "%d.%d.%d", &_version_major, &_version_minor, &_version_release); 
	}

	_PQclear(res);
}

// Find PostgreSQL installation paths
void SqlPgApi::FindPgPaths(std::list<std::string> &paths)
{
#if defined(WIN32) || defined(_WIN64)
	HKEY hKey, hSubkey;

	// At least for PostgreSQL 9.0 
	int rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\PostgreSQL\\Installations", 0, KEY_READ, &hKey);

	if(rc != ERROR_SUCCESS)
		return;

	char key[1024];
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

		// At least for version 9.0 key is "postgresql-90"
		if(strncmp(key, "postgresql", 10) != 0)
			continue;

		rc = RegOpenKeyEx(hKey, key, 0, KEY_READ, &hSubkey);

		if(rc != ERROR_SUCCESS)
			break;

		char location[1024];
		int value_size = 1024;

		rc = RegQueryValueEx(hSubkey, "Base Directory", NULL, NULL, (LPBYTE)location, (LPDWORD)&value_size); 

		if(rc == ERROR_SUCCESS)
			paths.push_back(std::string(location));
  
		RegCloseKey(hSubkey);	
	}

	RegCloseKey(hKey);

#endif
}

// Set error code and message for the last API call
void SqlPgApi::SetError()
{
	// Get native error text
	strcpy(_native_error_text, _PQerrorMessage(_conn));

	_error = SQL_DBAPI_UNKNOWN_ERROR;
	*_error_text = '\x0';
}
