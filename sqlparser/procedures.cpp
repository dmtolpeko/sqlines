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

// SQLParser for system procedures

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "listw.h"

// Parse a system procedure 
bool SqlParser::ParseSystemProcedure(Token *execute, Token *name)
{
	// EXECUTE | EXEC is optional (in SQL Server)

	bool exists = false;
	bool name_fetched = false;

	if(name == NULL)
	{
		name = GetNextToken();
		name_fetched = true;
	}

	if(name == NULL)
		return false;

    if(TOKEN_CMP(name, "RAISE_APPLICATION_ERROR") == true)
        exists = ParseProcedureRaiseApplicationError(name);
    else
	if(TOKEN_CMP(name, "SP_ADDTYPE") || TOKEN_CMP(name, "SYS.SP_ADDTYPE"))
		exists = ParseProcedureSpAddType(execute, name);
	else
	if(TOKEN_CMP(name, "SP_BINDRULE") || TOKEN_CMP(name, "SYS.SP_BINDRULE"))
		exists = ParseProcedureSpBindRule(execute, name);
	else
	if(TOKEN_CMP(name, "SP_ADDEXTENDEDPROPERTY") || TOKEN_CMP(name, "SYS.SP_ADDEXTENDEDPROPERTY"))
		exists = ParseProcedureSpAddExtendedProperty(execute, name);
	else
	// Check for system procedure prefix (like sp_ in SQL Server)
	if(ParseUnknownSystemProcedure(name))
		exists = true;
	else
	if(name_fetched == true)
		PushBack(name);

	return exists;
}

// Oracle RAISE_APPLICATION_ERROR system procedure
bool SqlParser::ParseProcedureRaiseApplicationError(Token *name)
{
    Token *open = TOKEN_GETNEXT('(');

	STATS_DECL
	STATS_DTL_DECL

	STATS_SET_DESC(SQL_PROC_RAISE_APPLICATION_ERROR_DESC)

    if(open == NULL)
        return false;

    // Error number
    Token *error_num = ParseExpression();

    Token *comma = TOKEN_GETNEXT(','); 

    if(comma == NULL)
        return false;

    // Error message
    Token *error_mess = ParseExpression();

	if(error_mess == NULL)
        return false;

    Token *close = TOKEN_GETNEXT(')');

	if(_target == SQL_SQL_SERVER)
	{
		// Make error number part of the error mesage
		if(error_mess->type == TOKEN_STRING)
		{
			// -20000, 'Text' -> '-20000 Text'
			TokenStr mess;
			APPENDSTR(mess, "'");
			mess.Append(error_num);
			APPENDSTR(mess, " ");
			mess.Append(error_mess, 1, error_mess->len - 1);

			Token::ChangeNoFormat(error_num, mess);
			Token::Remove(comma, error_mess);
		}

		TOKEN_CHANGE(name, "RAISERROR");
		PREPEND_NOFMT(close, ", 16, 1");
	}

    PROC_STATS(name) 

    if(_stats != NULL)
    {
        TokenStr st;
        st.Append(name);
        st.Append(open);
        st.Append(error_num);
        st.Append(", text)", L", text)", 9);

		PROC_DTL_STATS(st.GetCStr(), name)
    }

    return true;
}

