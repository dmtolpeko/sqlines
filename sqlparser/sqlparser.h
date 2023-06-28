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

// SQLParser Class

#ifndef sqlines_sqlparser_h
#define sqlines_sqlparser_h

#include <stdio.h>
#include <map>
#include "token.h"
#include "stats.h"
#include "report.h"
#include "listt.h"
#include "listw.h"
#include "listwm.h"
#include "doc.h"
#include "java.h"

// Conversion level
#define LEVEL_APP			1
#define LEVEL_SQL			2
#define LEVEL_STR			3

// SQL dialect types
#define SQL_SQL_SERVER			1
#define SQL_ORACLE				2
#define SQL_DB2					3
#define SQL_MYSQL				4
#define SQL_POSTGRESQL			5
#define SQL_SYBASE				6
#define SQL_INFORMIX			7
#define SQL_GREENPLUM			8
#define SQL_SYBASE_ASA			9
#define SQL_TERADATA			10
#define SQL_NETEZZA				11
#define SQL_MARIADB             12
#define SQL_HIVE                13
#define SQL_REDSHIFT            14
#define SQL_ESGYNDB             15
#define SQL_SYBASE_ADS          16
#define SQL_MARIADB_ORA         17

// Application types
#define APP_JAVA				1
#define APP_CS					2
#define APP_COBOL				3
#define APP_PLSQL				4

// SQL object scope
#define SQL_SCOPE_FREE			0
#define SQL_SCOPE_PROC			1
#define SQL_SCOPE_FUNC			2
#define SQL_SCOPE_TRIGGER		3
#define SQL_SCOPE_TABLE			4
#define SQL_SCOPE_TEMP_TABLE	5
#define SQL_SCOPE_INDEX			6
#define SQL_SCOPE_TABLESPACE	7

// Procedural block types
#define SQL_BLOCK_PROC			1
#define SQL_BLOCK_IF			2
#define SQL_BLOCK_WHILE			3
#define SQL_BLOCK_HANDLER		4
#define SQL_BLOCK_CASE			5
#define SQL_BLOCK_FOR			6
#define SQL_BLOCK_LOOP			7
#define SQL_BLOCK_CHECK			8
#define SQL_BLOCK_REPEAT		9
#define SQL_BLOCK_BEGIN			10
#define SQL_BLOCK_EXCEPTION		11
#define SQL_BLOCK_TRY           12
#define SQL_BLOCK_CATCH         13
#define SQL_BLOCK_AUTONOMOUS    14

// Boolean expression scope
#define SQL_BOOL_IF					1
#define SQL_BOOL_IF_EXP				2
#define SQL_BOOL_CASE				3
#define SQL_BOOL_CHECK				4
#define SQL_BOOL_JOIN_ON			5
#define SQL_BOOL_WHERE				6
#define SQL_BOOL_TRIGGER_WHEN		7
#define SQL_BOOL_WHILE				8
#define SQL_BOOL_EXISTS				9
#define SQL_BOOL_UNTIL				10
#define SQL_BOOL_HAVING				11
#define SQL_BOOL_CREATE_RULE		12
#define SQL_BOOL_ASSIGN             13
#define SQL_BOOL_CONNECT_BY         14
#define SQL_BOOL_START_WITH         15
#define SQL_BOOL_MERGE              16

// SQL clause scope
#define SQL_SCOPE_ASSIGNMENT_RIGHT_SIDE		1
#define SQL_SCOPE_CASE_FUNC					2
#define SQL_SCOPE_CURSOR_PARAMS				3
#define SQL_SCOPE_FUNC_PARAMS				4
#define SQL_SCOPE_INSERT_VALUES				5
#define SQL_SCOPE_PROC_PARAMS				6
#define SQL_SCOPE_SELECT_STMT				7
#define SQL_SCOPE_TAB_COLS					8
#define SQL_SCOPE_TRG_WHEN_CONDITION		9
#define SQL_SCOPE_VAR_DECL					10
#define SQL_SCOPE_XMLSERIALIZE_FUNC			11
#define SQL_SCOPE_SP_ADDTYPE				12
#define SQL_SCOPE_CONVERT_FUNC				13
#define SQL_SCOPE_CAST_FUNC					14
#define SQL_SCOPE_OBJ_TYPE_DECL             15
#define SQL_SCOPE_FUNC_RETURN_DECL          16

// SQL SELECT statement scope
#define SQL_SEL_INSERT					1	
#define SQL_SEL_CURSOR					2		
#define SQL_SEL_OPEN					3
#define SQL_SEL_FROM					4
#define SQL_SEL_EXP						5
#define SQL_SEL_SET_UNION				6
#define SQL_SEL_FOR						7
#define SQL_SEL_IN_PREDICATE			8
#define SQL_SEL_EXISTS_PREDICATE		9
#define SQL_SEL_SELECT_LIST				10
#define SQL_SEL_FOREACH					11
#define SQL_SEL_UPDATE_SET				12
#define SQL_SEL_UPDATE_FROM				13
#define SQL_SEL_JOIN					14
#define SQL_SEL_CREATE_TEMP_TABLE_AS	15
#define SQL_SEL_EXPORT					16
#define SQL_SEL_WITH_CTE				17
#define SQL_SEL_VIEW					18
#define SQL_SEL_MERGE					19

// SQL Statement scope
#define SQL_STMT_ALTER_TABLE            1
#define SQL_STMT_SELECT					2
#define SQL_STMT_INSERT					3
#define SQL_STMT_UPDATE					4
#define SQL_STMT_DELETE					5

// Operators
#define SQL_OPERATOR_CONCAT		1
#define SQL_OPERATOR_PLUS		2

// Expected identifier type
#define SQL_IDENT_OBJECT		    1
#define SQL_IDENT_COLUMN		    2
#define SQL_IDENT_COLUMN_SINGLE	    3
#define SQL_IDENT_COLVAR		    4
#define SQL_IDENT_PARAM			    5
#define SQL_IDENT_VAR			    6

// Bookmark types
#define BOOK_CT_START			1			// CREATE TABLE start
#define BOOK_CT_END				2			// CREATE TABLE end
#define BOOK_CTC_END			3			// CREATE TABLE column definition end
#define BOOK_CTC_ALL_END		4			// CREATE TABLE end of all column definitions, pointer to ) i.e. before storage and other properties
#define BOOK_CI_START			5			// CREATE INDEX start
#define BOOK_CI_END				6			// CREATE INDEX end
#define BOOK_USER_EXCEPTION		7			// User-defined exception

// Cope, Paste and Cut scopes
#define COPY_SCOPE_PROC		1			// Procedure, function, trigger or outer anonymous block

// Cope, Paste and Cut types
#define COPY_EXIT_HANDLER_NOT_FOUND				1
#define COPY_CONTINUE_HANDLER_NOT_FOUND			2
#define COPY_EXIT_HANDLER_SQLEXCEPTION			3
#define COPY_CONTINUE_HANDLER_SQLEXCEPTION		4
#define COPY_EXIT_HANDLER_USER_DEFINED			5
#define COPY_CONTINUE_HANDLER_USER_DEFINED		6
#define COPY_CURSOR_WITH_RETURN					7
#define COPY_EXIT_HANDLER_FOR_SQLSTATE			8
#define COPY_CONTINUE_HANDLER_FOR_SQLSTATE		9

#define TOKEN_GETNEXT(chr)           GetNext(chr, L##chr)
#define TOKEN_GETNEXTP(prev, chr)    GetNext(prev, chr, L##chr)
#define TOKEN_GETNEXTW(string)       GetNext(string, L##string, sizeof(string) - 1)
#define TOKEN_GETNEXTWP(prev, string) GetNext(prev, string, L##string, sizeof(string) - 1)
#define TOKEN_CMPC(token, chr)		 Token::Compare(token, chr, L##chr)
#define TOKEN_CMPCP(token, chr, pos) Token::Compare(token, chr, L##chr, pos)
#define TOKEN_CMP(token, string)     Token::Compare(token, string, L##string, sizeof(string) - 1)
#define TOKEN_CMP_PART0(token, string) Token::Compare(token, string, L##string, 0, sizeof(string) - 1)
#define TOKEN_CHANGE(token, string)  Token::Change(token, string, L##string, sizeof(string) - 1)
#define TOKEN_CHANGE_FMT(token, string, format)  Token::Change(token, string, L##string, sizeof(string) - 1, format)
#define TOKEN_CHANGE_NOFMT(token, string)  Token::ChangeNoFormat(token, string, L##string, sizeof(string) - 1)

#define APPEND(token, string) Append(token, string, L##string, sizeof(string) - 1)
#define APPEND_FMT(token, string, format) Append(token, string, L##string, sizeof(string) - 1, format)
#define APPEND_FIRST_FMT(token, string, format) AppendFirst(token, string, L##string, sizeof(string) - 1, format)
#define APPEND_FIRST_NOFMT(token, string) AppendFirstNoFormat(token, string, L##string, sizeof(string) - 1)
#define APPEND_NOFMT(token, string) AppendNoFormat(token, string, L##string, sizeof(string) - 1)
#define APPENDS(token, tokenstr) Append(token, tokenstr)

#define APPENDSTR(tokenstr, string) tokenstr.Append(string, L##string, sizeof(string) - 1)

#define PREPEND(token, string) Prepend(token, string, L##string, sizeof(string) - 1)
#define PREPEND_FMT(token, string, format) Prepend(token, string, L##string, sizeof(string) - 1, format)
#define PREPEND_NOFMT(token, string) PrependNoFormat(token, string, L##string, sizeof(string) - 1)

#define LOOKNEXT(string) LookNext(string, L##string, sizeof(string) - 1)

#define COMMENT(string, start, end) Comment(string, L##string, sizeof(string) - 1, start, end) 
#define COMMENT_WARN(start, end) COMMENT("Warning: ", start, end) 

typedef std::map<std::string, std::string> StringMap;
typedef std::pair<std::string, std::string> StringMapPair;

