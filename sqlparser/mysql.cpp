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

// MySQL specific clauses

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// User-defined delimiter can be specified for MySQL, often // @ (also used for DB2)
bool SqlParser::ParseMySQLDelimiter(Token * /*create*/)
{
    Token *char1 = NULL;
    Token *char2 = NULL;

    // Check default delimiter // and /
    MySQLCheckDelimiter('/', L'/', &char1, &char2);

    // Check @ and @@
    if(char1 == NULL)
        MySQLCheckDelimiter('@', L'@', &char1, &char2);

    if(char1 == NULL)
        return false;

    // DB2 and MySQL do not require the delimiter to be on a new line, while this required for other databases
    if(Target(SQL_SQL_SERVER, SQL_ORACLE) && Token::Compare(char1->prev, '\n', L'\n') == false)
        Prepend(char1, "\n", L"\n", 1);

    // Use GO for SQL Server
    if(_target == SQL_SQL_SERVER)
    {
        Token::Change(char1, "GO", L"GO", 2);
        Token::Remove(char2);

        _spl_delimiter_set = true;
    }
    else
        // Use / for Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Change(char1, "/", L"/", 1);
            Token::Remove(char2);

            _spl_delimiter_set = true;
        }

        return true;
}

// Check for MySQL and DB2 procedure delimiter 'char' and 'charchar'
bool SqlParser::MySQLCheckDelimiter(char ch, wchar_t wch, Token **char1_out, Token **char2_out)
{
    Token *char1 = GetNextCharToken(ch, wch);
    Token *char2 = NULL;

    // Character is often duplicated // i.e
    if(char1 != NULL)
    {
        char2 = GetNextCharToken(ch, wch);

        // There must no be a space between characters
        if(char2 != NULL && char1->next != char2)
        {
            char2 = NULL;
            PushBack(char2);
        }
    }

    if(char1_out != NULL)
        *char1_out = char1;

    if(char2_out != NULL)
        *char2_out = char2;

    if(char1 == NULL)
        return false;

    return true;
}

// Convert MySQL identifier
bool SqlParser::ConvertMysqlIdentifier(Token *token, int expected_type)
{
    // Column, variable or parameter name
    if(expected_type == SQL_IDENT_COLVAR)
    {
        // Check for new.column in a trigger
        if(_spl_scope == SQL_SCOPE_TRIGGER && Token::Compare(token, "new.", L"new.", 0, 4) == true)
        {
            // Use :new.column in Oracle
            if(_target == SQL_ORACLE)
                PrependNoFormat(token, ":", L":", 1);
        }
    }

    return true;
}

// Move inline non-unique indexes to separate CREATE INDEX statements
void SqlParser::MysqlMoveInlineIndexes(ListWM &inline_indexes, Token *append)
{
    if(append == NULL)
        return;

    for(ListwmItem *i = inline_indexes.GetFirst(); i != NULL; i = i->next)
    {
        Token *start = (Token*)i->value;
        Token *end = (Token*)i->value2;

        if(i->prev == NULL)
            Append(append, "\n", L"\n", 1);

        // Move index definition
        Append(append, "\n", L"\n", 1);
        AppendCopy(append, start, end, false);
        Append(append, ";", L";", 1);

        // Remove index from CREATE TABLE
        Token::Remove(start, end);
    }
}

// Add NOT_FOUND variable and condition handler
void SqlParser::MySQLAddNotFoundHandler()
{
    if(_spl_not_found_handler)
        return;

    Token *append_var = _spl_last_outer_declare_var;
    Token *append = GetDeclarationAppend();
    
    Token *format = Nvl(_spl_outer_begin, _spl_outer_as);

    if(append == NULL)
        append = Nvl(_spl_outer_begin, _spl_outer_as);

    if(append_var == NULL)
        append_var = Nvl(_spl_outer_begin, _spl_outer_as);

    // Variable must go before cursor and condition handlers
    AppendFirst(append_var, "\nDECLARE not_found INT DEFAULT 0;", L"\nDECLARE not_found INT DEFAULT 0;", 33, format);
    Append(append, "\nDECLARE CONTINUE HANDLER FOR NOT FOUND SET not_found = 1;", L"\nDECLARE CONTINUE HANDLER FOR NOT FOUND SET not_found = 1", 58, format);

    _spl_not_found_handler = true;
}

// Initialize not_found variable before second and subsequent OPEN cursors
void SqlParser::MySQLInitNotFoundBeforeOpen()
{
	int k  = 0;
	for(ListwmItem *i = _spl_open_cursors.GetFirst(); i != NULL; i = i->next)
	{
		Token *open = (Token*)i->value;

		if(open != NULL && k > 0)
			PREPEND(open, "SET NOT_FOUND = 0;\n");

		k++;
	}
}	

