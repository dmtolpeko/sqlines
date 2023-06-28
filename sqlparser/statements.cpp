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

// SQLParser for statements

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

bool SqlParser::ParseStatement(Token *token, int scope, int *result_sets)
{
	if(token == NULL)
		return false;

	bool exists = false;
	bool proc = false;

	// ALTER statement
	if(token->Compare("ALTER", L"ALTER", 5) == true)
		exists = ParseAlterStatement(token, result_sets, &proc);
	else
	// ALLOCATE statement
	if(token->Compare("ALLOCATE", L"ALLOCATE", 8) == true)
		exists = ParseAllocateStatement(token);
	else
	// ASSOCIATE statement
	if(token->Compare("ASSOCIATE", L"ASSOCIATE", 9) == true)
		exists = ParseAssociateStatement(token);
	else
	// BEGIN WORK statement
	if(token->Compare("BEGIN", L"BEGIN", 5) == true)
		exists = ParseBeginStatement(token);
	else
	// CALL
	if(token->Compare("CALL", L"CALL", 4) == true)
		exists = ParseCallStatement(token);
	else
	// CASE
	if(token->Compare("CASE", L"CASE", 4) == true)
		exists = ParseCaseStatement(token);
	else
	// CLOSE
	if(token->Compare("CLOSE", L"CLOSE", 5) == true)
		exists = ParseCloseStatement(token);
    else
	// COLLECT STATISTICS in Teradata
	if(token->Compare("COLLECT", L"COLLECT", 7) == true)
		exists = ParseCollectStatement(token);
	else
	// CONNECT
	if(token->Compare("CONNECT", L"CONNECT", 7) == true)
		exists = ParseConnectStatement(token);
	else
	// COMMENT 
	if(token->Compare("COMMENT", L"COMMENT", 7) == true)
		exists = ParseCommentStatement(token);
	else
	// COMMIT 
	if(token->Compare("COMMIT", L"COMMIT", 6) == true)
		exists = ParseCommitStatement(token);
	else
	// CREATE statement
	if(token->Compare("CREATE", L"CREATE", 6) == true)
		exists = ParseCreateStatement(token, result_sets, &proc);
	else
	// DECLARE variable statement
	if(token->Compare("DECLARE", L"DECLARE", 7) == true)
		exists = ParseDeclareStatement(token);
	else
	// DEFINE variable statement
	if(token->Compare("DEFINE", L"DEFINE", 6) == true)
		exists = ParseDefineStatement(token);
	else
	// DELETE statement, or DEL statement in Teradata
	if((token->Compare("DELETE", L"DELETE", 6) == true) || (_source == SQL_TERADATA && TOKEN_CMP(token, "DEL") == true))
		exists = ParseDeleteStatement(token);
	else
	// DELIMITER statement
	if(token->Compare("DELIMITER", L"DELIMITER", 9) == true)
		exists = ParseDelimiterStatement(token);
	else
	// DROP statement
	if(token->Compare("DROP", L"DROP", 4) == true)
		exists = ParseDropStatement(token);
	else
	// EXCEPTION block
	if(token->Compare("EXCEPTION", L"EXCEPTION", 9) == true)
		exists = ParseExceptionBlock(token);
	else
	// EXEC statement
	if(token->Compare("EXEC", L"EXEC", 4) == true)
		exists = ParseExecStatement(token);
	else
	// EXECUTE statement
	if(token->Compare("EXECUTE", L"EXECUTE", 7) == true)
		exists = ParseExecuteStatement(token);
	else
	// EXIT statement
	if(token->Compare("EXIT", L"EXIT", 4) == true)
		exists = ParseExitStatement(token);
	else
	// EXPORT statement
	if(token->Compare("EXPORT", L"EXPORT", 6) == true)
		exists = ParseExportStatement(token);
	else
	// FETCH statement
	if(token->Compare("FETCH", L"FETCH", 5) == true)
		exists = ParseFetchStatement(token);
	else
	// FOR statement
	if(token->Compare("FOR", L"FOR", 3) == true)
		exists = ParseForStatement(token, scope);
	else
	// FOREACH statement
	if(token->Compare("FOREACH", L"FOREACH", 7) == true)
		exists = ParseForeachStatement(token, scope);
	else
	// FREE statement
	if(token->Compare("FREE", L"FREE", 4) == true)
		exists = ParseFreeStatement(token);
	else
	// Package function
	if(token->Compare("FUNCTION", L"FUNCTION", 8) == true)
	{
		Token *create = Prepend(token, "CREATE ", L"CREATE ", 7);

		exists = ParseCreateFunction(create, NULL, NULL, token);
	}
	else
	// HELP statement
	if(token->Compare("HELP", L"HELP", 4) == true)
		exists = ParseHelpStatement(token);
	else
	// IF statement
	if(token->Compare("IF", L"IF", 2) == true)
		exists = ParseIfStatement(token, scope);
	else
	// INS statement in Teradata
	if(_source == SQL_TERADATA && token->Compare("INS", L"INS", 3) == true)
	{
		// INSERT in other databases
		if(_target != SQL_TERADATA)
			Token::Change(token, "INSERT", L"INSERT", 6);

		exists = ParseInsertStatement(token);
	}
	else
	// INSERT statement
	if(token->Compare("INSERT", L"INSERT", 6) == true)
		exists = ParseInsertStatement(token);
	else
	// GET statement
	if(token->Compare("GET", L"GET", 3) == true)
		exists = ParseGetStatement(token);
	else
	// GRANT statement
	if(token->Compare("GRANT", L"GRANT", 5) == true)
		exists = ParseGrantStatement(token);
	else
	// LEAVE statement 
	if(token->Compare("LEAVE", L"LEAVE", 5) == true)
		exists = ParseLeaveStatement(token);
	else
	// LET statement 
	if(token->Compare("LET", L"LET", 3) == true)
		exists = ParseLetStatement(token);
	else
	// LOCK statement
	if(token->Compare("LOCK", L"LOCK", 4) == true)
		exists = ParseLockStatement(token);
	else
	// LOOP statement
	if(token->Compare("LOOP", L"LOOP", 4) == true)
		exists = ParseLoopStatement(token, scope);
	else
	// MERGE statement
	if(token->Compare("MERGE", L"MERGE", 5) == true)
		exists = ParseMergeStatement(token);
	else
	// NULL statement (Oracle)
	if(token->Compare("NULL", L"NULL", 4) == true)
		exists = ParseNullStatement(token);
	else
	// ON EXCEPTION statement
	if(token->Compare("ON", L"ON", 2) == true)
		exists = ParseOnStatement(token);
	else
	// OPEN statement
	if(token->Compare("OPEN", L"OPEN", 4) == true)
		exists = ParseOpenStatement(token);
	else
	// PERFORM statement
	if(token->Compare("PERFORM", L"PERFORM", 7) == true)
		exists = ParsePerformStatement(token);
	else
	// PREPARE statement
	if(token->Compare("PREPARE", L"PREPARE", 7) == true)
		exists = ParsePrepareStatement(token);
	else
	// PRINT statement 
	if(token->Compare("PRINT", L"PRINT", 5) == true)
		exists = ParsePrintStatement(token);
	else
	// Package procedure
	if(token->Compare("PROCEDURE", L"PROCEDURE", 9) == true)
	{
		Token *create = Prepend(token, "CREATE ", L"CREATE ", 7);

		exists = ParseCreateProcedure(create, NULL, NULL, token, NULL);
	}
	else
	// PROMPT statement in Oracle SQL*Plus
	if(token->Compare("PROMPT", L"PROMPT", 6) == true)
		exists = ParsePromptStatement(token);
	else
	// REM or REMARK command in Oracle SQL*Plus
	if(token->Compare("REM", L"REM", 3) == true || token->Compare("REMARK", L"REMARK", 6) == true)
		exists = ParseRemStatement(token);
	else
	// RAISE statement
	if(token->Compare("RAISE", L"RAISE", 5) == true)
		exists = ParseRaiseStatement(token);
	else
	// REPEAT statement
	if(token->Compare("REPEAT", L"REPEAT", 6) == true)
		exists = ParseRepeatStatement(token, scope);
	else
	// REPLACE statement
	if(token->Compare("REPLACE", L"REPLACE", 7) == true)
		exists = ParseReplaceStatement(token);
	else
	// RESIGNAL statement
	if(token->Compare("RESIGNAL", L"RESIGNAL", 8) == true)
		exists = ParseResignalStatement(token);
	else
	// RETURN statement
	if(token->Compare("RETURN", L"RETURN", 6) == true)
		exists = ParseReturnStatement(token);
	else
	// REVOKE statement
	if(token->Compare("REVOKE", L"REVOKE", 6) == true)
		exists = ParseRevokeStatement(token);
	else
	// ROLLBACK statement
	if(token->Compare("ROLLBACK", L"ROLLBACK", 8) == true)
		exists = ParseRollbackStatement(token);
	else
	// SAVEPOINT statement
	if(token->Compare("SAVEPOINT", L"SAVEPOINT", 9) == true)
		exists = ParseSavepointStatement(token);
	else
	// SEL statement in Teradata
	if(_source == SQL_TERADATA && token->Compare("SEL", L"SEL", 3) == true)
	{
		// SELECT in other databases
		if(_target != SQL_TERADATA)
			Token::Change(token, "SELECT", L"SELECT", 6);

		exists = ParseSelectStatement(token, scope, 0, result_sets, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}
	else
	// SELECT statement
	if(token->Compare("SELECT", L"SELECT", 6) == true)
		exists = ParseSelectStatement(token, scope, 0, result_sets, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	else
	// SET statement
	if(token->Compare("SET", L"SET", 3) == true)
		exists = ParseSetStatement(token);
	else
	// SHOW statement
	if(token->Compare("SHOW", L"SHOW", 4) == true)
		exists = ParseShowStatement(token);
	else
	// SIGNAL statement
	if(token->Compare("SIGNAL", L"SIGNAL", 6) == true)
		exists = ParseSignalStatement(token);
	else
	// SYSTEM statement
	if(token->Compare("SYSTEM", L"SYSTEM", 6) == true)
		exists = ParseSystemStatement(token);
	else
	// TERMINATE
	if(token->Compare("TERMINATE", L"TERMINATE", 9) == true)
		exists = ParseTerminateStatement(token);
	else
	// TRUNCATE
	if(token->Compare("TRUNCATE", L"TRUNCATE", 8) == true)
		exists = ParseTruncateStatement(token, scope);
	else
	// UPDATE
	if(token->Compare("UPDATE", L"UPDATE", 6) == true)
		exists = ParseUpdateStatement(token);
	else
	// USE
	if(token->Compare("USE", L"USE", 3) == true)
		exists = ParseUseStatement(token);
	else
	// VALUES statement
	if(token->Compare("VALUES", L"VALUES", 6) == true)
		exists = ParseValuesStatement(token, result_sets);
	else
	// WHILE statement
	if(token->Compare("WHILE", L"WHILE", 5) == true)
		exists = ParseWhileStatement(token, scope);
	// Label declaration 
	else
		exists = ParseLabelDeclaration(token, false);		

	// Parse object type assignment statement
	if(!exists && Source(SQL_ORACLE))
		exists = ParseOracleObjectTypeAssignment(token);
	
	// Assignment statement
	if(!exists)
		exists = ParseAssignmentStatement(token);

	if(!exists)
		return false;

	// Optional delimiter at the end of the statement
	/*Token *semi */ (void) GetNextCharToken(';', L';');

	// Handle GO after a standalone statement in SQL Server
	if(_spl_scope == 0)
		SqlServerGoDelimiter(true);

	return exists;
}

// CREATE statement
bool SqlParser::ParseCreateStatement(Token *create, int *result_sets, bool *proc)
{
	if(create == NULL)
		return false;

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	Token *or_ = NULL;
	Token *replace = NULL;
	Token *unique = NULL;
	Token *external = NULL;
	Token *materialized = NULL;

    int prev_object_scope = _obj_scope;

    _obj_scope = 0;
	
	// OR REPLACE clause in Oracle
	if(next->Compare("OR", L"OR", 2) == true)
	{
		or_ = next;
		replace = GetNextWordToken("REPLACE", L"REPLACE", 7);

		next = GetNextToken();
	}
	else
	// UNIQUE for CREATE INDEX
	if(next->Compare("UNIQUE", L"UNIQUE", 6) == true)
	{
		unique = next;
		next = GetNextToken();
	}
	else
	// GLOBAL TEMPORARY is Oracle
	if(next->Compare("GLOBAL", L"GLOBAL", 6) == true)
	{
		/*Token *temp */ (void) GetNextWordToken("TEMPORARY", L"TEMPORARY", 9);
		next = GetNextToken();
		
		_obj_scope = SQL_SCOPE_TEMP_TABLE;
	}
	else
	// PUBLIC SYNONYM in Oracle
	if(Token::Compare(next, "PUBLIC", L"PUBLIC", 6) == true)
	{
		Token *public_ = next;

		next = GetNextToken();

		if(_target == SQL_SQL_SERVER)
			Token::Remove(public_);
	}
	else
	// LOB TABLESPACE in DB2
	if(Token::Compare(next, "LOB", L"LOB", 3) == true)
	{
		Token *lob = next;

		next = GetNextToken();

		if(_target != SQL_DB2)
			Token::Remove(lob);
	}
	else
	// SCRATCH TABLE in Informix (non-logged temporary table in XPS)
	if(Token::Compare(next, "SCRATCH", L"SCRATCH", 7) == true)
	{
		if(_target == SQL_ORACLE)
			Token::Change(next, "GLOBAL TEMPORARY", L"GLOBAL TEMPORARY", 16);

		next = GetNextToken();

		_obj_scope = SQL_SCOPE_TEMP_TABLE;
	}
	else
	// EXTERNAL TABLE in Informix 
	if(Token::Compare(next, "EXTERNAL", L"EXTERNAL", 8) == true)
	{
		external = next;

		// For Oracle, ORGANIZATION EXTERNAL is appended after the column list
		if(_target == SQL_ORACLE)
			Token::Remove(next);

		next = GetNextToken();
	}
	else
	// RAW TABLE in Informix (non-logged permanent table) 
	if(Token::Compare(next, "RAW", L"RAW", 3) == true)
	{
		if(_target != SQL_INFORMIX)
			Token::Remove(next, true);

		next = GetNextToken();
	}
	else
	// MULTISET TABLE in Teradata (allows duplicate rows) 
	if(Token::Compare(next, "MULTISET", L"MULTISET", 8) == true)
	{
		if(_target != SQL_TERADATA)
			Token::Remove(next, true);

		next = GetNextToken();
	}
	else
	// VOLATILE TABLE in Teradata (temporary table that exists until the end of session)
	if(Token::Compare(next, "VOLATILE", L"VOLATILE", 8) == true)
	{
		if(_target == SQL_ORACLE)
			Token::Change(next, "GLOBAL TEMPORARY", L"GLOBAL TEMPORARY", 16);

		next = GetNextToken();

		_obj_scope = SQL_SCOPE_TEMP_TABLE;
	}
	else
	// MySQL DEFINER, ALGORITHM clauses
	if(ParseMyqlDefinerClause(next) == true)
	{
		next = GetNextToken();
	}

	// FORCE VIEW in Oracle can follow OR REPLACE
	if(Token::Compare(next, "FORCE", L"FORCE", 5) == true)
	{
		Token *force = next;

		next = GetNextToken();

		if(_target != SQL_ORACLE)
			Token::Remove(force);
	}
	
	// EDITIONABLE or NONEDITIONABLE for Oracle packages
	if(TOKEN_CMP(next, "EDITIONABLE") || TOKEN_CMP(next, "NONEDITIONABLE"))
	{
		Token *edit = next;

		next = GetNextToken();

		if(_target != SQL_ORACLE)
			Token::Remove(edit);
	}
	
	// CREATE MATERIALIZED VIEW in Oracle
	if(TOKEN_CMP(next, "MATERIALIZED"))
	{
		materialized = next;
		next = GetNextToken();
	}

	if(next == NULL)
		return false;

	bool exists = false;

	// CREATE AUXILIARY TABLE in DB2
	if(next->Compare("AUXILIARY", L"AUXILIARY", 9) == true || next->Compare("AUX", L"AUX", 3) == true)
	{
		exists = ParseCreateAuxiliary(create, next);
	}
	else
	// CREATE DATABASE
	if(next->Compare("DATABASE", L"DATABASE", 8) == true)
	{
		exists = ParseCreateDatabase(create, next);
	}
	else
	// CREATE FUNCTION
	if(next->Compare("FUNCTION", L"FUNCTION", 8) == true)
	{
		exists = ParseCreateFunction(create, or_, replace, next);
	}
	else
	// CREATE INDEX
	if(next->Compare("INDEX", L"INDEX", 5) == true)
	{
        _obj_scope = SQL_SCOPE_INDEX;
		exists = ParseCreateIndex(create, unique, next);
	}
	else
	// CREATE PACKAGE
	if(next->Compare("PACKAGE", L"PACKAGE", 7) == true)
	{
		exists = ParseCreatePackage(create, or_, replace, next);
	}
	else
	// CREATE PROCEDURE
	if(next->Compare("PROCEDURE", L"PROCEDURE", 9) == true || next->Compare("PROC", L"PROC", 4) == true)
	{
		exists = ParseCreateProcedure(create, or_, replace, next, result_sets);

		if(proc != NULL)
			*proc = true;
	}
	else
	// Sybase ASE CREATE RULE
	if(next->Compare("RULE", L"RULE", 4) == true)
		exists = ParseCreateRule(create, next);
	else
	// CREATE SCHEMA
	if(next->Compare("SCHEMA", L"SCHEMA", 6) == true)
		exists = ParseCreateSchema(create, next);
	else
	// Oracle CREATE SEQUENCE
	if(next->Compare("SEQUENCE", L"SEQUENCE", 8) == true)
		exists = ParseCreateSequence(create, next);
	else
	// DB2 for z/OS CREATE STOGROUP
	if(next->Compare("STOGROUP", L"STOGROUP", 8) == true)
		exists = ParseCreateStogroup(create, next);
	else
	// CREATE TABLE
	if(next->Compare("TABLE", L"TABLE", 5) == true)
	{
        // Scope can be already set to TEMP table i.e.
        if(_obj_scope == 0)
		    _obj_scope = SQL_SCOPE_TABLE;
		
		exists = ParseCreateTable(create, next);
	}
	else
	// CREATE TABLESPACE in Oracle and DB2
	if(next->Compare("TABLESPACE", L"TABLESPACE", 10) == true)
		exists = ParseCreateTablespace(create, next);
	else
	// CREATE TRIGGER
	if(next->Compare("TRIGGER", L"TRIGGER", 7) == true)
		exists = ParseCreateTrigger(create, or_, next);
	else
	// CREATE TYPE
	if(next->Compare("TYPE", L"TYPE", 4) == true)
		exists = ParseCreateType(create, next);
	else
	// CREATE VIEW
	if(next->Compare("VIEW", L"VIEW", 4) == true)
		exists = ParseCreateView(create, materialized, next);

    _obj_scope = prev_object_scope;

	return exists;
}

// ALTER statement
bool SqlParser::ParseAlterStatement(Token *alter, int *result_sets, bool *proc)
{
	if(alter == NULL)
		return false;

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	bool exists = false;

	// ALTER TABLE
	if(next->Compare("TABLE", L"TABLE", 5) == true)
	{
		exists = ParseAlterTableStatement(alter, next);
	}
	else
	// ALTER INDEX
	if(next->Compare("INDEX", L"INDEX", 5) == true)
	{
		exists = ParseAlterIndexStatement(alter, next);
	}
	else
	// ALTER FUNCTION
	if(next->Compare("FUNCTION", L"FUNCTION", 8) == true)
	{
		if(_source != SQL_ORACLE)
		{
			if(_target == SQL_ORACLE)
				Token::Change(alter, "CREATE OR REPLACE", L"CREATE OR REPLACE", 17);
			else
			// In MySQL ALTER FUNCTION does not allow to change body, so change to CREATE
			if(Target(SQL_MARIADB, SQL_MYSQL))
				Token::Change(alter, "CREATE", L"CREATE", 6);

			exists = ParseCreateFunction(alter, NULL, NULL, next);
		}
		else
			exists = ParseOracleAlterProcedure();
	}
	else
	// ALTER PROCEDURE
	if(next->Compare("PROCEDURE", L"PROCEDURE", 9) == true || next->Compare("PROC", L"PROC", 4) == true)
	{
		if(_source != SQL_ORACLE)
		{
			if(_target == SQL_ORACLE)
				Token::Change(alter, "CREATE OR REPLACE", L"CREATE OR REPLACE", 17);
			else
			// In MySQL ALTER PROCEDURE does not allow to change body, so change to CREATE
			if(Target(SQL_MARIADB, SQL_MYSQL))
				Token::Change(alter, "CREATE", L"CREATE", 6);

			exists = ParseCreateProcedure(alter, NULL, NULL, next, result_sets);

			if(proc != NULL)
				*proc = true;
		}
		else
			exists = ParseOracleAlterProcedure();
	}
	else
	// ALTER PACKAGE
	if(TOKEN_CMP(next, "PACKAGE"))
		exists = ParseOracleAlterProcedure();
	else
	// ALTER SEQUENCE
	if(TOKEN_CMP(next, "SEQUENCE"))
		exists = ParseAlterSequenceStatement(alter, next);

	return exists;
}

// ALTER TABLE statement
bool SqlParser::ParseAlterTableStatement(Token *alter, Token *table)
{
	STATS_DECL

	// Get table name
	Token *table_name = GetNextIdentToken();

	if(table_name == NULL)
		return false;

    int prev_stmt_scope = _stmt_scope;
    _stmt_scope = SQL_STMT_ALTER_TABLE;

	bool gp_comment = false;

	ListW pkcols;

	// WITH CHECK | NOCHECK in SQL Server
	if(_source == SQL_SQL_SERVER)
	{
		Token *with = TOKEN_GETNEXTW("WITH");

		Token *check = TOKEN_GETNEXTWP(with, "CHECK");
		/*Token *nocheck */ (check == NULL) ? TOKEN_GETNEXTWP(with, "NOCHECK") : NULL;
	}

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	// Oracle specific clauses in ALTER TABLE
	if(ParseOracleAlterTable(next))
	{
		gp_comment = true;
	}
	else
	// ADD constraint
	if(next->Compare("ADD", L"ADD", 3) == true)
	{
		// Try to parse a standalone column constraint
		ParseStandaloneColumnConstraints(alter, table_name, pkcols, NULL);

		gp_comment = true;
	}
	else
	// DB2 ALTER COLUMN
	if(next->Compare("ALTER", L"ALTER", 5) == true)
	{
		AlterColumnClause(table, table_name, next);
	}
	else
	// DB2 PCTFREE number
	if(next->Compare("PCTFREE", L"PCTFREE", 7) == true)
	{
		GetNextNumberToken();

		gp_comment = true;
	}

	// Comment ALTER TABLE for Greenplum
	if(_target == SQL_GREENPLUM && gp_comment == true)
	{
		Token *last = GetNextCharToken(';', L';');

		if(last == NULL)
			last = GetLastToken();

		Comment(alter, last);
	}

    _stmt_scope = prev_stmt_scope;

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

    STMS_STATS_V("ALTER TABLE", alter);
    ALTER_TAB_STMS_STATS("ALTER TABLE statements")

	return true;
}

// ALTER INDEX statement
bool SqlParser::ParseAlterIndexStatement(Token *alter, Token * /*index*/)
{
	STATS_DECL
    STMS_STATS_V("ALTER INDEX", alter);

	int options = 0;
	int removed = 0;

	// Index name
	/*Token *name */ (void) GetNextIdentToken();

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Oracle NOPARALLEL
		if(next->Compare("NOPARALLEL", L"NOPARALLEL", 10) == true)
		{
			options++;

			// Mark the option to be removed
			if(_target != SQL_ORACLE)
				removed++;

			continue;
		}
		else
		// Oracle PARALLEL num
		if(next->Compare("PARALLEL", L"PARALLEL", 8) == true)
		{
			/*Token *num */ (void) GetNextToken();

			options++;

			// Mark the option to be removed
			if(_target != SQL_ORACLE)
				removed++;

			continue;
		}

		// Unknown option
		PushBack(next);
		break;
	}

	// If there are options and they are all need to removed comment the entire statement
	if(options > 0 && options == removed)
	{
		// Check for closing ;
		Token *semi = GetNextCharToken(';', L';');
		
		Token *last = (semi != NULL) ? semi : GetLastToken();

		// Comment the entire ALTER INDEX statement
		Comment(alter, last);
	}

	return true;
}

// DB2 ALLOCATE statement
bool SqlParser::ParseAllocateStatement(Token *allocate)
{
	if(allocate == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(allocate);

	// Cursor name
	Token *cursor_name = GetNextToken();

	// CURSOR FOR RESULT SET 
	Token *cursor = GetNextWordToken(cursor_name, "CURSOR", L"CURSOR", 6);
	Token *for_ = GetNextWordToken(cursor, "FOR", L"FOR", 3);

	Token *result = GetNextWordToken(for_, "RESULT", L"RESULT", 6);
	/*Token *set */ (void) GetNextWordToken(result, "SET", L"SET", 3);

	// Result set locator variable
	Token *locator_name = GetNextToken();

	if(locator_name == NULL)
		return false;

	_spl_rs_locator_cursors.Add(cursor_name, locator_name);

	// In Oracle add SYS_REFCURSOR variable instead of removed result set locator declaration
	if(_target == SQL_ORACLE)
	{
		Token *loc = Find(_spl_declared_rs_locators, locator_name);

		if(loc != NULL)
		{
			AppendCopy(loc, cursor_name);
			Append(loc, " SYS_REFCURSOR;", L" SYS_REFCURSOR;", 15); 

			Token::Remove(allocate, Nvl(GetNextCharToken(';', L';'), GetLastToken()));
		}

		// Find procedures associates with this locator
		for(ListwmItem *i = _spl_rs_locator_procedures.GetFirst(); i != NULL; i = i->next)
		{
			Token *proc = (Token*)i->value;
			Token *proc_locator = (Token*)i->value2;

			// Another locator, continue search
			if(Token::Compare(locator_name, proc_locator) == false)
				continue;
		
			// Add OUT cursor parameter for each procedure call using this locator
			for(ListwmItem *j = _spl_sp_calls.GetFirst(); j != NULL; j = j->next)
			{
				Token *called_proc = (Token*)j->value;
				Token *last_param_end = (Token*)j->value2;

				// Another procedure call, continue search
				if(Token::Compare(proc, called_proc) == false)
					continue;

				// Procedure has parameters
				if(last_param_end != NULL)
				{
					AppendNoFormat(last_param_end, ", ", L", ", 2);
					AppendCopy(last_param_end, cursor_name);
				}
				else
				{
					AppendNoFormat(called_proc, "(", L"(", 1);
					AppendCopy(called_proc, cursor_name);
					AppendNoFormat(called_proc, ")", L")", 1);
				}
			}
		}
	}

	return true;
}

// DB2 ASSOCIATE statement
bool SqlParser::ParseAssociateStatement(Token *associate)
{
	if(associate == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(associate);

	// RESULT SET is optional
	Token *result = GetNextWordToken("RESULT", L"RESULT", 6);
	Token *set = GetNextWordToken(result, "SET", L"SET", 3);

	// LOCATOR or LOCATORS
	Token *locator = GetNextToken(set);

	if(locator == NULL)
		return false;

	// List of locators
	/*Token *open */ (void) GetNextCharToken('(', '(');

	ListW locs;

	while(true)
	{
		// Locator name
		Token *loc = GetNextToken();

		if(loc == NULL)
			break;

		locs.Add(loc);

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	/*Token *close */ (void) GetNextCharToken(')', ')');

	// WITH PROCEDURE
	/*Token *with */ (void) GetNextWordToken("WITH", L"WITH", 4);
	Token *procedure = GetNextWordToken(result, "PROCEDURE", L"PROCEDURE", 9);

	// Procedure name
	Token *proc_name = GetNextToken(procedure);

	if(proc_name == NULL)
		return false;

	for(ListwItem *i = locs.GetFirst(); i != NULL; i = i->next)
		_spl_rs_locator_procedures.Add(proc_name, i->value);

	if(_target == SQL_ORACLE)
		Token::Remove(associate, Nvl(GetNextCharToken(';', L';'), GetLastToken()));

	return true;
}

// BEGIN; or BEGIN WORK; in Informix, PostgreSQL; BEGIN TRANSACTION in PostgreSQL, Sybase ASE
bool SqlParser::ParseBeginStatement(Token *begin)
{
	if(begin == NULL)
		return false;

	// Optional WORK keyword
	Token *work = GetNextWordToken("WORK", L"WORK", 4);

	// Optional TRANSACTION keyword in PostgreSQL, Sybase ASE
	Token *transaction = (work == NULL) ? GetNextWordToken("TRANSACTION", L"TRANSACTION", 11) : NULL;

	// TRAN in SQL Server and Sybase ASE
	Token *tran = (work == NULL && transaction == NULL) ? TOKEN_GETNEXTW("TRAN") : NULL;

	// When WORK or TRANSACTION keyword not set check for ; to not confuse with BEGIN-END block
	if(work == NULL && transaction == NULL && tran == NULL)
	{
		Token *semi = GetNextCharToken(';', L';');

		if(semi == NULL)
		{
			// Parse as Oracle autonomous block
			if(_source == SQL_ORACLE)
			{
				ParseBlock(SQL_BLOCK_AUTONOMOUS, true, 0, NULL);
				return true;
			}
			else
				return false;
		}

		PushBack(semi);
	}

	// Comment in Oracle; comment in PostgreSQL procedure
	if(_target == SQL_ORACLE || (_target == SQL_POSTGRESQL && _spl_scope == SQL_SCOPE_PROC))
		Comment(begin, Nvl(GetNextCharToken(';', L';'), work, transaction, tran));
	else
	// MySQL, MariaDB use START TRANSACTION or BEGIN [WORK]
	if(Target(SQL_MYSQL, SQL_MARIADB))
	{
		if(transaction != NULL || tran != NULL)
			TOKEN_CHANGE(begin, "START");

		if(tran != NULL)
			TOKEN_CHANGE(tran, "TRANSACTION");
	}

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// DB2, Teradata CALL statement
bool SqlParser::ParseCallStatement(Token *call)
{
	if(call == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(call);

	// Procedure name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	// Optional list of parameters
	Token *open = GetNextCharToken('(', L'(');

	Token *last_param_end = NULL;

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

			last_param_end = GetLastToken();

			// Next parameter
			Token *comma = GetNextCharToken(',', L',');

			if(comma == NULL)
				break;
		}

		/*Token *close */ (void) GetNextCharToken(')', L')');
	}

	_spl_sp_calls.Add(name, last_param_end);

	// CALL keyword is not required in Oracle
	if(_target == SQL_ORACLE)
		Token::Remove(call);

	return false;
}

// CASE expression
bool SqlParser::ParseCaseStatement(Token *case_, bool stmt)
{
	if(case_ == NULL)
		return false;

	Token *var = GetNextToken();

	if(var == NULL)
		return false;

	// Searched CASE
	if(Token::Compare(var, "WHEN", L"WHEN", 4) == true)
	{
		PushBack(var);
		var = NULL;
	}
	else
		ParseExpression(var);

	Token *else_ = NULL;
	Token *else_exp = NULL;

	// List of WHEN conditions
	while(true)
	{
		Token *when = GetNextWordToken("WHEN", L"WHEN", 4);
		
		// Optional ELSE
		if(when == NULL)
			else_ = GetNextWordToken("ELSE", L"ELSE", 4);

		if(when == NULL && else_ == NULL)
			break;

		// Regular or boolean expression
		if(when != NULL)
		{
			if(var != NULL)
			{
				Token *exp = GetNextToken();

				ParseExpression(exp);
			}
			else
				ParseBooleanExpression(SQL_BOOL_CASE);

			/*Token *then */ (void) GetNextWordToken("THEN", L"THEN", 4);
		}
		// ELSE expression or block
		else
		{
			// Save the start of ELSE
			else_exp = GetNext();
			PushBack(else_exp);
		}
		
		// CASE expression
		if(stmt == false)
		{
			Token *exp = GetNext();
			ParseExpression(exp);
		}
		// CASE statement
		else
			ParseBlock(SQL_BLOCK_CASE, true, 0, NULL);		
	}

	Token *end = GetNextWordToken("END", L"END", 3);
	Token *end_case = NULL;

	// END CASE in Oracle, DB2 CASE statement (!), not expression
	if(end != NULL)
		end_case = GetNextWordToken("CASE", L"CASE", 4);

	// Set string datatype if ELSE evaluates to string
	if(else_exp != NULL && else_exp->type == TOKEN_STRING && else_exp->IsNumericInString() == false)
		case_->data_type = TOKEN_DT_STRING;

	return true;
}

// CLOSE cursor statement
bool SqlParser::ParseCloseStatement(Token *close)
{
	if(close == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(close);

	// Cursor name
	Token *name = GetNextToken();

	if(name == NULL)
		return false;

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	if(_target_app == APP_JAVA)
	{
		APPEND_NOFMT(name, "_stmt.close()");
		Token::Remove(close);
	}
	else
	// Add DEALLOCATE for SQL Server
	if(Source(SQL_ORACLE, SQL_DB2, SQL_INFORMIX) == true && _target == SQL_SQL_SERVER)
	{
		Token *semi = GetNextCharToken(';', L';');
		Token *last = Nvl(semi, GetLastToken());

		Append(last, "\nDEALLOCATE ", L"\nDEALLOCATE ", 12, close);
		AppendCopy(last, name, semi);
	}
	else
	// Remove DEALLOCATE following CLOSE for SQL Server, Sybase
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) && Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
	{
		Token *deallocate = GetNext("DEALLOCATE", L"DEALLOCATE", 10);

		// Optional CURSOR keyword in Sybase ASE
		/*Token *d_cursor */ (void) GetNext(deallocate, "CURSOR", L"CURSOR", 6);

		Token *name = GetNext(deallocate);

		if(deallocate != NULL && name != NULL)
		{
			Token *semi = GetNextCharToken(';', L';');

			Token::Remove(deallocate, name);
			Token::Remove(semi);
		}
	}
	else
	// Netezza does not support cursor statements
	if(_target == SQL_NETEZZA)
	{
		Token::Remove(close, name);

		Token *semi = GetNextCharToken(';', L';');
		Token::Remove(semi);		
	}

	return true;
}

// Oracle COMMENT statement
bool SqlParser::ParseCommentStatement(Token *comment)
{
    STATS_DECL
	STATS_DTL_DECL

	if(comment == NULL)
		return false;

	// ON keyword
	Token *on = GetNextWordToken("ON", L"ON", 2);

	if(on == NULL)
		return false;

	bool index = false;

	// Comment type (TABLE, COLUMN, INDEX i.e.)
	Token *type = GetNextToken();

	if(type == NULL)
		return false;

	// Parse specific DB2 for z/OS COMMENT syntax
	if(ParseDb2Comment(comment, on, type) == true)
		return true;

	// Object name
	Token *name = GetNextIdentToken();
	
	Token *is = GetNextWordToken("IS", L"IS", 2);
	Token *text = GetNextToken();

	// COMMENT ON TABLE
	if(type->Compare("TABLE", L"TABLE", 5) == true)
	{
        STMS_STATS_V("COMMENT ON TABLE", comment);

		// ALTER TABLE name COMMENT text in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			Token::Change(comment, "ALTER", L"ALTER", 5);
			Token::Remove(on);
			Token::Change(is, "COMMENT", L"COMMENT", 7);
		}
	}
	else
	// COMMENT ON COLUMN
	if(type->Compare("COLUMN", L"COLUMN", 6) == true)
	{
		STATS_SET_DESC(SQL_STMT_COMMENT_ON_COLUMN_DESC)
		STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_LOW, "", "")
		
		STATS_UPDATE_STATUS
        STMS_STATS_V("COMMENT ON COLUMN", comment);

		// execute sp_addextendedproperty in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(comment, "EXECUTE", L"EXECUTE", 7);
			AppendNoFormat(comment, " sp_addextendedproperty", L" sp_addextendedproperty", 23);

			Token::ChangeNoFormat(is, "'MS_Description',", L"'MS_Description',", 17);

			int cnt = GetIdentPartsCount(name);
			size_t len = 0;
			TokenStr str;
			TokenStr nm;

			str.Append(", 'user', ", L", 'user', ", 10);
			if(cnt > 2)
			{
				GetNextIdentItem(name, nm, &len);
				str.Append(nm);
				nm.Clear();
			}
			else
				str.Append("user_name()", L"user_name()", 11);

			str.Append(", 'table', ", L", 'table', ", 11);
			GetNextIdentItem(name, nm, &len);
			str.Append(nm);
			nm.Clear();

			str.Append(", 'column', ", L", 'column', ", 12);
			GetNextIdentItem(name, nm, &len);
			str.Append(nm);
			nm.Clear();

			Append(text, &str);
			Token::Remove(on, name);
		}
		else
		// Comment and move to CREATE TABLE in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			// Find the end of column in CREATE TABLE
			Book *book = GetBookmark2(BOOK_CTC_END, name);

			// Add the comment to CREATE TABLE
			if(book != NULL)
			{
				Append(book->book, " COMMENT ", L" COMMENT ", 9);
				AppendCopy(book->book, text);
			}

			Token *last = GetNextCharOrLastToken(';', L';');

			// Comment the statement
			Comment("Moved to CREATE TABLE\n", L"Moved to CREATE TABLE\n", 22, comment, last);
		}
	}
	else
	// COMMENT ON INDEX
	if(type->Compare("INDEX", L"INDEX", 5) == true)
		index = true;

	// For Greenplum, comment comments on indexes
	if(_target == SQL_GREENPLUM && index == true)
	{
		Token *last = GetNextCharToken(';', L';');

		if(last == NULL)
			last = text;

		Comment(comment, last); 
	}

	return true;
}

