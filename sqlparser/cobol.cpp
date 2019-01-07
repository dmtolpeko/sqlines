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

// COBOL language parser

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "cobol.h"

// Constructor
Cobol::Cobol()
{
	_sqlparser = NULL;

	_source = 0;
	_target = 0;
	_target_app = 0;

	ClearScope();
}

// Parse a COBOL statement
bool Cobol::ParseStatement(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	// CBL RENT etc.
	if(TOKEN_CMP(token, "CBL"))
	{
		Token *rent = TOKEN_GETNEXTW("RENT");

		if(rent != NULL)
			Token::Remove(token, rent);

		exists = true;
	}
	else
	// IDENTIFICATION division
	if(TOKEN_CMP(token, "IDENTIFICATION"))
		exists = ParseIdentificationDivision(token);
	else
	// ENVIRONMENT division
	if(TOKEN_CMP(token, "ENVIRONMENT"))
		exists = ParseEnvironmentDivision(token);
	else
	// DATA division
	if(TOKEN_CMP(token, "DATA"))
		exists = ParseDataDivision(token);
	else
	// PROCEDURE division
	if(TOKEN_CMP(token, "PROCEDURE"))
		exists = ParseProcedureDivision(token);

	return exists;
}

// IDENTIFICATION DIVISION
bool Cobol::ParseIdentificationDivision(Token *identification)
{
	if(identification == NULL)
		return false;

	ClearScope();

	_proc_start = identification;

	// DIVISION keyword
	Token *division = TOKEN_GETNEXTW("DIVISION");
	Token *dot = TOKEN_GETNEXT('.');

	if(_target_app != APP_COBOL)
		Token::Remove(identification, Nvl(dot, division));

	// Parse IDENTIFICATION division paragraphs
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		/*Token *dot */ (void) GetNextCharToken('.', L'.');

		// PROGRAM_ID. name
		if(next->Compare("PROGRAM-ID", L"PROGRAM-ID", 10) == true)
		{
			// Program name
			_proc_name = GetNextToken();

			/*Token *dot2 */ (void) GetNextCharToken('.', L'.');
			
			if(_target_app == APP_PLSQL)
			{
				// Convert DB2 COBOL stored procedure to standalone PL/SQL procedure
				if(Source(SQL_DB2))
					_convert_to_proc = true;
				else
				{
					PREPEND(_proc_start, "CREATE OR REPLACE PACKAGE ");
					PrependCopy(next, _proc_name);
					Prepend(next, " IS\n", L" IS\n", 4);					
				}

				Prepend(next, "-- ", L"-- ", 3);
			}

			continue;
		}
		else
		// AUTHOR. text
		if(next->Compare("AUTHOR", L"AUTHOR", 6) == true)
		{
			/*Token *text */ (void) GetNextUntilNewlineToken();
			
			if(_target_app == APP_PLSQL)
				Prepend(next, "-- ", L"-- ", 3);				

			continue;
		}
		else
		// DATE-WRITTEN. date.
		if(next->Compare("DATE-WRITTEN", L"DATE-WRITTEN", 12) == true)
		{
			/*Token *date */ (void) GetNextUntilNewlineToken();
			
			if(_target_app == APP_PLSQL)
				Prepend(next, "-- ", L"-- ", 3);				

			continue;
		}
		else
		// DATE-COMPILED. date.
		if(next->Compare("DATE-COMPILED", L"DATE-COMPILED", 13) == true)
		{
			/*Token *date */ (void) GetNextUntilNewlineToken();
			
			if(_target_app == APP_PLSQL)
				Prepend(next, "-- ", L"-- ", 3);				

			continue;
		}
		
		// Not a division paragraph
		PushBack(next);
		break;
	}

	return true;
}

