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

// SQLParser for SQL patterns

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Try to recognize well-known patterns in Oracle trigger
bool SqlParser::ParseCreateTriggerOraclePattern(Token *create, Token *table, Token *when, Token *insert)
{
	bool exists = false;

	if(_target == SQL_SQL_SERVER)
	{
		// If the trigger is before INSERT, check for sequence generation pattern
		if(when->Compare("BEFORE", L"BEFORE", 6) == true && insert != NULL)
		{
			Token *sequence = NULL;
			Token *column = NULL;

			// Try to recognize SELECT seq.NEXTVAL INTO :new.id FROM dual
			exists = SelectNextvalFromDual(&sequence, &column);

			// Replace trigger body with DEFAULT in SQL Server
			if(exists == true)
			{
				Prepend(create, "ALTER TABLE ", L"ALTER TABLE ", 12);
				PrependCopy(create, table);
				Prepend(create, " ADD DEFAULT NEXT VALUE FOR ", L"ADD DEFAULT NEXT VALUE FOR ", 28);
				PrependCopy(create, sequence);
				Prepend(create, " FOR ", L" FOR ", 5);
				PrependCopy(create, column);
				Prepend(create, ";\nGO", L";\nGO", 4);
			}
		}
	}

	return exists;
}

// SELECT seq.NEXTVAL INTO :new.col FROM dual in Oracle
bool SqlParser::SelectNextvalFromDual(Token **sequence, Token **column)
{
	Token *select = GetNextWordToken("SELECT", L"SELECT", 6);

	if(select == NULL)
		return false;

	// Sequence name with NEXTVAL pseudocolumn
	Token *seq = GetNextIdentToken();

	// INTO clause
	Token *into = GetNextWordToken("INTO", L"INTO", 4);

	if(into == NULL)
		return false;

	// Column name possible with :NEW
	Token *col = GetNextIdentToken();

	// FROM dual
	/*Token *from */ (void) GetNextWordToken("FROM", L"FROM", 4);
	Token *dual = GetNextWordToken("dual", L"dual", 4);
	/*Token *semi1 */ (void) GetNextCharToken(';', L';');

	if(dual == NULL)
		return false;

	// END must follow SELECT
	Token *end = GetNextWordToken("END", L"END", 3);
	
	bool exists = false;

	// SELECT NEXTVAL FROM dual pattern matched
	if(end != NULL)
	{
		PushBack(end);

		if(sequence != NULL)
			*sequence = seq;

		if(column != NULL)
			*column = col;

		exists = true;
	}

	return exists;
}

// Patterns in (SELECT ...) expression
bool SqlParser::ParseSelectExpressionPattern(Token *open, Token *select)
{
	if(open == NULL || select == NULL)
		return false;

	bool exists = false;

	// Sybase ADS procedure parameters (SELECT param FROM __input)
	if(_source == SQL_SYBASE_ADS && _spl_scope == SQL_SCOPE_PROC)
	{
		Token *param = GetNext();
		Token *declared_param = (param != NULL) ? GetParameter(param) : NULL;
		
		if(declared_param != NULL)
		{
			Token *from = TOKEN_GETNEXTW("FROM");
			Token *input = (from != NULL)? TOKEN_GETNEXTW("__INPUT") : NULL;
			Token *close = (input != NULL)? TOKEN_GETNEXT(')') : NULL;

			// Pattern matched
			if(close != NULL)
			{
				// Leave only parameter name
				if(_target != SQL_SYBASE_ADS)
				{
					PrependCopy(open, declared_param);
					Token::Remove(open, close);
				}

				exists = true;
			}
		}
	}

	return exists;
}