// COMMIT statement
bool SqlParser::ParseCommitStatement(Token *commit)
{
	if(commit == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_STMT_COMMIT_DESC)

	// Optional WORK keyword
	Token *work = GetNextWordToken("WORK", L"WORK", 4);

	// Optional TRANSACTION keyword in Sybase ASE
	Token *transaction = (work == NULL) ? GetNextWordToken("TRANSACTION", L"TRANSACTION", 11) : NULL;
	Token *tran = NULL;
	bool commented = false;

	// TRAN can be used in Sybase ASE
	if(work == NULL && transaction == NULL)
		tran = GetNextWordToken("TRAN", L"TRAN", 4);

	if(_target_app == APP_JAVA)
	{
		TOKEN_CHANGE_NOFMT(commit, "conn.commit()");

		Token::Remove(work);
		Token::Remove(transaction);
		Token::Remove(tran);
	}
	else
	// PostgreSQL, Greenplum and Netezza do not allow COMMIT in a procedure
	if(_spl_scope == SQL_SCOPE_PROC && Target(SQL_POSTGRESQL, SQL_NETEZZA, SQL_GREENPLUM))
	{
		Comment(commit, Nvl(GetNextCharToken(';', L';'), work, transaction, tran));
		commented = true;
	}
	else
	// MySQL, MariaDB support COMMIT [WORK]
	if(Target(SQL_MYSQL, SQL_MARIADB))
	{
		if(transaction != NULL)
			Token::Remove(transaction);

		if(tran != NULL)
			Token::Remove(tran);
	}

	// Add statement delimiter if not set when source is SQL Server
	if(!commented)
		SqlServerAddStmtDelimiter();

	STATS_SET_CONV_OK(true) 
    STMS_STATS(commit);

	return true;
}

// DB2 CONNECT statement
bool SqlParser::ParseConnectStatement(Token *connect)
{
	if(connect == NULL)
		return false;

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	STATS_DECL
    STMS_STATS(connect);

	// CONNECT TO
	if(next->Compare("TO", L"TO", 2) == true)
	{
		Token *db = GetNextIdentToken();

		// Comment for Greenplum
		if(_target == SQL_GREENPLUM)
		{
			Token *last = GetNextCharToken(';', L';');

			if(last == NULL)
				last = db;

			Comment(connect, last); 
		}
	}
	else
	// CONNECT RESET
	if(next->Compare("RESET", L"RESET", 5) == true)
	{
		// Comment for Greenplum
		if(_target == SQL_GREENPLUM)
		{
			Token *last = GetNextCharToken(';', L';');

			if(last == NULL)
				last = next;

			Comment(connect, last); 
		}
	}

	return true;
}

// CREATE TABLE statement
bool SqlParser::ParseCreateTable(Token *create, Token *token)
{
	STATS_DECL
	STATS_SET_DESC(SQL_STMT_CREATE_TABLE_DESC)

    CREATE_TAB_STMS_STATS("CREATE TABLE statements")

	if(token == NULL)
		return false;

	// CREATE TABLE IF NOT EXISTS clause in MySQL
	Token *if_ = GetNextWordToken("IF", L"IF", 2);
			
	if(if_ != NULL)
	{
		Token *not_ = GetNextWordToken("NOT", L"NOT", 3);
		Token *exists = GetNextWordToken(not_, "EXISTS", L"EXISTS", 6);

		if(exists != NULL)
		{
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true)
				Token::Remove(if_, exists);
		}
		else
			PushBack(if_);
	}

	// Get table name
	Token *table = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(table == NULL)
	{
		STMS_STATS_V("CREATE TABLE", create);
		return false;
	}

	// Temporary table in SQL Server, Sybase ASE starts with #, in Sybase ASE it also can start with tempdb..
	if(Token::Compare(table, '#', L'#', 0) || Token::Compare(table, "tempdb..", L"tempdb..", 0, 8))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			Append(create, " TEMPORARY", L" TEMPORARY", 10);
	}

	// Save bookmark to the start of CREATE TABLE
	Bookmark(BOOK_CT_START, table, create);

	// In Teradata options can follow before columns
	ParseTeradataTableOptions();

    ListW pkcols;
	ListWM inline_indexes;
	
	// Identity start and increment
	Token *id_col = NULL;
	Token *id_start = NULL;
	Token *id_inc = NULL;
	bool id_default = true;

	Token *last_colname = NULL;
    Token *last_colend = NULL;
    
    // CREATE TABLE AS SELECT
    Token *as = TOKEN_GETNEXTW("AS");

    if(as != NULL)
    {
        // ( is optional in AS (SELECT ...) 
        Token *open = TOKEN_GETNEXT('(');

        // SELECT statement
	    Token *select = GetNextSelectStartKeyword();

	    if(select != NULL)
    		ParseSelectStatement(select, 0, SQL_SEL_CREATE_TEMP_TABLE_AS, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        
        if(open != NULL)
            /*Token *close */ (void) TOKEN_GETNEXT(')');
    }
    else
    {
	    // Next token must be (
	    if(GetNextCharToken('(', L'(') == NULL)
		{
			STMS_STATS_V("CREATE TABLE", create);
		    return false;
		}

	    ParseCreateTableColumns(create, table, pkcols, &id_col, &id_start, &id_inc, &id_default, 
		    &inline_indexes, &last_colname);

	    last_colend = GetLastToken();

	    // Next token must be )
	    Token *close = GetNextCharToken(')', L')');
		
	    if(close == NULL)
		{
			STMS_STATS_V("CREATE TABLE", create);
		    return false;
		}

	    // Save bookmark to the end of columns but before storage and other properties
	    Book *col_end = Bookmark(BOOK_CTC_ALL_END, table, close);

	    // For Greenplum, add DISTRIBUTED by based on primary keys
	    if(_target == SQL_GREENPLUM && pkcols.GetCount() > 0)
		    AddGreenplumDistributedBy(create, close, pkcols, col_end);
    }

    // Table comment in MySQL
    Token *comment = NULL;

	ParseStorageClause(table, &id_start, &comment, last_colname, last_colend);

	// Add statement delimiter if not set and handle GO when source is SQL Server, Sybase ASE
	SqlServerDelimiter();

	Token *semi = GetNext(';', L';');

	Token *last = GetLastToken();

    if(semi == NULL && Target(SQL_ESGYNDB))
        AppendNoFormat(last, ";", L";", 1); 

	// Save bookmark to the end of CREATE TABLE
	Bookmark(BOOK_CT_END, table, last);

	// Add a separate COMMENT ON TABLE statement
	if(comment != NULL && Target(SQL_ORACLE))
	{
		Append(last, "\n\nCOMMENT ON TABLE ", L"\n\nCOMMENT ON TABLE ", 19, create);
		AppendCopy(last, table);
		Append(last, " IS ", L" IS ", 4, create);
		AppendCopy(last, comment);
	}

	// For Oracle add sequence and trigger to emulate identity column
	if(id_col != NULL && _target == SQL_ORACLE)
	{
		// Terminate CREATE TABLE when the delimiter not set in in the source
		if(semi == NULL && Source(SQL_SQL_SERVER, SQL_SYBASE) == false)
			AppendNoFormat(last, ";", L";", 1);

		OracleEmulateIdentity(create, table, id_col, last, id_start, id_inc, id_default);
	}

	// If source is MySQL move inline non-unique indexes to separate CREATE INDEX statements
	if(_source == SQL_MYSQL && inline_indexes.GetCount() > 0)
		MysqlMoveInlineIndexes(inline_indexes, last);

	if(_spl_first_non_declare == NULL)
		_spl_first_non_declare = create;

	_spl_last_stmt = create;

	STMS_STATS_V("CREATE TABLE", create);

	return true;
}

// Parse CREATE TABLE column and constraints definitions
bool SqlParser::ParseCreateTableColumns(Token *create, Token *table_name, ListW &pkcols, 
										Token **id_col, Token **id_start, Token **id_inc, bool *id_default,
										ListWM *inline_indexes, Token **last_colname)
{
	bool exists = false;

	while(true)
	{
		// Try to parse a standalone column constraint
		bool cns = ParseStandaloneColumnConstraints(create, table_name, pkcols, inline_indexes);

		// Parse a regular column definition
		if(cns == false)
		{
            CREATE_TAB_STMS_STATS("Columns")

			// Column name
			Token *column = GetNextIdentToken(SQL_IDENT_COLUMN_SINGLE);

			if(column == NULL)
				break;

			// FOR COLUMN system_name in DB2 for OS/400 
			ParseDb2ForColumn();

			// Data type (system or user-defined)
			Token *type = GetNextToken();

			ParseDataType(type);

			if(Token::Compare(type, "SERIAL", L"SERIAL", 0, 6) == true && id_col != NULL)
				*id_col = column;

			Token *type_end = GetLastToken();

			ParseColumnConstraints(create, table_name, column, type, type_end, id_col, id_start, id_inc, id_default);

			Token *last =  GetLastToken();

			// Set a bookmark to the column end
			Bookmark(BOOK_CTC_END, table_name, column, last);

			if(last_colname != NULL)
				*last_colname = column;
		}
		
		exists = true;

		// If the next token is not comma exit
		if(GetNextCharToken(',', L',') == NULL)
			break;

		// Most common typo to have comma after last column, exist if next is )
		Token *close = GetNextCharToken(')', L')');

		if(close != NULL)
		{
			PushBack(close);
			break;
		}
	}

	return exists;
}

// CREATE TABLESPACE in Oracle and DB2
bool SqlParser::ParseCreateTablespace(Token *create, Token *tablespace)
{
	bool exists = false;

	STATS_DECL
    STMS_STATS_V("CREATE TABLESPACE", create);

	if(_source == SQL_DB2)
		exists = ParseDb2CreateTablespace(create, tablespace);

	return exists;
}

// CREATE INDEX
bool SqlParser::ParseCreateIndex(Token *create, Token *unique, Token *index)
{
	STATS_DECL
    STMS_STATS_V("CREATE INDEX", create);

	if(index == NULL)
		return false;

	// Get index name
	Token *name = GetNextIdentToken(SQL_IDENT_OBJECT, SQL_SCOPE_INDEX);

	if(name == NULL)
		return false;

	// Save bookmark to the start of CREATE INDEX
	Bookmark(BOOK_CI_START, name, create);

	/*Token *on */ (void) GetNextWordToken("ON", L"ON", 2);

	// Get table name
	Token *table = GetNextIdentToken(SQL_IDENT_OBJECT, SQL_SCOPE_TABLE);

	if(table == NULL)
		return false;

	Book *cr_table_start = NULL;
	Book *cr_table_end = NULL;

	bool greenplum_distributed = false;

	// Get bookmarks to CREATE TABLE
	if(unique != NULL && _target == SQL_GREENPLUM)
	{
		cr_table_start = GetBookmark(BOOK_CT_START, table);
		cr_table_end = GetBookmark(BOOK_CTC_ALL_END, table);

		// Add DISTRIBUTED BY to CREATE TABLE for Greenplum
		if(cr_table_start != NULL && cr_table_end != NULL && cr_table_end->distributed_by == false)
		{
			Append(cr_table_end->book, " DISTRIBUTED BY (", L" DISTRIBUTED BY (", 17, cr_table_start->book);
			greenplum_distributed = true;
		}
	}

	// Open (
	/*Token *open */ (void) GetNextCharToken('(', L'(');

	int count = 0;

	// Handle index columns
	while(true)
	{
		Token *col = GetNextIdentToken();

		if(col == NULL)
			break;

		// Parse column as expression as it can be a function-based indes
		ParseExpression(col);

		// For Greenplum, add column to DISTRIBUTED BY in CREATE TABLE
		if(greenplum_distributed)
		{
			if(count > 0)
				Append(cr_table_end->book, ", ", L", ", 2);

			AppendCopy(cr_table_end->book, col);
		}

		count++;

		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Optional ASC or DESC keyword
		if(next->Compare("ASC", L"ASC", 3) == true || next->Compare("DESC", L"DESC", 4) == true)
			next = GetNextToken();

		// Leave if not comma
		if(Token::Compare(next, ',', L',') == true)
			continue;

		PushBack(next);
		break;
	}	

	// Close )
	/*Token *close */ (void) GetNextCharToken(')', L')');

	ParseCreateIndexOptions();

	Token *last = GetLastToken();

	// Save bookmark to the start of CREATE TABLE
	Bookmark(BOOK_CI_END, name, last);

	// Close DISTRIBUTED BY clause for Greenplum
	if(greenplum_distributed)
	{
		Append(cr_table_end->book, ")", L")", 1);
		cr_table_end->distributed_by = true;
	}

	return true;
}

// CREATE AUXILIARY TABLE in DB2
bool SqlParser::ParseCreateAuxiliary(Token *create, Token *auxiliary)
{
	if(create == NULL || auxiliary == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("CREATE AUXILIARY TABLE", create);

	// TABLE keyword
	Token *table = GetNextWordToken("TABLE", L"TABLE", 5);

	if(table == NULL)
		return false;

	Token *name = GetNextToken();

	if(name == NULL)
		return false;

	// IN database.tablespace
	Token *in = GetNextWordToken("IN", L"IN", 2);
	Token *tablespace = NULL;

	if(in != NULL)
		tablespace = GetNextToken();

	// STORES tablename
	Token *stores = GetNextWordToken("STORES", L"STORES", 6);
	Token *stores_table = NULL;

	if(stores != NULL)
		stores_table = GetNextToken();

	// COLUMN colname
	Token *column = GetNextWordToken("COLUMN", L"COLUMN", 6);
	Token *col = NULL;

	if(column != NULL)
		col = GetNextToken();

	if(_target != SQL_DB2)
		Comment(create, GetLastToken(GetNextCharToken(';', L';')));

	return true;
}

// CREATE DATABASE in DB2, MySQL
bool SqlParser::ParseCreateDatabase(Token *create, Token *database)
{
	if(create == NULL || database == NULL)
		return false;

	if(_source == SQL_ORACLE)
		return OracleCreateDatabase(create, database);

	Token *name = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(name == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("CREATE DATABASE", create);

	// IF NOT EXIST is optional for MySQL
	if(name->Compare("IF", L"IF", 2) == true)
	{
		Token *not_ = GetNextWordToken("NOT", L"NOT", 3);
		Token *exists = NULL;

		if(not_ != NULL)
			exists = GetNextWordToken("EXISTS", L"EXISTS", 6);

		// Remove for other databases
		if(not_ != NULL && exists != NULL && _target != SQL_MYSQL)
		{
			Token::Remove(name, exists);
			name = GetNextToken();

			if(name == NULL)
				return false;
		}
	}

	// DB2 CREATE DATABASE options
	if(_source == SQL_DB2)
		Db2CreateDatabase(create, database, name);
	else
	// MySQL CREATE DATABASE options
	if(_source == SQL_MYSQL)
		MysqlCreateDatabase(create, database, name);

	return true;
}

// CREATE FUNCTION
bool SqlParser::ParseCreateFunction(Token *create, Token *or_, Token *replace, Token *function)
{
	STATS_DECL
	STATS_SET_DESC(SQL_STMT_CREATE_FUNC_DESC)

	// CREATE token can be NULL for a nested function (Oracle)
	if(create == NULL && function == NULL)
		return false;

	ClearSplScope();

	_spl_scope = SQL_SCOPE_FUNC;
	_spl_start = Nvl(create, function);

	// Function name
	Token* name = GetNextIdentToken(SQL_IDENT_OBJECT, SQL_SCOPE_FUNC);

	if(name == NULL)
		return false;

	_spl_name = name;

	// Check if it is specified to convert function to procedure
	_spl_func_to_proc = IsFuncToProc(name);

	if(_spl_func_to_proc)
		TOKEN_CHANGE(function, "PROCEDURE");

	// Oracle, DB2, PostgreSQL support OR REPLACE clause (always set it)
	if(Target(SQL_ORACLE, SQL_DB2, SQL_POSTGRESQL) == true && or_ == NULL)
		Token::Change(create, "CREATE OR REPLACE", L"CREATE OR REPLACE", 17);

	// OR REPLACE clause in Oracle, PostgreSQL
	if(or_ != NULL && replace != NULL)
	{
		// DROP FUNCTION for SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Prepend(create, "IF OBJECT_ID('", L"IF OBJECT_ID('", 14);
			PrependCopy(create, name);
			Prepend(create, "', 'FN') IS NOT NULL\n", L"', 'FN') IS NOT NULL\n", 21);
			Prepend(create, "  DROP FUNCTION ", L"  DROP FUNCTION ", 16);
			PrependCopy(create, name);	
			Prepend(create, ";\nGO\n\n", L";\nGO\n\n", 6);
				
			Token::Remove(or_, replace);
		}
		else
		// DROP FUNCTION IF EXISTS in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			Prepend(create, "DROP FUNCTION IF EXISTS ", L"DROP FUNCTION IF EXISTS ", 24);
			PrependCopy(create, name);	
			Prepend(create, ";\n\n", L";\n\n", 3);
				
			Token::Remove(or_, replace);
		}
	}

	// Set delimiter for MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Prepend(create, "DELIMITER //\n\n", L"DELIMITER //\n\n", 14);

	ParseFunctionParameters(name);

	// In DB2 any function option can be mixed with RETURNS clause (1st call) 
	ParseFunctionOptions();

	// RETURN in Oracle; RETURNS in SQL Server, DB2, PostgreSQL, MySQL, Informix; RETURNING in Informix
	ParseFunctionReturns(function);

	// Second call to parse function options DETERMINISTIC, NO SQL etc. 
	ParseFunctionOptions();

	// IS or AS in Oracle; AS in SQL Server; nothing in MySQL
	Token *as = GetNextWordToken("AS", L"AS", 2);
	Token *is = NULL;

	// Oracle also allows using IS
	if(as == NULL)
	{
		is = GetNextWordToken("IS", L"IS", 2);

		// Change to AS for SQL Server
		if(_target == SQL_SQL_SERVER && is != NULL)
			Token::Change(is, "AS", L"AS", 2);
	}

	// Check for PostgreSQL $$ or $mark$ mark of the body start 
	Token *dol = GetNextCharToken('$', L'$');

	if(dol != NULL)
	{
		// $ or body$ can follow
		Token *dol2 = GetNextCharToken('$', L'$');
		Token *name = NULL;

		if(dol2 == NULL)
		{
			name = GetNextToken();
			dol2 = GetNextCharToken('$', L'$');
		}

		// If not PostgreSQL remove
		if(_target != SQL_POSTGRESQL)
			Token::Remove(dol, dol2);
	}

	ParseFunctionBody(create, function, name, Nvl(as, is));

	// Define actual sizes for parameters, cursor parameters and records
	OracleAppendDataTypeSizes();

	// In PostgreSQL body terminates with $$ LANGUAGE plpgsql;
	ParsePostgresBodyEnd();

	// In Oracle body can be terminated with /
	Token *pl_sql = GetNextCharToken('/', L'/');

	if(pl_sql != NULL)
	{
		// Replace with GO for SQL Server, as only one CREATE FUNCTION is allowed in a batch
		if(_target == SQL_SQL_SERVER)
			Token::Change(pl_sql, "GO", L"GO", 2);
        else
        // For MySQL will be later replaced with DELIMITER
        if(Target(SQL_MARIADB, SQL_MYSQL))
            Token::Remove(pl_sql);
	}
	else
	// DB2, Sybase ADS function can be terminated without any special delimiter
	if(Source(SQL_DB2, SQL_SYBASE_ADS) && _spl_delimiter_set == false)
	{
		if(_target == SQL_SQL_SERVER)
			Append(GetLastToken(), "\nGO", L"\nGO", 3, create);
		else
		if(_target == SQL_ORACLE)
			Append(GetLastToken(), "\n/", L"\n/", 2);
	}
	
	// Reset delimiter for MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Append(GetLastToken(), "\n//\n\nDELIMITER ;\n\n", L"\n//\n\nDELIMITER ;\n\n", 18);
	}

	// Replace records with variable list
	if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL))
	{
		DiscloseRecordVariables(create);
		DiscloseImplicitRecordVariables(create);
	}

	// For SQL Server move cursor declarations that depend on procedural variables
	if(Target(SQL_SQL_SERVER))
		SqlServerMoveCursorDeclarations();

	SplPostActions();

	// Remove Copy/Paste blocks
	ClearCopy(COPY_SCOPE_PROC);

	// In case of parser error can enter to this function recursively, reset variables
	ClearSplScope();

    STMS_STATS_V("CREATE FUNCTION", Nvl(create, function));
	
	return true;
}

// CREATE FUNCTION parameters
bool SqlParser::ParseFunctionParameters(Token *function_name)
{
	Token *open = GetNextCharToken('(', L'(');

	// In Oracle () cannot be used if the function is without parameters
	if(open == NULL)
	{
		// SQL Server requires () in case of empty parameter list
		if(_target == SQL_SQL_SERVER)
			Append(function_name, "()", L"()", 2);

		return true;
	}

	// First DEFAULT
	Token *first_default = NULL;

	while(true)
	{
		Token *in = NULL;
		Token *out = NULL;
		Token *inout = NULL;

		// In DB2 IN, OUT or INOUT precedes the name; in MySQL function keywords not supported, parameter is 
		// only IN; in Sybase ASA IN precedes the name
		if(Source(SQL_DB2, SQL_SYBASE_ASA) == true)
		{
			Token *param_type = GetNextToken();

			if(Token::Compare(param_type, "IN", L"IN", 2) == true)
			{
				in = param_type;

				// For SQL Server IN is not supported, assumed by default, and OUT goes after default
				if(_target == SQL_SQL_SERVER)
					Token::Remove(param_type);
			}
			else
			if(Token::Compare(param_type, "OUT", L"OUT", 3) == true)
				out = param_type;
			else
			if(Token::Compare(param_type, "INOUT", L"INOUT", 5) == true)
				inout = param_type;
			else
				PushBack(param_type);
		}

		Token *name = GetNextIdentToken(SQL_IDENT_PARAM);

		// NULL also returned in case of )
		if(name == NULL)
			break;

		// Add @ for parameter names for SQL Server and Sybase
		if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
			ConvertToTsqlVariable(name);

		_spl_parameters.Add(name);

		// In Oracle IN, OUT or IN OUT follows the name
		if(_source == SQL_ORACLE)
		{
			Token *param_type = GetNextToken();

			if(Token::Compare(param_type, "IN", L"IN", 2) == true)
			{
				in = param_type;

				// Check for IN OUT
				out = GetNextWordToken("OUT", L"OUT", 3);

				// For SQL Server and MySQL IN is not supported, assumed by default, 
				// In SQL Server OUT goes after default
				if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
					Token::Remove(param_type);
			}
			else
			if(Token::Compare(param_type, "OUT", L"OUT", 3) == true)
				out = param_type;
			else
				PushBack(param_type);

			// NOCOPY can follow in Oracle
			/*Token *nocopy */ TOKEN_GETNEXTW("NOCOPY");
		}

		Token *data_type = GetNextToken();

		// Check and resolve Oracle %TYPE, Informix LIKE variable
		bool typed = ParseTypedVariable(name, data_type);

		// Get the parameter data type
		if(typed == false)
		{
			ParseDataType(data_type, SQL_SCOPE_FUNC_PARAMS);
			ParseVarDataTypeAttribute();

			// Propagate data type to variable name
			name->data_type = data_type->data_type;
			name->data_subtype = data_type->data_subtype;
		}

		Token *next = GetNextToken();

		if(next == NULL)
			break;

		bool tsql_default = false;
		bool ora_default = false;

		// In SQL Server = denotes a default value
		if(next->Compare('=', L'=') == true)
			tsql_default = true;
		else
		// In Oracle, Informix DEFAULT keyword
		if(next->Compare("DEFAULT", L"DEFAULT", 7) == true)
			ora_default = true;
		else
		// Oracle := for default
		if(TOKEN_CMPC(next, ':'))
		{
			if(TOKEN_GETNEXT('=') != NULL)
				ora_default = true;
		}
		else
			PushBack(next);
		
		if(tsql_default == true || ora_default == true)
		{
			if(first_default == NULL)
				first_default = next;

			// Default is a constant
			Token *default_exp = GetNextToken();
			
			// SQL Server uses =
			if(ora_default == true && _target == SQL_SQL_SERVER)
				Token::Change(next, "=", L"=", 1);
			else
			// Oracle uses DEFAULT keyword
			if(tsql_default == true && _target == SQL_ORACLE)
				Token::Change(next, "DEFAULT", L"DEFAULT", 7);
			else
			// MySQL does not support default
			if(Target(SQL_MARIADB, SQL_MYSQL))
			{
				Token::Remove(next);
				Token::Remove(default_exp);
			}
		}
		// No DEFAULT for the current column, but there was a default for previous column (Informix allows this in definition)
		else
		if(first_default != NULL)
		{
			// PostgreSQL requires that all following columns included default
			if(_target == SQL_POSTGRESQL)
			{
				PrependNoFormat(next, " ", L" ", 1);
				PrependCopy(next, first_default);
				Prepend(next, " NULL ", L" NULL ", 6, first_default);
			}
		}

		// Oracle IN 
		if(_source == SQL_ORACLE && (in != NULL || out != NULL))
		{
			// In DB2 IN goes before name; in MySQL IN not used
			if(_target == SQL_DB2)
			{
				if(in != NULL)
				{
					Prepend(name, "IN ", L"IN ", 3, in);
					Token::Remove(in);
				}
			}
		}
		else
		// DB2, MySQL IN, OUT or INOUT parameter 
		if(Source(SQL_DB2, SQL_MYSQL) == true && (in != NULL || out != NULL || inout != NULL))
		{
			// In MySQL OUT/INOUT go before name, in SQL Server after default
			if(_target == SQL_SQL_SERVER)
			{
				// SQL Server does not support INOUT, always use OUT
				Append(GetLastToken(), " OUT", L" OUT", 4, Nvl(out, inout));
				
				Token::Remove(out);
				Token::Remove(inout);
			}
			else
			// In Oracle IN, OUT and IN OUT go after name
			if(_target == SQL_ORACLE)
			{
				if(in != NULL)
				{
					Append(name, " IN", L" IN", 3, in);
					Token::Remove(in);
				}
				else
				if(out != NULL)
				{
					Append(name, " OUT", L" OUT", 4, out);
					Token::Remove(out);
				}
				else
				// INOUT also changed to IN OUT
				if(inout != NULL)
				{
					Append(name, " IN OUT", L" IN OUT", 7, inout);
					Token::Remove(inout);
				}
			}
		}

		// Exit if not comma
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	_spl_param_close =  TOKEN_GETNEXT(')');

	return true;
}

// CREATE PACKAGE
bool SqlParser::ParseCreatePackage(Token *create, Token *or_, Token *replace, Token *package)
{
	if(package == NULL)
		return false;

	Token *body = TOKEN_GETNEXTW("BODY");

	// Package body
	if(body != NULL)
		ParseCreatePackageBody(create, or_, replace, package, body);
	else
		ParseCreatePackageSpec(create, or_, replace, package);

	return true;
}

// CREATE PACKAGE Specification
bool SqlParser::ParseCreatePackageSpec(Token *create, Token* /*or_*/, Token* /*replace*/, Token* /*package*/)
{
	STATS_DECL
	STATS_SET_DESC(SQL_STMT_CREATE_PACKAGE_SPEC_DESC)

	// Package name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	// Optional AUTHID CURRENT_USER | DEFINER
	Token *authid = TOKEN_GETNEXTW("AUTHID");
	/*Token *authid_value */ GetNext(authid);

	Token *as = TOKEN_GETNEXTW("AS");

	if(as == NULL)
		as = TOKEN_GETNEXTW("IS");

	_spl_package_spec = name;

	// Parse declarations
	ParseOracleVariableDeclarationBlock(as);

	Token *end = TOKEN_GETNEXTW("END");

	// In Oracle ; must follow END proc_name
	Token *semi = TOKEN_GETNEXTP(end, ';');

	// In Oracle procedure name can be specified after END
	if(end != NULL && semi == NULL)
	{
		if(ParseSplEndName(name, end) == true)
			semi = TOKEN_GETNEXT(';');
	}

	// In Oracle body can be terminated with /
	/*Token *pl_sql */ TOKEN_GETNEXT('/');

	_spl_package_spec = NULL;

	STMS_STATS_V("CREATE PACKAGE SPEC", create);

	return true;
}

// CREATE PACKAGE BODY
bool SqlParser::ParseCreatePackageBody(Token *create, Token *or_, Token *replace, Token *package, Token *body)
{
	STATS_DECL
	STATS_SET_DESC(SQL_STMT_CREATE_PACKAGE_BODY_DESC)

	if(body == NULL)
		return false;

	// Package name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	_spl_package = name;

	Token *as = TOKEN_GETNEXTW("AS");

	if(as == NULL)
		as = TOKEN_GETNEXTW("IS");

	if(_target_app == APP_JAVA)
	{
		TOKEN_CHANGE_NOFMT(create, "public class");
		TOKEN_CHANGE_NOFMT(as, "{");
		Token::Remove(Nvl(or_, replace, package), body);
	}
	else
	// Remove if not Oracle
	if(_target != SQL_ORACLE)
	{
		Token::Remove(create, replace);
		Token::Remove(as);

		Comment(package, name);
	}

	// Parse procedures and functions until END;
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Procedure
		if(TOKEN_CMP(next, "PROCEDURE"))
		{
			Token *create = (_target_app != APP_JAVA) ? Prepend(next, "CREATE ", L"CREATE ", 7) : NULL;

			ParseCreateProcedure(create, or_, replace, next, NULL);
			continue;
		}
		else
		// Function
		if(TOKEN_CMP(next, "FUNCTION"))
		{
			Token *create = Prepend(next, "CREATE ", L"CREATE ", 7);

			ParseCreateFunction(create, or_, replace, next);
			continue;
		}
		else
		// Package initialization block
		if(TOKEN_CMP(next, "BEGIN"))
		{
			ParseBlock(SQL_BLOCK_PROC, true, SQL_SCOPE_PROC, NULL);

			// Instance initializer in Java
			if(_target_app == APP_JAVA)
			{
				TOKEN_CHANGE_NOFMT(next, "{");
				APPEND_NOFMT(GetLastToken(), "\n}");
			}

			continue;
		}
		else
		// End of package
		if(TOKEN_CMP(next, "END"))
		{
			Token *end = next;
			Token *semi = TOKEN_GETNEXT(';');

			// Package name can be after END
			if(semi == NULL)
			{
				if(ParseSplEndName(name, end))
					semi = TOKEN_GETNEXT(';');
			}

			Token *run_pl_sql = TOKEN_GETNEXT('/');
			
			if(_target_app == APP_JAVA)
			{
				TOKEN_CHANGE_NOFMT(end, "}");
				Token::Remove(semi);
				Token::Remove(run_pl_sql);
			}
			
			break;
		}

		PushBack(next);

		// Try to parse declaration
		if(ParseOracleVariableDeclarationBlock(as))
			continue;

		break;
	}

    STMS_STATS_V("CREATE PACKAGE BODY", create);

	_spl_package = NULL;

	return true;
}

// CREATE PROCEDURE
bool SqlParser::ParseCreateProcedure(Token *create, Token *or_, Token *replace, Token *procedure, int *result_sets)
{
	if(procedure == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_STMT_CREATE_PROC_DESC)

	ClearSplScope();

	_spl_scope = SQL_SCOPE_PROC;
	_spl_start = Nvl(create, procedure);

	AddEvalModeComment(_spl_start);

	if(_target_app == APP_JAVA)
	{
		TOKEN_CHANGE_NOFMT(Nvl(create, procedure), "public static void");

		if(create != NULL)
			Token::Remove(procedure);
	}

	// Change PROC word
	if(_target != SQL_SYBASE && _target != SQL_SQL_SERVER)
	{
		if(procedure->Compare("PROC", L"PROC", 4) == true)
			Token::Change(procedure, "PROCEDURE", L"PROCEDURE", 9);
	}

	// Oracle, DB2, PostgreSQL support OR REPLACE clause (always set it)
	if(Target(SQL_ORACLE, SQL_DB2, SQL_POSTGRESQL) == true && or_ == NULL)
	{
		Token::Change(create, "CREATE OR REPLACE", L"CREATE OR REPLACE", 17);

		if(Token::IsBlank(procedure->prev) == false)
			PrependNoFormat(procedure, " ", L" ", 1);
	}

	// PostgerSQL supports only CREATE FUNCTION
	if(_target == SQL_POSTGRESQL)
		Token::Change(procedure, "FUNCTION", L"FUNCTION", 8);

	// Procedure name
	Token* name = GetNextIdentToken(SQL_IDENT_OBJECT, SQL_SCOPE_PROC);

	if(name == NULL)
		return false;

	_spl_name = name;

	// OR REPLACE clause in Oracle
	if(or_ != NULL && replace != NULL)
	{
		// DROP PROCEDURE for SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Prepend(create, "IF OBJECT_ID('", L"IF OBJECT_ID('", 14);
			PrependCopy(create, name);
			PrependNoFormat(create, "', 'P')", L"', 'P'", 7);
			Prepend(create, " IS NOT NULL\n", L" IS NOT NULL\n", 13);
			Prepend(create, "  DROP PROCEDURE ", L"  DROP PROCEDURE ", 17);
			PrependCopy(create, name);	
			Prepend(create, ";\nGO\n\n", L";\nGO\n\n", 6);
				
			Token::Remove(or_, replace);
		}
		else
		// DROP PROCEDURE IF EXISTS in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			Prepend(create, "DROP PROCEDURE IF EXISTS ", L"DROP PROCEDURE IF EXISTS ", 25);
			PrependCopy(create, name);	
			Prepend(create, ";\n\n", L";\n\n", 3);
				
			Token::Remove(or_, replace);
		}
	}

	// Set delimiter for MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Prepend(create, "DELIMITER //\n\n", L"DELIMITER //\n\n", 14);

	int param_count = 0;
	Token *last_param = NULL;

	ParseProcedureParameters(name, &param_count, &last_param);

	// AS can go before options, for example AS LANGUAGE JAVA in Oracle
	Token *as = TOKEN_GETNEXTW("AS");
	
	// Procedure options DETERMINISTIC, NO SQL etc.
	ParseProcedureOptions(create);

	// External stored procedure written in C, Java, COBOL etc., no body after header declaration
	if(_spl_external == true)
	{
	    STMS_STATS_V("CREATE PROCEDURE", create);
		return true;
	}

	// SQL Server, Oracle, Sybase require AS before body; DB2, MySQL and Informix do not allow AS
	if(as == NULL)
		as = TOKEN_GETNEXTW("AS");

	Token *is = NULL;

	// Oracle also allows using IS
	if(as == NULL)
	{
		is = GetNextWordToken("IS", L"IS", 2);

		if(_target_app == APP_JAVA)
			TOKEN_CHANGE_NOFMT(Nvl(as, is), "{");
		else
		// Change to AS for other databases
		if(Target(SQL_ORACLE, SQL_MARIADB_ORA) == false && is != NULL)
			Token::Change(is, "AS", L"AS", 2);
	}

	ParseProcedureBody(create, procedure, name, Nvl(as, is), result_sets);

	// Add cursor parameters if the procedure returns a result set
	if(_target == SQL_ORACLE)
		OracleAddOutRefcursor(create, name, last_param);

	if(result_sets != NULL && *result_sets > 0)
	{
		if(_target == SQL_ORACLE)
		{
			if(param_count == 0)
				Append(name, " (cur OUT SYS_REFCURSOR)", L" (cur OUT SYS_REFCURSOR)", 24);
		}
	}

	// In Oracle body can be terminated with /
	Token *pl_sql = GetNextCharToken('/', L'/');

	if(pl_sql != NULL)
	{
        // Replace with MySQL delimiter // and reset it to ; 
        if(Target(SQL_MARIADB, SQL_MYSQL))
            TOKEN_CHANGE(pl_sql, "//\n\nDELIMITER ;\n\n");
		// Not required for PostgreSQL
		if(Target(SQL_POSTGRESQL))
			Token::Remove(pl_sql);
        else
		// Replace with GO for SQL Server, as only one CREATE PROCEDURE is allowed in a batch
		if(_target == SQL_SQL_SERVER)
			TOKEN_CHANGE(pl_sql, "GO");
		else
		// Replace with END_PROC; for Netezza
		if(_target == SQL_NETEZZA)
			TOKEN_CHANGE_FMT(pl_sql, "END_PROC;", create);
	}
	else
	// In DB2 procedure can be terminated without any special delimiter
	if(Source(SQL_DB2, SQL_MYSQL) && _spl_delimiter_set == false)
	{
		if(_target == SQL_ORACLE)
			Append(GetLastToken(), "\n/", L"\n/", 2, create);
		else
		if(_target == SQL_SQL_SERVER)
			Append(GetLastToken(), "\nGO", L"\nGO", 3, create);
	}
	else
	// Reset delimiter for MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Append(GetLastToken(), "\n//\n\nDELIMITER ;\n\n", L"\n//\n\nDELIMITER ;\n\n", 18);
	}

	// For PostgreSQL add $$ LANGUAGE plpgsql;
	if(_source != SQL_POSTGRESQL && _target == SQL_POSTGRESQL)
	{
		Token *last = GetLastToken();

		Append(last, "\n$$ LANGUAGE", L"\n$$ LANGUAGE", 12, create);
		AppendNoFormat(last, " plpgsql;", L" plpgsql;", 9);
	}

	// Replace records with variable list
	if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL))
	{
		DiscloseRecordVariables(create);
		DiscloseImplicitRecordVariables(create);
	}

	// For SQL Server move cursor declarations that depend on procedural variables
	if(Target(SQL_SQL_SERVER))
		SqlServerMoveCursorDeclarations();

	SplPostActions();

    STMS_STATS_V("CREATE PROCEDURE", Nvl(create, procedure));

	return true;
}