// ENVIRONMENT DIVISION
bool Cobol::ParseEnvironmentDivision(Token *environment)
{
	if(environment == NULL)
		return false;

	// DIVISION keyword
	Token *division = TOKEN_GETNEXTW("DIVISION");
	Token *dot = TOKEN_GETNEXT('.');

	if(_target_app != APP_COBOL)
		Token::Remove(environment, Nvl(dot, division));

	// Parse ENVIRONMENT division paragraphs
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// INPUT-OUTPUT SECTION.
		if(TOKEN_CMP(next, "INPUT-OUTPUT"))
		{
			ParseEnvironmentInputOutputSection(next);
			continue;
		}
				
		// Not a division paragraph
		PushBack(next);
		break;
	}

	return true;
}

// INPUT-OUTPUT section of ENVIRONMENT DIVISION
bool Cobol::ParseEnvironmentInputOutputSection(Token *inputoutput)
{
	if(inputoutput == NULL)
		return false;

	Token *section = TOKEN_GETNEXTW("SECTION");
	Token *dot = TOKEN_GETNEXT('.'); 

	if(_target_app != APP_COBOL)
		Token::Remove(inputoutput, Nvl(dot, section));

	// Parse INPUT-OUTPUT section paragraphs
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// FILE-CONTROL.
		if(TOKEN_CMP(next, "FILE-CONTROL"))
		{
			ParseEnvironmentInputOutputSectionFileControl(next);
			continue;
		}
				
		// Not a division paragraph
		PushBack(next);
		break;
	}

	return true;
}

// FILE-CONTROL. section of INPUT-OUTPUT section
bool Cobol::ParseEnvironmentInputOutputSectionFileControl(Token *filecontrol)
{
	if(filecontrol == NULL)
		return false;

	Token *dot = TOKEN_GETNEXT('.'); 

	if(_target_app != APP_COBOL)
		Token::Remove(filecontrol, dot);

	return true;
}

// DATA DIVISION
bool Cobol::ParseDataDivision(Token *data)
{
	if(data == NULL)
		return false;

	// DIVISION keyword
	Token *division = TOKEN_GETNEXTW("DIVISION");
	Token *dot = TOKEN_GETNEXT('.');

	if(_target_app != APP_COBOL)
		Token::Remove(data, Nvl(dot, division));

	// Parse DATA division paragraphs
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// FILE SECTION.
		if(TOKEN_CMP(next, "FILE"))
		{
			ParseDataFileSection(next);
			continue;
		}
		else
		// WORKING-STORAGE SECTION.
		if(TOKEN_CMP(next, "WORKING-STORAGE"))
		{
			ParseDataWorkingStorageSection(next);
			continue;
		}
		else
		// LINKAGE SECTION.
		if(TOKEN_CMP(next, "LINKAGE"))
		{
			ParseDataLinkageSection(next);
			continue;
		}
				
		// Not a division paragraph
		PushBack(next);
		break;
	}

	return true;
}

// FILE SECTION. of DATA DIVISION
bool Cobol::ParseDataFileSection(Token *file)
{
	if(file == NULL)
		return false;

	Token *section = TOKEN_GETNEXTW("SECTION");
	Token *dot = TOKEN_GETNEXT('.'); 

	if(_target_app != APP_COBOL)
		Token::Remove(file, Nvl(dot, section));

	return true;
}

// WORKING-STORAGE SECTION. of DATA DIVISION - program static data
bool Cobol::ParseDataWorkingStorageSection(Token *working_storage)
{
	if(working_storage == NULL)
		return false;

	Token *section = TOKEN_GETNEXTW("SECTION");
	Token *dot = TOKEN_GETNEXT('.'); 

	if(_target_app != APP_COBOL)
		Token::Remove(working_storage, Nvl(dot, section));

	ParseDataSection();

	return true;
}

// LINKAGE SECTION. of DATA DIVISION - program input/output parameters
bool Cobol::ParseDataLinkageSection(Token *linkage)
{
	if(linkage == NULL)
		return false;

	Token *section = TOKEN_GETNEXTW("SECTION");
	Token *dot = TOKEN_GETNEXT('.'); 

	if(_target_app != APP_COBOL)
		Token::Remove(linkage, Nvl(dot, section));

	ParseDataSection(CBL_SCOPE_LINKAGE_SECTION);

	return true;
}