class Cobol;

// Bookmark element
struct Book
{
	int type;
	Token *name;
	Token *name2;
	Token *book;

	int data1;

	bool distributed_by;		// DISTRIBUTE BY was added in CREATE TABLE for Greenplum

	Book *prev;
	Book *next;

	Book() 
	{ 
		type = 0; name = NULL; name2 = NULL; book = NULL; data1 = 0; prev = NULL; next = NULL; 
		distributed_by = false;
	}
};

// Copy, Paste and Cut element
struct CopyPaste
{
	int scope;
	int type;
	Token *name;
	ListT<Token> tokens;

	CopyPaste *next;
	CopyPaste *prev;

	CopyPaste() { scope = 0; type = 0; name = NULL; next = NULL; prev = NULL; }
};

// Metadata information
class Meta
{
public:
    // Table, view, procedure name
    std::string object;
    // Column or parameter
	std::string column;
    // Data type
    std::string dtype;

    Meta(std::string o, std::string c, std::string dt) 
    { 
        object = o; column = c; dtype = dt; prev = next = NULL; 
    }

    Meta *prev;
	Meta *next;
};

class SqlParser
{
	// Source and target SQL dialects
	short _source;
	short _target;

	// Source and target application types
	short _source_app;
	short _target_app;

	// Current conversion level (application, SQL code, dynamic string i.e.)
	int _level;

    // Current object scope (table, view, procedure etc.)
    int _obj_scope;
    // Current statement scope (CREATE TABLE, CREATE VIEW, ALTER TABLE etc.)
    int _stmt_scope;

	// Currently converted input
	const char *_start;
	const char *_next_start;
	int _size;
	int _remain_size;
	int _line;

	// Input tokens
	ListT<Token> _tokens;

	// Bookmarks
	ListT<Book> _bookmarks;
	// Copy, Paste and Cut blocks
	ListT<CopyPaste> _copypaste;

    // Metadata information
    std::map<std::string, ListT<Meta>*> _meta;

	// Scope list
	ListWM _scope;

	// Current procedural scope (procedure, function, trigger i.e.)
	int _spl_scope;

	// Name of current procedure (function, trigger)
	Token *_spl_name;
	// Start token of procedure, function or trigger (usually it is CREATE keyword)
	Token *_spl_start;

	// External stored procedure written in C, Java, COBOL etc.
	bool _spl_external;

	// Current variables and parameters
	ListW _spl_variables;
	ListW _spl_parameters;

	// Outer BEGIN keyword
    Token *_spl_outer_begin;
    // Standalone BEGIN levels in the current procedure (each BEGIN entry is cleared on block exit)
    ListW _spl_begin_blocks;

	// Outer AS or IS keyword
    Token *_spl_outer_as;

    // Last declare statement before the first non-declare statement
	Token *_spl_last_declare;
    // First non-declare statement in procedure or function
	Token *_spl_first_non_declare;

	// Current statement in procedure or function
	Token *_spl_current_stmt;
	// Last statement in procedure or function
	Token *_spl_last_stmt;

    // Last token of last declared variable (not cursor) in the outer PL/SQL DECLARE block
    Token *_spl_last_outer_declare_var;
	Token *_spl_last_outer_declare_varname;
    
	// Number of result sets returned from procedure
	int _spl_result_sets;
	// Result set cursor names (declared cursor with return)
	ListW _spl_result_set_cursors;
	// Result set generated cursor names (for standalone SELECT returning a result set)
	ListT<TokenStr> _spl_result_set_generated_cursors;

	// Number of REFCURSOR parameters in procedure (Oracle, PostgreSQL)
	int _spl_refcursor_params_num;
	// REFCURSOR parameter names
	ListW _spl_refcursor_params;
	
	// Delimiter is set at the end of procedure
	bool _spl_delimiter_set;
	// User-defined exceptions and condition handlers
	ListWM _spl_user_exceptions;

	// NEW and OLD correlation name in trigger
	Token *_spl_new_correlation_name;
	Token *_spl_old_correlation_name;
	// Referenced NEW and OLD columns
	ListW _spl_tr_new_columns;
	ListW _spl_tr_old_columns;

	// Outer block label in DB2 procedure
	Token *_spl_outer_label;
	// Oracle PL/SQL cursor parameters
	ListWM _spl_cursor_params;
	// Variables generated for Oracle PL/SQL parameters
	ListWM _spl_cursor_vars;
	// The names and definitions of declared cursors
	ListW _spl_declared_cursors;
	ListW _spl_declared_cursors_using_vars;
	ListWM _spl_declared_cursors_stmts;
	// Current declaring cursor
	Token *_spl_current_declaring_cursor;
	// Current declaring cursor uses procedural variables
	bool _spl_current_declaring_cursor_uses_vars;
	// Cursor for which updates WHERE CURRENT OF is performed
	ListW _spl_updatable_current_of_cursors;
	// Declared cursors and their SELECT statements
	ListWM _spl_declared_cursors_select;
	// Declared cursors and their select list expressions
	ListWM _spl_declared_cursors_select_first_exp;
	ListWM _spl_declared_cursors_select_exp;

	// The current PL/SQL package
	Token *_spl_package_spec;
	Token *_spl_package;
	// Declared record variables %ROWTYPE
	ListW _spl_rowtype_vars;
	// Referenced %ROWTYPE record fields
	ListWM _spl_rowtype_fields;
	// Fetch into record referenced
	ListW _spl_rowtype_fetches;
	// TYPE name OF TABLE datatype
	ListWM _spl_obj_type_table;

	// Implicit rowtype created by for cursor loops
	ListWM _spl_implicit_rowtype_vars;
	// Implicit rowtype fields
	ListWM _spl_implicit_rowtype_fields;
	// Generated FETCH statements to emulate cursor loops
	ListWM _spl_implicit_rowtype_fetches;

	// Declared local temporary tables, table variables
	ListW _spl_declared_local_tables;
	// Created session temporary tables
	ListW _spl_created_session_tables;
	// Temporary table replacing their content each time they are declared (DB2 WITH REPLACE)
	ListW _spl_declared_tables_with_replace;

	// Closing parenthesis ) after parameter list
	Token *_spl_param_close;
	
	// RETURNS, RETURN, RETURNING keyword
	Token *_spl_returns;
	// RETURNING data types in Informix
	ListW _spl_returning_datatypes;
	// End of RETURNING clause in Informix
	Token *_spl_returning_end;
	// Generated OUT variable names for Informix RETURNING clause
	ListWM _spl_returning_out_names;
	// Function returns integer data type
	bool _spl_return_int;

	// Number of FOREACH statements in the current procedure (function, trigger)
	int _spl_foreach_num;
	// Number of RETURN WITH RESUME in procedure
	int _spl_return_with_resume;
	// Number of RETURN statements in procedure
	int _spl_return_num;

	// Table name from last INSERT statement
	Token *_spl_last_insert_table_name;
	// Cursor name from last OPEN statement
	Token *_spl_last_open_cursor_name;
	// Cursor name from last FETCH statement
	Token *_spl_last_fetch_cursor_name;
	// List of OPEN cursor statemenents 
	ListWM _spl_open_cursors;

	// Prepared statements id
	ListWM _spl_prepared_stmts;
	// Cursors for prepared statements
	ListWM _spl_prepared_stmts_cursors;
	ListWM _spl_prepared_stmts_cursors_with_return;

	// Fields of implicit records created by FOR loops
	ListWM _spl_implicit_record_fields;
	
	// Declared result set locators in DB2
	ListW _spl_declared_rs_locators;
	// Result set locators associated with procedures
	ListWM _spl_rs_locator_procedures;
	// Cursors allocated for result set locators in DB2
	ListWM _spl_rs_locator_cursors;

    // Loops level in the current procedure (each loop entry is cleared on loop exit)
    ListW _spl_loops;
    // Number of loop labels already generated in the current procedure
    int _spl_loop_labels;

	// Stored procedures calls inside procedural block
	ListWM _spl_sp_calls;

	// Number of SELECT statements moved out of IF boolean condition
	int _spl_moved_if_select;

	// Procedure converted to function
	bool _spl_proc_to_func;
	// Function converted to procedure
	bool _spl_func_to_proc;
    // Handler for NOT FOUND condition
    bool _spl_not_found_handler;
	bool _spl_need_not_found_handler;

	// Variable is added to hold row count value
	bool _spl_need_rowcount_var;

	// Variable is added to hold dynamic SQL (in SQL Server, it is @sql for sp_executesql that does not allow expressions)
	bool _spl_dyn_sql_var;

	// Monday is 1 day, Sunday is 7 (false if it is unknown from context)
	bool _spl_monday_1;

	// User-defined data types
	ListWM _udt;
	// Domain rules in Sybase ASE
	ListWM _domain_rules;

	// Number of SELECT statements in the current outer expression
	int _exp_select;

	// Formatting tokens for newly generated code
	Token *_declare_format;

	// Push back token
	Token *_push_back_token;

	// Options
	bool _option_rems;
	std::string _option_oracle_plsql_number_mapping;
	std::string _option_set_explicit_schema;
	std::string _option_cur_file;
	bool _option_eval_mode;

	// Mappings
	StringMap _object_map;
	StringMap _schema_map;
	StringMap _func_to_sp_map;

    // Statistics and report
    Stats *_stats;
    Report *_report;

	// Application scope
	Java *_java;
	Cobol *_cobol;

public:
	SqlParser();
	~SqlParser();

	// Set source and target types
	void SetTypes(short source, short target) { _source = source; _target = target; }
    // Set target programming language
    void SetLang(const char *value, bool source);
	// Set option
	void SetOption(const char *option, const char *value);

	// Perform conversion
	int Convert(const char *input, int size, const char **output, int *out_size, int *lines);

	// Generate output
	void CreateOutputString(const char **output, int *out_size);

	// Post conversion when all tokens processed
	void Post();

