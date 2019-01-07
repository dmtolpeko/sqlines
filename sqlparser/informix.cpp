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

// Informix clauses

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "listw.h"

// RETURNS or RETURNING in Informix procedure
bool SqlParser::ParseInformixReturns(Token *returns)	
{
	if(returns == NULL)
		return false;

	int num = 0;

	// List of comma-separated data types
	while(true)
	{
		Token *datatype = GetNextToken();

		if(datatype == NULL)
			break;

		_spl_returning_datatypes.Add(datatype);

		// For Oracle it is unknown yet whether data type will be used as OUT parameter and length must be removed
		// or OBJECT and length must be preserved; so parse as columns and preserve sizes
		
		// For other databases parameters and columns both preserve sizes, so it is not important how to parse data type
		ParseDataType(datatype);

		num++;

		// Optional AS name can follow
		Token *as = GetNextWordToken("AS", L"AS", 2);

		if(as != NULL)
		{
			Token *name = GetNextToken();

			// Use it OUT parameters name
			_spl_returning_out_names.Add(name, NULL);

			// Alias will be specified before name at any case, so remove it
			if(_target != SQL_INFORMIX)
				Token::Remove(as, name);
		}

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	// The clause terminates with ; as a statement
	Token *semi = GetNextCharToken(';', L';');

	_spl_returning_end = Nvl(semi, GetLastToken());

	return true;
}

// Convert Informix RETURN statement expressions to OUT variable assignment 
void SqlParser::InformixReturnToAssignment(ListWM &return_list)
{
	ListwmItem *p =  _spl_returning_out_names.GetFirst();

	int cnum = 1;

	// Append parameter name and := before each RETURN expression
	for(ListwmItem *i = return_list.GetFirst(); i != NULL; i = i->next)
	{
		Token *ret = (Token*)i->value;
		Token *comma = (Token*)i->value2;

		Token *param = (p != NULL) ? (Token*)p->value : NULL;

		TokenStr name;
				
		// Define parameter name
		InformixGetReturningName(param, name, "p", L"p", 1, cnum, true);

		PrependNoFormat(ret, &name);
		PrependNoFormat(ret, " := ", L" := ", 4);

		Token::Change(comma, ";", L";", 1);

		if(p != NULL)
			p = p->next;

		cnum++;
	}
}

// Replace RETURNING values with OUT parameters or table function when converting from Informix
void SqlParser::InformixConvertReturning(Token *create, Token *procedure)
{
	// No RETURNING clause
	if(_spl_returns == NULL)
		return;

	// Regular RETURN with single or multiple values returned
	if(_spl_return_with_resume == 0)
	{
		// Single scalar value is returned; use function in Oracle, SQL Server, PostgreSQL
		if(_spl_returning_datatypes.GetCount() == 1 && _target != SQL_INFORMIX)
		{
			ListwItem *d = _spl_returning_datatypes.GetFirst(); 
		
			Token *datatype = (d != NULL) ? (Token*)d->value : NULL;

			// Data type size was left, remove it now
			if(_target == SQL_ORACLE)
			{
				OracleRemoveDataTypeSize(datatype);
				Token::Change(_spl_returns, "RETURN", L"RETURN", 6);
			}
			else
			// RETURNS keyword in SQL Server, PostgreSQL
			if(Target(SQL_SQL_SERVER, SQL_POSTGRESQL) == true)
				Token::Change(_spl_returns, "RETURNS", L"RETURNS", 7);

			// RETURNING can be used in a function as well
			if(_spl_scope == SQL_SCOPE_PROC)
			{
				Token::Change(procedure, "FUNCTION", L"FUNCTION", 8);	
				_spl_proc_to_func = true;
			}

			// Remove optinal terminating ;
			if(Token::Compare(_spl_returning_end, ';', L';') == true)
				Token::Remove(_spl_returning_end);
		}
		// Multiple values are returned
		else
		{
			// Convert to OUT parameters
			if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
			{
				ListwmItem *p =  _spl_returning_out_names.GetFirst();

				int cnum = 1;

				// Append parameter name before each RETURNING data type
				for(ListwItem *i = _spl_returning_datatypes.GetFirst(); i != NULL; i = i->next)
				{
					Token *datatype = (Token*)i->value;
					Token *param = (p != NULL) ? (Token*)p->value : NULL;

					TokenStr name;
				
					// Define parameter name
					InformixGetReturningName(param, name, "p", L"p", 1, cnum, true);

					PrependNoFormat(datatype, &name);
					Prepend(datatype, " OUT ", L" OUT ", 5, create);

					// Data type sizes were left, remove them now
					if(Target(SQL_ORACLE) == true)
						OracleRemoveDataTypeSize(datatype);

					if(p != NULL)
						p = p->next;

					cnum++;
				}

				// Remove RETURNS/RETURNING
				Token::ReplaceWithSpaces(_spl_returns);
			
				if ( _spl_parameters.GetCount() == 0 )
					Token::Remove(_spl_param_close);
				else
					Token::Change(_spl_param_close, ",", L",", 1);
				Token::Change(_spl_returning_end, ")", L")", 1);
			}
		}
	}
	// Convert procedure with RETURN WITH RESUME to a table function
	else
	{
		// Use PIPELINED function in Oracle
		if(_target == SQL_ORACLE)
		{
			TokenStr row(_spl_name);
			TokenStr tab(_spl_name);

			row.Append("_row", L"_row", 4);
			tab.Append("_tab", L"_tab", 4);

			// Define row object
			PrependNoFormat(create, "-- Row and table types for pipelined function", L"-- Row and table types for pipelined function", 45);
			Prepend(create, "\n", L"\n", 1);
			Prepend(create, "CREATE OR REPLACE TYPE ", L"CREATE OR REPLACE TYPE ", 23);
			PrependNoFormat(create, &row);
			Prepend(create, " AS OBJECT (", L" AS OBJECT (", 12);

			// Use RETURNING data types as OBJECT members
			ListwmItem *p =  _spl_returning_out_names.GetFirst();

			Token *start = NULL;

			int cnum = 1;

			// Append column name before each RETURNING data type
			for(ListwItem *i = _spl_returning_datatypes.GetFirst(); i != NULL; i = i->next)
			{
				Token *datatype = (Token*)i->value;
				Token *param = (p != NULL) ? (Token*)p->value : NULL;

				TokenStr name;
				
				// Define OBJECT column name
				InformixGetReturningName(param, name, "c", L"c", 1, cnum, false);

				Token *prepend = PrependNoFormat(datatype, &name);
				PrependNoFormat(datatype, " ", L" ", 1);

				if(cnum == 1)
					start = prepend;

				if(p != NULL)
					p = p->next;

				cnum++;
			}

			// Remove optinal terminating ;
			if(Token::Compare(_spl_returning_end, ';', L';') == true)
				Token::Remove(_spl_returning_end);

			// Move columns and datatype to object definition
			PrependCopy(create, start, _spl_returning_end, false);
			// Now remove data types in RETURNING clause
			Token::Remove(start, _spl_returning_end);

			Prepend(create, ");\n/\n", L");\n/\n", 5);

			// Define table object
			Prepend(create, "CREATE OR REPLACE TYPE ", L"CREATE OR REPLACE TYPE ", 23);
			PrependNoFormat(create, &tab);
			Prepend(create, " AS TABLE OF ", L" AS TABLE OF ", 13);
			PrependNoFormat(create, &row);
			Prepend(create, ";\n/\n\n", L";\n/\n\n", 5);

			Token::Change(procedure, "FUNCTION", L"FUNCTION", 8);

			// Convert RETURNS/RETURNING to RETURN and add PIPELINED clause
			Token::Change(_spl_returns, "RETURN", L"RETURN", 6);
			AppendNoFormat(_spl_returns, " ", L" ", 1);
			AppendNoFormat(_spl_returns, &tab);
			Append(_spl_returns, " PIPELINED", L" PIPELINED", 10);	

			_spl_proc_to_func = true;
		}
		else
		// Use table-valued function in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			TokenStr tab("@", L"@", 1);
			tab.Append(_spl_name);
			tab.Append("_tab", L"_tab", 4);

			// Use RETURNING data types as columns
			ListwmItem *p =  _spl_returning_out_names.GetFirst();

			Token *start = NULL;

			int cnum = 1;

			// Append column name before each RETURNING data type
			for(ListwItem *i = _spl_returning_datatypes.GetFirst(); i != NULL; i = i->next)
			{
				Token *datatype = (Token*)i->value;
				Token *param = (p != NULL) ? (Token*)p->value : NULL;

				TokenStr name;
				
				// Define column name
				InformixGetReturningName(param, name, "c", L"c", 1, cnum, false);

				Token *prepend = PrependNoFormat(datatype, &name);
				PrependNoFormat(datatype, " ", L" ", 1);

				if(cnum == 1)
					start = prepend;

				if(p != NULL)
					p = p->next;

				cnum++;
			}

			// Remove optinal terminating ;
			if(Token::Compare(_spl_returning_end, ';', L';') == true)
				Token::Remove(_spl_returning_end);

			Token::Change(procedure, "FUNCTION", L"FUNCTION", 8);

			// Convert RETURNING to RETURNS and add TABLE clause
			Token::Change(_spl_returns, "RETURNS", L"RETURNS", 7);

			AppendNoFormat(_spl_returns, " ", L" ", 1);
			AppendNoFormat(_spl_returns, &tab);
			
			Append(_spl_returns, " TABLE (", L" TABLE (", 8);	
			AppendFirstNoFormat(_spl_returning_end, ")", L")", 1);

			_spl_proc_to_func = true;
		}
	}
}

