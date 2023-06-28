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

// SQLParser for storage clauses

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Temporary table options
bool SqlParser::ParseTempTableOptions(Token *table_name, Token **start_out, Token **end_out, bool *no_data)
{
	bool exists = false;

	Token *start = NULL;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		if(start == NULL)
			start = next;

		// ON COMMIT PRESERVE | DELETE ROWS in Oracle, DB2; ON ROLLBACK PRESERVE | DELETE ROWS in DB2   
		if(next->Compare("ON", L"ON", 2) == true)
		{
			Token *commit = GetNextWordToken("COMMIT", L"COMMIT", 6);
			Token *rollback = NULL;

			if(commit == NULL)
				rollback = GetNextWordToken("ROLLBACK", L"ROLLBACK", 8);

			if(commit == NULL && rollback == NULL)
				break;

			Token *delete_ = GetNextWordToken("DELETE", L"DELETE", 6);
			Token *preserve = NULL;

			if(delete_ == NULL)
				preserve = GetNextWordToken("PRESERVE", L"PRESERVE", 8);

			Token *rows = GetNextWordToken("ROWS", L"ROWS", 4);

			if(_target == SQL_SQL_SERVER)
				Token::Remove(next, rows);
			else
			// Oracle does not support ON ROLLBACK, but DELETE ROWS in default on rollback
			if(_target == SQL_ORACLE && rollback != NULL) 
			{
				if(delete_ != NULL)
					Token::Remove(next, rows);
				else
					Comment(next, rows);
			}
			
			exists = true;
			continue;
		}
		else
		// NOT LOGGED in DB2   
		if(next->Compare("NOT", L"NOT", 3) == true)
		{
			Token *logged = GetNextWordToken("LOGGED", L"LOGGED", 6);

			if(logged != NULL)
			{
				if(_target == SQL_ORACLE) 
					Token::Remove(next, logged);

				exists = true;
				continue;
			}
		}
		else
		// WITH REPLACE, WITH NO DATA in DB2   
		if(next->Compare("WITH", L"WITH", 4) == true)
		{
			Token *replace = GetNextWordToken("REPLACE", L"REPLACE", 7);
			Token *no = NULL;

			if(replace == NULL)
				no = GetNextWordToken("NO", L"NO", 2);

			// WITH REPLACE in DB2
			if(replace != NULL)
			{
				if(Target(SQL_DB2) == false) 
					Token::Remove(next, replace);

				_spl_declared_tables_with_replace.Add(table_name);

				exists = true;
				continue;
			}
			else
			// WITH NO DATA in DB2
			if(no != NULL)
			{
				Token *data = GetNextWordToken("DATA", L"DATA", 4);

				if(data != NULL)
				{
					if(_target == SQL_ORACLE) 
						Token::Remove(next, data);

					if(no_data != NULL)
						*no_data = true;

					exists = true;
					continue;
				}
			}
		}
		else
		// DEFINITION ONLY in DB2   
		if(next->Compare("DEFINITION", L"DEFINITION", 10) == true)
		{
			Token *only = GetNextWordToken("ONLY", L"ONLY", 4);

			if(only != NULL)
			{
				if(_target == SQL_ORACLE) 
					Token::Remove(next, only);

				if(no_data != NULL)
					*no_data = true;

				exists = true;
				continue;
			}
		}
		else
		// IN tablespace in DB2   
		if(next->Compare("IN", L"IN", 2) == true)
		{
			Token *tablespace_name = GetNextToken();

			if(tablespace_name != NULL)
			{
				if(_target == SQL_ORACLE) 
					Token::Remove(next, tablespace_name);

				exists = true;
				continue;
			}
		}
				
		// Not a temporary table clause
		PushBack(next);
		break;
	}

	if(exists == true)
	{
		if(start_out != NULL)
			*start_out = start;

		if(end_out != NULL)
			*end_out = GetLastToken();
	}

	return exists;
}

