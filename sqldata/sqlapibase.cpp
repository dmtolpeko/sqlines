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

// SqlApiBase Base API

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <stdio.h>
#include <algorithm>
#include "sqlapibase.h"
#include "str.h"

bool SqlApiBase::_static_init = false;

const char *g_spec_symbols = " _\"'.,;:(){}[]=+-*<>!$~|~`@#%^&/\\\n\r\t";

SqlApiBase::SqlApiBase()
{
	_connected = false; 

	_subtype = 0;
	_version_major = 0;
	_version_minor = 0;
	_version_release = 0;

	_cursor_cols_count = 0;
	_cursor_allocated_rows = 0;
	_cursor_cols = NULL;
	_cursor_lob_exists = false;
	_cursor_fetch_lob_as_varchar = false;
	
	_ins_cols_count = 0;
	_ins_allocated_rows = 0;
	_ins_cols = NULL;
	_ins_all_rows_inserted = 0;

	_error = 0;
	_error_text[0] = '\x0';

	_native_error = 0;
	_native_error_text[0] = '\x0';

	_source_api_provider = NULL;
	_source_api_type = 0;
	_source_api_subtype = 0;

	_target_api_provider = NULL;

	_loaded_driver[0] = '\x0';
	_parameters = NULL;
	_log = NULL;

	_batch_max_rows = 0;
    _lob_bind_buffer = 0;
	_char_length_ratio = 0.0;

	_trace = false;
	_trace_data = false;
} 

// Set source database API provider
void SqlApiBase::SetSourceApiProvider(SqlApiBase *api) 
{
	_source_api_provider = api; 

	if(_source_api_provider != NULL)
	{
		_source_api_type = _source_api_provider->GetType();
		_source_api_subtype = _source_api_provider->GetSubType();
	}
}

// Set target database API provider
void SqlApiBase::SetTargetApiProvider(SqlApiBase *api)
{
	_target_api_provider = api;
}

// Clear schema objects
void SqlApiBase::ClearSchema()
{
	_table_columns.clear();
	_table_constraints.clear();
	_table_cons_columns.clear();
	_table_comments.clear();
	_table_indexes.clear();
	_table_ind_columns.clear();
	_table_ind_expressions.clear();
	_sequences.clear();
}

// Check if identifier is a reserved word
bool SqlApiBase::IsReservedWord(const char *name)
{
	if(name == NULL)
		return false;

	bool exists = false;

	// Find a reserved word
	for(std::list<std::string>::iterator i = _reserved_words_ddl.begin(); i != _reserved_words_ddl.end(); i++)
	{
		if(_stricmp((*i).c_str(), name) == 0)
		{
			exists = true;
			break;
		}
	}

	return exists;
}

// Get constraint by name
SqlConstraints* SqlApiBase::GetConstraint(const char *schema, const char *table, const char *name)
{
	SqlConstraints *cns = NULL;

	// Schema and table can be NULL
	if(name == NULL)
		return NULL;

	// Find a constraint
	for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
	{
		// Compare schema if set
		if(schema != NULL)
		{
			if((*i).schema == NULL || strcmp(schema, (*i).schema) != 0)
				continue;
		}

		// Compare table if set
		if(table != NULL)
		{
			if((*i).table == NULL || strcmp(table, (*i).table) != 0)
				continue;
		}

		if((*i).constraint == NULL)
			continue;

		// Compare constraints name
		if(strcmp(name, (*i).constraint) == 0)
		{
			cns = &(*i);
			break;
		}
	}

	return cns;
}

// Map the data type of column
void SqlApiBase::MapDataType(SqlCol *col, std::list<SqlDataTypeMap> *dtmap)
{
    if(col == NULL || dtmap == NULL)
        return;

    for(std::list<SqlDataTypeMap>::iterator i = dtmap->begin(); i != dtmap->end(); i++)
	{
        if(_stricmp(col->_datatype_name.c_str(), (*i).name.c_str()) != 0)
            continue;

        // Check min length/range
        if((*i).len_min != -1 && col->_len < (*i).len_min) 
            continue;

        // Check max length/range
        if((*i).len_max != -1 && col->_len > (*i).len_max) 
            continue;

        // Check min scale
        if((*i).scale_min != -1 && col->_scale < (*i).scale_min)
            continue;

        // Check max scale
        if((*i).scale_max != -1 && col->_scale > (*i).scale_max)
            continue;

        // All conditions met, set the mapping
        col->_t_datatype_name = (*i).t_name;
    }
}