	// Get next token from the input
	Token* GetNextToken();
	Token* GetNextToken(Token *prev);
	Token* GetNextIdentToken(int expected_type = 0, int scope = 0);
	Token* GetNextCharToken(const char ch, const wchar_t wch);
	Token* GetNextCharToken(Token *prev, const char ch, const wchar_t wch);
	Token* GetNextCharOrLastToken(const char ch, const wchar_t wch);
	Token* GetNextPlusMinusAsOperatorToken(const char ch, const wchar_t wch);
	Token* GetNextSpecialCharToken(Token *prev, const char *str, const wchar_t *wstr, size_t len);
	Token* GetNextSpecialCharToken(const char *str, const wchar_t *wstr, size_t len);
	Token* GetNextWordToken(const char *str, const wchar_t *wstr, size_t len);
	Token* GetNextWordToken(Token *prev, const char *str, const wchar_t *wstr, size_t len);
	Token* GetNextUntilNewlineToken();
	Token* GetNextNumberToken();
	Token* GetNextNumberToken(Token *prev);
	Token* GetNextStringToken();
	Token* GetPrevToken(Token *token);
	Token* GetLastToken();
	Token* GetLastToken(Token *last);
	Token* GetVariable(Token *name);
	Token* GetParameter(Token *name);
	Token* GetVariableOrParameter(Token *name);
	Token* GetBomToken();

	Token* GetNext() { return GetNextToken(); }
	Token* GetNext(Token *prev) { return GetNextToken(prev); }
	Token* GetNext(const char *str, const wchar_t *wstr, size_t len) { return GetNextWordToken(str, wstr, len); }
	Token* GetNext(Token *prev, const char *str, const wchar_t *wstr, size_t len) { return GetNextWordToken(prev, str, wstr, len); }
	Token* GetNext(const char ch, const wchar_t wch) { return GetNextCharToken(ch, wch); }
	Token* GetNext(Token *prev, const char ch, const wchar_t wch) { return GetNextCharToken(prev, ch, wch); }

	// Handle local block variables
	void EnterLocalVariablesBlock();
	void LeaveLocalVariablesBlock();

	void AddVariable(Token *name) { _spl_variables.Add(name); }
	void AddParameter(Token *name) { _spl_parameters.Add(name); }

	// Return the last fetched token to the input
	void PushBack(Token *token);
	// Check next token for the specific value but do not fecth it from the input
	Token *LookNext(const char *str, const wchar_t *wstr, size_t len);

	// Append the token with the specified value
	Token *Append(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format = NULL);
    Token *AppendWithSpaceAfter(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format = NULL);
	void Append(Token *token, int value);
	void Append(Token *token, TokenStr *str, Token *format = NULL);
    void AppendFirst(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format = NULL);
	Token* AppendNoFormat(Token *token, const char *str, const wchar_t *wstr, size_t len);
	void AppendNoFormat(Token *token, TokenStr *str);
	void AppendFirstNoFormat(Token *token, const char *str, const wchar_t *wstr, size_t len);
	Token* AppendCopy(Token *token, Token *append);
	Token* AppendCopy(Token *token, Token *first, Token *last, bool append_removed = true);
	void AppendSpaceCopy(Token *token, Token *append);
	void AppendSpaceCopy(Token *token, Token *first, Token *last, bool append_removed = true);
    void AppendNewlineCopy(Token *token, Token *first, Token *last, size_t newlines = 1, bool append_removed = true);
	void Append(Token *token, Token *append);
	
	// Prepend the token with the specified value
	Token* Prepend(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format = NULL);
	Token* Prepend(Token *token, TokenStr *str, Token *format = NULL);
	Token* PrependNoFormat(Token *token, const char *str, const wchar_t *wstr, size_t len);
	Token* PrependNoFormat(Token *token, TokenStr *str);
	void PrependFirstNoFormat(Token *token, const char *str, const wchar_t *wstr, size_t len);
	Token* PrependCopy(Token *token, Token *prepend);
	Token* PrependCopy(Token *token, Token *first, Token *last, bool prepend_removed = true);
	void PrependSpaceCopy(Token *token, Token *first, Token *last = NULL, bool prepend_removed = true);
	void Prepend(Token *token, Token *prepend);

	void ChangeWithSpacesAround(Token *token, const char *new_str, const wchar_t *new_wstr, size_t len, Token *format = NULL);
	
	// Check for the source and target type
	bool Source(short s1, short s2 = -1, short s3 = -1, short s4 = -1, short s5 = -1, short s6 = -1); 
	bool Target(short t1, short t2 = -1, short t3 = -1, short t4 = -1, short t5 = -1, short t6 = -1); 

	// Return first not NULL
	Token* Nvl(Token *first, Token *second, Token *third = NULL, Token *fourth = NULL);
    Token* NvlLast(Token *first);

	// Enter, leave and check the specified scope
	void Enter(int scope);
	void Leave(int scope);
	bool IsScope(int scope, int scope2 = 0);
	
	// Save and get bookmark
	Book* Bookmark(int type, Token *name, Token *book);
	Book* Bookmark(int type, Token *name, Token *name2, Token *book);

	Book* GetBookmark(int type, Token *name); 
	Book* GetBookmark(int type, Token *name, Token *name2); 
	Book* GetBookmark2(int type, Token *name); 

	// Copy, Paste and Cut operations
	void Cut(int scope, int type, Token *name, Token *start, Token *end);
	void ClearCopy(int scope);

	// Actions after converting each procedure, function or trigger
	void SplPostActions();
	// Clear all procedural lists, statuses
	void ClearSplScope();

	// Get tokens
	void SkipSpaceTokens();
	bool GetSingleCharToken(Token *token);
	bool GetWordToken(Token *token);
	bool GetQuotedIdentifier(Token *token, bool starts_as_unquoted = false);
	bool GetStringLiteral(Token *token);
	
	// Parser functions
	void Parse(Token *token, int scope, int *result_sets);
	bool ParseStatement(Token *token, int scope, int *result_sets);
	bool ParseDataType(Token *type, int clause_scope = SQL_SCOPE_TAB_COLS);
	bool ParseTypedVariable(Token *var, Token *ref_type);
	bool ParseVarDataTypeAttribute();
    Token* ParseExpression();
	bool ParseExpression(Token *token, int prev_operator = 0);
	bool ParseBooleanExpression(int scope, Token *stmt_start = NULL, int *conditions_count = NULL, int *rowlimit = NULL, Token *prev_open = NULL, 
		bool *bool_operator_not_exists = NULL);
	bool ParseBooleanAndOr(int scope, Token *stmt_start, int *conditions_count, int *rowlimit);
	bool ParseInPredicate(Token *in);
	bool ParseOraclePackage(Token *token);
	bool ParseFunction(Token *token);
	bool ParseFunctionConstant(Token *token);
	bool ParseFunctionWithoutParameters(Token *token);
	bool ParseDatetimeLiteral(Token *token);
	bool ParseBooleanLiteral(Token *token);
    bool ParseNamedVarExpression(Token *token);
	bool ParseBlock(int type, bool frontier, int scope, int *result_sets);
	bool ParseComment();
	bool ParseLabelDeclaration(Token *word, bool outer_sp_block);
	bool ParseCaseExpression(Token *first);
	bool ParseIfExpression(Token *first);
	bool ParseAnalyticFunctionOverClause(Token *over);
	bool ParseSystemProcedure(Token *execute, Token *name);

	// Operators
	bool ParseAdditionOperator(Token *first, int prev_operator);
	bool ParseDivisionOperator(Token *first);
	bool ParseMultiplicationOperator(Token *first);
	bool ParseStringConcatenation(Token *first, int prev_operator);
	bool ParseConcatOperator(Token *first);
	bool ParseSubtractionOperator(Token *first);
	bool ParsePercentOperator(Token *first);
	bool ParseIndexOperator(Token *first);
	bool ParseCastOperator(Token *first);
	bool ParseModOperator(Token *first);
	bool ParseUnitsOperator(Token *first);
	bool ParseAddSubIntervalChain(Token *first, int prev_operator);