// Sybase ASE sp_addtype
bool SqlParser::ParseProcedureSpAddType(Token *execute, Token *sp_addtype)
{
	if(sp_addtype == NULL)
		return false;

	STATS_DECL

	// User-defined data type name
	Token *name = GetNextToken();

	if(name == NULL)
		return false;

	Token *comma = GetNextCharToken(',', L',');

	// In Sybase ASE, data type must be enclosed with ' or " if it contains spaces, () i.e.
	Token *dt_open = GetNextSpecialCharToken("'\"", L"'\"", 2); 

	Token *datatype = GetNextToken();

	// Parse base data type
	ParseDataType(datatype, SQL_SCOPE_SP_ADDTYPE);

	Token *dt_close = GetNextSpecialCharToken(dt_open, "'\"", L"'\"", 2); 

	// Parse optional attributes
	while(true)
	{
		Token *comman = GetNextCharToken(',', L',');

		if(comman == NULL)
			break;

		// Optional quotes, but required for NOT NULL attribute i.e
		Token *att_open = GetNextSpecialCharToken("'\"", L"'\"", 2); 

		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// NOT NULL
		if(next->Compare("NOT", L"NOT", 3) == true)
			/*Token *null */ (void) GetNextWordToken("NULL", L"NULL", 4);

		Token *att_close = GetNextSpecialCharToken(att_open, "'\"", L"'\"", 2);

		// Remove comma and quotes
		if(_target == SQL_POSTGRESQL)
		{
			Token::Remove(comman);
			Token::Remove(att_open);
			Token::Remove(att_close);
		}
	}

	// CREATE DOMAIN name AS type in PostgreSQL
	if(_target == SQL_POSTGRESQL)
	{
		if(execute != NULL)
		{
			Token::Change(execute, "CREATE", L"CREATE", 6);
			Token::Change(sp_addtype, "DOMAIN", L"DOMAIN", 6, execute);
		}
		else
			Token::Change(sp_addtype, "CREATE DOMAIN", L"CREATE DOMAIN", 13);

		Token::Change(comma, " AS ", L" AS ", 4, sp_addtype);

		Token::Remove(dt_open);
		Token::Remove(dt_close);
	}

	// Add statement delimiter if not set when source is SQL Server, Sybase ASE
	SqlServerAddStmtDelimiter(true);

	PROC_STATS(name)

	return true;
}

// Sybase ASE sp_bindrule
bool SqlParser::ParseProcedureSpBindRule(Token *execute, Token *sp_bindrule)
{
	if(sp_bindrule == NULL)
		return false;

	STATS_DECL

	// Rule name created using CREATE RULE statement
	Token *name = GetNextToken();

	if(name == NULL)
		return false;

	Token *comma = GetNextCharToken(',', L',');

	// In Sybase ASE, table.column or user-defined type can be enclosed with ' or " 
	Token *open = GetNextSpecialCharToken("'\"", L"'\"", 2); 

	// Table.column or user-defined type
	Token *object = GetNextToken();

	if(object == NULL)
		return false;

	Token *close = GetNextSpecialCharToken(open, "'\"", L"'\"", 2); 

	// Single part means user-defined type, otherwise means table.column
	int num = GetIdentPartsCount(object);

	TokenStr table;
	TokenStr column;

	SplitIdentifierByLastPart(object, table, column, num);

	// ALTER TABLE table ADD CHECK condition in PostgreSQL
	if(_target == SQL_POSTGRESQL)
	{
		if(execute != NULL)
		{
			Token::Change(execute, "ALTER", L"ALTER", 5);
			Token::Change(sp_bindrule, "TABLE", L"TABLE", 5, execute);
		}
		else
			Token::Change(sp_bindrule, "ALTER TABLE", L"ALTER TABLE", 11);

		PrependNoFormat(name, &table);
		Prepend(name, " ADD CHECK ", L" ADD CHECK ", 11, sp_bindrule);

		// Find the rule
		ListwmItem *rule = Find(_domain_rules, name);

		if(rule != NULL)
		{
			Token *start = (Token*)rule->value2;
			Token *end = (Token*)rule->value3;
			Token *cur = start;

			// Copy the rule condition
			while(cur != NULL)
			{
				// Replace @any_ident with the column name in the rule
				if(Token::Compare(cur, '@', '@', 0) == true)
					AppendNoFormat(object, &column);
				else
					AppendCopy(object, cur);

				if(cur == end)
					break;

				cur = cur->next;
			}
		}
		
		CommentNoSpaces(name);

		Token::Remove(comma);
		Token::Remove(open);
		Token::Remove(object);
		Token::Remove(close);
	}

	// Add statement delimiter if not set when source is SQL Server, Sybase ASE
	SqlServerAddStmtDelimiter(true);

	PROC_STATS(sp_bindrule) 

	return true;
}