// Parse CREATE TABLE storage clause
bool SqlParser::ParseStorageClause(Token *table_name, Token **id_start, Token **comment,
									Token *last_colname, Token *last_colend)
{
	if(_source == SQL_ORACLE && ParseOracleStorageClause() == true)
		return true;

	if(_source == SQL_SQL_SERVER && ParseSqlServerStorageClause() == true)
		return true;

	if(_source == SQL_MYSQL && ParseMysqlStorageClause(table_name, id_start, comment) == true)
		return true;

	if(_source == SQL_DB2 && ParseDb2StorageClause() == true)
		return true;

	if(_source == SQL_INFORMIX && ParseInformixStorageClause() == true)
		return true;

	if(_source == SQL_TERADATA && ParseTeradataStorageClause(last_colname, last_colend) == true)
		return true;

	if(_source == SQL_SYBASE_ADS && ParseSybaseAdsStorageClause() == true)
		return true;

	return false;
}

// Parse SQL Server CREATE TABLE storage clause
bool SqlParser::ParseSqlServerStorageClause()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNext();

		if(next == NULL)
			break;

		// ON filegroup_name
		if(next->Compare("ON", L"ON", 2) == true)
		{
			// File group name
			Token *name = GetNext();
			
			if(_target != SQL_SQL_SERVER)
			{
				// Remove clause if it is the default PRIMARY file group
				if(Token::Compare(name, "PRIMARY", L"PRIMARY", 7) == true ||
					Token::Compare(name, "[PRIMARY]", L"[PRIMARY]", 9) == true)
				{
					Token::Remove(next, name);
				}
			}

			exists = true;
		}
		else
		{
			PushBack(next);
			break;
		}
	}

	return exists;
}

