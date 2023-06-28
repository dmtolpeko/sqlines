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

// SQLParser for SELECT statement

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "listw.h"

// SELECT statement 
bool SqlParser::ParseSelectStatement(Token *select, int block_scope, int select_scope, int *result_sets,
										Token **list_end, ListW *exp_starts, ListW *out_cols, ListW *into_cols,
										int *appended_subquery_aliases, Token **from_end_out, Token **where_end_out)
{
	if(select == NULL)
		return false;

	Token *select_open = NULL;

	// Any SELECT can be optionally enclosed with () and in this case select points to open
	if(TOKEN_CMPC(select, '('))
	{
		select_open = select;
		select = GetNextSelectStartKeyword();
	}

	STATS_DECL

	Token *from = NULL;
	Token *from_end = NULL;

	ListWM from_table_end;

	Token *where_ = NULL;
	Token *where_end = NULL;

	Token *order = NULL;

	int rowlimit = 0;

	// Row limit specified in SELECT list and SELECT options
	Token *rowlimit_slist = NULL;
	Token *rowlimit_soptions = NULL;
	
	bool rowlimit_percent = false;

	bool into = false;
	bool dummy_not_required = false;
	bool agg_func = false;
	bool agg_list_func = false;

	// Number of generated aliases for subqueries (required for SQL Server), start counter here if outer counter is NULL
	int app_subq_aliases = 0;

	if(appended_subquery_aliases != NULL)
		app_subq_aliases = *appended_subquery_aliases;

	Enter(SQL_SCOPE_SELECT_STMT);

	// Check for common table expression CTE
	if(select->Compare("WITH", L"WITH", 4) == true)
	{
		if(ParseSelectCteClause(select) == true)
			GetNextWordToken("SELECT", L"SELECT", 6);
	}

	ListW out_cols_internal;

	// Fill list of columns for further use even if it is not requested by caller
	if(out_cols == NULL)
		out_cols = &out_cols_internal;

	ParseSelectList(select, select_scope, &into, &dummy_not_required, &agg_func, &agg_list_func, 
		exp_starts, out_cols, into_cols, &rowlimit_slist, &rowlimit_percent);

    Token *select_list_end = GetLastToken();

	if(list_end != NULL)
		*list_end = select_list_end;

	// FROM
	ParseSelectFromClause(select, false, &from, &from_end, &app_subq_aliases, dummy_not_required, &from_table_end);

	// Resolve data types for columns in select list
	SelectSetOutColsDataTypes(out_cols, &from_table_end);

	// WHERE
	ParseWhereClause(SQL_STMT_SELECT, &where_, &where_end, &rowlimit);

	if(_source == SQL_ORACLE)
		ParseConnectBy();

	// GROUP BY
	ParseSelectGroupBy();

	// HAVING clause
	ParseSelectHaving();

    // QUALIFY clause in Teradata
    ParseSelectQualify(select, select_list_end);

	Token *select_close = NULL;

	// Check for closing (SELECT ...) before checking for set (UNION i.e.)
	// Standalone (SELECT ... ) UNION (SELECT ...) is allowed but (SELECT ... UNION SELECT ...) not (standalone, not subquery)
	if(select_open != NULL)
		select_close = TOKEN_GETNEXT(')');

	// UNION ALL i.e. must go before ORDER BY and options that belong to the entire SELECT
	ParseSelectSetOperator(block_scope, select_scope);

	// Other clauses belong to the entire SELECT, not individual SELECT in set operators
	if(select_scope == SQL_SEL_SET_UNION)
		return true;

	Token *pre_order = GetLastToken();

	// ORDER BY
	ParseSelectOrderBy(&order);

	// Add row limitation that was defined in WHERE clause (rownum in Oracle)
	if(rowlimit > 0)
	{
		Token *last = GetLastToken();

		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			Append(last, "\nLIMIT ", L"\nLIMIT", 7, select); 
			Append(last, rowlimit);
		}
	}

	ParseSelectOptions(select, from_end, where_, order, &rowlimit_soptions, &rowlimit);

	// Informix INTO TEMP | SCRATCH table
	bool into_temp = ParseInformixIntoTable(select);

	// No assignment and it is a result set for outer SELECT in a procedure
	if(into == false && into_temp == false && select_scope == 0 && block_scope == SQL_BLOCK_PROC)
	{
		// For Oracle add OPEN cur FOR
		if(_target == SQL_ORACLE)
		{
			TokenStr *cursor = new TokenStr();

			GenerateResultSetCursorName(cursor);

			Prepend(select, "OPEN ", L"OPEN ", 5);
			PrependNoFormat(select, cursor);
			Prepend(select, " FOR ", L" FOR ", 5);

			_spl_result_set_generated_cursors.Add(cursor);
		}

		_spl_result_sets++;

		if(result_sets != NULL)
			*result_sets = (*result_sets) + 1;
	}
	else 
	// Assignment and without FROM
	if(into == true && from == NULL)
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			Token::Change(select, "SET", L"SET", 3);
	}

	// Complete LIST aggregate function for SQL Server
	if(agg_list_func == true && _target == SQL_SQL_SERVER)
		Append(GetLastToken(), " FOR XML PATH('')), 1, 1, '')", L" FOR XML PATH('')), 1, 1, '')", 29, select); 

	// Convert row limits
	if(rowlimit_slist != NULL || rowlimit_soptions != NULL || rowlimit > 0)
		SelectConvertRowlimit(select, from, from_end, where_, where_end, pre_order, order, rowlimit_slist, rowlimit_soptions, rowlimit, rowlimit_percent);

	// Implement CONTINUE handler for NOT FOUND in Oracle
	if(_target == SQL_ORACLE && into == true && agg_func == false)
		OracleContinueHandlerForSelectInto(select);

	// Sybase ADS nested UDF call without variable assinment SELECT udf(params) FROM System.iota (standalone SELECT)
	if(_source == SQL_SYBASE_ADS && _spl_scope == SQL_SCOPE_FUNC && select_scope == 0 && !into &&
		from != NULL && from->IsRemoved() && out_cols != NULL && out_cols->GetCount() == 1)
		SybaseAdsSelectNestedUdfCall(select, out_cols);

	// Add statement delimiter if not set when source is SQL Server
	if(select_scope == 0)
	{
		SqlServerAddStmtDelimiter();

		// If there are no non-declare statements above, set last declare
		if(_spl_first_non_declare == NULL)
			_spl_first_non_declare = select;
	}

	if(select_open != NULL && select_close == NULL)
		select_close = TOKEN_GETNEXT(')');

	if(select_scope == 0)
	{
		STATS_SET_DESC(SQL_STMT_SELECT_DESC)
		STMS_STATS(select);
	}

	Leave(SQL_SCOPE_SELECT_STMT);

	if(appended_subquery_aliases != NULL)
		*appended_subquery_aliases = app_subq_aliases;

	if(from_end_out != NULL)
		*from_end_out = from_end;

	if(where_end_out != NULL)
		*where_end_out = where_end;

	return true;
}