// Actions after converting each procedure, function or trigger
void SqlParser::SplPostActions()
{
	// Check if temporary table need to be truncated at the beginning of block in Oracle
	if(_source == SQL_DB2 && _target == SQL_ORACLE)
		OracleTruncateTemporaryTablesWithReplace();

	// Add declarations for SELECT statement moved out of IF condition
	if(_spl_moved_if_select > 0 && _target == SQL_ORACLE)
		OracleIfSelectDeclarations();

	// Remove Copy/Paste blocks
	ClearCopy(COPY_SCOPE_PROC);

	// In case of parser error can enter to this function recursively, reset variables
	ClearSplScope();

	return;
}

// CREATE TRIGGER statement 
bool SqlParser::ParseCreateTrigger(Token *create, Token *or_, Token *trigger)
{
	STATS_DECL
    STMS_STATS_V("CREATE TRIGGER", create);

	if(trigger == NULL)
		return false;

	ClearSplScope();

	_spl_scope = SQL_SCOPE_TRIGGER;
	_spl_start = create;

	// Oracle, DB2 support OR REPLACE clause; always set it
	if(Target(SQL_ORACLE, SQL_DB2) == true && or_ == NULL)
		Token::Change(create, "CREATE OR REPLACE", L"CREATE OR REPLACE", 17);

	// Trigger name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	_spl_name = name;

	// NO CASCADE BEFORE in DB2
	Token *no = GetNextWordToken("NO", L"NO", 2);

	if(no != NULL)
	{
		Token *cascade = GetNextWordToken("CASCADE", L"CASCADE", 7);

		if(_target == SQL_ORACLE && cascade != NULL)
			Token::Remove(no, cascade);
	}

	Token *on = NULL;
	Token *table = NULL;

	// Table name in Sybase ADS
	if(_source == SQL_SYBASE_ADS)
	{
		on = TOKEN_GETNEXTW("ON");
		table = GetNextIdentToken();
	}

	// BEFORE, AFTER, INSTEAD OF
	Token *when = GetNextWordToken("BEFORE", L"BEFORE", 6);

	if(when == NULL)
	{
		when = GetNextWordToken("AFTER", L"AFTER", 5);

		if(when == NULL)
		{
			when = GetNextWordToken("INSTEAD", L"INSTEAD", 7);

			// OF keyword after INSTEAD
			if(when != NULL)
				/*Token *of */ (void) GetNextWordToken("OF", L"OF", 2);
		}
	}

	// INSERT, UPDATE or DELETE
	Token *insert = NULL;
	Token *update = NULL;
	Token *delete_ = NULL;

	// List of events can be specified
	while(true)
	{
		Token *operation = GetNextToken();

		if(operation == NULL)
			break;

		if(operation->Compare("INSERT", L"INSERT", 6) == true)
			insert = operation;
		else
		if(operation->Compare("DELETE", L"DELETE", 6) == true)
			delete_ = operation;
		else
		if(operation->Compare("UPDATE", L"UPDATE", 6) == true)
		{
			update = operation;

			// OF col, ... for UPDATE operation
			Token *of = GetNextWordToken("OF", L"OF", 2);

			// Column list
			while(of != NULL)
			{
				/*Token *col */ (void) GetNextToken();
				Token *comma = GetNextCharToken(',', L',');

				if(comma == NULL)
					break;
			}
		}
		else
		{
			PushBack(operation);
			break;
		}

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	// ON table name
	if(_source != SQL_SYBASE_ADS)
	{
		on = TOKEN_GETNEXTW("ON");
		table = GetNextIdentToken();
	}

	if(_target == SQL_SQL_SERVER)
	{
		// In SQL Server ON TABLE goes before type and event
		if(!Source(SQL_SQL_SERVER, SQL_SYBASE_ADS))
		{
			AppendSpaceCopy(name, on, table); 
			Token::Remove(on, table);
		}

		// BEFORE is not supported in SQL Server, convert to INSTEAD OF
		if(when->Compare("BEFORE", L"BEFORE", 6) == true)
			Token::Change(when, "INSTEAD OF", L"INSTEAD OF", 10);
	}

	// REFERENCING NEW [AS] name OLD [AS] name
	Token *referencing = GetNextWordToken("REFERENCING", L"REFERENCING", 11);

	// REFERENCING not supported in SQL Server
	if(_target == SQL_SQL_SERVER && referencing != NULL)
		Token::Remove(referencing);

	while(referencing != NULL)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// NEW [AS] name
		if(next->Compare("NEW", L"NEW", 3) == true)
		{
			// AS is optional in Oracle, DB2
			/*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

			_spl_new_correlation_name = GetNextToken();		

			if(_target == SQL_SQL_SERVER)
				Token::Remove(next, _spl_new_correlation_name);

			continue;
		}
		else
		// OLD [AS] name
		if(next->Compare("OLD", L"OLD", 3) == true)
		{
			// AS is optional in Oracle, DB2
			/*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

			_spl_old_correlation_name = GetNextToken();

			if(_target == SQL_SQL_SERVER)
				Token::Remove(next, _spl_old_correlation_name);

			continue;
		}
		else
		// NEW_TABLE [AS] name in DB2
		if(next->Compare("NEW_TABLE", L"NEW_TABLE", 9) == true)
		{
			// AS is optional 
			/*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

			/*Token *name */ (void) GetNextToken();			
			continue;
		}
		else
		// OLD_TABLE [AS] name
		if(next->Compare("OLD_TABLE", L"OLD_TABLE", 9) == true)
		{
			// AS is optional
			/*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

			/*Token *name */ (void) GetNextToken();
			continue;
		}

		PushBack(next);
		break;
	}

	// FOR EACH ROW or FOR EACH STATEMENT
	Token *for_ = GetNextWordToken("FOR", L"FOR", 3);
	Token *each = NULL;	
	Token *row = NULL;
	Token *statement = NULL;

	if(for_ != NULL)
		each = GetNextWordToken("EACH", L"EACH", 4);

	if(each != NULL) 
		row = GetNextWordToken("ROW", L"ROW", 3);

	if(row == NULL) 
		statement = GetNextWordToken("STATEMENT", L"STATEMENT", 9);

	// MODE DB2SQL in DB2 for z/OS
	Token *mode = GetNextWordToken("MODE", L"MODE", 4);

	if(mode != NULL)
	{
		Token *db2sql = GetNextWordToken("DB2SQL", L"DB2SQL", 6);

		if(_target == SQL_ORACLE && db2sql != NULL)
			Token::Remove(mode, db2sql);
	}

	// WHEN (condition)
	Token *when2 = GetNextWordToken("WHEN", L"WHEN", 4);

	if(when2 != NULL)
	{
		Enter(SQL_SCOPE_TRG_WHEN_CONDITION);

		// Parse trigger condition
		ParseBooleanExpression(SQL_BOOL_TRIGGER_WHEN);

		Leave(SQL_SCOPE_TRG_WHEN_CONDITION);
	}

	// AS keyword (not allowed in Sybase ASA)
	Token *as = GetNextWordToken("AS", L"AS", 2);

	if(as == NULL)
	{
		// AS keyword required in SQL Server
		if(_target == SQL_SQL_SERVER)
			Append(GetLastToken(), " AS", L" AS", 3);
	}

	Token *end = NULL;

	ParseCreateTriggerBody(create, name, table, when, insert, update, delete_, &end);

	// PRIORITY num in Sybase ADS
	Token *priority = TOKEN_GETNEXTW("PRIORITY");

	if(priority != NULL)
	{
		Token *num = GetNextNumberToken();

		if(_target != SQL_SYBASE_ADS)
			Token::Remove(priority, num);
	}

	// In DB2 procedure can be terminated without any special delimiter
	if(Source(SQL_DB2) && Target(SQL_ORACLE) && _spl_delimiter_set == false)
		Append(GetLastToken(), "\n/", L"\n/", 2);

	// FOR EACH ROW not supported in SQL Server
	if(_target == SQL_SQL_SERVER && row != NULL)
	{
		// Convert using inserted and deleted system tables
		SqlServerConvertRowLevelTrigger(table, when, insert, update, delete_, end);

		Token::Remove(for_, each);
		Token::Remove(row);
	}

	// Remove Copy/Paste blocks
	ClearCopy(COPY_SCOPE_PROC);

	// In case of parser error can enter to this function recursively, reset variables
	ClearSplScope();

	return true;
}

// Body of CREATE TRIGGER statement
bool SqlParser::ParseCreateTriggerBody(Token *create, Token *name, Token *table, Token *when, 
											Token *insert, Token * /*update*/, Token * /*delete_*/, Token **end_out)
{
	// In DB2 block can start with a label 
	ParseLabelDeclaration(NULL, true);

	Token *begin = GetNextWordToken("BEGIN", L"BEGIN", 5);

	Token *atomic = NULL;

	// BEGIN ATOMIC in DB2
	if(begin != NULL)
	{
		atomic = GetNextWordToken("ATOMIC", L"ATOMIC", 6);

		if(_target == SQL_ORACLE)
			Token::Remove(atomic);
	}

	// Try to recognize a pattern in the trigger body
	bool pattern = ParseCreateTriggerOraclePattern(create, table, when, insert);

	// If pattern matched, remove the trigger
	if(pattern == true)
	{
		Token *end = GetNextWordToken("END", L"END", 3);
		Token *semi = GetNextCharToken(';', L';');

		Token *last = (semi == NULL) ? end : semi;

		Token::Remove(create, last);
	}

	// SQL Server, MySQL trigger may not have BEGIN, add it for other databases 
	if(begin == NULL && Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == false)
	{
		// Check for Oracle DECLARE block
		if((Source(SQL_ORACLE) && LOOKNEXT("DECLARE") == NULL) ||
			// DB2 single statement trigger may not have BEGIN-END
			Source(SQL_DB2))
			Append(GetLastToken(), "\nBEGIN", L"\nBEGIN", 6, create);
	}

	bool frontier = (begin != NULL) ? true : false;

	ParseBlock(SQL_BLOCK_PROC, frontier, SQL_SCOPE_TRIGGER, NULL);

	Token *end = GetNextWordToken("END", L"END", 3);
	
	// END required for Oracle trigger
	if(begin == NULL && end == NULL && Target(SQL_ORACLE) == true)
		Append(GetLastToken(), "\nEND;", L"\nEND;", 5, create);
	
	// In Oracle ; must follow END name, in SQL Server, MySQL ; is optional after END
	Token *semi = GetNextCharToken(';', L';');

	// In Oracle procedure name, in DB2 label name can be specified after END
	if(semi == NULL)
	{
		if(ParseSplEndName(name, end) == true)
			semi = GetNextCharToken(';', L';');

		// For Oracle append ; after END
		if(end != NULL && semi == NULL)
			AppendNoFormat(end, ";", L";", 1);
	}

	// User-defined delimiter can be specified for MySQL, often // @ (also used for DB2)
	ParseMySQLDelimiter(create);

	if(end_out != NULL)
		*end_out = end;

	return true;
}

// SQL Server, DB2, MySQL DECLARE variable statement; Oracle DECLARE block (inner or anonymous)
bool SqlParser::ParseDeclareStatement(Token *declare)
{
	if(declare == NULL)
		return false;

	_declare_format = declare;

	// Check for Oracle inner or anonymous DECLARE block
	if(_source == SQL_ORACLE)
	{
		// If the scope is not set yet, it means anonymous block for Oracle
		if(_spl_scope == 0)
			_spl_scope = SQL_SCOPE_PROC;

		bool exists = ParseOracleVariableDeclarationBlock(declare);

		if(exists == true)
			return true;
	}

	int cnt_list = 0;

	while(true)
	{
		// Variable name of unknown type yet (local variable, cursor, table, condition etc.)
		Token *name = GetNextToken();

		if(name == NULL)
			break;

		// Check for HANDLER
		if(ParseDeclareHandler(declare, name) == true)
			break;

		// Check for GLOBAL TEMPORARY TABLE in DB2
		if(ParseDeclareGlobalTemporaryTable(declare, name) == true)
			break;

		// Check for LOCAL TEMPORARY TABLE in Sybase ASA
		if(ParseDeclareLocalTemporaryTable(declare, name) == true)
			break;

		// Optional AS keyword before the data type in SQL Server
		Token *as = GetNext("AS", L"AS", 2);

		// AS not allowed in other databases
		if(as != NULL && _target != SQL_SQL_SERVER)
			Token::Remove(as);

		Token *type = GetNextToken();

		if(type == NULL)
			break;

		// DB2, MySQL DECLARE CURSOR
		if(type->Compare("CURSOR", L"CURSOR", 6) == true)
			ParseDeclareCursor(declare, name, type);
		else
		// DB2 DECLARE name CONDITION; Sybase ASA DECLARE name EXCEPTION
		if(type->Compare("CONDITION", L"CONDITION", 9) == true || type->Compare("EXCEPTION", L"EXCEPTION", 9) == true)
			ParseDeclareCondition(declare, name, type);
		else
		// SQL Server TABLE
		if(type->Compare("TABLE", L"TABLE", 5) == true)
			ParseDeclareTable(declare, name, type);
		else
		// DB2 RESULT_SET_LOCATOR
		if(type->Compare("RESULT_SET_LOCATOR", L"RESULT_SET_LOCATOR", 18) == true)
			ParseDb2ResultSetLocatorDeclaration(declare, name, type);
		// Variable declaration
		else
			ParseDeclareVariable(declare, name, type, cnt_list);
		
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		cnt_list++;

		// Oracle and PostgreSQL use a statement for each declaration
		if(Target(SQL_ORACLE, SQL_POSTGRESQL))
			TOKEN_CHANGE(comma, ";");
        else
        // MySQL and MariaDB require DECLARE for each variable
        if(Target(SQL_MYSQL, SQL_MARIADB))
        {
			Token *next_name = GetNextToken();

			// If the next variable are on the same line put DECLARE on this line as well
			if(next_name != NULL && next_name->line == comma->line)
				AppendWithSpaceAfter(comma, " DECLARE", L" DECLARE", 8, declare); 
			// otherwise move DECLARE to the variable name line
			else
				PREPEND_FMT(next_name, "DECLARE ", declare); 
			
			TOKEN_CHANGE(comma, ";");
			PushBack(next_name);
        }
	}

	return true;
}

// Informix DEFINE variable statement
bool SqlParser::ParseDefineStatement(Token *define)
{
	if(define == NULL)
		return false;

	_declare_format = define;

	// Variable name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	Token *type = GetNextToken();

	if(type == NULL)
		return false;

	ParseDeclareVariable(define, name, type, 0);

	return true;
}