// Define the name of parameter or column to be used for RETURNING value
void SqlParser::InformixGetReturningName(Token *retvalue, TokenStr &name, const char *prefix, 
											const wchar_t *wprefix, int64_t plen, int num, bool always_prefix)
{
	if(retvalue == NULL)
		return;

	// String literal in return value
	if(retvalue->type == TOKEN_STRING)
	{
		// Use prefix and order number instead of literal
		name.Append(prefix, wprefix, plen);
		name.Append(num);
	}
	else
	{
		if(always_prefix == true)
			name.Append(prefix, wprefix, plen);

		name.Append(retvalue);
	}
}

// Informix INTO TEMP | SCRATCH | RAW table clause of SELECT statement
bool SqlParser::ParseInformixIntoTable(Token *select)
{
	if(select == NULL)
		return false;

	Token *into = GetNextWordToken("INTO", L"INTO", 4);

	if(into == NULL)
		return false;

	// TEMP keyword to create and insert into a temporary table
	Token *temp = GetNextWordToken("TEMP", L"TEMP", 4);
	Token *scratch = NULL;
	Token *raw = NULL;

	// SCRATCH is a non-logged temporary table in Informix XPS
	if(temp == NULL)
		scratch = GetNextWordToken("SCRATCH", L"SCRATCH", 7);

	// RAW is a non-logged permanent table
	if(temp == NULL && scratch == NULL)
		raw = GetNextWordToken("RAW", L"RAW", 3);

	Token *table_name = GetNextToken();

	if(table_name == NULL)
		return false;

	// Storage clause can follow after table name 
	ParseInformixStorageClause();

	// Use INSERT INTO table_name SELECT in Oracle
	if(_target == SQL_ORACLE)
	{
		// Temporary table
		if(temp != NULL || scratch != NULL)
		{
			// If inside a procedural block, use insert as temporary table is created outside the procedure
			if(_spl_scope != 0)
			{
				Prepend(select, "INSERT INTO ", L"INSERT INTO ", 12);
				PrependCopy(select, table_name);
				Prepend(select, "\n", L"\n", 1);
			}
			else
			{
				Prepend(select, "CREATE GLOBAL TEMPORARY TABLE ", L"CREATE GLOBAL TEMPORARY TABLE ", 30);
				PrependCopy(select, table_name);

				// PRESERVE ROWS required so data not deleted right after copy
				Prepend(select, " ON COMMIT PRESERVE ROWS AS\n", L" ON COMMIT PRESERVE ROWS AS\n", 28);
			}

			_spl_created_session_tables.Add(table_name);
		}
		else
		// Permanent non-logged table
		if(raw != NULL)
		{
			Prepend(select, "CREATE TABLE ", L"CREATE TABLE ", 13);
			PrependCopy(select, table_name);
			Prepend(select, " AS\n", L" AS\n", 4);
		}

		Token::Remove(into, table_name);
	}

	return true;
}