// SQL Server sp_addextendedproperty
bool SqlParser::ParseProcedureSpAddExtendedProperty(Token *execute, Token *sp_extendedproperty)
{
	if(sp_extendedproperty == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_PROC_SP_ADDEXTENDED_PROPERTY_DESC)

	Token *name = NULL;
	Token *value = NULL;
    
	Token *level0_type =  NULL;
    Token *level0_name =  NULL;
    
	Token *level1_type =  NULL;
    Token *level1_name =  NULL;
    
	Token *level2_type =  NULL;
    Token *level2_name =  NULL;

	int no_named_param_cnt = 1;

	while(true)
	{
		Token *next = GetNext();

		if(next == NULL)
			break;

		Token *equal = TOKEN_GETNEXT('=');

		// Check for named parameter starting with @ following by = 
		if(TOKEN_CMP_PART0(next, "@") && equal != NULL)
		{
			Token *val = GetNextToken();

			if(TOKEN_CMP(next, "@name"))
				name = val;
			else
			if(TOKEN_CMP(next, "@value"))
				value = val;
			else
			if(TOKEN_CMP(next, "@level0type"))
				level0_type = val;
			else
			if(TOKEN_CMP(next, "@level0name"))
				level0_name = val;
			else
			if(TOKEN_CMP(next, "@level1type"))
				level1_type = val;
			else
			if(TOKEN_CMP(next, "@level1name"))
				level1_name = val;
			else
			if(TOKEN_CMP(next, "@level2type"))
				level2_type = val;
			else
			if(TOKEN_CMP(next, "@level2name"))
				level2_name = val;

			if(Target(SQL_MARIADB, SQL_MYSQL))
				Token::Remove(next, equal);
		}
		else
		// Parameters by position
		{
			switch(no_named_param_cnt) {
				case 1: name = next; break;
				case 2: value = next; break;
				case 3: level0_type = next; break;
				case 4: level0_name = next; break;
				case 5: level1_type = next; break;
				case 6: level1_name = next; break;
				case 7: level2_type = next; break;
				case 8: level2_name = next; break;
			}

			ParseExpression(next);
			no_named_param_cnt++;
		}

		Token *comma = TOKEN_GETNEXT(',');

		if(comma == NULL)
			break;

		if(Target(SQL_MARIADB, SQL_MYSQL))
			Token::Remove(comma);
	}

	// Check for a comment for a table or column - N'MS_Description'
	if(TOKEN_CMP(name, "N'MS_Description'") || TOKEN_CMP(name, "'MS_Description'"))
	{
		// Table comment (no 'Column' parameter in call)
		if(level2_name == NULL)
		{
			// ALTER TABLE name COMMENT 'comment' table comment
			if(Target(SQL_MARIADB, SQL_MYSQL))
			{
				TOKEN_CHANGE(Nvl(execute, sp_extendedproperty), "ALTER TABLE");
				PREPEND_FMT(value, "COMMENT ", Nvl(execute, sp_extendedproperty));

				if(execute != NULL)
					Token::Remove(sp_extendedproperty);

				TokenStr table_name;

				// Table name usually quoted, for example, 'employees'
				if(TOKEN_CMP_PART0(level1_name, "'") && level1_name->len > 2)
					table_name.Append(level1_name, 1, level1_name->len - 2);
				else
					table_name.Append(level1_name);

				// Add a table name
				APPEND_NOFMT(sp_extendedproperty, " ");
				APPENDS(sp_extendedproperty, &table_name);
				
				Token::Remove(name);

				Token::Remove(level0_type);
				Token::Remove(level0_name);
    			
				Token::Remove(level1_type);
				Token::Remove(level1_name);

				STATS_SET_CONV_OK(Target(SQL_MARIADB, SQL_MYSQL)) 
			}
		}
	}

	// Add statement delimiter if not set when source is SQL Server, Sybase ASE
	SqlServerAddStmtDelimiter(true);

	PROC_STATS(sp_extendedproperty) 

	return true;
}

// Unknown system procedure
bool SqlParser::ParseUnknownSystemProcedure(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL

	bool exists = false;

	// SQL Server system procedures start with sys.sp_xxx or sp_xxx
	if(_source == SQL_SQL_SERVER)
	{
		if(TOKEN_CMP_PART0(name, "SYS.SP_") || TOKEN_CMP_PART0(name, "SP_"))
			exists = true;
	}

	if(exists)
	{
		STATS_SET_DESC(SQL_PROC_SYSTEM_PROC_DESC)
		PROC_STATS(name) 
	}

	return exists;
}