// DELETE statement
bool SqlParser::ParseDeleteStatement(Token *delete_)
{
	if(delete_ == NULL)
		return false;

    // DEL in Teradata
    if(_source == SQL_TERADATA && _target != SQL_TERADATA && TOKEN_CMP(delete_, "DEL") == true)
        TOKEN_CHANGE(delete_, "DELETE");

	STATS_DECL
    STMS_STATS(delete_);

    Token *name = NULL;
    Token *from = NULL;
    Token *next = GetNext();

    bool delete_from = false;

    if(next == NULL)
        return false;

    // DELETE FROM tab1 ...
    if(TOKEN_CMP(next, "FROM") == true)
    {
        from = next;

        // Table name
	    name = GetNextIdentToken();

	    if(name == NULL)
		    return false;

        // Table alias or WHERE
	    next = GetNextToken();

	    if(next == NULL)
		    return false;
    }
    // DELETE t1 FROM tab1 t1, tab2 t2 ...
    else
    {
        // Alias for deleted table
	    name = next;

        from = TOKEN_GETNEXTW("FROM");

        if(name == NULL || from == NULL)
		    return false;

        int num = 0;

        while(true)
        {
            // Table name
	        Token *tab = GetNextIdentToken(SQL_IDENT_OBJECT);

            // Alias 
            /*Token *alias */ (void) GetNextToken();

            Token *comma = TOKEN_GETNEXT(',');

            // Convert to MERGE USING in EsgynDB
            if(_target == SQL_ESGYNDB)
            {
                if(num == 0)
                    TOKEN_CHANGE(comma, " USING");
                else
                {
                    Prepend(tab, "(SELECT * FROM ", L"(SELECT * FROM ", 15, delete_);
                    AppendNoFormat(tab, ")", L")", 1);
                }
            }

            if(comma == NULL)
                break;

            num++;
        }

        delete_from = true;
    }

	_spl_last_fetch_cursor_name = NULL;	

	Token *alias = NULL;

	// Check for WHERE
	if(next->Compare("WHERE", L"WHERE", 5) == true)
		PushBack(next);
	else
		alias = next;

    Token *where_ = NULL;

	// Optional WHERE clause
	bool where_exists = ParseWhereClause(SQL_STMT_DELETE, &where_, NULL, NULL);

	// If there is no WHERE return alias to input if no followed by ; may be it is just next statement start word
	if(where_exists == false && alias != NULL)
	{
		Token *semi = GetNextCharToken(';', L';');

		if(semi == NULL)
			PushBack(alias);
		else
			PushBack(semi);
	}

    if(delete_from == true)
    {
        // Convert to MERGE in EsgynDB
        if(_target == SQL_ESGYNDB)
        {
            TOKEN_CHANGE(delete_, "MERGE");
            TOKEN_CHANGE(from, "INTO");
            TOKEN_CHANGE(where_, "ON");

            Append(GetLastToken(), "\nWHEN MATCHED THEN DELETE", L"\nWHEN MATCHED THEN DELETE", 25, Nvl(where_, delete_));

            Token::Remove(name);
        }
    }

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// DELIMITER statement in MySQL
bool SqlParser::ParseDelimiterStatement(Token *delimiter)
{
	if(delimiter == NULL)
		return false;

	// Get delimiter, often //
	Token *sep = GetNextUntilNewlineToken();

	if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
		Token::Remove(delimiter, sep);

	return true;
}

// DROP statement
bool SqlParser::ParseDropStatement(Token *drop)
{
	if(drop == NULL)
		return false;

	bool exists = false;

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	// DROP DATABASE
	if(next->Compare("DATABASE", L"DATABASE", 8) == true)
		exists = ParseDropDatabaseStatement(drop, next);
	else
	// DROP TABLE
	if(next->Compare("TABLE", L"TABLE", 5) == true)
		exists = ParseDropTableStatement(drop, next);
	else
	// DROP PROCEDURE
	if(next->Compare("PROCEDURE", L"PROCEDURE", 9) == true)
		exists = ParseDropProcedureStatement(drop, next);
	else
	// DROP TRIGGER
	if(next->Compare("TRIGGER", L"TRIGGER", 7) == true)
		exists = ParseDropTriggerStatement(drop, next);
	else
	// DROP SCHEMA
	if(next->Compare("SCHEMA", L"SCHEMA", 6) == true)
		exists = ParseDropSchemaStatement(drop, next);
	else
	// DROP SEQUENCE
	if(next->Compare("SEQUENCE", L"SEQUENCE", 8) == true)
		exists = ParseDropSequenceStatement(drop, next);
	else
	// DROP STOGROUP in DB2 for z/OS
	if(next->Compare("STOGROUP", L"STOGROUP", 8) == true)
		exists = ParseDropStogroupStatement(drop, next);

	return exists;
}

// DROP DATABASE statement in DB2, MySQL
bool SqlParser::ParseDropDatabaseStatement(Token *drop, Token *database)
{
	if(drop == NULL || database == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("DROP DATABASE", drop);

	// IF EXISTS in MySQL
	Token *if_ = GetNext("IF", L"IF", 2);
	Token *exists = GetNext(if_, "EXISTS", L"EXISTS", 6);

	// Database name
	Token *name = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(name == NULL)
		return false;

	if(_target == SQL_ORACLE)
	{
		// DB2 database not mapped to schema in Oracle, so comment the statement
		if(_source == SQL_DB2)
			Comment(drop, GetLastToken(GetNextCharToken(';', L';')));
		else
		// Use DROP USER in Oracle
		if(_source == SQL_MYSQL)
		{
		
			Token::Change(database, "USER", L"USER", 4);

			// CASCADE must be added to drop a user having objects
			Append(name, " CASCADE", L" CASCADE", 8, drop);

			Token::Remove(if_, exists);
		}
	}

	return true;
}

// DROP TABLE statement
bool SqlParser::ParseDropTableStatement(Token *drop, Token *table)
{
	if(drop == NULL || table == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("DROP TABLE", drop);

	Token *table_name = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(table_name == NULL)
		return false;

	bool drop_if_exists = false;

	// IF EXISTS in MySQL
	if(table_name->Compare("IF", L"IF", 2) == true)
	{
		Token *exists = GetNextWordToken("EXISTS", L"EXISTS", 6);

		// Remove keywords for Oracle
		if(_target == SQL_ORACLE)
			Token::Remove(table_name, exists);

		drop_if_exists = true;

		table_name = GetNextIdentToken();
	}

	// Add PL/SQL block to implement IF EXISTS in Oracle
	if(drop_if_exists == true && _target == SQL_ORACLE)
	{
		Prepend(drop, "BEGIN\n   EXECUTE IMMEDIATE '", L"BEGIN\n   EXECUTE IMMEDIATE '", 28);
		Append(table_name, "';\nEXCEPTION\n   WHEN OTHERS THEN NULL;\nEND;\n/", 
			              L"';\nEXCEPTION\n   WHEN OTHERS THEN NULL;\nEND;\n/", 45, drop);

		// If ; follows the DROP remove it as we already set all delimiters
		Token *semi = GetNextCharToken(';', L';');

		if(semi != NULL)
			Token::Remove(semi);
	}

	// For Oracle, check if a temporary table is removed
	if(_target == SQL_ORACLE)
	{
		for(ListwItem *i = _spl_created_session_tables.GetFirst(); i != NULL; i = i->next)
		{
			Token *temp = (Token*)i->value;

			if(Token::Compare(table_name, temp) == true)
			{
				// Use TRUNCATE as it is a permanent object in Oracle
				Token::Change(drop, "TRUNCATE", L"TRUNCATE", 8);

				// If inside a procedure block surround with EXECUTE IMMEDIATE
				if(_spl_scope == SQL_SCOPE_PROC || _spl_scope == SQL_SCOPE_FUNC || _spl_scope == SQL_SCOPE_TRIGGER)
				{
					Prepend(drop, "EXECUTE IMMEDIATE '", L"EXECUTE IMMEDIATE '", 19); 
					AppendNoFormat(table_name, "'", L"'", 1);
				}

				break;
			}
		}
	}

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();
	  
	return true;
}

// DROP PROCEDURE statement
bool SqlParser::ParseDropProcedureStatement(Token *drop, Token *procedure)
{
	if(drop == NULL || procedure == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("DROP PROCEDURE", drop);

	Token *name = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(name == NULL)
		return false;	
  
	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// DROP TRIGGER statement
bool SqlParser::ParseDropTriggerStatement(Token *drop, Token *trigger)
{
	if(drop == NULL || trigger == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("DROP TRIGGER", drop);

	Token *trigger_name = GetNextIdentToken();

	if(trigger_name == NULL)
		return false;

	bool drop_if_exists = false;

	// IF EXISTS in MySQL
	if(trigger_name->Compare("IF", L"IF", 2) == true)
	{
		Token *exists = GetNextWordToken("EXISTS", L"EXISTS", 6);

		// Remove keywords for Oracle
		if(_target == SQL_ORACLE)
			Token::Remove(trigger_name, exists);

		drop_if_exists = true;

		trigger_name = GetNextIdentToken();
	}

	// Add PL/SQL block to implement IF EXISTS in Oracle
	if(drop_if_exists == true && _target == SQL_ORACLE)
	{
		Prepend(drop, "BEGIN\n   EXECUTE IMMEDIATE '", L"BEGIN\n   EXECUTE IMMEDIATE '", 28);
		Append(trigger_name, "';\nEXCEPTION\n   WHEN OTHERS THEN NULL;\nEND;\n/", 
			              L"';\nEXCEPTION\n   WHEN OTHERS THEN NULL;\nEND;\n/", 45, drop);

		// If ; follows the DROP remove it as we already set all delimiters
		Token *semi = GetNextCharToken(';', L';');

		if(semi != NULL)
			Token::Remove(semi);
	}
  
	return true;
}

// DROP SCHEMA in MySQL
bool SqlParser::ParseDropSchemaStatement(Token *drop, Token *schema)
{
	if(drop == NULL || schema == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("DROP SCHEMA", drop);

	// IF EXISTS in MySQL
	Token *if_ = GetNext("IF", L"IF", 2);
	Token *exists = GetNext(if_, "EXISTS", L"EXISTS", 6);
	
	// Database name
	Token *name = GetNextIdentToken(SQL_IDENT_OBJECT);

	// DROP USER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(schema, "USER", L"USER", 4);

		// CASCADE must be added to drop a user having objects
		Append(name, " CASCADE", L" CASCADE", 8, drop);

		Token::Remove(if_, exists);
	}

	return true;
}

// Oracle DROP SEQUENCE
bool SqlParser::ParseDropSequenceStatement(Token *drop, Token *sequence)
{
	if(drop == NULL || sequence == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("DROP SEQUENCE", drop);
	int conv_stats = STATS_CONV_UNDEFINED;

	Token *seq_name = GetNextIdentToken();

	if(seq_name == NULL)
		return false;

	if(Target(SQL_MARIADB))
		conv_stats = STATS_CONV_NO_NEED;
	else
	// A stored procedure call in MySQL
	if(Target(SQL_MYSQL))
	{
		Token::Change(drop, "CALL", L"CALL", 4);
		Token::Remove(sequence);

		PrependNoFormat(seq_name, "DropSequence('", L"DropSequence('", 14);
		Append(seq_name, "')", L"')", 2);
	}

	SEQ_STATS_V("DROP SEQUENCE", drop);
	return true;
}

// DB2 for z/OS DROP STOGROUP
bool SqlParser::ParseDropStogroupStatement(Token *drop, Token *stogroup)
{
	if(drop == NULL || stogroup == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("DROP STOGROUP", drop);

	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	// Comment the entire statement for other databases
	if(_target != SQL_DB2)
	{
		Token *end = GetLastToken(GetNextCharToken(';', L';'));
		Comment(drop, end);
	}

	return true;
}

// SQL Server, Sybase ASE EXEC statement
bool SqlParser::ParseExecStatement(Token *exec)
{
	if(exec == NULL)
		return false;

	// EXECUTE in Oracle, MySQL
	if(Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
		Token::Change(exec, "EXECUTE", L"EXECUTE", 7);

	return ParseExecuteStatement(exec);
}

// Oracle, Teradata EXECUTE IMMEDIATE; Teradata, MySQL, PostgreSQL EXECUTE statement; 
// SQL Server, Sybase ASE EXECUTE/EXEC; 
bool SqlParser::ParseExecuteStatement(Token *execute)
{
	if(execute == NULL)
		return false;

    STATS_DECL

	// Check for executing a system procedure in SQL Server, Sybase ASE (exec sp_addtype i.e.)
	if(ParseSystemProcedure(execute, NULL) == true)
	{
		PL_STMS_STATS(execute);
		return true;
	}

	// Sybase ADS EXECUTE PROCEDURE statement 
	if(_source == SQL_SYBASE_ADS && ParseSybaseExecuteProcedureStatement(execute))
		return true;

	bool semi_set = false;

	// EXECUTE IMMEDIATE in Oracle, Teradata
	Token *immediate = GetNextWordToken("IMMEDIATE", L"IMMEDIATE", 9);
	
	if(immediate != NULL)
	{
		STATS_SET_DESC(PLSQL_STMT_EXECUTE_IMMEDIATE_DESC)
		PL_STMS_STATS_V("EXECUTE IMMEDIATE", execute);

		// In Oracle statement can enclose with ()
		Token *open = GetNext('(', L'(');

		// Variable containing SQL statement
		Token *var = GetNextIdentToken();

		if(var == NULL)
			return false;

		Token *close = GetNext(open, ')', L')');

		// EXECUTE sp_executesql in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Token::ChangeNoFormat(immediate, "sp_executesql", L"sp_executesql", 13); 
			ConvertToTsqlVariable(var);
		}
		// EXECUTE in PostgreSQL
		if(_target == SQL_POSTGRESQL)
		{
			Token::Remove(immediate);
		}
		else
		if(_target == SQL_NETEZZA)
		{
			// Netezza does not allow ()
			Token::Remove(open);
			Token::Remove(close);
		}

		// INTO clause in Oracle
		Token *into = TOKEN_GETNEXTW("INTO");
		ListW into_cols;

		if(into != NULL)
		{
			while(true)
			{
				Token *var = GetNextIdentToken();

				if(var == NULL)
					break;

				// Add @ before name
				if(_target == SQL_SQL_SERVER)
				{
					ConvertToTsqlVariable(var);
					APPEND(var, " OUTPUT");
				}

				into_cols.Add(var);

				Token *comma = TOKEN_GETNEXT(',');

				if(comma == NULL)
					break;
			}
		}

		// In Teradata USING cannot be specified with EXECUTE IMMEDIATE, only with EXECUTE
		Token *using_ = TOKEN_GETNEXTW("USING");
		ListW using_cols;

		// Parameters are specified (declared variables or procedure parameters)
		if(using_ != NULL)
		{
			while(true)
			{
				Token *param = GetNextIdentToken();

				if(param == NULL)
					break;

				// Add @ before name
				if(_target == SQL_SQL_SERVER)
					ConvertToTsqlVariable(param);

				using_cols.Add(param);

				Token *comma = TOKEN_GETNEXT(',');

				if(comma == NULL)
					break;
			}
		}

		// For SQL Server add format string and remove INTO and USING
		if(_target == SQL_SQL_SERVER && (into != NULL || using_ != NULL))
		{
			// Parameter format string for SQL Server '@param datatype, ...'
			TokenStr format;
			format.Append(", N'", L", N'", 4);

			int num = 0;

			// Note that assignment in select list 'SELECT @var = ... ' is optional; it is enough just specify OUTPUT parameters
			for(ListwItem *i = into_cols.GetFirst(); i != NULL; i=i->next)
			{
				if(num > 0)
					APPENDSTR(format, ", ");

				format.Append((Token*)i->value);
				APPENDSTR(format, " output");
				num++;
			}

			for(ListwItem *i = using_cols.GetFirst(); i != NULL; i=i->next)
			{
				if(num > 0)
					APPENDSTR(format, ", ");

				format.Append((Token*)i->value);
				num++;
			}

			APPENDSTR(format, "',");

			AppendNoFormat(var, &format);
			Token::Remove(into);
			Token::Remove(using_);
		}
	}
	// EXECUTE in SQL Server, PostgreSQL, MySQL, Teradata, Sybase ASE
	else
	{
		PL_STMS_STATS(execute);

		// Optional ()
		Token *open = GetNextCharToken('(', L'(');
		Token *close = NULL;

		Token *exp = GetNextToken();
		Token *exp_end = NULL;

		Token *stmt_id = NULL;
		Token *stmt_id_var = NULL;

		// Statement ID pointing to a variable containing SQL can be specified in Teradata
		ListwmItem *i = Find(_spl_prepared_stmts, exp);

		bool exec_sp = false;

		// Prepared statement found
		if(i != NULL)
		{
			stmt_id = (Token*)i->value;
			stmt_id_var = (Token*)i->value2;
		}
		// Dynamic SQL expression or variable
		else
		{
			ParseExpression(exp);
			exp_end = GetLastToken();

			// Check for procedure call
			if(exp == exp_end && (exp->type == TOKEN_IDENT || exp->type == TOKEN_WORD))
			{
				if(Find(_spl_variables, exp) == NULL)
					exec_sp = true;
			}
		}

		close = GetNextCharToken(open, ')', L')');

		// EXECUTE IMMEDIATE in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Change(execute, "EXECUTE IMMEDIATE ", L"EXECUTE IMMEDIATE ", 18);

			// Replace statement ID with the variable containing SQL
			if(stmt_id_var != NULL)
				Token::Change(exp, stmt_id_var);

			Token::Remove(open);
			Token::Remove(close);
		}
		else
		// PREPARE, EXECUTE and DEALLOCATE PREPARE in MySQL
		if(_source != SQL_MYSQL && Target(SQL_MARIADB, SQL_MYSQL))
		{
			if(exec_sp == false)
			{
				// Session variable must be used in PREPARE in MySQL
				Token::Change(execute, "SET", L"SET", 3);
				AppendNoFormat(execute, " @stmt_str = ", L" @stmt_str = ", 13);
						
				// Delimiter can be omitted in SQL Server
				Token *append = GetNext(';', L';');

				if(append == NULL)
					append = AppendNoFormat(exp_end, ";", L";", 1);

				// PREPARE stmt FROM @stmt_str
				Append(append, "\nPREPARE", L"\nPREPARE", 8, execute);
				AppendNoFormat(append, " stmt ", L" stmt ", 6);
				Append(append, "FROM", L"FROM", 4, execute);	
				AppendNoFormat(append, " @stmt_str;", L" @stmt_str;", 11);

				Append(append, "\nEXECUTE", L"\nEXECUTE", 8, execute);
				AppendNoFormat(append, " stmt;", L" stmt;", 6);

				Append(append, "\nDEALLOCATE PREPARE", L"\nDEALLOCATE PREPARE", 19, execute);
				AppendNoFormat(append, " stmt;", L" stmt;", 6);

				semi_set = true;

				Token::Remove(open);
				Token::Remove(close);
			}
			// Stored procedure call
			else
			{
				if(Source(SQL_SQL_SERVER, SQL_SYBASE))
					ParseSqlServerExecProcedure(execute, exp);
				else
					Token::Change(execute, "CALL", L"CALL", 4);
			}
		}
	}

	// Add statement delimiter if not set when source is SQL Server
	if(semi_set == false)
		SqlServerAddStmtDelimiter();

	return true;
}

// EXIT statement in Oracle, Informix, Netezza
bool SqlParser::ParseExitStatement(Token *exit)
{
	if(exit == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_EXIT_DESC)
	PL_STMS_STATS(exit);

	// FOR, FOREACH, LOOP or WHILE keyword in Informix
	Token *loop_type = GetNextWordToken("FOR", L"FOR", 3);
	
	if(loop_type == NULL)
		loop_type = GetNextWordToken("FOREACH", L"FOREACH", 7);

	if(loop_type == NULL)
		loop_type = GetNextWordToken("LOOP", L"LOOP", 4);

	if(loop_type == NULL)
		loop_type = GetNextWordToken("WHILE", L"WHILE", 5);

	// Loop type not required for Oracle
	if(_target == SQL_ORACLE)
		Token::Remove(loop_type);

	// Optional WHEN condition in Oracle, Informix
	Token *when = GetNextWordToken("WHEN", L"WHEN", 4);

	if(_target_app == APP_JAVA)
		TOKEN_CHANGE_NOFMT(exit, "break");

	if(when == NULL)
		return true;

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	Token *cent = GetNextCharToken('%', L'%');
    Token *notfound = NULL;

	// cursor%NOTFOUND
    if(cent != NULL)
	    notfound = GetNextWordToken("NOTFOUND", L"NOTFOUND", 8);

	if(notfound != NULL)
	{
		// IF @@FETCH_STATUS <> 0 BREAK in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(exit, "IF", L"IF", 2);
			Token::Change(when, "@@FETCH_STATUS <> 0 BREAK", L"@@FETCH_STATUS <> 0 BREAK", 25);

			Token::Remove(next, notfound); 
		}
        else
        // Check generated NOT_FOUND variable 
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Change(exit, "IF NOT_FOUND = 1", L"IF NOT_FOUND = 1", 16);
            Token::Change(when, "THEN", L"THEN", 4);

            // Generate the label name
            TokenStr label("loop_label", L"loop_label", 10);

            if(_spl_loop_labels > 0)
                label.Append(_spl_loop_labels + 1);            
                  
            Token::Change(notfound, "LEAVE", L"LEAVE", 5);
            AppendNoFormat(notfound, " ", L" ", 1);
            AppendNoFormat(notfound, &label);
            AppendNoFormat(notfound, ";", L";", 1);

            ListwItem *i = _spl_loops.GetLast();

            if(i != NULL)
            {
                Token *loop = (Token*)i->value;
                PrependNoFormat(loop, &label);
                Prepend(loop, ":\n", L":\n", 2);
            }

            _spl_loop_labels++;

            /*Token *semi */ (void) GetNext(';', L';');
            Append(notfound, " END IF;", L" END IF;", 7);

            Token::Remove(next, cent); 

            // Add NOT FOUND handler and variable
			_spl_need_not_found_handler = true;
        }
		else
		// WHEN NOT FOUND in PostgreSQL, Netezza
		if(Target(SQL_POSTGRESQL, SQL_NETEZZA))
		{
			Token::Change(notfound, "NOT FOUND", L"NOT FOUND", 9);
			Token::Remove(next, cent); 
		}
	}
    // Boolean condition
    else
    {
        ParseBooleanExpression(SQL_BOOL_IF, exit);
        Token *last = GetLastToken();

        // IF-LEAVE in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Change(exit, "IF", L"IF", 2);
            Token::Remove(when);

            Append(last, " THEN LEAVE", L" THEN LEAVE", 11, exit);
            AppendNoFormat(last, " label;", L" label;", 7);

            Append(last, " END IF;", L" END IF;", 7);
        }
    }

	return true;
}

// DB2 CLP EXPORT command
bool SqlParser::ParseExportStatement(Token *export_)
{
	if(export_ == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("EXPORT", export_);

	// TO file
	Token *to = GetNextWordToken("TO", L"TO", 2);

	if(to == NULL)
		return false;

	// File name with double quotes
	/*Token *filename */ (void) GetNextToken();

	// OF filetype (DEL, IXF)
	Token *of = GetNextWordToken("OF", L"OF", 2);

	Token *del = GetNextWordToken(of, "DEL", L"DEL", 3);
	Token *ixf = NULL;

	if(del == NULL)
		ixf = GetNextWordToken(of, "IXF", L"IFX", 3);

	if(del == NULL && ixf == NULL)
		return false;

	Token *colsep = NULL;

	// Export options
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// MODIFIED BY
		if(next->Compare("MODIFIED", L"MODIFIED", 8) == true)
		{
			Token *by = GetNextWordToken("BY", L"BY", 2);

			if(by != NULL)
				Db2ParseModifiedByOptions(&colsep);

			if(_target == SQL_ORACLE)
				Token::Remove(next, by);
		}
		else
		// MESSAGES file
		if(next->Compare("MESSAGES", L"MESSAGES", 8) == true)
		{
			Token *file = GetNextToken();

			if(_target == SQL_ORACLE)
				Comment(next, file);
		}
		// Unknown option
		else
		{
			PushBack(next);
			break;
		}
	}

	// SELECT statement 
	Token *select = GetNextSelectStartKeyword();
	
	if(select != NULL)
		ParseSelectStatement(select, 0, SQL_SEL_EXPORT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	if(_target == SQL_ORACLE)
	{
		// Use SPOOL "file" command (quotes are optional)
		Token::Change(export_, "SPOOL", L"SPOOL", 5);
		Token::Remove(to);

		Token::Remove(of, Nvl(del, ixf));

		// Add settings to format output, PAGESIZE tells to have one single page without repeating header
		Prepend(export_, "SET PAGESIZE 0\n", L"SET PAGESIZE 0\n", 15);
		Prepend(export_, "SET LINESIZE 1000\n", L"SET LINESIZE 1000\n", 18);

		// Set column separator in SQL*Plus output
		if(colsep != NULL)
		{
			Prepend(export_, "SET COLSEP ", L"SET COLSEP ", 11);
			PrependCopy(export_, colsep);
			Prepend(export_, "\n", L"\n", 1);
		}

		Token *semi = GetNextCharToken(';', L';');

		Token *last = Nvl(semi, GetLastToken());

		// Terminate spooling
		Append(last, "\nSPOOL OFF", L"\nSPOOL OFF", 10, export_);

		// Reset column separator to the default blank
		if(colsep != NULL)
			Append(last, "\nSET COLSEP \" \"", L"\nSET COLSEP \" \"", 15, export_);
	}
	
	return true;
}

// CREATE TYPE in Oracle, SQL Server
bool SqlParser::ParseCreateType(Token *create, Token *type)
{
	if(create == NULL || type == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("CREATE TYPE", create);

	// User-defined type name
	Token *name = GetNextToken();

	if(name == NULL)
		return false;

	// FROM keyword in SQL Server
	Token *from = GetNextWordToken("FROM", L"FROM", 4);

	// System data type
	Token *datatype = GetNextToken();

	ParseDataType(datatype);

	Token *type_end = GetLastToken();

	// Optional constraint
	ParseColumnConstraints(create, name, name, datatype, type_end, NULL, NULL, NULL, NULL);

	Token *last = GetLastToken();

	_udt.Add(name, datatype, type_end);

	// CREATE TYPE name AS OBJECT (name type) in Oracle
	if(_source != SQL_ORACLE && _target == SQL_ORACLE)
	{
		Append(create, " OR REPLACE", L" OR REPLACE", 11);

		if(_source == SQL_SQL_SERVER)
		{
			Token::Change(from, "AS OBJECT (", L"AS OBJECT (", 11);
			AppendCopy(from, name);
			AppendNoFormat(last, ")", L")", 1);
		}

		Token *semi = GetNextCharToken(';', L';');

		// In Oracle CREATE TYPE requires / for execution
		Append(Nvl(semi, last), "\n/", L"\n/", 2);
	}

	return true;
}

// Sybase ASE CREATE RULE
bool SqlParser::ParseCreateRule(Token *create, Token *rule)
{
	if(create == NULL || rule == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("CREATE RULE", create);

	// Rule name
	Token *name = GetNextToken();

	if(name == NULL)
		return false;

	// AS keyword
	Token *as = GetNextWordToken("AS", L"AS", 2);

	// Rule condition
	ParseBooleanExpression(SQL_BOOL_CREATE_RULE);

	Token *start = (as != NULL) ? as->next : NULL;
	Token *last = GetLastToken();

	_domain_rules.Add(name, start, last);

	if(_target == SQL_POSTGRESQL)
		Comment("Converted to a CHECK constraint\n", L"Converted to a CHECK constraint\n", 32, create, last);

	return true;
}

// CREATE SCHEMA in SQL Server, MySQL, Redshift
bool SqlParser::ParseCreateSchema(Token *create, Token *schema)
{
	if(create == NULL || schema == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_STMT_CREATE_SCHEMA_DESC)

	// IF NOT EXISTS in MySQL
	Token *if_ = GetNext("IF", L"IF", 2);

	Token *not_ = GetNext(if_, "NOT", L"NOT", 3);
	Token *exists = GetNext(not_, "EXISTS", L"EXISTS", 6);

	// Database name in MySQL
	Token *name = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(name == NULL)
	{
	    STMS_STATS_V("CREATE SCHEMA", create);
		return false;
	}

	// Options
	while(true)
	{
		// In MySQL options can start with optional DEFAULT keyword
		Token *default_ = GetNext("DEFAULT", L"DEFAULT", 7);

		Token *next = GetNext();

		if(next == NULL)
			break;

		// [DEFAULT] CHARACTER SET [=] name in MySQL
		if(next->Compare("CHARACTER", L"CHARACTER", 9) == true)
		{
			Token *set = GetNext("SET", L"SET", 3);

			// Optional = 
			/*Token *equal */ (void) GetNext(set, '=', L'=');

			// Default character set name
			Token *name = GetNext(set);

			if(_target == SQL_ORACLE && name != NULL)
				Comment(Nvl(default_, next), name);
		}
		else
		// [DEFAULT] COLLATE [=] name in MySQL
		if(next->Compare("COLLATE", L"COLLATE", 7) == true)
		{
			// Optional = 
			/*Token *equal */ (void) GetNext('=', L'=');

			// Default collate name
			Token *name = GetNext();

			if(_target == SQL_ORACLE && name != NULL)
				Comment(Nvl(default_, next), name);
		}
		else
		// AUTHORIZATION name in DB2
		if(TOKEN_CMP(next, "AUTHORIZATION"))
		{
			// Authorization name
			/*Token *name */ (void) GetNext();
		}
		else
		{
			PushBack(next);
			break;
		}
	}

	// CREATE USER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(schema, "USER", L"USER", 4);

		// Add default password
		Append(name, " IDENTIFIED BY ", L" IDENTIFIED BY ", 15, create);
		AppendCopy(name, name);

		Token::Remove(if_, exists);
	}

	// Add statement delimiter if not set and handle GO when source is SQL Server, Sybase ASE
	SqlServerDelimiter();

    STMS_STATS_V("CREATE SCHEMA", create);

	return true;
}

// Oracle CREATE SEQUENCE
bool SqlParser::ParseCreateSequence(Token *create, Token *sequence)
{
	if(create == NULL || sequence == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_STMT_CREATE_SEQUENCE_DESC)

	Token *seq_name = GetNextIdentToken();

	if(seq_name == NULL)
		return false;

	Token *start_with = NULL;
	Token *increment_by = NULL;

	ParseSequenceOptions(&start_with, &increment_by, ssi);	
	
	// A stored procedure call in MySQL
	if(Target(SQL_MYSQL))
	{
		Token::Change(create, "CALL", L"CALL", 4);
		Token::Remove(sequence);
		PrependNoFormat(seq_name, "CreateSequence('", L"CreateSequence('", 16);

		Append(seq_name, "', ", L"', ", 3);

		// Specify START WITH value
		if(start_with != NULL)
			AppendCopy(seq_name, start_with);
		else
			Append(seq_name, "1", L"1", 1);

		Append(seq_name, ", ", L", ", 2);

		// Specify INCREMENT BY value
		if(increment_by != NULL)
			AppendCopy(seq_name, increment_by);
		else
			Append(seq_name, "1", L"1", 1);

		Append(seq_name, ")", L")", 1);
	}

    STMS_STATS_V("CREATE SEQUENCE", create);
	SEQ_STATS_V("CREATE SEQUENCE", create);

	return true;
}

// ALTER SEQUENCE statement
bool SqlParser::ParseAlterSequenceStatement(Token *alter, Token *sequence)
{
	if(alter == NULL || sequence == NULL)
		return false;

	STATS_DECL
	STMS_STATS_V("ALTER SEQUENCE", alter);

	Token *seq_name = GetNextIdentToken();

	if(seq_name == NULL)
		return false;

	ParseSequenceOptions(NULL, NULL, ssi);

	SEQ_STATS_V("ALTER SEQUENCE", alter);
	return true;
}

// DB2 for z/OS CREATE STOGROUP
bool SqlParser::ParseCreateStogroup(Token *create, Token *stogroup)
{
	if(create == NULL || stogroup == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("CREATE STOGROUP", create);

	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	bool exists = false;
	
	// Options
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// VOLUMES (v1, ...)
		if(next->Compare("VOLUMES", L"VOLUMES", 7) == true)
		{
			Token *open = GetNextCharToken('(', L'(');

			// Comma separated list
			while(true)
			{
				if(open == NULL)
					break;

				// Identifier or '*' 
				Token *name = GetNextToken();

				if(name == NULL)
					break;

				Token *comma = GetNextCharToken(',', L',');

				if(comma == NULL)
					break;
			}

			/*Token *close */ (void) GetNextCharToken(')', L')');

			exists = true;
			continue;
		}
		else
		// VCAT name
		if(next->Compare("VCAT", L"VCAT", 4) == true)
		{
			/*Token *name */ (void) GetNextToken();

			exists = true;
			continue;
		}
			
		PushBack(next);
		break;
	}

	// Comment the entire statement for other databases
	if(_target != SQL_DB2)
	{
		Token *end = GetLastToken(GetNextCharToken(';', L';'));
		Comment(create, end);
	}

	return true;
}

// CREATE VIEW statement
bool SqlParser::ParseCreateView(Token *create, Token *materialized, Token *view)
{
    STATS_DECL
	STATS_DTL_DECL

	if(create == NULL || view == NULL)
		return false;

	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	Token *open = GetNext('(', L'(');

	// Optional column list 
	while(true)
	{
		if(open == NULL)
			break;

		// Column name 
		Token *column = GetNextIdentToken();

		if(column == NULL)
			break;

		Token *comma = GetNext(',', L',');

		if(comma == NULL)
			break;
	}

	/*Token *close */ (void) GetNext(open, ')', L')');

	// AS keyword
	/*Token *as */ (void) GetNext("AS", L"AS", 2);

	Token *select = GetNextSelectStartKeyword();

	if(select != NULL)
		ParseSelectStatement(select, 0, SQL_SEL_VIEW, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	// View options
	while(true)
	{
		bool exists = false;

		Token *option = GetNextToken();

		if(option == NULL)
			break;

		// WITH READ ONLY
		if(option->Compare("WITH", L"WITH", 4) == true)
		{
			Token *read = GetNext("READ", L"READ", 4);
			Token *only = GetNext(read, "ONLY", L"ONLY", 4);

			if(read != NULL && only != NULL)
			{
				// Remove for MySQL
				if(Target(SQL_MARIADB, SQL_MYSQL))
					Token::Remove(option, only);

				exists = true;
				continue;
			}
			else
				PushBack(option);
		}

		if(exists == false)
			break;
	}

	// Regular view
	if(materialized == NULL)
	{
		STATS_SET_DESC(SQL_STMT_CREATE_VIEW_DESC)
		STMS_STATS_V("CREATE VIEW", create);
	}
	// Materialized view
	else
	{
		STATS_SET_DESC(SQL_STMT_CREATE_MATERIALIZED_VIEW_DESC)
		STATS_DTL_DESC(SQL_STMT_CREATE_MATERIALIZED_VIEW_DESC) 
		STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_HIGH, "", "")

		STATS_UPDATE_STATUS
		STMS_STATS_V("CREATE MATERIALIZED VIEW", create);
	}

	return true;
}

// DB2 DECLARE name CONDITION FOR SQLSTATE 'value'; Sybase ASA DECLARE name EXCEPTION FOR SQLSTATE VALUE 'value'
bool SqlParser::ParseDeclareCondition(Token *declare, Token *name, Token *condition)
{
	if(declare == NULL || name == NULL || condition == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS_V("DECLARE CONDITION", declare);

	// FOR SQLSTATE
	Token *for_ = GetNextWordToken("FOR", L"FOR", 3);
	Token *sqlstate = NULL;

	if(for_ != NULL)
		sqlstate = GetNextWordToken("SQLSTATE", L"SQLSTATE", 8);

	if(sqlstate == NULL)
		return false;

	// VALUE keyword in Sybase ASA
	/*Token *value */ (void) GetNextWordToken("VALUE", L"VALUE", 5);
		
	// String literal in DB2, Sybase ASA i.e. '23505'
	Token *error = GetNextToken();

	// Save user-defined exception
	_spl_user_exceptions.Add(name, error);

	// Remove statement for SQL Server, Oracle
	if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true)
	{
		Token::Remove(declare, error);
		Token::Remove(GetNextCharToken(';', L';'));
	}

	return true;
}

// DB2, MySQL, Informix, Teradata DECLARE CURSOR
bool SqlParser::ParseDeclareCursor(Token *declare, Token *name, Token *cursor)
{
	if(declare == NULL || name == NULL || cursor == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS_V("DECLARE CURSOR", declare);

	_spl_declared_cursors.Add(name);
	_spl_current_declaring_cursor = name;
	_spl_current_declaring_cursor_uses_vars = false;

	// Optional WITH HOLD in DB2
	Token *with = GetNextWordToken("WITH", L"WITH", 4);
	Token *hold = NULL;

	if(with != NULL)
	{
		hold = GetNextWordToken("HOLD", L"HOLD", 4);

		if(hold != NULL)
		{
			// Remove for Oracle
			if(_target == SQL_ORACLE)
				Token::Remove(with, hold);
		}
		else
		{
			PushBack(with);
			with = NULL;
		}
	}

	// Optional WITH RETURN in DB2, Teradata
	Token *with2 = GetNextWordToken("WITH", L"WITH", 4);
	Token *return_ = NULL;

	if(with2 != NULL)
	{
		return_ = GetNextWordToken("RETURN", L"RETURN", 6);
		
		Token *to = NULL;
		Token *rctype = NULL;

		// Optional ONLY in Teradata
		Token *only = GetNextWordToken("ONLY", L"ONLY", 4);

		// Optional TO CALLER or TO CLIENT
		if(return_ != NULL)
		{
			to = GetNextWordToken("TO", L"TO", 2);
						
			if(to != NULL)
				rctype = GetNextToken();

			_spl_result_set_cursors.Add(name);
			_spl_result_sets++;
		}

		// Remove return options
		if(!Target(SQL_DB2, SQL_TERADATA))
		{
			Token::Remove(with2, return_);
			Token::Remove(only);
			Token::Remove(to, rctype);
		}
	}
		
	// FOR keyword
	Token *for_ = GetNextWordToken("FOR", L"FOR", 3);

	// AS keyword in Sybase ADS
	Token *as = (for_ == NULL)? TOKEN_GETNEXTW("AS") : NULL;

	// SELECT statement, variable, statement ID
	Token *select = GetNextSelectStartKeyword();
	
	Token *stmt_id = NULL;

	if(select == NULL)
	{
		// Statement ID in Informix, Teradata
		Token *next = GetNextToken();

		// In Informix, PREPARE stmt_id FROM select_string goes before DECLARE cur FOR stmt_id
		// In Teradata, cursor declared first, then PREPARE executed

		// Prepared statement ID can be specified in Informix, find it
		ListwmItem *i = Find(_spl_prepared_stmts, next);

		if(i != NULL)
			stmt_id = (Token*)i->value;			
		else
		// No PREPARE was executed in Teradata yet 
		if(next != NULL)
			stmt_id = next;
		
		_spl_prepared_stmts_cursors.Add(name, stmt_id);

		if(return_ != NULL)
			_spl_prepared_stmts_cursors_with_return.Add(name, stmt_id);
	}
	else
	{
		ListW exp_starts;
		ListW out_cols;

		ParseSelectStatement(select, 0, SQL_SEL_CURSOR, NULL, NULL, &exp_starts, &out_cols, NULL, NULL, NULL, NULL);

		ListwItem *first_exp = exp_starts.GetFirst();

		// Save the first select expression of the declared cursor
		if(first_exp != NULL)
			_spl_declared_cursors_select_first_exp.Add(name, first_exp->value);

		ListwItem *exp = out_cols.GetFirst();

		// Save the columns of the declared cursor
		while(exp != NULL)
		{
			_spl_declared_cursors_select_exp.Add(name, exp->value);
			exp = exp->next;
		}
	}

	Token *cut_start = NULL;

	// SELECT statement is specified
	if(select != NULL)
	{
		// Cursor with no return to client/caller
		if(return_ == NULL)
		{
			// CURSOR cur IS SELECT 
			if(_target == SQL_ORACLE)
			{
				Token::Change(declare, "CURSOR", L"CURSOR", 6);
				Token::Change(for_, "IS", L"IS", 2);
				Token::Remove(cursor);
			}
			else
			if(_target == SQL_SQL_SERVER)
				TOKEN_CHANGE(as, "FOR");
		}
		else
		{
			// Cut FOR SELECT to be used in OPEN
			if(Target(SQL_ORACLE, SQL_SQL_SERVER) && _source_app != APP_COBOL)
			{
				cut_start = for_;

				// in SQL Server, Standalone SELECT is used in place of OPEN cursor, so FOR keyword is not required
				if(_target == SQL_SQL_SERVER)
				{
					cut_start = select;
					Token::Remove(for_);
				}

				// Block will be transferred to OPEN cur
				Cut(COPY_SCOPE_PROC, COPY_CURSOR_WITH_RETURN, name, for_, GetLastToken());
			
				Token::Remove(declare, cursor);

				// Remove ; following cut DECLARE
				Token::Remove(GetNextCharToken(';', L';'));
			}
		}
	}
	else
	// Prepared statement ID is specified
	if(stmt_id != NULL)
	{
		// In Oracle declare REF CURSOR that will be used in OPEN cur FOR sql_string
		if(_target == SQL_ORACLE)
		{
			// Cursor is without return
			if(return_ == NULL)
			{
				// cur SYS_REFCURSOR; 
				Token::Change(cursor, "SYS_REFCURSOR", L"SYS_REFCURSOR", 13);
				
				Token::Remove(declare);
				Token::Remove(for_, stmt_id);
			}
			// OUT SYS_REFCURSOR will be added as an OUT parameter 
			else
				Token::Remove(declare, GetNextCharOrLastToken(';', L';'));
		}
	}

	if(_spl_current_declaring_cursor_uses_vars)
		_spl_declared_cursors_using_vars.Add(name);

	if(cut_start == NULL && !Source(SQL_SQL_SERVER) && Target(SQL_SQL_SERVER))
	{
		// In SQL Server cursor are global by default in procedures (but not in functions) i.e if you call procedure multiple time
		// you can get "already exists error" if the cursor was not deallocated. We will just add LOCAL option to prevent this
		if(_spl_scope == SQL_SCOPE_PROC || _spl_func_to_proc)
			APPEND(cursor, " LOCAL");

		// SQL Server takes variable values at declare time, not open cursor time, so if there are any dependencies on variable we have to
		// move the cursor declaration right before OPEN cursor statements
		if(_spl_current_declaring_cursor_uses_vars)
			cut_start = declare;
	}

	Token *cursor_end = GetLastToken(TOKEN_GETNEXT(';'));

	_spl_declared_cursors_stmts.Add(name, declare, cursor_end);

	// If there are no non-declare statements above, set last declare
	if(_spl_first_non_declare == NULL && cut_start == NULL)
		_spl_last_declare = cursor_end;

	bool moved = false;

	// MySQL, MariaDB require cursor declaration go before any other statements
	if(Target(SQL_MYSQL, SQL_MARIADB))
		moved = MySQLMoveCursorDeclarations(declare, cursor_end);
	
	// Add statement delimiter if not set when source is SQL Server
	if(!moved)
		SqlServerAddStmtDelimiter();

	_spl_current_declaring_cursor = NULL;

	return true;
}

// DB2, MySQL, Teradata DECLARE type HANDLER
bool SqlParser::ParseDeclareHandler(Token *declare, Token *type)
{
	if(declare == NULL || type == NULL)
		return false;

	// Possible conflict with variable name
	if(Source(SQL_DB2, SQL_MYSQL, SQL_TERADATA) == false)
		return false;

    STATS_DECL
	PL_STMS_STATS_V("DECLARE HANDLER", declare);

	// CONTINUE, EXIT, UNDO possible handler types
	bool continue_ = false;
	bool exit = false;
	bool undo = false;

	if(Token::Compare(type, "CONTINUE", L"CONTINUE", 8) == true)
		continue_ = true;
	else
	if(Token::Compare(type, "EXIT", L"EXIT", 4) == true)
		exit = true;
	else
	if(Token::Compare(type, "UNDO", L"UNDO", 4) == true)
		undo = true;

	// Not a handler
	if(continue_ == false && exit == false && undo == false)
		return false;

	// HANDLER keyword
	Token *handler = GetNextWordToken("HANDLER", L"HANDLER", 7);

	// FOR keyword
	Token *for_ = GetNextWordToken("FOR", L"FOR", 3);

	Token *not_ = NULL;
	Token *found = NULL;
	Token *sqlexception = NULL;
	Token *sqlwarning = NULL;
	Token *user = NULL;

	// FOR SQLSTATE [VALUE] 'value' can be specified in Teradata
	Token *sqlstate = NULL;
	Token *value = NULL;
	Token *sqlstate_value = NULL;

	// Multiple comma-separated conditions can be specified NOT FOUND, SQLEXCEPTION and SQLWARNING
	while(true)
	{
		Token *condition = GetNextToken();

		if(condition == NULL)
			break;

		// NOT FOUND
		if(Token::Compare(condition, "NOT", L"NOT", 3) == true)
		{
			not_ = condition;
			found = GetNextWordToken("FOUND", L"FOUND", 5);
		}
		else
		// SQLEXCEPTION
		if(Token::Compare(condition, "SQLEXCEPTION", L"SQLEXCEPTION", 12) == true)
			sqlexception = condition;
		else
		// SQLWARNING
		if(Token::Compare(condition, "SQLWARNING", L"SQLWARNING", 10) == true)
			sqlwarning = condition;
		else
		// SQLSTATE in Teradata
		if(Token::Compare(condition, "SQLSTATE", L"SQLSTATE", 8) == true)
		{
			sqlstate = condition;

			// VALUE is optional in Teradata
			value = GetNextWordToken("VALUE", L"VALUE", 5);

			sqlstate_value = GetNextToken();
		}
		else
		// User-defined exception
		{
			user = condition;
			break;
		}

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		if(_target == SQL_ORACLE)
			Token::Remove(comma);
	}

	// Optional BEGIN END
	Token *begin = GetNextWordToken("BEGIN", L"BEGIN", 5);
	Token *end = NULL;

	bool frontier = (begin != NULL) ? true : false;

	ParseBlock(SQL_BLOCK_HANDLER, frontier, 0, NULL);

	if(begin != NULL)
		end = GetNextWordToken("END", L"END", 3);

	// Convert to EXCEPTION clause in Oracle
	if(_target == SQL_ORACLE)
	{
		int cut_type = 0;
		Token *name = NULL;

		if(exit == true)
			Token::Change(declare, "WHEN", L"WHEN", 4);
		else
			Token::Remove(declare);

		// If SQLEXCEPTION is set it takes priority on other conditions
		if(sqlexception != NULL)
		{
			// EXIT handler always transferred to OTHERS exception
			if(exit == true)
			{
				Token::Change(type, "OTHERS", L"OTHERS", 6);
				Token::Change(handler, "THEN", L"THEN", 4);

				cut_type = COPY_EXIT_HANDLER_SQLEXCEPTION;
			}
			// CONTINUE handler implementation depends on the statement
			else
			{
				Token::Remove(type);
				Token::Remove(handler);

				cut_type = COPY_CONTINUE_HANDLER_SQLEXCEPTION;
			}
		}
		else
		if(not_ != NULL)
		{
			// EXIT handler always transferred to NO_DATA_FOUND exception
			if(exit == true)
			{
				Token::Change(type, "NO_DATA_FOUND", L"NO_DATA_FOUND", 13);
				Token::Change(handler, "THEN", L"THEN", 4);

				cut_type = COPY_EXIT_HANDLER_NOT_FOUND;
			}
			// CONTINUE handler implementation depends on the statement
			else
			{
				Token::Remove(type);
				Token::Remove(handler);

				cut_type = COPY_CONTINUE_HANDLER_NOT_FOUND;
			}
		}
		else
		// For SQLSTATE
		if(sqlstate != NULL)
		{
			if(exit == true)
				cut_type = COPY_EXIT_HANDLER_FOR_SQLSTATE;
			else
				cut_type = COPY_CONTINUE_HANDLER_FOR_SQLSTATE;

			if(_source == SQL_TERADATA)
			{
				// Duplicate key in Teradata
				if(Token::Compare(sqlstate_value, "'23505'", L"'23505'", 7) == true)
				{
					Token::Change(sqlstate, "DUP_VAL_ON_INDEX THEN ", L"DUP_VAL_ON_INDEX THEN ", 22);

					Token::Remove(type);
					Token::Remove(handler);

					Token::Remove(value);
					Token::Remove(sqlstate_value);
				}
				// Unknown SQLSTATE value
				else
				{
					Token::Change(type, "OTHERS THEN ", L"OTHERS THEN ", 12);

					Token::Remove(handler);

					Comment(sqlstate, sqlstate_value);
					Token::Remove(value);					
				}
			}
		}
		else
		// User-defined handler
		if(user != NULL)
		{
			name = user;

			Token::Remove(type);
			Token::Remove(handler);

			if(exit == true)
				cut_type = COPY_EXIT_HANDLER_USER_DEFINED;
			else
				cut_type = COPY_CONTINUE_HANDLER_USER_DEFINED;
		}

		Token::Remove(for_);
		Token::Remove(not_, found);
		Token::Remove(sqlexception);
		Token::Remove(sqlwarning);
		Token::Remove(user);
		
		// END can be terminated with ;
		Token *semi = (end != NULL) ? GetNextCharToken(';', L';') : NULL;

		// Remove outer BEGIN END; as each block starts with THEN
		Token::Remove(begin);
		Token::Remove(end, semi);

		Token *last = GetLastToken();

		// Block will be transferred to EXCEPTION section or IF cur%NOTFOUND, SQL%ROWCOUNT condition
		Cut(COPY_SCOPE_PROC, cut_type, name, declare, last);
	}

	// If there are no non-declare statements above, set last declare
	if(_spl_first_non_declare == NULL)
		_spl_last_declare = GetLastToken();

	return true;
}

// SQL Server DECLARE TABLE variable
bool SqlParser::ParseDeclareTable(Token *declare, Token *name, Token *table)
{
    STATS_DECL
	PL_STMS_STATS_V("DECLARE TABLE", declare);

	// Next token must be (
	/*Token *open */ (void) GetNextCharToken('(', L'(');

	ListW pkcols;

	// Identity start and increment
	Token *id_col = NULL;
	Token *id_start = NULL;
	Token *id_inc = NULL;
	bool id_default = true;

	ParseCreateTableColumns(declare, name, pkcols, &id_col, &id_start, &id_inc, &id_default, NULL, NULL);

	// Next token must be )
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// In MySQL, use temporary table
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Prepend(declare, "DROP TEMPORARY TABLE IF EXISTS ", L"DROP TEMPORARY TABLE IF EXISTS ", 31);  
		PrependCopy(declare, name); 
		Prepend(declare, ";\n", L";\n", 2);

		Token::Change(declare, "CREATE TEMPORARY TABLE", L"CREATE TEMPORARY TABLE", 22);
		Token::Remove(table);
	}

	return true;
}

// DECLARE GLOBAL TEMPORARY TABLE in DB2
bool SqlParser::ParseDeclareGlobalTemporaryTable(Token *declare, Token *global)
{
	if(declare == NULL || global == NULL)
		return false;

	if(global->Compare("GLOBAL", L"GLOBAL", 6) == false)
		return false;

	Token *temporary = GetNextWordToken("TEMPORARY", L"TEMPORARY", 9);

	if(temporary == NULL)
		return false;

	Token *table = GetNextWordToken("TABLE", L"TABLE", 5);

	if(table == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS_V("DECLARE GLOBAL TEMPORARY TABLE", declare);

	// Table name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	if(_source == SQL_DB2 && _target != SQL_DB2)
	{
		// In DB2 temporary table name can include SESSION. qualifier in declaration
		if(name->Compare("SESSION.", L"SESSION.", 0, 8) == true)
		{
			TokenStr name2;
			name2.Append(name, 8, name->len - 8);

			Token::ChangeNoFormat(name, name2);
		}
		else
		// Add # before table name in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			TokenStr name2;

			name2.Append("#", L"#", 1);
			name2.Append(name);

			Token::ChangeNoFormat(name, name2);
		}
	}

	_spl_created_session_tables.Add(name);

	// In DB2 if AS keyword is specified next, then (SELECT ...) must follow
	Token *as = GetNextWordToken("AS", L"AS", 2);

	// Next token must be (
	/*Token *open */ (void) GetNextCharToken('(', L'(');

	// Column definitions or SELECT statement can be specified
	Token *select = GetNextSelectStartKeyword();
	
	Token *from_end = NULL;
	Token *where_end = NULL;

	// Parse colum definitions
	if(as == NULL)
	{
		ListW pkcols;

		ParseCreateTableColumns(declare, name, pkcols, NULL, NULL, NULL, NULL, NULL, NULL);
	}
	// Subquery
	else
		ParseSelectStatement(select, 0, SQL_SEL_CREATE_TEMP_TABLE_AS, NULL, NULL, NULL, NULL, NULL, NULL, &from_end, &where_end);

	// Next token must be )
	/*Token *close */ (void) GetNextCharToken(')', L')');

	bool no_data;

	Token *options_start = NULL;
	Token *options_end = NULL; 

	// Optional ON COMMIT DELETE | PRESERVE ROWS etc.
	ParseTempTableOptions(name, &options_start, &options_end, &no_data);

	// CREATE GLOBAL TEMPORARY TABLE in Oracle moved outside the procedure
	if(_target == SQL_ORACLE)
	{
		Token::Change(declare, "CREATE", L"CREATE", 6);		

		// Move CREATE TEMPORARY TABLE definition before CREATE PROCEDURE, CREATE FUNCTION etc.
		if(_spl_scope != 0)
		{
			Token *last = Nvl(GetNextCharToken(';', L';'), GetLastToken());

			PrependCopy(_spl_start, declare, last, false);
			Prepend(_spl_start, "\n\n", L"\n\n", 2);

			Token::Remove(declare, last);
		}

		// Definition only, add WHERE 1=0
		if(no_data == true)
		{
			// No WHERE clause
			if(where_end == NULL)
				Append(from_end, " WHERE 1=0", L" WHERE 1=0", 10, declare);
			else
				Append(where_end, " AND 1=0", L" AND 1=0", 8, declare);
		}

		// If SELECT is specified options must be before SELECT in Oracle, not after as in DB2
		if(select != NULL && options_start != NULL)
		{
			PrependSpaceCopy(as, options_start, options_end, false);
			Token::Remove(options_start, options_end);
		}
	}
	else
	// CREATE TABLE #name in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(declare, "CREATE", L"CREATE", 6);		

		Token::Remove(global, temporary);
	}

	return true;
}

// DECLARE LOCAL TEMPORARY TABLE in Sybase ASA
bool SqlParser::ParseDeclareLocalTemporaryTable(Token *declare, Token *local)
{
	if(declare == NULL || local == NULL)
		return false;

	if(local->Compare("LOCAL", L"LOCAL", 5) == false)
		return false;

	Token *temporary = GetNextWordToken("TEMPORARY", L"TEMPORARY", 9);

	if(temporary == NULL)
		return false;

	Token *table = GetNextWordToken("TABLE", L"TABLE", 5);

	if(table == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS_V("DECLARE LOCAL TEMPORARY TABLE", declare);

	// Table name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	// Add @ for parameter names for SQL Server and Sybase
	if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
		ConvertToTsqlVariable(name);

	_spl_declared_local_tables.Add(name);

	// Next token must be (
	/*Token *open */ (void) GetNextCharToken('(', L'(');

	ListW pkcols;

	ParseCreateTableColumns(declare, name, pkcols, NULL, NULL, NULL, NULL, NULL, NULL);

	// Next token must be )
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Optional ON COMMIT DELETE | PRESERVE ROWS
	ParseTempTableOptions(name, NULL, NULL, NULL);

	// DECLARE @name TABLE (columns) in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Remove(local, temporary);

		AppendNoFormat(name, " ", L" ", 1);
		AppendCopy(name, table);

		Token::Remove(table);
	}

	return true;
}

// DECLARE for variable in SQL Server, DB2, MySQL; DEFINE in Informix
bool SqlParser::ParseDeclareVariable(Token *declare, Token *name, Token *type, int cnt_list)
{
	if(name == NULL || type == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS_V("DECLARE", declare);

	// In PostgreSQL DECLARE is specified once (start DECLARE section that goes until BEGIN)
	// Make sure it is not the same DECLARE keyword (when variables are in a list)
	if(_spl_last_declare != NULL && cnt_list == 0 && _target == SQL_POSTGRESQL)
		Token::Remove(declare);
	else
	// Convert DEFINE to DECLARE from Informix
	if(_source == SQL_INFORMIX && _target != SQL_INFORMIX)
		Token::Change(declare, "DECLARE", L"DECLARE", 7);

	ConvertIdentifier(name, SQL_IDENT_VAR);

	// Add @ for parameter names for SQL Server and Sybase
	if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
		ConvertToTsqlVariable(name);

	_spl_variables.Add(name);

	ListW vars;
	vars.Add(name);

	int num = 1;

	// DECLARE v1, v2, v3 INT DEFAULT 0 in DB2, MySQL, MariaDB, Informix
	while(true)
	{
		// Check for list of variables
		if(Token::Compare(type, ',', L',') == false)
			break;

		// Separate statements in Oracle, PostgreSQL
		if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
			Token::Change(type, ";", L";", 1);

		// Next variable in list
		Token *var = GetNextToken();

		if(var == NULL)
			break;

		// Add @ for parameter names for SQL Server and Sybase
		if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
			ConvertToTsqlVariable(var);

		_spl_variables.Add(var);

		vars.Add(var);
		num++;

		type = GetNextToken();
	}

	// Check and resolve Oracle %TYPE, Informix LIKE variable
	bool typed = ParseTypedVariable(name, type);

	// Get the variable data type
	if(typed == false)
	{
		ParseDataType(type);
		ParseVarDataTypeAttribute();
	}

	// Propagate data type to variable
	name->data_type = type->data_type;
	name->data_subtype = type->data_subtype;

	// Optional DEFAULT exp in Teradata, DB2, MySQL
	Token *default_ = GetNextWordToken("DEFAULT", L"DEFAULT", 7);
	Token *default_exp = NULL;

	// = is used in SQL Server to specify default
	Token *default_equal = NULL;

	if(default_ == NULL)
		default_equal = GetNextCharToken('=', '=');

	if(default_ != NULL || default_equal != NULL)
	{
		default_exp = GetNextToken();
		ParseExpression(default_exp);

		// = default_exp specified
		if(default_equal != NULL)
		{
			// Oracle uses := and DEFAULT 
			if(_target == SQL_ORACLE)
				PrependNoFormat(default_equal, ":", L":", 1);
			else
			// MySQL uses DEFAULT keyword
			if(Target(SQL_MYSQL) == true)
				ChangeWithSpacesAround(default_equal, "DEFAULT", L"DEFAULT", 7, declare);
		}
		else
		// SQL Server uses = 
		if(_target == SQL_SQL_SERVER && default_ != NULL)
			Token::Change(default_, "=", L"=", 1);
	}

	Token *last = GetLastToken();

	// If multiple variables specified copy data type and DEFAULT for Oracle, SQL Server, PostgreSQL
	if(Target(SQL_SQL_SERVER, SQL_ORACLE, SQL_POSTGRESQL) == true && num > 1)
	{
		ListwItem *item = vars.GetFirst();

		for(int i=0; i < num - 1; i++)
		{
			if(item == NULL)
				break;

			Token *var = (Token*)item->value;

			Append(var, " ", L" ", 1);
			AppendCopy(var, type, last, false);

			item = item->next;
		}
	}

	// If there are no non-declare statements above, set last declare
	if(_spl_first_non_declare == NULL)
		_spl_last_declare = last;

	// Remove DECLARE keyword for Oracle, declarations will be in DECLARE section after IS
	if(_target == SQL_ORACLE)
		Token::Remove(declare);

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	// Remove DB2 SQLCODE and SQLSTATE declarations
	if(Target(SQL_DB2) == false &&
		(Token::Compare(name, "SQLCODE", L"SQLCODE", 7) || Token::Compare(name, "SQLSTATE", L"SQLSTATE", 8)))
	{
		/*Token *semi */ (void) GetNextCharToken(';', L';');
		Token::Remove(declare, GetLastToken());
	}
	
	return true;
}

// FETCH statement
bool SqlParser::ParseFetchStatement(Token *fetch)
{
	if(fetch == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(fetch);

	// Optional FROM in DB2
	Token *from = GetNextWordToken("FROM", L"FROM", 4);

	// Cursor name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	// Oracle does not support FROM keyword
	if(from != NULL && _target == SQL_ORACLE)
		Token::Remove(from);

	_spl_last_fetch_cursor_name = name;

	// INTO keyword 
	Token *into = GetNextWordToken("INTO", L"INTO", 4);

	if(_target_app == APP_JAVA)
	{
		TokenStr rs("if (");
		rs.Append(name);
		APPENDSTR(rs, "_found = ");
		rs.Append(name);
		APPENDSTR(rs, "_rs.next())");
		Token::ChangeNoFormat(fetch, rs);

		Token::Remove(name);
		Token::Remove(into);
	}
	else
	// Netezza does not support FETCH, convert to record variable assignments
	if(_target == SQL_NETEZZA)
		Token::Remove(fetch, into);

	// Comma separated list of variables
	while(true)
	{
		// Get variable
		Token *var = GetNextIdentToken(SQL_IDENT_VAR);

		if(var == NULL)
			break;

		// Check for a fetch into a record variable
		Token *rec = Find(_spl_rowtype_vars, var);

		if(rec != NULL)
		{
			_spl_rowtype_fetches.Add(var);

			if(_target_app == APP_JAVA)
				APPEND_NOFMT(var, ".set()");
		}
		
		// Add @ for parameter names for SQL Server and Sybase
		if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
			ConvertToTsqlVariable(var);

		Token *comma = GetNextCharToken(',', L',');

		if(rec == NULL)
		{
			// Convert to record variable assignments in Netezza
			if(_target == SQL_NETEZZA)
			{
				AppendNoFormat(var, " := ", L" := ", 4);
				AppendCopy(var, name);
				AppendNoFormat(var, ".", L".", 1);
				AppendCopy(var, var);

				Token::Change(comma, ";", L";", 1);
			}
		}

		if(comma == NULL)
			break;
	}

	// Implement CONTINUE handler for NOT FOUND in Oracle
	if(_target == SQL_ORACLE)
		OracleContinueHandlerForFetch(fetch, name);

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// Oracle, DB2, Teradata FOR loop
bool SqlParser::ParseForStatement(Token *for_, int scope)
{
	if(for_ == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_FOR_LOOP_DESC)
	PL_STMS_STATS(for_);

	// Loop variable
	Token *var = GetNextIdentToken();

	if(var == NULL)
		return false;

	// IN keyword in Oracle
	Token *in = GetNextWordToken("IN", L"IN", 2);

	// AS keyword in DB2, Teradata
	Token *as = GetNextWordToken("AS", L"AS", 2);

	if(in == NULL && as == NULL)
		return false;

    // SELECT can be enclosed with ()
    Token *open = GetNext('(', L'(');

	// Next can be an integer, variable, expression, cursor, SELECT or (SELECT ...)
	Token *first = GetNextToken();

	if(first == NULL)
		return false;

	// DB2, Teradata do not allow to use a declared cursor, but a cursor name and attributes can be 
	// assigned before the SELECT statement
	if(Source(SQL_DB2, SQL_TERADATA) == true && first->Compare("SELECT", L"SELECT", 6) == false)
	{
		// first points to cursor name, then CURSOR keyword follows
		Token *cursor = GetNextWordToken("CURSOR", L"CURSOR", 6);

		// FOR keyword before SELECT
		Token *for_ = GetNextWordToken(cursor, "FOR", L"FOR", 3);

		if(for_ != NULL)
		{
			// For Oracle, delete cursor name
			if(_target == SQL_ORACLE)
				Token::Remove(first, for_);

			// Now first should point to SELECT
			first = GetNextToken();

			if(first == NULL)
				return false;
		}
	}

	Token *select = NULL;
	Token *select_end = NULL;
	Token *cursor = NULL;

	bool select_loop = false;
	bool cursor_loop = false;

	ListW out_cols;

	// SELECT statement
	if(Token::Compare(first, "SELECT", L"SELECT", 6) == true)
	{
		ParseSelectStatement(first, 0, SQL_SEL_FOR, NULL, NULL, NULL, &out_cols, NULL, NULL, NULL, NULL);

		// Save output columns and corresponding record variable name (DB2 allows referencing output columns
		// without qualified record name, while Oracle not)
		for(ListwItem *i = out_cols.GetFirst(); i != NULL; i = i->next) 
			_spl_implicit_record_fields.Add(var, i->value);
		
		select = first;
		select_end = GetLastToken();

		select_loop = true;
	}

	bool range_loop = false;

	// Rangle loop: 1..10
	if(select == NULL)
	{
		Token *dot1 = TOKEN_GETNEXT('.');
		Token *dot2 = TOKEN_GETNEXTP(dot1, '.');

		if(dot1 != NULL && dot2 != NULL)
		{
			range_loop = true;
			Token *to = GetNextToken();
			ParseExpression(to);
		}
	}

	// Cursor loop
	if(select == NULL && !range_loop)
	{
		// Check if a cursor with such name was declared
		if(Find(_spl_declared_cursors, first) != NULL)
		{
			cursor = first;
			cursor_loop = true;
		}

		// Handle cursor parameters
		if(cursor_loop == true)
		{
			Token *open = GetNextCharToken('(', L'(');
			Token *close = NULL;

			ListwmItem *formal_params = NULL;

			// Get reference to formal cursor parameters
			if(open != NULL && Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
			{
				formal_params = _spl_cursor_params.GetFirst();

				while(formal_params != NULL)
				{
					Token *cursor_name = (Token*)formal_params->value2;

					if(Token::Compare(cursor_name, cursor) == true)
						break;

					formal_params = formal_params->next;
				}
			}			

			// Handle each cursor parameter
			while(open != NULL)
			{
				Token *param = GetNextToken();

				if(param == NULL)
					break;				

				Token *comma = GetNextCharToken(',', L',');

				// Use assignment statement for variable for SQL Server, MySQL
				if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
				{
					Prepend(for_, "SET ", L"SET ", 4);

					if(formal_params != NULL)
					{
						// The target value of parameter contains variable name
						PrependCopy(for_, (Token*)formal_params->value3);

						formal_params = formal_params->next;
					}

					Prepend(for_, " = ", L" = ", 3);
					PrependCopy(for_, param);
					Prepend(for_, ";\n", L";\n", 2);

					if(comma == NULL)
						Prepend(for_, "\n", L"\n", 1);
				}

				if(comma == NULL)
					break;
			}

			if(open != NULL)
				close = GetNextCharToken(')', L')');

			// For SQL Server, MySQL cursor parameters are removed
			if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
				Token::Remove(open, close);
		}
		else
		{
			ParseExpression(first);
			/*Token *second */ (void) GetNextToken();
		}
	}

    Token *close = NULL;

    if(open != NULL)
        close = GetNext(')', L')');

	// LOOP keyword starts the block in Oracle
	Token *loop = GetNextWordToken("LOOP", L"LOOP", 4);
	Token *do_ = NULL;

	// DO keyword starts the block in DB2, Teradata
	if(loop == NULL)
		do_ = GetNextWordToken("DO", L"DO", 2);

	if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL))
	{
		// DECLARE var CURSOR AS
		Token::Change(for_, "DECLARE", L"DECLARE", 7);
		Append(var, " CURSOR", L" CURSOR", 7, for_); 
		Token::Change(Nvl(in, as), "FOR", L"FOR", 3);

		Token::Remove(open);
		Token::Remove(close);

		Append(select_end, ";", L";", 1);

		Token *begin = loop;

		// Use BEGIN keyword in SQL Server, DO in MySQL
		if(loop != NULL)
		{
            if(_target == SQL_SQL_SERVER)
			    Token::Change(loop, "BEGIN", L"BEGIN", 5);
            else
            if(Target(SQL_MARIADB, SQL_MYSQL))
			    Token::Change(loop, "DO", L"DO", 2);
		}
		else
		if(do_ != NULL)
		{
			Token::Change(do_, "BEGIN", L"BEGIN", 5);
			begin = do_;
		}

		// OPEN cursor
		Prepend(begin, "OPEN ", L"OPEN ", 5);
		PrependCopy(begin, var);
		Prepend(begin, ";\n", L";\n", 2);

		// FETCH the first row 
		Prepend(begin, "FETCH ", L"FETCH ", 6);
		PrependCopy(begin, var);
		Prepend(begin, " INTO;\n", L" INTO;\n", 7);

		// Start WHILE loop
        if(_target == SQL_SQL_SERVER)
		    Prepend(begin, "WHILE @@FETCH_STATUS=0\n", L"WHILE @@FETCH_STATUS=0\n", 23);
        else
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Prepend(begin, "WHILE not_found=0\n", L"WHILE not_found=0\n", 18);

            // Add NOT FOUND handler and variable
           	_spl_need_not_found_handler = true;
        }
	}
	else
	// Use LOOP keyword in Oracle
	if(_target == SQL_ORACLE)
	{
		// DB2, Teradata syntax
		if(as != NULL)
		{
			Token::Change(as, "IN", L"IN", 2);

			// SELECT must be enclosed in ()
			PrependNoFormat(select, "(", L"(", 1);
			AppendNoFormat(select_end, ")", L")", 1);

			Token::Change(do_, "LOOP", L"LOOP", 4);
		}
	}
	else
	// Use cursor and WHILE-FETCH in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		// OPEN cursor; FETCH cursor
		Token::Change(for_, "OPEN", L"OPEN", 4);
		Append(cursor, ";", L";", 1);
		Append(cursor, "\nFETCH ", L"\nFETCH ", 7, for_);
		AppendCopy(cursor, cursor);
		Append(cursor, " INTO ", L" INTO ", 6, for_);
		Append(cursor, ";", L";", 1);

		Append(cursor, "\nWHILE ", L"\nWHILE ", 7, for_);
		AppendNoFormat(cursor, "no_data = 0", L"no_data = 0", 11);

		Token::Remove(var);
		Token::Remove(in);

		// Use DO keyword 
		if(loop != NULL)
			Token::Change(loop, "DO", L"DO", 2);
	}

	ParseBlock(SQL_BLOCK_FOR, true, scope, NULL);

	Token *end = GetNextWordToken("END", L"END", 3);

	if(end != NULL)
	{
		// END LOOP in Oracle
		Token *loop2 = GetNextWordToken("LOOP", L"LOOP", 4);
		Token *for2 = NULL;

		// END FOR in DB2, Teradata
		if(loop2 == NULL)
			for2 = GetNextWordToken("FOR", L"FOR", 3);

		// END in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			// Fetch the next row at the end of loop
			Prepend(end, "FETCH ", L"FETCH ", 6);
			PrependCopy(end, var);
			Prepend(end, " INTO;\n", L" INTO;\n", 7);

			if(loop2 != NULL)
				Token::Remove(loop2);
			else
			if(for2 != NULL)
				Token::Remove(for2);

			Token *semi = GetNextCharOrLastToken(';', L';');

			// Close and deallocate cursor
			Append(semi, "\nCLOSE ", L"\nCLOSE ", 7, end);
			AppendCopy(semi, var);
			AppendNoFormat(semi, ";", L";", 1);

			Append(semi, "\nDEALLOCATE ", L"\nDEALLOCATE ", 12, end);
			AppendCopy(semi, var);
			AppendNoFormat(semi, ";", L";", 1);
		}
		else
		// Use END LOOP in Oracle
		if(for2 != NULL && Target(SQL_ORACLE) == true)
			Token::Change(for2, "LOOP", L"LOOP", 4);
		else
		// END WHILE in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			if(loop2 != NULL)
				Token::Change(loop2, "WHILE", L"WHILE", 5);

			// Fetch the next row at the end of loop
			Prepend(end, "FETCH ", L"FETCH ", 6);
			PrependCopy(end, cursor);
			Prepend(end, " INTO;\n", L" INTO;\n", 7);

			Token *semi = GetNextCharOrLastToken(';', L';');

			// Close cursor
			Append(semi, "\nCLOSE ", L"\nCLOSE ", 7, end);
			AppendCopy(semi, cursor);
			AppendNoFormat(semi, ";", L";", 1);
		}
	}

	// Remove record variable for the current loop
	_spl_implicit_record_fields.DeleteItems(var);		

	return true;
}

// Informix FOREACH loop
bool SqlParser::ParseForeachStatement(Token *foreach_, int scope)
{
	if(foreach_ == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(foreach_);

	// Optional WITH HOLD option before SELECT
	Token *with = GetNextWordToken("WITH", L"WITH", 4);
	Token *hold = (with != NULL) ? GetNextWordToken("HOLD", L"HOLD", 4) : NULL;
	
	if(_target != SQL_INFORMIX && with != NULL)
		Token::Remove(with, hold);

	// Cursor name or SELECT statement
	Token *select = GetNextWordToken("SELECT", L"SELECT", 6);

	// In Informix cursor name can be specified before SELECT for further references WHERE CURRENT OF cur inside the loop body
	Token *curname = (select == NULL) ? GetNextToken() : NULL;

	// FOR keyword before SELECT when cursor name is defined
	Token *for_ = NULL;

	if(select == NULL && curname == NULL)
		return false;

	// Save FOREACH number in order of apperance since it can be changed by recursive calls
	int foreach_num = ++_spl_foreach_num;

	// Cursor name is defined
	if(curname != NULL)
	{
		// Optional WITH HOLD option 
		Token *with = GetNextWordToken("WITH", L"WITH", 4);
		Token *hold = (with != NULL) ? GetNextWordToken("HOLD", L"HOLD", 4) : NULL;
	
		if(_target != SQL_INFORMIX && with != NULL)
			Token::Remove(with, hold);

		for_ = GetNextWordToken("FOR", L"FOR", 3);
		select = GetNextWordToken("SELECT", L"SELECT", 6);
	}

	ListW into_cols;
	ListW out_cols;

	// Informix uses INTO in SELECT, so FOREACH fetches into existing local variables
	ParseSelectStatement(select, 0, SQL_SEL_FOREACH, NULL, NULL, NULL, &out_cols, &into_cols, NULL, NULL, NULL);

	Token *select_end = GetLastToken();

	// Set frontier although there is not start keyword, but the block terminates with END FOREACH;
	bool frontier = true;

	// Save beginning og the block
	Token *block_start = GetNextToken();
	PushBack(block_start);

	ParseBlock(SQL_BLOCK_FOR, frontier, scope, NULL);

	// END FOREACH
	Token *end = GetNextWordToken("END", L"END", 3);
	Token *foreach2 = (end != NULL) ? GetNextWordToken("FOREACH", L"FOREACH", 7) : NULL;

	// Semicolon is optional in Informix, but required for Oracle, PostgreSQL
	Token *semi = GetNextCharToken(';', L';');

	if(foreach2 != NULL && semi == NULL && Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
		AppendNoFormat(foreach2, ";", L";", 1);

	// Convert to FOR LOOP in Oracle, PostgreSQL
	if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
	{
		Token::Change(foreach_, "FOR", L"FOR", 3);

		// Use record variable in Oracle
		if(_target == SQL_ORACLE)
		{
			TokenStr rec("rec", L"rec", 3);

			if(foreach_num > 1)
				rec.Append(foreach_num);

			AppendNoFormat(foreach_, " ", L" ", 1);
			AppendNoFormat(foreach_, &rec);

			ListwItem *c = out_cols.GetFirst();

			// In Oracle values fetched into record variable, while in Informix into local variables
			for(ListwItem *i = into_cols.GetFirst(); i != NULL; i = i->next)
			{
				Token *var = (Token*)i->value;
				Token *col = (c != NULL) ? (Token*)c->value : NULL;
				
				if(i->prev == NULL)
					Prepend(block_start, "\n", L"\n", 1);

				// Assign local variables with values from record variables				
				PrependCopy(block_start, var);
				PrependNoFormat(block_start, " := ", L" := ", 4);
				PrependNoFormat(block_start, &rec);
				PrependNoFormat(block_start, ".", L".", 1);
				PrependCopy(block_start, col);
				Prepend(block_start, ";\n", L";\n", 2);

				if(c != NULL)
					c = c->next;
			}
		}
		else
		// For PostgreSQL a list of variable can be used instead of record variable
		if(_target == SQL_POSTGRESQL)
		{
			// Specify list of INTO variables
			for(ListwItem *i = into_cols.GetFirst(); i != NULL; i = i->next)
			{
				if(i->prev != NULL)
					AppendNoFormat(foreach_, ",", L",", 1);

				AppendNoFormat(foreach_, " ", L" ", 1);
				AppendCopy(foreach_, (Token*)i->value);
			}
		}
		
		// Cursor name declared (and can be further referenced in WHERE CURRENT OF)
		if(curname != NULL)
		{
			Prepend(curname, "IN ", L"IN ", 3, foreach_);
			
			if(_target == SQL_ORACLE)
				Token::Change(for_, "IS", L"IS", 2);

			// Add cursor declaration
			Token *append = Append(GetDeclarationAppend(), "\n\n", L"\n\n", 2);
			AppendCopy(append, curname);
			Append(append, " CURSOR ", L" CURSOR ", 8, foreach_);
			AppendCopy(append, for_, select_end, false);
			
			// Append FOR UPDATE if the cursor used in UPDATE, DELETE WHERE CURRENT OF 
			Token *updatable = Find(_spl_updatable_current_of_cursors, curname);

			if(updatable != NULL)
				Append(append, " FOR UPDATE", L" FOR UPDATE", 11, foreach_);
			
			AppendNoFormat(append, ";", L";", 1);

			// Remove SELECT statement from FOREACH
			Token::Remove(for_, select_end);
		}
		// SELECT statement in specified
		else
		{
			Prepend(select, "IN ", L"IN ", 3, foreach_);

			// Oracle requires () around SELECT
			if(_target == SQL_ORACLE)
			{
				PrependNoFormat(select, "(", L"(", 1);
				AppendNoFormat(select_end, ")", L")", 1);
			}
		}

		Append(select_end, " LOOP", L" LOOP", 5, foreach_);
		Token::Change(foreach2, "LOOP", L"LOOP", 4);
	}
	else
	// DECLARE CURSOR, OPEN, WHILE and FETCH in SQL Server
	if(Target(SQL_SQL_SERVER) == true)
	{
		TokenStr curs;

		// Cursor name can be optionally specified
		if(curname != NULL)
			curs.Append(curname);
		else
			curs.Append("cur", L"cur", 3);

		// DECLARE cur CURSOR FOR
		Token::Change(foreach_, "DECLARE", L"DECLARE", 7);
		Prepend(for_, "CURSOR ", L"CURSOR ", 7);

		AppendNoFormat(select_end, ";", L";", 1);

		// OPEN cur
		Append(select_end, "\nOPEN ", L"\nOPEN ", 6, foreach_);
		AppendNoFormat(select_end, &curs);
		AppendNoFormat(select_end, ";", L";", 1);

		// FETCH INTO
		Append(select_end, "\nFETCH ", L"\nFETCH ", 7, foreach_);
		AppendNoFormat(select_end, &curs);
		Append(select_end, " INTO", L" INTO", 5, foreach_);

		// Specify list of INTO variables
		for(ListwItem *i = into_cols.GetFirst(); i != NULL; i = i->next)
		{
			if(i->prev != NULL)
				AppendNoFormat(select_end, ",", L",", 1);

			AppendNoFormat(select_end, " ", L" ", 1);
			AppendCopy(select_end, (Token*)i->value);
		}

		AppendNoFormat(select_end, ";", L";", 1);

		// WHILE cursor loop
		Append(select_end, "\nWHILE @@FETCH_STATUS = 0 BEGIN", L"\nWHILE @@FETCH_STATUS = 0 BEGIN", 31, foreach_);

		// FETCH INTO at the end of loop
		Prepend(end, "FETCH ", L"\nFETCH ", 6, foreach_);
		PrependNoFormat(end, &curs);
		Prepend(end, " INTO", L" INTO", 5, foreach_);

		// Specify list of INTO variables
		for(ListwItem *i = into_cols.GetFirst(); i != NULL; i = i->next)
		{
			if(i->prev != NULL)
				PrependNoFormat(end, ",", L",", 1);

			PrependNoFormat(end, " ", L" ", 1);
			PrependCopy(end, (Token*)i->value);
		}

		Prepend(end, ";\n", L";\n", 2);

		Token::Remove(foreach2, semi);

		// Close and deallocate cursor
		Append(end, "\nCLOSE ", L"\nCLOSE ", 7, foreach_);
		AppendNoFormat(end, &curs);
		AppendNoFormat(end, ";", L";", 1);

		Append(end, "\nDEALLOCATE ", L"\nDEALLOCATE ", 12, foreach_);
		AppendNoFormat(end, &curs);
		AppendNoFormat(end, ";", L";", 1);
	}

	return true;
}

// Informix FREE statement
bool SqlParser::ParseFreeStatement(Token *free)
{
	if(free == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(free);

	// Statement ID or cursor
	Token *name = GetNextToken();

	if(name == NULL)
		return false;

	// Not required for Oracle
	if(_target == SQL_ORACLE)
	{
		Token *semi = GetNextCharToken(';', L';');

		Token::Remove(free, Nvl(semi, GetLastToken()));
	}

	return true;
}

// HELP statement in Teradata
bool SqlParser::ParseHelpStatement(Token *help)
{
	if(help == NULL)
		return false;

	if(_source == SQL_TERADATA && ParseTeradataHelpStatement(help) == true)
		return true;

	return false;
}

// IF statement
bool SqlParser::ParseIfStatement(Token *if_, int scope)
{
	if(if_ == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_IF_DESC)
	PL_STMS_STATS(if_)

	int old_spl_scope = _spl_scope;
	_spl_current_stmt = if_;

	// Force block scope to handle delimiters
	if(_spl_scope == 0)
		_spl_scope = SQL_SCOPE_PROC;

    // Check for ELSE IF construct
    bool prev_else = TOKEN_CMP(GetPrevToken(if_), "ELSE"); 

	ParseBooleanExpression(SQL_BOOL_IF, if_);

	if(_target_app == APP_JAVA)
	{
		TOKEN_CHANGE_NOFMT(if_, "if");

		if(!TOKEN_CMPC(GetLastToken(), ')'))
		{
			APPEND_NOFMT(if_, "(");
			APPEND_NOFMT(GetLastToken(), ")");
		}
	}

	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true)
	{
		Token *begin = NULL;
		Token *end = NULL; 
		Token *last_true = NULL;
		Token *last_condition = GetLastToken();

		Token *next = GetNextToken();

		if(next == NULL)
			return false;

		// In SQL Server, either a single statement or BEGIN can go next
		if(next->Compare("BEGIN", L"BEGIN", 5) == true)
		{
			begin = next;
			ParseBlock(SQL_BLOCK_IF, true, scope, NULL);

			end = GetNextWordToken("END", L"END", 3);
		}
		else
		{
			ParseStatement(next, scope, NULL);
			last_true = GetLastToken();
		}

		// Now check is ELSE is specified
		Token *else_ = GetNextWordToken("ELSE", L"ELSE", 4);

		Token *else_begin = NULL;
		Token *else_end = NULL;
		Token *last_false = NULL;

		if(else_ != NULL)
		{
			next = GetNextToken();

			// Block or a single statement can go in ELSE
			if(next->Compare("BEGIN", L"BEGIN", 5) == true)
			{
				else_begin = next;
				ParseBlock(SQL_BLOCK_IF, true, scope, NULL);

				else_end = GetNextWordToken("END", L"END", 3);
			}
			else
			{
                // Check for ELSE IF
                if(TOKEN_CMP(next, "IF"))
                {
                    // Convert to ELSEIF for MySQL and MariaDB
                    if(Target(SQL_MYSQL, SQL_MARIADB))
                    {
                        TOKEN_CHANGE(else_, "ELSEIF");
                        Token::Remove(next);
                    }
                }

				ParseStatement(next, scope, NULL);
				last_false = GetLastToken();
			}
		}

		// Other databases use THEN END IF;
		if(Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
		{
			// If BEGIN goes after condition replace it with THEN
			if(begin != NULL)
			{
				Token::Change(begin, "THEN", L"THEN", 4);

				// If ELSE exists remove END in true block
                // Also if it is part of ELSE IF then only outer IF will contain END IF
				if(else_ != NULL || prev_else)
					Token::Remove(end);
				else
					// Otherwise change END to END IF
					Token::Change(end, "END IF;", L"END IF;", 7);
			}
			else
				// Append THEN right after boolean condition
				Append(last_condition, " THEN", L" THEN", 5, if_);

			// Remove BEGIN after ELSE
			Token::Remove(else_begin);

			// Change END in ELSE block to END IF
			Token::Change(else_end, "END IF;", L"END IF;", 7);

			// No END at the end of statement
            // Also if it is part of ELSE IF then only outer IF will contain END IF
			if(!prev_else && (last_true != NULL || last_false != NULL))
			{
				Token *last = (last_false != NULL) ? last_false : last_true;
				
				Append(last, "\nEND IF;", L"\nEND IF;", 8, if_);
			}
		}
	}
	// Oracle, DB2, MySQL, PostgreSQL, Informix, Sybase ASA, Sybase ADS IF THEN ELSEIF/ELSIF ELSE END IF;
	else
	{
		bool then_to_begin = false;

		// Variable number of ELSIF (Oracle) or ELSEIF (DB2, MySQL) conditions
		while(true)
		{
			Token *then = GetNextWordToken("THEN", L"THEN", 4);

			if(then == NULL)
				return false;

			if(_target_app == APP_JAVA)
				TOKEN_CHANGE_NOFMT(then, "{");
			else
			// For SQL Server use BEGIN unless true block is defined as THEN BEGIN ... END ELSE ...
			if(_target == SQL_SQL_SERVER)
			{
				// Check if BEGIN follows
				Token *next_begin = GetNext("BEGIN", L"BEGIN", 5);

				if(next_begin == false)
				{
					Token::Change(then, "BEGIN", L"BEGIN", 5);
					then_to_begin = true;
				}
				else
				{
					Token::Remove(then);
					PushBack(next_begin);
				}
			}

			ParseBlock(SQL_BLOCK_IF, true, scope, NULL);

			// ELSEIF in DB2 and MySQL
			Token *elseif = GetNextWordToken("ELSEIF", L"ELSEIF", 6);
			Token *elsif = NULL;

			// ELSIF in Oracle
			if(elseif == NULL)
				elsif = GetNextWordToken("ELSIF", L"ELSIF", 5);

			if(elseif == NULL && elsif == NULL)
				break;

			// ELSEIF condition
			ParseBooleanExpression(SQL_BOOL_IF);

			if(_target_app == APP_JAVA)
				TOKEN_CHANGE_NOFMT(Nvl(elseif, elsif), "} else if");
			else
			if(_target == SQL_ORACLE && elseif != NULL)
				Token::Change(elseif, "ELSIF", L"ELSIF", 5);
			else
			// SQL Server does not supprt ELSEIF construct
			if(_target == SQL_SQL_SERVER)
			{
				Token *elf = Nvl(elseif, elsif);

				// Close true block with END if there was not BEGIN in source
				if(then_to_begin)
					PREPEND(elf, "END\n"); 

				TOKEN_CHANGE(elf, "ELSE IF");
			}
			else
			if(Target(SQL_DB2, SQL_MYSQL) == true && elsif != NULL)
				Token::Change(elsif, "ELSEIF", L"ELSEIF", 6);
		}

		// Optional ELSE condition
		Token *else_ = GetNextWordToken("ELSE", L"ELSE", 4);
		
		if(else_ != NULL)
		{
			ParseBlock(SQL_BLOCK_IF, true, scope, NULL);

			if(_target_app == APP_JAVA)
				TOKEN_CHANGE_NOFMT(else_, "} else {");
			else
			// For SQL Server end block before ELSE and begin again
			if(_target == SQL_SQL_SERVER && then_to_begin == true)
			{
				Prepend(else_, "END\n", L"END\n", 4);
				Append(else_, " BEGIN", L" BEGIN", 6);
			}
		}

		// END IF
		Token *end = GetNextWordToken("END", L"END", 3);

		// ENDIF in Sybase ADS
		Token *endif = (end == NULL) ? TOKEN_GETNEXTW("ENDIF") : NULL;

		if(end != NULL || endif != NULL)
		{
			Token *end_if = GetNextWordToken("IF", L"IF", 2);
			Token *semi = GetNextCharToken(';', L';');

			if(_target_app == APP_JAVA)
			{
				TOKEN_CHANGE_NOFMT(Nvl(end_if, end), "}");
				Token::Remove(end);
				Token::Remove(semi);
			}
			else
			// For SQL Server use just END, semicolon is optional in SQL Server
			if(_target == SQL_SQL_SERVER)
			{
				Token::Remove(end_if);

				// Remove last END as well if BEGIN END is also used around true/false blocks 
				if(end != NULL && then_to_begin == false)
					Token::Remove(end);

				if(endif != NULL)
					TOKEN_CHANGE(endif, "END");

				// Prevent text merge in case of "end if;select..."
				Token::Change(semi, " ", L" ", 1);
			}
			else
			// Semicolon is optional in Informix, but required for Oracle, PostgreSQL
			if(semi == NULL && Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
				AppendNoFormat(end_if, ";", L";", 1);
		}
	}
	
	if(_spl_first_non_declare == NULL)
		_spl_first_non_declare = if_;

	_spl_current_stmt = NULL;
	_spl_last_stmt = if_;
	_spl_scope = old_spl_scope; 

	return true;
}

// WHILE statement 
bool SqlParser::ParseWhileStatement(Token *while_, int scope)
{
	if(while_ == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_WHILE_DESC)
	PL_STMS_STATS(while_);

	// WHILE FETCH cur DO ... in Sybase ADS
	if(_source == SQL_SYBASE_ADS)
	{
		Token *fetch = TOKEN_GETNEXTW("FETCH");

		if(fetch != NULL)
		{
			ParseSybaseWhileFetchStatement(while_, fetch, scope);
			return true;
		}
	}

	ParseBooleanExpression(SQL_BOOL_WHILE);

	// LOOP keyword in Oracle
	Token *loop = GetNext("LOOP", L"LOOP", 4);

	// DO keyword in DB2, MySQL, Teradata
	Token *do_ = (loop == NULL) ? GetNext("DO", L"DO", 2) : NULL;

	// BEGIN keyword in SQL Server, Sybase ASE
	Token *begin = (loop == NULL && do_ == NULL) ? GetNext("BEGIN", L"BEGIN", 5) : NULL;

	// Informix does not use any block start keyword
	Token *condition_end = GetLastToken();

	// Use BEGIN keyword in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(loop != NULL)
			Token::Change(loop, "BEGIN", L"BEGIN", 5);
		else
		if(do_ != NULL)
			Token::Change(do_, "BEGIN", L"BEGIN", 5);
	}
	else
	// Use LOOP keyword in Oracle, PostgreSQL
	if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true && loop == NULL)
	{
		if(do_ != NULL) 
			Token::Change(do_, "LOOP", L"LOOP", 4);
		else
		if(begin != NULL) 
			Token::Change(begin, "LOOP", L"LOOP", 4);
		else
			Append(condition_end, " LOOP", L" LOOP", 5, while_);
	}
	else
	// Use DO keyword in DB2, MySQL
	if(Target(SQL_DB2, SQL_MYSQL, SQL_MARIADB, SQL_TERADATA) == true)
	{
		if(loop != NULL) 
			Token::Change(loop, "DO", L"DO", 2);
		else
		if(begin != NULL) 
			Token::Change(begin, "DO", L"DO", 2);
	}

	ParseBlock(SQL_BLOCK_WHILE, true, scope, NULL);

	// END clause
	ParseWhileStatementEnd();

	return true;
}

// END clause of WHILE statement
bool SqlParser::ParseWhileStatementEnd()
{
	// Just END in SQL Server, Sybase ASE
	Token *end = GetNextWordToken("END", L"END", 3);

	if(end == NULL)
		return false;

	// END LOOP in Oracle
	Token *loop = GetNextWordToken("LOOP", L"LOOP", 4);

	// END WHILE in DB2, Informix, Teradata, MySQL
	Token *while_ = (loop == NULL && !Source(SQL_SQL_SERVER, SQL_SYBASE)) ? GetNextWordToken("WHILE", L"WHILE", 5) : NULL;

	// END in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(loop != NULL)
			Token::Remove(loop);
		else
		if(while_ != NULL)
			Token::Remove(while_);
	}
	else
	// Use END LOOP in Oracle, PostgreSQL
	if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
	{
		if(while_ != NULL)
			Token::Change(while_, "LOOP", L"LOOP", 4);
		else
		if(loop == NULL && Source(SQL_ORACLE, SQL_DB2, SQL_INFORMIX, SQL_TERADATA, SQL_MYSQL) == false)
			Append(end, " LOOP", L" LOOP", 5);
	}
	else
	// Use END WHILE in DB2, MySQL, MariaDB, Teradata
	if(Target(SQL_DB2, SQL_MYSQL, SQL_MARIADB, SQL_TERADATA))
	{
		if(loop != NULL)
			Token::Change(loop, "WHILE", L"WHILE", 5);
		else
		if(while_ == NULL && Source(SQL_ORACLE, SQL_DB2, SQL_INFORMIX, SQL_TERADATA, SQL_MYSQL, SQL_MARIADB) == false)
			Append(end, " WHILE", L" WHILE", 6);
	}
	
	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// INSERT statement
bool SqlParser::ParseInsertStatement(Token *insert)
{
	if(insert == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_STMT_INSERT_DESC)

	Token *into = GetNextWordToken("INTO", L"INTO", 4);

	if(into == NULL)
		return false;

	Token *table_name = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(table_name == NULL)
		return false;

	_spl_last_insert_table_name = table_name;

	// Optional column list
	Token *open1 = TOKEN_GETNEXT('(');

	// Check for table alias before the column list (Oracle i.e.)
	if(open1 == NULL)
	{
		Token *alias = GetNextToken();

		// Make sure it is not VALUES keyword
		if(!TOKEN_CMP(alias, "VALUES"))
			open1 = TOKEN_GETNEXT('(');

		if(open1 == NULL)
			PushBack(alias);
	}

	ListWM cols;

	if(open1 != NULL)
	{
		bool column_list = true;

		// Make sure it is not (SELECT ...) 
		Token *select = GetNextWordToken("SELECT", L"SELECT", 6);
		Token *comma = (select != NULL) ? GetNextCharToken('(', L'(') : NULL;

		if(select != NULL && comma == NULL)
		{
			PushBack(open1);
			column_list = false;
		}

		// Get columns
		while(column_list)
		{
			// Column name
			Token *col = GetNextIdentToken(SQL_IDENT_COLUMN_SINGLE);

			if(col == NULL)
				break;

			// Comma or )
			Token *del = GetNextToken();

			cols.Add(col, del);

			if(Token::Compare(del, ',', L','))
				continue;

			break;
		}
	}

	// VALUES or SELECT can go next
	Token *values = GetNextWordToken("VALUES", L"VALUES", 6);
	Token *select = NULL;
	Token *open2 = NULL;

	if(values == NULL)
	{
		// SELECT can be optionaly enclosed with ()
		open2 = GetNextCharToken('(', L'('); 
		
		select = GetNextSelectStartKeyword();
	}

	if(values == NULL && select == NULL)
	{
	    STMS_STATS(insert);
		return false;
	}

	// VALUES clause
	if(values != NULL)
	{
		int rows = 0;
		Token *close2 = NULL;

		Enter(SQL_SCOPE_INSERT_VALUES);

		// For MySQL multiple comma-separated rows can be specified
		while(true)
		{
			Token *open2 = GetNextCharToken('(', L'(');
			close2 = NULL;

			if(open2 == NULL)
			{
			    STMS_STATS(insert);
				return false;
			}

			int num = 0;

			// Get list of values
			while(true)
			{
				// Insert expression
				Token *exp = GetNextToken();

				if(exp == NULL)
					break;

				ParseExpression(exp);

				// Comma or )
				close2 = GetNextToken();
				
				bool val_removed = false;

				// Check for seq.NEXTVAL column
				if(exp->type == TOKEN_IDENT && exp->subtype == TOKEN_SUB_IDENT_SEQNEXTVAL)
				{
					// Remove for SQL Server as identity column will be used
					if(_target == SQL_SQL_SERVER)
					{
						Token::Remove(exp);

						// Remove column name from column list if any
						ListwmItem *col = cols.GetNth(num);

						if(col != NULL)
							Token::Remove((Token*)col->value, (Token*)col->value2);

						val_removed = true;
					}
				}

				if(Token::Compare(close2, ',', L','))
				{
					if(val_removed)
						Token::Remove(close2);

					num++;
					continue;
				}

				break;
			}

			// Check if another row specified
			Token *comma = GetNextCharToken(',', L',');

			rows++;

			// For Oracle, convert multiple rows to SELECT UNION ALL
			if(_target == SQL_ORACLE && (comma != NULL || rows > 1))
			{
				if(rows == 1)
					Token::Remove(values);

				Prepend(open2, " SELECT ", L" SELECT ", 8, values);
				Append(close2, " FROM ", L" FROM ", 6, values);
				AppendNoFormat(close2, "dual", L"dual", 4);

				if(comma != NULL)
					Append(close2, " UNION ALL ", L" UNION ALL ", 11, values);

				Token::Remove(open2);
				Token::Remove(close2);
				Token::Remove(comma);
			}

			if(comma == NULL)
				break;
		}

		Leave(SQL_SCOPE_INSERT_VALUES);

		// RETURNING clause in Oracle
		Token *returning = TOKEN_GETNEXTW("RETURNING");

		if(returning != NULL)
		{
			Token *col = GetNextIdentToken(SQL_IDENT_COLUMN_SINGLE);
			Token *into = TOKEN_GETNEXTWP(col, "INTO");
			Token *var = GetNextIdentToken();

			// SET var = SCOPE_IDENTITY() in SQL Server
			if(_target == SQL_SQL_SERVER)
			{
				ConvertToTsqlVariable(var);

				TOKEN_CHANGE(into, "SET");
				APPEND_FMT(var, " = SCOPE_IDENTITY()", into);

				APPEND_NOFMT(close2, ";");
				Token::Remove(returning, col);
			}
		}
	}
	// SELECT clause
	else
	{
		ParseSelectStatement(select, 0, SQL_SEL_INSERT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);	

        if(open2 != NULL) 
		    /*Token *close2 */ (void) GetNextCharToken(')', L')');
	}

	// Implement CONTINUE handler in Oracle
	if(_target == SQL_ORACLE)
		OracleContinueHandlerForInsert(insert);

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

    STMS_STATS(insert);

	return true;
}

// Informix LET statement
bool SqlParser::ParseLetStatement(Token *let)
{
	if(let == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(let);

	// Check for pattern LET var = DBINFO('sqlca.sqlerrd1') that gets the last SERIAL value
	if(_target == SQL_ORACLE && InformixPatternAssignLastSerial(let) == true)
		return true;

	// Use SET for SQL Server, DB2, MySQL
	if(Target(SQL_SQL_SERVER, SQL_DB2, SQL_MYSQL) == true)
		Token::Change(let, "SET", L"SET", 3);

	return ParseSetStatement(let);
}

// LEAVE statement in DB2, Sybase ADS
bool SqlParser::ParseLeaveStatement(Token *leave)
{
	if(leave == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(leave);

	// Mandatory label name in DB2; no label in Sybase ADS
	Token *label = GetNextToken();

	if(TOKEN_CMPC(label, ';'))
	{
		PushBack(label);
		label = NULL;
	}

	// EXIT label or RETURN in Oracle
	if(_target == SQL_ORACLE)
	{
		// Use RETURN from procedure if it is the outer procedure block label
		if(Token::Compare(_spl_outer_label, label) == true)
		{
			Token::Change(leave, "RETURN", L"RETURN", 6);
			Token::Remove(label);
		}
		else
			Token::Change(leave, "EXIT", L"EXIT", 4);
	}
	else
	if(_target == SQL_SQL_SERVER && label == NULL)
		TOKEN_CHANGE(leave, "BREAK");

	return true;
}

// DB2, PostgreSQL GET DIAGNOSTICS statement
bool SqlParser::ParseGetStatement(Token *get)
{
	if(get == NULL)
		return false;

	Token *diagnostics = GetNextWordToken("DIAGNOSTICS", L"DIAGNOSTICS", 11);

	if(diagnostics == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS_V("GET DIAGNOSTICS", get);

	// GET DIAGNOSTICS EXCEPTION 1 var = type in DB2
	Token *exception = GetNextWordToken("EXCEPTION", L"EXCEPTION", 9);
	Token *num = NULL;

	if(exception != NULL)
		num = GetNextNumberToken();

	Token *var = GetNextIdentToken();
	Token *equal = GetNextCharToken('=', L'=');

	if(equal == NULL)
		return false;

	Token *type = GetNextToken();

	if(type == NULL)
		return false;

	// GET DIAGNOSTICS var = ROW_COUNT
	if(type->Compare("ROW_COUNT", L"ROW_COUNT", 9) == true)
	{
		// Use var := SQL%ROWCOUNT in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Remove(get, diagnostics);
			Prepend(equal, ":", L":", 1);
			Token::Change(type, "SQL%ROWCOUNT", L"SQL%ROWCOUNT", 12);
		}
	}
	else
	// GET DIAGNOSTICS EXCEPTION 1 var = MESSAGE_TEXT
	if(type->Compare("MESSAGE_TEXT", L"MESSAGE_TEXT", 12) == true)
	{
		// Use var := SQLERRM in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Remove(get, diagnostics);
			Token::Remove(exception, num);

			Prepend(equal, ":", L":", 1);
			Token::Change(type, "SQLERRM", L"SQLERRM", 7);
		}
	}
	else
	// GET DIAGNOSTICS var = DB2_RETURN_STATUS
	if(type->Compare("DB2_RETURN_STATUS", L"DB2_RETURN_STATUS", 17) == true)
	{
		// Get last CALL statement
		ListwmItem *last_call = _spl_sp_calls.GetLast();

		// Use OUT parameter for the last called procedure in Oracle
		if(_target == SQL_ORACLE && last_call != NULL)
		{
			Token *called_proc = (Token*)last_call->value;
			Token *last_param_end = (Token*)last_call->value2;

			// Procedure has parameters
			if(last_param_end != NULL)
			{
				AppendNoFormat(last_param_end, ", ", L", ", 2);
				AppendCopy(last_param_end, var);
			}
			else
			{
				AppendNoFormat(called_proc, "(", L"(", 1);
				AppendCopy(called_proc, var);
				AppendNoFormat(called_proc, ")", L")", 1);
			}

			Token::Remove(get, Nvl(GetNextCharToken(';', L';'), GetLastToken()));
		}
	}

	return true;
}

// GRANT statement
bool SqlParser::ParseGrantStatement(Token *grant)
{
	if(grant == NULL)
		return false;

	STATS_DECL
    STMS_STATS(grant);

	Token *priv = GetNextToken();

	if(priv == NULL)
		return false;

	// EXECUTE ON in Informix, PostgreSQL, Sybase ASE
	if(priv->Compare("EXECUTE", L"EXECUTE", 7) == true)
	{
		Token *on = GetNextWordToken("ON", L"ON", 2);

		// PROCEDURE or FUNCTION keyword
		Token *procedure = GetNextWordToken("PROCEDURE", L"PROCEDURE", 9);
		Token *function = NULL;

		if(procedure == NULL)
			function = GetNextWordToken("FUNCTION", L"FUNCTION", 8);
		else
		// Convert to fuction in PostgreSQL
		if(_target == SQL_POSTGRESQL)
			Token::Change(procedure, "FUNCTION", L"FUNCTION", 8);

		// Procedure or function name
		if(on != NULL)
		/*Token *name */ (void) GetNextIdentToken(SQL_IDENT_OBJECT);

		// Parameters in Informix and PostgreSQL
		Token *open = GetNextCharToken('(', L'(');

		if(open != NULL)
		{
			// Parameter mode (IN, OUT) and name are optional, datatype is required in PostgreSQL and Informix 
			while(true)
			{
				Token *next = GetNextToken();

				if(next == NULL)
					break;

				if(next->Compare(')', L')') == true)
					break;
			}
		}
	}
	else
	// USAGE ON LANGUAGE in Informix, PostgreSQL
	if(priv->Compare("USAGE", L"USAGE", 5) == true)
	{
		/*Token *on */ (void) GetNextWordToken("ON", L"ON", 2);

		// LANGUAGE keyword
		Token *language = GetNextWordToken("LANGUAGE", L"LANGUAGE", 8);

		// Language name
		Token *name = GetNextToken(language);

		// SPL in Informix
		if(Token::Compare(name, "SPL", L"SPL", 3) == true)
		{
			if(_target == SQL_POSTGRESQL)
				Token::Change(name, "PLPGSQL", L"PLPGSQL", 7);
		}
	}
	else
	// DROP TABLE, USER etc. in Oracle
	if(TOKEN_CMP(priv, "DROP"))
		/*Token *what */ GetNextToken();

	// TO grantee
	Token *to = GetNextWordToken("TO", L"TO", 2);
	/*Token *grantee */ (void) GetNextToken(to);

	// Optional AS grantor in Informix
	Token *as = GetNextWordToken("AS", L"AS", 2);

	if(as != NULL)
	{
		Token *grantor = GetNextToken();

		// Not supported in PostgreSQL
		if(_target == SQL_POSTGRESQL)
			Token::Remove(as, grantor);
	}

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// LOCK statement in Oracle
bool SqlParser::ParseLockStatement(Token *lock_)
{
	if(lock_ == NULL)
		return false;

	STATS_DECL
    STMS_STATS(lock_);

	// TABLE keyword in Oracle
	Token *table = GetNext("TABLE", L"TABLE", 5);

	if(table != NULL)
	{
		// TABLES in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
			Token::Change(table, "TABLES", L"TABLES", 6);
	}

	// Table name
	/*Token *name */ (void) GetNextIdentToken(SQL_IDENT_OBJECT);

	// Optional IN mode
	Token *in = GetNext("IN", L"IN", 2);

	if(in != NULL)
	{
		// EXCLUSIVE in Oracle
		Token *exclusive = GetNext("EXCLUSIVE", L"EXCLUSIVE", 9);

		// SHARE in Oracle
		Token *share = (exclusive == NULL) ? GetNext("SHARE", L"SHARE", 5) : NULL;

		// MODE in Oracle
		Token *mode = GetNext("MODE", L"MODE", 4);

		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			// WRITE in MySQL 
			if(exclusive != NULL && mode != NULL)
			{
				Token::Change(in, "WRITE", L"WRITE", 5);
				Token::Remove(exclusive, mode);
			}
			else
			// READ in MySQL 
			if(share != NULL && mode != NULL)
			{
				Token::Change(in, "READ", L"READ", 4);
				Token::Remove(share, mode);
			}
		}
	}
		
	return true;
}

// LOOP statement in Oracle, DB2
bool SqlParser::ParseLoopStatement(Token *loop, int scope)
{
	if(loop == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(loop);

	if(_target_app == APP_JAVA)
		TOKEN_CHANGE_NOFMT(loop, "while (true) {");
	else
	// Use WHILE 1=1 BEGIN in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(loop, "WHILE 1=1 BEGIN", L"WHILE 1=1 BEGIN", 15);

    _spl_loops.Add(loop);

	ParseBlock(SQL_BLOCK_LOOP, true, scope, NULL);

    _spl_loops.DeleteLast();

	Token *end = GetNextWordToken("END", L"END", 3);

	if(end == NULL)
		return false;

	// END LOOP in Oracle, DB2
	Token *loop2 = GetNextWordToken("LOOP", L"LOOP", 4);

	// Optional loop lable name in Oracle, DB2
	Token *end_name = GetNextToken(loop2);

	if(end_name != NULL)
	{
		// Check if it follows by ;
		Token *semi = GetNextCharToken(';', L';');

		// Not a lable
		if(semi == NULL)
			PushBack(end_name);
		else
			PushBack(semi);
	}

	if(_target_app == APP_JAVA)
	{
		TOKEN_CHANGE_NOFMT(end, "}");
		Token::Remove(loop2, TOKEN_GETNEXT(';'));
	}
	else
	// END in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Remove(loop2);

	return true;
}

// MERGE statement
bool SqlParser::ParseMergeStatement(Token *merge)
{
	if(merge == NULL)
		return false;

	Token *into = TOKEN_GETNEXTW("INTO");

	if(into == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_STMT_MERGE_DESC)

	Token *table = GetNextIdentToken();
	Token *alias = GetNextToken(table);

	Token *using_ = NULL;

	if(TOKEN_CMP(alias, "USING"))
	{
		using_ = alias;
		alias = NULL;
	}
	else
		using_ = TOKEN_GETNEXTWP(table, "USING");

	Token *next = GetNextToken(using_);

	Token *src_table = NULL;
	Token *src_alias = NULL;

	// table | (subquery) alias
	if(TOKEN_CMPC(next,'('))
	{
		ParseSelectStatement(next, 0, SQL_SEL_MERGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		/*Token *close */ TOKEN_GETNEXT(')');
	}
	else
		src_table = next;

	next = GetNextToken(using_);
	Token *on = NULL;

	// ON (boolean expression)
	if(!TOKEN_CMP(next, "ON"))
	{
		src_alias = next;
		on = TOKEN_GETNEXTWP(using_, "ON");
	}
	else
		on = next;

	Token *open = TOKEN_GETNEXTP(on, '(');

	ParseBooleanExpression(SQL_BOOL_MERGE);

	/*Token *close */ TOKEN_GETNEXTP(open, ')');

	while(true)
	{
		// WHEN [NOT] MATCHED THEN
		Token *when = TOKEN_GETNEXTW("WHEN");
		/*Token *not_ */ TOKEN_GETNEXTWP(when, "NOT");
		Token *matched = TOKEN_GETNEXTWP(when, "MATCHED");
		Token *then = TOKEN_GETNEXTWP(matched, "THEN");

		if(then == NULL)
			break;

		bool exists = false;
		Token *operation = GetNextToken();

		// UPDATE or INSERT statements
		if(TOKEN_CMP(operation, "UPDATE"))
			exists = ParseUpdateStatement(operation);
		else
		// Note INSERT clause slightly differs from standard INSERT statment, so we parse it individually
		if(TOKEN_CMP(operation, "INSERT"))
		{
			// Optional column list
			Token *open_cols = TOKEN_GETNEXT('(');

			while(open_cols != NULL)
			{
				// Column name
				Token *col = GetNextIdentToken(SQL_IDENT_COLUMN_SINGLE);

				if(col == NULL)
					break;

				// Comma or )
				Token *del = GetNextToken();

				if(TOKEN_CMPC(del, ','))
					continue;

				break;
			}

			Token *values = TOKEN_GETNEXTW("VALUES");

			// VALUES clause
			while(values != NULL)
			{
				// Insert expression
				Token *exp = GetNextToken();

				if(exp == NULL)
					break;

				ParseExpression(exp);

				// Comma or )
				Token *del = GetNextToken();

				if(TOKEN_CMPC(del, ','))
					continue;

				break;
			}

			if(values != NULL)
				exists = true;
		}

		if(!exists)
			break;
	}

	STMS_STATS(merge);
	
	return true;
}

// Oracle NULL statement
bool SqlParser::ParseNullStatement(Token *null_)
{
	if(_source != SQL_ORACLE)
		return false;

	STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_NULL_DESC)
	STATS_SET_CONV_NO_NEED(Target(SQL_MARIADB_ORA));
	PL_STMS_STATS(null_);

	return true;
}

// Informix ON EXCEPTION
bool SqlParser::ParseOnStatement(Token *on)
{
	if(on == NULL)
		return false;

	bool exists = false;

	Token *next = GetNextToken();

	// Informix ON EXCEPTION block
	if(Token::Compare(next, "EXCEPTION", L"EXCEPTION", 9) == true)
		exists = ParseOnExceptionStatement(on, next);

	return exists;
}

// Oracle EXCEPTION block
bool SqlParser::ParseExceptionBlock(Token *exception)
{
	if(exception == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_EXCEPTION_BLOCK_DESC)
	
	bool exists = false;
	int cnt = 0;

	// List of WHEN conditions 
	while(true)
	{
		bool add_end_if = false;

		Token *when = GetNext("WHEN", L"WHEN", 4);

		if(when == NULL)
			break;

		// Exception type
		Token *condition = GetNext();

		if(condition == NULL)
			break;

		Token *then = GetNext("THEN", L"THEN", 4);

		if(then == NULL)
			break;

		STATS_DTL_DECL

		if(_target_app == APP_JAVA)
		{
			if(cnt > 0)
				PREPEND(when, "}\n");

			TOKEN_CHANGE_NOFMT(when, "catch");
			TOKEN_CHANGE_NOFMT(then, "{");
			PREPEND_NOFMT(condition, "(");
			APPEND_NOFMT(condition, " e)");
		}
		else
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			Token::Change(when, "DECLARE EXIT HANDLER FOR", L"DECLARE EXIT HANDLER FOR", 24);
			Token::Change(then, "BEGIN", L"BEGIN", 5);
			
			Token::Remove(exception);
		}

		// Duplicate key
		if(TOKEN_CMP(condition, "DUP_VAL_ON_INDEX"))
		{
			if(Target(SQL_MARIADB, SQL_MYSQL))
				Token::Change(condition, "SQLSTATE '23000'", L"SQLSTATE '23000'", 16);

			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_DUP_VAL_ON_INDEX_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))
		}
		else
		// No rows found
		if(TOKEN_CMP(condition, "NO_DATA_FOUND"))
		{
			if(Target(SQL_MARIADB, SQL_MYSQL))
				Token::Change(condition, "NOT FOUND", L"NOT FOUND", 9);
			else
			if(_target == SQL_NETEZZA)
			{
				Token::Change(when, "IF", L"IF", 2);
				Token::Change(condition, "NOT FOUND", L"NOT FOUND", 9);

				Token::Remove(exception);

				add_end_if = true;
			}

			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_NO_DATA_FOUND_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))
		}
		else
		// All others
		if(TOKEN_CMP(condition, "OTHERS"))
		{
			if(_target_app == APP_JAVA)
				TOKEN_CHANGE_NOFMT(condition, "SQLException");
			else
			if(Target(SQL_MARIADB, SQL_MYSQL))
				Token::Change(condition, "SQLEXCEPTION", L"SQLEXCEPTION", 12);

			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_OTHERS_DESC)
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))
		}
		else if(TOKEN_CMP(condition, "ACCESS_INTO_NULL")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_ACCESS_INTO_NULL_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "CASE_NOT_FOUND")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_CASE_NOT_FOUND_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "COLLECTION_IS_NULL")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_COLLECTION_IS_NULL_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "CURSOR_ALREADY_OPEN")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_CURSOR_ALREADY_OPEN_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "INVALID_CURSOR")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_INVALID_CURSOR_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "INVALID_NUMBER")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_INVALID_NUMBER_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "LOGIN_DENIED")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_LOGIN_DENIED_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "NO_DATA_NEEDED")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_NO_DATA_NEEDED_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "NOT_LOGGED_ON")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_NOT_LOGGED_ON_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "PROGRAM_ERROR")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_PROGRAM_ERROR_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "ROWTYPE_MISMATCH")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_ROWTYPE_MISMATCH_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "SELF_IS_NULL")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_SELF_IS_NULL_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "STORAGE_ERROR")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_STORAGE_ERROR_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "SUBSCRIPT_BEYOND_COUNT")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_SUBSCRIPT_BEYOND_COUNT_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "SUBSCRIPT_OUTSIDE_LIMIT")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_SUBSCRIPT_OUTSIDE_LIMIT_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "SYS_INVALID_ROWID")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_SYS_INVALID_ROWID_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "TIMEOUT_ON_RESOURCE")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_TIMEOUT_ON_RESOURCE_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "TOO_MANY_ROWS")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_TOO_MANY_ROWS_DESC) 
			STATS_DTL_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA))
		} 
		else if(TOKEN_CMP(condition, "VALUE_ERROR")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_VALUE_ERROR_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 
		else if(TOKEN_CMP(condition, "ZERO_DIVIDE")) { 
			STATS_DTL_DESC(PLSQL_STMT_EXCEPTION_ZERO_DIVIDE_DESC) 
			STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_MEDIUM, "", "")
		} 

		// Check if optional declaration section follows
		Token *declare = GetNext("DECLARE", L"DECLARE", 7);

		if(declare != NULL)
		{
			ParseOracleVariableDeclarationBlock(declare);

			// Now BEGIN should follow
			Token *begin = GetNext("BEGIN", L"BEGIN", 5);

			if(begin != NULL)
			{
				// For MySQL declaration put inside BEGIN-END and THEN was converted to BEGIN keyword
				if(Target(SQL_MARIADB, SQL_MYSQL))
					Token::Remove(begin);
			}
		}

		ParseBlock(SQL_BLOCK_EXCEPTION, true /*frontier*/, SQL_SCOPE_PROC, NULL);

		// DECLARE-BEGIN-END;
		if(declare != NULL)
		{
			Token *end = GetNext("END", L"END", 3);
			/*Token *semi */ (void) GetNext(end, ';', L';');
		}

		if(add_end_if == true)
			Append(GetLastToken(), "\nEND IF;", L"\nEND IF;", 8, when); 

        if(Target(SQL_MARIADB, SQL_MYSQL))
		    Append(GetLastToken(), "\nEND;", L"\nEND;", 5, when);

		PL_STMS_EXCEPTION_STATS(condition)
		STATS_UPDATE_STATUS

		exists = true;
		cnt++;
	}

	if(exists)
	{
		if(_target_app == APP_JAVA)
		{
			Token::Remove(exception);
			APPEND(GetLastToken(), "\n}");
		}
		else
		// Move to the declaration section of the current block
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			ListwItem *i = _spl_begin_blocks.GetLast();
			Token *begin = NULL;

			if(i != NULL)
				begin = (Token*)i->value;

			Token *last = GetLastToken();

			if(begin != NULL)
			{
				AppendCopy(begin, exception, last, false);
				Token::Remove(exception, last);
			}
		}
	}

	PL_STMS_STATS(exception);

	return exists;
}

