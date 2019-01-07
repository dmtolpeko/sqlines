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

// Greenplum specific functions

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Add DISTRIBUTED BY to CREATE TABLE based on primary keys
void SqlParser::AddGreenplumDistributedBy(Token *create, Token *close, ListW &pkcols, Book *col_end)
{
	if(close == NULL || col_end == NULL)
		return;

	bool first = true;

	ListwItem *cur = pkcols.GetFirst();

	// Browse primary keys columns
	while(cur != NULL)
	{
		if(first)
		{
			Append(close, " DISTRIBUTED BY (", L" DISTRIBUTED BY (", 17, create);

			col_end->distributed_by = true;
			first = false;
		}
		else
			AppendNoFormat(close, ", ", L", ", 2);

		// A key column
		AppendCopy(close, (Token*)cur->value);

		cur = cur->next;

		if(cur == NULL)
			AppendNoFormat(close, ")", L")", 1);
	}
}
