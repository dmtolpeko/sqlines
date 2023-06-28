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

	// Sybase ADS creates implicit record to access cursor items as cursor.field
	_spl_implicit_rowtype_vars.Add(cursor);

	// DO keyword starts the block
	Token *do_ = TOKEN_GETNEXTW("DO");

	if(_target == SQL_SQL_SERVER)
	{
		TOKEN_CHANGE(do_, "BEGIN");

		// FETCH the first row
		Token *format = PREPEND(while_, "FETCH ");
		PrependCopy(while_, cursor);
		Token *fetch_last = PREPEND(while_, " INTO ");
		_spl_implicit_rowtype_fetches.Add(cursor, fetch_last, format);

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
			Token *format = PREPEND(end, "FETCH ");
			PrependCopy(end, cursor);
			Token *fetch_last = PREPEND(end, " INTO ");
			_spl_implicit_rowtype_fetches.Add(cursor, fetch_last, format);

			Token::Remove(cursor);
			Token::Remove(end_while);
		}		
	}
	
	// Delete reference to implicit record variable
	_spl_implicit_rowtype_vars.DeleteItems(cursor);

	return true;
}

// Sybase ADS nested UDF call without variable assinment SELECT udf(params) FROM System.iota (standalone SELECT)
void SqlParser::SybaseAdsSelectNestedUdfCall(Token *select, ListW *select_cols)
{
	if(select == NULL && select_cols == NULL)
		return;

	ListwItem *i = select_cols->GetFirst();
	Token *func = (i != NULL) ? (Token*)i->value : NULL;

	if(func != NULL && func->type == TOKEN_FUNCTION)
	{
		// SQL Server requires EXECUTE to call a UDF and () not allowed
		if(_target == SQL_SQL_SERVER)
		{
			TOKEN_CHANGE(select, "EXECUTE");
			TOKEN_CHANGE(func->open, " ");
			Token::Remove(func->close);
		}
	}
}

// Map DATETIME style for CONVERT function in Sybase ASE
void SqlParser::SybaseMapDatetimeStyle(Token *style)
{
	if(TOKEN_CMP(style, "0") || TOKEN_CMP(style, "100"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d %Y %h:%i%p'");
	}
	else
	if(TOKEN_CMP(style, "1"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%m/%d/%y'");
	}
	else
	if(TOKEN_CMP(style, "101"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%m/%d/%Y'");
	}
	else
	if(TOKEN_CMP(style, "2"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%y.%m.%d'");
	}
	else
	if(TOKEN_CMP(style, "102"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%Y.%m.%d'");
	}
	else
	if(TOKEN_CMP(style, "3"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d/%m/%y'");
	}
	else
	if(TOKEN_CMP(style, "103"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d/%m/%Y'");
	}
	else
	if(TOKEN_CMP(style, "4"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d.%m.%y'");
	}
	else
	if(TOKEN_CMP(style, "104"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d.%m.%Y'");
	}
	else
	if(TOKEN_CMP(style, "5"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d-%m-%y'");
	}
	else
	if(TOKEN_CMP(style, "105"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d-%m-%Y'");
	}
	else
	if(TOKEN_CMP(style, "6"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d %b %y'");
	}
	else
	if(TOKEN_CMP(style, "106"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d %b %Y'");
	}
	else
	if(TOKEN_CMP(style, "7"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d, %y'");
	}
	else
	if(TOKEN_CMP(style, "107"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d, %Y'");
	}
	else
	if(TOKEN_CMP(style, "8") || TOKEN_CMP(style, "108"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%H:%i:%s'");
	}
	else
	if(TOKEN_CMP(style, "9") || TOKEN_CMP(style, "109"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d %Y %h:%i:%s:%f%p'");
	}
	else
	if(TOKEN_CMP(style, "10"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%m-%d-%y'");
	}
	else
	if(TOKEN_CMP(style, "110"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%m-%d-%Y'");
	}
	else
	if(TOKEN_CMP(style, "11"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%y/%m/%d'");
	}
	else
	if(TOKEN_CMP(style, "111"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%Y/%m/%d'");
	}
	else
	if(TOKEN_CMP(style, "12"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%y%m%d'");
	}
	else
	if(TOKEN_CMP(style, "112"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%Y%m%d'");
	}
	else
	if(TOKEN_CMP(style, "13"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%y/%d/%m'");
	}
	else
	if(TOKEN_CMP(style, "113"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%Y/%d/%m'");
	}
	else
	if(TOKEN_CMP(style, "14"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%m/%y/%d'");
	}
	else
	if(TOKEN_CMP(style, "114"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%m/%Y/%d'");
	}
	else
	if(TOKEN_CMP(style, "15"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d/%y/%m'");
	}
	else
	if(TOKEN_CMP(style, "115"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%d/%Y/%m'");
	}
	else
	if(TOKEN_CMP(style, "16") || TOKEN_CMP(style, "116"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d %Y %H:%i:%s'");
	}
	else
	if(TOKEN_CMP(style, "17") || TOKEN_CMP(style, "117"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%h:%i%p'");
	}
	else
	if(TOKEN_CMP(style, "18") || TOKEN_CMP(style, "118"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%H:%i'");
	}
	else
	if(TOKEN_CMP(style, "19"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%h:%i:%s:%f%p'");
	}
	else
	if(TOKEN_CMP(style, "20"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%H:%i:%s:%f'");
	}
	else
	if(TOKEN_CMP(style, "21"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%y/%m/%d %H:%i:%s'");
	}
	else
	if(TOKEN_CMP(style, "22"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%y/%m/%d %h:%i%p'");
	}
	else
	if(TOKEN_CMP(style, "23"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%Y-%m-%dT%H:%i:%s'");
	}
	else
	if(TOKEN_CMP(style, "36") || TOKEN_CMP(style, "136"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%h:%i:%s.%f%p'");
	}
	else
	if(TOKEN_CMP(style, "37") || TOKEN_CMP(style, "137"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%H:%i:%s.%f'");
	}
	else
	if(TOKEN_CMP(style, "38"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d %y %h:%i:%s.%f%p'");
	}
	else
	if(TOKEN_CMP(style, "138"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d %Y %h:%i:%s.%f%p'");
	}
	else
	if(TOKEN_CMP(style, "39"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d %y %H:%i:%s.%f'");
	}
	else
	if(TOKEN_CMP(style, "139"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%b %d %Y %H:%i:%s.%f'");
	}
	else
	if(TOKEN_CMP(style, "40"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%y-%m-%d %H:%i:%s.%f'");
	}
	else
	if(TOKEN_CMP(style, "140"))
	{
		if(Target(SQL_MARIADB, SQL_MYSQL))
			TOKEN_CHANGE_NOFMT(style, "'%Y-%m-%d %H:%i:%s.%f'");
	}
}