// PROCEDURE DIVISION
bool Cobol::ParseProcedureDivision(Token *procedure)
{
	if(procedure == NULL)
		return false;

	Token *division = TOKEN_GETNEXTW("DIVISION");
	Token *using_ = TOKEN_GETNEXTW("USING");

	// List of parameters
	if(using_ != NULL)
	{
		Token *name = GetNextToken();

		while(true)
		{
			_proc_using_params.Add(name);
			ConvertIdentifier(name, SQL_IDENT_PARAM);

			Token *comma = TOKEN_GETNEXT(',');

			if(_convert_to_proc)
				Token::Remove(name, comma);

			if(comma == NULL)
				break;
						
			name = GetNextToken();
		}
	}
	
	Token *dot = TOKEN_GETNEXT('.');
	
	if(_target_app != APP_COBOL)
	{
		if(_convert_to_proc)
			TOKEN_CHANGE(procedure, "BEGIN");

		Token::Remove(division, using_);
		Token::Remove(dot);
	}

	ParseBlock();

	// Terminate the procedure
	APPEND_NOFMT(GetLastToken(), "\nEND;\n/");

	// Add procedure parameters
	if(_target == SQL_ORACLE && _proc_start != NULL && _proc_name != NULL && _proc_using_params.GetCount() > 0)
	{
		TokenStr params;

		APPENDSTR(params, "CREATE OR REPLACE PROCEDURE ");
		params.Append(_proc_name);
		APPENDSTR(params, "(");

		int c = 0;
		for(ListwItem *i = _proc_using_params.GetFirst(); i != NULL; i = i->next)
		{
			Token *param = (Token*)i->value;

			if(param == NULL)
				break;

			if(c > 0)
				APPENDSTR(params, ", ");

			params.Append(param);
			APPENDSTR(params, " ");

			bool found = false;

			// Find parameter type defined in the linkage section
			for(ListwmItem *j = _proc_linkage_params.GetFirst(); j != NULL; j = j->next)
			{
				Token *link_param = (Token*)j->value;

				if(Token::Compare(param, link_param))
				{
					params.Append((Token*)j->value2);

					found = true;
					break;
				}
			}

			if(!found)
				APPENDSTR(params, "VARCHAR2");

			c++;
		}

		APPENDSTR(params, ")\nIS");
		APPENDS(_proc_start, &params); 		
	}

	return true;
}

// Parse procedure division statements
void Cobol::ParseBlock()
{
	while(true)
	{
		ParseStatement();

		Token *next = GetNextToken();

		if(next == NULL)
			break;
		else
			PushBack(next);
	}
}

// Parse single statement
void Cobol::ParseStatement(int stmt_scope)
{
	// Not that statements not followed by . make up the single statement with the subsequent statements until .
	while(true)
	{
		bool exists = false;
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// EXEC SQL statement 
		if(TOKEN_CMP(next, "EXEC"))
			exists = ParseExecSqlStatement(next);
		else
		if(TOKEN_CMP(next, "IF"))
			exists = ParseIfStatement(next);			
		else
		if(TOKEN_CMP(next, "MOVE"))
			exists = ParseMoveStatement(next);	
		else
		if(TOKEN_CMP(next, "PROG-END") || TOKEN_CMP(next, "GOBACK"))
		{
			Token *dot = TOKEN_GETNEXT('.');
			Token::Remove(next, dot);

			exists = true;	
		}

		if(!exists)
			break;

		Token *dot = TOKEN_GETNEXT('.');

		// Statement is ended, so exit
		if(dot != NULL)
		{
			// Every statement must terminate itself (sometimes statements just removed so no delimiter is required)
			Token::Remove(dot);
			break;
		}
		else
		// True block of IF statement can be ended by ELSE (still without . in the last statement before ELSE)
		if(stmt_scope == CBL_SCOPE_IF_TRUE && LOOKNEXT("ELSE"))
			break;
	}
}