// Informix ON EXCEPTION
bool SqlParser::ParseOnExceptionStatement(Token *on, Token *exception)
{
	if(on == NULL || exception == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS_V("ON EXCEPTION", on);

	// SET var clause can optionally follow to set error code to existing variable
	Token *set = GetNextWordToken("SET", L"SET", 3);

	if(set != NULL)
	{
		// Existing local variable name
		Token *var = GetNextToken();

		// Use var := SQLCODE in Oracle
		if(_target == SQL_ORACLE)
		{
			AppendNoFormat(var, " := ", L" := ", 4);
			Append(var, "SQLCODE;", L"SQLCODE;", 8, exception);

			Token::Remove(set);
		}

		// Then ISAM variable can optionally follow
		Token *comma = GetNextCharToken(',', L',');

		if(comma != NULL)
		{
			// Existing local variable for ISAM error
			Token *isam = GetNextToken();

			if(_target == SQL_ORACLE)
				Token::Remove(comma, isam);
		}
	}

	// Set frontier as the exception terminates with END EXCEPTION;
	bool frontier = true;

	ParseBlock(SQL_BLOCK_HANDLER, frontier, 0, NULL);

	// END EXCEPTION;
	Token *end = GetNextWordToken("END", L"END", 3);
	Token *exception2 = NULL;

	if(end != NULL)
		exception2 = GetNextWordToken("EXCEPTION", L"EXCEPTION", 9);

	// Convert to EXCEPTION WHEN OTHERS THEN in Oracle and PostgreSQL
	if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
	{
		int cut_type = COPY_EXIT_HANDLER_SQLEXCEPTION;

		// EXCEPTION keyword will be added once per block
		Token::Change(exception, " WHEN OTHERS THEN", L" WHEN OTHERS THEN", 17);

		Token::Remove(on);
		Token::Remove(end, exception2);
		Token::Remove(GetNextCharToken(';', L';'));

		Token *last = GetLastToken();

		// Block will be transferred to EXCEPTION section 
		Cut(COPY_SCOPE_PROC, cut_type, NULL, exception, last);
	}

	return true;
}

// OPEN cursor statement
bool SqlParser::ParseOpenStatement(Token *open)
{
	if(open == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_OPEN_DESC)

	// Cursor name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	_spl_last_open_cursor_name = name;
	_spl_open_cursors.Add(open, name);

	// Optional FOR keyword
	Token *for_ = GetNextWordToken("FOR", L"FOR", 3); 
	Token *select = NULL;

	// SELECT keyword or variable can go next
	if(for_ != NULL)
	{
		select = GetNextWordToken("SELECT", L"SELECT", 6);

		// FOR SELECT
		if(select != NULL)
		{
			ParseSelectStatement(select, 0, SQL_SEL_OPEN, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

			// For SQL Server remove OPEN cur FOR
			if(_target == SQL_SQL_SERVER)
				Token::Remove(open, for_);
			else
			// FOR cur IN EXECUTE variable LOOP in Netezza
			if(_target == SQL_NETEZZA)
			{
				Token::Change(open, "FOR", L"FOR", 3);
				Token::Change(for_, "IN EXECUTE", L"IN EXECUTE", 10);

				Token *semi = GetNext(';', L';');

				// LOOP statement must follow and now it is part of FOR IN EXECUTE statement
				if(semi != NULL)
					Token::Remove(semi);
			}
		}
		else
		// FOR variable
		{
			// Variable or string containing SQL statement; string literal can be part of concatenation expression
			Token *exp = ParseExpression();
			Token *exp_end = GetLastToken();
			
			// USING var, ... clause
			Token *using_ = TOKEN_GETNEXTW("USING");

			if(using_ != NULL)
			{
				while(true)
				{
					Token *var = GetNextIdentToken();

					if(var == NULL)
						break;

					Token *comma = TOKEN_GETNEXT(',');

					if(comma == NULL)
						break;
				}
			}
			
			if(_target == SQL_SQL_SERVER)
			{
				// Single literal or variable
				if(exp == exp_end)
				{
					if(exp->type == TOKEN_STRING)
					{
						TOKEN_CHANGE(open, "EXECUTE");
						TOKEN_CHANGE_NOFMT(name, "sp_executesql");

						// Only unicode literals are allowed
						PREPEND_NOFMT(exp, "N");						
					}
					// Variable; EXECUTE (@variable) in SQL Server, must be enclosed with () unless stored procedure name
					else
					{
						TOKEN_CHANGE(open, "EXECUTE");

						// Add @ before the name
						ConvertToTsqlVariable(exp);

						PREPEND_NOFMT(exp, "(");
						APPEND_NOFMT(exp_end, ")");
	
						TOKEN_CHANGE(open, "EXECUTE");
						Token::Remove(name, for_);
					}
				}
				// Multi-literal expression; sp_executesql does not allow expressions
				else
				{
					// Use a variable to keep dynamic SQL expression
					TOKEN_CHANGE(open, "SET");
					TOKEN_CHANGE_NOFMT(name, "@sql = ");
						
					APPEND_NOFMT(exp_end, ";");
					APPEND_FMT(exp_end, "\nEXECUTE", open);
					APPEND_NOFMT(exp_end, " sp_executesql @sql");

					Token::Remove(for_);
					_spl_dyn_sql_var = true;
				}

				if(using_ != NULL)
				{
					APPEND_NOFMT(exp_end, ",");
					Token::Remove(using_);
				}
			}
			else
			// FOR cur IN EXECUTE variable LOOP in Netezza
			if(_target == SQL_NETEZZA)
			{
				Token::Change(open, "FOR", L"FOR", 3);
				Token::Change(for_, "IN EXECUTE", L"IN EXECUTE", 10);

				Token *semi = GetNext(';', L';');

				// LOOP statement must follow and now it is part of FOR IN EXECUTE statement
				if(semi != NULL)
					Token::Remove(semi);
			}
		}
	}
	// FOR is not specified
	else
	{
		Token *prepared_select = NULL;

		// Cursor can be open for a prepared statement in Informix, find its declaration
		ListwmItem *d = Find(_spl_prepared_stmts_cursors, name);

		if(d != NULL)
		{
			Token *stmt_id = (Token*)d->value2;			

			// Find prepared statement and get SELECT variable
			ListwmItem *p = Find(_spl_prepared_stmts, stmt_id);

			if(p != NULL)
				prepared_select = (Token*)p->value2;
		}

		// Prepared cursor
		if(prepared_select != NULL)
		{
			if(_target == SQL_ORACLE)
			{
				Append(name, " FOR ", L" FOR ", 5, open);
				AppendCopy(name, prepared_select);
			}
		}
		else
		{
			// Add FOR SELECT for WITH RETURN cursors
			if(Target(SQL_ORACLE, SQL_SQL_SERVER))
			{
				if(OpenWithReturnCursor(name))
				{
					// For SQL Server leave only SELECT, remove OPEN cur
					if(_target == SQL_SQL_SERVER)
						Token::Remove(open, name);
				}
			}
		}

		// Cursor parameters can be specified in Oracle
		Token *open2 = GetNextCharToken('(', L'(');
		Token *close2 = NULL;

		ListwmItem *formal_params = NULL;

		// Get reference to the first formal parameter for this cursor
		if(open2 != NULL && Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
		{
			formal_params = _spl_cursor_params.GetFirst();

			while(formal_params != NULL)
			{
				Token *cursor_name = (Token*)formal_params->value2;

				if(Token::Compare(cursor_name, name) == true)
					break;

				formal_params = formal_params->next;
			}
		}			

		// Handle each cursor parameter
		while(open2 != NULL)
		{
			Token *param = GetNextToken();

			if(param == NULL)
				break;			

			ParseExpression(param);

			Token *comma = GetNextCharToken(',', L',');

			// Use assignment statement for variable for SQL Server, MySQL
			if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
			{
				Prepend(open, "SET ", L"SET ", 4);

				if(formal_params != NULL)
				{
					// The target value of parameter contains variable name
					PrependCopy(open, (Token*)formal_params->value3);

					formal_params = formal_params->next;
				}

				Prepend(open, " = ", L" = ", 3);
				PrependCopy(open, param);
				Prepend(open, ";\n", L";\n", 2);

				if(comma == NULL)
					Prepend(open, "\n", L"\n", 1);
			}

			if(comma == NULL)
				break;
		}

		if(open2 != NULL)
			close2 = GetNextCharToken(')', L')');

		// For SQL Server, MySQL cursor parameters are removed
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
			Token::Remove(open2, close2);

		if(_target_app == APP_JAVA)
		{
			TokenStr rs("ResultSet ");
			rs.Append(name);
			APPENDSTR(rs, "_rs = stmt.executeQuery(");
			PrependNoFormat(name, &rs);
			APPEND_NOFMT(name, ")");
			Token::Remove(open);
		}
		else
		// FOR cur IN SELECT LOOP in Netezza
		if(_target == SQL_NETEZZA)
		{
			Token::Change(open, "FOR", L"FOR", 3);
			Append(name, " IN ", L" IN ", 4, open);

			// Find associated SELECT statement
			ListwmItem *s = Find(_spl_declared_cursors_select, name);

			if(s != NULL)
			{
				// Append SELECT statement
				AppendCopy(name, (Token*)s->value2, (Token*)s->value3);
				// Remove SELECT from the cursor declaration
				Token::Remove((Token*)s->value2, (Token*)s->value3);
			}

			Token *semi = GetNext(';', L';');

			// LOOP statement must follow and now it is part of FOR IN SELECT statement
			if(semi != NULL)
				Token::Remove(semi);
		}
	}

	Token *semi = GetNextCharToken(';', L';');

	// OPEN WHILE(SQLCODE = 0) DO FETCH pattern in Teradata
	if(PatternSqlCodeCursorLoop(name) == false)
		PushBack(semi);

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	PL_STMS_STATS(open);

	return true;
}

// PostgreSQL PERFORM
bool SqlParser::ParsePerformStatement(Token *perform)
{
	if(perform == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(perform);

	Token *func = GetNextToken();

	if(func == NULL)
		return false;

	// Should be function call with parameters
	ParseExpression(func);

	// For Oracle remove PERFORM keyword
	if(_target == SQL_ORACLE)
		Token::Remove(perform);

	return true;
}

// PREPARE statement in Informix
bool SqlParser::ParsePrepareStatement(Token *prepare)
{
	if(prepare == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_PREPARE_DESC)
	PL_STMS_STATS(prepare);

	// Statement ID that will be referenced in DECLARE CURSOR FOR stmt_id i.e.
	Token *stmt_id = GetNextToken();

	if(stmt_id == NULL)
		return false;

	// FROM keyword
	Token *from = GetNextWordToken("FROM", L"FROM", 4);

	if(from == NULL)
		return false;

	// Statement text or variable
	Token *stmt = GetNextToken();

	if(stmt == NULL)
		return false;

	_spl_prepared_stmts.Add(stmt_id, stmt);

	// Oracle does not have PREPARE statement in PL/SQL, remove it
	if(_target == SQL_ORACLE)
	{
		Token *semi = GetNextCharToken(';', L';');

		Token::Remove(prepare, Nvl(semi, GetLastToken()));
	}

	return true;
}

// PRINT statement in SQL Server
bool SqlParser::ParsePrintStatement(Token *print)
{
	if(print == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(print);

	// Expression can be specified
	Token *exp = GetNext();

	ParseExpression(exp);

	// Comment for MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Comment(print, Nvl(GetNext(';', L';'), GetLastToken()));

	return true;
}

// PROMPT statement in Oracle SQL*Plus
bool SqlParser::ParsePromptStatement(Token *prompt)
{
	if(prompt == NULL)
		return false;

	STATS_DECL
    STMS_STATS(prompt);

	Token *first = GetNextToken();
	Token *last = NULL;

	Token *cur = first;

	// Parse tokens until newline
	while(cur)
	{
		// Check if the previous non-space token is a newline 
		Token *prev = cur->prev;

		// Skip spaces
		while(prev && (Token::Compare(prev, ' ', L' ') == true || Token::Compare(prev, '\t', L'\t') == true))
			prev = prev->prev;

		if(Token::Compare(prev, '\r', L'\r') == true || Token::Compare(prev, '\n', L'\n') == true)
		{
			PushBack(cur);
			break;
		}

		last = cur;
		cur = GetNextToken();
	}

	// PRINT 'message' in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(prompt, "PRINT", L"PRINT", 5);
		PrependNoFormat(first, "'", L"'", 1);
		AppendNoFormat(last, "'", L"'", 1);

		// Add GO to show the message even if there is an error in batch
		Append(last, "\nGO", L"\nGO", 3, prompt);
	}

	return true;
}

// REM or REMARK command in Oracle SQL*Plus (single line comment)
bool SqlParser::ParseRemStatement(Token *rem)
{
	if(rem == NULL)
		return false;

	STATS_DECL
    STMS_STATS(rem);

	// Comment text
	/*Token *comment */ (void) GetNextUntilNewlineToken();

	// Use -- in other databases
	if(_target != SQL_ORACLE)
		Token::Change(rem, "--", L"--", 2);

	return true;
}

// Oracle, PostgreSQL RAISE statement; Informix RAISE EXCEPTION 
bool SqlParser::ParseRaiseStatement(Token *raise)
{
	if(raise == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_RAISE_DESC)
	PL_STMS_STATS(raise);

	// EXCEPTION, NOTICE etc.
	Token *notice = GetNextWordToken("NOTICE", L"NOTICE", 6);
	Token *exception = NULL;

	if(notice == NULL)
		exception = GetNextWordToken("EXCEPTION", L"EXCEPTION", 9);

	// Standalone RAISE
	if(notice == NULL && exception == NULL)
	{
		// Exception name in Oracle
		Token *name = GetNextToken();

		if(_target_app == APP_JAVA)
		{
			TOKEN_CHANGE_NOFMT(raise, "throw new ");
			APPEND_NOFMT(name, "()");
		}
		else
		// Netezza does not support standalone RAISE
		if(_target == SQL_NETEZZA)
			Append(raise, " EXCEPTION ''", L" EXCEPTION ''", 13);

		return true;
	}

	// Format and parameters in PostgreSQL
	Token *format = NULL;
	ListW params;

	// Error code, isam error variable and text message in Informix
	Token *error_code = NULL;
	Token *isam_error = NULL;
	Token *text = NULL;

	if(_source == SQL_POSTGRESQL)
		format = GetNextToken();
	else
	if(_source == SQL_INFORMIX)
	{
		error_code = GetNextToken();

		Token *comma = GetNextCharToken(',', L',');

		// ISAM error is optional
		if(comma != NULL)
			isam_error = GetNextToken();

		comma = GetNextCharToken(',', L',');

		// Message text is optional
		if(comma != NULL)
			text = GetNextToken();
	}
	
	// String literal representing format in PostgreSQL
	if(format != NULL && format->type == TOKEN_STRING)
	{
		// Parse parameters
		while(true)
		{
			Token *comma = GetNextCharToken(',', L',');

			// No parameters anymore
			if(comma == NULL)
				break;

			Token *exp = GetNextToken();

			if(exp == NULL)
				break;

			ParseExpression(exp);

			params.Add(exp);

			// Remove comma for Oracle as expressions will be concatenated
			if(_target == SQL_ORACLE)
				Token::Remove(comma);
		}
	}

	// RAISE NOTICE
	if(notice != NULL)
	{
		// DBMS_OUTPUT.PUT_LINE() in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Change(raise, "DBMS_OUTPUT.PUT_LINE(", L"DBMS_OUTPUT.PUT_LINE(", 21);
			Token::Remove(notice);

			// Transform format string with parameters to concatenation expression
			PostgresFormatToConcat(format, params);

			AppendNoFormat(GetLastToken(), ")", L")", 1);
		}
	}	
	else
	// RAISE EXCEPTION
	if(exception != NULL)
	{
		// Use RAISE_APPLICATION_ERROR in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Change(raise, "RAISE_APPLICATION_ERROR(", L"RAISE_APPLICATION_ERROR(", 24);
			Token::Remove(exception);

			AppendNoFormat(GetLastToken(), ")", L")", 1);
		}
	}

	return true;
}

// DB2 REPEAT statement
bool SqlParser::ParseRepeatStatement(Token *repeat, int scope)
{
	if(repeat == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(repeat);

	ParseBlock(SQL_BLOCK_REPEAT, true, scope, NULL);

	// UNTIL keyword
	Token *until = GetNextWordToken("UNTIL", L"UNTIL", 5);

	if(until == NULL)
		return false;

	ParseBooleanExpression(SQL_BOOL_UNTIL);

	Token *end_condition = GetLastToken();

	// END REPEAT
	Token *end = GetNextWordToken("END", L"END", 3);
	Token *repeat2 = GetNextWordToken(end, "REPEAT", L"REPEAT", 6);

	// Optional loop lable name 
	Token *end_name = GetNextToken(repeat2);

	if(end_name != NULL)
	{
		// Check if it followed by ;
		Token *semi = GetNextCharToken(';', L';');

		// Not a lable
		if(semi == NULL)
			PushBack(end_name);
		else
			PushBack(semi);
	}

	// LOOP stmt EXIT WHEN condition; END LOOP label; in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(repeat, "LOOP", L"LOOP", 4);

		Token::Change(until, "EXIT WHEN", L"EXIT WHEN", 9);
		AppendNoFormat(end_condition, ";", L";", 1);

		Token::Change(repeat2, "LOOP", L"LOOP", 4);
	}

	return true;
}

// REPLACE statement in Teradata
bool SqlParser::ParseReplaceStatement(Token *replace)
{
	if(replace == NULL)
		return false;

	// REPLACE PROCEDURE in Teradata
	Token *procedure = GetNextWordToken("PROCEDURE", L"PROCEDURE", 9);

	if(procedure != NULL)
	{
		if(_target != SQL_TERADATA)
			Token::Change(replace, "CREATE", L"CREATE", 6);

		ParseCreateProcedure(replace, NULL, NULL, procedure, NULL);
	}

	return true;
}

// RESIGNAL statement in DB2
bool SqlParser::ParseResignalStatement(Token *resignal)
{
	if(resignal == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(resignal);

	// RAISE in Oracle 
	if(_target == SQL_ORACLE)
		Token::Change(resignal, "RAISE", L"RAISE", 5);

	return true;
}

// RETURN statement
bool SqlParser::ParseReturnStatement(Token *return_)
{
	if(return_ == NULL)
		return false;

    STATS_DECL
	STATS_SET_DESC(PLSQL_STMT_RETURN_DESC)
	STATS_SET_CONV_NO_NEED(Source(SQL_ORACLE) && Target(SQL_MARIADB_ORA));
	PL_STMS_STATS(return_);

	_spl_return_num++;
	_spl_last_stmt = return_;
	
	if(_spl_first_non_declare == NULL)
		_spl_first_non_declare = return_;

	// MySQL, MariaDB do not allow RETURN in procedures
	if(_spl_scope == SQL_SCOPE_PROC && Target(SQL_MYSQL, SQL_MARIADB))
	{
		TOKEN_CHANGE(return_, "LEAVE");
		APPEND_NOFMT(return_, " sp_lbl");
	}

	// Check for RETURN without any expression
	Token *semi = GetNextCharToken(';', L';');

	if(semi != NULL)
	{
		PushBack(semi);
		return true;
	}

	// In SQL Server, Sybase ASE RETURN can be without expression and semicolon, check that it is not followed by END
	if(Source(SQL_SQL_SERVER, SQL_SYBASE))
	{
		Token *end = TOKEN_GETNEXTW("END");

		if(end != NULL)
		{
			PushBack(end);

			// Add statement delimiter if not set when source is SQL Server/Sybase
			SqlServerAddStmtDelimiter();			
			return true;
		}
	}

	int num = 0;

	Token *start = NULL;
	Token *end = NULL;

	ListWM return_list;

	// Multiple return expressions can be specified in Informix
	while(true)
	{
		// Get the expression
		Token *exp = GetNextToken();

		if(start == NULL)
			start = exp;

		ParseExpression(exp);
		num++;

		Token *comma = GetNextCharToken(',', L',');

		_spl_returning_out_names.Add(exp, comma);
		return_list.Add(exp, comma);

		if(comma == NULL)
		{
			// WITH RESUME can be specified in Informix
			Token *with = GetNextWordToken("WITH", L"WITH", 4);
			Token *resume = GetNextWordToken(with, "RESUME", L"RESUME", 6);

			if(resume != NULL)
			{
				_spl_return_with_resume++;

				if(_target != SQL_INFORMIX)
					Token::Remove(with, resume);
			}

			end = GetLastToken();

			break;
		}
	}

	// Regular RETURN statement
	if(_spl_return_with_resume == 0)
	{
		// Multiple scalar values are returned
		if(_spl_returning_datatypes.GetCount() > 1)
		{
			// Use separate assignment statements for OUT variables in Oracle
			if(_spl_scope == SQL_SCOPE_PROC && num > 0 && Target(SQL_ORACLE) == true)
			{
				InformixReturnToAssignment(return_list);

				Token::Remove(return_);
			}
		}
		// Single value is returned
		else		
		{
			// SQL Server procedure can return the status code (integer value), Oracle does not allow this
			if(_spl_scope == SQL_SCOPE_PROC && num > 0 && Target(SQL_ORACLE) == true)
			{
				ListwmItem *i = return_list.GetFirst();

				Token *value = (i != NULL) ? (Token*)i->value : NULL;

				// Check for number
				if(value != NULL && value->IsNumeric() == true)
					CommentNoSpaces(value);
			}
		}
	}
	// RETURN WITH RESUME in Informix
	else
	{
		// Use PIPE ROW in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Change(return_, "PIPE ROW", L"PIPE ROW", 8);
			PrependNoFormat(start, "(", L"(", 1);
			PrependCopy(start, _spl_name);
			PrependNoFormat(start, "_row(", L"_row(", 5);
			AppendNoFormat(end, "))", L"))", 2);
		}		
		else
		// Use INSERT into table in SQL Server (table-valued function)
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(return_, "INSERT INTO", L"INSERT INTO", 11);

			PrependNoFormat(start, "@", L"@", 1);
			PrependCopy(start, _spl_name);
			PrependNoFormat(start, "_tab", L"_tab", 4);

			Prepend(start, " VALUES (", L" VALUES (", 9, return_);
			AppendNoFormat(end, ")", L")", 1);
		}		
	}

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();
	
	return true;
}

// REVOKE statement
bool SqlParser::ParseRevokeStatement(Token *revoke)
{
	if(revoke == NULL)
		return false;

	STATS_DECL
    STMS_STATS(revoke);

	Token *priv = GetNextToken();

	if(priv == NULL)
		return false;

	// USAGE ON LANGUAGE in Informix, PostgreSQL
	if(priv->Compare("USAGE", L"USAGE", 5) == true)
	{
		/*Token *on */ (void) GetNextWordToken("ON", L"ON", 2);

		// LANGUAGE keyword
		Token *language = GetNextWordToken("LANGUAGE", L"LANGUAGE", 8);

		// Language name
		Token *name = GetNextToken(language);

		// SPL in Informix
		if(Token::Compare(name, "SPL", L"SPL", 3) == true)
		{
			if(_target == SQL_POSTGRESQL)
				Token::Change(name, "PLPGSQL", L"PLPGSQL", 7);
		}
	}

	// FROM grantee
	Token *from = GetNextWordToken("FROM", L"FROM", 4);
	/*Token *grantee */ (void) GetNextToken(from);

	// Optional AS revoker in Informix
	Token *as = GetNextWordToken("AS", L"AS", 2);

	if(as != NULL)
	{
		Token *revoker = GetNextToken();

		// Not supported in PostgreSQL
		if(_target == SQL_POSTGRESQL)
			Token::Remove(as, revoker);
	}

	return true;
}

// ROLLBACK statement
bool SqlParser::ParseRollbackStatement(Token *rollback)
{
	if(rollback == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(rollback);

	// WORK keyword in Informix
	Token *work = GetNextWordToken("WORK", L"WORK", 4);

	// Optional TRANSACTION keyword in PostgreSQL, Sybase ASE
	Token *transaction = (work == NULL) ? GetNextWordToken("TRANSACTION", L"TRANSACTION", 11) : NULL;

	// TRAN in SQL Server and Sybase ASE
	Token *tran = (work == NULL && transaction == NULL) ? TOKEN_GETNEXTW("TRAN") : NULL;

	// TO savepoint_ident
	Token *to = TOKEN_GETNEXTW("TO");

	if(to != NULL)
		/*Token *savepoint_ident */ GetNextToken();

	// PostgreSQL and Greenplum does not allow ROLLBACK in a procedure
	if(_spl_scope == SQL_SCOPE_PROC && Target(SQL_POSTGRESQL, SQL_GREENPLUM) == true)
		Comment(rollback, Nvl(GetNextCharToken(';', L';'), work));
	else
	// MySQL, MariaDB use ROLLBACK
	if(Target(SQL_MYSQL, SQL_MARIADB))
	{
		Token::Remove(transaction);
		Token::Remove(tran);
	}

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// SAVEPOINT statement
bool SqlParser::ParseSavepointStatement(Token *savepoint)
{
	if(savepoint == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(savepoint);

	/*Token *savepoint_ident */ GetNextToken();

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	return true;
}

// SQL Server, DB2, MySQL SET statement (variable assignment); Informix LET
bool SqlParser::ParseSetStatement(Token *set)
{
	if(set == NULL)
		return false;

   	// First try to parse SET options such as SET ISOLATION ...
	if(Token::Compare(set, "SET", L"SET", 3) == true && ParseSetOptions(set) == true)
		return true;	

    STATS_DECL
	PL_STMS_STATS(set);

	// MySQL allows to have SELECT inside an expression as a parameter of a function
	// SET var = IFNULL((SELECT ...), 0);
	bool select_deep_inside = false;

	// Comma-separated list of assignments can be specified
	while(true)
	{
		// SET (var, var2, ...) = (value, value2, ...) list or (SELECT c1, c2, ...) can be specified in DB2
		// LET var, var2, ... = value, value2, ... or (SELECT c1, c2, ...) in Informix
		Token *open = GetNextCharToken('(', L'(');

		Token *var = GetNext();

		if(var == NULL)
			break;

		// Check if a parameter exists with this name
		Token *param = GetParameter(var);

		// Change only if it was changed at the declaration
		if(param != NULL && param->t_len != 0)
			ConvertParameterIdentifier(var, param);
		else
		{
			PushBack(var);
			var = GetNextIdentToken(SQL_IDENT_VAR);
		}

		if(var == NULL)
			return false;

		// Add @ for parameter names for SQL Server and Sybase (variables already converted)
		if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
			ConvertToTsqlVariable(var);

		// In DB2 trigger, column can be assigned without NEW correlation name, make sure it is not an existing local variable
		if(_source == SQL_DB2 && _spl_scope == SQL_SCOPE_TRIGGER && GetVariable(var) == NULL)
			Db2TriggerCorrelatedName(var);
	
		ListW vars;
		int num = 1;

		// List of variables
		while(true)
		{
			Token *comma = GetNextCharToken(',', L',');

			if(comma == NULL)
				break;

			// Separate assignment used in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true)
				Token::Remove(comma);

			// Get next variable
			Token *varn = GetNextIdentToken(SQL_IDENT_COLVAR);

			if(varn == NULL)
				break;

			// Add @ for parameter names for SQL Server and Sybase (variable is already declared)
			if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
				ConvertToTsqlVariable(varn);

			vars.Add(varn);
			num++;
		}

		Token *close = (open != NULL) ? GetNextCharToken(')', L')') : NULL;

		// = sign
		Token *equal = GetNextCharToken('=', L'=');

		if(equal == NULL)
			return false;

		Enter(SQL_SCOPE_ASSIGNMENT_RIGHT_SIDE);

		// var = (SELECT...) or (var,...) = (SELECT
		Token *open2 = GetNextCharToken('(', L'(');

		// SELECT statement can be specified
		Token *select = GetNextWordToken("SELECT", L"SELECT", 6);

		if(select == NULL)
		{
			// SET var = (var2 + var3)/3 is possible so check the number of assignments
			if(open2 != NULL && num == 1)
			{
				// ( is a part of expression, not assignment list
				PushBack(open2);
				open2 = NULL;
			}

			// But SELECT still can be inside an expression in MySQL
			_exp_select = 0; 

			Token *exp = GetNextToken();
			ParseExpression(exp);

			if(_exp_select > 0)
				select_deep_inside = true;

			_exp_select = 0;

			// List of assignment expressions
			for(ListwItem *i = vars.GetFirst(); i != NULL; i = i->next)
			{
				Token *comma = GetNextCharToken(',', L',');

				if(comma == NULL)
					break;

				// Get next expression
				Token *expn = GetNextToken();
				ParseExpression(expn);

				// Convert to separate statements in SQL Server and Oracle
				if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true)
				{
					Token::Change(comma, ";", L";", 1);

					// Append variable name with assignment operator
					if(i->value != NULL)
					{
						Token *var = (Token*)i->value;
												
						if(_target == SQL_SQL_SERVER)
						{
							Prepend(expn, "SET ", L"SET ", 4, set);
							PrependCopy(expn, var);	
							Prepend(expn, " = ", L" = ", 3);
						}
						else
						if(_target == SQL_ORACLE)
						{
							PrependCopy(expn, var);	
							Prepend(expn, " := ", L" := ", 4);
						}

						Token::Remove(var);
					}
				}
			}
		}
		// SELECT statement is specified
		else
		{
			Token *list_end = NULL;
			ListW sel_exp;

			ParseSelectStatement(select, 0, SQL_SEL_EXP, NULL, &list_end, &sel_exp, NULL, NULL, NULL, NULL, NULL);
		
			// Use @var = exp in SQL Server
			if(_target == SQL_SQL_SERVER)
			{
				ListwItem *e_item = sel_exp.GetFirst();

				// Add first variable before first SELECT expression
				if(e_item != NULL && e_item->value != NULL)
				{
					Token *expn = (Token*)e_item->value;

					PrependCopy(expn, var); 
					PrependNoFormat(expn, " = ", L" = ", 3); 

					e_item = e_item->next;
				}

				ListwItem *item = vars.GetFirst();

				// Add other variables
				while(item != NULL && item->value != NULL && e_item != NULL)
				{
					Token *varn = (Token*)item->value;
					Token *expn = (Token*)e_item->value;

					PrependCopy(expn, varn); 
					PrependNoFormat(expn, " = ", L" = ", 3); 

					Token::Remove(varn);

					item = item->next;
					e_item = e_item->next;
				}
			}
			else
			// Convert to INTO clause in Oracle
			if(_target == SQL_ORACLE)
			{
				Append(list_end, " INTO ", L" INTO ", 6);
				AppendCopy(list_end, var);

				ListwItem *item = vars.GetFirst();

				// Add other variables
				while(item != NULL && item->value != NULL)
				{
					Token *varn = (Token*)item->value;

					Append(list_end, ", ", L", ", 2);
					AppendCopy(list_end, varn);

					Token::Remove(varn);
					item = item->next;
				}
			}
		}

		Token *close2 = (open2 != NULL) ? GetNextCharToken(')', L')') : NULL;

		Leave(SQL_SCOPE_ASSIGNMENT_RIGHT_SIDE);

		// In SQL Server convert to separate SET statements, so braces can be removed
		if(_target == SQL_SQL_SERVER)
		{
			if(select != NULL)
			{
				Token::Remove(set);
				Token::Remove(var, equal);
			}

			Token::Remove(open);
			Token::Remove(close);
			Token::Remove(open2);
			Token::Remove(close2);
		}
		else
		// Convert to var := exp in Oracle and PostgreSQL
		if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
		{
			if(select_deep_inside == false || _target == SQL_POSTGRESQL)
			{
				Token::Remove(set, false);

				if(select != NULL)
					Token::Remove(var, equal);
				else
					PrependNoFormat(equal, ":", L":", 1);

				Token::Remove(open);
				Token::Remove(close);
				Token::Remove(open2);
				Token::Remove(close2);
			}
			// There is a SELECT deep inside assignment expression, it is not allowed in Oracle
			// Convert to SELECT exp INTO var FROM dual
			else
			{
				Token::Change(set, "SELECT", L"SELECT", 6);

				Token *last = GetLastToken();

				Append(last, " INTO ", L" INTO ", 6, set);
				AppendCopy(last, var);
				Append(last, " FROM ", L" FROM ", 6, set);
				AppendNoFormat(last, "dual", L"dual", 4);

				Token::Remove(var, equal);
			}
		}

		// Another assignment expression
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		// Separate statements in Oracle
		if(_target == SQL_ORACLE)
			Token::Change(comma, ";", L";", 1);
	}		

	// Add statement delimiter if not set when source is SQL Server
	SqlServerAddStmtDelimiter();

	if(_spl_first_non_declare == NULL)
		_spl_first_non_declare = set;

	return true;
}

// Various SET options such as SET ISOLATION ...
bool SqlParser::ParseSetOptions(Token *set)
{
	if(set == NULL)
		return false;

	if(_source == SQL_ORACLE && ParseOracleSetOptions(set) == true)
		return true;

	if(_source == SQL_SQL_SERVER && ParseSqlServerSetOptions(set) == true)
		return true;

	if(_source == SQL_DB2 && ParseDb2SetOptions(set) == true)
		return true;

	if(_source == SQL_INFORMIX && ParseInformixSetOptions(set) == true)
		return true;

	if(_source == SQL_MYSQL && ParseMysqlSetOptions(set) == true)
		return true;

	return false;
}

// SHOW statement in Oracle SQL*Plus
bool SqlParser::ParseShowStatement(Token *show)
{
	if(show == NULL)
		return false;

	STATS_DECL
    STMS_STATS(show);

	Token *option = GetNext();

	if(option == NULL)
		return false;

	// SHOW ERRORS in Oracle
	if(option->Compare("ERRORS", L"ERRORS", 6) == true || option->Compare("ERR", L"ERR", 3) == true)
	{
		if(_target != SQL_ORACLE)
			Token::Remove(show, option);
	}

	return true;
}

// DB2 SIGNAL statement; Sybase ASA SIGNAL exception_name
bool SqlParser::ParseSignalStatement(Token *signal)
{
	if(signal == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(signal);

	// SQLSTATE keyword in DB2
	Token *sqlstate = GetNextWordToken("SQLSTATE", L"SQLSTATE", 8);

	// Error code in single quotes in DB2; exception name in Sybase ASA
	Token *error = GetNextToken();
	Token *error_text = error;

	if(error == NULL)
		return false;

	// If error does not contain string literal (Sybase ASA), find error code
	if(error->type != TOKEN_STRING)
	{
		ListwmItem *i = Find(_spl_user_exceptions, error);

		// Exception/condition found
		if(i != NULL)
			error_text = (Token*)i->value2;
	}

	// Message text ('message') in DB2
	Token *open = GetNextCharToken('(', L'(');

	Token *message = NULL;
	Token *close = NULL;

	if(open != NULL)
	{
		message = GetNextToken();
		close = GetNextCharToken(')', L')');
	}

	Token *end = GetLastToken();

	// RAISERROR(text, 16, 1) in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(signal, "RAISERROR", L"RAISERROR", 9);

		// Replace blank with (
		if(signal->next != NULL && signal->next->IsBlank() == true)
			Token::Change(signal->next, "(", L"(", 1);
		else
			Append(signal, "(", L"(", 1);

		if(error != error_text)
			Token::Change(error, error_text);

		Append(end, ", 16, 1)", L", 16, 1)", 8);
	}
	else
	// RAISE_APPLICATION(-error, 'message') in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(signal, "RAISE_APPLICATION_ERROR", L"RAISE_APPLICATION_ERROR", 23);
		Token::Remove(sqlstate);
		Token::Remove(error);

		// Error number is negative and within -20000 and -20999
		TokenStr err("-20", L"-20", 3);

		// Skip ' and first 2 digits of SQLSTATE as well as closing '
		err.Append(error_text, 3, 3);
		err.Append(", ", L", ", 2);

		AppendNoFormat(open, &err); 
	}

	return true;
}

// SYSTEM statement in Informix
bool SqlParser::ParseSystemStatement(Token *system)
{
	if(system == NULL)
		return false;

    STATS_DECL
	PL_STMS_STATS(system);

	// Command to execute
	Token *cmd = GetNextToken();

	if(cmd == NULL)
		return false;

	// DBMS_SCHEDULER.CREATE_JOB in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(system, "DBMS_SCHEDULER.CREATE_JOB(", L"DBMS_SCHEDULER.CREATE_JOB(", 26);

		AppendNoFormat(system, "job_name => '", L"job_name => '", 13);
		AppendCopy(system, _spl_name);
		AppendNoFormat(system, "_job', job_type => 'EXECUTABLE', job_action =>", L"_job', job_type => 'EXECUTABLE', job_action =>", 46);
	
		AppendNoFormat(cmd, ", enabled => true)", L", enabled => true)", 18); 
	}

	return true;
}

// DB2 TERMINATE statement
bool SqlParser::ParseTerminateStatement(Token *terminate)
{
	if(terminate == NULL)
		return false;

	STATS_DECL
    STMS_STATS(terminate);

	// Comment for Greenplum
	if(_target == SQL_GREENPLUM)
	{
		Token *last = GetNextCharToken(';', L';');

		if(last == NULL)
			last = terminate;

		Comment(terminate, last); 
	}

	return true;
}

// TRUNCATE TABLE statement
bool SqlParser::ParseTruncateStatement(Token *truncate, int scope)
{
	if(truncate == NULL)
		return false;

	Token *table = GetNextWordToken("TABLE", L"TABLE", 5);

	if(table == NULL)
		return false;

	STATS_DECL
    STMS_STATS_V("TRUNCATE TABLE", truncate);

	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	// In PostgreSQL can be executed inside function body, Oracle requires EXECUTE IMMEDIATE
	if(_target == SQL_ORACLE && (scope == SQL_SCOPE_PROC || scope == SQL_SCOPE_FUNC))
	{
		Prepend(truncate, "EXECUTE IMMEDIATE '", L"EXECUTE IMMEDIATE '", 19);
		Append(name, "'", L"'", 1);
	}

	return true;
}

// UPDATE statement
bool SqlParser::ParseUpdateStatement(Token *update)
{
	if(update == NULL)
		return false;

	// UPDATE STATISTICS in Informix
	if(_source == SQL_INFORMIX && ParseInformixUpdateStatistics(update) == true)
		return true;

	STATS_DECL
	STATS_SET_DESC(SQL_STMT_UPDATE_DESC)

	_spl_last_fetch_cursor_name = NULL;
	
	// Parse SQL Server, Sybase ASE UPDATE statememt
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) && ParseSqlServerUpdateStatement(update))
	{
	    STMS_STATS(update);
		return true;
	}

	// Table name
	Token *name = GetNextIdentToken(SQL_IDENT_OBJECT);

	if(name == NULL)
		return false;

	// Table alias or SET
	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	Token *update_from = NULL;
    Token *update_from_end = NULL;

    Token *alias = NULL;
	Token *set = NULL;

    // UPDATE t1 FROM tab t1, tab2 t2, ...
    if(TOKEN_CMP(next, "FROM") == true)
    {
        update_from = next;

        int num = 0;

        while(true)
        {
            Token *open = TOKEN_GETNEXT('(');
            Token *tab = NULL;

            // (SELECT ...)
            if(open != NULL)
            {
                Token *sel = GetNextSelectStartKeyword();

			    // A subquery used to specify update table join
			    if(sel != NULL)
				ParseSelectStatement(sel, 0, SQL_SEL_UPDATE_FROM, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

                /*Token *close */ (void) TOKEN_GETNEXT(')');
            }
            // Table name
            else
	            tab = GetNextIdentToken(SQL_IDENT_OBJECT);

            // Alias 
            /*Token *alias */ (void) GetNextToken();

            Token *comma = TOKEN_GETNEXT(',');

            // Convert to MERGE USING in EsgynDB
            if(tab != NULL && _target == SQL_ESGYNDB)
            {
                if(num == 0)
                    TOKEN_CHANGE(comma, " USING");
                else
                {
                    Prepend(tab, "(SELECT * FROM ", L"(SELECT * FROM ", 15, update);
                    AppendNoFormat(tab, ")", L")", 1);
                }
            }

            if(comma == NULL)
                break;

            num++;
        }

        update_from_end = GetLastToken();

        // Get SET keyword should follow now
		set = TOKEN_GETNEXTW("SET");

        if(set != NULL && _target == SQL_ESGYNDB)
            TOKEN_CHANGE(set, "WHEN MATCHED THEN UPDATE SET");
    }
    else
	// Check for alias
	if(TOKEN_CMP(next, "SET") == false)
	{
		alias = next;

		// Get SET keyword after alias
		set = TOKEN_GETNEXTW("SET");
	}
	else
		set = next;

	// List of assignments; DB2 allows: (c1, c2, ...) = (v1, v2, ...), c3 = v3, ... 
	while(true)
	{
		ListWM cols;
		ListW values;

		// List of single column assignments or (c1, c2, ...) = (v1, v2, ...) can be specified
		Token *open = GetNextCharToken('(', L'(');

		while(true)
		{
			Token *col = GetNextIdentToken();

			if(col == NULL || open == NULL)
				break;

			Token *comma = GetNextCharToken(',', L',');

			cols.Add(col, comma);

			if(comma == NULL)
				break;
		}

		Token *close = GetNextCharToken(open, ')', L')');

		Token *equal = GetNextCharToken('=', L'=');

		if(equal == NULL)
			break;

		// Single value, (v1, v2, ...) or (SELECT c1, c2, ...) can be specified
		Token *open2 = GetNextCharToken(open, '(', L'(');

		Token *select = NULL;

		// Check for SELECT statement
		if(open2 != NULL)
		{
			select = GetNextSelectStartKeyword();

			// A subquery used to specify assignment values
			if(select != NULL)
				ParseSelectStatement(select, 0, SQL_SEL_UPDATE_SET, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		}
		
		// Parse set values
		while(true)
		{
			if(select != NULL)
				break;

			Token *exp = GetNextToken();

			if(exp == NULL)
				break;

			ParseExpression(exp);

			values.Add(exp);

			// Single assignment
			if(open2 == NULL)
				break;

			Token *comma = GetNextCharToken(',', L',');			

			if(comma == NULL)
				break;
		}

		Token *close2 = GetNextCharToken(open2, ')', L')');

		// Oracle does not support list assignment, use = for each column and value
		if(_target == SQL_ORACLE && values.GetCount() > 1)
		{
			ListwmItem *c = cols.GetFirst();
			ListwItem *v = values.GetFirst();

			while(c != NULL && v != NULL)
			{
				Token *col = (Token*)c->value;
				Token *col_comma = (Token*)c->value2;
				Token *val = (Token*)v->value;

				PrependCopy(val, col);
				PrependNoFormat(val, " = ", L" = ", 3);

				Token::Remove(col);
				Token::Remove(col_comma);

				c = c->next;
				v = v->next;
			}

			Token::Remove(open);
			Token::Remove(close);

			Token::Remove(equal);
			
			Token::Remove(open2);
			Token::Remove(close2);
		}

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

    Token *where_ = NULL;
    Token *where_end = NULL;

	// optional WHERE clause
	ParseWhereClause(SQL_STMT_UPDATE, &where_, &where_end, NULL);

    if(update_from != NULL)
    {
        // Convert to MERGE in EsgynDB
        if(_target == SQL_ESGYNDB)
        {
            TOKEN_CHANGE(update, "MERGE");
            TOKEN_CHANGE(update_from, "INTO");
            TOKEN_CHANGE(where_, "ON");

            AppendSpaceCopy(update_from_end, where_, where_end, false);

            Token::Remove(name);
            Token::Remove(where_, where_end);
        }
    }

	// Implement CONTINUE handler for NOT FOUND in Oracle
	if(_target == SQL_ORACLE)
		OracleContinueHandlerForUpdate(update);

    STMS_STATS(update);

	return true;
}

// SQL Server, MySQL USE statement
bool SqlParser::ParseUseStatement(Token *use)
{
	if(use == NULL)
		return false;

	STATS_DECL
    STMS_STATS(use);

	// Database name
	Token *name = GetNextIdentToken();

	if(name == NULL)
		return false;

	// ALTER SESSION SET CURRENT_SCHEMA = name in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(use, "ALTER SESSION SET CURRENT_SCHEMA =", L"ALTER SESSION SET CURRENT_SCHEMA =", 34);

	// Handle GO after the statement in SQL Server
	SqlServerGoDelimiter();

	return true;
}

// Procedure parameters
bool SqlParser::ParseProcedureParameters(Token *proc_name, int *count, Token **end)
{
	// SQL Server and Sybase have optional parentheses around parameters
	// Informix, Teradata require () if there are no parameters, Oracle does not allow empty ()
	Token *open = GetNextCharToken('(', L'(');

	// If there is no open ( check for beginning of definition
	if(open == NULL)
	{
		Token *next = GetNextToken();

		// AS in Oracle, SQL Server, Sybase ASE; IS in Oracle, BEGIN in DB2, MySQL, Teradata
		if(Token::Compare(next, "AS", L"AS", 2) == true || Token::Compare(next, "IS", L"IS", 2) == true ||
			Token::Compare(next, "BEGIN", L"BEGIN", 5) == true)
		{
			// If not parameters set, add () after procedure name for MySQL, MariaDB
			if(Target(SQL_MYSQL, SQL_MARIADB, SQL_POSTGRESQL))
				Append(proc_name, "()", L"()", 2);

			PushBack(next);
			return false;
		}

		PushBack(next);
	}

	Token *last = NULL;
	int num = 0;

	Token *first = NULL;

	// Informix uses () when there are no parameters
	while(true)
	{
		Token *in = NULL;
		Token *out = NULL;
		Token *inout = NULL;

		// In MySQL, DB2, Sybase ASA and Teradata IN, OUT or INOUT precedes the name
		if(Source(SQL_DB2, SQL_MYSQL, SQL_SYBASE_ASA, SQL_TERADATA) == true)
		{
			Token *param_type = GetNextToken();

			if(Token::Compare(param_type, "IN", L"IN", 2) == true)
			{
				in = param_type;

				// For SQL Server IN is not supported, assumed by default, and OUT goes after default
				if(_target == SQL_SQL_SERVER)
					Token::Remove(param_type);
			}
			else
			if(Token::Compare(param_type, "OUT", L"OUT", 3) == true)
				out = param_type;
			else
			if(Token::Compare(param_type, "INOUT", L"INOUT", 5) == true)
				inout = param_type;
			else
				PushBack(param_type);
		}

		Token *name = GetNextIdentToken(SQL_IDENT_PARAM);

		// Not an identifier
		if(name == NULL)
			break;

		// Add @ for parameter names for SQL Server and Sybase
		if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
			ConvertToTsqlVariable(name);

		_spl_parameters.Add(name);

		if(num == 0)
			first = name;

		// AS or IS keyword marks the end in SQL Server, Sybase when there are () around parameters
		if(name->Compare("AS", L"AS", 2) == true)
		{
			PushBack(name);
			break;
		}

		// In SQL Server AS keyword can follow after parameter name
		Token *as = GetNextWordToken("AS", L"AS", 2); 

		if(as != NULL && Target(SQL_MARIADB, SQL_MYSQL))
			Token::Remove(as);

		// In Oracle IN, OUT or IN OUT follows the name
		if(_source == SQL_ORACLE)
		{
			Token *param_type = GetNextToken();

			if(Token::Compare(param_type, "IN", L"IN", 2) == true)
			{
				in = param_type;

				// Check for IN OUT
				out = GetNextWordToken("OUT", L"OUT", 3);

				// For SQL Server IN is not supported, assumed by default, and OUT goes after default
				if(Target(SQL_SQL_SERVER) == true)
					Token::Remove(param_type);
			}
			else
			if(Token::Compare(param_type, "OUT", L"OUT", 3) == true)
				out = param_type;
			else
				PushBack(param_type);

			// NOCOPY can follow in Oracle
			/*Token *nocopy */ TOKEN_GETNEXTW("NOCOPY");
		}
		
		bool sys_refcursor = false;

		Token *data_type = GetNextToken();

		// Check and resolve Oracle %TYPE, Informix LIKE variable
		bool typed = ParseTypedVariable(name, data_type);

		// Get the parameter data type
		if(typed == false)
		{
			ParseDataType(data_type, SQL_SCOPE_PROC_PARAMS);
			ParseVarDataTypeAttribute();
		}

		// Propagate data type to variable
		name->data_type = data_type->data_type;

		Token *data_type_end = GetLastToken();

		// Oracle refcursor
		if(_source == SQL_ORACLE && Token::Compare(data_type, "SYS_REFCURSOR", L"SYS_REFCURSOR", 13) == true)
		{
			sys_refcursor = true;

			_spl_refcursor_params.Add(name);
			_spl_refcursor_params_num++;
		}

		// Netezza does not allow specifying parameter name and type (only IN assumed), only data type
		if(_target == SQL_NETEZZA)
		{
			Token::Remove(name, false);
			Token::Remove(in, false);
		}

		Token *next = GetNextToken();

		if(next == NULL)
			break;

		bool tsql_default = false;
		bool ora_default = false;

		// In SQL Server = denotes a default value
		if(next->Compare('=', L'=') == true)
			tsql_default = true;
		else
		// In Oracle DEFAULT keyword
		if(next->Compare("DEFAULT", L"DEFAULT", 7) == true)
			ora_default = true;
		else
		// Oracle := for default
		if(TOKEN_CMPC(next, ':'))
		{
			if(TOKEN_GETNEXT('=') != NULL)
				ora_default = true;
		}
		else
			PushBack(next);
		
		if(tsql_default == true || ora_default == true)
		{
			// Default is a constant
			Token *default_exp = GetNextToken();
			
			// SQL Server uses =
			if(ora_default == true && _target == SQL_SQL_SERVER)
				Token::Change(next, "=", L"=", 1);
			else
			// Oracle uses DEFAULT keyword (INT=0 without spaces is possible)
			if(tsql_default == true && _target == SQL_ORACLE)
				OracleChangeEqualToDefault(next);
			else
			// MySQL does not support default
			if(Target(SQL_MARIADB, SQL_MYSQL))
				Comment(next, default_exp);
		}

		// In SQL Server OUT or OUTPUT go after the default; Sybase ASE, Sybase ADS use OUTPUT
		if(Source(SQL_SQL_SERVER, SQL_SYBASE, SQL_SYBASE_ADS))
		{
			Token *param_type = GetNextToken();

			if(Token::Compare(param_type, "OUT", L"OUT", 3) == true ||
				Token::Compare(param_type, "OUTPUT", L"OUTPUT", 6) == true)
			{
				out = param_type;

				// In Oracle OUT goes after name
				if(Target(SQL_ORACLE))
					Append(name, " OUT", L" OUT", 4, out);
				else
				// In MariaDB OUT goes before name
				if(Target(SQL_MYSQL, SQL_MARIADB))
					Prepend(name, "OUT ", L"OUT ", 4, out);

				// OUT moved for other databases
				if(_target != SQL_SQL_SERVER)
					Token::Remove(param_type);
			}
			else
				PushBack(param_type);
		}

		if(_target_app == APP_JAVA)
		{
			if(inout != NULL || out != NULL)
				_java->SetOutDataType(data_type);

			// Java syntax requires data type before variable name
			PrependSpaceCopy(name, data_type);
			Token::Remove(data_type, data_type_end);

			Token::Remove(in);
			Token::Remove(inout);
			Token::Remove(out);
		}
		else
		// Oracle IN, OUT 
		if(_source == SQL_ORACLE && (in != NULL || out != NULL) && sys_refcursor == false)
		{			
			// In Oracle OUT goes before default, in SQL Server after
			if(_target == SQL_SQL_SERVER)
			{
				if(out != NULL)
				{
					Append(GetLastToken(), " OUT", L" OUT", 4, out);
					Token::Remove(out);
				}
			}
			else
			// In MySQL IN, OUT and IN OUT go before name
			if(Target(SQL_MARIADB, SQL_MYSQL))
			{
				// IN OUT in Oracle
				if(in != NULL && out != NULL)
				{
					Prepend(name, "INOUT ", L"INOUT ", 6, in);
					Token::Remove(in);
					Token::Remove(out);
				}
				else
				if(in != NULL)
				{
					Prepend(name, "IN ", L"IN ", 3, in);
					Token::Remove(in);
				}
				else
				if(out != NULL)
				{
					Prepend(name, "OUT ", L"OUT ", 4, out);
					Token::Remove(out);
				}
			}
		}
		else
		// DB2, MySQL, Teradata IN, OUT or INOUT parameter 
		if(Source(SQL_DB2, SQL_MYSQL, SQL_TERADATA) == true && (in != NULL || out != NULL || inout != NULL))
		{
			// In MySQL OUT/INOUT go before name, in SQL Server after default
			if(_target == SQL_SQL_SERVER)
			{
				// SQL Server does not support INOUT, always use OUT
				if(out != NULL || inout != NULL)
					Append(GetLastToken(), " OUT", L" OUT", 4, Nvl(out, inout));
				
				Token::Remove(out);
				Token::Remove(inout);
			}
			else
			// In Oracle IN, OUT and IN OUT go after name
			if(_target == SQL_ORACLE)
			{
				if(in != NULL)
				{
					Append(name, " IN", L" IN", 3, in);
					Token::Remove(in);
				}
				else
				if(out != NULL)
				{
					Append(name, " OUT", L" OUT", 4, out);
					Token::Remove(out);
				}
				else
				// INOUT also changed to IN OUT
				if(inout != NULL)
				{
					Append(name, " IN OUT", L" IN OUT", 7, inout);
					Token::Remove(inout);
				}
			}
		}

		// Handle Oracle SYS_REFCURSOR parameter
		if(sys_refcursor == true)
		{
			// User REFCURSOR in PostgreSQL
			if(Target(SQL_POSTGRESQL))
			{
				// REFCURSOR is OUT parameter and contains cursor name as string literal
				// Actual REFCURSOR value is returned using RETURN
				Token::Remove(out);

				TOKEN_CHANGE(data_type, "REFCURSOR");
			}
			else
			// Remove for other databases
			if(_target != SQL_ORACLE)
			{
				Token::Remove(name, data_type);

				Token *prev = GetPrevToken(name);

				// Remove previous comma
				if(Token::Compare(prev, ',', L',') == true)
					Token::Remove(prev);
			}
		}

		last = GetLastToken();
		num++;

		// Exit if not a comma
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	// SQL Server and Sybase have optional parentheses around parameters
	Token *close = GetNextCharToken(')', L')');

	// If parameters are not enclosed with (), add them for all dataabases except SQL Server and Sybase
	if(open == NULL && Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
	{
		// If parameters exist, add ( before first and ) after last parameter
		if(num > 0)
		{
			Append(proc_name, " (", L" (", 2);

			if(close == NULL)
				Append(last, ")", L")", 1);
		}
		else
		// If not parameters set, add () after procedure name for MySQL
		if(Target(SQL_MYSQL) == true)
			Append(proc_name, "()", L"()", 2);
	}
	else
	// Empty list of parameters ()
	if(open != NULL && close != NULL && num == 0)
	{
		// Oracle does not allow empty ()
		if(_target == SQL_ORACLE)
			Token::Remove(open, close);
	}

	_spl_param_close = close;

	if(count != NULL)
		*count = num;

	if(end != NULL)
		*end = last;

	return true;
}

// Procedure block body
bool SqlParser::ParseProcedureBody(Token *create, Token *procedure, Token *name, Token *as, int *result_sets)
{
	Token *body_start = GetLastToken();

	// In DB2, Teradata, MySQL block can start with a label 
	ParseLabelDeclaration(NULL, true);

	// In SQL Server and Sybase procedural block can start without BEGIN; In Informix BEGIN not used
	Token *begin = GetNextWordToken("BEGIN", L"BEGIN", 5);

	// Possible BEGIN transaction at the beginning
	if(!Source(SQL_ORACLE) && begin != NULL)
	{
		if(ParseBeginStatement(begin))
			begin = NULL;
	}

	_spl_outer_as = as;
    _spl_outer_begin = begin;

	// In Oracle, variable declaration goes before BEGIN
	if(_source == SQL_ORACLE && begin == NULL)
	{
		bool declare = ParseOracleVariableDeclarationBlock(as);

		// Now get BEGIN
		begin = GetNextWordToken("BEGIN", L"BEGIN", 5);

		// In SQL Server, MySQL declaration is after BEGIN, append BEGIN after AS, and remove BEGIN
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true && declare == true && begin != NULL)
		{
			// AS will be removed for MySQL
			Append(as, "\nBEGIN", L"\nBEGIN", 6);

			// Change to a space to prevent line deletion
			Token::Change(begin, " ", L" ", 1);       
		}
	}
	
	// In SQL Server, often SET NOCOUNT ON is specified after AS before BEGIN
	if(_source == SQL_SQL_SERVER && _target != SQL_SQL_SERVER && begin == NULL)
	{
		Token *set = GetNext("SET", L"SET", 3);

		// Parse SET options and try to get BEGIN
		if(set != NULL && ParseSqlServerSetOptions(set) == true)
			begin = GetNext("BEGIN", L"BEGIN", 5);
	}
	else
	// Add SET NOCOUNT ON;
	if(_source != SQL_SQL_SERVER && _target == SQL_SQL_SERVER)
	{
		Append(begin, "\nSET NOCOUNT ON;", L"\nSET NOCOUNT ON;", 16);
	}

	// DB2, MySQL, Informix do not use AS
	if(as == NULL)
	{
		// Prepend AS before BEGIN; for Oracle see OracleMoveBeginAfterDeclare()
		if(_target == SQL_SQL_SERVER)
		{
			if(begin != NULL)
				Prepend(begin, "AS\n", L"AS\n", 3);
			else
				Append(body_start, " AS ", L" AS ", 4, create);
		}
		else
		// AS required for PostgreSQL
		if(_target == SQL_POSTGRESQL)
		{
			if(begin != NULL)
				Prepend(begin, "AS $$\n", L"AS $$\n", 6);
			else
				Append(body_start, " AS $$\n", L" AS $$\n", 7, create);
		}
	}

	if(as != NULL)
	{
		// PostgreSQL requires $$ after AS
		if(_source != SQL_POSTGRESQL && _target == SQL_POSTGRESQL)
			AppendFirst(as, " $$", L" $$", 3);
		else
		// Add header for Netezza
		if(_source != SQL_NETEZZA && _target == SQL_NETEZZA)
			Token::Change(as, "RETURNS INT EXECUTE AS CALLER\nLANGUAGE NZPLSQL AS\nBEGIN_PROC", L"RETURNS INT EXECUTE AS CALLER\nLANGUAGE NZPLSQL AS\nBEGIN_PROC", 60);
	}

	// MySQL does not allow AS
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		// If AS is followed by BEGIN, remove AS, otherwise change AS to BEGIN
		if(begin != NULL)
			Token::Remove(as);
		else
			Token::Change(as, "BEGIN", L"BEGIN", 5);
	}
	else
	// Add BEGIN if it not set
	// If source SQL Server then standalone DECLARE statements can go in block, and
	// if target is Oracle BEGIN must be after all declarations, not after AS
	if(begin == NULL && Target(SQL_ORACLE, SQL_SQL_SERVER, SQL_SYBASE) == false)
		Append(as, "\nBEGIN", L"\nBEGIN", 6);

	if(_target_app == APP_JAVA)
		Token::Remove(begin);

	bool frontier = (begin != NULL) ? true : false;

	// If BEGIN END is not specified in SQL Server and Sybase ASE, body is terminated with GO, it acts as the frontier
	// Informix does not use BEGIN keyword, but body always terminated with END PROCEDURE, set frontier
	if(Source(SQL_SQL_SERVER, SQL_SYBASE, SQL_INFORMIX) == true)
		frontier = true;

    _spl_begin_blocks.Add(GetLastToken(begin));

	ParseBlock(SQL_BLOCK_PROC, frontier, SQL_SCOPE_PROC, result_sets);

    _spl_begin_blocks.DeleteLast();

	Token *end = GetNextWordToken("END", L"END", 3);

	// Informix uses END PROCEDURE
	if(_source == SQL_INFORMIX && end != NULL)
	{
		Token *procedure = GetNextWordToken("PROCEDURE", L"PROCEDURE", 9);

		// Remove for other databases
		if(procedure != NULL && _target != SQL_INFORMIX)
			Token::Remove(procedure);
	}

	// In Oracle ; must follow END name, in SQL Server, MySQL ; is optional after END
	Token *semi = GetNextCharToken(';', L';');

	// In Oracle procedure name, in DB2 label name can be specified after END
	if(semi == NULL)
	{
		if(ParseSplEndName(name, end) == true)
			semi = GetNextCharToken(';', L';');

		// For Oracle append ; after END
		if(end != NULL && semi == NULL)
			AppendNoFormat(end, ";", L";", 1);
	}

	if(_target_app == APP_JAVA)
	{
		TOKEN_CHANGE_NOFMT(end, "}");
		Token::Remove(semi);
	}

	// For Informix replace RETURNING values with OUT parameters or table function
	if(_source == SQL_INFORMIX)
		InformixConvertReturning(create, procedure);

	// Replace GO
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
	{
		// In SQL Server GO can be used to terminate procedure block without using BEGIN END
		if(begin == NULL && Token::Compare(end, "GO", L"GO", 2) == true)
		{
			if(_target == SQL_ORACLE)
				Token::Change(end, "END;\n/", L"END;\n/", 6);
			else
			if(Target(SQL_MARIADB, SQL_MYSQL))
				Token::Change(end, "END;\n//", L"END;\n//", 7);
		}
		else
		// No BEGIN-END
		if(begin == NULL && end == NULL)
		{
			Token *go = GetNextWordToken("GO", L"GO", 2);

			// GO exists
			if(go != NULL)
			{
				if(Target(SQL_MARIADB, SQL_MYSQL))
					Token::Change(go, "\nEND;", L"\nEND;", 5);
			}
			else
			{
				Token *last = GetLastToken();

				if(_target == SQL_ORACLE)
					Append(last, "\nEND;", L"\nEND;", 5, create);
				else
				if(Target(SQL_MARIADB, SQL_MYSQL))
					Append(last, "\nEND;", L"\nEND;", 5, create);
			}
		}
		else
		if(end != NULL)
		{
			Token *go = GetNextWordToken("GO", L"GO", 2);

			// GO exists
			if(go != NULL)
			{
				if(_target == SQL_ORACLE)
					Token::Change(go, "/", L"/", 1);
				else
				if(Target(SQL_MARIADB, SQL_MYSQL))
					Token::Remove(go);
			}
			else
			{
				if(_target == SQL_ORACLE)
					Append(end, "\n/", L"\n/", 2);
			}
		}
	}
	else
	// Informix does not use any special delimiter after END PROCEDURE; Teradata also after END; 
	if(Source(SQL_INFORMIX, SQL_TERADATA) == true)
	{
		if(_target == SQL_ORACLE)
			Append(semi, "\n/", L"\n/", 2);
		else
		// In SQL Server procedure block can be within AS and GO, no BEGIN END required
		if(_target == SQL_SQL_SERVER)
		{
			// But BEGIN END required for function in SQL Server
			if(_spl_proc_to_func == true)
				Append(body_start, "\nBEGIN", L"\nBEGIN", 6, create);
			else
				Token::Remove(end);

			Token::Remove(semi);

			Append(semi, "\nGO", L"\nGO", 3, end);
		}
	}
	
	// User-defined delimiter can be specified for MySQL, often // @ (also used for DB2)
	if(Source(SQL_DB2, SQL_MYSQL) == true)
		ParseMySQLDelimiter(create);

	// For target Oracle and PostgreSQL, if there declarations in the body move BEGIN after last DECLARE
	if(!Source(SQL_ORACLE, SQL_POSTGRESQL) && Target(SQL_ORACLE, SQL_POSTGRESQL) && _target_app != APP_JAVA)
		OracleMoveBeginAfterDeclare(create, as, begin, body_start);

	// Transform DB2, MySQL, Teradata EXIT HANDLERs, Informix ON EXCEPTION to Oracle and PostgreSQL EXCEPTION
	if(Source(SQL_DB2, SQL_MYSQL, SQL_INFORMIX, SQL_TERADATA) && Target(SQL_ORACLE, SQL_POSTGRESQL))
		OracleExitHandlersToException(end);

	// Add label for target MySQL, MariaDB if there is a RETURN inside the procedure (converted to LEAVE lbl)
	if(_spl_return_num > 0 && Target(SQL_MYSQL, SQL_MARIADB))
		PREPEND_NOFMT(Nvl(as, begin), "sp_lbl:\n"); 

	// For PostgreSQL add RETURN statements for REFCURSORs
	if(_spl_refcursor_params_num > 0)
		AddReturnRefcursors(end);

	// Add variable declarations generated in the procedural block
	AddGeneratedVariables();

	return true;
}

// In Oracle procedure name, in DB2 label name can be specified after outer END
bool SqlParser::ParseSplEndName(Token *name, Token * /*end*/)
{
	if(name == NULL)
		return false;

	bool exists = false;

	Token *next = GetNextToken();

	if(next == NULL)
		return false;

	// Compare with procedure name, here it can be specified without schema name, or one can be quoted
	// another unquoted
	if(CompareIdentifiersExistingParts(name, next) == true)
	{
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL, SQL_POSTGRESQL) || _target_app == APP_JAVA)
			Token::Remove(next);

		exists = true;
	}
	else
	// Compare with label name, label is without : here
	if(_spl_outer_label != NULL && Token::Compare(_spl_outer_label, next, _spl_outer_label->len-1) == true)
	{
		// Remove for other databases
		if(_target != SQL_DB2)
			Token::Remove(next);

		exists = true;
	}
	else
		PushBack(next);

	return exists;
}

