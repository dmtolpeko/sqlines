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

// SQL Server conversions

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Convert row-level trigger using inserted and deleted system tables
void SqlParser::SqlServerConvertRowLevelTrigger(Token *table, Token *when, Token *insert, Token *update, 
					Token *delete_, Token *end)
{
	if(table == NULL || when == NULL)
		return;

	TokenStr old_cols;
	TokenStr old_vars;

	Token *decl_end = _spl_last_declare;

	// Skip statement delimiter
	if(decl_end != NULL && Token::Compare(decl_end->next, ';', L';') == true)
		decl_end = decl_end->next;
	
	int num = 0;
	size_t old_prefix_len = 4;

	// Skip old. in column name references (old already prefixed with @ in target)
	if(_spl_old_correlation_name != NULL)
		old_prefix_len = _spl_old_correlation_name->len + 1 + 1;

	// Generate declaration for referenced OLD columns
	for(ListwItem *i = _spl_tr_old_columns.GetFirst(); i != NULL; i = i->next)
	{
		Token *var = (Token*)i->value;

		if(num == 0)
			Append(decl_end, "\n", L"\n", 1);
		else
		{
			old_cols.Append(", ", L", ", 2);
			old_vars.Append(", ", L", ", 2);
		}

		Append(decl_end, "\nDECLARE ", L"\nDECLARE ", 9, _declare_format);
		AppendCopy(decl_end, var);
		Append(decl_end, " ", L" ", 1);

		// Define the data type of the variable
		TokenStr type;
		GuessType(var, type);
		AppendNoFormat(decl_end, &type);

		Append(decl_end, ";", L";", 1);

		old_cols.Append(var, old_prefix_len, var->len - old_prefix_len + 1);
		old_vars.Append(var);

		num++;
	}
	
	// Declare deleted cursor
	if(delete_ != NULL)
	{
		Append(decl_end, "\nDECLARE ", L"\nDECLARE ", 9, _declare_format); 
		AppendNoFormat(decl_end, "deleted_cur ", L"deleted_cur ", 12); 
		Append(decl_end, "CURSOR FOR SELECT ", L"CURSOR FOR SELECT ", 18, _declare_format);
		AppendNoFormat(decl_end, &old_cols);
		Append(decl_end, " FROM ", L" FROM ", 6, _declare_format);
		AppendNoFormat(decl_end, "deleted;", L"deleted;", 8);
	}

	if(delete_ != NULL)
		Append(decl_end, "\n\nOPEN deleted_cur;", L"\n\nOPEN deleted_cur;", 19); 

	Append(decl_end, "\nWHILE 1=0 BEGIN", L"\nWHILE 1=0 BEGIN", 16); 

	if(delete_ != NULL)
	{
		Append(decl_end, "\nFETCH deleted_cur INTO ", L"\nFETCH deleted_cur INTO ", 24);
		AppendNoFormat(decl_end, &old_vars);
		Append(decl_end, ";", L";", 1);
	}

	Append(decl_end, "\nIF @@FETCH_STATUS <> 0 BREAK;", L"\nIF @@FETCH_STATUS <> 0 BREAK;", 30);

	// Terminate the cursor loop
	Token *end_loop = Prepend(end, "END\n", L"END\n", 4);

	if(delete_ != NULL)
	{
		Append(end_loop, "CLOSE deleted_cur;", L"CLOSE deleted_cur;", 18);
		Append(end_loop, "\nDEALLOCATE deleted_cur;", L"\nDEALLOCATE deleted_cur;", 24);
	}

	// Perform DELETE in INSTEAD OF trigger
	if(delete_ != NULL && insert == NULL && update == NULL)
	{
		Append(end_loop, "\nDELETE FROM ", L"\nDELETE FROM ", 13);
		AppendCopy(end_loop, table);
		Append(end_loop, " WHERE (", L" WHERE (", 8); 
		AppendNoFormat(end_loop, &old_cols);
		Append(end_loop, ") IN (SELECT ", L") IN (SELECT ", 13);
		AppendNoFormat(end_loop, &old_cols);
		Append(end_loop, " FROM deleted);\n", L" FROM deleted);\n", 16);
	}
}

// SQL Server requires an alias for subquery
void SqlParser::SqlServerAppendSubqueryAlias(Token *append, int *appended_subquery_aliases)
{
	if(append == NULL)
		return;

	int alias_num = 0;

	// Define the current counter within SELECT
	if(appended_subquery_aliases != NULL)
		alias_num = *appended_subquery_aliases;

	TokenStr alias(" s", L" s", 2);
	
	if(alias_num != 0)
		alias.Append(alias_num);

	AppendNoFormat(append, &alias);

	alias_num++;

	if(appended_subquery_aliases != NULL)
		*appended_subquery_aliases = alias_num;
}