// MySQL, MariaDB require cursor declaration go before any other statements
bool SqlParser::MySQLMoveCursorDeclarations(Token *declare, Token *cursor_end)
{
	if(_spl_first_non_declare == NULL || declare == NULL || cursor_end == NULL)
		return false;

	// Non declare statement goes earlier
	if(_spl_first_non_declare->remain_size > declare->remain_size)
	{
		Token *semi = TOKEN_GETNEXT(';');

		AppendNewlineCopy(_spl_last_declare, declare, Nvl(semi, cursor_end), 2, false);
		Token::Remove(declare, cursor_end);

		if(semi == NULL)
			APPEND_NOFMT(_spl_last_declare, ";");

		return true;
	}

	return false;
}

// MySQL DEFINER, ALGORITHM clauses in CREATE statements
bool SqlParser::ParseMyqlDefinerClause(Token *token)
{
    if(token == NULL)
        return false;

    bool exists = false;

    Token *next = token;

    while(true)
    {
        // DEFINER = user
        if(next->Compare("DEFINER", L"DEFINER", 7) == true)
        {
            Token *equal = GetNextCharToken('=', L'=');

            // User can be specified as `admin`@`%` i.e
            Token *user = GetNextToken();
            Token *at = GetNextCharToken(user, '@', L'@');
            Token *host = GetNextToken(at);

            if(_target != SQL_MYSQL)
            {
                Token::Remove(next, equal);
                Token::Remove(user, host);
            }

            exists = true;
        }
        else
            // ALGORITHM = UNDEFINED | MERGE | TEMPTABLE for views
            if(next->Compare("ALGORITHM", L"ALGORITHM", 9) == true)
            {
                Token *equal = GetNextCharToken('=', L'=');
                Token *value = GetNextToken(equal);

                if(_target != SQL_MYSQL)
                    Token::Remove(next, value);

                exists = true;
            }
            else
                // SQL SECURITY { DEFINER | INVOKER }
                if(next->Compare("SQL", L"SQL", 3) == true)
                {
                    Token *security = GetNextWordToken("SECURITY", L"SECURITY", 8);

                    if(security == NULL)
                    {
                        PushBack(next);
                        break;
                    }

                    // DEFINER | INVOKER
                    Token *value = GetNextToken();

                    if(_target != SQL_MYSQL)
                        Token::Remove(next, value);

                    exists = true;
                }
                else
                    // Not a definer option
                {
                    if(next != token)
                        PushBack(next);

                    break;
                }

                next = GetNextToken();

                if(next == NULL)
                    break;
    }

    return exists;
}

// MySQL SET options, i.e. SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
bool SqlParser::ParseMysqlSetOptions(Token *set)
{
    if(set == NULL)
        return false;

    bool single_exists = false;
    bool exists = false;

    Token *start = NULL;

    // First check single options (one option can be specified per SET statement)
    Token *option = GetNext();

    // SET NAMES character_set
    if(Token::Compare(option, "NAMES", L"NAMES", 5) == true)
    {
        /*Token *charset */ (void) GetNext();

        single_exists = true;
        exists = true;
    }
    else
        PushBack(option);

    // Multiple options can be set or read
    while(true)
    {
        if(single_exists == true)
            break;

        // Any option can appear on the left (without @@) and right side (with @@)
        Token *option1 = GetNextToken();

        if(start == NULL)
            start = option1;

        Token *equal = GetNextCharToken(option1, '=', L'=');

        Token *option2 = GetNextToken(equal);

        if(option1 == NULL || option2 == NULL)
            break;

        // @@UNIQUE_CHECKS
        if(option1->Compare("UNIQUE_CHECKS", L"UNIQUE_CHECKS", 13) == true ||
            option2->Compare("@@UNIQUE_CHECKS", L"@@UNIQUE_CHECKS", 15) == true)
            exists = true;
        else
            // @@FOREIGN_KEY_CHECKS
            if(option1->Compare("FOREIGN_KEY_CHECKS", L"FOREIGN_KEY_CHECKS", 18) == true ||
                option2->Compare("@@FOREIGN_KEY_CHECKS", L"@@FOREIGN_KEY_CHECKS", 20) == true)
                exists = true;
            else
                // @@SQL_MODE
                if(option1->Compare("SQL_MODE", L"SQL_MODE", 8) == true ||
                    option2->Compare("@@SQL_MODE", L"@@SQL_MODE", 10) == true)
                    exists = true;

        Token *comma = GetNextCharToken(',', L',');

        if(comma == NULL)
            break;
    }

    // At least one system option is set
    if(exists == true)
    {
        if(_target != SQL_MYSQL)
            Comment(set, GetNextCharOrLastToken(';', L';'));
    }
    else
        PushBack(start);

    return exists;
}

// MySQL CREATE DATABASE options
bool SqlParser::MysqlCreateDatabase(Token *create, Token *database, Token *name)
{
    if(create == NULL || database == NULL || name == NULL)
        return false;

    // Options
    while(true)
    {
        // Optional DEFAULT keyword
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
            {
                PushBack(next);
                break;
            }
    }

    // CREATE USER in Oracle
    if(_target == SQL_ORACLE)
    {
        Token::Change(database, "USER", L"USER", 4);

        // Add default password
        Append(name, " IDENTIFIED BY ", L" IDENTIFIED BY ", 15, create);
        AppendCopy(name, name);
    }

    return true;
}