// Parse subquery
bool SqlParser::ParseSubSelect(Token *open, int select_scope)
{
	if(open == NULL)
		return false;

	bool exists = false;

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	// One more level - ((SELECT ...) EXCEPT (SELECT ...))
	if(next->Compare('(', L'(') == true)
	{
		exists = ParseSubSelect(next, select_scope);

		// Make sure it was SELECT since ((t1 JOIN ... is also possible
		if(exists == true)
		{
			// Nested )
			/*Token *close */ (void) GetNextCharToken(')', L')');

			// (SELECT ...) EXCEPT SELECT is possible
			ParseSelectSetOperator(0, 0);
		}
	}
	else
	// SELECT statement
	if(next->Compare("SELECT", L"SELECT", 6) == true)
	{
		exists = ParseSelectStatement(next, 0, select_scope, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	// Not a select statement
	if(exists == false)
		PushBack(next);

	return exists; 
}

// Returns SELECT or WITH if it is the next input token
Token* SqlParser::GetNextSelectStartKeyword()
{
	// Any SELECT can be optionally enclosed with ()
	Token *open = TOKEN_GETNEXT('(');

	Token *select = GetNext("SELECT", L"SELECT", 6);

	Token *sel = NULL;

	// SEL in Teradata
	if(select == NULL && _source == SQL_TERADATA)
	{
		sel = GetNext("SEL", L"SEL", 3);

		if(sel != NULL)
		{
			// Use SELECT in other databases
			if(_target != SQL_TERADATA)
				Token::Change(sel, "SELECT", L"SELECT", 6);
		}
	}

	Token *with = NULL;

	if(select == NULL && sel == NULL)
		with = GetNext("WITH", L"WITH", 4);

	Token *select_keyword = Nvl(select, sel, with);

	if(open != NULL)
	{
		if(select_keyword != NULL)
		{
			PushBack(select_keyword);
			return open;
		}
		else
			PushBack(open);
	}

	return select_keyword;
}

// Common table expression
bool SqlParser::ParseSelectCteClause(Token *with)
{
	if(with == NULL)
		return false;

	// Comma-separated list of table expressions
	while(true)
	{
		Token *table = GetNextToken();

		if(table == NULL)
			break;

		/*Token *open */ (void) GetNextCharToken('(', L'(');

		// Comma-separated list of colums
		while(true)
		{
			Token *column = GetNextToken();

			if(column == NULL)
				break;

			Token *comma = GetNextCharToken(',', L',');

			if(comma == NULL)
				break;
		}

		/*Token *close */ (void) GetNextCharToken(')', L')');

		// AS keyword
		/*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

		/*Token *open2 */ (void) GetNextCharToken('(', L'(');

		// SELECT statement
		Token *select = GetNextSelectStartKeyword();

		if(select == NULL)
			break;

		ParseSelectStatement(select, 0, SQL_SEL_WITH_CTE, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

		/*Token *close2 */ (void) GetNextCharToken(')', L')');

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	return true;
}

// Select list
bool SqlParser::ParseSelectList(Token *select, int select_scope, bool *select_into, bool *dummy_not_required,
								bool *agg_func, bool *agg_list_func, 
								ListW *exp_starts, ListW *out_cols, ListW *into_cols, 
								Token **rowlimit_slist, bool *rowlimit_percent)
{
	bool into_exists = false;
	bool dummy_not_req = false;
	bool agg_func_exists = false;
	bool agg_list_func_exists = false;

	// Parse predicate DISTINCT, TOP, FIRST etc.
	ParseSelectListPredicate(rowlimit_slist, rowlimit_percent);

	ListW cols;
	ListWM tsql_assignments;

	bool tsql_assign_exists = false;
	bool tsql_assign_subselect = false;

	while(true)
	{
		// Column, expression, (subquery), (subquery) + exp and other complex expressions can be specified  
		Token *first = GetNextToken();

		if(first == NULL)
			break;

		// Add the first position to the list
		cols.Add(first);

		if(exp_starts != NULL)
			exp_starts->Add(first);

		ParseExpression(first);

		Token *first_end = GetLastToken();

		// Standalone column name 
		if(first == first_end && first->type == TOKEN_IDENT)
			first->subtype = TOKEN_SUB_COLUMN_NAME;

		// Check for an aggregate function
		if(IsAggregateFunction(first) == true)
			agg_func_exists = true;

		// Check for LIST aggregate function
		if(IsListAggregateFunction(first) == true)
			agg_list_func_exists = true;
		
		// Token following the expression
		Token *second = GetNextToken();

		// AS keyword between column and alias
		if(Token::Compare(second, "AS", L"AS", 2) == true)
			second = GetNextToken();
        else
        // (NAMED alias) in Teradata
        if(Token::Compare(second, '(', L'(') == true)
        {
            Token *named = TOKEN_GETNEXTW("NAMED");

            if(named != NULL)
            {
                /*Token *alias */ (void) GetNext();
                Token *close = TOKEN_GETNEXT(')');

                // Change to AS for other databases
                if(_target != SQL_TERADATA)
                {
                    TOKEN_CHANGE(named, "AS");
                    Token::Remove(second);
                    Token::Remove(close);
                }

                second = GetNextToken();
            }
        }

		if(second == NULL)
			break;

		// For SQL Server and Sybase = means INTO clause
		if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && second->Compare('=', L'=') == true)
		{
			// SELECT @v = (SELECT ...), @v2 = (SELECT ...) without or with further FROM can be used
			// in SQL Server
			Token *open = GetNextCharToken('(', L'(');

			Token *select = GetNextWordToken(open, "SELECT", L"SELECT", 6);

			if(select != NULL)
			{
				Token *list_end = NULL;

				ParseSelectStatement(select, 0, SQL_SEL_SELECT_LIST, NULL, &list_end, NULL, NULL, NULL, NULL, NULL, NULL);

				Token *close = GetNextCharToken(open, ')', L')');

				tsql_assignments.Add(first, second, open, list_end, close);
				tsql_assign_subselect = true;
			}
			else
			{
				// Save @var = 
				tsql_assignments.Add(first, second);

				second = GetNextToken();
			
				// Parse assignment expression
				ParseExpression(second);

				/*Token *close */ (void) GetNextCharToken(open, ')', L')');
			}

			tsql_assign_exists = true;
			into_exists = true;

			// Get next token after expression, should be a comma, alias, or FROM etc.
			second = GetNextToken();

			if(second == NULL)
				break;
		}

		// If comma follows the expression parse next
		if(second->Compare(',', L',') == true)
		{
			// No alias specified, so the result set name is equal to column name
			if(out_cols != NULL)
				out_cols->Add(first);

			continue;
		}

		// If INTO or FROM keyword, invalid alias or ; is next, end the select list
		if(second->Compare("INTO", L"INTO", 4) == true || second->Compare("FROM", L"FROM", 4) == true || 
			second->Compare(';', L';') == true || IsValidAlias(second) == false)
		{
			// No alias specified, so the result set name is equal to column name
			if(out_cols != NULL)
				out_cols->Add(first);

			PushBack(second);
			break;
		}

		// Assign alias as output column
		if(out_cols != NULL)
			out_cols->Add(second);

		// It was an alias, now must be a comma
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	Token *into = GetNextWordToken("INTO", L"INTO", 4);

	bool into_temp_table = false;

	// SELECT ... INTO temp_table WHERE ... syntax in SQL Server, Sybase ASE, Sybase ADS
	if(into != NULL && Source(SQL_SQL_SERVER, SQL_SYBASE, SQL_SYBASE_ADS))
	{
		Token *name = GetNextToken();

		if(Token::Compare(name, "#", L"#", 0, 1))
			into_temp_table = true;
	}

	// Parse into clause
	if(into != NULL && !into_temp_table)
	{
		ListwItem *cur_select = cols.GetFirst();

		// Process each variable
		while(true)
		{
			Token *name = GetNextToken();

			if(name == NULL)
				break;

			// Add @ for parameter names for SQL Server and Sybase
			if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
			{
				ConvertToTsqlVariable(name);

				Token *exp = (cur_select != NULL) ? (Token*)cur_select->value : NULL;

				// Use SQL Server @var = syntax if it is not Informix FOREACH SELECT INTO that converted to cursor loop
				if(select_scope != SQL_SEL_FOREACH)
				{
					PrependCopy(exp, name); 
					PrependNoFormat(exp, " = ", L" = ", 3); 
				}
			}

			if(into_cols != NULL)
				into_cols->Add(name);

			if(cur_select != NULL)
				cur_select = cur_select->next;

			Token *comma = GetNextCharToken(',', L',');

			if(comma == NULL || cur_select == NULL)
				break;
		}

		Token *last = GetLastToken();

		// For SQL Server, remove INTO clause
		if(_target == SQL_SQL_SERVER)
			Token::Remove(into, last);

		// Remove INTO clause in Informix FOR EACH statement when converted to Oracle, PostgreSQL FOR
		if(select_scope == SQL_SEL_FOREACH && Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
			Token::Remove(into, last);

		into_exists = true;
	}

	// SQL Server assignment SELECT @v = ...
	if(tsql_assign_exists == true)
	{
		Token *from = GetNextWordToken("FROM", L"FROM", 4);

		// Without FROM
		if(from == NULL)
		{
			// SELECT @v = (SELECT ...), @v2 = (SELECT ...) without FROM
			if(tsql_assign_subselect == true)
			{
				if(_target == SQL_ORACLE)
				{
					// Embed INTO to each SELECT statement inside
					for(ListwmItem *i = tsql_assignments.GetFirst(); i != NULL; i = i->next)
					{
						Token *var = (Token*)i->value;
						Token *equal = (Token*)i->value2;
						Token *open = (Token*)i->value3;
						Token *end_list = (Token*)i->value4;
						Token *close = (Token*)i->value5;

						Append(end_list, " INTO ", L" INTO ", 6, select);
						AppendCopy(end_list, var);

						Token::Remove(var, equal);
						Token::Remove(open);
						Token::Remove(close);
					}

					// Remove outer SELECT
					Token::Remove(select);

					// No outer FROM dual required since tables of inner SELECTs are used
					dummy_not_req = true;
				}			
			}
			// No subquery, convert to variable assignment
			else
			{
				if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
				{
					for(ListwmItem *i = tsql_assignments.GetFirst(); i != NULL; i = i->next)
					{
						Token *equal = (Token*)i->value2;
						PrependNoFormat(equal, ":", L":", 1);
					}

					Token::Remove(select);
				}
			}
		}
		// FROM clause exists, use SELECT INTO
		else
		{
			PushBack(from);

			// Use SELECT INTO
			if(Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
			{
				Token *append = GetLastToken();

				// Append INTO before FROM clause
				for(ListwmItem *i = tsql_assignments.GetFirst(); i != NULL; i = i->next)
				{
					Token *var = (Token*)i->value;
					Token *equal = (Token*)i->value2;
					
					if(i->prev == NULL)
						Append(append, " INTO ", L" INTO ", 6, select);
					else
						Append(append, ", ", L", ", 2, select);

					AppendCopy(append, var);
					Token::Remove(var, equal);
				}
			}
		}
	}			

	if(select_into != NULL)
		*select_into = into_exists;

	if(dummy_not_required != NULL)
		*dummy_not_required = dummy_not_req;

	if(agg_func != NULL)
		*agg_func = agg_func_exists;

	if(agg_list_func != NULL)
		*agg_list_func = agg_list_func_exists;

	return true;
}

// Parse predicate DISTINCT, TOP, FIRST etc.
bool SqlParser::ParseSelectListPredicate(Token **rowlimit_slist, bool *rowlimit_percent)
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// DISTINCT in all databases
		if(next->Compare("DISTINCT", L"DISTINCT", 8) == true)
		{
			exists = true;
			continue;
		}
		else
		// UNIQUE in Informix
		if(next->Compare("UNIQUE", L"UNIQUE", 6) == true)
		{
			// Use DISTINCT in other databases
			if(_target != SQL_INFORMIX)
				Token::Change(next, "DISTINCT", L"DISTINCT", 8);

			exists = true;
			continue;
		}
		else
		// TOP num [PERCENT] in SQL Server and Sybase (limit after sorting)
		if(next->Compare("TOP", L"TOP", 3) == true)
		{
			// Limit can be optionally in ()
			Token *open = GetNextCharToken('(', L'(');

			// Number
			Token *num = GetNextToken();

			Token *close = GetNextCharToken(')', L')');

			// Limit in percents can be specified
			Token *percent = GetNextWordToken("PERCENT", L"PERCENT", 7);

			if(percent != NULL && rowlimit_percent != NULL)
				*rowlimit_percent = true;

			if(rowlimit_slist != NULL)
				*rowlimit_slist = num;

			// ROWNUM <= num is used in Oracle
			if(_target == SQL_ORACLE)
			{
				Token::Remove(next, num);
				Token::Remove(open);
				Token::Remove(close);
				Token::Remove(percent);
			}

			exists = true;
			continue;
		}
		else
		// FIRST num in Informix and Sybase ASA (return first num rows after sorting)
		if(next->Compare("FIRST", L"FIRST", 5) == true)
		{
			// Make sure it is not FIRST(column) function in ASA that means 1 row and handled separately
			Token *open = GetNextCharToken('(', L'(');

			if(open != NULL)
			{
				PushBack(next);
				break;
			}

			// Number or variable allowed in Informix
			Token *num = GetNextToken();

			if(rowlimit_slist != NULL)
				*rowlimit_slist = num;

			// ROWNUM <= num is used in Oracle
			if(_target == SQL_ORACLE)
				Token::Remove(next, num);
			else
			// TOP n in SQL Server
			if(_target == SQL_SQL_SERVER)
			{
				Token::Change(next, "TOP", L"TOP", 3);

				// In Sybase ASA number can be omitted, but Informix also allows variable name
				if(num != NULL && num->IsNumeric() == false)
				{
					// If variable or parameter do not exists add TOP 1
					if(GetVariable(num) == NULL && GetParameter(num) == NULL)
					{
						AppendNoFormat(next, " 1", L" 1", 2);
						PushBack(num);
					}
				}
			}

			exists = true;
			continue;
		}

		// Not a predicate
		PushBack(next);
		break;
	}

	return exists;
}

// FROM clause
bool SqlParser::ParseSelectFromClause(Token *select, bool nested_from, Token **from_out, Token **from_end, 
									int *appended_subquery_aliases, bool dummy_not_required, ListWM *from_table_end)
{
	Token *from = NULL;

	if(nested_from == false)
	{
		from = GetNextWordToken("FROM", L"FROM", 4);

		// FROM clause is absent
		if(from == NULL)
		{
			Token *last = GetLastToken();

			if(dummy_not_required == false)
			{

				// For Oracle add FROM dual
				if(_target == SQL_ORACLE)
				{
					Append(last, " FROM ", L" FROM ", 6, select);
					AppendNoFormat(last, "dual", L"dual", 4);
				}
				else
				// For DB2 add sysibm.sysdummy1
				if(_target == SQL_DB2)
				{
					Append(last, " FROM ", L" FROM ", 6, select);
					AppendNoFormat(last, "sysibm.sysdummy1", L"sysibm.sysdummy1", 16);
				}
			}

			return true;
		}
	}

	bool dummy_exists = false;

	int count = 0;

	while(true)
	{
		bool subquery = false;

		// Subquery or nested join
		Token *first = GetNextCharToken('(', L'(');
		
		if(first != NULL)
		{
			bool subselect_exists = ParseSubSelect(first, SQL_SEL_FROM);

			// Parse nested join condition
			if(subselect_exists == false)
				ParseSelectFromClause(select, true, NULL, NULL, NULL, false, NULL);			

			Token *close = GetNextCharToken(')', L')');

			if(close != NULL)
				subquery = true;
		}
		// Table name
		else
		{
			// TABLE () table function
			Token *table_func = GetNextWordToken("TABLE", L"TABLE", 5);

			if(table_func != NULL)
			{
				/*Token *open */ (void) GetNextCharToken('(', L'(');

				// Parse the table function
				ParseExpression(GetNextToken());

				/*Token *close */ (void) GetNextCharToken(')', L')');
			}
			else
				// Table name
				first = GetNextIdentToken(SQL_IDENT_OBJECT);

			if(first == NULL && table_func == NULL)
				break;

			if(first != NULL)
			{
				// Check for Oracle dual
				if(first->Compare("dual", L"dual", 4) == true)
				{
					if(_target == SQL_NETEZZA)
						Token::Change(first, "_v_dual", L"_v_dual", 7);

					dummy_exists = true;
				}
				else
				// DB2 SYSIBM.SYSDUMMY1 (or SYSIBM.SYSDUMMY in case of typo)
				if(first->Compare("SYSIBM.SYSDUMMY1", L"SYSIBM.SYSDUMMY1", 16) == true ||
						first->Compare("SYSIBM.SYSDUMMY", L"SYSIBM.SYSDUMMY", 15) == true)
				{
					if(_target == SQL_ORACLE)
						Token::Change(first, "dual", L"dual", 4);

					dummy_exists = true;
				}
				else
				// Sybase ADS System.iota
				if(TOKEN_CMP(first, "SYSTEM.IOTA"))
				{
					if(_target == SQL_ORACLE)
						Token::Change(first, "dual", L"dual", 4);

					dummy_exists = true;
				}
			}
		}

		Token *first_end = GetLastToken();

		// Token following the table name (candidate for alias)
		Token *second = GetNextToken();

		// AS keyword between table name and alias
		if(Token::Compare(second, "AS", L"AS", 2) == true)
			second = GetNextToken();

		if(second == NULL)
		{
			// SQL Server requires an alias for subquery
			if(subquery == true && _target == SQL_SQL_SERVER)
				SqlServerAppendSubqueryAlias(first_end, appended_subquery_aliases);

			count++;
			break;
		}

		// If comma follows the expression parse next
		if(second->Compare(',', L',') == true)
		{
			// SQL Server requires an alias for subquery
			if(subquery == true && _target == SQL_SQL_SERVER)
				SqlServerAppendSubqueryAlias(first_end, appended_subquery_aliases);

			count++;
			continue;
		}

		// Check if the next is end of statement, subquery, or alias (not GO, WHERE, ORDER etc.)
		if(second->Compare(';', L';') == true || second->Compare(')', L')') == true || 
			IsValidAlias(second) == false)
		{
			// SQL Server requires an alias for subquery
			if(subquery == true && _target == SQL_SQL_SERVER)
				SqlServerAppendSubqueryAlias(first_end, appended_subquery_aliases);

			count++;

			// Table name without alias
			if(from_table_end != NULL)
				from_table_end->Add(first, first);

			PushBack(second);
			break;
		}

		// Second can point to alias or join keyword
		ParseJoinClause(first, second, subquery, from_table_end);

		count++;

		// Alias exists so comma must follow, otherwise FROM clause ended no matter what goes next
		if(GetNextCharToken(',', L',') == false)
			break;
	}

	// Delete FROM if only dummy table is used
	if(count == 1 && dummy_exists == true)
	{
		Token *last = GetLastToken();

		if(_target == SQL_SQL_SERVER) 
			Token::Remove(from, last);
		else
		// MySQL supports Oracle's dual
		if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_ORACLE)
			Token::Remove(from, last);		
	}

	if(from_out != NULL)
		*from_out = from;

	if(from_end != NULL)
		*from_end = GetLastToken();

	return true;
}

// Join clause in FROM clause of SELECT statement 
bool SqlParser::ParseJoinClause(Token *first, Token *second, bool first_is_subquery, ListWM *from_table_end)
{
	if(second == NULL)
		return false;

	bool exists = false;

	Token *left_right_full = NULL;
	Token *outer_inner = NULL;
	Token *join = NULL;
	Token *on = NULL;

	exists = GetJoinKeywords(second, &left_right_full, &outer_inner, &join);

	// Second points to alias, try to get the first join keyword
	if(exists == false)
	{
		if(!first_is_subquery && from_table_end != NULL)
			from_table_end->Add(second, first);

		Token *token = GetNext();

		exists = GetJoinKeywords(token, &left_right_full, &outer_inner, &join);

		if(!exists)
			PushBack(token);
	}

	if(join == NULL)
		return false;

	// Second table name, or (SELECT ...)
	Token *third = GetNextToken();

	if(third == NULL)
		return false;

	// Check for nested SELECT
	if(third->Compare('(', L'(') == true)
	{
		Token *select = GetNextSelectStartKeyword();

		if(select != NULL)
			ParseSelectStatement(select, 0, SQL_SEL_JOIN, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

		/*Token *close */ (void) GetNextCharToken(')', ')');
	}

	Token *fourth = NULL;

	// If ON does not follows, it means optional alias for the second table in join
	on = GetNextWordToken("ON", L"ON", 2);

	if(on == NULL)
	{
		// Alias
		fourth = GetNextIdentToken();

		on = GetNextWordToken("ON", L"ON", 2);
	}

	// Parse join condition
	if(on != NULL)
		ParseBooleanExpression(SQL_BOOL_JOIN_ON, NULL, NULL);

	// List of join conditions can be specified JOIN ON ... JOIN ON ...
	Token *next = GetNextToken();

	if(ParseJoinClause(NULL, next, false, NULL) == false)
		PushBack(next);

	return true;
}

// Get JOIN clause keywords
bool SqlParser::GetJoinKeywords(Token *token, Token **left_right_full, Token **outer_inner, Token **join)
{
	bool exists = false;

	// LEFT | RIGHT | FULL [OUTER] JOIN
	if(TOKEN_CMP(token, "LEFT") || TOKEN_CMP(token, "RIGHT") || TOKEN_CMP(token, "FULL"))
	{
		if(left_right_full != NULL)
			*left_right_full = token;

		Token *outer = TOKEN_GETNEXTW("OUTER");

		if(outer != NULL && outer_inner != NULL)
			*outer_inner = outer;

		Token *jn = TOKEN_GETNEXTW("JOIN");

		if(jn != NULL && join != NULL)
			*join = jn;

		exists = true;
	}
	else
	// INNER | OUTER JOIN
	if(TOKEN_CMP(token, "INNER") || TOKEN_CMP(token, "OUTER"))
	{
		if(outer_inner != NULL)
			*outer_inner = token;

		Token *jn = TOKEN_GETNEXTW("JOIN");

		if(jn != NULL && join != NULL)
			*join = jn;

		exists = true;
	}
	else
	// JOIN
	if(TOKEN_CMP(token, "JOIN"))
	{
		if(join != NULL)
			*join = token;

		exists = true;
	}

	return exists;
}

// WHERE clause in SELECT statement
bool SqlParser::ParseWhereClause(int stmt_scope, Token **where_out, Token **where_end_out, int *rowlimit)
{
	Token *where_ = GetNextWordToken("WHERE", L"WHERE", 5);

	if(where_ == NULL)
		return false;

	if(where_out != NULL)
		*where_out = where_;

	// WHERE CURRENT OF cursor
	if(ParseWhereCurrentOfCursor(stmt_scope) == true)
		return true;

	int count = 0;

	ParseBooleanExpression(SQL_BOOL_WHERE, where_, &count, rowlimit);

	// If Oracle rownum condition is set, and there are no conditions anymore remove WHERE
	if(rowlimit != NULL && *rowlimit > 0 && count == 0)
		Token::Remove(where_);

	if(where_end_out != NULL)
		*where_end_out = GetLastToken();
	
	return true;
}

// Oracle, DB2 WHERE CURRENT OF cursor
bool SqlParser::ParseWhereCurrentOfCursor(int stmt_scope)
{
	Token *current = GetNextWordToken("CURRENT", L"CURRENT", 7);

	if(current == NULL)
		return false;

	Token *of = GetNextWordToken("OF", L"OF", 2);
	Token *cursor = NULL;

	if(of != NULL)
		cursor = GetNextIdentToken();

	if(cursor == NULL)
	{
		PushBack(current);
		return false;
	}

	// Save updatable cursor
	if(stmt_scope == SQL_STMT_UPDATE || stmt_scope == SQL_STMT_DELETE)
		_spl_updatable_current_of_cursors.Add(cursor);

	return true;
}

// CONNECT BY in Oracle
bool SqlParser::ParseConnectBy()
{
	bool exists = false;

	// CONNECT BY and START WITH can go in any order
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// CONNECT BY
		if(TOKEN_CMP(next, "CONNECT"))
		{
			Token *by = TOKEN_GETNEXTW("BY");

			// [PRIOR] condition [AND [PRIOR] condition ...] clauses
			if(by != NULL)
			{
				// Optional PIOR
				/*Token *prior */ TOKEN_GETNEXTW("PRIOR");
				ParseBooleanExpression(SQL_BOOL_CONNECT_BY);
				exists = true;
			}

			continue;
		}
		else
		// START WITH
		if(TOKEN_CMP(next, "START"))
		{
			Token *with = TOKEN_GETNEXTW("WITH");

			// condition [AND condition ...] clauses
			if(with != NULL)
			{
				ParseBooleanExpression(SQL_BOOL_START_WITH);
				exists = true;
			}

			continue;
		}

		PushBack(next);
		break;
	}

	return exists;
}

// GROUP BY clause in SELECT statement
bool SqlParser::ParseSelectGroupBy()
{
	Token *group = GetNextWordToken("GROUP", L"GROUP", 5);

	if(group == NULL)
		return false;

	Token *by = GetNextWordToken("BY", L"BY", 2);

	if(by == NULL)
		return false;

	// Parse group expressions
	while(true)
	{
		Token *exp = GetNextToken();

		if(exp == NULL)
			break;

		// An GROUP BY expression 
		ParseExpression(exp);

		// Must be comma if next expression exists
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	return true;
}

// HAVING clause
bool SqlParser::ParseSelectHaving()
{
	Token *having = GetNextWordToken("HAVING", L"HAVING", 6);

	if(having == NULL)
		return false;

	ParseBooleanExpression(SQL_BOOL_HAVING);

	return true;
}

// QUALIFY clause in Teradata
bool SqlParser::ParseSelectQualify(Token *select, Token *select_list_end)
{
	Token *qualify = TOKEN_GETNEXTW("QUALIFY");

	if(qualify == NULL)
		return false;

    Token *exp = ParseExpression();
    Token *exp_end = GetLastToken();

    Token *op = TOKEN_GETNEXT('=');

    if(op == NULL)
        return false;

	/*Token *exp2 */ (void) ParseExpression();

    // Convert to SELECT * FROM (original SELECT) WHERE rn = exp2
    if(_target != SQL_TERADATA)
    {
        Prepend(select, "SELECT * FROM (", L"SELECT * FROM (", 15);
        Prepend(op, ") WHERE ", L") WHERE ", 8, qualify);
        PrependNoFormat(op, "rn ", L"rn ", 3);

        AppendNoFormat(select_list_end, ", ", L", ", 2);
        AppendCopy(select_list_end, exp, exp_end);
        AppendNoFormat(select_list_end, " rn", L" rn", 3);

        Token::Remove(qualify, exp_end);
    }

	return true;
}

// ORDER BY clause in SELECT statement
bool SqlParser::ParseSelectOrderBy(Token **order_out)
{
	Token *order = GetNextWordToken("ORDER", L"ORDER", 5);

	if(order == NULL)
		return false;

	Token *by = GetNextWordToken("BY", L"BY", 2);

	if(by == NULL)
		return false;

	// Parse order expressions
	while(true)
	{
		Token *exp = GetNextToken();

		if(exp == NULL)
			break;

		// An ORDER expression 
		ParseExpression(exp);

		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// ASC or DESC
		if(next->Compare("ASC", L"ASC", 3) == true || next->Compare("DESC", L"DESC", 4) == true)
			next = GetNextToken();

		// Must be comma
		if(Token::Compare(next, ',', L',') == false)
		{
			PushBack(next);
			break;
		}
	}

	if(order_out != NULL)
		*order_out = order;

	return true;
}

// UNION, UNION ALL, EXCEPT and MINUS operators
bool SqlParser::ParseSelectSetOperator(int block_scope, int /*select_scope*/)
{
	bool exists = false;

	while(true)
	{
		Token *op = GetNextToken();

		if(op == NULL)
			break;

		// UNION and UNION ALL
		if(op->Compare("UNION", L"UNION", 5) == true)
		{
			/*Token *all */ (void) GetNextWordToken("ALL", L"ALL", 3);

			exists = true;
			break;
		}
		else
		// EXCEPT in SQL Server, DB2
		if(op->Compare("EXCEPT", L"EXCEPT", 6) == true)
		{
			// MINUS in Oracle
			if(_target == SQL_ORACLE)
				Token::Change(op, "MINUS", L"MINUS", 5);

			exists = true;
			break;
		}
		else
		// MINUS in Oracle
		if(op->Compare("MINUS", L"MINUS", 5) == true)
		{
			exists = true;
			break;
		}

		PushBack(op);
		break;
	}

	// If a set operator exists next must a SELECT statement
	if(exists == true)
	{
		// SELECT or (SELECT ...) can be specified
		Token *open = GetNextCharToken('(', L'(');
		
		if(open != NULL)
		{
			ParseSubSelect(open, SQL_SEL_SET_UNION);

			/*Token *close */ (void) GetNextCharToken(')', L')');
		}
		else
		{
			Token *select = GetNextWordToken("SELECT", L"SELECT", 6);

			if(select != NULL)
				ParseSelectStatement(select, block_scope, SQL_SEL_SET_UNION, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL); 
		}
	}

	return exists;
}

// SELECT statements options at the end of the statement
bool SqlParser::ParseSelectOptions(Token * /*select*/, Token * /*from_end*/, Token * /*where_*/, Token * /*order*/, 
									Token **rowlimit_soptions, int *rowlimit)
{
	bool exists = false;

	while(true)
	{
		Token *option = GetNextToken();

		if(option == NULL)
			break;

		// Oracle, MySQL FOR UPDATE; DB2 FOR READ ONLY
		if(option->Compare("FOR", L"FOR", 3) == true)
		{
			Token *update = GetNextWordToken("UPDATE", L"UPDATE", 6);
			Token *read = NULL;

			if(update == NULL)
				read = GetNextWordToken("READ", L"READ", 4);

			if(update != NULL)
			{
				// OF column, ... clause
				Token *of = GetNextWordToken("OF", L"OF", 2);

				while(of != NULL)
				{
					Token *col = GetNextIdentToken();

					if(col == NULL)
						break;

					Token *comma = GetNextCharToken(',', L',');

					if(comma == NULL)
						break;
				}

				// Oracle SKIP LOCKED
				Token *skip = GetNextWordToken("SKIP", L"SKIP", 4);
				Token *locked = NULL;

				if(skip != NULL)
					locked = GetNextWordToken("LOCKED", L"LOCKED", 6);

				Token *nowait = NULL;

				// Oracle NOWAIT
				if(skip == NULL)
					nowait = TOKEN_GETNEXTW("NOWAIT");

				// MySQL does not support SKIP LOCKED, comment it
				if(Target(SQL_MARIADB, SQL_MYSQL) && skip != NULL && locked != NULL)
					Comment(skip, locked);
			}
			else
			if(read != NULL)
			{
				Token *only = GetNextWordToken("ONLY", L"ONLY", 4);

				if(only != NULL && Target(SQL_SQL_SERVER))
					Token::Remove(option, only);
			}			

			exists = true;
			continue;
		}
		else
		// DB2 WITH UR, CS, RS, RR option
		if(option->Compare("WITH", L"WITH", 4) == true)
		{
			Token *value = GetNextToken();

			if(Token::Compare(value, "UR", L"UR", 2) || Token::Compare(value, "CS", L"CS", 2) ||
				Token::Compare(value, "RS", L"RS", 2) ||Token::Compare(value, "RR", L"RR", 2))
			{
				if(!Target(SQL_DB2))
					Token::Remove(option, value);

				exists = true;
				continue;
			}
			else
			{
				PushBack(option);
				break;
			}
		}
		else
		// DB2 FETCH FIRST n ROWS ONLY
		if(ParseDb2FetchFirstRowOnly(option, rowlimit_soptions, rowlimit) == true)
		{
			exists = true;
			continue;
		}
		else
		// MySQL LIMIT num option
		if(option->Compare("LIMIT", L"LIMIT", 5) == true)
		{
			Token *num = GetNextToken();

			// ROWNUM is used in Oracle
			if(Target(SQL_ORACLE) == true && num != NULL)
				Token::Remove(option, num);

			if(rowlimit_soptions != NULL)
				*rowlimit_soptions = num;

			exists = true;
			continue;
		}

		PushBack(option);
		break;
	}

	return exists;
}

// DB2 VALUES statement
bool SqlParser::ParseValuesStatement(Token *values, int *result_sets)
{
	if(values == NULL)
		return false;

	// Check for VALUES NEXTVAL seq INTO var
	if(Db2ValuesNextValIntoPattern(values) == true)
		return true;

	int rows = 1;

	ListWM exp;

	// Multiple rows can be specified
	while(true)
	{
		// For single row column values optionally enclosed in (), for multirow () are required
		Token *open = GetNextCharToken('(', L'(');

		// Multiple column expressions can be specified
		while(true)
		{
			// Column expression
			Token *col = GetNextToken();

			if(col == NULL)
				break;

			ParseExpression(col);

			Token *comma = GetNextCharToken(',', L',');

			exp.Add(col, comma);

			if(comma == NULL)
				break;
		}

		Token *close = GetNextCharToken(')', L')');

		if(_target == SQL_ORACLE)
		{
			// Avoid merge in case of VALUES(c1...
			Token::Change(open, " ", L" ", 1);
			Token::Change(close, " ", L" ", 1);
		}

		// Next row
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		rows++;
	}

	// Optional INTO keyword that means assignment
	Token *into = GetNextWordToken("INTO", L"INTO", 4);

	// Result set 
	if(into == NULL)
	{
		// Change to SELECT 
		if(_target != SQL_DB2)
			Token::Change(values, "SELECT", L"SELECT", 6);

		// For Oracle append FROM dual
		if(_target == SQL_ORACLE)
		{
			Token *last = GetLastToken();

			if(last != NULL)
				Append(last, " FROM dual", L" FROM dual", 10);
		}

		if(result_sets != NULL)
			*result_sets = (*result_sets)++;
	}
	// Assignment
	else
	{
		ListwmItem *e = exp.GetFirst();

		// List of variables
		while(true)
		{
			// Variable
			Token *var = GetNextToken();
			
			// Assignment expression
			Token *exp = (e != NULL) ? (Token*)e->value : NULL;
			Token *exp_comma = (e != NULL) ? (Token*)e->value2 : NULL;

			Token *comma = GetNextCharToken(',', L',');

			// Convert to assignment statements
			if(_target == SQL_ORACLE)
			{
				PrependCopy(exp, var);
				PrependNoFormat(exp, " := ", L" := ", 4);

				Token::Change(exp_comma, ";", L";", 1);

				Token::Remove(var);
				Token::Remove(comma);
			}

			if(comma == NULL)
				break;

			if(e != NULL)
				e = e->next; 
		}

		Token::Remove(values);
		Token::Remove(into);
	}

	return true;
}

// Resolve data types for columns in select list
void SqlParser::SelectSetOutColsDataTypes(ListW *out_cols, ListWM *from_table_end)
{
	if(out_cols == NULL || from_table_end == NULL)
		return;

	ListwItem *col_item = out_cols->GetFirst();

	while(col_item != NULL)
	{
		Token *col = (Token*)col_item->value;

		if(col == NULL)
			break;

		// Standalone column name
		if(col->subtype == TOKEN_SUB_COLUMN_NAME)
		{
			ListwmItem *from_item = from_table_end->GetFirst();

			// Find a table containing this column, and define its data type
			while(from_item != NULL)
			{
				Token *table = (Token*)from_item->value2;

				col->datatype_meta = GetMetaType(table, col);

				if(col->datatype_meta != NULL)
				{
					col->table = table;
					break;
				}

				from_item = from_item->next;
			}
		}
		else
		// All columns selected from single table
		if(TOKEN_CMPC(col, '*') && from_table_end->GetCount() == 1)
		{
			// Save the table name
			col->table = (Token*)from_table_end->GetFirst()->value2;
		}

		col_item = col_item->next;
	}
}

// Convert row limits specified in SELECT
void SqlParser::SelectConvertRowlimit(Token *select, Token *from, Token *from_end, Token *where_, 
										Token *where_end, Token *pre_order, Token *order, 
										Token *rowlimit_slist, Token *rowlimit_soptions, int rowlimit, 
										bool rowlimit_percent)
{
	// One of limits should be set
	if(rowlimit_slist == NULL && rowlimit_soptions == NULL && rowlimit == 0)
		return;

	int limit = rowlimit;

	if(rowlimit_slist != NULL)
		limit = rowlimit_slist->GetInt();
	else
	if(rowlimit_soptions != NULL)
		limit = rowlimit_soptions->GetInt();

	// 100 percent
	if(limit == 100 && rowlimit_percent == true)
		return;

	Token *end = GetLastToken();

	// ROWNUM in Oracle
	if(_source != SQL_ORACLE && _target == SQL_ORACLE)
	{
		// No ORDER BY, just add a condition to WHERE
		if(order == NULL)
		{
			// No WHERE clause too
			if(where_ == NULL)
				end = Append(from_end, " WHERE ", L" WHERE ", 7, select);
			else
				end = Append(where_end, " AND ", L" AND ", 5, where_);
		}
		// ORDER BY exists so a subquery must be used
		else		
		{
			Prepend(select, "SELECT * FROM (", L"SELECT * FROM (", 15);
			Append(end, ") WHERE ", L") WHERE ", 8, select);
		}
		
		AppendNoFormat(end, "rownum <= ", L"rownum <= ", 10);

		if(rowlimit_slist != NULL || rowlimit_soptions != NULL)
			AppendCopy(end, Nvl(rowlimit_slist, rowlimit_soptions));
		else
			Append(end, rowlimit);

		// Row limit in percent
		if(rowlimit_percent == true)
		{
			Append(end, "/100 * ", L"/100 * ", 7);
			Append(end, "\n (SELECT COUNT(*) ", L"\n (SELECT COUNT(*) ", 19, select);

			// Copy FROM, WHERE, UNION, GROUP BY, HAVING COUNT, but not ORDER BY
			AppendCopy(end, from, pre_order);

			Append(end, ")", L")", 1);
		}
	}
}