// Split qualified name to schema and object name
void SqlApiBase::SplitQualifiedName(const char *qualified, std::string &schema, std::string &object)
{
	if(qualified == NULL)
		return;

	const char *cur = qualified;

	// Get schema name
	if(strchr(qualified, '.') != NULL)
	{
		while(*cur && *cur != '.')
		{
			schema += *cur;
			cur++;
		}
	}

	if(*cur == '.')
		cur++;

	// Get object name
	while(*cur)
	{
		object += *cur;
		cur++;
	}
}

// Split connection string to user, password and database
void SqlApiBase::SplitConnectionString(const char *conn, std::string &user, std::string &pwd, std::string &db)
{
	if(conn == NULL)
		return;

	// Find @ that separates user/password from server name
	const char *amp = strchr(conn, '@');
	const char *sl = strchr(conn, '/');

	// Set database info
	if(amp != NULL)
	{
		db = amp + 1; 

		const char *cur = amp;

		// @ can be used in password, so find the latest @
		while(cur != NULL)
		{
			cur = strchr(cur + 1, '@');

			if(cur != NULL)
			{
				amp = cur;
				db = amp + 1; 
			}
		}
	}

	if(amp == NULL)
		amp = conn + strlen(conn);

	// Define the end of the user name
	const char *end = (sl == NULL || amp < sl) ? amp : sl;

	user.assign(conn, (size_t)(end - conn));

	// Define password
	if(sl != NULL && amp > sl)
	{
		pwd.assign(sl + 1, (size_t)(amp - sl - 1));
	}
}

void SqlApiBase::SplitConnectionString(const char *conn, std::string &user, std::string &pwd, std::string &server, std::string &db, std::string &port)
{
	if(conn == NULL)
		return;

	std::string db_full;

	SplitConnectionString(conn, user, pwd, db_full);

	const char *start = db_full.c_str();

	// Find : and , that denote the server port and the database name
	const char *semi = strchr(start, ':');
	const char *comma = strchr(start, ',');

	const char *end = (semi != NULL) ? semi :comma;

	// Define server name
	if(end != NULL)
		server.assign(start, (size_t)(end - start));
	else
		server = start;

	// Define port
	if(semi != NULL)
	{
		if(comma != NULL && comma > semi)
			port.assign(semi + 1, (size_t)(comma - semi - 1));
		else
			port = semi + 1;
	}

	if(comma != NULL)
		db = Str::SkipSpaces(comma + 1);
}

// Build a condition to select objects from the catalog and exclude from selection
void SqlApiBase::GetSelectionCriteria(const char *select, const char *exclude, const char *schema, 
					const char *object, std::string &output, const char *default_schema, bool upper_case)
{
	std::string sel, excl;

	// Get a condition to select objects from the catalog
	GetSelectionCriteria(select, schema, object, sel, default_schema, upper_case);

	output += sel;

	// Get a condition to exclude object 
	GetSelectionCriteria(exclude, schema, object, excl, default_schema, upper_case);

	if(excl.empty() == false)
	{
		if(sel.empty() == false)
			output += " AND";

		output += " NOT (";
		output += excl;
		output += ")";
	}
}