// Parse data section that can contain variable definitions, COPY and INCLUDE commands
bool Cobol::ParseDataSection(int section_scope)
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Check for a number starting variable or record declaration
		if(next->IsNumeric() == true)
		{
			PushBack(next);
			ParseVariableSection(section_scope);

			exists = true;
			continue;
		}
		else
		// EXEC SQL statement (cursor declaration i.e)
		if(TOKEN_CMP(next, "EXEC"))
		{
			ParseExecSqlStatement(next);

			exists = true;
			continue;
		}
		else
		// COPY command
		if(TOKEN_CMP(next, "COPY"))
		{
			ParseCopyStatement(next);

			exists = true;
			continue;
		}

		// Not a data section command
		PushBack(next);
		break;
	}

	return exists;
}

// Parse variable definitions
bool Cobol::ParseVariableSection(int section_scope, Token *parent_record, ListWM *record_items)
{
	bool exists = false;

	// Level number for the first item
	Token *entry_num = NULL;

	int e_num = 0;

	// Period after the last parsed item
	Token *var_dot = NULL;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Check for a number representing item hierarchy 01, 05, 10 etc.
		if(next->IsNumeric() == true)
		{
			// Check that a higher level is not got
			if(entry_num != NULL)
			{
				int c_num = next->GetInt();

				// If within a record, change period for the previous item to comma for Oracle; it will be removed for last item later
				if(parent_record != NULL && _target_app == APP_PLSQL)
					Token::Change(var_dot, ",", L",", 1);

				// 01 is higher level than 05 i.e.
				if(c_num < e_num)
				{
					PushBack(next);
					break;
				}
			}
			else
			{
				entry_num = next;
				e_num = entry_num->GetInt();
			}

			// Identifier must be next
			Token *var = GetNextToken();

			if(var == NULL)
				break;

			if(section_scope == CBL_SCOPE_LINKAGE_SECTION)
				AddParameter(var);
			else
				AddVariable(var);

			// If period follows the name it means record definition, otherwise variable definition
			Token *dot = GetNextCharToken('.', L'.');

			// Parse the record
			if(dot != NULL)
			{
				if(parent_record == NULL)
					_current_top_record = next;

				ParseRecordDeclaration(section_scope, parent_record, next, var, dot);
			}
			else
			{
				ParseVariableDeclaration(section_scope, next, var, record_items);

				// Each variable declaration ends with period
				var_dot = TOKEN_GETNEXT('.');

				if(var_dot != NULL)
					TOKEN_CHANGE(var_dot, ";");

				// All variables from linkage section are moved to parameter list, but we will comment the declarations, 
				// not remove them to preserve variable length, code comments etc. 
				if(section_scope == CBL_SCOPE_LINKAGE_SECTION && parent_record == NULL)
					_sqlparser->Comment(next, var_dot);
			}
		}
		else
		{
			PushBack(next);
			break;
		}
	}

	return exists;
}

// Record declaration
bool Cobol::ParseRecordDeclaration(int section_scope, Token *parent_record, Token *num, Token *name, Token *dot)
{
	if(name == NULL)
		return false;

	ListWM record_items;
	bool special_case = false;

	ParseVariableSection(section_scope, name, &record_items);

	// Last token should point to period (end of last variable of this record)
	Token *end_dot = GetLastToken();

	// Check for DB2 special records
	if(Source(SQL_DB2))
		special_case = ParseDb2RecordDeclaration(section_scope, num, name, dot, end_dot, &record_items);

	// Use TYPE rec IS RECORD in Oracle
	if(!special_case && _target_app == APP_PLSQL)
	{
		if(parent_record == NULL)
			Token::Change(end_dot, ";", L";", 1);

		Prepend(name, "TYPE ", L"TYPE ", 5);
		Append(name, " IS RECORD (", L" IS RECORD (", 12);
		/*Token *last */ (void) Prepend(end_dot, ")", L")", 1);

		Token::Remove(num);
		Token::Remove(dot);

		// If it is not top level record, move TYPE definition before top level recod
		if(parent_record != NULL)
		{
			Token *last = Prepend(end_dot, ";", L";", 1);

			PrependCopy(_current_top_record, num, last, false);
			Prepend(_current_top_record, "\n\n", L"\n\n", 2);

			// Specify a record variable inside the record instead full nested record definition
			PrependCopy(end_dot, name);
			Prepend(end_dot, " ", L" ", 1);
			PrependCopy(end_dot, name);
			//Append(last, ";", L";", 1);

			Token::Remove(num, last);
		}
	}

	return false;
}