	// Data types
	bool ParseAutoIncType(Token *name);
	bool ParseBfileType(Token *name);
	bool ParseBigdatetimeType(Token *name);
	bool ParseBigintType(Token *name, int clause_scope);
	bool ParseBigserialType(Token *name);
	bool ParseBigtimeType(Token *name);
	bool ParseBinaryType(Token *name);
	bool ParseBinaryDoubleType(Token *name);
	bool ParseBinaryFloatType(Token *name);
	bool ParseBitType(Token *name);
	bool ParseBlobType(Token *name);
	bool ParseBooleanType(Token *name);
	bool ParseByteaType(Token *name);
	bool ParseByteType(Token *name);
	bool ParseByteintType(Token *name);
	bool ParseCharacterType(Token *name, int clause_scope);
	bool ParseCharType(Token *name, int clause_scope);
	bool ParseClobType(Token *name);
	bool ParseCurDoubleType(Token *name);
	bool ParseDatetimeType(Token *name);
	bool ParseDatetime2Type(Token *name);
	bool ParseDatetimeoffsetType(Token *name);
	bool ParseDateType(Token *name);
	bool ParseDbclobType(Token *name);
	bool ParseDecfloatType(Token *name);
	bool ParseDecimalType(Token *name, int clause_scope);
	bool ParseDoubleType(Token *name);
	bool ParseFixedType(Token *name);
	bool ParseFloatType(Token *name);
	bool ParseFloat4Type(Token *name);
	bool ParseFloat8Type(Token *name);
	bool ParseGraphicType(Token *name);
	bool ParseImageType(Token *name);
	bool ParseIntervalType(Token *name);
	bool ParseIntType(Token *name, int clause_scope);
	bool ParseInt1Type(Token *name);
	bool ParseInt2Type(Token *name);
	bool ParseInt3Type(Token *name);
	bool ParseInt4Type(Token *name);
	bool ParseInt8Type(Token *name);
	bool ParseLogicalType(Token *name);
	bool ParseLongblobType(Token *name);
	bool ParseLongType(Token *name);
	bool ParseLongtextType(Token *name);
	bool ParseLvarcharType(Token *name);
	bool ParseMediumblobType(Token *name);
	bool ParseMediumintType(Token *name);
	bool ParseMediumtextType(Token *name);
	bool ParseMemoType(Token *name);
	bool ParseMiddleintType(Token *name);
	bool ParseMoneyType(Token *name);
	bool ParseNationalType(Token *name);
	bool ParseNcharType(Token *name);
	bool ParseNclobType(Token *name);
	bool ParseNmemoType(Token *name);
	bool ParseNtextType(Token *name);
	bool ParseNumberType(Token *name, int clause_scope);
	bool ParseNumericType(Token *name);
	bool ParseNvarcharType(Token *name);
	bool ParseNvarchar2Type(Token *name, int clause_scope);
	bool ParseRawType(Token *name);
	bool ParseRealType(Token *name);
	bool ParseRefcursor(Token *name);
	bool ParseRowidType(Token *name);
	bool ParseRowversionType(Token *name);
	bool ParseSerialType(Token *name);
	bool ParseSerial2Type(Token *name);
	bool ParseSerial4Type(Token *name);
	bool ParseSerial8Type(Token *name);
	bool ParseShortType(Token *name);
	bool ParseSmalldatetimeType(Token *name);
	bool ParseSmallfloatType(Token *name);
	bool ParseSmallintType(Token *name, int clause_scope);
	bool ParseSmallmoneyType(Token *name);
	bool ParseSmallserialType(Token *name);
	bool ParseStringType(Token *name);
	bool ParseTextType(Token *name);
	bool ParseTimeType(Token *name);
	bool ParseTimetzType(Token *name);
	bool ParseTimestampType(Token *name);
	bool ParseTimestamptzType(Token *name);
	bool ParseTinyblobType(Token *name);
	bool ParseTinyintType(Token *name, int clause_scope);
	bool ParseTinytextType(Token *name);
	bool ParseUnicodeType(Token *name);
	bool ParseUnicharType(Token *name);
	bool ParseUniqueidentifierType(Token *name);
	bool ParseUniqueidentifierstrType(Token *name);
	bool ParseUnitextType(Token *name);
	bool ParseUnivarcharType(Token *name);
	bool ParseUnsignedType(Token *name);
	bool ParseUrowidType(Token *name);
	bool ParseUuidType(Token *name);
	bool ParseVarbinaryType(Token *name);
	bool ParseVarbitType(Token *name);
	bool ParseVarcharType(Token *name, int clause_scope);
	bool ParseVarchar2Type(Token *name, int clause_scope);
	bool ParseVargraphicType(Token *name);
	bool ParseXmlType(Token *name);
	bool ParseXmltypeType(Token *name);
	bool ParseYearType(Token *name);
	bool ParseUdt(Token *name, int clause_scope);

	// Statements
	bool ParseAlterStatement(Token *token, int *result_sets, bool *proc);
	bool ParseAlterTableStatement(Token *alter, Token *table);
	bool ParseAlterIndexStatement(Token *alter, Token *index);
	bool ParseAlterSequenceStatement(Token *alter, Token *sequence);
	bool ParseAssignmentStatement(Token *variable);
	bool ParseAllocateStatement(Token *allocate);
	bool ParseAssociateStatement(Token *associate);
	bool ParseBeginStatement(Token *begin);
	bool ParseCallStatement(Token *call);
	bool ParseCaseStatement(Token *case_, bool stmt = true);
	bool ParseCloseStatement(Token *close);
	bool ParseCommentStatement(Token *comment);
	bool ParseCommitStatement(Token *commit);
    bool ParseCollectStatement(Token *collect);
	bool ParseConnectStatement(Token *connect);
	bool ParseCreateAuxiliary(Token *create, Token *auxiliary);
	bool ParseCreateDatabase(Token *create, Token *database);
	bool ParseCreateFunction(Token *create, Token *or_, Token *replace, Token *function);
	bool ParseCreateStatement(Token *token, int *result_sets, bool *proc);
	bool ParseCreateTable(Token *create, Token *table);
	bool ParseCreateTablespace(Token *create, Token *tablespace);
	bool ParseCreateIndex(Token *create, Token *unique, Token *index);
	bool ParseCreatePackage(Token *create, Token *or_, Token *replace, Token *package);
	bool ParseCreatePackageSpec(Token *create, Token *or_, Token *replace, Token *package);
	bool ParseCreatePackageBody(Token *create, Token *or_, Token *replace, Token *package, Token *body);
	bool ParseCreateProcedure(Token *create, Token *or_, Token *replace, Token *procedure, int *result_sets);
	bool ParseCreateTrigger(Token *create, Token *or_, Token *trigger);
	bool ParseCreateTriggerBody(Token *create, Token *name, Token *table, Token *when, Token *insert, Token *update, Token *delete_, Token **end);
	bool ParseCreateTriggerOraclePattern(Token *create, Token *table, Token *when, Token *insert);
	bool ParseCreateType(Token *create, Token *type);
	bool ParseCreateRule(Token *create, Token *rule);
	bool ParseCreateSchema(Token *create, Token *schema);
	bool ParseCreateSequence(Token *create, Token *sequence);
	bool ParseSequenceOptions(Token **start_with, Token **increment_by, StatsSummaryItem &sti);
	bool ParseCreateStogroup(Token *create, Token *stogroup);
	bool ParseCreateView(Token *create, Token *materialized, Token *view);
	bool ParseDeclareStatement(Token *declare);
	bool ParseDefineStatement(Token *define);
	bool ParseDeleteStatement(Token *delete_);
	bool ParseDelimiterStatement(Token *delimiter);
	bool ParseDropStatement(Token *drop);
	bool ParseDropDatabaseStatement(Token *drop, Token *database);
	bool ParseDropTableStatement(Token *drop, Token *table);
	bool ParseDropProcedureStatement(Token *drop, Token *procedure);
	bool ParseDropTriggerStatement(Token *drop, Token *trigger);
	bool ParseDropSchemaStatement(Token *drop, Token *schema);
	bool ParseDropSequenceStatement(Token *drop, Token *sequence);
	bool ParseDropStogroupStatement(Token *drop, Token *stogroup);
	bool ParseExceptionBlock(Token *exception);
	bool ParseExecStatement(Token *exec);
	bool ParseExecuteStatement(Token *execute);
	bool ParseExitStatement(Token *exit);
	bool ParseExportStatement(Token *export_);
	bool ParseFetchStatement(Token *fetch);
	bool ParseForStatement(Token *for_, int scope);
	bool ParseForeachStatement(Token *foreach_, int scope);
	bool ParseFreeStatement(Token *free);
	bool ParseHelpStatement(Token *help);
	bool ParseIfStatement(Token *token, int scope);
	bool ParseInsertStatement(Token *insert);
	bool ParseLetStatement(Token *let);
	bool ParseLeaveStatement(Token *leave);
	bool ParseGetStatement(Token *get);
	bool ParseGrantStatement(Token *grant);
	bool ParseLockStatement(Token *lock_);
	bool ParseLoopStatement(Token *loop, int scope);
	bool ParseMergeStatement(Token *merge);
	bool ParseNullStatement(Token *null_);
	bool ParseOnStatement(Token *on);
	bool ParseOnExceptionStatement(Token *on, Token *exception);
	bool ParseOpenStatement(Token *open);
	bool ParsePerformStatement(Token *perform);
	bool ParsePrepareStatement(Token *prepare);
	bool ParsePrintStatement(Token *print);
	bool ParsePromptStatement(Token *prompt);
	bool ParseRemStatement(Token *rem);
	bool ParseRaiseStatement(Token *raise);
	bool ParseRepeatStatement(Token *repeat, int scope);
	bool ParseReplaceStatement(Token *replace);
	bool ParseResignalStatement(Token *resignal);
	bool ParseReturnStatement(Token *return_);
	bool ParseRevokeStatement(Token *revoke);
	bool ParseRollbackStatement(Token *rollback);
	bool ParseSavepointStatement(Token *savepoint);
	bool ParseSelectStatement(Token *token, int block_scope, int select_scope, int *result_sets, Token **list_end, ListW *exp_starts, ListW *out_cols, ListW *into_cols, int *appended_subquery_aliases, Token **from_end, Token **where_end);
	bool ParseSelectExpressionPattern(Token *open, Token *select); 
	bool ParseSetStatement(Token *set);
	bool ParseSetOptions(Token *set);
	bool ParseShowStatement(Token *show);
	bool ParseSignalStatement(Token *signal);
	bool ParseSystemStatement(Token *system);
	bool ParseTerminateStatement(Token *terminate);
	bool ParseTruncateStatement(Token *truncate, int scope);
	bool ParseValuesStatement(Token *token, int *result_sets);
	bool ParseUpdateStatement(Token *update);
	bool ParseUseStatement(Token *use);
	bool ParseWhileStatement(Token *while_, int scope);
	bool ParseWhileStatementEnd();