// PostgreSQL CAST(TIMEOFDAY() AS TIMESTAMP) that returns current date and time
bool SqlParser::ParseCastTimeofdayAsTimestamp(Token *cast, Token *open, Token *first)
{
	if(cast == NULL || open == NULL || first == NULL)
		return false;

	// TIMEOFDAY()
	if(first->Compare("TIMEOFDAY", L"TIMEOFDAY", 9) == false)
		return false;

	Token *open2 = GetNextCharToken('(', L'(');
	
	Token *close2 = NULL;
	Token *as = NULL; 
	Token *timestamp = NULL;
	Token *close = NULL;

	if(open2 != NULL)
		close2 = GetNextCharToken(')', ')');

	// AS TIMESTAMP
	if(close2 != NULL)
		as = GetNextWordToken("AS", L"AS", 2);

	if(as != NULL)
		timestamp = GetNextWordToken("TIMESTAMP", L"TIMESTAMP", 9);

	if(timestamp != NULL)
		close = GetNextCharToken(')', L')');

	// Pattern not matched
	if(close == NULL)
	{
		// Rollback tokens until open (
		PushBack(open2);
		return false;
	}

	// SYSTIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(cast, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);
		Token::Remove(open, close);
	}
	
	return true;
}

// Typical error check boolean expressions involving = operator
bool SqlParser::ParseBooleanErrorCheckPattern()
{
	if(Source(SQL_DB2, SQL_TERADATA) == false || _target != SQL_ORACLE)
		return false;

	bool exists = false;

	Token *sqlstate = GetNextWordToken("SQLSTATE", L"SQLSTATE", 8);
	
	if(sqlstate == NULL)
		return false;

	Token *equal = GetNextCharToken('=', L'=');
	Token *value = GetNextToken(equal);

	if(value == NULL)
	{
		PushBack(sqlstate);
		return false;
	}

	// Row not found condition
	if(Source(SQL_DB2, SQL_TERADATA) == true && sqlstate != NULL && 
		value->Compare("'02000'", L"'02000'", 7) == true)
	{
		if(_target == SQL_ORACLE)
		{
			// If specific cursor was fetched its %NOTFOUND attribute must be checked
			if(_spl_last_fetch_cursor_name != NULL)
			{
				PrependCopy(sqlstate, _spl_last_fetch_cursor_name);
				Token::Change(sqlstate, "%NOTFOUND", L"%NOTFOUND", 9);
			}
			else
				// SQL%NOTFOUND can be used for implicit cursors only
				Token::Change(sqlstate, "SQL%NOTFOUND", L"SQL%NOTFOUND", 12);

			Token::Remove(equal, value);
			exists = true;
		}
	}
	else
		PushBack(sqlstate);

	return exists;
}