bool Cobol::ParseDb2RecordDeclaration(int section_scope, Token *num, Token *name, Token *dot, Token *end_dot, ListWM *record_items)
{
	if(record_items == NULL)
		return false;

	bool exists = false;

	// Check for DB2 SQL VARCHAR(n) record
	if(record_items->GetCount() == 2)
	{
		ListwmItem* v1 = record_items->GetFirst();
		ListwmItem* v2 = v1->next;

		Token *type1 = (Token*)v1->value3;
		Token *type2 = (Token*)v2->value3;
		Token *len = (Token*)v2->value4;

		// First data type is 9 or S9, second type is X
		if(type1 != NULL && type2 != NULL && (TOKEN_CMP(type1, "S9") || TOKEN_CMP(type1, "9")) && TOKEN_CMP(type2, "X"))
		{
			APPEND(name, " VARCHAR2(");
			AppendCopy(name, len);
			APPEND_NOFMT(name, ");");

			Token::Remove(num);
			Token::Remove(dot);
			Token::Remove((Token*)v1->value, (Token*)v2->value5);
			Token::Remove(end_dot);

			if(section_scope == CBL_SCOPE_LINKAGE_SECTION)
				_proc_linkage_params.Add(name, type2);

			exists = true;
		}
	}

	// All variables from linkage section are moved to parameter list, but we will comment the declarations, 
	// not remove them to preserve variable length, code comments etc. 
	if(exists && section_scope == CBL_SCOPE_LINKAGE_SECTION)
		_sqlparser->Comment(num, dot);

	return exists;
}

// Parse single variable declaration, not a record
bool Cobol::ParseVariableDeclaration(int section_scope, Token *num, Token *name, ListWM *record_items)
{
	if(name == NULL)
		return false;

	Token *type = NULL;
	Token *len = NULL;
	Token *var_end = NULL;

	// Variable options
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Exit when the options are ended with period
		if(next->Compare('.', L'.') == true)
		{
			PushBack(next);
			break;
		}	

		// PIC
		if(next->Compare("PIC", L"PIC", 3) == true)
			ParsePicFormat(next, &type, &len, &var_end);

		if(record_items != NULL)
			record_items->Add(num, name, type, len, var_end);
	}

	if(record_items == NULL && section_scope == CBL_SCOPE_LINKAGE_SECTION)
		_proc_linkage_params.Add(name, type);

	if(_target_app == APP_PLSQL)
		Token::Remove(num);

	return true;
}

