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

// PostgreSQL specific functions

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Transform format string with parameters to concatenation expression
void SqlParser::PostgresFormatToConcat(Token *format, ListW &params)
{
	if((format == NULL) || ((format->str == NULL) && (format->len > 0)))
		return;

	ListwItem *list = params.GetFirst();

	TokenStr str("'", L"'", 1);

	int64_t num = 0;

	for(int64_t i = 1; i < format->len - 1; i++)
	{
		// Next placeholder % matched
		if(format->Compare('%', L'%', i) == true)
		{
			// Prepend the string before placeholder
			if(str.len > 1)
			{
				str.Append("' || ", L"' || ", 5); 

				if(list != NULL)
					PrependNoFormat((Token*)list->value, str.str.c_str(), str.wstr.c_str(), str.len);  
			}

			str.Set(" || '", L" || '", 5);
			num++;

			// List can be already empty but trail values still exist
			if(list != NULL)
				list = list->next;
			
			continue;
		}

		str.Append(format, i, 1);
	}

	// For not PostgreSQL, remove format if a % placeholder exists
	if(num > 0 && Target(SQL_POSTGRESQL) == false)
	{
		// Append the trail string
		if(str.len > 5)
		{
			ListwItem *last = params.GetLast();

			str.Append("'", L"'", 1);

			if(last != NULL)
				AppendNoFormat((Token*)last->value, str.str.c_str(), str.wstr.c_str(), str.len);  
		}

		Token::Remove(format);
	}
}

// In PostgreSQL body terminates with $$ LANGUAGE plpgsql;
bool SqlParser::ParsePostgresBodyEnd()
{
	Token *dol = GetNextCharToken('$', L'$');

	if(dol == NULL)
		return false;

	Token *dol2 = GetNextCharToken('$', L'$');
	Token *name = NULL;

	// $body$
	if(dol2 == NULL)
	{
		name = GetNextToken();
		dol2 = GetNextCharToken('$', L'$');
	}

	if(dol2 == NULL)
		return false;

	Token *language = NULL;
	Token *plpgsql = NULL;
	Token *semi = NULL;

	if(dol2 != NULL)
		language = GetNextWordToken("LANGUAGE", L"LANGUAGE", 8);

	if(language != NULL)
		plpgsql = GetNextWordToken("PLPGSQL", L"PLPGSQL", 7);

	if(plpgsql != NULL)
	{
		// Options
		while(true)
		{
			Token *option = GetNextToken();

			if(option == NULL)
				break;

			bool exists = false;

			// VOLATILE
			if(option->Compare("VOLATILE", L"VOLATILE", 8) == true)
			{
				if(_target != SQL_POSTGRESQL)
					Token::Remove(option);

				exists = true;
				continue;
			}
			else
			// COST n
			if(option->Compare("COST", L"COST", 4) == true)
			{
				Token *num = GetNextToken();

				if(_target != SQL_POSTGRESQL)
					Token::Remove(option, num);

				exists = true;
				continue;
			}

			PushBack(option);
			break;
		}

		semi = GetNextCharToken(';', L';');
	}

	// Change to / for Oracle
	if(_target == SQL_ORACLE)
	{
		Token::ChangeNoFormat(dol, "/", L"/", 1);
		
		Token::Remove(name);
		Token::Remove(dol2, plpgsql);
		Token::Remove(semi);
	}

	return true;
}