// Function RETURNS clause
bool SqlParser::ParseFunctionReturns(Token *function)
{
	Token *returns = GetNextToken();

	if(returns == NULL)
		return false;

	// RETURN in Oracle
	if(Token::Compare(returns, "RETURN", L"RETURN", 6) == true)
	{
		// Change to RETURNS in SQL Server, MySQL
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
			Token::Change(returns, "RETURNS", L"RETURNS", 7);

		// Return data type
		Token *data_type = GetNextToken();

		ParseDataType(data_type, SQL_SCOPE_FUNC_PARAMS);
	}
	else
	// RETURNS in SQL Server, DB2, PostgreSQL, MySQL, Informix, Sybase ADS
	if(_source != SQL_INFORMIX && Token::Compare(returns, "RETURNS", L"RETURNS", 7) == true)
	{
		// Change to RETURN in Oracle
		if(_target == SQL_ORACLE)
			Token::Change(returns, "RETURN", L"RETURN", 6);

		// Return data type
		Token *data_type = GetNextToken();
		Token *data_type_end = data_type; 

		// Check for 'void' data type in PostgreSQL meaning stored procedure
		if(Token::Compare(data_type, "VOID", L"VOID", 4) == true)
		{
			// If not PostgreSQL convert to procedure and remove return clause
			if(_target != SQL_POSTGRESQL)
			{
				Token::Change(function, "PROCEDURE", L"PROCEDURE", 9);
				Token::Remove(returns, data_type);
			}
		}
		else
		{
			ParseDataType(data_type, SQL_SCOPE_FUNC_PARAMS);
			data_type_end = GetLastToken();

			if(data_type->data_subtype == TOKEN_DT2_INT || data_type->data_subtype == TOKEN_DT2_BOOL)
				_spl_return_int = true;
		}

		// Function is converted to procedure
		if(_spl_func_to_proc)
		{
			// SQL Server stored procedure allows returning integer value
			if(Target(SQL_SQL_SERVER))
			{
				if(!_spl_return_int)
				{
					PREPEND_NOFMT(_spl_param_close, ", @retval ");
					PrependCopy(_spl_param_close, data_type, data_type_end);			
					PREPEND_FMT(_spl_param_close, " output", data_type);
				}

				Token::Remove(returns, data_type_end);
			}
		}
	}
	else
	// RETURNS or RETURNING in Informix
	if(Token::Compare(returns, "RETURNING", L"RETURNING", 9) == true)
	{
		_spl_returns = returns;

		ParseInformixReturns(returns);	
	}

	return true;
}