// Check for pattern LET var = DBINFO('sqlca.sqlerrd1') that gets the last SERIAL value
bool SqlParser::InformixPatternAssignLastSerial(Token *let)
{
	if(let == NULL || _target != SQL_ORACLE)
		return false;

	// Variable name
	Token *var = GetNextToken();

	if(var == NULL)
		return false;

	Token *equal = GetNextCharToken('=', L'=');

	Token *dbinfo = GetNextWordToken(equal, "DBINFO", L"DBINFO", 6);

	// Not DBINFO function
	if(dbinfo == NULL)
	{
		PushBack(var);
		return false;
	}

	Token *open = GetNextCharToken(dbinfo, '(', L'(');

	// Check DBINFO property
	Token *sqlerrd1 = GetNextWordToken(open, "\"sqlca.sqlerrd1\"", L"\"sqlca.sqlerrd1\"", 16);

	if(sqlerrd1 == NULL)
		sqlerrd1 = GetNextWordToken(open, "'sqlca.sqlerrd1'", L"'sqlca.sqlerrd1'", 16);

	Token *close = GetNextCharToken(sqlerrd1, ')', L')');
	
	// Not DBINFO('sqlca.sqlerrd1') function
	if(sqlerrd1 == NULL)
	{
		PushBack(var);
		return false;
	}

	// SELECT table_name_seq.CURRVAL INTO var FROM dual in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(let, "SELECT ", L"SELECT ", 7);

		// Use the sequence for the table from last INSERT statement
		AppendCopy(let, _spl_last_insert_table_name);
		AppendNoFormat(let, "_seq.currval ", L"_seq.currval ", 13);
		Append(let, "INTO", L"INTO", 4);

		Append(var, " FROM ", L" FROM ", 6, let);
		AppendNoFormat(var, "dual", L"dual", 4);

		Token::Remove(equal, close);
	}

	return true;
}

