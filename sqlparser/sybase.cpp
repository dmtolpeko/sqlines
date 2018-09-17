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

// Sybase conversions

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Sybase ADS EXECUTE PROCEDURE statement 
bool SqlParser::ParseSybaseExecuteProcedureStatement(Token * /*execute*/)
{
	Token *procedure = TOKEN_GETNEXTW("PROCEDURE");

	if(procedure == NULL)
		return false;

	// Procedure name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	// List of parameters, empty () when there are no params
	Token *open = TOKEN_GETNEXT('(');
	Token *close = NULL;

	if(open != NULL)
	{
		while(true)
		{
			Token *param = GetNextToken();

			if(param == NULL)
				break;

			// () can be specified
			if(param->Compare(')', L')') == true)
			{
				PushBack(param);
				break;
			}

			ParseExpression(param);

			// Next parameter
			Token *comma = TOKEN_GETNEXT(',');

			if(comma == NULL)
				break;
		}

		close = TOKEN_GETNEXT(')');
	}

	// SQL Server does not allow ( and )
	if(_target == SQL_SQL_SERVER)
	{
		Token::Remove(open);
		Token::Remove(close);
	}

	if(_target != SQL_SYBASE_ADS)
		Token::Remove(procedure);

	return true;
}

// Sybase ADS WHILE FETCH cur DO loop
bool SqlParser::ParseSybaseWhileFetchStatement(Token *while_, Token *fetch, int scope)
{
	// Curson name (it must be already declared)
	Token *cursor = GetNextIdentToken();

	if(cursor == NULL)
		return false;

	// DO keyword starts the block
	Token *do_ = TOKEN_GETNEXTW("DO");

	if(_target == SQL_SQL_SERVER)
	{
		TOKEN_CHANGE(do_, "BEGIN");

		// FETCH the first row
		PREPEND(while_, "FETCH ");
		PrependCopy(while_, cursor);
		PREPEND(while_, " INTO;\n");

		TOKEN_CHANGE(fetch, "@@FETCH_STATUS=0");		
	}
	
	ParseBlock(SQL_BLOCK_WHILE, true, scope, NULL);

	Token *end = TOKEN_GETNEXTW("END");
	Token *end_while = (end != NULL) ? TOKEN_GETNEXTW("WHILE") : NULL;

	if(end != NULL)
	{
		if(_target == SQL_SQL_SERVER)
		{
			// Fetch the next row at the end of loop
			PREPEND(end, "FETCH ");
			PrependCopy(end, cursor);
			PREPEND(end, " INTO;\n");

			Token::Remove(cursor);
			Token::Remove(end_while);
		}		
	}

	return true;
}


// SYBASE ASE CREATE INDEX storage options
bool SqlParser::ParseSybaseCreateIndexOptions()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// SYBASE WITH options
		if(next->Compare("WITH", L"WITH", 4) == true)			
		{
			Token *keyword = GetNextToken();
			if(keyword->Compare("MAX_ROWS_PER_PAGE", L"MAX_ROWS_PER_PAGE", 17) == true) {
				Token *set = GetNextToken();

				/*Token *equal */ (void) GetNext(set, '=', L'=');

				// Row count
				Token *rows = GetNext(set);

				if(_target != SQL_SYBASE)
					Token::Remove(next, rows);
				exists = true;
				continue;						
			} else if (keyword->Compare("ALLOW_DUP_ROW", L"ALLOW_DUP_ROW", 13) == true) {
				if(_target != SQL_SYBASE)
					Token::Remove(next, keyword);
				exists = true;
				continue;
			}
		}

		// SYBASE ON <SEGMENT> option
		if(next->Compare("ON", L"ON", 2) == true)
		{
			Token *value = GetNextToken();			

			if(value != NULL)
			{
				if(_target != SQL_SYBASE)
					Token::Remove(next, value);

				exists = true;
				continue;
			}
		}


		// Not an index clause
		PushBack(next);

		break;
	}
	return exists;
}