// Parse PIC format for variable
bool Cobol::ParsePicFormat(Token *pic, Token **type, Token **len, Token **var_end)
{
	if(pic == NULL)
		return false;

	Token *num = NULL;
	Token *typ = NULL;

	// Multiple format items can be specified
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Exit when period is faced
		if(next->Compare('.', L'.') == true)
		{
			PushBack(next);
			break;
		}

		// X(n)
		if(next->Compare("X", L"X", 1) == true)
		{
			Token *open = GetNextCharToken('(', L'(');
			num = GetNextToken(open);
			/*Token *close */ (void) GetNextCharToken(')', L')');

			// VARCHAR2 in Oracle
			if(_target_app == APP_PLSQL)
			{
				Token::Change(next, "VARCHAR2", L"VARCHAR2", 8);
				Token::Remove(pic);
			}

			typ = next;
			continue;
		}
		else
		// XXXXX...
		if(next->Compare("X", L"X", 1, 1) == true && next->len > 1)
		{
			TokenStr len("VARCHAR2(", L"VARCHAR2(", 9);
			len.Append((int)next->len);
			len.Append(")", L")", 1);

			// VARCHAR2(len) in Oracle
			if(_target_app == APP_PLSQL)
			{
				Token::ChangeNoFormat(next, len);
				Token::Remove(pic);
			}

			continue;
		}
		else
		// 9(n) and S9(n) for numbers
		if(TOKEN_CMP(next, "9") || TOKEN_CMP(next, "S9"))
		{
			Token *open = GetNextCharToken('(', L'(');
			num = GetNextToken(open);
			/*Token *close */ (void) GetNextCharToken(')', L')');

			// NUMBER in Oracle
			if(_target_app == APP_PLSQL)
			{
				Token::Change(next, "NUMBER", L"NUMBER", 6);
				Token::Remove(pic);
			}

			typ = next;
			continue;
		}

		PushBack(next);
		break;
	}

	// USAGE BINARY | DISPLAY
	Token *usage = TOKEN_GETNEXTW("USAGE");

	if(usage != NULL)
	{
		Token *value = GetNextToken();

		if(value != NULL && _target_app != APP_COBOL)
			Token::Remove(usage, value);
	}

	if(type != NULL)
		*type = typ;

	if(len != NULL)
		*len = num;

	if(var_end != NULL)
		*var_end = GetLastToken();

	return true;
}

// EXEC SQL statement
bool Cobol::ParseExecSqlStatement(Token *exec)
{
	bool exists = false;

	Token *sql = TOKEN_GETNEXTW("SQL");

	// Parse DATA division paragraphs
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// INCLUDE SQLCA
		if(TOKEN_CMP(next, "INCLUDE"))
		{
			Token *sqlca = TOKEN_GETNEXTW("SQLCA");
			Token::Remove(next, sqlca);

			exists = true;
			continue;
		}
		else
		// DECLARE name CURSOR
		if(TOKEN_CMP(next, "DECLARE"))
		{
			Token *name = GetNextToken();
			Token *cursor = TOKEN_GETNEXTW("CURSOR");

			if(name != NULL && cursor != NULL)
			{
				_sqlparser->SetLevel(LEVEL_SQL);
				_sqlparser->ParseDeclareCursor(next, name, cursor);
				_sqlparser->SetLevel(LEVEL_APP);
			}

			Token::Remove(next);
			APPEND_NOFMT(GetLastToken(), ";");

			exists = true;
			continue;
		}
		else
		// SELECT INTO statement
		if(TOKEN_CMP(next, "SELECT"))
		{
			ListW into_cols;

			_sqlparser->SetLevel(LEVEL_SQL);
			_sqlparser->ParseSelectStatement(next, 0, 0, NULL, NULL, NULL, NULL, &into_cols, NULL, NULL, NULL);
			_sqlparser->SetLevel(LEVEL_APP);

			// INTO clause contains variables prefixed with :
			for(ListwItem *i = into_cols.GetFirst(); i != NULL; i = i->next)
				ConvertDeclaredIdentifierInSql((Token*)i->value);
			
			APPEND_NOFMT(GetLastToken(), ";");

			exists = true;
			continue;
		}
		else
		// OPEN cursor
		if(TOKEN_CMP(next, "OPEN"))
		{
			// Cursor name
			Token *cur  = GetNextToken();
			APPEND_NOFMT(cur, ";");

			exists = true;
			continue;
		}
						
		PushBack(next);
		break;
	}

	// END-EXEC. at the end 
	if(exists)
	{
		Token *end_exec = TOKEN_GETNEXTW("END-EXEC");
		Token *dot = NULL;

		// END-EXEC. can be taken as part of SQL parsing
		if(end_exec == NULL)
			end_exec = TOKEN_GETNEXTW("END-EXEC.");
		else
			dot = TOKEN_GETNEXT('.');

		if(_target_app != APP_COBOL)
		{
			Token::Remove(exec, sql);
			Token::Remove(end_exec, dot);
		}
	}

	return exists;
}