	// Functions
	bool ParseFunctionAbs(Token *name, Token *open);
	bool ParseFunctionAbsval(Token *name, Token *open);
	bool ParseFunctionAcos(Token *name, Token *open);
	bool ParseFunctionAddDate(Token *name, Token *open);
	bool ParseFunctionAddMonths(Token *name, Token *open);
	bool ParseFunctionArgn(Token *name, Token *open);
	bool ParseFunctionAscii(Token *name, Token *open);
	bool ParseFunctionAsciistr(Token *name, Token *open);
	bool ParseFunctionAsehostname(Token *name, Token *open);
	bool ParseFunctionAsin(Token *name, Token *open);
	bool ParseFunctionAtan(Token *name, Token *open);
	bool ParseFunctionAtan2(Token *name, Token *open);
	bool ParseFunctionAtanh(Token *name, Token *open);
	bool ParseFunctionAtn2(Token *name, Token *open);
	bool ParseFunctionAvg(Token *name, Token *open);
	bool ParseFunctionBase64Decode(Token *name, Token *open);
	bool ParseFunctionBase64Encode(Token *name, Token *open);
	bool ParseFunctionBigint(Token *name, Token *open);
	bool ParseFunctionBiginttohex(Token *name, Token *open);
	bool ParseFunctionBinToNum(Token *name, Token *open);
	bool ParseFunctionBintostr(Token *name, Token *open);
	bool ParseFunctionBitand(Token *name, Token *open);
	bool ParseFunctionBitandnot(Token *name, Token *open);
	bool ParseFunctionBitLength(Token *name, Token *open);
	bool ParseFunctionBitnot(Token *name, Token *open);
	bool ParseFunctionBitor(Token *name, Token *open);
	bool ParseFunctionBitSubstr(Token *name, Token *open);
	bool ParseFunctionBitxor(Token *name, Token *open);
	bool ParseFunctionBlob(Token *name, Token *open);
	bool ParseFunctionByteLength(Token *name, Token *open);
	bool ParseFunctionByteSubstr(Token *name, Token *open);
	bool ParseFunctionCast(Token *name, Token *open);
	bool ParseFunctionCeil(Token *name, Token *open);
	bool ParseFunctionCeiling(Token *name, Token *open);
	bool ParseFunctionChar(Token *name, Token *open);
	bool ParseFunctionCharacterLength(Token *name, Token *open);
	bool ParseFunctionCharindex(Token *name, Token *open);
	bool ParseFunctionChr(Token *name, Token *open);
	bool ParseFunctionClob(Token *name, Token *open);
	bool ParseFunctionCoalesce(Token *name, Token *open);
	bool ParseFunctionColLength(Token *name, Token *open);
	bool ParseFunctionColName(Token *name, Token *open);
	bool ParseFunctionCompare(Token *name, Token *open);
	bool ParseFunctionConcat(Token *name, Token *open);
	bool ParseFunctionConvert(Token *name, Token *open);
	bool ParseFunctionConvertMySql(Token *name, Token *open);
	bool ParseFunctionConvertDatetime(Token *name, Token *open, Token *datatype);
	bool ParseFunctionConvertImage(Token *name, Token *open, Token *datatype);
	bool ParseFunctionConvertInteger(Token *name, Token *open, Token *datatype);
	bool ParseFunctionConvertText(Token *name, Token *open, Token *datatype);
	bool ParseFunctionConvertVarchar(Token *name, Token *open, Token *datatype);
	bool ParseFunctionCos(Token *name, Token *open);
	bool ParseFunctionCosh(Token *name, Token *open);
	bool ParseFunctionCot(Token *name, Token *open);
	bool ParseFunctionCount(Token *name, Token *open);
	bool ParseFunctionCsconvert(Token *name, Token *open);
	bool ParseFunctionCurdate(Token *name, Token *open);
	bool ParseFunctionCurrentBigdatetime(Token *name, Token *open);
	bool ParseFunctionCurrentBigtime(Token *name, Token *open);
	bool ParseFunctionCurrentDate(Token *name, Token *open);
	bool ParseFunctionCurrentTime(Token *name, Token *open);
	bool ParseFunctionCurrentTimestamp(Token *name, Token *open);
	bool ParseFunctionCursorRowcount(Token *name, Token *open);
	bool ParseFunctionDatalength(Token *name, Token *open);
	bool ParseFunctionDate(Token *name, Token *open);
	bool ParseFunctionDateadd(Token *name, Token *open);
	bool ParseFunctionDatediff(Token *name, Token *open);
	bool ParseFunctionDateFormat(Token *name, Token *open);
	bool ParseFunctionDateformatASA(Token *name, Token *open);
	bool ParseFunctionDatename(Token *name, Token *open);
	bool ParseFunctionDatepart(Token *name, Token *open);
	bool ParseFunctionDatesub(Token *name, Token *open);
	bool ParseFunctionDatetime(Token *name, Token *open);
	bool ParseFunctionDatetimeInformix(Token *name, Token *open);
	bool ParseFunctionDay(Token *name, Token *open);
	bool ParseFunctionDayname(Token *name, Token *open);
	bool ParseFunctionDayofmonth(Token *name, Token *open);
	bool ParseFunctionDayofweek(Token *name, Token *open);
	bool ParseFunctionDayofweekIso(Token *name, Token *open);
	bool ParseFunctionDayofyear(Token *name, Token *open);
	bool ParseFunctionDays(Token *name, Token *open);
	bool ParseFunctionDenseRank(Token *name, Token *open);
	bool ParseFunctionDbclob(Token *name, Token *open);
	bool ParseFunctionDbId(Token *name, Token *open);
	bool ParseFunctionDbinfo(Token *name, Token *open);
	bool ParseFunctionDbInstanceid(Token *name, Token *open);
	bool ParseFunctionDbmsOutput(Token *name, Token *open, StatsDetailItem &sdi);
	bool ParseFunctionDbName(Token *name, Token *open);
	bool ParseFunctionDecfloat(Token *name, Token *open);
	bool ParseFunctionDecfloatFormat(Token *name, Token *open);
	bool ParseFunctionDecimal(Token *name, Token *open);
	bool ParseFunctionDecode(Token *name, Token *open);
	bool ParseFunctionDegrees(Token *name, Token *open);
	bool ParseFunctionDeref(Token *name, Token *open);
	bool ParseFunctionDigits(Token *name, Token *open);
	bool ParseFunctionDouble(Token *name, Token *open);
	bool ParseFunctionDow(Token *name, Token *open);
	bool ParseFunctionEmptyBlob(Token *name, Token *open);
	bool ParseFunctionEmptyClob(Token *name, Token *open);
	bool ParseFunctionEmptyDbclob(Token *name, Token *open);
	bool ParseFunctionEmptyNclob(Token *name, Token *open);
	bool ParseFunctionErrormsg(Token *name, Token *open);
	bool ParseFunctionExp(Token *name, Token *open);
	bool ParseFunctionExprtype(Token *name, Token *open);
	bool ParseFunctionExtract(Token *name, Token *open);
	bool ParseFunctionFirst(Token *name, Token *open);
	bool ParseFunctionFloat(Token *name, Token *open);
	bool ParseFunctionFloor(Token *name, Token *open);
	bool ParseFunctionGetBit(Token *name, Token *open);
	bool ParseFunctionGetdate(Token *name, Token *open);
	bool ParseFunctionGetutcdate(Token *name, Token *open);
	bool ParseFunctionGreater(Token *name, Token *open);
	bool ParseFunctionGreatest(Token *name, Token *open);
	bool ParseFunctionHash(Token *name, Token *open);
	bool ParseFunctionHex(Token *name, Token *open);
	bool ParseFunctionHextobigint(Token *name, Token *open);
	bool ParseFunctionHextoint(Token *name, Token *open);
	bool ParseFunctionHextoraw(Token *name, Token *open);
	bool ParseFunctionHostId(Token *name, Token *open);
	bool ParseFunctionHostName(Token *name, Token *open);
	bool ParseFunctionHour(Token *name, Token *open);
	bool ParseFunctionHtmlDecode(Token *name, Token *open);
	bool ParseFunctionHtmlEncode(Token *name, Token *open);
	bool ParseFunctionHttpDecode(Token *name, Token *open);
	bool ParseFunctionHttpEncode(Token *name, Token *open);
	bool ParseFunctionIdentity(Token *name, Token *open);
	bool ParseFunctionIdentity(Token *name);
	bool ParseFunctionIfnull(Token *name, Token *open);
	bool ParseFunctionIif(Token *name, Token *open);
	bool ParseFunctionIndexCol(Token *name, Token *open);
	bool ParseFunctionIndexColorder(Token *name, Token *open);
	bool ParseFunctionIndexName(Token *name, Token *open);
	bool ParseFunctionInitcap(Token *name, Token *open);
	bool ParseFunctionInsert(Token *name, Token *open);
	bool ParseFunctionInsertstr(Token *name, Token *open);
	bool ParseFunctionInstanceId(Token *name, Token *open);
	bool ParseFunctionInstanceName(Token *name, Token *open);
	bool ParseFunctionInstr(Token *name, Token *open);
	bool ParseFunctionInstrb(Token *name, Token *open);
	bool ParseFunctionInteger(Token *name, Token *open);
	bool ParseFunctionInttohex(Token *name, Token *open);
	bool ParseFunctionIsdate(Token *name, Token *open);
	bool ParseFunctionIsnull(Token *name, Token *open);
	bool ParseFunctionIsnumeric(Token *name, Token *open);
	bool ParseFunctionIsSingleusermode(Token *name, Token *open);
	bool ParseFunctionJulianDay(Token *name, Token *open);
	bool ParseFunctionLastDay(Token *name, Token *open);
	bool ParseFunctionLastAutoInc(Token *name, Token *open);
	bool ParseFunctionLcase(Token *name, Token *open);
	bool ParseFunctionLeast(Token *name, Token *open);
	bool ParseFunctionLeft(Token *name, Token *open);
	bool ParseFunctionLen(Token *name, Token *open);
	bool ParseFunctionLength(Token *name, Token *open);
	bool ParseFunctionLengthb(Token *name, Token *open);
	bool ParseFunctionLesser(Token *name, Token *open);
	bool ParseFunctionList(Token *name, Token *open);
	bool ParseFunctionLn(Token *name, Token *open);
	bool ParseFunctionLocate(Token *name, Token *open);
	bool ParseFunctionLocateInString(Token *name, Token *open);
	bool ParseFunctionLog(Token *name, Token *open);
	bool ParseFunctionLog10(Token *name, Token *open);
	bool ParseFunctionLongVarchar(Token *name, Token *open);
	bool ParseFunctionLongVargraphic(Token *name, Token *open);
	bool ParseFunctionLower(Token *name, Token *open);
	bool ParseFunctionLpad(Token *name, Token *open);
	bool ParseFunctionLtrim(Token *name, Token *open);
	bool ParseFunctionMax(Token *name, Token *open);
	bool ParseFunctionMdy(Token *name, Token *open);
	bool ParseFunctionMicrosecond(Token *name, Token *open);
	bool ParseFunctionMidnightSeconds(Token *name, Token *open);
	bool ParseFunctionMin(Token *name, Token *open);
	bool ParseFunctionMinute(Token *name, Token *open);
	bool ParseFunctionMod(Token *name, Token *open);
	bool ParseFunctionMonth(Token *name, Token *open);
	bool ParseFunctionMonthname(Token *name, Token *open);
	bool ParseFunctionMonthsBetween(Token *name, Token *open);
	bool ParseFunctionMultiplyAlt(Token *name, Token *open);
	bool ParseFunctionNchar(Token *name, Token *open);
	bool ParseFunctionNclob(Token *name, Token *open);
	bool ParseFunctionNewid(Token *name, Token *open);
	bool ParseFunctionNewidstring(Token *name, Token *open);
	bool ParseFunctionNextDay(Token *name, Token *open);
	bool ParseFunctionNextIdentity(Token *name, Token *open);
	bool ParseFunctionNow(Token *name, Token *open);
	bool ParseFunctionNullif(Token *name, Token *open);
	bool ParseFunctionNumber(Token *name, Token *open);
	bool ParseFunctionNvarchar(Token *name, Token *open);
	bool ParseFunctionNvl(Token *name, Token *open);
	bool ParseFunctionNvl2(Token *name, Token *open);
	bool ParseFunctionObjectId(Token *name, Token *open);
	bool ParseFunctionObjectName(Token *name, Token *open);
	bool ParseFunctionObjectOwnerId(Token *name, Token *open);
	bool ParseFunctionOctetLength(Token *name, Token *open);
	bool ParseFunctionOverlay(Token *name, Token *open);
	bool ParseFunctionPartitionId(Token *name, Token *open);
	bool ParseFunctionPartitionName(Token *name, Token *open);
	bool ParseFunctionPartitionObjectId(Token *name, Token *open);
	bool ParseFunctionPasswordRandom(Token *name, Token *open);
	bool ParseFunctionPatindex(Token *name, Token *open);
	bool ParseFunctionPi(Token *name, Token *open);
	bool ParseFunctionPosition(Token *name, Token *open);
	bool ParseFunctionPosstr(Token *name, Token *open);
	bool ParseFunctionPower(Token *name, Token *open);
	bool ParseFunctionQuarter(Token *name, Token *open);
	bool ParseFunctionRadians(Token *name, Token *open);
	bool ParseFunctionRaiseError(Token *name, Token *open);
	bool ParseFunctionRaiserror(Token *name, Token *open);
	bool ParseFunctionRank(Token *name, Token *open);
	bool ParseFunctionRand(Token *name, Token *open);
	bool ParseFunctionRand2(Token *name, Token *open);
	bool ParseFunctionReal(Token *name, Token *open);
	bool ParseFunctionRegexpLike(Token *name, Token *open);
	bool ParseFunctionRegexpSubstr(Token *name, Token *open);
	bool ParseFunctionRemainder(Token *name, Token *open);
	bool ParseFunctionRepeat(Token *name, Token *open);
	bool ParseFunctionReplace(Token *name, Token *open);
	bool ParseFunctionReplicate(Token *name, Token *open);
	bool ParseFunctionReserveIdentity(Token *name, Token *open);
	bool ParseFunctionReverse(Token *name, Token *open);
	bool ParseFunctionRight(Token *name, Token *open);
	bool ParseFunctionRound(Token *name, Token *open);
	bool ParseFunctionRoundTimestamp(Token *name, Token *open);
    bool ParseFunctionRowNumber(Token *name, Token *open);
	bool ParseFunctionRpad(Token *name, Token *open);
	bool ParseFunctionRtrim(Token *name, Token *open);
	bool ParseFunctionScopeIdentity(Token *name, Token *open);
	bool ParseFunctionSecond(Token *name, Token *open);
	bool ParseFunctionSign(Token *name, Token *open);
	bool ParseFunctionSin(Token *name, Token *open);
	bool ParseFunctionSinh(Token *name, Token *open);
	bool ParseFunctionSmallint(Token *name, Token *open);
	bool ParseFunctionSoundex(Token *name, Token *open);
	bool ParseFunctionSpace(Token *name, Token *open);
	bool ParseFunctionSpidInstanceId(Token *name, Token *open);
	bool ParseFunctionSqlerrm(Token *name, Token *open);
	bool ParseFunctionSqrt(Token *name, Token *open);
	bool ParseFunctionSquare(Token *name, Token *open);
	bool ParseFunctionStr(Token *name, Token *open);
	bool ParseFunctionString(Token *name, Token *open);
	bool ParseFunctionStrip(Token *name, Token *open);
	bool ParseFunctionStrReplace(Token *name, Token *open);
	bool ParseFunctionStrtobin(Token *name, Token *open);
	bool ParseFunctionStuff(Token *name, Token *open);
	bool ParseFunctionSubdate(Token *name, Token *open);
	bool ParseFunctionSubstr(Token *name, Token *open);
	bool ParseFunctionSubstr2(Token *name, Token *open);
	bool ParseFunctionSubstrb(Token *name, Token *open);
	bool ParseFunctionSubstring(Token *name, Token *open);
	bool ParseFunctionSum(Token *name, Token *open);
	bool ParseFunctionSuserId(Token *name, Token *open);
	bool ParseFunctionSuserName(Token *name, Token *open);
	bool ParseFunctionSwitchoffset(Token *name, Token *open);
	bool ParseFunctionSysdatetimeoffset(Token *name, Token *open);
	bool ParseFunctionSysContext(Token *name, Token *open);
	bool ParseFunctionSysGuid(Token *name, Token *open);
	bool ParseFunctionTan(Token *name, Token *open);
	bool ParseFunctionTanh(Token *name, Token *open);
	bool ParseFunctionTextptr(Token *name, Token *open);
	bool ParseFunctionTextvalid(Token *name, Token *open);
	bool ParseFunctionTime(Token *name, Token *open);
	bool ParseFunctionTimestamp(Token *name, Token *open);
	bool ParseFunctionTimestampadd(Token *name, Token *open);
	bool ParseFunctionTimestampdiff(Token *name, Token *open);
	bool ParseFunctionTimestampFormat(Token *name, Token *open);
	bool ParseFunctionTimestampIso(Token *name, Token *open);
	bool ParseFunctionToChar(Token *name, Token *open);
	bool ParseFunctionToClob(Token *name, Token *open);
	bool ParseFunctionToDate(Token *name, Token *open);
	bool ParseFunctionTodatetimeoffset(Token *name, Token *open);
	bool ParseFunctionToday(Token *name, Token *open);
	bool ParseFunctionToLob(Token *name, Token *open);
	bool ParseFunctionToNchar(Token *name, Token *open);
	bool ParseFunctionToNclob(Token *name, Token *open);
	bool ParseFunctionToNumber(Token *name, Token *open);
	bool ParseFunctionToSingleByte(Token *name, Token *open);
	bool ParseFunctionToTimestamp(Token *name, Token *open);
	bool ParseFunctionToTimestampTz(Token *name, Token *open);
	bool ParseFunctionToUnichar(Token *name, Token *open);
	bool ParseFunctionTranslate(Token *name, Token *open);
	bool ParseFunctionTrim(Token *name, Token *open);
	bool ParseFunctionTrunc(Token *name, Token *open);
	bool ParseFunctionTruncate(Token *name, Token *open);
	bool ParseFunctionTruncnum(Token *name, Token *open);
	bool ParseFunctionTruncTimestamp(Token *name, Token *open);
	bool ParseFunctionTsequal(Token *name, Token *open);
	bool ParseFunctionUcase(Token *name, Token *open);
	bool ParseFunctionUhighsurr(Token *name, Token *open);
	bool ParseFunctionUlowsurr(Token *name, Token *open);
	bool ParseFunctionUnicode(Token *name, Token *open);
	bool ParseFunctionUnistr(Token *name, Token *open);
	bool ParseFunctionUpper(Token *name, Token *open);
	bool ParseFunctionUscalar(Token *name, Token *open);
	bool ParseFunctionUserenv(Token *name, Token *open);
	bool ParseFunctionUserId(Token *name, Token *open);
	bool ParseFunctionUserName(Token *name, Token *open);
	bool ParseFunctionValue(Token *name, Token *open);
	bool ParseFunctionVarchar(Token *name, Token *open);
	bool ParseFunctionVarcharBitFormat(Token *name, Token *open);
	bool ParseFunctionVarcharFormat(Token *name, Token *open);
	bool ParseFunctionVarcharFormatBit(Token *name, Token *open);
	bool ParseFunctionVargraphic(Token *name, Token *open);
	bool ParseFunctionVsize(Token *name, Token *open);
	bool ParseFunctionWeek(Token *name, Token *open);
	bool ParseFunctionWeekIso(Token *name, Token *open);
	bool ParseFunctionXmlagg(Token *name, Token *open);
	bool ParseFunctionXmlattributes(Token *name, Token *open);
	bool ParseFunctionXmlcast(Token *name, Token *open);
	bool ParseFunctionXmlcdata(Token *name, Token *open);
	bool ParseFunctionXmlcomment(Token *name, Token *open);
	bool ParseFunctionXmlconcat(Token *name, Token *open);
	bool ParseFunctionXmldiff(Token *name, Token *open);
	bool ParseFunctionXmldocument(Token *name, Token *open);
	bool ParseFunctionXmlelement(Token *name, Token *open);
	bool ParseFunctionXmlextract(Token *name, Token *open);
	bool ParseFunctionXmlforest(Token *name, Token *open);
	bool ParseFunctionXmlgen(Token *name, Token *open);
	bool ParseFunctionXmlisvalid(Token *name, Token *open);
	bool ParseFunctionXmlnamespaces(Token *name, Token *open);
	bool ParseFunctionXmlparse(Token *name, Token *open);
	bool ParseFunctionXmlpatch(Token *name, Token *open);
	bool ParseFunctionXmlpi(Token *name, Token *open);
	bool ParseFunctionXmlquery(Token *name, Token *open);
	bool ParseFunctionXmlrepresentation(Token *name, Token *open);
	bool ParseFunctionXmlroot(Token *name, Token *open);
	bool ParseFunctionXmlrow(Token *name, Token *open);
	bool ParseFunctionXmlsequence(Token *name, Token *open);
	bool ParseFunctionXmlserialize(Token *name, Token *open);
	bool ParseFunctionXmltext(Token *name, Token *open);
	bool ParseFunctionXmltransform(Token *name, Token *open);
	bool ParseFunctionXmlvalidate(Token *name, Token *open);
	bool ParseFunctionXmlxsrobjectid(Token *name, Token *open);
	bool ParseFunctionXsltransform(Token *name, Token *open);
	bool ParseFunctionXmlChainedMethods();
	bool ParseFunctionYear(Token *name, Token *open);
	bool ParseFunctionZeroifnull(Token *name, Token *open);
	bool ParseUnknownFunction(Token *name, Token *open);