// Parse MySQL CREATE TABLE storage clause
bool SqlParser::ParseMysqlStorageClause(Token *table_name, Token **id_start, Token **comment_out)
{
	bool exists = false;

	// Auto_increment start value
	Token *auto_start = NULL;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// ENGINE = type
		if(next->Compare("ENGINE", L"ENGINE", 6) == true)
		{
			// Equal sign = is optional in the clause
			Token *equal = GetNextCharToken('=', L'=');
			Token *type = GetNextToken();

			if(_target != SQL_MYSQL)
			{
				Token::Remove(next);
				Token::Remove(equal);
				Token::Remove(type);
			}

			exists = true;
			continue;
		}
		else
		// AUTO_INCREMENT = start table option
		if(next->Compare("AUTO_INCREMENT", L"AUTO_INCREMENT", 14) == true)
		{
			// Equal sign = is optional in the clause
			Token *equal = GetNextCharToken('=', L'=');
			auto_start = GetNextNumberToken();

			if(_target != SQL_MYSQL)
			{
				Token::Remove(next);
				Token::Remove(equal);
				Token::Remove(auto_start);
			}

			exists = true;
			continue;
		}
		else
		// DEFAULT CHARSET
		if(next->Compare("DEFAULT", L"DEFAULT", 7) == true)
		{
			Token *option = GetNextToken();

			if(option == NULL)
				break;

			// CHARSET
			if(option->Compare("CHARSET", L"CHARSET", 7) == true)
			{
				/*Token *equal */ (void) GetNextCharToken('=', L'=');
				Token *value = GetNextIdentToken();

				if(_target != SQL_MYSQL)
					Token::Remove(next, value);
			}
			else
			// CHARACTER SET
			if(option->Compare("CHARACTER", L"CHARACTER", 9) == true)
			{
				/*Token *set */ (void) GetNextWordToken("SET", L"SET", 3);
				/*Token *equal */ (void) GetNextCharToken('=', L'=');
				Token *value = GetNextIdentToken();

				if(_target != SQL_MYSQL)
					Token::Remove(next, value);
			}

			exists = true;
			continue;
		}
		else
		// COLLATE = value
		if(next->Compare("COLLATE", L"COLLATE", 7) == true)
		{
			/*Token *equal */ (void) GetNextCharToken('=', L'=');
			Token *value = GetNextIdentToken();

			if(_target != SQL_MYSQL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// COMMENT = 'table comment'
		if(next->Compare("COMMENT", L"COMMENT", 7) == true)
		{
			// Equal sign = is optional in the clause
			/*Token *equal */ (void) GetNextCharToken('=', L'=');
			Token *text = GetNextToken();

			if(comment_out != NULL)
				*comment_out = text;

			// Remove from CREATE TABLE
			if(_target != SQL_MYSQL)
				Token::Remove(next, text);

			exists = true;
			continue;
		}
		else
		// PACK_KEYS = 0 | 1 | DEFAULT
		if(next->Compare("PACK_KEYS", L"PACK_KEYS", 9) == true)
		{
			// Optional = 
			/*Token *equal */ (void) GetNextCharToken('=', L'=');
			Token *value = GetNextToken();

			if(_target != SQL_MYSQL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// ROW_FORMAT = type | DEFAULT
		if(next->Compare("ROW_FORMAT", L"ROW_FORMAT", 10) == true)
		{
			// Optional = 
			/*Token *equal */ (void) GetNextCharToken('=', L'=');
			Token *value = GetNextToken();

			if(_target != SQL_MYSQL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}

		// Not a MySQL stoage clause
		PushBack(next);

		break;
	}

	if(id_start != NULL)
		*id_start = auto_start;

	// Restart sequence for PostgreSQL and Greenplum
	if(auto_start != NULL && (_target == SQL_POSTGRESQL || _target == SQL_GREENPLUM))
	{
		// Try to get ;
		Token *semi = GetNextCharToken(';', L';');

		Token *last = (semi != NULL) ? semi : GetLastToken();

		// Append ALTER SEQUENCE command
		Append(last, "\n\nALTER SEQUENCE ", L"\n\nALTER SEQUENCE ", 17);

		// Add sequence name
		Token *seq_name = AppendIdentifier(table_name, "_seq", L"_seq", 4);
		Append(last, seq_name);

		Append(last, " RESTART WITH ", L" RESTART WITH ", 14);
		AppendCopy(last, auto_start);
		Append(last, ";", L";", 1);
	}

	return exists;
}

// Parse Oracle CREATE TABLE, CREATE INDEX, PARTITION definition storage clause
bool SqlParser::ParseOracleStorageClause()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// SEGMENT CREATION IMMEDIATE | DEFERRED (or undocumented DB CREATION IMMEDIATE found in one customer)
		if(next->Compare("SEGMENT", L"SEGMENT", 7) == true || TOKEN_CMP(next, "DB"))
		{
			Token *creation = GetNextWordToken("CREATION", L"CREATION", 8);
			Token *value = GetNextToken();

			if(_target != SQL_ORACLE && creation != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// PCTFREE num
		if(next->Compare("PCTFREE", L"PCTFREE", 7) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// PCTUSED num
		if(next->Compare("PCTUSED", L"PCTUSED", 7) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// INITRANS num
		if(next->Compare("INITRANS", L"INITRANS", 8) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// MAXTRANS num
		if(next->Compare("MAXTRANS", L"MAXTRANS", 8) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// COMPRESS [BASIC] or COMPRESS num (for index-orginized tables and indexes)
		if(next->Compare("COMPRESS", L"COMPRESS", 8) == true)
		{
			// Optional BASIC keyword
			Token *basic = GetNextWordToken("BASIC", L"BASIC", 5);
			Token *num = NULL;

			// Check for a number
			if(basic == NULL)
				num = GetNextNumberToken();

			if(_target != SQL_ORACLE)
			{
				Token::Remove(next);
				Token::Remove(basic);
				Token::Remove(num);
			}

			exists = true;
			continue;
		}
		else
		// NOCOMPRESS 
		if(next->Compare("NOCOMPRESS", L"NOCOMPRESS", 10) == true)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOCACHE 
		if(next->Compare("NOCACHE", L"NOCACHE", 7) == true)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// LOGGING 
		if(next->Compare("LOGGING", L"LOGGING", 7) == true)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOLOGGING 
		if(next->Compare("NOLOGGING", L"NOLOGGING", 9) == true)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOPARALLEL 
		if(next->Compare("NOPARALLEL", L"NOPARALLEL", 10) == true)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// PARALLEL num
		if(next->Compare("PARALLEL", L"PARALLEL", 8) == true)
		{
			Token *value = GetNextNumberToken();

			if(_target != SQL_ORACLE && value != NULL)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// NOMONITORING 
		if(next->Compare("NOMONITORING", L"NOMONITORING", 12) == true)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// TABLESPACE name 
		if(next->Compare("TABLESPACE", L"TABLESPACE", 10) == true)
		{
			Token *name = GetNextIdentToken();

			// ON name
			if(_target == SQL_SQL_SERVER)
				Token::Change(next, "ON", L"ON", 2);
			else
			// IN name
			if(_target == SQL_DB2)
				Token::Change(next, "IN", L"IN", 2);
			else
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// STORAGE () clause 
		if(next->Compare("STORAGE", L"STORAGE", 7) == true)
		{
			exists = ParseOracleStorageClause(next);
			continue;
		}
		else
		// LOB (column) STORE AS (params)
		if(next->Compare("LOB", L"LOB", 3) == true)
		{
			if(ParseOracleLobStorageClause(next) == true)
			{
				exists = true;
				continue;
			}
		}
		else
		// Oracle partitioning clauses
		if(ParseOraclePartitions(next) == true)
		{
			exists = true;
			continue;
		}
		else
		// COMPUTE STATISTICS 
		if(next->Compare("COMPUTE", L"COMPUTE", 7) == true)
		{
			Token *statistics = GetNextWordToken("STATISTICS", L"STATISTICS", 10);

			// Remove if not Oracle
			if(_target != SQL_ORACLE)
				Token::Remove(next, statistics);

			exists = true;
			continue;
		}
		else
		// ENABLE ROW MOVEMENT 
		if(next->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			Token *row = GetNextWordToken("ROW", L"ROW", 3);

			if(row != NULL)
			{
				Token *movement = GetNextWordToken("MOVEMENT", L"MOVEMENT", 8);
			
				// Remove if not Oracle
				if(_target != SQL_ORACLE)
					Token::Remove(next, movement);

				exists = true;
				continue;
			}
		}
		else
		// REVERSE index or primary key storage attribute 
		if(next->Compare("REVERSE", L"REVERSE", 7) == true)
		{
			// Remove if not Oracle
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		
		// Not an Oracle storage clause
		PushBack(next);
		break;
	}

	return exists;
}

// Parse DB2 CREATE TABLE storage clause
bool SqlParser::ParseDb2StorageClause()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// IN tablespace
		if(next->Compare("IN", L"IN", 2) == true)
		{
			// Get tablespace name (can include database name: dbname.tbsname)
			Token *name = GetNextIdentToken();

			// TABLESPACE name in Oracle
			if(_target == SQL_ORACLE)
			{
				Token::Change(next, "TABLESPACE", L"TABLESPACE", 10);

				// Name can contain database name, remove it
				ConvertIdentRemoveLeadingPart(name);
			}
			else
			if(_target != SQL_DB2)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// INDEX IN tablespace
		if(next->Compare("INDEX", L"INDEX", 5) == true)
		{
			/*Token *in */ (void) GetNextWordToken("IN", L"IN", 2);
			Token *name = GetNextIdentToken();

			if(_target != SQL_DB2)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// COMPRESS YES | NO clause
		if(next->Compare("COMPRESS", L"COMPRESS", 8) == true)
		{
			Token *yesno = GetNextToken();
			
			if(_target != SQL_DB2)
				Token::Remove(next, yesno);

			exists = true;
			continue;
		}
		else
		// CCSID ASCII | UNICODE | EBCDIC
		if(next->Compare("CCSID", L"CCSID", 5) == true)
		{
			Token *encoding = GetNextToken();
			
			if(_target != SQL_DB2)
				Token::Remove(next, encoding);

			exists = true;
			continue;
		}
		else
		// DATA CAPTURE CHANGES | NONE
		if(next->Compare("DATA", L"DATA", 4) == true)
		{
			Token *capture = GetNextWordToken("CAPTURE", L"CAPTURE", 7);
			Token *option = NULL;
			
			if(capture != NULL)
			{
				option = GetNextToken();
			
				if(_target != SQL_DB2)
					Token::Remove(next, option);

				exists = true;
				continue;
			}
		}
		else
		// DB2 z/OS AUDIT CHANGES | NONE | ALL
		if(next->Compare("AUDIT", L"AUDIT", 5) == true)
		{
			Token *option = GetNextToken();
			
			if(_target != SQL_DB2)
				Token::Remove(next, option);

			exists = true;
			continue;
		}
		else
		// DB2 z/OS WITH RESTRICT ON DROP  
		if(next->Compare("WITH", L"WITH", 4) == true)
		{
			Token *restrict = GetNextWordToken("RESTRICT", L"RESTRICT", 8);
			Token *on = NULL;
			Token *drop = NULL;

			if(restrict != NULL)
			{
				on = GetNextWordToken("ON", L"ON", 2);

				if(on != NULL)
					drop = GetNextWordToken("DROP", L"DROP", 4);

				if(_target != SQL_DB2)
					Token::Remove(next, drop);

				exists = true;
				continue;
			}
		}
		else
		// DB2 z/OS NOT VOLATILE  
		if(next->Compare("NOT", L"NOT", 3) == true)
		{
			Token *volatile_ = GetNextWordToken("VOLATILE", L"VOLATILE", 8);

			if(volatile_ != NULL)
			{
				if(_target != SQL_DB2)
					Token::Remove(next, volatile_);

				exists = true;
				continue;
			}
		}
		else
		// DB2 z/OS VOLATILE  
		if(next->Compare("VOLATILE", L"VOLATILE", 8) == true)
		{
			if(_target != SQL_DB2)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// DB2 z/OS APPEND NO | YES  
		if(next->Compare("APPEND", L"APPEND", 6) == true)
		{
			Token *value = GetNextToken();

			if(value != NULL)
			{
				if(_target != SQL_DB2)
					Token::Remove(next, value);

				exists = true;
				continue;
			}
		}
		else
		// PARTITION BY SIZE EVERY n G 
		if(next->Compare("PARTITION", L"PARTITION", 9) == true)
		{
			Token *by = GetNextWordToken("BY", L"BY", 2);
			Token *size = GetNextWordToken(by, "SIZE", L"SIZE", 4);

			// Partition by size
			if(size != NULL)
			{
				// Optional EVERY n G
				Token *every = GetNextWordToken("EVERY", L"EVERY", 5);
				Token *num = GetNextToken(every);
				Token *g = GetNextWordToken("G", L"G", 1);

				if(_target != SQL_DB2)
				{
					Token::Remove(next, size);
					Token::Remove(every, num);
					Token::Remove(g);
				}

				exists = true;
				continue;
			}
			else
			if(ParseDb2PartitioningClause(next, by) == true)
			{
				exists = true;
				continue;
			}
		}
		else
		// ORGANIZE BY COLUMN | ROW
		if(TOKEN_CMP(next, "ORGANIZE"))
		{
			Token *by = TOKEN_GETNEXTW("BY");

			Token *column = TOKEN_GETNEXTWP(by, "COLUMN");
			/*Token *row */ (column == NULL) ? TOKEN_GETNEXTWP(by, "ROW") : NULL;

			exists = true;
			continue;
		}

		// Not a DB2 storage clause
		PushBack(next);

		break;
	}

	return exists;
}

// Parse Informix CREATE TABLE storage clause
bool SqlParser::ParseInformixStorageClause()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// EXTENT SIZE num
		if(next->Compare("EXTENT", L"EXTENT", 6) == true)
		{
			Token *size = GetNextWordToken("SIZE", L"SIZE", 4);
			Token *num = GetNextToken(size);

			if(_target != SQL_INFORMIX && num != NULL)
				Token::Remove(next, num);

			exists = true;
			continue;
		}
		else
		// FRAGMENT BY partitioning clause
		if(next->Compare("FRAGMENT", L"FRAGMENT", 8) == true)
		{
			ParseInformixFragmentBy(next);

			exists = true;
			continue;
		}
		else
		// IN dbspace
		if(next->Compare("IN", L"IN", 2) == true)
		{
			// Get database space name
			Token *name = GetNextToken();

			// TABLESPACE name in Oracle
			if(_target == SQL_ORACLE)
				Token::Change(next, "TABLESPACE", L"TABLESPACE", 10);
			else
			// DB2 also uses IN keyword to specify a tablespace
			if(Target(SQL_DB2, SQL_INFORMIX) == false)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// LOCK MODE PAGE | ROW | TABLE
		if(next->Compare("LOCK", L"LOCK", 4) == true)
		{
			Token *mode = GetNextWordToken("MODE", L"MODE", 4);
			Token *type = GetNextToken(mode);

			if(_target != SQL_INFORMIX && type != NULL)
				Token::Remove(next, type);

			exists = true;
			continue;
		}
		else
		// NEXT SIZE num
		if(next->Compare("NEXT", L"NEXT", 4) == true)
		{
			Token *size = GetNextWordToken("SIZE", L"SIZE", 4);
			Token *num = GetNextToken(size);

			if(_target != SQL_INFORMIX && num != NULL)
				Token::Remove(next, num);

			exists = true;
			continue;
		}

		// Not a storage clause
		PushBack(next);
		break;
	}

	return exists;
}

// Parse Teradata CREATE TABLE storage clause
bool SqlParser::ParseTeradataStorageClause(Token *last_colname, Token *last_colend)
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// UNIQUE PRIMARY INDEX hash partitioning clause
		if(next->Compare("UNIQUE", L"UNIQUE", 6) == true)
		{
			Token *primary = GetNext("PRIMARY", L"PRIMARY", 7);

			if(primary != NULL)
				ParseTeradataPrimaryIndex(next, primary, last_colname, last_colend);

			exists = true;
			continue;
		}
		else
		// PRIMARY INDEX hash partitioning clause
		if(next->Compare("PRIMARY", L"PRIMARY", 7) == true)
		{
			ParseTeradataPrimaryIndex(NULL, next, last_colname, last_colend);

			exists = true;
			continue;
		}
        else
		// WITH [NO] DATA in CREATE TABLE AS SELECT
		if(next->Compare("WITH", L"WITH", 4) == true)
		{
            Token *no = GetNext("NO", L"NO", 2);
            Token *data = GetNext("DATA", L"DATA", 4);
			
            // WITH DATA is default in EsgynDB
            if(no == NULL && data != NULL && Target(SQL_ESGYNDB))
                Token::Remove(next, data);

			exists = true;
			continue;
		}

		// Not a storage clause
		PushBack(next);
		break;
	}

	return exists;
}

// Parse Sybase ADS CREATE TABLE storage clause
bool SqlParser::ParseSybaseAdsStorageClause()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// IN DATABASE
		if(TOKEN_CMP(next, "IN"))
		{
			Token *database = TOKEN_GETNEXTW("DATABASE");

			if(_target != SQL_SYBASE_ADS)
				Token::Remove(next, database);

			exists = true;
			continue;
		}

		// Not a storage clause
		PushBack(next);
		break;
	}

	return exists;
}

// CREATE INDEX storage options
bool SqlParser::ParseCreateIndexOptions()
{
	if(ParseOracleStorageClause() == true)
		return true;

	if(ParseDb2CreateIndexOptions() == true)
		return true;

	return false;
}