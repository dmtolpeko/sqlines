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

// Post processing

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Post conversion when all tokens processed
void SqlParser::Post()
{
	if(_target != SQL_GREENPLUM)
		return;

	Book *bookmark = _bookmarks.GetFirstNoCurrent();

	// Iterate through bookmarks
	while(bookmark != NULL)
	{
		// Greenplum
		if(_target == SQL_GREENPLUM)
		{
			// Add DISTRIBUTED RANDOMLY for tables that are still without DISTRIBUTED clause
			if(bookmark->type == BOOK_CTC_ALL_END && bookmark->distributed_by == false)
				Append(bookmark->book, " DISTRIBUTED RANDOMLY", L" DISTRIBUTED RANDOMLY", 21);
			else
			// Comment out indexes
			if(bookmark->type == BOOK_CI_START)
				Prepend(bookmark->book, "/* ", L"/* ", 3);
			else
			// Comment out indexes
			if(bookmark->type == BOOK_CI_END)
				Append(bookmark->book, " */", L" */", 3);
		}
		
		bookmark = bookmark->next;
	}
}