// COPY statement
bool Cobol::ParseCopyStatement(Token *copy)
{
	if(copy == NULL)
		return false;

	// Copy book
	/*Token *name */ (void) GetNextToken();

	/*Token *dot */ (void) GetNextCharToken('.', L'.');

	// Comment for PL/SQL
	if(_target_app == APP_PLSQL)
		Prepend(copy, "-- ", L"-- ", 3);

	return true;
}

// IF statement
bool Cobol::ParseIfStatement(Token *if_)
{
	if(if_ == NULL)
		return false;

	ParseBooleanExpression(SQL_BOOL_IF, if_);

	Token *last_condition = GetLastToken();
	Token *then = TOKEN_GETNEXTW("THEN");

	// In some syntaxes THEN can be skipped
	if(then != NULL)
	{
	}
	else
	{
		// Add THEN for Oracle, MariaDB, MySQL
		if(Target(SQL_ORACLE, SQL_MYSQL, SQL_MARIADB))
			APPEND_FMT(last_condition, " THEN", if_);

		// Parse single statement, it can be a chain of statements until .
		ParseStatement(CBL_SCOPE_IF_TRUE);
	}

	// Now check is ELSE is specified
	Token *else_ = TOKEN_GETNEXTW("ELSE");

	if(else_ != NULL)
	{
		ParseStatement();
	}

	Token *last = GetLastToken();

	// END IF
	Token *end = TOKEN_GETNEXTW("END");

	// Optional for some syntaxes
	if(end == NULL)
	{
		if(Target(SQL_ORACLE, SQL_MYSQL, SQL_MARIADB))
			APPEND_FMT(last, "\nEND IF;", if_);
	}

	return true;
}

// MOVE statement
bool Cobol::ParseMoveStatement(Token *move)
{
	if(move == NULL)
		return false;

	Token *exp = GetNextToken();
	ParseExpression(exp);
	Token *exp_end = GetLastToken();

	Token *to = TOKEN_GETNEXTW("TO");
	
	if(to == NULL)
		return false;

	Token *name = ConvertDeclaredIdentifier(GetNextToken());

	if(Target(SQL_ORACLE))
	{
		PrependCopy(exp, name);
		PREPEND_NOFMT(exp, " := ");
		APPEND_NOFMT(exp_end, ";");

		Token::Remove(move);
		Token::Remove(to, name);
	}

	return true;
}

// Add variable
void Cobol::AddVariable(Token *name)
{
	if(name == NULL)
		return;

	ConvertIdentifier(name, SQL_IDENT_VAR);
	_sqlparser->AddVariable(name);
}

// Add parameter
void Cobol::AddParameter(Token *name)
{
	if(name == NULL)
		return;

	ConvertIdentifier(name, SQL_IDENT_PARAM);
	_sqlparser->AddParameter(name);
}

// Convert variable name
void Cobol::ConvertIdentifier(Token *name, int type)
{
	if(name == NULL || name->str == NULL)
		return;

	const char *cur = name->str;
	int64_t new_len = name->len + 2;

	char *ident = new char[new_len + 1];

	// Add v_ or p_ prefix to avoid conflicts with column names as COBOL uses :var syntax in SQL
	if(type == SQL_IDENT_PARAM)
		ident[0] = 'p';
	else
		ident[0] = 'v';

	ident[1] = '_';

	// Replace - with _
	for(int64_t i = 0; i < name->len; i++)
	{
		if(cur[i] == '-')
			ident[i + 2] = '_';
		else
			ident[i + 2] = cur[i];
	}

	ident[new_len] = '\x0';

	name->t_str = ident;
	name->t_len = new_len;
}