// Any Transact-SQL statement can be followed by ; and GO, any combination of them or nothing at all
void SqlParser::SqlServerDelimiter()
{
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == false || Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
		return;

	Token *last = GetLastToken();

	// Optional ; can go before GO
	Token *semi = GetNextCharToken(';', L';');

	// Optional GO to terminate the batch
	Token *go = GetNextWordToken("GO", L"GO", 2);

	// No ; delimiter
	if(semi == NULL)
		AppendNoFormat(last, ";", L";", 1);

	Token::Remove(go);
}

// Add statement delimiter if not set when source is SQL Server, Sybase ASE
void SqlParser::SqlServerAddStmtDelimiter(bool force)
{
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == false || TOKEN_CMPC(GetLastToken(), ';'))
		return;

	// Add the delimiter in statements in procedural block only
	if(_spl_scope == 0 && force == false)
	{
		// Special case when a single statement followed by GO, then add ; even if we are no inside procedure scope
		if(!LookNext("GO", L"GO", 2))
			return;
	}

	// Check if the delimiter already set
	Token *semi = GetNextCharToken(';', L';');

	if(semi != NULL)
	{
		PushBack(semi);
		return;
	}

	// Do not set if comma followed
	Token *comma = GetNextCharToken(',', L',');

	if(comma != NULL)
	{
		PushBack(comma);
		return;
	}

	AppendNoFormat(GetLastToken(), ";", L";", 1);
}

// Handle GO after a statement in SQL Server
bool SqlParser::SqlServerGoDelimiter(bool just_remove)
{
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == false || Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
		return false;

	Token *stmt_end = GetLastToken();

	// Optional ; can go before GO
	Token *semi = GetNextCharToken(';', L';');

	Token *go = GetNextWordToken("GO", L"GO", 2);

	if(go == NULL)
	{
		// ; is processed at statement parser
		if(semi != NULL)
			PushBack(semi);

		return false;
	}

	// The delimiter ; does not already exist, add it otherwise just remove GO
	if(semi == NULL && just_remove == false)
		AppendNoFormat(stmt_end, ";", L";", 1);

	Token::Change(go, " ", L" ", 1);

	return true;
}

// SQL Server SET options, SET ANSI_NULLS ON i.e
bool SqlParser::ParseSqlServerSetOptions(Token *set)
{
	if(set == NULL)
		return false;

	bool exists = false;

	bool comment = false;
	bool remove = false;

	Token *option = GetNextToken();

	if(option == NULL)
		return false;

	// SET ANSI_NULLS ON | OFF
	if(option->Compare("ANSI_NULLS", L"ANSI_NULLS", 10) == true)
	{
		/*Token *value */ (void) GetNextToken();

		if(_target != SQL_SQL_SERVER)
			comment = true;

		exists = true;
	}
	else
	// SET ANSI_PADDING ON | OFF
	if(option->Compare("ANSI_PADDING", L"ANSI_PADDING", 12) == true)
	{
		/*Token *value */ (void) GetNextToken();

		if(_target != SQL_SQL_SERVER)
			comment = true;

		exists = true;
	}
	else
	// SET NOCOUNT ON | OFF
	if(option->Compare("NOCOUNT", L"NOCOUNT", 7) == true)
	{
		/*Token *value */ (void) GetNextToken();

		if(_target != SQL_SQL_SERVER)
			remove = true;

		exists = true;
	}
	else
	// SET QUOTED_IDENTIFIER ON | OFF
	if(option->Compare("QUOTED_IDENTIFIER", L"QUOTED_IDENTIFIER", 17) == true)
	{
		/*Token *value */ (void) GetNextToken();

		if(_target != SQL_SQL_SERVER)
			comment = true;

		exists = true;
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

		// Remove following GO if any
		SqlServerGoDelimiter(true);
	}

	return exists;
}

