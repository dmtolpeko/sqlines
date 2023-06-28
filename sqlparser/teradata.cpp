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

// Copyright (c) 2013 SQLines. All rights reserved.

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Teradata CREATE TABLE options before column definition
bool SqlParser::ParseTeradataTableOptions()
{
	bool exists = false;

	// , goes after table name before even first option
	while(true)
	{
		Token *comma = TOKEN_GETNEXT(',');

		if(comma == NULL)
			break;

		// Most options can start with NO keyword to disable option
		Token *no = GetNextWordToken("NO", L"NO", 2);

		Token *next = GetNextToken();

		if(next == NULL)
		{
			if(no != NULL)
				PushBack(no);

			break;
		}

		// [NO] FALLBACK to store a row copy
		if(TOKEN_CMP(next, "FALLBACK") == true)
		{
			if(_target != SQL_TERADATA)
				Token::Remove(comma, next);

			exists = true;
			continue;
		}
		else
		// [NO] BEFORE JOURNAL to store before image
		if(TOKEN_CMP(next, "BEFORE") == true)
		{
			Token *journal = GetNextWordToken("JOURNAL", L"JOURNAL", 7);

			if(_target != SQL_TERADATA && journal != NULL)
				Token::Remove(comma, journal);

			exists = true;
			continue;
		}
		else
		// [NO] AFTER JOURNAL to store after image
		if(TOKEN_CMP(next, "AFTER") == true)
		{
			Token *journal = GetNextWordToken("JOURNAL", L"JOURNAL", 7);

			if(_target != SQL_TERADATA && journal != NULL)
				Token::Remove(comma, journal);

			exists = true;
			continue;
		}
        else
		// CHECKSUM = DEFAULT (NONE, HIGH, MEDIUM, LOW) 
		if(TOKEN_CMP(next, "CHECKSUM") == true)
		{
			Token *equal = TOKEN_GETNEXT('=');
            Token *value = GetNext(equal);

			if(_target != SQL_TERADATA && value != NULL)
				Token::Remove(comma, value);

			exists = true;
			continue;
		}
        else
		// DEFAULT MERGEBLOCKRATIO 
		if(TOKEN_CMP(next, "DEFAULT") == true)
		{
			Token *mergeblockratio = TOKEN_GETNEXTW("MERGEBLOCKRATIO");

            if(mergeblockratio != NULL)
            {
                if(_target != SQL_TERADATA)
				    Token::Remove(comma, mergeblockratio);

			    exists = true;
			    continue;
            }
		}

		if(no != NULL)
			PushBack(no);

        PushBack(next);
		break;
	}

	return exists;
}