	// Functions without parameters
	bool ParseFunctionActivityCount(Token *activity_count);
	bool ParseFunctionCurrent(Token *name);
	bool ParseFunctionCurrentDate(Token *name);
	bool ParseFunctionCurrentSchema(Token *name);
	bool ParseFunctionCurrentSqlid(Token *name);
	bool ParseFunctionCurrentTimestamp(Token *name);
	bool ParseFunctionCurrentTime(Token *name);
	bool ParseFunctionCurrentUser(Token *name);
	bool ParseFunctionDate(Token *name);
	bool ParseFunctionInterval(Token *name);
	bool ParseFunctionLocaltimestamp(Token *name);
	bool ParseFunctionNextval(Token *name);
	bool ParseFunctionNull(Token *name);
	bool ParseFunctionRowcount(Token *name);
	bool ParseFunctionSqlcode(Token *name);
	bool ParseFunctionSqlstate(Token *name);
	bool ParseFunctionSqlPercent(Token *name);
	bool ParseFunctionSysdate(Token *name);
	bool ParseFunctionSystemUser(Token *name);
	bool ParseFunctionSystimestamp(Token *name);
	bool ParseFunctionTime(Token *name);
	bool ParseFunctionToday(Token *name);
	bool ParseFunctionUser(Token *name);
	bool ParseFunctionUtf8(Token *name);