// SQL Server index options WITH (PAD_INDEX = OFF, ...)
bool SqlParser::ParseSqlServerIndexOptions(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	// Start with WITH keyword
	if(token->Compare("WITH", L"WITH", 4) == false)
		return false;

	// ( can be omitted if FILLFACTOR is only specified
	Token *open = GetNext('(', L'(');

	while(true)
	{
		bool remove = false;

		Token *option = GetNextToken();

		if(option == NULL)
			break;

		// = between option name and value
		/*Token *equal */ (void) GetNext('=', L'=');

		Token *value = NULL;

		// ALLOW_PAGE_LOCKS = ON | OFF
		if(option->Compare("ALLOW_PAGE_LOCKS", L"ALLOW_PAGE_LOCKS", 16) == true)
		{
			value = GetNextToken();

			if(_target != SQL_SQL_SERVER)
				remove = true;

			exists = true;
		}
		else
		// ALLOW_ROW_LOCKS = ON | OFF
		if(option->Compare("ALLOW_ROW_LOCKS", L"ALLOW_ROW_LOCKS", 15) == true)
		{
			value = GetNextToken();

			if(_target != SQL_SQL_SERVER)
				remove = true;

			exists = true;
		}
		else
		// FILLFACTOR = num
		if(option->Compare("FILLFACTOR", L"FILLFACTOR", 10) == true)
		{
			value = GetNextToken();

			if(_target != SQL_SQL_SERVER)
				remove = true;

			exists = true;
		}
		else
		// IGNORE_DUP_KEY = ON | OFF
		if(option->Compare("IGNORE_DUP_KEY", L"IGNORE_DUP_KEY", 14) == true)
		{
			value = GetNextToken();

			if(_target != SQL_SQL_SERVER)
				remove = true;

			exists = true;
		}
		else
		// PAD_INDEX = ON | OFF
		if(option->Compare("PAD_INDEX", L"PAD_INDEX", 9) == true)
		{
			value = GetNextToken();

			if(_target != SQL_SQL_SERVER)
				remove = true;

			exists = true;
		}
		else
		// STATISTICS_NORECOMPUTE = ON | OFF
		if(option->Compare("STATISTICS_NORECOMPUTE", L"STATISTICS_NORECOMPUTE", 22) == true)
		{
			value = GetNextToken();

			if(_target != SQL_SQL_SERVER)
				remove = true;

			exists = true;
		}
		else
		{
			PushBack(option);
			break;
		}

		Token *comma = GetNext(',', L',');

		// Remove the option
		if(remove == true)
		{
			Token::Remove(option, value);
			Token::Remove(comma);
		}

		if(comma == NULL)
			break;
	}

	if(exists == false)
	{
		// PostgreSQL also has WITH ( ... option, so return open token
		PushBack(open);

		return false;
	}

	Token *close = GetNext(open, ')', L')');

	// Remove WITH ()
	if(_target != SQL_SQL_SERVER)
	{
		Token::Remove(token);
		Token::Remove(open);
		Token::Remove(close);
	}

	// Storage clause can follow 
	ParseSqlServerStorageClause();

	return exists;
}

// Convert interval +/- expression to DATEADD function
void SqlParser::SqlServerToDateAdd(Token *op, Token *first, Token *first_end, Token *second, Token *second_end)
{
	if(op == NULL || first == NULL || second == NULL)
		return;

	PREPEND(first, "DATEADD(");

	if(second->data_subtype == TOKEN_DT2_INTVL_MON)
		PREPEND_NOFMT(first, "mm");
	else
	if(second->data_subtype == TOKEN_DT2_INTVL_DAY)
		PREPEND_NOFMT(first, "dd");
	else
	if(second->data_subtype == TOKEN_DT2_INTVL_MIN)
		PREPEND_NOFMT(first, "mi");
	else
	if(second->data_subtype == TOKEN_DT2_INTVL_SEC)
		PREPEND_NOFMT(first, "ss");
	else
		PREPEND_NOFMT(first, "dd");

	PREPEND_NOFMT(first, ", ");

	// Copy - sign only, + will not be used
	if(TOKEN_CMPC(op, '-'))
		PrependCopy(first, op);

	PrependCopy(first, second, second_end, false);

	PREPEND_NOFMT(first, ", ");
	APPEND_NOFMT(first_end, ")");

	Token::Remove(op, second_end);
}

// SQL Server, Sybase ASE EXEC procedure statement
void SqlParser::ParseSqlServerExecProcedure(Token *execute, Token *name)
{
	if(execute == NULL || name == NULL)
		return;

	ConvertIdentifier(name, SQL_IDENT_OBJECT);

	if(Target(SQL_MARIADB, SQL_MYSQL))
		TOKEN_CHANGE(execute, "CALL");

	int params = 0;

	// Parse parameters
	// Sybase ASE does not use () around parameters
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// If there are no params, we can have conflict with next statement as there may be no delimiters in Transact-SQL
		// If there is at least one parameter, no conflict is possible for subsequent params as comma defines whether it exists or not
		if(params == 0 && !IsValidAlias(next))
		{
			PushBack(next);
			break;
		}

		ConvertIdentifier(next, SQL_IDENT_VAR);

		// OUTPUT qualifier can be specified for parameter in Sybase ASE
		Token *output = GetNext("OUTPUT", L"OUTPUT", 6);

		if(output != NULL && Target(SQL_MARIADB, SQL_MYSQL))
			Token::Remove(output);

		Token *comma = GetNext(',', L',');

		if(comma == NULL)
			break;

		params++;
	}

	// When there are no parameters MySQL, MariaDB allow as CALL name() and CALL name
	if(params > 0)
	{		
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			APPEND_NOFMT(name, "(");
			APPEND_NOFMT(GetLastToken(), ")");
		}
	}
}

