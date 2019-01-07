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

// Oracle clauses

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "listw.h"

// Oracle STORAGE clause in CREATE TABLE 
bool SqlParser::ParseOracleStorageClause(Token *storage)
{
	if(storage == NULL)
		return false;

	Token *open = GetNextCharToken('(', L'(');

	if(open == NULL)
		return false;

	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// INITIAL num
		if(next->Compare("INITIAL", L"INITIAL", 7) == true)
		{
			Token *value = GetNextToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// NEXT num
		if(next->Compare("NEXT", L"NEXT", 4) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// MINEXTENTS num
		if(next->Compare("MINEXTENTS", L"MINEXTENTS", 10) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// MAXEXTENTS num | UNLIMITED
		if(next->Compare("MAXEXTENTS", L"MAXEXTENTS", 10) == true)
		{
			Token *value = GetNextToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// PCTINCREASE num
		if(next->Compare("PCTINCREASE", L"PCTINCREASE", 11) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// FREELISTS num
		if(next->Compare("FREELISTS", L"FREELISTS", 9) == true)
		{
			Token *num = GetNextToken();

			if(_target != SQL_ORACLE && num != NULL)
				Token::Remove(next, num);

			exists = true;
			continue;
		}
		else
		// FREELIST GROUPS num
		if(next->Compare("FREELIST", L"FREELIST", 8) == true)
		{
			/*Token *groups */ (void) GetNextWordToken("GROUPS", L"GROUPS", 6);
			Token *num = GetNextToken();

			if(_target != SQL_ORACLE && num != NULL)
				Token::Remove(next, num);

			exists = true;
			continue;
		}
		else
		// BUFFER_POOL name 
		if(next->Compare("BUFFER_POOL", L"BUFFER_POOL", 11) == true)
		{
			Token *value = GetNextToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// FLASH_CACHE name 
		if(next->Compare("FLASH_CACHE", L"FLASH_CACHE", 11) == true)
		{
			Token *value = GetNextToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// CELL_FLASH_CACHE name 
		if(next->Compare("CELL_FLASH_CACHE", L"CELL_FLASH_CACHE", 16) == true)
		{
			Token *value = GetNextToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}

		// Not an Oracle storage clause
		PushBack(next);
		break;
	}

	Token *close = GetNextCharToken(')', L')');

	// If target is not Oracle, remove STORAGE ()
	if(_target != SQL_ORACLE)
	{
		Token::Remove(storage);
		Token::Remove(open);
		Token::Remove(close);
	}

	return exists;
}

// Oracle LOB (column) STORE AS (params) 
bool SqlParser::ParseOracleLobStorageClause(Token *lob)
{
	if(lob == NULL)
		return false;

	Token *open = GetNextCharToken('(', L'(');

	if(open == NULL)
		return false;

	// LOB column
	/*Token *col */ (void) GetNextIdentToken();

	/*Token *close*/ (void) GetNextCharToken(')', L')');

	/*Token *store*/ (void) GetNextWordToken("STORE", L"STORE", 5);
	Token *as = GetNextWordToken("AS", L"AS", 2);

	// Optional BASICFILE or SECUREFILE
	Token *type = GetNextWordToken("BASICFILE", L"BASICFILE", 9);

	if(type == NULL)
		type = GetNextWordToken("SECUREFILE", L"SECUREFILE", 10);

	// Optional LOB segment name
	Token *segment_name = GetNextIdentToken();

	// Remove if the target is not Oracle
	if(_target != SQL_ORACLE)
	{
		Token::Remove(lob, as);
		Token::Remove(type);
		Token::Remove(segment_name);
	}

	// Start with open (
	Token *open2 = GetNextCharToken('(', L'(');

	if(open2 == NULL)
		return false;

	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// TABLESPACE name
		if(next->Compare("TABLESPACE", L"TABLESPACE", 10) == true)
		{
			Token *name = GetNextToken();

			if(_target != SQL_ORACLE && name != NULL)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// DISABLE | ENABLE STORAGE IN ROW
		if(next->Compare("DISABLE", L"DISABLE", 7) == true || next->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			/*Token *storage*/ (void) GetNextWordToken("STORAGE", L"STORAGE", 7);
			/*Token *in*/ (void) GetNextWordToken("IN", L"IN", 2);
			Token *row = GetNextWordToken("ROW", L"ROW", 3);

			if(_target != SQL_ORACLE && row != NULL)
				Token::Remove(next, row);

			exists = true;
			continue;
		}
		else
		// CHUNK num
		if(next->Compare("CHUNK", L"CHUNK", 5) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// PCTVERSION for LOB storage 
		if(next->Compare("PCTVERSION", L"PCTVERSION", 10) == true)
		{
			Token *num = GetNextNumberToken();

			if(num != NULL)
			{
				// Remove if not Oracle
				if(_target != SQL_ORACLE)
					Token::Remove(next, num);

				exists = true;
				continue;
			}
		}
		else
		// LOGGING 
		if(next->Compare("LOGGING", L"LOGGING", 7) == true)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOCACHE 
		if(next->Compare("NOCACHE", L"NOCACHE", 7) == true)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// STORAGE () clause 
		if(next->Compare("STORAGE", L"STORAGE", 7) == true)
		{
			exists = ParseOracleStorageClause(next);
			continue;
		}

		// Not an Oracle storage clause
		PushBack(next);
		break;
	}

	Token *close2 = GetNextCharToken(')', L')');

	// If target is not Oracle, remove ()
	if(_target != SQL_ORACLE)
	{
		Token::Remove(open2);
		Token::Remove(close2);
	}

	return exists;
}

// Oracle partitions definitions
bool SqlParser::ParseOraclePartitions(Token *token)
{
	if(token == NULL)
		return false;

	// LOCAL or GLOBAL can be specified for index partition
	Token *local = token->Compare("LOCAL", L"LOCAL", 5) ? token : NULL;
	Token *global = NULL;
	
	if(local == NULL && token->Compare("GLOBAL", L"GLOBAL", 6))
		global = token;

	if(local != NULL || global != NULL)
	{
		// Remove if the target is not Oracle
		if(_target != SQL_ORACLE)
			Token::Remove(token);

		if(global != NULL)
			token = GetNextToken();
	}

	Token *subpartition = NULL;

	// PARTITION BY is not specified for LOCAL partitioned indexes
	if(local == NULL)
	{
		// PARTITION BY clause
		if(ParseOraclePartitionsBy(token) == false)
			return false;

		subpartition = GetNextWordToken("SUBPARTITION", L"SUBPARTITION", 12);

		// SUBPARTITION clause can follow
		if(subpartition != NULL)
		{
			ParseOraclePartitionsBy(subpartition);

			// SUBPARTITION TEMPLATE
			Token *subpartition2 = GetNextWordToken("SUBPARTITION", L"SUBPARTITION", 12);
			Token *template_ = NULL;

			if(subpartition2 != NULL)
			{
				template_ = GetNextWordToken("TEMPLATE", L"TEMPLATE", 8);

				// Subpartition template definition
				ParseOraclePartition(NULL, subpartition2);
			}
		}

		// PARTITION definition including subpartitions
		ParseOraclePartition(token, subpartition);
	}
	else
	{
		// A dummy tokens to parse PARTITION and SUBPARTITION definitions for index
		Token part, subpart;

		// Greenplum does not support partitions in indexes, so set removed flags to remove definitions
		if(_target == SQL_GREENPLUM)
		{
			Token::Remove(&part);
			Token::Remove(&subpart);
		}

		// PARTITION definition including subpartitions for LOCAL index
		ParseOraclePartition(&part, &subpart);
	}

	return true;
}

// Oracle partition or subpartition type and columns 
bool SqlParser::ParseOraclePartitionsBy(Token *token)
{
	if(token == NULL)
		return false;
		
	Token *partition = token->Compare("PARTITION", L"PARTITION", 9) ? token : NULL;
	Token *subpartition = NULL;

	if(partition == NULL)
		subpartition = token->Compare("SUBPARTITION", L"SUBPARTITION", 12) ? token : NULL;
	
	if(partition == NULL && subpartition == NULL)
		return false;

	/*Token *by */ (void) GetNextWordToken("BY", L"BY", 2);

	// RANGE, HASH or LIST
	Token *range = GetNextWordToken("RANGE", L"RANGE", 5);
	Token *hash = NULL;
	Token *list = NULL;
	
	if(range == NULL)
		hash = GetNextWordToken("HASH", L"HASH", 4);

	if(range == NULL && hash == NULL)
		list = GetNextWordToken("LIST", L"LIST", 4);

	Token *open = GetNextCharToken('(', L'(');

	if(open == NULL)
		return false;

	// Multiple columns can be specified
	while(true)
	{
		Token *column = GetNextIdentToken();

		if(column == NULL)
			break;

		Token *comma = GetNextCharToken(',', ',');

		if(comma == NULL)
			break;
	}

	Token *close = GetNextCharToken(')', L')');

	// SUBPARTITIONS num
	if(subpartition != NULL)
	{
		Token *subpartitions = GetNextWordToken("SUBPARTITIONS", L"SUBPARTITIONS", 13);
		Token *num = NULL;

		if(subpartitions != NULL)
		{
			num = GetNextToken();

			if(_target != SQL_ORACLE)
				Token::Remove(subpartitions, num);
		}
	}

	if(_target == SQL_GREENPLUM)
	{
		// Greenplum does not support partitions on indexs and hash partitions
		if(_obj_scope == SQL_SCOPE_INDEX || hash != NULL)
			Token::Remove(token, close);
	}

	return true;
}

// Oracle PARTITION or SUBPARTITION definition clause 
bool SqlParser::ParseOraclePartition(Token *partition, Token *subpartition)
{
	// Both partition and subpartition parameters can be NULL for LOCAL partitioned indexes
	Token *open = GetNextCharToken('(', L'(');

	if(open == NULL)
		return false;

	bool exists = false;

	// List of partition/subpartition definitions can be specified
	while(true)
	{
		Token *type = NULL;

		if(partition != NULL)
			type = GetNextWordToken("PARTITION", L"PARTITION", 9);
		else
			type = GetNextWordToken("SUBPARTITION", L"SUBPARTITION", 12);

		if(type == NULL)
			break;

		Token *name = GetNextIdentToken();

		if(name == NULL)
			break;

		exists = true;

		// VALUES clause (optional for example for index)
		Token *values = GetNextWordToken("VALUES", L"VALUES", 6);

		if(values != NULL)
		{
			// LESS THAN
			Token *less = GetNextWordToken("LESS", L"LESS", 4);
			Token *than = GetNextWordToken("THAN", L"THAN", 4);

			/*Token *open_range */ (void) GetNextCharToken('(', L'(');
			Token *exp = GetNextToken();

			// Check for MAXVALUE
			if(Token::Compare(exp, "MAXVALUE", L"MAXVALUE", 8))
			{
				if(_target == SQL_GREENPLUM)
					Token::Change(exp, "DATE '9999-01-01'", L"DATE '9999-01-01'", 17);
			}
			// Expression to define the partition range
			else				
				ParseExpression(exp);

			Token *close_range = GetNextCharToken(')', L')');

			if(close_range == NULL)
				break;

			// In Greenplum use END (exp) EXCLUSIVE
			if(_target == SQL_GREENPLUM)
			{
				Token::Change(values, "END", L"END", 3);
				Token::Remove(less, than);
				Append(close_range, " EXCLUSIVE", L" EXCLUSIVE", 10, values);
			}
		}

		// Storage for the partition 
		ParseOracleStorageClause();

		// Now parse SUBPARTITION definitions for the current PARTITION recursively
		if(partition != NULL && subpartition != NULL)
			ParseOraclePartition(NULL, subpartition);

		// Partition definitions are comma separated
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	Token *close = GetNextCharToken(')', L')');

	// PARTITION BY was removed, so remove partition definitions as well
	if(partition != NULL && partition->IsRemoved())
		Token::Remove(open, close);
	else
	// SUBPARTITION BY was removed
	if(partition == NULL && subpartition != NULL && subpartition->IsRemoved())
		Token::Remove(open, close);

	return exists;
}

// DBMS_OUTPUT package
bool SqlParser::ParseFunctionDbmsOutput(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	// DBMS_OUTPUT.function
	if(name->Compare('.', L'.', 11) == false)
		return false;

	// DBMS_OUTPUT.PUT_LINE(text)
	if(name->Compare("PUT_LINE", L"PUT_LINE", 12, 8) == true)
	{
		Token *exp = GetNextToken();

		ParseExpression(exp);

		Token *close = GetNextCharToken(')', L')');
		
		// PRINT in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "PRINT", L"PRINT", 5);
			Token::Change(open, " ", L" ", 1);
			Token::Remove(close);
		}
		else
		// PUT_LINE user-defined function (not built-in) in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			Token::Change(name, "PUT_LINE", L"PUT_LINE", 8);
		}
		else
		// RAISE NOTICE '%', text in PostgreSQL and Netezza
		if(Target(SQL_POSTGRESQL, SQL_NETEZZA) == true)
		{
			Token::Change(name, "RAISE NOTICE '%',", L"RAISE NOTICE '%',", 17);

			Token::Remove(open);
			Token::Remove(close);

			name->t_type = TOKEN_STATEMENT;
		}
	}
    else
    // DBMS_OUTPUT.ENABLE(buffer => NULL)
	if(name->Compare("ENABLE", L"ENABLE", 12, 6) == true)
	{
		Token *exp = GetNextToken();

		ParseExpression(exp);

        Token *close = GetNext(')', L')');
        Token *semi = GetNext(';', L';');

        if(close != NULL)
            Token::Remove(name, Nvl(semi, close));
    }
	
	return true;
}

// Oracle and PostgreSQL variable declaration block
bool SqlParser::ParseOracleVariableDeclarationBlock(Token *declare)
{
	bool exists = false;

	// declare points to AS keyword for outer procedural block

	// Remove DECLARE for SQL Server, MySQL as it will be specified before each variable
	if(Token::Compare(declare, "DECLARE", L"DECLARE", 7) == true)
	{
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
			Token::Remove(declare);
	}
	else
	// PostgreSQL, Netezza required DECLARE keyword before outer declaration section after AS keyword
	if(_source == SQL_ORACLE && Target(SQL_POSTGRESQL, SQL_NETEZZA) == true)
	{
		Append(declare, "\nDECLARE", L"\nDECLARE", 8);

		// Add parameter aliases for each parameter in Netezza
		if(_target == SQL_NETEZZA)
		{
			int i = 1;

			for(ListwItem *p = _spl_parameters.GetFirst(); p != NULL; p = p->next)
			{
				Append(declare, "\n", L"\n", 1);
				AppendCopy(declare, (Token*)p->value);
				Append(declare, " ALIAS FOR $", L" ALIAS FOR $", 12);
				Append(declare, i);
				Append(declare, ";", L";", 1);
				i++;
			}
		}
	}

    // Last token of last declared variable (not cursor) in the current PL/SQL DECLARE block
    Token *last_declare_var = NULL;

    // Cursor definitions
	ListWM cursors;

	// Process declaration statements until BEGIN
	while(true)
	{
		// Variable name
		Token *name = GetNextIdentToken();

		if(name == NULL)
			break;

		// Exit if BEGIN matched
		if(name->Compare("BEGIN", L"BEGIN", 5) == true)
		{
			PushBack(name);
			break;
		}

		// CURSOR cur IS SELECT definition
		if(ParseOracleCursorDeclaration(name, &cursors) == true)
		{
			exists = true;
			continue;
		}
		
		// Add @ for parameter names for SQL Server and Sybase
		if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
			ConvertToTsqlVariable(name);

		_spl_variables.Add(name);
		exists = true;

		// Add DECLARE before name in SQL Server, Sybase and MySQL
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL, SQL_SYBASE) == true)
			Prepend(name, "DECLARE ", L"DECLARE ", 8, declare);

		Token *data_type = GetNextToken();

		// Check and resolve Oracle %TYPE and %ROWTYPE variable
		bool typed = ParseTypedVariable(name, data_type);

		// Get the parameter data type
		if(typed == false)
			ParseDataType(data_type, SQL_SCOPE_VAR_DECL);

		// Optional initialization value := value in Oracle and = value in PostgreSQL
		Token *colon = GetNextCharToken(':', L':');
		Token *equal = GetNextCharToken('=', L'=');
		Token *default_ = NULL;

		// DEFAULT keyword can be also specified
		if(equal == NULL)
			default_ = GetNext("DEFAULT", L"DEFAULT", 7);

		if(colon != NULL || equal != NULL || default_ != NULL)
		{
			Token *exp = GetNextToken();

			// Parse initialization expression
			ParseExpression(exp);

			// For SQL Server and PostgreSQL, delete colon
			if(colon != NULL && Target(SQL_SQL_SERVER, SQL_POSTGRESQL) == true)
				Token::Remove(colon);
			else
			// For Oracle add :
			if(colon == NULL && _target == SQL_ORACLE)
				Prepend(equal, ":", L":", 1);
			else
			// In MySQL use DEFAULT keyword
			if(Target(SQL_MARIADB, SQL_MYSQL) && equal != NULL)
			{
				Token::Change(equal, " DEFAULT ", L" DEFAULT ", 9);
				Token::Remove(colon);
			}
		}

		// ; after each declaration
		Token *semi = GetNextCharToken(';', L';');

        last_declare_var = Nvl(semi, GetLastToken());

        // Check if we are in the outer BEGIN block
        if(_spl_begin_blocks.GetCount() == 0)
		{
            _spl_last_outer_declare_var = last_declare_var;
			_spl_last_outer_declare_varname = name;
		}

		if(semi == NULL)
			break;
	}

	// Generate variables for cursor parameters 
	if(exists == true && Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true && _spl_cursor_params.GetCount() > 0)
	{
		for(ListwmItem *i = _spl_cursor_params.GetFirst(); i != NULL; i = i->next)
		{
			Token *cursor = (Token*)i->value;
			Token *name = (Token*)i->value2;
			Token *param = (Token*)i->value3;
			Token *data_type = (Token*)i->value4;

			Prepend(cursor, "DECLARE ", L"DECLARE ", 8);

			// The target value of parameter already contains variable name
			PrependCopy(cursor, param);
			Prepend(cursor, " ", L" ", 1);
			
			Token *last_added = PrependCopy(cursor, data_type);

			// The lengths for some cursor variables can be set later
			_spl_cursor_vars.Add(name, param, last_added);

			if(i->next != NULL)
				Prepend(cursor, ";\n", L";\n", 2);
			else
				Prepend(cursor, ";\n\n", L";\n\n", 3);
		}
	}

    // Cursors must be after all variables in MySQL and MariaDB, so move them if required
	if(exists == true && last_declare_var != NULL && Target(SQL_MARIADB, SQL_MYSQL) == true && 
        cursors.GetCount() > 0)
	{
        for(ListwmItem *i = cursors.GetFirst(); i != NULL; i = i->next)
		{
            Token *cursor = (Token*)i->value;
			Token *semi = (Token*)i->value2;

            // Cursor goes before the last variable
            if(cursor->remain_size > last_declare_var->remain_size)
            {
                AppendNewlineCopy(last_declare_var, cursor, semi, 2, false);
                Token::Remove(cursor, semi);
            }
        }
    }

	_declare_format = declare;
	_spl_last_declare = GetLastToken();

	return exists;
}

// CURSOR cur(params) IS SELECT definition
bool SqlParser::ParseOracleCursorDeclaration(Token *cursor, ListWM *cursors)
{
	if(cursor == NULL)
		return false;

	if(cursor->Compare("CURSOR", L"CURSOR", 6) == false)
		return false;

	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	_spl_declared_cursors.Add(name);
	_spl_current_declaring_cursor = name;

	// Optional cursor parameters
	Token *open = GetNextCharToken('(', L'(');

	bool params_exist = false;

	if(open != NULL)
	{
		// Cursor parameters are used as local variable in the following SELECT
		EnterLocalVariablesBlock();

		params_exist = true;

		while(true)
		{
			// Parameter name
			Token *param = GetNextIdentToken();

			if(param == NULL)
				break;

			// Optional IN keyword
			/*Token *in */ (void) GetNextWordToken("IN", L"IN", 2);

			Token *data_type = GetNextToken();

			ParseDataType(data_type, SQL_SCOPE_CURSOR_PARAMS);

			// SQL Server does support parameters, and cursor get all variable values during declaration,
			// not during OPEN cursor, so DECLARE is moved before OPEN 
			// MySQL also does not support cursor parameters

			// Generate variable not substitute with actual values (especially if they are constants)
			// to increase chance that SQL execution plan is re-used

			// Define variable name to use instead of parameter 
			if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
			{
				TokenStr str;

				if(_target == SQL_SQL_SERVER)
					str.Append("@", L"@", 1);

				str.Append(name);
				str.Append("_", L"_", 1);
				str.Append(param);
				
				// Save variable name
				Token::ChangeNoFormat(param, str);

				Token::Remove(param, data_type);
			}

			// Save information about cursor parameters (variables will be generated for MySQL i.e.)
			_spl_cursor_params.Add(cursor, name, param, data_type);

			_spl_variables.Add(param);

			// Next must be comma if list continues
			Token *comma = GetNextCharToken(',', L',');

			if(comma == NULL)
				break;

			if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
				Token::Remove(comma);
		}

		Token *close = GetNextCharToken(')', L')');

		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
		{
			Token::Remove(open);
			Token::Remove(close);
		}
	}

	Token *is = GetNextWordToken("IS", L"IS", 2);

	// FOR in SQL Server, MySQL
	if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
		Token::Change(is, "FOR", L"FOR", 3);

	// For SQL Server, MySQL change to DECLARE name CURSOR
	if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
	{
		Token::Change(cursor, "DECLARE", L"DECLARE", 7);
		Append(name, " CURSOR", L"CURSOR", 7, cursor); 

		// For SQL Server add LOCAL since cursor are global by default and visible in other scopes
		if(_target == SQL_SQL_SERVER)
			Append(name, " LOCAL", L" LOCAL", 6, cursor);
	}

	Token *select = GetNextWordToken("SELECT", L"SELECT", 6);

	if(select != NULL)
	{
		ParseSelectStatement(select, 0, SQL_SEL_CURSOR, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

		_spl_declared_cursors_select.Add(name, select, GetLastToken());
	}

	// ; after each declaration
	Token *semi = GetNextCharToken(';', L';');

    // Save cursor definitions for the current declaration block
    if(cursors != NULL)
        cursors->Add(cursor, GetLastToken(semi));

	// Netezza does not support cursors, they are converted to records and loops
	if(_target == SQL_NETEZZA)
	{
		// SELECT statement will be moved to OPEN and removed later
		Token::Change(is, "RECORD;", L"RECORD;", 7);
		Token::Remove(cursor);
		Token::Remove(semi);
	}

	// End of scope for cursor parameters
	if(params_exist == true)
		LeaveLocalVariablesBlock();

	_spl_current_declaring_cursor = NULL;

	return true;
}

// Define actual sizes for parameters, cursor parameters and records
void SqlParser::OracleAppendDataTypeSizes()
{
	// Variables generated for cursor parameters
	if(_spl_cursor_vars.GetCount() > 0)
	{
		ListwmItem *item = _spl_cursor_vars.GetFirst();

		while(item != NULL)
		{
			Token *data_type = (Token*)item->value3;

			// Define the size based on the data type
			OracleAppendDataTypeSize(data_type);

			item = item->next;
		}
	}
}

// Append size for the specified data type token
void SqlParser::OracleAppendDataTypeSize(Token *data_type)
{
	if(data_type == NULL)
		return;

	// VARCHAR2
	if(data_type->Compare("VARCHAR2", L"VARCHAR2", 8) == true)
		AppendFirstNoFormat(data_type, "(4000)", L"(4000)", 6);
}

void SqlParser::OracleRemoveDataTypeSize(Token *data_type)
{
	if(data_type == NULL)
		return;

	Token *cur = Token::SkipSpaces(data_type->next);

	// Datatype without size specifier; target (size) can be added when INT converted to NUMBER(10) i.e.
	if(Token::Compare(cur, '(', L'(') == false && Token::CompareTarget(cur, "(", L"(", 1) == false)
		return;

	// Remove all tokens until ) inclusive
	while(cur != NULL)
	{
		Token::Remove(cur);

		if(Token::Compare(cur, ')', L')') == true || Token::CompareTarget(cur, ")", L")", 1) == true)
			break;

		cur = cur->next;
	}
}

// Oracle 'rownum = <= < num' condition in WHERE clause 
bool SqlParser::ParseOracleRownumCondition(Token *first, Token *op, Token *second, int *rowlimit)
{
	if(first == NULL || op == NULL || second == NULL)
		return false;

	// rownum can be at any side of the expression
	bool rownum1 = first->Compare("rownum", L"rownum", 6);
	bool rownum2 = (rownum1 == false) ? second->Compare("rownum", L"rownum", 6) : false;

	if(rownum1 == false && rownum2 == false)
		return false;

	int limit = -1;

	// Get row limit value
	if(rownum1 == true)
		limit = second->GetInt();
	else
		limit = first->GetInt();

	// If target not Oracle remove condition
	if(_target != SQL_ORACLE)
		Token::Remove(first, second);

	if(rowlimit != NULL)
		*rowlimit = limit;

	return true;
}

// Get Oracle format string for specified string literal containing date or timestamp value
bool SqlParser::RecognizeOracleDateFormat(Token *str, TokenStr &format)
{
	if(str == NULL || str->type != TOKEN_STRING || str->len == 0)
		return false;

	bool exists = false;

	// YYYYMMDD format
	if(str->len == 10)
	{
		format.Append("'YYYYMMDD'", L"'YYYYMMDD'", 10);
		exists = true;
	}
	else
	// Full DATE with 2 delimiters
	if(str->len == 12)
	{
		// 'xx/xx/yyyy', 'xx-xx-yyyy', 'xx.xx.yyyy'
		if((Token::Compare(str, '/', L'/', 3) == true && Token::Compare(str, '/', L'/', 6) == true) ||
			(Token::Compare(str, '-', L'-', 3) == true && Token::Compare(str, '-', L'-', 6) == true) ||
			(Token::Compare(str, '.', L'.', 3) == true && Token::Compare(str, '.', L'.', 6) == true))
		{
			// Month or day can be at the first position
			int n1 = str->GetInt(1, 2);
			int n2 = str->GetInt(4, 2);

			if(n1 != -1 && n2 != -1)
			{
				bool mdy = false;
				bool dmy = false;

				// Day and month are equal in string literal
				if(n1 == n2)
				{
					// If delimiter is / use US notation by default MM/DD/YYYY
					if(Token::Compare(str, '/', L'/', 3) == true)
						mdy = true;
					else
						dmy = true;
				}

				// Month, day, year
				if(mdy == true)
				{
					if(Token::Compare(str, '/', L'/', 3) == true)
						format.Append("'MM/DD/YYYY'", L"'MM/DD/YYYY'", 12);
					else
					if(Token::Compare(str, '-', L'-', 3) == true)
						format.Append("'MM-DD-YYYY'", L"'MM-DD-YYYY'", 12);
					else
					if(Token::Compare(str, '.', L'.', 3) == true)
						format.Append("'MM.DD.YYYY'", L"'MM.DD.YYYY'", 12);
				
					exists = true;
				}
			}
		}
	}
	else
	// ISO format YYYY-MM-DD HH24:MI:SS.FF 
	if(str->len >= 19)
	{
		// Month < 12
		int month = str->GetInt(6, 2);

		// Check delimiters in 'YYYY-MM-DD HH:MI:SS.FF'
		if(Token::Compare(str, '-', L'-', 5) == true && Token::Compare(str, '-', L'-', 8) == true &&
			Token::Compare(str, ':', L':', 14) == true && Token::Compare(str, ':', L':', 17) == true &&
			month <= 12)
		{
			format.Append("'YYYY-MM-DD HH24:MI:SS", L"'YYYY-MM-DD HH24:MI:SS", 22);

			// Check if fraction is set, FF recognizes any number of digits
			if(str->len > 19)
				format.Append(".FF", L".FF", 3);

			format.Append("'", L"'", 1);
		
			exists = true;
		}		
	}

	return exists;
}

// Add sequence and trigger to emulate identity column
void SqlParser::OracleEmulateIdentity(Token *create, Token *table, Token *column, Token *last, 
										Token *id_start, Token *id_inc, bool id_default)
{
	if(last == NULL)
		return;

	TokenStr seq_name(table);
	TokenStr tr_name(table);

	AppendIdentifier(seq_name, "_seq", L"_seq", 4);
	AppendIdentifier(tr_name, "_seq_tr", L"_seq_tr", 7);
		
	// Generate CREATE SEQUENCE after CREATE TABLE
	Append(last, "\n\n", L"\n\n", 2);
	AppendNoFormat(last, "-- Generate ID using sequence and trigger", L"-- Generate ID using sequence and trigger", 41);
	Append(last, "\n", L"\n", 1);

	Append(last, "CREATE SEQUENCE ", L"CREATE SEQUENCE ", 16, create);
	AppendNoFormat(last, &seq_name);
	Append(last, " START WITH ", L" START WITH ", 12, create);

	if(id_start != NULL)
		AppendCopy(last, id_start);
	else
		AppendNoFormat(last, "1", L"1", 1);

	Append(last, " INCREMENT BY ", L" INCREMENT BY ", 14, create);

	if(id_inc != NULL)
		AppendCopy(last, id_inc);
	else
		AppendNoFormat(last, "1", L"1", 1);

	Append(last, ";\n\n", L";\n\n", 3);

	Append(last, "CREATE OR REPLACE TRIGGER ", L"CREATE OR REPLACE TRIGGER ", 26, create);
	AppendNoFormat(last, &tr_name);
	Append(last, "\n BEFORE INSERT ON ", L"\n BEFORE INSERT ON ", 19, create);
	AppendCopy(last, table);
	Append(last, " FOR EACH ROW", L" FOR EACH ROW", 13, create);

	// GENERATED ALWAYS or BY DEFAULT can be used in DB2
	if(id_default == true)
	{
		Append(last, "\n WHEN (NEW.", L"\n  WHEN (NEW.", 12, create);
		AppendCopy(last, column);
		Append(last, " IS NULL)", L" IS NULL)", 9, create);
	}
	
	Append(last, "\nBEGIN\n SELECT ", L"\nBEGIN\n SELECT ", 15, create);
	AppendNoFormat(last, &seq_name);
	Append(last, ".NEXTVAL INTO :NEW.", L".NEXTVAL INTO :NEW.", 19, create);
	AppendCopy(last, column);
	Append(last, " FROM dual;\nEND;\n/", L" FROM dual;\nEND;\n/", 18, create);
}

// Transform DB2, MySQL EXIT HANDLERs to Oracle EXCEPTION
void SqlParser::OracleExitHandlersToException(Token *end)
{
	bool exists = false;

	// Find EXIT handlers
	for(CopyPaste *i = _copypaste.GetFirstNoCurrent(); i != NULL; i = i->next)
	{
		if(i->type != COPY_EXIT_HANDLER_NOT_FOUND && i->type != COPY_EXIT_HANDLER_SQLEXCEPTION &&
			i->type != COPY_EXIT_HANDLER_FOR_SQLSTATE)
			continue;

		// Add EXCEPTION keyword once
		if(exists == false)
		{
			Prepend(end, "EXCEPTION\n", L"\nEXCEPTION\n", 10);
			exists = true;
		}

		Token *token = i->tokens.GetFirstNoCurrent();

		// Copy tokens
		while(token != NULL)
		{
			PrependCopy(end, token);
			token = token->next;
		}

		Prepend(end, "\n", L"\n", 1);
	}
}

// Implement CONTINUE handler for NOT FOUND after FETCH statement in Oracle
void SqlParser::OracleContinueHandlerForFetch(Token *fetch, Token *cursor)
{
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

	Token *end = NULL;
	bool exists = false;

	// Find CONTINUE handler for NOT FOUND
	while(cur != NULL)
	{
		bool found = false;

		if(cur->type == COPY_CONTINUE_HANDLER_NOT_FOUND)
			found = true;

		// For DB2 check user-defined condition for SQLSTATE '02000'
		if(found == false && _source == SQL_DB2 && cur->type == COPY_CONTINUE_HANDLER_USER_DEFINED)
		{
			// Find SQLSTATE associated with the condition
			ListwmItem *i = Find(_spl_user_exceptions, cur->name);

			if(i != NULL && Token::Compare((Token*)i->value2, "'02000'", L"'02000'", 7) == true)
				found = true;
		}

		if(found == false)
		{
			cur = cur->next;
			continue;
		}

		// Define the end of FETCH statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Append(end, "\nIF ", L"\nIF ", 4, fetch);
		AppendCopy(end, cursor);
		Append(end, "%NOTFOUND THEN", L"%NOTFOUND THEN", 14, fetch);

		Token *token = cur->tokens.GetFirstNoCurrent();

		// Copy tokens
		while(token != NULL)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND IF;", L"\nEND IF;", 8, fetch);

		cur = cur->next;
	}
}

// Implement CONTINUE handler for NOT FOUND and SQLEXCEPTION after SELECT INTO in Oracle
void SqlParser::OracleContinueHandlerForSelectInto(Token *select)
{
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

	Token *end = NULL;
	bool exists = false;

	// Find CONTINUE handler for NOT FOUND
	while(cur != NULL)
	{
		if(cur->type != COPY_CONTINUE_HANDLER_NOT_FOUND && cur->type != COPY_CONTINUE_HANDLER_SQLEXCEPTION)
		{
			cur = cur->next;
			continue;
		}

		// Define the end of SELECT INTO statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Prepend(select, "BEGIN\n", L"BEGIN\n", 6);
		Append(end, "\nEXCEPTION WHEN ", L"\nEXCEPTION WHEN ", 16, select);
		
		if(cur->type == COPY_CONTINUE_HANDLER_NOT_FOUND)
			Append(end, "NO_DATA_FOUND THEN ", L"NO_DATA_FOUND THEN ", 19, select);
		else
			Append(end, "OTHERS THEN ", L"OTHERS THEN ", 12, select);

		Token *token = cur->tokens.GetFirstNoCurrent();

		// Copy tokens
		while(token != NULL)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND;", L"\nEND;", 5, select);

		cur = cur->next;
	}
}

// Implement CONTINUE handler after INSERT in Oracle
void SqlParser::OracleContinueHandlerForInsert(Token *insert)
{
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

	Token *end = NULL;
	bool exists = false;

	// Find user-defined handler 
	while(cur != NULL)
	{
		if(cur->type != COPY_CONTINUE_HANDLER_USER_DEFINED || cur->name == NULL)
		{
			cur = cur->next;
			continue;
		}

		Token *error = NULL;

		// Find native error code by condition name
		ListwmItem *i = Find(_spl_user_exceptions, cur->name);

		if(i != NULL)
			error = (Token*)i->value2;

		// No error code found
		if(error == NULL)
		{
			cur = cur->next;
			continue;
		}

		// Define the end of INSERT statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Prepend(insert, "BEGIN\n", L"BEGIN\n", 6);
		Append(end, "\nEXCEPTION WHEN ", L"\nEXCEPTION WHEN ", 16, insert);

		// Duplicate key in DB2
		if(_source == SQL_DB2 && Token::Compare(error, "'23505'", L"'23505'", 7))
			Append(end, "DUP_VAL_ON_INDEX THEN ", L"DUP_VAL_ON_INDEX THEN ", 22, insert);

		Token *token = cur->tokens.GetFirstNoCurrent();

		// Copy tokens
		while(token != NULL)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND;", L"\nEND;", 5, insert);

		cur = cur->next;
	}

	// Find a handler for SQLSTATE in Teradata
	for(CopyPaste *i = _copypaste.GetFirstNoCurrent(); i != NULL; i = i->next)
	{
		if(i->type != COPY_CONTINUE_HANDLER_FOR_SQLSTATE)
			continue;

		// Define the end of INSERT statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Prepend(insert, "BEGIN\n", L"BEGIN\n", 6);
		Append(end, "\nEXCEPTION WHEN ", L"\nEXCEPTION WHEN ", 16, insert);

		Token *token = i->tokens.GetFirstNoCurrent();

		// Copy tokens
		while(token != NULL)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND;", L"\nEND;", 5, insert);
	}
}

// Implement CONTINUE handler for NOT FOUND after UPDATE statement in Oracle
void SqlParser::OracleContinueHandlerForUpdate(Token *update)
{
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

	Token *end = NULL;
	bool exists = false;

	// Find CONTINUE handler for NOT FOUND
	while(cur != NULL)
	{
		if(cur->type != COPY_CONTINUE_HANDLER_NOT_FOUND)
		{
			cur = cur->next;
			continue;
		}

		// Define the end of UPDATE statement
		if(exists == false)
		{
			end = GetLastToken(GetNextCharToken(';', L';'));
			exists = true;
		}

		Append(end, "\nIF SQL%ROWCOUNT = 0 THEN ", L"\nIF SQL%ROWCOUNT = 0 THEN ", 26, update);

		Token *token = cur->tokens.GetFirstNoCurrent();

		// Copy tokens
		while(token != NULL)
		{
			AppendCopy(end, token);
			token = token->next;
		}

		Append(end, "\nEND IF;", L"\nEND IF;", 8, update);

		cur = cur->next;
	}
}

// If there declarations in the body move BEGIN after last DECLARE (used for PostgreSQL as well)
void SqlParser::OracleMoveBeginAfterDeclare(Token *create, Token *as, Token *begin, Token *body_start)
{
	// No declaration statements
	if(_spl_last_declare == NULL)
	{
		// Oracle requires IS keyword
		if(_target == SQL_ORACLE && as == NULL)
		{
			if(begin != NULL)
				Prepend(begin, "IS\n", L"IS\n", 3);
			else
				Append(body_start, " IS ", L" IS ", 4, create);
		}

		// No BEGIN in source, but it is required for Oracle
		if(begin == NULL)
			Append(body_start, "\nBEGIN", L"\nBEGIN", 6, create);
	}
	// There are declarations
	else
	{
		Token *append = GetDeclarationAppend();

		if(append != NULL)
		{
			if(append->next != NULL && append->next->IsBlank() == true)
				Append(append, "\nBEGIN", L"\nBEGIN", 6, Nvl(begin, create));
			else
			if(append->prev != NULL && append->prev->IsBlank() == true)
				Append(append, "BEGIN\n", L"BEGIN\n", 6, Nvl(begin, create));
			else
				Append(append, "\nBEGIN\n", L"\nBEGIN\n", 7, Nvl(begin, create));
		}

		// Oracle requires IS keyword
		if(_target == SQL_ORACLE && as == NULL)
		{
			if(begin != NULL)
				Token::Change(begin, "IS", L"IS", 2);
			else
				Append(body_start, " IS ", L" IS ", 4, create);
		}
		// Remove BEGIN in the original position
		else
			Token::Remove(begin);
	}
}

// Add cursor parameters if the procedure returns a result set
void SqlParser::OracleAddOutRefcursor(Token *create, Token *name, Token *last_param)
{
	// No result sets
	if(_spl_result_sets == 0)
		return;

	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

	int num = 0;

	Token *append = NULL;

	// Procedure has parameters
	if(last_param != NULL)
	{
		AppendNoFormat(last_param, ", ", L", ", 2);
		append = last_param;
	}
	else
	{
		AppendNoFormat(name, " (", L" (", 2);
		append = name;
	}

	// Find cursor names
	while(cur != NULL)
	{
		if(cur->type != COPY_CURSOR_WITH_RETURN)
		{
			cur = cur->next;
			continue;
		}

		if(num > 0)
			AppendNoFormat(append, ", ", L", ", 2);

		AppendCopy(append, cur->name);
		Append(append, " OUT SYS_REFCURSOR", L" OUT SYS_REFCURSOR", 18, create);

		cur = cur->next;
		num++;
	}

	// Find generated cursor names for standalone SELECT statements returning a result set
	for(TokenStr *t = _spl_result_set_generated_cursors.GetFirst(); t != NULL; t = t->next)
	{
		if(num > 0)
			AppendNoFormat(append, ", ", L", ", 2);

		AppendNoFormat(append, t);
		Append(append, " OUT SYS_REFCURSOR", L" OUT SYS_REFCURSOR", 18, create);

		num++;
	}

	// Find cursor name for prepared statements with return
	for(ListwmItem *i = _spl_prepared_stmts_cursors_with_return.GetFirst(); i != NULL; i = i->next)
	{
		Token *cursor = (Token*)i->value;

		if(num > 0)
			AppendNoFormat(append, ", ", L", ", 2);

		AppendCopy(append, cursor);
		Append(append, " OUT SYS_REFCURSOR", L" OUT SYS_REFCURSOR", 18, create);

		num++;
	}

	if(last_param == NULL && num > 0)
		AppendNoFormat(append, ")", L")", 1);
}

// Check if temporary tables need to be truncated at the beginning of block in Oracle
void SqlParser::OracleTruncateTemporaryTablesWithReplace()
{
	for(ListwItem *i = _spl_declared_tables_with_replace.GetFirst(); i != NULL; i = i->next)
	{
		Token *table_name = (Token*)i->value;

		if(i->prev == NULL)
			Prepend(_spl_first_non_declare, "\n", L"\n", 1);

		Prepend(_spl_first_non_declare, "EXECUTE IMMEDIATE 'TRUNCATE TABLE ", L"EXECUTE IMMEDIATE 'TRUNCATE TABLE ", 34);
		PrependCopy(_spl_first_non_declare, table_name);
		Prepend(_spl_first_non_declare, "';\n", L"';\n", 3);

		if(i->next == NULL)
			Prepend(_spl_first_non_declare, "\n", L"\n", 1);
	}
}

// Move SELECT statement inside IF to a standalone SELECT INTO before IF
void SqlParser::OracleMoveSelectOutofIf(Token *stmt_start, Token *select, Token *select_list_end, 
										Token *open, Token *close)
{
	if(stmt_start == NULL || open == NULL || close == NULL)
		return;

	TokenStr var("v_cnt", L"v_cnt", 5);

	// ( and ) are not required in the standalone SELECT INTO
	Token::ChangeNoFormat(open, var);
	Token::Change(close, ";", L";", 1);

	Append(select_list_end, " INTO ", L" INTO ", 6, select);
	AppendNoFormat(select_list_end, &var);

	PrependCopy(stmt_start, open->next, close, false);
	Prepend(stmt_start, "\n\n", L"\n\n", 2);

	// Remove SELECT in IF
	Token::Remove(open->next, close);

	_spl_moved_if_select++;
}

// Add declarations for SELECT statement moved out of IF condition
void SqlParser::OracleIfSelectDeclarations()
{
	if(_spl_moved_if_select == 0)
		return;

	//Token *append = GetDeclarationAppend();

	//Append(append, "v_cnt NUMBER(10);", L"v_cnt NUMBER(10);", 17); 
}

// Change = to DEFAULT keyword in procedure parameters 
void SqlParser::OracleChangeEqualToDefault(Token *equal)
{
	if(equal == NULL)
		return;

	bool space_before = false;
	bool space_after = false;

	// INT=0 without spaces is possible, so check if we need to add spaces arounf DEFAULT keyword
	if(equal->prev != NULL && equal->prev->IsBlank() == true)
		space_before = true;

	if(equal->next != NULL && equal->next->IsBlank() == true)
		space_after = true;

	if(space_before == true && space_after == true)
		Token::Change(equal, "DEFAULT", L"DEFAULT", 7);
	else
	if(space_before == true && space_after == false)
		Token::Change(equal, "DEFAULT ", L"DEFAULT ", 8);
	else
	if(space_before == false && space_after == true)
		Token::Change(equal, " DEFAULT", L" DEFAULT", 8);
	else
		Token::Change(equal, " DEFAULT ", L" DEFAULT ", 9);
}

// Oracle SQL*Plus SET options, SET DEFINE ON/OFF i.e
bool SqlParser::ParseOracleSetOptions(Token *set)
{
	if(set == NULL)
		return false;

	bool exists = false;

	bool comment = false;
	bool remove = false;

	Token *option = GetNextToken();

	if(option == NULL)
		return false;

	// SET DEFINE ON | OFF
	if(option->Compare("DEFINE", L"DEFINE", 6) == true)
	{
		Token *on = GetNext("ON", L"ON", 2);
		Token *off = NULL;

		if(on == NULL)
			off = GetNext("OFF", L"OFF", 3);

		if(on != NULL || off != NULL)
		{
			if(_target != SQL_ORACLE)
				comment = true;

			exists = true;
		}
		else
			PushBack(option);
	}
	// Not a SET option
	else
		PushBack(option);

	if(exists == true)
	{
		Token *last = Nvl(GetNextCharToken(';', L';'), GetLastToken());

		if(comment == true)
			Comment(set, last);
		else
		if(remove == true)
			Token::Remove(set, last);
	}

	return exists;
}
