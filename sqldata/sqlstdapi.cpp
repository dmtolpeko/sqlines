/** 
 * Copyright (c) 2017 SQLines
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

// SqlStdApi Standard Output API

#include <stdio.h>
#include "sqlstdapi.h"
#include "sqlociapi.h"
#include "sqlmysqlapi.h"
#include "sqlctapi.h"
#include "../sqlcommon/str.h"
#include "../sqlcommon/os.h"

// Constructor
SqlStdApi::SqlStdApi()
{
}

SqlStdApi::~SqlStdApi()
{
}

// Initialize API
int SqlStdApi::Init()
{
	// No driver load is required
	return 0;
}

// Set additional information about the driver type
void SqlStdApi::SetDriverType(const char * /*info*/)
{
}

// Set the connection string in the API object
void SqlStdApi::SetConnectionString(const char* /*conn*/)
{
}

// Connect to the database
int SqlStdApi::Connect(size_t * /*time_spent*/)
{
	return 0;
}

// Get row count for the specified object
int SqlStdApi::GetRowCount(const char* /*object*/, long* /*count*/, size_t* /*time_spent*/)
{
	return -1;
}

// Execute the statement and get scalar result
int SqlStdApi::ExecuteScalar(const char* /*query*/, long* /*result*/, size_t* /*time_spent*/)
{
	return -1;
}

// Execute the statement
int SqlStdApi::ExecuteNonQuery(const char* /*query*/, size_t* /*time_spent*/)
{
	return -1;
}

// Open cursor and allocate buffers
int SqlStdApi::OpenCursor(const char* /*query*/, long /*buffer_rows*/, long /*buffer_memory*/, long* /*col_count*/, long* /*allocated_array_rows*/, 
		long* /*rows_fetched*/, SqlCol** /*cols*/, size_t* /*time_spent*/, bool /*catalog_query*/, std::list<SqlDataTypeMap>* /*dtmap*/)
{
	return -1;
}

// Fetch next portion of data to allocate buffers
int SqlStdApi::Fetch(long* /*rows_fetched*/, size_t* /*time_spent*/) 
{
	return -1;
}

// Close the cursor and deallocate buffers
int SqlStdApi::CloseCursor()
{
	return -1;
}

// Initialize the bulk copy from one database into another
int SqlStdApi::InitBulkTransfer(const char * /*table*/, long col_count, long allocated_array_rows, SqlCol * /*s_cols*/, SqlCol ** /*t_cols*/)
{
	_ins_cols_count = col_count;
	_ins_allocated_rows = allocated_array_rows;
	_ins_all_rows_inserted = 0;

	return 0;
}

// Transfer rows between databases
int SqlStdApi::TransferRows(SqlCol *s_cols, long rows_fetched, long *rows_written, size_t *bytes_written,
							size_t *time_spent)
{
	size_t bytes = 0;
	int rc = 0;

	if(rows_fetched <= 0)
		return -1;

	size_t start = GetTickCount();

	// Prepare buffers and calculate data size
	for(long i = 0; i < rows_fetched; i++)
	{
		for(long k = 0; k < _ins_cols_count; k++)
		{
			// Output colum delimiter
			if(k > 0)
				printf("\t");	

			// Length indicator
			long ind = -1;

			if(_source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_SQL_SERVER || 
				_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_MYSQL ||
				_source_api_type == SQLDATA_ASA)
			{
				if(s_cols[k].ind != NULL)
					ind = s_cols[k].ind[i];

#if defined(WIN64)
				// DB2 11 64-bit CLI driver still writes indicators to 4-byte array
				if(_source_api_type == SQLDATA_DB2 /*&& s_cols[k].ind[0] & 0xFFFFFFFF00000000*/)
					ind = ((int*)(s_cols[k].ind))[i];
#endif
			}
            else
			if(_source_api_type == SQLDATA_ORACLE && s_cols[k]._ind2 != NULL && s_cols[k]._ind2[i] != -1)
	            ind = s_cols[k]._len_ind2[i];
    		else
			if(_source_api_type == SQLDATA_SYBASE && s_cols[k]._ind2 != NULL && s_cols[k]._ind2[i] != -1)
				ind = s_cols[k]._len_ind4[i];

			// Calculate size for non-NULL values
			if(ind != -1)
				bytes += ind;
			else
				continue;

			// Oracle CHAR, VARCHAR2
			if((_source_api_type == SQLDATA_ORACLE && s_cols[k]._native_fetch_dt == SQLT_STR) ||
				// Sybase CHAR
				(_source_api_type == SQLDATA_SYBASE && s_cols[k]._native_fetch_dt == CS_CHAR_TYPE) ||
				// ODBC CHAR
				((_source_api_type == SQLDATA_ODBC || _source_api_type == SQLDATA_INFORMIX || 
				 _source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_SQL_SERVER) && 
							s_cols[k]._native_fetch_dt == SQL_C_CHAR))
			{
				char *data = s_cols[k]._data + s_cols[k]._fetch_len * i;
				int precision = static_cast<int>(ind);
				printf("%.*s", precision, data);				
			}	
		}

		// Output the row delimiter
		printf("\n");
	}

	_ins_all_rows_inserted += rows_fetched;

	if(rows_written != NULL)
		*rows_written = rows_fetched;

	if(bytes_written != NULL)
		*bytes_written = bytes;

	if(time_spent)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Complete bulk transfer
int SqlStdApi::CloseBulkTransfer()
{
	return 0;
}

// Specifies whether API allows to parallel reading from this API and write to another API
bool SqlStdApi::CanParallelReadWrite()
{
	return true;
}

// Drop the table
int SqlStdApi::DropTable(const char* /*table*/, size_t * /*time_spent*/, std::string & /*drop_stmt*/)
{
	return -1;
}

// Remove foreign key constraints referencing to the parent table
int SqlStdApi::DropReferences(const char* /*table*/, size_t * /*time_spent*/)
{
	return -1;
}

// Get the length of LOB column in the open cursor
int SqlStdApi::GetLobLength(long /*row*/, long /*column*/, long * /*length*/)
{
	return -1;
}

// Get LOB content
int SqlStdApi::GetLobContent(long /*row*/, long /*column*/, void * /*data*/, long /*length*/, long * /*len_ind*/)
{
	return -1;
}

// Get partial LOB content
int SqlStdApi::GetLobPart(long /*row*/, long /*column*/, void* /*data*/, long /*length*/, long* /*len_ind*/)
{
	return -1;
}

// Get the list of available tables
int SqlStdApi::GetAvailableTables(std::string & /*table_template*/, std::string & /*exclude*/, 
									std::list<std::string> & /*tables*/)
{
	return -1;
}

// Read schema information
int SqlStdApi::ReadSchema(const char * /*select*/, const char * /*exclude*/, bool /*read_cns*/, bool /*read_idx*/)
{
	return -1;
}

// Get table name by constraint name
int SqlStdApi::ReadConstraintTable(const char * /*schema*/, const char * /*constraint*/, std::string & /*table*/)
{
	return -1;
}

// Read information about constraint columns
int SqlStdApi::ReadConstraintColumns(const char * /*schema*/, const char * /*table*/, const char * /*constraint*/, std::string & /*cols*/)
{
	return -1;
}

// Build a condition to select objects from the catalog
void SqlStdApi::GetCatalogSelectionCriteria(std::string & /*selection_template*/, std::string & /*output*/)
{
}