	// System procedures
    bool ParseProcedureRaiseApplicationError(Token *name);
	bool ParseProcedureSpAddType(Token *execute, Token *sp_addtype);
	bool ParseProcedureSpBindRule(Token *execute, Token *sp_bindrule);
	bool ParseProcedureSpAddExtendedProperty(Token *execute, Token *sp_extendedproperty);
	bool ParseUnknownSystemProcedure(Token *name);

    // Read the data type from available meta information
    const char *GetMetaType(Token *object, Token *column = NULL);

	// Guess functions
	char GuessType(Token *name);
	char GuessType(Token *name, TokenStr &type);

	// Clauses
	bool AlterColumnClause(Token *table, Token *table_name, Token *alter);
	bool ParseCreateTableColumns(Token *create, Token *table_name, ListW &pkcols, Token **id_col, Token **id_start, Token **id_inc, bool *id_default, ListWM *inline_indexes, Token **last_colname);
	bool ParseColumnConstraints(Token *create, Token *table_name, Token *column, Token *type, Token *type_end, Token **id_col, Token **id_start, Token **id_inc, bool *id_default);
	bool ParseDeclareCondition(Token *declare, Token *name, Token *condition);
	bool ParseDeclareCursor(Token *declare, Token *name, Token *cursor);
	bool ParseDeclareHandler(Token *declare, Token *type);
	bool ParseDeclareTable(Token *declare, Token *name, Token *table);
	bool ParseDeclareGlobalTemporaryTable(Token *declare, Token *local);
	bool ParseDeclareLocalTemporaryTable(Token *declare, Token *local);
	bool ParseDeclareVariable(Token *declare, Token *name, Token *type, int cnt_list);
	bool ParseDefaultExpression(Token *type, Token *type_end, Token *token, int pos);
	bool FormatDefaultExpression(Token *datatype, Token *exp, Token *default_);
	bool ParseStandaloneColumnConstraints(Token *alter, Token *table_name, ListW &pkcols, ListWM *inline_indexes);
	bool ParseInlineColumnConstraint(Token *type, Token *type_end, Token *constraint, int num);
	bool ParseKeyConstraint(Token *alter, Token *name, Token *primary, ListW &pkcols, bool inline_cns, ListWM *inline_indexes);
	bool ParseKeyIndexOptions();
	bool ParseForeignKey(Token *foreign);
	bool ParseCheckConstraint(Token *check);
	bool ParseConstraintOption();
	bool ParseProcedureParameters(Token *name, int *count, Token **end);
	bool ParseProcedureOptions(Token *create);
	bool ParseProcedureBody(Token *create, Token *procedure, Token *name, Token *as, int *result_sets);
	bool ParseSplEndName(Token *name, Token *end);
	bool ParseOracleVariableDeclarationBlock(Token *declare);
	bool ParseOracleCursorDeclaration(Token *cursor, ListWM *cursors);
	bool ParseOracleObjectType(Token *type);
	bool ParseOracleObjectSubtype(Token *subtype);
	bool ParseOracleObjectTypeAssignment(Token *name);
	bool ParseOraclePragma(Token *pragma);
	bool ParseOracleException(Token *name);
	bool ParseOracleProcSpec(Token *procedure);
	bool ParseOracleFuncSpec(Token *function);
	bool ParseOracleNestedProcBody(Token *procedure);
	bool ParseOracleNestedFuncBody(Token *function);
	bool ParseFunctionParameters(Token *function_name);
	bool ParseFunctionReturns(Token *function);
	bool ParseFunctionOptions();
	bool ParseFunctionBody(Token *create, Token *function, Token *name, Token *as);
	bool ParseSubSelect(Token *open, int select_scope);
	Token* GetNextSelectStartKeyword();
	bool ParseSelectCteClause(Token *with);
	bool ParseSelectList(Token *select, int select_scope, bool *into, bool *dummy_not_required, bool *agg_func, bool *agg_list_func, ListW *exp_starts, ListW *out_cols, ListW *into_cols, Token **rowlimit_slist, bool *rowlimit_percent);
	bool ParseSelectListPredicate(Token **rowlimit_slist, bool *rowlimit_percent);
	bool ParseSelectFromClause(Token *select, bool nested_from, Token **from, Token **from_end, int *appended_subquery_aliases, bool dummy_not_required, ListWM *from_table_end);
	bool ParseJoinClause(Token *first, Token *second, bool first_is_subquery, ListWM *from_table_end);
	bool GetJoinKeywords(Token *token, Token **left_right_full, Token **outer_inner, Token **join);
	bool ParseWhereClause(int stmt_scope, Token **where_, Token **where_end, int *rowlimit = NULL);
	bool ParseWhereCurrentOfCursor(int stmt_scope);
	bool ParseConnectBy();
	bool ParseSelectGroupBy();
	bool ParseSelectHaving();
    bool ParseSelectQualify(Token *select, Token *select_list_end);
	bool ParseSelectOrderBy(Token **order);
	bool ParseSelectSetOperator(int block_scope, int select_scope);
	bool ParseSelectOptions(Token *select, Token *from_end, Token *where_, Token *order, Token **rowlimit_soptions, int *rowlimit);
	void SelectSetOutColsDataTypes(ListW *out_cols, ListWM *from_table_end);
	void SelectConvertRowlimit(Token *select, Token *from, Token *from_end, Token *where_, Token *where_end, Token *pre_order, Token *order, Token *rowlimit_slist, Token *rowlimit_soptions, int rowlimit, bool rowlimit_percent);
	
	bool ParseTempTableOptions(Token *table_name, Token **start, Token **end, bool *no_data);
	bool ParseStorageClause(Token *table_name, Token **id_start, Token **comment, Token *last_colname, Token *last_colend);
	bool ParseCreateIndexOptions();
	bool OpenWithReturnCursor(Token *name);

	void SqlServerConvertRowLevelTrigger(Token *table, Token *when, Token *insert, Token *update, Token *delete_, Token *end);
	void SqlServerAppendSubqueryAlias(Token *append, int *appended_subquery_aliases);
	void SqlServerDelimiter();
	void SqlServerAddStmtDelimiter(bool force = false);
	bool SqlServerGoDelimiter(bool just_remove = false);
	void SqlServerToDateAdd(Token *op, Token *first, Token* first_end, Token *second, Token *second_end);
	bool ParseSqlServerSetOptions(Token *set);
	bool ParseSqlServerIndexOptions(Token *token);
	bool ParseSqlServerStorageClause();
	void ParseSqlServerExecProcedure(Token *execute, Token *name);
	bool ParseSqlServerUpdateStatement(Token *update);
	void SqlServerConvertUdfIdentifier(Token *name);
	void SqlServerMoveCursorDeclarations();

