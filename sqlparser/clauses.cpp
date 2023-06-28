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

// Various SQL clauses

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// DB2 ALTER COLUMN clause in ALTER TABLE
bool SqlParser::AlterColumnClause(Token *table, Token *table_name, Token *alter)
{
	if(alter == NULL)
		return false;

	Token *column = GetNextWordToken("COLUMN", L"COLUMN", 6);

	if(column == NULL)
		return false;

	Token *name = GetNextIdentToken();

	// Option
	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	// RESTART WITH for identity column
	if(next->Compare("RESTART", L"RESTART", 7) == true)
	{
		/*Token *with */ (void) GetNextWordToken("WITH", L"WITH", 4);
		/*Token *value*/ (void) GetNextNumberToken();

		// Change to ALTER SEQUENCE for PostgreSQL and Greenplum
		if(_target == SQL_POSTGRESQL || _target == SQL_GREENPLUM)
		{
			Token *seq_name = AppendIdentifier(table_name, "_seq", L"_seq", 4);
		
			Token::Change(table, "SEQUENCE", L"SEQUENCE", 8);
			Token::Change(table_name, seq_name);

			// Remove ALTER COLUMN name
			Token::Remove(alter, name);

			delete seq_name;
		}
	}

	return true;
}

// ALTER/CREATE SEQUENCE options
bool SqlParser::ParseSequenceOptions(Token **start_with_out, Token **increment_by_out, StatsSummaryItem &ssi)
{
	bool exists = false;

	while(true)
	{
		STATS_DTL_DECL

		Token *option = GetNextToken();

		if(option == NULL)
			break;

		// START WITH num
		if(TOKEN_CMP(option, "START"))
		{
			/*Token *with */ (void) TOKEN_GETNEXTW("WITH");
			Token *start_with = GetNextNumberToken();

			if(Target(SQL_MYSQL))
				Token::Remove(option, start_with);

			if(start_with_out != NULL)
				*start_with_out = start_with;

			STATS_DTL_DESC(SEQUENCE_START_WITH_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("START WITH num", option);
			SEQ_OPT_DTL_STATS(option, start_with);

			exists = true;
		}
		else
		// INCREMENT BY
		if(TOKEN_CMP(option, "INCREMENT"))
		{
			/*Token *by */ (void) TOKEN_GETNEXTW("BY");
			Token *increment_by = GetNextNumberToken();

			if(Target(SQL_MYSQL))
				Token::Remove(option, increment_by);

			if(increment_by_out != NULL)
				*increment_by_out = increment_by;

			STATS_DTL_DESC(SEQUENCE_INCREMENT_BY_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("INCREMENT BY num", option);
			SEQ_OPT_DTL_STATS(option, increment_by);

			exists = true;
		}
		else
		// MINVALUE num
		if(TOKEN_CMP(option, "MINVALUE"))
		{
			Token *value = GetNextToken();

			if(Target(SQL_MYSQL))
				Token::Remove(option, value);

			STATS_DTL_DESC(SEQUENCE_MINVALUE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("MINVALUE num", option);
			SEQ_OPT_DTL_STATS(option, value);

			exists = true;
		}
		else
		// NOMINVALUE
		if(TOKEN_CMP(option, "NOMINVALUE"))
		{
			if(Target(SQL_MYSQL))
				Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOMINVALUE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("NOMINVALUE", option);

			exists = true;
		}
		else
		// MAXVALUE num
		if(TOKEN_CMP(option, "MAXVALUE"))
		{
			Token *value = GetNextToken();

			if(Target(SQL_MYSQL))
				Token::Remove(option, value);

			STATS_DTL_DESC(SEQUENCE_MAXVALUE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("MAXVALUE num", option);
			SEQ_OPT_DTL_STATS(option, value);

			exists = true;
		}
		else
		// NOMAXVALUE
		if(TOKEN_CMP(option, "NOMAXVALUE"))
		{
			if(Target(SQL_MYSQL))
				Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOMAXVALUE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))
			SEQ_DTL_STATS_V("NOMAXVALUE", option);

			exists = true;
		}
		else
		// CACHE num
		if(TOKEN_CMP(option, "CACHE"))
		{
			Token *value = GetNextToken();

			if(Target(SQL_MYSQL))
				Token::Remove(option, value);

			STATS_DTL_DESC(SEQUENCE_CACHE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("CACHE num", option);
			SEQ_OPT_DTL_STATS(option, value);

			exists = true;
		}
		else
		// NOCACHE
		if(TOKEN_CMP(option, "NOCACHE"))
		{
			if(Target(SQL_MYSQL))
				Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOCACHE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("NOCACHE", option);

			exists = true;
		}
		else
		// NOCYCLE
		if(TOKEN_CMP(option, "NOCYCLE"))
		{
			if(Target(SQL_MYSQL))
				Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOCYCLE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("NOCYCLE", option);

			exists = true;
		}
		else
		// CYCLE
		if(TOKEN_CMP(option, "CYCLE"))
		{
			if(Target(SQL_MYSQL))
				Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_CYCLE_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))

			SEQ_DTL_STATS_V("CYCLE", option);

			exists = true;
		}
		else
		// NOORDER
		if(TOKEN_CMP(option, "NOORDER"))
		{
			if(Target(SQL_MYSQL, SQL_MARIADB))
				Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOORDER_DESC)
			STATS_DTL_CONV_OK(Target(SQL_ORACLE, SQL_MARIADB_ORA), STATS_CONV_VERY_LOW, SEQUENCE_NOORDER_CONV, "")

			SEQ_DTL_STATS_V("NOORDER", option);

			exists = true;
		}
		else
		// ORDER
		if(TOKEN_CMP(option, "ORDER"))
		{
			if(Target(SQL_MYSQL))
				Token::Remove(option);
			else
			if(Target(SQL_MARIADB))
				COMMENT_WARN(option, NULL);

			STATS_DTL_DESC(SEQUENCE_ORDER_DESC)
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_HIGH, SEQUENCE_ORDER_CONV, SEQUENCE_ORDER_URL)
			SEQ_DTL_STATS_V("ORDER", option);
			
			exists = true;
		}
		else
		// NOKEEP
		if(TOKEN_CMP(option, "NOKEEP"))
		{
			if(Target(SQL_MYSQL, SQL_MARIADB))
				Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_NOKEEP_DESC)
			STATS_DTL_CONV_OK(Target(SQL_ORACLE, SQL_MARIADB_ORA), STATS_CONV_VERY_LOW, SEQUENCE_NOKEEP_CONV, "")

			SEQ_DTL_STATS_V("NOKEEP", option);

			exists = true;
		}
		else
		// KEEP
		if(TOKEN_CMP(option, "KEEP"))
		{
			if(Target(SQL_MYSQL))
				Token::Remove(option);
			else
			if(Target(SQL_MARIADB))
				COMMENT_WARN(option, NULL);

			STATS_DTL_DESC(SEQUENCE_KEEP_DESC)
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_HIGH, SEQUENCE_KEEP_CONV, "")
			SEQ_DTL_STATS_V("KEEP", option);
			
			exists = true;
		}
		else
		// GLOBAL
		if(TOKEN_CMP(option, "GLOBAL"))
		{
			if(Target(SQL_MYSQL, SQL_MARIADB))
				Token::Remove(option);

			STATS_DTL_DESC(SEQUENCE_GLOBAL_DESC)
			STATS_DTL_CONV_OK(Target(SQL_ORACLE, SQL_MARIADB_ORA), STATS_CONV_VERY_LOW, SEQUENCE_GLOBAL_CONV, "")

			SEQ_DTL_STATS_V("GLOBAL", option);

			exists = true;
		}
		else
		{
			PushBack(option);
			break;
		}

		STATS_UPDATE_STATUS
	}

	return exists;
}