// Typical cursor check - @@FETCH_STATUS = 0 in SQL Server, @@SQLSTATUS = 0 in Sybase
bool SqlParser::ParseBooleanCursorCheckPattern()
{
	if(!Source(SQL_SQL_SERVER, SQL_SYBASE) || !Target(SQL_ORACLE, SQL_MYSQL, SQL_MARIADB, SQL_POSTGRESQL))
		return false;

	bool exists = false;

	// @@FETCH_STATUS = 0 in SQL Server, @@SQLSTATUS = 0 in Sybase ASE
	// Also check @@SQLSTATUS != 2 that means no rows found in Sybase ASE
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true)
	{
		// Optionally enclosed
		Token *open = GetNext('(', L'(');

		Token *fetch_status = GetNext("@@FETCH_STATUS", L"@@FETCH_STATUS", 14);
		Token *sqlstatus = NULL;

		if(fetch_status == NULL)
			sqlstatus = GetNext("@@SQLSTATUS", L"@@SQLSTATUS", 11);		

		Token *status = Nvl(fetch_status, sqlstatus);

		if(status != NULL)
		{
			Token *equal = GetNext('=', L'=');
			Token *zero = GetNext(equal, "0", L"0", 1);

			Token *not_ = NULL;
			Token *two = NULL;

			bool row_found = false;

			// Check the same functionality taken using non-equality operator 
			if(equal == NULL && zero == NULL)
			{
				not_ = GetNext('!', L'!');
				equal = GetNext(not_, '=', L'=');
				two = GetNext(equal, "2", L"2", 1);

				// @@sqlstatus != 2 in Sybase ASE
				if(sqlstatus != NULL && two != NULL)
					row_found = true;
			}
			else
				row_found = true;

			// Fetch was successful, a row returned
			if(row_found)
			{
				if(open != NULL)
					GetNext(')', L')');

				if(_target == SQL_ORACLE)
				{
					// If specific cursor was fetched its %FOUND attribute must be checked
					if(_spl_last_fetch_cursor_name != NULL)
					{
						PrependCopy(status, _spl_last_fetch_cursor_name);
						Token::Change(status, "%FOUND", L"%FOUND", 6);
					}
					else
					// Use last open cursor name if fetched name was not found
					if(_spl_last_open_cursor_name != NULL)
					{
						PrependCopy(status, _spl_last_open_cursor_name);
						Token::Change(status, "%FOUND", L"%FOUND", 6);
					}

					Token::Remove(equal, zero);
					exists = true;
				}
				else
				if(Target(SQL_MYSQL, SQL_MARIADB))
				{
					TOKEN_CHANGE(status, "NOT_FOUND");

					// @@sqlstatus != 2 in Sybase ASE
					if(sqlstatus != NULL && not_ != NULL && two != NULL)
					{
						TOKEN_CHANGE(two, "0");
						Token::Remove(not_);
					}

					// Add NOT FOUND handler and variable later as it must go after all variable and cursor declarations
					_spl_need_not_found_handler = true;

					exists = true;
				}
				else
				if(_target == SQL_POSTGRESQL)
				{
					Token::Change(status, "FOUND", L"FOUND", 5);

					Token::Remove(equal, zero);
					exists = true;
				}
			}
			else
			{
				PushBack(status);
				PushBack(open);
			}
		}
		else
			PushBack(open);
	}

	return exists;
}

// OPEN WHILE(SQLCODE = 0) DO FETCH pattern in Teradata
// WHILE @@FETCH_STATUS = 0 pattern in SQL Server
bool SqlParser::PatternSqlCodeCursorLoop(Token *cursor_name)
{
	if(cursor_name == NULL)
		return false;

	Token *while_ = GetNextWordToken("WHILE", L"WHILE", 5);

	if(while_ == NULL)
		return false;

	Token *open = GetNextCharToken('(', L'(');

	// SQLCODE = 0 DO condition
	Token *sqlcode = GetNextWordToken("SQLCODE", L"SQLCODE", 7);

	Token *equal = GetNextCharToken(sqlcode, '=', L'=');
	Token *zero = GetNextToken(equal);

	Token *close = GetNextCharToken(open, ')', L')');

	Token *do_ = GetNextWordToken(zero, "DO", L"DO", 2);

	// FETCH statement follows next 
	Token *fetch = GetNextWordToken(do_, "FETCH", L"FETCH", 5);
	Token *fetch_end = NULL;

	if(fetch == NULL)
	{
		PushBack(while_);
		return false;
	}

	if(ParseFetchStatement(fetch) == true)
		fetch_end = GetNextCharOrLastToken(';', L';');

	// Parse remaining part of the WHILE statement
	ParseBlock(SQL_BLOCK_WHILE, true, 0, NULL);

	// END clause
	ParseWhileStatementEnd();

	// LOOP FETCH EXIT WHEN cur%NOTFOUND in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(while_, "LOOP", L"LOOP", 4);

		Append(fetch_end, "\nEXIT WHEN ", L"\nEXIT WHEN ", 11, fetch); 
		AppendCopy(fetch_end, cursor_name);
		Append(fetch_end, "%NOTFOUND;", L"%NOTFOUND;", 10, fetch); 

		Token::Remove(sqlcode, do_);
		Token::Remove(open);
		Token::Remove(close);
	}

	return true;
}