// Build a condition to select objects from the catalog
void SqlApiBase::GetSelectionCriteria(const char *selection_template, const char *in_schema, const char *in_object, 
	std::string &output, const char *default_schema, bool upper_case)
{
	if(selection_template == NULL)
		return;

	const char *cur = selection_template;

	std::string conditions;
	int count = 0;

	bool more = true;

	// Handle each item in the list
	while(more)
	{
		std::string item;

		// Get next table template
		cur = Str::GetNextInList(cur, item);

		if(item.empty() == true)
		{
			more = false;
			break;
		}

		const char *table_template = item.c_str();

		// Find schema and table delimiter
		const char *dot = strchr(table_template, '.');

		std::string schema;
		std::string table;
	
		// Define schema and table
		if(dot != NULL)
		{
			schema.assign(table_template, (size_t)(dot - table_template)); 
			table = dot + 1;
		}
		else
		// if schema name is not specified use the default schema
		{
			if(default_schema != NULL)
				schema = default_schema;

			table = table_template;
		}

		char *s_ast = strchr((char*)schema.c_str(), '*');
		char *t_ast = strchr((char*)table.c_str(), '*');

		std::replace(schema.begin(), schema.end(), '*', '%');
		std::replace(table.begin(), table.end(), '*', '%');

		// Convert schema and table to upper case
		if(upper_case == true)
		{
			std::transform(schema.begin(), schema.end(), schema.begin(), ::toupper);
			std::transform(table.begin(), table.end(), table.begin(), ::toupper);
		}

		std::string condition;

		if(schema.empty() == false)
		{
			// Add schema filter, do not add LIKE if all schema are searched (*.t1)
			if(s_ast != NULL && strcmp(schema.c_str(), "%") != 0)
			{
				condition += in_schema;
				condition += " LIKE '";
				condition += schema;
				condition += "'";
			}
			else
			// No template in the schema name
			if(s_ast == NULL)
			{
				condition += in_schema;
				condition += " = '";
				condition += schema;
				condition += "'";
			}
		}

		bool and_exists = false;

		// Add table filter, do not add LIKE if all tables are searched (s1.*)
		if(t_ast && strcmp(table.c_str(), "%") != 0)
		{
			if(condition.empty() == false)
			{
				condition += " AND ";
				and_exists = true;
			}

			condition += in_object;
			condition += " LIKE '";
			condition += table;
			condition += "'";
		}
		else
		// No template in object name
		if(t_ast == NULL)
		{
			if(condition.empty() == false)
			{
				condition += " AND ";
				and_exists = true;
			}

			condition += in_object;
			condition += " = '";
			condition += table;
			condition += "'";
		}

		// Condition will be empty in case of *.*
		if(condition.empty() == false)
		{
			if(conditions.empty() == false)
				conditions += " OR ";

			if(and_exists == true)
			{
				conditions += '(';
				conditions += condition;
				conditions += ')';
			}
			else
				conditions += condition;

			count++;
		}
	}

	// Enclose multiple OR condition with () 
	if(count > 1)
	{
		output = '(';
		output += conditions;
		output += ')';
	}
	else
	if(count == 1)
		output = conditions;
}

// Get a list of columns for specified primary or unique key
int SqlApiBase::GetKeyConstraintColumns(SqlConstraints &cns, std::string &output)
{
	std::list<std::string> columns;

	// Each driver redefines the way to get the list of columns
	int rc = GetKeyConstraintColumns(cns, columns);

	int c = 0;

	for(std::list<std::string>::iterator i = columns.begin(); i != columns.end(); i++)
	{
		if(c > 0)
			output += ", ";

		output += (*i);
		c++;
	}

	return rc;
}

// Get a list of columns for specified index
int SqlApiBase::GetIndexColumns(SqlIndexes &idx, std::string &idx_cols)
{
	std::list<std::string> columns;
	std::list<std::string> sorts;

	// Each driver redefines the way to get the list of columns
	int rc = GetIndexColumns(idx, columns, sorts);

	int c = 0;

	for(std::list<std::string>::iterator i = columns.begin(); i != columns.end(); i++)
	{
		if(c > 0)
			idx_cols += ", ";

		idx_cols += (*i);
		c++;
	}

	return rc;
}

// Get the driver search paths
void SqlApiBase::GetDriverPaths(std::list<std::string> &paths)
{
	for(std::list<std::string>::iterator i = _driver_paths.begin(); i != _driver_paths.end(); i++)
		paths.push_back(*i);
}

// Set parameters
void SqlApiBase::SetParameters(Parameters *p) 
{ 
	_parameters = p; 

	if(_parameters->GetTrue("-trace") != NULL)
		_trace = true;

	if(_parameters->GetTrue("-trace_data") != NULL)
		_trace_data = true;
}

// Initialize session by setting general options
void SqlApiBase::InitSession()
{
   	if(_parameters == NULL)
		return;

    int num = 0;
	const char *value = _parameters->Get("-batch_max_rows");

	if(Str::IsSet(value))
	{
        sscanf(value, "%d", &num);

        if(num > 0)
            _batch_max_rows = (size_t)num;		
	}

	num = 0;
	value = _parameters->Get("-lob_bind_buffer");

	if(Str::IsSet(value))
	{
        sscanf(value, "%d", &num);

        if(num > 0 && num < 1024*1024*100)
            _lob_bind_buffer = num;		
	}

	value = _parameters->Get("-char_length_ratio");

	if(Str::IsSet(value))
	{
		float val = 0.0;
		sscanf(value, "%f", &val);

		if(val != 0.0)
			_char_length_ratio = val;
	}
}

// Check if the database version equal or higher than the specified value
bool SqlApiBase::IsVersionEqualOrHigher(int mj, int mi, int rel)
{
	// If major version is higher it is enough
	if(mj < _version_major)
		return true;

	// If major version is equal and minor is higher it is enough
	if(mj == _version_major && mi < _version_minor)
		return true;

	if(mj == _version_major && mi == _version_minor && rel <= _version_release)
		return true;

	return false;		
}