// Various SET options such as SET ISOLATION ...
bool SqlParser::ParseInformixSetOptions(Token *set)
{
	if(set == NULL)
		return false;

	bool exists = false;
	bool comment = false;

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	// SET ISOLATION TO DIRTY READ
	if(next->Compare("ISOLATION", L"ISOLATION", 9) == true)
	{
		Token *to = GetNextWordToken("TO", L"TO", 2);
		Token *dirty = GetNextWordToken(to, "DIRTY", L"DIRTY", 5);
		Token *read = NULL;

		if(dirty != NULL)
			read = GetNextWordToken("READ", L"READ", 4);

		// Comment for Oracle
		if(read != NULL && _target == SQL_ORACLE)
			comment = true;

		exists = true;
	}
	else
	// SET EXPLAIN FILE TO 'file'
	if(next->Compare("EXPLAIN", L"EXPLAIN", 7) == true)
	{
		Token *file = GetNextWordToken("FILE", L"FILE", 4);
		Token *to = GetNextWordToken(file, "TO", L"TO", 2);

		// File name
		Token *name = GetNextToken(to);

		// Optional WITH APPEND
		Token *with = GetNextWordToken(name, "WITH", L"WITH", 4);
		/*Token *append */ (void) GetNextWordToken(with, "APPEND", L"APPEND", 6);

		// Comment for Oracle
		if(name != NULL && _target == SQL_ORACLE)
			comment = true;

		exists = true;
	}

	// Check if the statement needs to be commented
	if(exists == true && comment == true)
	{
		Token *semi = GetNextCharToken(';', L';');
		Comment(set, Nvl(semi, GetLastToken()));
	}

	return exists;
}