// Parse SQL Server, Sybase ASE UPDATE statememt
bool SqlParser::ParseSqlServerUpdateStatement(Token *update)
{
	Token *name = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(name == NULL)
		return false;

	Token *set = TOKEN_GETNEXTW("SET");

	if(set == NULL)
	{
		PushBack(name);
		return false;
	}
	
	// Parser list of assignments: c1 = exp1, ...
	while(true)
	{
		Token *col = GetNextIdentToken();

		if(col == NULL)
			break;
		
		Token *equal = TOKEN_GETNEXT('=');

		if(equal == NULL)
			break;

		// Single value or (SELECT c1, c2, ...) can be specified
		Token *open = TOKEN_GETNEXT('(');

		Token *select = NULL;

		// Check for SELECT statement
		if(open != NULL)
		{
			select = GetNextSelectStartKeyword();

			// A subquery used to specify assignment values
			if(select != NULL)
				ParseSelectStatement(select, 0, SQL_SEL_UPDATE_SET, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

			TOKEN_GETNEXT(')');
		}
		else
		{
			Token *exp = GetNextToken();

			if(exp == NULL)
				break;

			ParseExpression(exp);
		}
		
		Token *comma = TOKEN_GETNEXT(',');

		if(comma == NULL)
			break;
	}

	Token *from = NULL;
	Token *from_end = NULL;

	// FROM clause can include inner/outer joins
	ParseSelectFromClause(NULL, false, &from, &from_end, NULL, true, NULL);
        	
    Token *where_ = NULL;
    Token *where_end = NULL;

	// optional WHERE clause
	ParseWhereClause(SQL_STMT_UPDATE, &where_, &where_end, NULL);

	// UPDATE FROM syntax is used
	if(from != NULL)
	{
		// MySQL, MariaDB use syntax UPDATE t1, t2 SET ... WHERE
		if(Target(SQL_MYSQL, SQL_MARIADB))
		{
			Token::Remove(from);

			AppendCopy(name, from, from_end, false);

			Token::Remove(name);
			Token::Remove(from, from_end);
		}
	}

    // Implement CONTINUE handler for NOT FOUND in Oracle
	if(_target == SQL_ORACLE)
		OracleContinueHandlerForUpdate(update);

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// Convert user-defined function name in function call
void SqlParser::SqlServerConvertUdfIdentifier(Token *name)
{
	if(name == NULL)
		return;

	int parts = GetIdentPartsCount(name);

	// SQL Server requires explicit schema/owner name for calling UDFs
	if(parts == 1)
	{
		TokenStr name2("dbo.", L"dbo.", 4);
		name2.Append(name);

		Token::ChangeNoFormat(name, name2);
	}
}

// For SQL Server move cursor declarations that depend on procedural variables
void SqlParser::SqlServerMoveCursorDeclarations()
{
	ListwItem *cursor_item = _spl_declared_cursors_using_vars.GetFirst();

	// Check of cursors that use variables
	while(cursor_item != NULL)
	{
		Token *cursor = (Token*)cursor_item->value;
		ListwmItem *open_item = _spl_open_cursors.GetFirst();

		Token *declare = NULL;
		Token *declare_end = NULL;

		// Find all OPEN statements for this cursor (the same cursor can be open multiple times)
		while(open_item != NULL)
		{
			Token *open_stmt = (Token*)open_item->value;
			Token *open_cursor = (Token*)open_item->value2;

			if(Token::Compare(cursor, open_cursor))
			{
				// Get cursor declaration statement
				ListwmItem *declare_item = Find(_spl_declared_cursors_stmts, cursor);

				if(declare_item != NULL)
				{
					declare = (Token*)declare_item->value2;
					declare_end = (Token*)declare_item->value3;

					PrependCopy(open_stmt, declare, declare_end, false);
					PREPEND(open_stmt, "\n");
				}
			}

			open_item = open_item->next;
		}
		
		// Now we can remove the original declaration
		if(declare != NULL && declare_end != NULL)
			Token::Remove(declare, declare_end);

		cursor_item = cursor_item->next;
	}
}