// PRIMARY INDEX hash partitioning clause
bool SqlParser::ParseTeradataPrimaryIndex(Token *unique, Token *primary, Token *last_colname, Token *last_colend)
{
	if(primary == NULL)
		return false;

	Token *index = GetNextWordToken("INDEX", L"INDEX", 5);

	if(index == NULL)
		return false;

	Token *open = GetNextCharToken('(', L'(');
    Token *name = NULL;

    // Index name is optional
    if(open == NULL)
    {
        name = GetNextToken();
        open = GetNextCharToken('(', L'(');
    }

	// List of partitioning columns
	while(true)
	{
		Token *column = GetNextToken();

		if(column == NULL)
			break;

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	Token *close = GetNextCharToken(open, ')', L')');

	// Add UNIQUE constraint for other databases
	if(unique != NULL)
	{
        if(_target != SQL_TERADATA && _target != SQL_HIVE)
        {
		    AppendNoFormat(last_colend, ",", L",", 1);
		    Append(last_colend, "\n", L"\n", 1, last_colname);
		
		    AppendCopy(last_colend, unique);
		    AppendNoFormat(last_colend, " ", L" ", 1);
		    AppendCopy(last_colend, open, close);

            if(_target == SQL_ORACLE)
		        Token::Remove(unique);
            else
                Token::Remove(unique, close);
        }       
	}
    else
    {
        // STORE BY (col, ...) in EsgynDB
        if(_target == SQL_ESGYNDB)
        {
            TOKEN_CHANGE(primary, "STORE");
            TOKEN_CHANGE(index, "BY");
            Token::Remove(name);
        }
    }

	// PARTITION BY HASH in Oracle
	if(_target == SQL_ORACLE)
	{
		// In Oracle, temporary tables cannot be partitioned 
		if(_obj_scope != SQL_SCOPE_TEMP_TABLE)
		{
			Token::Change(primary, "PARTITION", L"PARTITION", 9);
			Token::Change(index, "BY HASH", L"BY HASH", 7);
		}
		else
            COMMENT("Temporary table cannot be partitioned\n", primary, close);
	}
    else
    // Hive does not support constraints
    if(_target == SQL_HIVE)
        Token::Remove(Nvl(unique, index), close);

	return true;
}

// COLLECT STATISTICS statement in Teradata
bool SqlParser::ParseCollectStatement(Token *collect)
{
	if(collect == NULL)
		return false;

	Token *statistics = TOKEN_GETNEXTW("STATISTICS");
    Token *stats = (statistics == NULL) ? TOKEN_GETNEXTW("STATS") : NULL;

	if(statistics == NULL && stats == NULL)
		return false;

    Token *on = TOKEN_GETNEXTW("ON");
    /*Token *name */ (void) GetNextIdentToken();

	STATS_DECL
    STMS_STATS(collect);

	// Convert to UPDATE STATISTICS and comment because options are different for EsgynDB
	if(_target == SQL_ESGYNDB)
	{
        TOKEN_CHANGE(collect, "UPDATE");
        TOKEN_CHANGE(stats, "STATISTICS");
        TOKEN_CHANGE(on, "FOR TABLE");

		Comment(collect, GetNextCharOrLastToken(';', L';')); 
	}
	
	return true;
}

// Teradata HELP statement
bool SqlParser::ParseTeradataHelpStatement(Token *help)
{
	if(help == NULL)
		return false;

	// HELP STATISTICS
	Token *statistics = GetNext("STATISTICS", L"STATISTICS", 10);

	if(statistics != NULL)
		return ParseTeradataHelpStatistics(help, statistics);	

	return false;
}

// Teradata HELP STATISTICS statement
bool SqlParser::ParseTeradataHelpStatistics(Token *help, Token *statistics)
{
	if(help == NULL || statistics == NULL)
		return false;

	// Table name
	/*Token *table_name */ (void) GetNextIdentToken(SQL_IDENT_OBJECT);

	// Comment for other databases
	if(_target != SQL_TERADATA)
		Comment(help, Nvl(GetNext(';', L';'), GetLastToken()));

	return true;
}

// Teradata comparison operators NE, EQ, LT, LE, GT, GE
bool SqlParser::ParseTeradataComparisonOperator(Token *op)
{
	if(op == NULL || _source != SQL_TERADATA)
		return false;

	bool exists = false;

	// Equal
	if(op->Compare("EQ", L"EQ", 2) == true)
	{
		if(_target != SQL_TERADATA)
			Token::Change(op, "=", L"=", 1);

		exists = true;
	}
	else
	// Less than or equal
	if(op->Compare("LE", L"LE", 2) == true)
	{
		if(_target != SQL_TERADATA)
			Token::Change(op, "<=", L"<=", 2);

		exists = true;
	}
	else
	// Less than 
	if(op->Compare("LT", L"LT", 2) == true)
	{
		if(_target != SQL_TERADATA)
			Token::Change(op, "<", L"<", 1);

		exists = true;
	}
	else
	// Not equal
	if(op->Compare("NE", L"NE", 2) == true)
	{
		if(_target != SQL_TERADATA)
			Token::Change(op, "<>", L"<>", 2);

		exists = true;
	}
	else
	// Greater than or equal
	if(op->Compare("GE", L"GE", 2) == true)
	{
		if(_target != SQL_TERADATA)
			Token::Change(op, ">=", L">=", 2);

		exists = true;
	}
	else
	// Greater than 
	if(op->Compare("GT", L"GT", 2) == true)
	{
		if(_target != SQL_TERADATA)
			Token::Change(op, ">", L">", 1);

		exists = true;
	}

	return exists;
}

// COMPRESS values clause in Teradata
bool SqlParser::ParseTeradataCompressClauseDefaultExpression(Token *compress)
{
	if(compress == NULL)
		return false;

	Token *open = GetNext('(', L'(');

	// List of values to compress or a single value without ()
	while(true)
	{
		Token *value = NULL;

        // Just COMPRESS without any value can be specified
        if(open == NULL)
        {
            // Get only string or number literals
            value = GetNextStringToken();

            if(value == NULL)
                value = GetNextNumberToken();
        }
        else
            value = GetNext();

		if(value == NULL || open == NULL)
			break;

		Token *comma = GetNext(',', L',');

		if(comma == NULL)
			break;
	}

	Token *close = GetNext(open, ')', L')');

	// Remove the clause for other databases
	if(_target != SQL_TERADATA)
		Token::Remove(compress, GetLastToken(close));

	return true;
}