// UPDATE STATISTICS statement in Informix
bool SqlParser::ParseInformixUpdateStatistics(Token *update)
{
	if(update == NULL)
		return false;

	Token *statistics = GetNextWordToken("STATISTICS", L"STATISTICS", 10);

	if(statistics == NULL)
		return false;

	// LOW, MEDIUM or HIGH mode
	Token *low = GetNextWordToken("LOW", L"LOW", 3);
	Token *medium = NULL;
	Token *high = NULL;

	if(low == NULL)
	{
		medium = GetNextWordToken("MEDIUM", L"MEDIUM", 6);

		if(medium == NULL)
			high = GetNextWordToken("HIGH", L"HIGH", 4);
	}

	// FOR TABLE keywords
	Token *for_ = GetNextWordToken("FOR", L"FOR", 3);
	Token *table = GetNextWordToken(for_, "TABLE", L"TABLE", 5);

	// Table name
	Token *table_name = GetNextToken(table);

	if(table_name == NULL)
		return false;

	// Optional RESOLUTION percent confidence
	Token *resolution = GetNextWordToken("RESOLUTION", L"RESOLUTION", 10);

	if(resolution != NULL)
	{
		Token *percent = GetNextNumberToken();
		/*Token *confidence */ (void) GetNextNumberToken(percent);
	}

	// Comment for Oracle
	if(_target == SQL_ORACLE)
		Comment(update, Nvl(GetNextCharToken(';', L';'), GetLastToken()));

	return true;
}

// MATCHES predicate in Informix
bool SqlParser::ParseInformixMatchesPredicate(Token *pre_first, Token *first, Token *not_, Token *matches)
{
	if(first == NULL || matches == NULL)
		return false;

	Token *pattern = GetNextToken();

	if(pattern == NULL)
		return false;

	// Pattern can be an expression '*' || var || '*' i.e. 
	ParseExpression(pattern);

	Token *pattern_end = GetLastToken();

	// Use REGEXP_LIKE in Oracle
	if(_target == SQL_ORACLE)
	{
		// NOT must be specified before REGEXP_LIKE
		if(not_ != NULL)
		{
			Append(pre_first, " NOT", L" NOT", 4, not_);
			Token::Remove(not_);
		}

		Append(pre_first, " REGEXP_LIKE(", L" REGEXP_LIKE(", 13, matches);
		Token::Change(matches, ",", L",", 1);
		AppendNoFormat(pattern_end, ")", L")", 1);
	}

	return true;
}

// FRAGMENT BY partitioning clause
bool SqlParser::ParseInformixFragmentBy(Token *fragment)
{
	if(fragment == NULL)
		return false;

	Token *by = GetNextWordToken("BY", L"BY", 2);

	if(by == NULL)
		return false;

	// Partitioning by HASH in XPS
	/*Token *hash */ (void) GetNextWordToken("HASH", L"HASH", 4);

	/*Token *open */ (void) GetNextCharToken('(', L'(');

	// List of columns
	while(true)
	{
		Token *col = GetNextToken();

		if(col == NULL)
			break;

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
		Token::Change(fragment, "PARTITION", L"PARTITION", 9);

	return true;
}
