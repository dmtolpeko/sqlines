/** 
 * Copyright (c) 2018 SQLines
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

// Java Conversion Class

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "str.h"
#include "java.h"

// Constructor
Java::Java()
{
	_sqlparser = NULL;
}

// Set data type for procedure OUT parameters
void Java::SetOutDataType(Token *data_type)
{
	if(data_type == NULL)
		return;

	TokenStr out(data_type);
	APPENDSTR(out, "Out");

	Token::ChangeNoFormat(data_type, out);
}

// Map SQL data type name (without length, precision, scale) to Java object type
const char *Java::MapSqlDataType(const char *name)
{
	if(name == NULL)
		return NULL;

	if(_stricmp(name, "NUMBER") == 0)
		return "Double";
	else
	if(_stricmp(name, "INT") == 0 || _stricmp(name, "INTEGER") == 0)
		return "Integer";

	return "Object";
}

// Create a string literal from a set of tokens (support multi-line strings)
void Java::MakeStringLiteral(Token *start, Token *end)
{
	if(start == NULL || end == NULL)
		return;

	Token *cur = start;

	while(cur != NULL && cur != end)
	{
		// Find every new line in the string
		if(TOKEN_CMPC(cur, '\n'))
		{
			Token *prev = cur;

			// Handle \r\n on Windows
			if(cur->prev != NULL && TOKEN_CMPC(cur->prev, '\r'))
				prev = cur->prev;

			// Terminate the current line
			JAVA_PREPEND(prev, "\" +");

			cur = cur->next;

			// Now find next first non-space token
			while(cur != NULL && cur != end)
			{
				if(TOKEN_CMPC(cur, ' ') || TOKEN_CMPC(cur, '\t') || TOKEN_CMPC(cur, '\r') || TOKEN_CMPC(cur, '\n'))
				{
					cur = cur->next;
					continue;
				}

				// Start the new line
				JAVA_PREPEND(cur, "\"");

				cur = cur->next;
				break;
			}
		}
		else
			cur = cur->next;
	}
	
	JAVA_PREPEND(start, "\"");
	JAVA_PREPEND(end, "\"");
}