// Function options DETERMINISTIC, NO SQL etc.
bool SqlParser::ParseFunctionOptions()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// DETERMINISTIC in Oracle, DB2, MySQL
		if(next->Compare("DETERMINISTIC", L"DETERMINISTIC", 13) == true)
		{
			// Not supported in SQL Server
			if(_target == SQL_SQL_SERVER)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOT DETERMINISTIC in DB2, MySQL
		if(next->Compare("NOT", L"NOT", 3) == true)
		{
			Token *deterministic = GetNextWordToken("DETERMINISTIC", L"DETERMINISTIC", 13);

			// Not supported in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && deterministic != NULL)
				Token::Remove(next, deterministic);

			exists = true;
			continue;
		}
		else
		// LANGUAGE SQL in DB2, MySQL
		if(next->Compare("LANGUAGE", L"LANGUAGE", 8) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);

			// Not supported in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && sql != NULL)
				Token::Remove(next, sql);

			exists = true;
			continue;
		}
		else
		// CONTAINS SQL in DB2, MySQL
		if(next->Compare("CONTAINS", L"CONTAINS", 8) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);

			// Not supported in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && sql != NULL)
				Token::Remove(next, sql);

			exists = true;
			continue;
		}
		else
		// NO SQL in MySQL; NO EXTERNAL ACTION in DB2
		if(next->Compare("NO", L"NO", 2) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);
			Token *external = NULL;

			if(sql == NULL)
				external = GetNextWordToken("EXTERNAL", L"EXTERNAL", 8);

			// NO SQL
			if(sql != NULL)
			{
				if(_target == SQL_ORACLE)
					Token::Remove(next, sql);
			}
			else
			// NO EXTERNAL ACTION
			if(external != NULL)
			{
				Token *action = GetNextWordToken("ACTION", L"ACTION", 6);

				// Not supported in SQL Server and Oracle
				if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && action != NULL)
					Token::Remove(next, action);
			}

			exists = true;
			continue;
		}
		else
		// EXTERNAL ACTION in DB2
		if(next->Compare("EXTERNAL", L"EXTERNAL", 8) == true)
		{
			Token *action = GetNextWordToken("ACTION", L"ACTION", 6);

			// Not supported in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && action != NULL)
				Token::Remove(next, action);

			exists = true;
			continue;
		}
		else
		// READS SQL DATA in DB2, MySQL
		if(next->Compare("READS", L"READS", 5) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);
			Token *data = (sql != NULL) ? GetNextWordToken("DATA", L"DATA", 4) : NULL;

			// Not supported in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && sql != NULL)
				Token::Remove(next, data);

			exists = true;
			continue;
		}
		else
		// MODIFIES SQL DATA in DB2, MySQL
		if(next->Compare("MODIFIES", L"MODIFIES", 8) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);
			Token *data = (sql != NULL) ? GetNextWordToken("DATA", L"DATA", 4) : NULL;

			// Not supported in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && sql != NULL)
				Token::Remove(next, data);

			exists = true;
			continue;
		}
		else
		// COMMENT 'text' in MySQL
		if(next->Compare("COMMENT", L"COMMENT", 7) == true)
		{
			Token *text = GetNextToken();

			if(_target == SQL_ORACLE && text != NULL)
				Token::Remove(next, text);

			exists = true;
			continue;
		}
		else
		// SPECIFIC name in DB2
		if(next->Compare("SPECIFIC", L"SPECIFIC", 8) == true)
		{
			Token *name = GetNextToken();

			if(_target != SQL_DB2 && name != NULL)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// CALLED ON NULL INPUT in DB2
		if(next->Compare("CALLED", L"CALLED", 6) == true)
		{
			Token *on = GetNextWordToken("ON", L"ON", 2);
			Token *null = NULL;
			Token *input = NULL;			
			
			if(on != NULL)
				null = GetNextWordToken("NULL", L"NULL", 4);

			if(null != NULL)
				input = GetNextWordToken("INPUT", L"INPUT", 5);

			// Not supported in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && input != NULL)
				Token::Remove(next, input);

			exists = true;
			continue;
		}
		else
		// RETURNS NULL ON NULL INPUT in DB2
		if(next->Compare("RETURNS", L"RETURNS", 7) == true)
		{
			Token *null = GetNextWordToken("NULL", L"NULL", 4);

			// DB2 allows mixing options and RETURNS datatype
			if(null != NULL)
			{
				Token *on = GetNextWordToken("ON", L"ON", 2);
				Token *null2 = GetNextWordToken(on, "NULL", L"NULL", 4);
				Token *input = GetNextWordToken(null2, "INPUT", L"INPUT", 5);;			
			
				// Not supported in SQL Server and Oracle
				if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && input != NULL)
					Token::Remove(next, input);

				exists = true;
				continue;
			}
		}
		else
		// INHERIT SPECIAL REGISTERS in DB2
		if(next->Compare("INHERIT", L"INHERIT", 7) == true)
		{
			Token *special = GetNextWordToken("SPECIAL", L"SPECIAL", 7);
			Token *registers = NULL;
			
			if(special != NULL)
				registers = GetNextWordToken("REGISTERS", L"REGISTERS", 9);

			// Not supported in SQL Server and Oracle
			if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true && registers != NULL)
				Token::Remove(next, registers);

			exists = true;
			continue;
		}
		else
		// SQL SECURITY { DEFINER | INVOKER } in MySQL
		if(next->Compare("SQL", L"SQL", 3) == true)
		{
			Token *security = GetNextWordToken("SECURITY", L"SECURITY", 8);
			Token *type = (security != NULL) ? GetNextToken() : NULL;

			// AUTHID DEFINER | CURRENT_USER in Oracle
			if(_target == SQL_ORACLE && security != NULL)
			{
				Token:: Change(next, "AUTHID", L"AUTHID", 6);
				Token::Remove(security);

				if(Token::Compare(type, "INVOKER", L"INVOKER", 7) == true)
					Token::Change(type, "CURRENT_USER", L"CURRENT_USER", 12);
			}

			exists = true;
			continue;
		}
		
		// Not a function option
		PushBack(next);
		break;
	}

	return exists;
}