// Convert previously declared variable or parameter
Token* Cobol::ConvertDeclaredIdentifier(Token *name)
{
	if(name == NULL)
		return NULL;

	// Check if a variable exists with this name
	Token *var = _sqlparser->GetVariable(name);

	// Change only if it was changed at the declaration
	if(var != NULL && var->t_len != 0)
		_sqlparser->ConvertVariableIdentifier(name, var);

	Token *param = NULL;

	// Check if a parameter exists with this name
	if(var == NULL)
		param = _sqlparser->GetParameter(name);

	// Change only if it was changed at the declaration
	if(param != NULL && param->t_len != 0)
		_sqlparser->ConvertParameterIdentifier(name, param);

	return name;
}

// Convert variable or parameter referenced in SQL (should contain : prefix)
Token* Cobol::ConvertDeclaredIdentifierInSql(Token *name)
{
	if(name == NULL || name->str == NULL || name->str[0] != ':')
		return name;

	// Remove prefix :
	Token *name2 = Token::GetCopy(name);
	name2->str++;
	name2->len--;

	// Check if a variable exists with this name
	Token *var = _sqlparser->GetVariable(name2);

	// Change only if it was changed at the declaration
	if(var != NULL && var->t_len != 0)
		_sqlparser->ConvertVariableIdentifier(name, var);

	Token *param = NULL;

	// Check if a parameter exists with this name
	if(var == NULL)
		param = _sqlparser->GetParameter(name2);

	// Change only if it was changed at the declaration
	if(param != NULL && param->t_len != 0)
		_sqlparser->ConvertParameterIdentifier(name, param);	

	return name;
}

// Clear the program scope
void Cobol::ClearScope()
{
	_div_scope = 0;
	_current_top_record = NULL;
	
	_convert_to_proc = false;

	_proc_start = NULL;
	_proc_name = NULL;
	_proc_using_params.DeleteAll();
	_proc_linkage_params.DeleteAll();
}

Token* Cobol::GetNextToken(Token *prev)
{
	if(_sqlparser != NULL) 
		return _sqlparser->GetNextToken(prev); 
	else 
		return NULL; 
}

Token* Cobol::GetNextCharToken(const char ch, const wchar_t wch)
{ 
	if(_sqlparser != NULL) 
		return _sqlparser->GetNextCharToken(ch, wch); 
	else 
		return NULL; 
}

Token* Cobol::GetNextWordToken(const char *str, const wchar_t *wstr, int64_t len) 
{ 
	if(_sqlparser != NULL) 
		return _sqlparser->GetNextWordToken(str, wstr, len); 
	else 
		return NULL; 
}

Token* Cobol::GetNextUntilNewlineToken()
{ 
	if(_sqlparser != NULL) 
		return _sqlparser->GetNextUntilNewlineToken(); 
	else 
		return NULL; 
}

Token* Cobol::Append(Token *token, const char *str, const wchar_t *wstr, int64_t len, Token *format)
{
	if(_sqlparser != NULL) 
		return _sqlparser->Append(token, str, wstr, len, format);
	else
		return NULL;
}

Token* Cobol::AppendCopy(Token *token, Token *append)
{
	if(_sqlparser != NULL) 
		return _sqlparser->AppendCopy(token, append);

	return NULL;
}

Token* Cobol::Prepend(Token *token, const char *str, const wchar_t *wstr, int64_t len, Token *format)
{
	if(_sqlparser != NULL) 
		return _sqlparser->Prepend(token, str, wstr, len, format);
	else
		return NULL;
}

Token* Cobol::PrependCopy(Token *token, Token *prepend)
{
	if(_sqlparser != NULL) 
		return _sqlparser->PrependCopy(token, prepend);
	else
		return NULL;
}

Token* Cobol::PrependCopy(Token *token, Token *first, Token *last, bool prepend_removed)
{
	if(_sqlparser != NULL) 
		return _sqlparser->PrependCopy(token, first, last, prepend_removed);
	else
		return NULL;
}