	bool ParseOracleStorageClause();
	bool ParseOracleStorageClause(Token *storage);
	bool ParseOracleLobStorageClause(Token *lob);
	bool ParseOraclePartitions(Token *token);
	bool ParseOraclePartitionsBy(Token *token);
	bool ParseOraclePartition(Token *partition, Token *subpartition);
	bool ParseOracleOuterJoin(Token *exp_start, Token *column);
	bool ParseOracleRownumCondition(Token *first, Token *op, Token *second, int *rowlimit);
	bool RecognizeOracleDateFormat(Token *str, TokenStr &format);
	void OracleEmulateIdentity(Token *create, Token *table, Token *column, Token *last, Token *id_start, Token *id_inc, bool id_default);
	void OracleExitHandlersToException(Token *end);
	void OracleContinueHandlerForFetch(Token *fetch, Token *cursor);
	void OracleContinueHandlerForSelectInto(Token *select);
	void OracleContinueHandlerForUpdate(Token *update);
	void OracleContinueHandlerForInsert(Token *insert);
	void OracleMoveBeginAfterDeclare(Token *create, Token *as, Token *begin, Token *body_start);
	void OracleAddOutRefcursor(Token *create, Token *name, Token *last_param);
	void OracleAppendDataTypeSizes();
	void OracleAppendDataTypeSize(Token *data_type);
	void OracleRemoveDataTypeSize(Token *data_type);
	void OracleTruncateTemporaryTablesWithReplace();
	void OracleMoveSelectOutofIf(Token *stmt_start, Token *select, Token *select_list_end, Token *open, Token *close);
	void OracleIfSelectDeclarations();
	void OracleChangeEqualToDefault(Token *equal);
	bool ParseOracleSetOptions(Token *set);
	bool ParseOracleAlterTable(Token *next);
	bool ParseOracleAlterProcedure();
	bool OracleCreateDatabase(Token *create, Token *database);

	bool ParseMysqlStorageClause(Token *table_name, Token **id_start, Token **comment);
	bool ParseMySQLDelimiter(Token *create);
	bool MySQLCheckDelimiter(char ch, wchar_t wch, Token **char1, Token **char2);
	void MysqlMoveInlineIndexes(ListWM &inline_indexes, Token *append);
	bool ParseMyqlDefinerClause(Token *token);
	bool ParseMysqlSetOptions(Token *set);
	bool MysqlCreateDatabase(Token *create, Token *database, Token *name);
    void MySQLAddNotFoundHandler();
	void MySQLInitNotFoundBeforeOpen();
	bool MySQLMoveCursorDeclarations(Token *declare, Token *cursor_end);

	bool ParseDb2StorageClause();
	bool ParseDb2PartitioningClause(Token *partition, Token *by);
	bool ParseDb2CreateIndexOptions();
	bool ParseDb2GeneratedClause(Token *create, Token *table_name, Token *column, Token *generated, Token **id_col, Token **id_start, Token **id_inc, bool *id_default);
	bool ParseDb2FetchFirstRowOnly(Token *fetch, Token **rowlimit_soptions, int *rowlimit);
	bool ParseDb2StogroupClause(Token *objname, Token *using_, Token *stogroup, int scope);
	bool ParseDb2CreateTablespace(Token *create, Token *tablespace);
	bool ParseDb2Comment(Token *comment, Token *on, Token *name);
	bool Db2CreateDatabase(Token *create, Token *database, Token *name);
	Token* Db2SizeSpecifier();
	void Db2TriggerCorrelatedName(Token *var);
	bool ParseDb2SetOptions(Token *set);
	bool Db2ValuesNextValIntoPattern(Token *values);
	bool ParseDb2ResultSetLocatorDeclaration(Token *declare, Token *name, Token *result_set_locator);
	bool ParseDb2AndBooleanExpression(Token *open);
	bool Db2ParseModifiedByOptions(Token **colsep_out);
	bool ParseDb2ForColumn();

	void PostgresFormatToConcat(Token *format, ListW &params);
	bool ParsePostgresBodyEnd();

	bool ParseInformixStorageClause();
	bool ParseInformixReturns(Token *returns);
	void InformixReturnToAssignment(ListWM &return_list);
	void InformixConvertReturning(Token *create, Token *procedure);
	void InformixGetReturningName(Token *retvalue, TokenStr &name, const char *prefix, const wchar_t *wprefix, size_t plen, int num, bool always_prefix);
	bool ParseInformixIntoTable(Token *select);
	bool InformixPatternAssignLastSerial(Token *let);
	bool ParseInformixSetOptions(Token *set);
	bool ParseInformixUpdateStatistics(Token *update);
	bool ParseInformixMatchesPredicate(Token *pre_first, Token *first, Token *not_, Token *matches);
	bool ParseInformixFragmentBy(Token *fragment);

	// Teradata specific functions
	bool ParseTeradataTableOptions();
	bool ParseTeradataStorageClause(Token *last_colname, Token *last_colend);
	bool ParseTeradataPrimaryIndex(Token *unique, Token *primary, Token *last_colname, Token *last_colend);
	bool ParseTeradataHelpStatement(Token *help);
	bool ParseTeradataHelpStatistics(Token *help, Token *statistics);
	bool ParseTeradataComparisonOperator(Token *op);
	bool ParseTeradataCompressClauseDefaultExpression(Token *compress);

	// Greenplum specific functions
	void AddGreenplumDistributedBy(Token *create, Token *close, ListW &pkcols, Book *col_end);

	// Sybase specific functions
	bool ParseSybaseAdsStorageClause();
	bool ParseSybaseExecuteProcedureStatement(Token *execute);
	bool ParseSybaseWhileFetchStatement(Token *while_, Token *fetch, int scope);
	void SybaseAdsSelectNestedUdfCall(Token *select, ListW *select_cols);
	void SybaseMapDatetimeStyle(Token *style);

	// Add RETURN statements for REFCURSORs
	void AddReturnRefcursors(Token *end);
	// Add variable declarations generated in the procedural block
	void AddGeneratedVariables();

	// Patterns
	bool SelectNextvalFromDual(Token **sequence, Token **column);
	bool ParseCastTimeofdayAsTimestamp(Token *cast, Token *open, Token *first);
	bool ParseBooleanErrorCheckPattern();
	bool ParseBooleanCursorCheckPattern();
	bool PatternSqlCodeCursorLoop(Token *cursor_name);

	// Identifiers
	void ConvertIdentifier(Token *token, int expected_type = 0, int scope = 0);
	void ConvertObjectIdentifier(Token *token, int scope = 0);
	void ConvertColumnIdentifier(Token *token, int scope = 0);
    void ConvertColumnIdentifierSingle(Token *token, int scope = 0);
	void ConvertParameterIdentifier(Token *param);
	void ConvertParameterIdentifier(Token *ref, Token *decl);
	void ConvertVariableIdentifier(Token *token);
	void ConvertVariableIdentifier(Token *ref, Token *decl);
	void ConvertSchemaName(Token *token, TokenStr &ident, size_t *len);
	void ConvertObjectName(Token *token, TokenStr &ident, size_t *len);
	bool ConvertCursorParameter(Token *token);
	bool ConvertRecordVariable(Token *token);
	bool ConvertImplicitRecordVariable(Token *token);
	bool ConvertImplicitRecordVariable(Token *token, Token *rec);
	bool ConvertImplicitForRecordVariable(Token *token);
	bool ConvertTriggerNewOldColumn(Token *token);
	bool ConvertTableVariable(Token *token);
	bool ConvertLocalTableColumn(Token *token);
	bool ConvertSessionTemporaryTable(Token *token);
	int GetIdentPartsCount(Token *token);
	void GetNextIdentItem(Token *token, TokenStr &ident, size_t *len);
	void SplitIdentifierByLastPart(Token *token, TokenStr &lead, TokenStr &trail, int parts = -1);
	bool ConvertTsqlVariable(Token *token);
	bool ConvertToTsqlVariable(Token *token);
	bool ConvertOraclePseudoColumn(Token *token);
	bool ConvertDb2Identifier(Token *token, int expected_type);
	bool ConvertMysqlIdentifier(Token *token, int expected_type);
	void ConvertIdentRemoveLeadingPart(Token *token);
	void PrefixPackageName(TokenStr &ident);
	void DiscloseRecordVariables(Token *format);
	void DiscloseImplicitRecordVariables(Token *format);
	bool IsFuncToProc(Token *name);

	// Get position to append new generated declarations
	Token* GetDeclarationAppend(); 
	// Generate cursor name for standalone SELECT returning a result set
	void GenerateResultSetCursorName(TokenStr *name);
	
	// Find a token in list
	Token* Find(ListW &list, Token *what);
	ListwmItem* Find(ListWM &list, Token *what, Token *what2 = NULL);

	// Create a new identifier by appending the word
	Token *AppendIdentifier(Token *source, const char *word, const wchar_t *w_word, size_t len);
	void AppendIdentifier(TokenStr &source, const char *word, const wchar_t *w_word, size_t len);
	void AppendIdentifier(TokenStr &source, TokenStr &append);
	// Split identifier to 2 parts
	void SplitIdentifier(Token *source, Token *first, Token *second);

	// Add /* */ comments around the tokens
	void Comment(Token *first, Token *last = NULL);
	void Comment(const char *word, const wchar_t *w_word, size_t len, Token *first, Token *last);
	void CommentNoSpaces(Token *first, Token *last = NULL);

	// Check whether the token is valid column/table alias
	bool IsValidAlias(Token *token);

	// Check for an aggregate function
	bool IsAggregateFunction(Token *name);
	// Check for LIST aggregate function
	bool IsListAggregateFunction(Token *name);			

	// Define database object name mappings, meta information etc.
	void SetObjectMappingFromFile(const char *file);
	void SetSchemaMapping(const char *mapping);
    void SetMetaFromFile(const char *file);
	void SetFuncToSpMappingFromFile(const char *file);

	// Map object name for identifier
	bool MapObjectName(Token *token);

	// Compare identifiers
	bool CompareIdentifiers(Token *token, const char *str);
	bool CompareIdentifiersExistingParts(Token *first, Token *second);
	bool CompareIdentifierPart(TokenStr &first, TokenStr &second);

	// Set parser level (application, SQL, string i.e.)
	void SetLevel(int level) { _level = level; }

	// Set application type (Java, C#, PowerBuilder, COBOL etc.)
	void SetApplicationSource();

    // Create report file
    int CreateReport(const char *summary); 

	// Check if the conversion running in evaluation mode and add comment
	void AddEvalModeComment(Token *token);
};

#endif // sqlines_sqlparser_h