// Procedure options DETERMINISTIC, NO SQL etc.
bool SqlParser::ParseProcedureOptions(Token *create)
{
	bool exists = false;

	Token *options_start = GetLastToken();

	while(true)
	{
		STATS_DTL_DECL
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// DETERMINISTIC in DB2, MySQL
		if(next->Compare("DETERMINISTIC", L"DETERMINISTIC", 13) == true)
		{
			if(_target == SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// NOT DETERMINISTIC in DB2, MySQL
		if(next->Compare("NOT", L"NOT", 3) == true)
		{
			Token *deterministic = GetNextWordToken("DETERMINISTIC", L"DETERMINISTIC", 13);

			if(_target == SQL_ORACLE && deterministic != NULL)
				Token::Remove(next, deterministic);

			exists = true;
			continue;
		}
		else
		// LANGUAGE SQL in DB2, MySQL; LANGUAGE C in DB2, Oracle; Java in Oracle
		if(next->Compare("LANGUAGE", L"LANGUAGE", 8) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);
			Token *c = NULL;
			Token *java = NULL;

			if(sql == NULL)
			{
				c = TOKEN_GETNEXTW("C");

				if(c == NULL)
					java = TOKEN_GETNEXTW("JAVA");

				if(c != NULL)
					_spl_external = true;

				if(java != NULL)
				{
					STATS_DTL_DESC(SQL_STMT_CRPROC_JAVA_DESC)
					STATS_DTL_CONV_ERROR(Target(SQL_MARIADB_ORA), STATS_CONV_HIGH, "", "")
					CREATE_PROC_STMS_STATS_V("LANGUAGE JAVA", next)

					_spl_external = true;
				}
			}

			// No need to specify LANGUAGE SQL in Oracle, not supported
			if(sql != NULL && _target == SQL_ORACLE)
				Token::Remove(next, sql);			

			exists = true;
			continue;
		}
		else
		// CONTAINS SQL in MySQL
		if(next->Compare("CONTAINS", L"CONTAINS", 8) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);

			if(_target == SQL_ORACLE && sql != NULL)
				Token::Remove(next, sql);

			exists = true;
			continue;
		}
		else
		// NO SQL in MySQL; NO EXTERNAL ACTION in DB2
		if(next->Compare("NO", L"NO", 2) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);
			Token *external = NULL;

			if(sql == NULL)
				external = GetNextWordToken("EXTERNAL", L"EXTERNAL", 8);

			// NO SQL
			if(sql != NULL)
			{
				if(_target == SQL_ORACLE)
					Token::Remove(next, sql);
			}
			else
			// NO EXTERNAL ACTION
			if(external != NULL)
			{
				Token *action = GetNextWordToken("ACTION", L"ACTION", 6);

				if(_target == SQL_ORACLE)
					Token::Remove(next, action);
			}

			exists = true;
			continue;
		}
		else
		// EXTERNAL ACTION in DB2; EXTERNAL NAME in DB2 (C/Java etc. procedures)
		if(next->Compare("EXTERNAL", L"EXTERNAL", 8) == true)
		{
			Token *action = GetNextWordToken("ACTION", L"ACTION", 6);
			Token *name = NULL;
			Token *ext_name = NULL;

			if(action == NULL)
			{
				name = GetNextWordToken("NAME", L"NAME", 4);
				ext_name = GetNextToken(name);
			}

			if(_target == SQL_ORACLE && action != NULL)
				Token::Remove(next, action);

			CREATE_PROC_STMS_STATS_V("External", next)

			exists = true;
			continue;
		}
		else
		// READS SQL DATA in MySQL
		if(next->Compare("READS", L"READS", 5) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);
			Token *data = (sql != NULL) ? GetNextWordToken("DATA", L"DATA", 4) : NULL;

			if(_target == SQL_ORACLE && sql != NULL)
				Token::Remove(next, data);

			exists = true;
			continue;
		}
		else
		// MODIFIES SQL DATA in MySQL
		if(next->Compare("MODIFIES", L"MODIFIES", 8) == true)
		{
			Token *sql = GetNextWordToken("SQL", L"SQL", 3);
			Token *data = (sql != NULL) ? GetNextWordToken("DATA", L"DATA", 4) : NULL;

			if(_target == SQL_ORACLE && sql != NULL)
				Token::Remove(next, data);

			exists = true;
			continue;
		}
		else
		// COMMENT 'text' in MySQL
		if(next->Compare("COMMENT", L"COMMENT", 7) == true)
		{
			Token *text = GetNextToken();

			if(_target == SQL_ORACLE && text != NULL)
				Token::Remove(next, text);

			exists = true;
			continue;
		}
		else
		// SPECIFIC name in DB2
		if(next->Compare("SPECIFIC", L"SPECIFIC", 8) == true)
		{
			Token *name = GetNextToken();

			if(_target == SQL_ORACLE && name != NULL)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// [DYNAMIC] RESULT SETS num in DB2, Teradata
		if(Source(SQL_DB2, SQL_TERADATA) == true && 
			(next->Compare("DYNAMIC", L"DYNAMIC", 7) == true || next->Compare("RESULT", L"RESULT", 6) == true))
		{
			Token *result = NULL;
			Token *sets = NULL;
			Token *num = NULL;
			
			if(next->Compare("RESULT", L"RESULT", 6) == true)
				result = next;
			else
				result = GetNextWordToken("RESULT", L"RESULT", 6);
			
			if(result != NULL)
				sets = GetNextWordToken("SETS", L"SETS", 4);

			if(sets != NULL)
				num = GetNextToken();

			if(!Target(SQL_DB2, SQL_TERADATA) && num != NULL)
				Token::Remove(next, num);

			exists = true;
			continue;
		}
		else
		// RESULT (col datatype, ...) in Sybase ASA
		if(_source == SQL_SYBASE_ASA && next->Compare("RESULT", L"RESULT", 6) == true)
		{
			Token *open = GetNextCharToken('(', L'(');

			// Variable comma separated list of columns and data types
			while(open != NULL)
			{
				Token *col = GetNextToken();

				if(col == NULL)
					break;

				Token *datatype = GetNextToken();

				ParseDataType(datatype);
				
				Token *comma = GetNextCharToken(',', L',');

				if(comma == NULL)
					break;
			}
				
			Token *close = GetNextCharToken(')', L')');
			
			// Remove for other databases
			if(_target != SQL_SYBASE_ASA)
				Token::Remove(next, close);

			exists = true;
			continue;
		}
		else
		// CALLED ON NULL INPUT in DB2
		if(next->Compare("CALLED", L"CALLED", 6) == true)
		{
			Token *on = GetNextWordToken("ON", L"ON", 2);
			Token *null = NULL;
			Token *input = NULL;			
			
			if(on != NULL)
				null = GetNextWordToken("NULL", L"NULL", 4);

			if(null != NULL)
				input = GetNextWordToken("INPUT", L"INPUT", 5);

			if(_target == SQL_ORACLE && input != NULL)
				Token::Remove(next, input);

			exists = true;
			continue;
		}
		else
		// INHERIT SPECIAL REGISTERS in DB2
		if(next->Compare("INHERIT", L"INHERIT", 7) == true)
		{
			Token *special = GetNextWordToken("SPECIAL", L"SPECIAL", 7);
			Token *registers = NULL;
			
			if(special != NULL)
				registers = GetNextWordToken("REGISTERS", L"REGISTERS", 9);

			if(_target == SQL_ORACLE && registers != NULL)
				Token::Remove(next, registers);

			exists = true;
			continue;
		}
		else
		// PARAMETER STYLE in DB2 (external procedure)
		if(next->Compare("PARAMETER", L"PARAMETER", 9) == true)
		{
			Token *style = GetNextWordToken("STYLE", L"STYLE", 5);
						
			// Style DB2GENERAL, DB2SQL, GENERAL, GENERAL WITH NULLS, JAVA, SQL
			Token *style_value = GetNextToken(style);
			
			if(_target == SQL_ORACLE && style_value != NULL)
				Token::Remove(next, style_value);

			exists = true;
			continue;
		}
		else
		// FENCED in DB2 (external procedure)
		if(next->Compare("FENCED", L"FENCED", 6) == true)
		{
			// Optional NOT THREADSAFE, THREADSAFE
			/*Token *not_ */ (void) GetNextWordToken("NOT", L"NOT", 3);
			
			Token *fenced_type = GetNextWordToken("THREADSAFE", L"THREADSAFE", 10);
			
			if(_target != SQL_DB2)
			{
				if(fenced_type != NULL)
					Token::Remove(next, fenced_type);
				else
					Token::Remove(next);
			}

			exists = true;
			continue;
		}
		else
		// DBINFO in DB2 (external procedure)
		if(next->Compare("DBINFO", L"DBINFO", 6) == true)
		{
			if(_target == SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// COLLID name in DB2 (package collection)
		if(next->Compare("COLLID", L"COLLID", 6) == true)
		{
			Token *name = GetNextToken();

			if(_target != SQL_DB2)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// WLM ENVIRONMENT name in DB2 (workload manager)
		if(next->Compare("WLM", L"WLM", 3) == true)
		{
			Token *environment = GetNextWordToken("ENVIRONMENT", L"ENVIRONMENT", 11);
			Token *name = GetNextToken(environment);

			if(_target != SQL_DB2)
				Token::Remove(next, name);

			exists = true;
			continue;
		}
		else
		// RUN OPTIONS 'options' in DB2
		if(next->Compare("RUN", L"RUN", 3) == true)
		{
			Token *options = GetNextWordToken("OPTIONS", L"OPTIONS", 7);

			// Options are in string literal
			Token *value = GetNextToken(options);

			if(_target != SQL_DB2)
				Token::Remove(next, value);

			exists = true;
			continue;
		}
		else
		// SQL SECURITY { DEFINER | INVOKER } in MySQL
		if(next->Compare("SQL", L"SQL", 3) == true)
		{
			Token *security = GetNextWordToken("SECURITY", L"SECURITY", 8);
			Token *type = (security != NULL) ? GetNextToken() : NULL;

			// AUTHID DEFINER | CURRENT_USER in Oracle
			if(_target == SQL_ORACLE && security != NULL)
			{
				Token:: Change(next, "AUTHID", L"AUTHID", 6);
				Token::Remove(security);

				if(Token::Compare(type, "INVOKER", L"INVOKER", 7) == true)
					Token::Change(type, "CURRENT_USER", L"CURRENT_USER", 12);
			}

			exists = true;
			continue;
		}
		else
		// WITH EXECUTE AS { OWNER | name } or WITH RECOMPILE in SQL Server
		if(next->Compare("WITH", L"WITH", 4) == true)
		{
			Token *execute = GetNext("EXECUTE", L"EXECUTE", 7);
			Token *as = (execute != NULL) ? GetNext("AS", L"AS", 2) : NULL;
			Token *name = (as != NULL) ? GetNext() : NULL;
			Token *last = name;

			if(last == NULL)
				last = GetNext("RECOMPILE", L"RECOMPILE", 9);

			if(last != NULL)
			{
				if(_target != SQL_SQL_SERVER)
					Token::Remove(next, last);

				exists = true;
				continue;
			}
		}
		else
		// RETURNS or RETURNING in Informix
		if(next->Compare("RETURNS", L"RETURNS", 7) == true || next->Compare("RETURNING", L"RETURNING", 9) == true)
		{
			_spl_returns = next;

			ParseInformixReturns(next);	

			exists = true;
			continue;
		}
		
		// Not a function option
		PushBack(next);
		break;
	}

	// PostgreSQL requires RETURNS VOID (set if there is no any other RETURN keyword as in Informix i.e)
	if(_spl_returns == NULL && _target == SQL_POSTGRESQL)
	{
		bool as_exists = TOKEN_CMP(options_start, "AS");

		TokenStr returns;

		if(!as_exists)
			APPENDSTR(returns, " ");

		APPENDSTR(returns, "RETURNS ");

		// If REFCURSOR is returned then this must be specified in RETURNS clause, not VOID
		if(_spl_refcursor_params_num > 0)
		{
			if(_spl_refcursor_params_num > 1)
				APPENDSTR(returns, "SETOF ");

			APPENDSTR(returns, "REFCURSOR");
		}
		// No refcursors returned
		else
			APPENDSTR(returns, "VOID");

		if(as_exists)
			APPENDSTR(returns, " ");

		if(as_exists)
			Prepend(options_start, &returns, create);
		else
			Append(options_start, &returns, create);
	}

	return exists;
}

// Parse CREATE FUNCTION body
bool SqlParser::ParseFunctionBody(Token *create, Token *function, Token *name, Token *as)
{
	Token *body_start = GetLastToken();

	// In DB2, Teradata, MySQL block can start with a label 
	ParseLabelDeclaration(NULL, true);

	// Oracle, SQL Server require BEGIN for functions; DB2 and MySQL not (if single statement); In Informix BEGIN not used
	Token *begin = GetNextWordToken("BEGIN", L"BEGIN", 5);

	// In Oracle and PostgreSQL, variable declaration goes before BEGIN
	if(Source(SQL_ORACLE, SQL_POSTGRESQL) == true && begin == NULL)
	{
		// In PostgreSQL DECLARE keyword goes before variable block, but not in Oracle
		Token *declare = GetNextWordToken("DECLARE", L"DECLARE", 7);

		// Remove if not PostgerSQL
		if(declare != NULL && _target != SQL_POSTGRESQL)
			Token::Remove(declare);

		bool declarations = ParseOracleVariableDeclarationBlock(as);

		// Now get BEGIN
		begin = GetNextWordToken("BEGIN", L"BEGIN", 5);

		// In SQL Server, MySQL declaration is after BEGIN, append BEGIN after AS, and remove BEGIN
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true && declarations == true && begin != NULL)
		{
			// AS will be removed for MySQL
			Append(as, "\nBEGIN", L"\nBEGIN", 6);

			// Change to a space to prevent line deletion
			Token::Change(begin, " ", L" ", 1);       
		}
	}

	Token *atomic = NULL;

	// BEGIN ATOMIC in DB2
	if(begin != NULL)
	{
		atomic = GetNextWordToken("ATOMIC", L"ATOMIC", 6);

		// ATOMIC is not supported in SQL Server and Oracle
		if(Target(SQL_SQL_SERVER, SQL_ORACLE) == true)
			Token::Remove(atomic);
	}

	// DB2, MySQL, Informix do not use AS; in SQL Server, Sybase ASA it is optional
	if(as == NULL)
	{
		// AS required for PostgreSQL
		if(_target == SQL_POSTGRESQL)
		{
			if(begin != NULL)
				Prepend(begin, "AS $$\n", L"AS $$\n", 6);
			else
				Append(body_start, " AS $$\n", L" AS $$\n", 7, create);
		}
		else
		// If we converted function to procedures, AS is required for SQL Server
		if(_target == SQL_SQL_SERVER && _spl_func_to_proc)
			PREPEND(begin, "AS\n");
	}

	// MySQL does not allow AS
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		// If AS is followed by BEGIN, remove AS, otherwise change AS to BEGIN
		if(begin != NULL)
			Token::Remove(as);
		else
			Token::Change(as, "BEGIN", L"BEGIN", 5);
	}

	// MySQL function may not have BEGIN, add it for other databases including SQL Server; for Oracle added in OracleMoveBeginAfterDeclare
	if(begin == NULL && Target(SQL_MARIADB, SQL_MYSQL, SQL_ORACLE, SQL_POSTGRESQL) == false)
		Append(Nvl(as, GetLastToken()), "\nBEGIN", L"\nBEGIN", 6);

	bool frontier = (begin != NULL) ? true : false;

	// Informix does not use BEGIN keyword, but body always terminated with END FUNCTION, set frontier
	if(_source == SQL_INFORMIX)
		frontier = true;

    _spl_begin_blocks.Add(GetLastToken(begin));

	ParseBlock(SQL_BLOCK_PROC, frontier, SQL_SCOPE_FUNC, NULL);

    _spl_begin_blocks.DeleteLast();

	// SQL Server require last statement in function to be RETURN even if the previous is IF RETURN n ELSE RETURN k
	if(_target == SQL_SQL_SERVER && !_spl_func_to_proc && !TOKEN_CMP(_spl_last_stmt, "RETURN"))
		APPEND_FMT(GetLastToken(), "\nRETURN NULL;", _spl_last_stmt);

	// Monday is 1 day was defined from the context
	if(_spl_monday_1 && _target == SQL_SQL_SERVER)
		APPEND_NOFMT(begin, "\n/* Make sure SET DATEFIRST 1; is set for session. Use SELECT @@DATEFIRST; to see the current value. */");

	Token *end = GetNextWordToken("END", L"END", 3);

	// Informix uses END FUNCTION
	if(end != NULL)
	{
		Token *function = GetNextWordToken("FUNCTION", L"FUNCTION", 8);

		// Remove for other databases
		if(function != NULL && _target != SQL_INFORMIX)
			Token::Remove(function);
	}
	
	// BEGIN END required for SQL Server, Oracle functions
	if(begin == NULL && end == NULL && Target(SQL_SQL_SERVER, SQL_ORACLE) == true)
		Append(GetLastToken(), "\nEND;", L"END;", 5);

	// In Oracle ; must follow END name, in SQL Server, DB2, MySQL ; is optional after END
	Token *semi = GetNextCharToken(';', L';');

	// In Oracle procedure name can be specified after END
	if(semi == NULL)
	{
		Token *next = GetNextToken();

		if(Token::Compare(name, next) == true)
		{
			// Remove for SQL Server, MySQL
			if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL))
				Token::Remove(next);

			semi = GetNextCharToken(';', L';');
		}
		else
			PushBack(next);

		// For Oracle append ; after END
		if(end != NULL && semi == NULL)
			AppendNoFormat(end, ";", L";", 1);
	}

	// For Informix replace RETURNING values with OUT parameters or table function
	if(_source == SQL_INFORMIX)
		InformixConvertReturning(create, function);

	// For PostgreSQL add $$ LANGUAGE plpgsql;
	if(_source != SQL_POSTGRESQL && _target == SQL_POSTGRESQL)
	{
		Token *last = GetLastToken();

		Append(last, "\n$$ LANGUAGE", L"\n$$ LANGUAGE", 12, create);
		AppendNoFormat(last, " plpgsql;", L" plpgsql;", 9);
	}

	// For target Oracle, if there declarations in the body move BEGIN after last DECLARE
	if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
		OracleMoveBeginAfterDeclare(create, as, begin, body_start);

	// Transform DB2, MySQL EXIT HANDLERs to Oracle EXCEPTION
	if(Source(SQL_DB2, SQL_MYSQL) && Target(SQL_ORACLE))
		OracleExitHandlersToException(end);

	// User-defined delimiter can be specified for MySQL, often // @ (also used for DB2)
    if(Source(SQL_DB2, SQL_MYSQL))
	    ParseMySQLDelimiter(create);

	// Add variable declarations generated in the procedural block
	AddGeneratedVariables();

	return true;
}