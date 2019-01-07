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

// COBOL Class

#ifndef sqlines_cobol_h
#define sqlines_cobol_h

// Section scope
#define CBL_SCOPE_LINKAGE_SECTION  1

// Statement scope
#define CBL_SCOPE_IF_TRUE   1

//#define CBL_GETNEXTW(string)     _sqlparser->GetNext(string, L##string, sizeof(string) - 1)

class SqlParser;
class Token;

class Cobol
{
	SqlParser *_sqlparser;

	// Source and target SQL dialects
	short _source;
	short _target;

	// Target application type
	short _target_app;

	// Current top level record
	Token *_current_top_record;

	// Conversion to package/class or standalone procedure
	bool _convert_to_proc;

	Token *_proc_start;
	Token *_proc_name;
	ListW _proc_using_params;	
	ListWM _proc_linkage_params;	

public:
	Cobol();

	// Current division (identification i.e.)
	int _div_scope;

	bool ParseStatement(Token *token);

	bool ParseIdentificationDivision(Token *identification);
	
	bool ParseEnvironmentDivision(Token *environment);
	bool ParseEnvironmentInputOutputSection(Token *inputoutput);
	bool ParseEnvironmentInputOutputSectionFileControl(Token *filecontrol);
	
	bool ParseDataDivision(Token *data);
	bool ParseDataFileSection(Token *file);	
	bool ParseDataWorkingStorageSection(Token *working_storage);
	bool ParseDataLinkageSection(Token *linkage);

	// Parse variable definitions
	bool ParseDataSection(int section_scope = 0);
	bool ParseVariableSection(int section_scope, Token *parent_record = NULL, ListWM *record_items = NULL);
	bool ParseRecordDeclaration(int section_scope, Token *parent_record, Token *num, Token *name, Token *dot);
	bool ParseVariableDeclaration(int section_scope, Token *num, Token *name, ListWM *record_items = NULL);
	bool ParsePicFormat(Token *pic, Token **type = NULL, Token **len = NULL, Token **var_end = NULL);
	bool ParseDb2RecordDeclaration(int section_scope, Token *num, Token *name, Token *dot, Token *end_dot, ListWM *record_items);

	bool ParseProcedureDivision(Token *procedure);
	void ParseBlock();
	void ParseStatement(int stmt_scope = 0);

	// Statements
	bool ParseCopyStatement(Token *copy);
	bool ParseExecSqlStatement(Token *exec);
	bool ParseIfStatement(Token *if_);
	bool ParseMoveStatement(Token *move);

	// Add variable and parameter
	void AddVariable(Token *name);
	void AddParameter(Token *name);

	// Convert variable or parameter name
	void ConvertIdentifier(Token *name, int type = 0);
	Token* ConvertDeclaredIdentifier(Token *name);
	Token* ConvertDeclaredIdentifierInSql(Token *name);

	// Clear the program scope
	void ClearScope(); 

	void SetParser(SqlParser *parser) { _sqlparser = parser; }
	void SetTypes(short s, short t, short ta) { _source = s; _target = t; _target_app = ta; }

	// SqlParser helpers
	Token* GetNext(const char ch, const wchar_t wch) { return _sqlparser->GetNext(ch, wch); }
	Token* GetNext(const char *str, const wchar_t *wstr, int64_t len) { return _sqlparser->GetNext(str, wstr, len); }
	Token* GetNextToken() { return _sqlparser->GetNextToken(); }
	Token* GetNextToken(Token *prev);
	Token* GetNextCharToken(const char ch, const wchar_t wch);
	Token* GetNextWordToken(const char *str, const wchar_t *wstr, int64_t len);
	Token* GetNextUntilNewlineToken();

	bool ParseExpression(Token *token, int prev_operator = 0) { return _sqlparser->ParseExpression(token, prev_operator); }
	bool ParseBooleanExpression(int scope, Token *stmt_start = NULL) { return _sqlparser->ParseBooleanExpression(scope, stmt_start); } 

	bool Source(short s1, short s2 = -1, short s3 = -1, short s4 = -1, short s5 = -1, short s6 = -1) { return _sqlparser->Source(s1, s2, s3, s4, s5, s6); } 
	bool Target(short t1, short t2 = -1, short t3 = -1, short t4 = -1, short t5 = -1, short t6 = -1) { return _sqlparser->Target(t1, t2, t3, t4, t5, t6); } 

	Token* Nvl(Token *first, Token *second, Token *third = NULL, Token *fourth = NULL) { return _sqlparser->Nvl(first, second, third, fourth); }

	void PushBack(Token *token) { return _sqlparser->PushBack(token); }
	Token *GetLastToken() { return _sqlparser->GetLastToken(); }
	Token *LookNext(const char *str, const wchar_t *wstr, int64_t len) { return _sqlparser->LookNext(str, wstr, len); }

	Token* Append(Token *token, const char *str, const wchar_t *wstr, int64_t len, Token *format = NULL);
	Token* AppendCopy(Token *token, Token *append);
	void Append(Token *token, TokenStr *str, Token *format = NULL) { return _sqlparser->Append(token, str, format); }
	Token* AppendNoFormat(Token *token, const char *str, const wchar_t *wstr, int64_t len) { return _sqlparser->AppendNoFormat(token, str, wstr, len); }
	void AppendFirstNoFormat(Token *token, const char *str, const wchar_t *wstr, int64_t len) { _sqlparser->AppendFirstNoFormat(token, str, wstr, len); }

	Token* Prepend(Token *token, const char *str, const wchar_t *wstr, int64_t len, Token *format = NULL);
	Token* PrependCopy(Token *token, Token *prepend);
	Token* PrependCopy(Token *token, Token *first, Token *last, bool prepend_removed = true);
	Token* PrependNoFormat(Token *token, const char *str, const wchar_t *wstr, int64_t len) { return _sqlparser->PrependNoFormat(token, str, wstr, len); }
};

#endif // sqlines_cobol_h
