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

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "str.h"
#include "cobol.h"

char *g_symbols = (char*)" _\"'.,;:(){}[]=+-*<>!$~|~`@#%^&/\\\n\r\t";

// Not valid words as an alias (all databases)
const char *g_no_alias[] =      {  "END",  "GO",  "ORDER",  "SELECT",  "WHERE", NULL };
const wchar_t *g_no_alias_w[] = { L"END", L"GO", L"ORDER", L"SELECT", L"WHERE", NULL };
size_t g_no_alias_size[] =      {      3,      2,     5,        6,         5,      0 };

// Constructor/Destructor
SqlParser::SqlParser()
{
	_source = 0;
	_target = 0;

	_source_app = 0;
	_target_app = 0;

	_level = LEVEL_SQL;
    _obj_scope = 0;
    _stmt_scope = 0;

	_start = NULL;
	_next_start = NULL;
	_size = 0;
	_remain_size = 0;
	_line = 1;

	ClearSplScope();

	_spl_package_spec = NULL;
	_spl_package = NULL;
	_declare_format = NULL;
	_push_back_token = NULL;

	_option_rems = false;

    _stats = NULL;
    _report = NULL;
	_java = NULL;
	_cobol = NULL;

	_option_eval_mode = false;
}

SqlParser::~SqlParser() {}

// Set target programming language
void SqlParser::SetLang(const char *value, bool source)
{
    if(value == NULL)
        return;

	short app = 0;

    if(_stricmp(value, "java") == 0)
	{
        app = APP_JAVA;
		_java = new Java();
		_java->SetParser(this);
	}
	else
    if(_stricmp(value, "cobol") == 0)
        app = APP_COBOL;

	if(source)
		_source_app = app;
	else
		_target_app = app;
}

// Set conversion option
void SqlParser::SetOption(const char *option, const char *value)
{
	if(option == NULL)
		return;

    // Perform an assessment
    if(_stricmp(option, "-a") == 0)
    {
		_stats = new Stats();
        _report = new Report();
    }
	else
	// Remove schema name option
	if(_stricmp(option, "-rems") == 0)
		_option_rems = true;
	else
	// Schema mapping
	if(_stricmp(option, "-smap") == 0 && value != NULL)
		SetSchemaMapping(value);
	else
	// Object mapping file
	if(_stricmp(option, "-omapf") == 0 && value != NULL)
		SetObjectMappingFromFile(value);
    else
	// Meta information about table columns
	if(_stricmp(option, "-meta") == 0 && value != NULL)
		SetMetaFromFile(value);
	else
	// Object mapping file
	if(_stricmp(option, "-fspmapf") == 0 && value != NULL)
		SetFuncToSpMappingFromFile(value);
    else
	// Source programming language
	if(_stricmp(option, "-sl") == 0 && value != NULL)
		SetLang(value, true);
    else
	// Target programming language
	if(_stricmp(option, "-tl") == 0 && value != NULL)
		SetLang(value, false);
	else
	if(_stricmp(option, "-oracle_plsql_number_mapping") == 0 && value != NULL)
		_option_oracle_plsql_number_mapping = value;
	else
	if(_stricmp(option, "-set_explicit_schema") == 0 && value != NULL)
		_option_set_explicit_schema = value;
	else
	if(_stricmp(option, "__cur_file__") == 0 && value != NULL)
	{
		_option_cur_file = value;

		if(_stats != NULL)
			_stats->SetSourceFile(value);
	}
	else
	if(_stricmp(option, "__eval_mode__") == 0 && value != NULL)
		_option_eval_mode = true;
}

// Perform conversion
int SqlParser::Convert(const char *input, int size, const char **output, int *out_size, int *lines)
{
	if(input == NULL)
		return -1;

	_start = input;
	_next_start = input;
	_size = size;
	_remain_size = size;
	_line = 1;

	ClearSplScope();

	// Byte order mark for Unicode
	GetBomToken();

	// Set application type (Java, C#, PowerBuilder, COBOL etc.)
	SetApplicationSource();

	// Process tokens until the end of input
	while(true)
	{
		Token *token = GetNextToken();

		if(token == NULL)
			break;

		int result_sets = 0;

		// Parser high-level token
		Parse(token, SQL_SCOPE_FREE, &result_sets);
	}

	Post();

	CreateOutputString(output, out_size);

	// Delete global items since they point to deleted source code
	_udt.DeleteAll();
	_domain_rules.DeleteAll();
	_spl_obj_type_table.DeleteAll();

	_bookmarks.DeleteAll();
	_tokens.DeleteAll();

	if(lines != NULL)
		*lines = _line;

	return 0;
}

// Generate output
void SqlParser::CreateOutputString(const char **output, int *out_size)
{
	if(output == NULL)
		return;

	Token *token = _tokens.GetFirst();

	size_t len = 0;

	int not_removed = 0;
	int removed = 0;

	// Calculate the output size in bytes and format output
	while(token != NULL)
	{
		bool r = false;
		bool n = false;

		// If token removed its target length is 0
		len += token->GetTargetLength();

		if(token->IsRemoved() == false)
		{
			r = token->Compare('\r', L'\r');

			if(r == false)
				n = token->Compare('\n', L'\n');
		}

		// Check if we need to remove newline (0D0A \r\n on Windows, 0D on Unix)
		if(r == true || n == true)
		{
			// String was not empty and all tokens removed
			if(not_removed == 0 && removed != 0)
			{
				Token::Remove(token);
				len--;

				// If current is \r and next is \n remove \n 
				if(r == true && token->next != NULL && token->next->Compare('\n', L'\n') == true)
				{
					// Its size will be reduced in the next iteration
					Token::Remove(token->next);
				}

				// Remove all spaces in this empty line
				Token *cur = token->prev;

				while(cur != NULL)
				{
					// Remove until new line
					if(cur->Compare('\n', L'\n') || cur->Compare('\r', L'\r'))
						break;

					if(cur->IsBlank() == true && cur->IsRemoved() == false)
					{
						Token::Remove(cur);
						len--;
					}

					cur = cur->prev;
				}
			}

			not_removed = 0;
			removed = 0;
		}
		// Calculate the number of removed and not removed tokens in line
		else
		{
			if(token->IsBlank() == false)
			{
				if(token->IsRemoved() == true)
					removed++;
				else
					not_removed++;
			}
		}

		token = _tokens.GetNext();
	}
	
	if(len == 0)
	{
		*output = NULL;

		if(out_size != NULL)
			*out_size = 0;

		return;
	}

	// Allocate buffer
	char *out = new char[len + 1]; *out = 0;

	token = _tokens.GetFirst();
	int cur_len = 0;

	while(token != NULL)
	{
		token->AppendTarget(out, &cur_len);

		token = _tokens.GetNext();
	}

	out[cur_len] = 0;

	*output = out; 

	if(out_size != NULL)
		*out_size = cur_len;
}

// Parser high-level token
void SqlParser::Parse(Token *token, int scope, int *result_sets)
{
	bool exists = false;

	// If application scope is set, start from an application statement
	if(_source_app != 0)
	{
		_level = LEVEL_APP;

		if(_source_app == APP_COBOL && _cobol != NULL)
			exists = _cobol->ParseStatement(token);

		_level = LEVEL_SQL;
	}

	if(exists == true)
		return;

	if(ParseStatement(token, scope, result_sets) == true)
		return;

	if(ParseSystemProcedure(NULL, token) == true)
		return;

	exists = ParseExpression(token);

	// Standalone function call in Oracle
	if(exists == true && _source == SQL_ORACLE && token->type == TOKEN_FUNCTION && token->t_type != TOKEN_STATEMENT && 
		_spl_scope == SQL_SCOPE_PROC)
	{
		// PostgreSQL requires PERFORM proc() from within PL/pgSQL or SELECT proc() for standalone execution;
		if(_target == SQL_POSTGRESQL)
			PREPEND(token, "PERFORM ");
		else
		// Netezza requires explicit CALL keyword
		if(_target == SQL_NETEZZA)
			PREPEND(token, "CALL ");
	}
}

// Get next token from the input
Token* SqlParser::GetNextToken()
{
	// Check push back tokens first
	while(_push_back_token != NULL)
	{
		Token *token = _push_back_token;

		// Multiple tokens can be push back
		_push_back_token = token->next;

		if(token->IsBlank() == true || token->type == TOKEN_COMMENT)
			continue;

		return token;
	}

	Token *token = new Token();
	bool exists = false;

	while(true)
	{
		if(_next_start == NULL || _remain_size <= 0)
		{
			delete token;
			return NULL;
		}

		SkipSpaceTokens();
		
		// Check for a word first as it can start with special symbol such as _ @
		exists = GetWordToken(token);

		if(exists == true)
			break;

		// Check for a single char token
		exists = GetSingleCharToken(token);

		if(exists == true)
			break;
	}
	
	return token;
}

// Get the next token if the previous is set 
Token* SqlParser::GetNextToken(Token *prev)
{
	if(prev == NULL)
		return NULL;

	return GetNextToken();
}

// Get next token that must be identifier
Token* SqlParser::GetNextIdentToken(int expected_type, int scope)
{
	Token *token = GetNextToken();

	if(token == NULL)
		return NULL;

	// If it is a word token, convert identifier
	if(token->type == TOKEN_WORD || token->type == TOKEN_IDENT)
		ConvertIdentifier(token, expected_type, scope);

	// Must not be a single char token (non-alpabetical)
	if(token->chr != 0 || token->wchr != 0)
	{
		PushBack(token);
		return NULL;
	}

	return token;
}

// Get the next token and make sure if contains the specified char
Token* SqlParser::GetNextCharToken(const char ch, const wchar_t wch)
{
	Token *token = GetNextToken();

	if(token == NULL)
		return NULL;

	if(token->Compare(ch, wch) == true)
		return token;
	else
		PushBack(token);

	return NULL;
}

// Get next token if the previous is set
Token* SqlParser::GetNextCharToken(Token *prev, const char ch, const wchar_t wch)
{
	if(prev == NULL)
		return NULL;

	return GetNextCharToken(ch, wch);
}

// Get the specified character token or the last selected
Token* SqlParser::GetNextCharOrLastToken(const char ch, const wchar_t wch)
{
	Token *out = GetNextCharToken(ch, wch);

	// Not found
	if(out == NULL)
		out = GetLastToken();

	return out;
}

// Get + or - as operator, not sign and number 
Token* SqlParser::GetNextPlusMinusAsOperatorToken(const char ch, const wchar_t wch)
{
	Token *token = GetNextToken();

	if(token == NULL)
		return NULL;

	// If + or - already returned as separate token then exit, for example when there is  "+ var"
	if(token->Compare(ch, wch) == true)
		return token;

	// If a number follows + or - they will be in single token as +1, but check for + or - first
	if(token->Compare(ch, wch, 0) == false)
	{
		PushBack(token);
		return NULL;
	}

	// Change the first token to hold operator +/-
	token->type = TOKEN_SYMBOL;
	token->chr = ch;
	token->wchr = 0;
	token->str = NULL;
	token->wstr = NULL;

	// Rewind input pointer
	_next_start -= token->len - 1;
	_remain_size += token->len - 1;

	token->len = 0;

	token->next_start = _next_start;
	token->remain_size = _remain_size;

	return token;
}

// Get special character ', " i.e. as a separate token
Token* SqlParser::GetNextSpecialCharToken(Token *prev, const char *str, const wchar_t *wstr, size_t len)
{
	if(prev == NULL)
		return NULL;

	return GetNextSpecialCharToken(str, wstr, len);
}

Token* SqlParser::GetNextSpecialCharToken(const char *str, const wchar_t *wstr, size_t len)
{
	if(str == NULL || wstr == NULL || len == 0 )
		return NULL;

	SkipSpaceTokens();

	if(_remain_size == 0)
		return NULL;

	const char *cur = _next_start;
	
	bool found = false;

	// Check if the next char is one of expected chars
	for(size_t i = 0; i < len; i++)
	{
		if(str[i] == *cur)
		{
			found = true;
			break;
		}
	}

	if(found == false)
		return NULL;

	_next_start++;
	_remain_size--;

	Token *token = new Token();
	token->type = TOKEN_SYMBOL;
	token->chr = *cur;
	token->wchr = 0;
	token->str = NULL;
	token->wstr = NULL;
	token->len = 0;
	token->remain_size = _remain_size;
	token->next_start = _next_start;

	_tokens.Add(token);

	return token;
}

// Get next token and make sure it contains the specified word
Token* SqlParser::GetNextWordToken(const char *str, const wchar_t *wstr, size_t len)
{
	Token *token = GetNextToken();

	if(token == NULL)
		return NULL;

	if(token->Compare(str, wstr, len) == true)
		return token;
	else
		PushBack(token);

	return NULL;
}

// Get next token if the previous is set
Token* SqlParser::GetNextWordToken(Token *prev, const char *str, const wchar_t *wstr, size_t len)
{
	if(prev == NULL)
		return NULL;

	return GetNextWordToken(str, wstr, len);
}

// Create a token from the current position until a new line
Token* SqlParser::GetNextUntilNewlineToken()
{
	const char *start = _next_start;
	const char *cur = start;

	size_t len = 0;

	// Go until the end of line
	while(_remain_size > 0)
	{
		if(*cur == '\r' || *cur == '\n')
			break;

		_next_start++;
		_remain_size--;
		cur++;
		len++;
	}
	
	if(len == 0)
		return NULL;

	Token *token = new Token();
	token->type = TOKEN_COMMENT;
	token->chr = 0;
	token->wchr = 0;
	token->str = start;
	token->wstr = NULL;
	token->len = len;
	token->remain_size = _remain_size;

	_tokens.Add(token);
			
	return token;	
}

// Get the next token that must be a number
Token* SqlParser::GetNextNumberToken()
{
	Token *token = GetNextToken();

	if(token == NULL)
		return NULL;

	if(token->IsNumeric() == false)
	{
		PushBack(token);
		return NULL;
	}

	return token;
}

Token* SqlParser::GetNextNumberToken(Token *prev)
{
	if(prev == NULL)
		return NULL;

	return GetNextNumberToken();
}

// Get the next token that must be a string literal
Token* SqlParser::GetNextStringToken()
{
	Token *token = GetNextToken();

	if(token == NULL)
		return NULL;

	if(token->type != TOKEN_STRING)
	{
		PushBack(token);
		return NULL;
	}

	return token;
}

// Get the previous non-blank, non comment token
Token* SqlParser::GetPrevToken(Token *token)
{
	if(token == NULL)
		return NULL;

	Token *prev = NULL;
	Token *cur = token->prev;

	while(cur != NULL)
	{
		if(cur->IsBlank() == false && cur->type != TOKEN_COMMENT)
		{
			prev = cur;
			break;
		}

		cur = cur->prev;
	}

	return prev;
}

// Skip space tokens
void SqlParser::SkipSpaceTokens()
{
	while(_remain_size > 0)
	{
		const char *cur = _next_start;

		// Check for a space character
		if(*cur == ' ' || *cur == '\t' || *cur == '\r' || *cur == '\n')
		{
			_next_start++;
			_remain_size--;

			Token *space = new Token();
			space->type = TOKEN_SYMBOL;
			space->chr = *cur;
			space->remain_size = _remain_size;
			space->next_start = _next_start;

			_tokens.Add(space);

			// Count lines if any character follows \n
			if((*cur == '\n' && _remain_size > 0) ||
				// Sometimes a newline is represented single \r (0D), not followed by \n (file from Mac i.e.)
				(*cur == '\r' && _remain_size > 0 && cur[1] != '\n'))
				_line++;

			continue;
		}

		break;
	}
}

// Get a single char token
bool SqlParser::GetSingleCharToken(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	while(_remain_size > 0)
	{
		const char *cur = _next_start;

		// Check for a comment, but do not return it as a token
		if(ParseComment() == true)
			continue;

		// Check for quoted identifier
		exists = GetQuotedIdentifier(token);

		if(exists == true)
			break;

		// Check for string literal
		exists = GetStringLiteral(token);

		if(exists == true)
			break;

		// Return if not a single char
		if(strchr(g_symbols, *cur) == NULL)
			break;

		_next_start++;
		_remain_size--;
	
		token->type = TOKEN_SYMBOL;
		token->chr = *cur;
		token->wchr = 0;
		token->str = NULL;
		token->wstr = NULL;
		token->len = 0;
		token->line = _line;
		token->remain_size = _remain_size;
		token->next_start = _next_start;

		_tokens.Add(token);

		cur++;
		exists = true;

		break;
	}

	return exists;
}

// Return last token 
Token* SqlParser::GetLastToken(Token *last)
{
	if(last != NULL)
		return last;

	return GetLastToken();
}

// Return the last fetched token skipping push backed token if it exists
Token* SqlParser::GetLastToken()
{
	Token *token = NULL; 

	if(_push_back_token != NULL)
		token = _push_back_token->prev;
	else
		token = _tokens.GetLastNoCurrent();

	// Return previous non-blank, non-comment token
	while(token && (token->IsBlank() == true || token->type == TOKEN_COMMENT))
		token = token->prev;

	return token;
}

// Get a procedure or function variable token by name
Token* SqlParser::GetVariable(Token *name)
{
	if(name == NULL || _spl_variables.GetCount() == 0)
		return NULL;

	Token *token = NULL;

	// Start from the end of list as the local block variables have priority
	for(ListwItem *i = _spl_variables.GetLast(); i != NULL; i = i->prev)
	{
		// Local block boundary
		if(i->value == (void*)-3)
			continue;

		if(Token::Compare((Token*)i->value, name) == true)
		{
			token = (Token*)i->value;
			break;
		}
	}

	return token;
}

// Enter a block with own local variables
void SqlParser::EnterLocalVariablesBlock()
{
	// -3 block boundary
	_spl_variables.Add((void*)-3);
}

// Leave the block with own local variables
void SqlParser::LeaveLocalVariablesBlock()
{
	ListwItem *boundary = NULL;

	// Find the last boundary
	for(ListwItem *i = _spl_variables.GetLast(); i != NULL; i = i->prev)
	{
		// Local block boundary
		if(i->value == (void*)-3)
		{
			boundary = i;
			break;
		}
	}

	// No boundary found, only outer variable block set
	if(boundary == NULL)
		return;

	// Remove local block variables
	_spl_variables.DeleteSince(boundary);
}

// Get a procedure or function parameter by name
Token* SqlParser::GetParameter(Token *name)
{
	if(name == NULL || _spl_parameters.GetCount() == 0)
		return NULL;

	Token *token = NULL;

	// Find the parameter
	for(ListwItem *i = _spl_parameters.GetFirst(); i != NULL; i = i->next)
	{
		if(Token::Compare((Token*)i->value, name) == true)
		{
			token = (Token*)i->value;
			break;
		}
	}

	return token;
}

// Get variable or parameter
Token* SqlParser::GetVariableOrParameter(Token *name)
{
	Token *var = GetVariable(name);

	if(var == NULL)
		return GetParameter(name);

	return var;
}

bool SqlParser::GetQuotedIdentifier(Token *token, bool starts_as_unquoted)
{
	if(token == NULL)
		return false;

	const char *cur = _next_start;
	const char *start = cur;

	bool exists = false;
	size_t len = 0;

	while(true)
	{
		// Identifier can be qualified "schema"."table"."name"
		if(*cur == '`' || *cur == '"' || 
			// In Informix [] is the substring operator
			(*cur == '[' && _source != SQL_INFORMIX))
		{
			char end = (*cur == '[') ? ']' : *cur;

			cur++;
			len++;

			_remain_size--;
			_next_start++;

			int initial_remain_size = _remain_size;
			const char *initial_next_start = _next_start;

			// Skip until the terminating quote found
			while(_remain_size > 0)
			{
				if(*cur == end)
					break;

				cur++;
				len++;

				_remain_size--;
				_next_start++;
			}

			// No closing quote or delimiter found
			if(*cur != end)
			{
				// Return to the initial position 
				_remain_size = initial_remain_size;
				_next_start = initial_next_start;

				break;
			}

			cur++;
			len++;
			_remain_size--;
			_next_start++;

			exists = true;
		}
		else
		// Process not quoted part only if it not first
		if(len > 0 || starts_as_unquoted == true)
		{
			// Go until the end of part
			while(_remain_size > 0)
			{
				// Check whether we meet a special character allowed in identifiers (:NEW.name i.e.)
				if(strchr(g_symbols, *cur) != NULL && *cur != '_' && *cur != ':')
					break;
		
				cur++;
				len++;
				_remain_size--;
				_next_start++;
			}
		}

		// Check if the last part is followed by . 
		if(_remain_size > 0 && len > 0 && *cur == '.')
		{
			cur++;
			len++;
			_remain_size--;
			_next_start++;

			continue;
		}

		break;
	}

	if(exists == true)
	{
		token->type = TOKEN_IDENT;
		token->str = start;
		token->len = len;
		token->line = _line;
		token->remain_size = _remain_size;
		token->next_start = _next_start + len;

		_tokens.Add(token);

		//ConvertIdentifier(token);
	}

	return exists;
}

// Get string literal
bool SqlParser::GetStringLiteral(Token *token)
{
	if(token == NULL)
		return false;

	const char *cur = _next_start;
	const char *start = cur;

	bool exists = false;

	size_t len = 0;

	// 'literal'
	if(*cur == '\'')
	{
		len++;
		cur++;

		_remain_size--;
		_next_start++;
	}
	else
	// N'literal' in SQL Server (spaces between N and ' not allowed, N must be upper case)
	if(*cur == 'N' && _remain_size > 0 && cur[1] == '\'')
	{
		len += 2;
		cur += 2;

		_remain_size -= 2;
		_next_start += 2;
	}
	else
		return false;

	// Skip until the terminating quote found
	while(_remain_size > 0)
	{
		// Count lines
		if(*cur == '\n')
			_line++;

		if(*cur == '\'')
		{
			// Check for '' escaping
			if(_remain_size > 1 && cur[1] == '\'')
			{
				cur += 2;
				len += 2;

				_remain_size -= 2;
				_next_start += 2;

				continue;
			}

			break;
		}

		cur++;
		len++;

		_remain_size--;
		_next_start++;
	}

	if(*cur == '\'')
	{
		cur++;
		_remain_size--;
		_next_start++;

		token->type = TOKEN_STRING;
		token->data_type = TOKEN_DT_STRING;
		token->nullable = false;
		token->chr = 0;
		token->wchr = 0;
		token->str = start;
		token->wstr = NULL;
		token->len = len + 1;
		token->remain_size = _remain_size;

		_tokens.Add(token);

		exists = true;
	}

	return exists;
}

// Parse a comment token 
bool SqlParser::ParseComment()
{
	bool exists = false;

	// Multiple consecutive comments can be specified
	while(true)
	{	
		SkipSpaceTokens();

		const char *cur = _next_start;

		// Single line comment --
		if(_remain_size >= 2  && *cur == '-' && cur[1] == '-')
		{
			// Use 2 tokens to represent the comment
			Token *start = new Token();
			start->type = TOKEN_COMMENT;
			start->str = cur;
			start->len = 2;
			start->remain_size = _remain_size;

			_tokens.Add(start);

			_next_start += 2;
			_remain_size -= 2;
			cur += 2;

			// MySQL requires a blank after --
			if(Target(SQL_MARIADB, SQL_MYSQL) && _remain_size > 0 && *cur != ' ' && *cur != '\t') 
			{
				Token *space = new Token();
				space->type = TOKEN_SYMBOL;
				space->chr = ' ';

				_tokens.Add(space);
			}
		
			const char *text = cur;
			size_t len = 0;

			// Go until the end of line
			while(_remain_size > 0)
			{
				if(*cur == '\r' || *cur == '\n')
					break;

				_next_start++;
				_remain_size--;
				cur++;
				len++;
			}

			Token *token = new Token();
			token->type = TOKEN_COMMENT;
			token->chr = 0;
			token->wchr = 0;
			token->str = text;
			token->wstr = NULL;
			token->len = len;
			token->remain_size = _remain_size;

			_tokens.Add(token);
			
			exists = true;
			continue;
		}
		else
		// Multiline comment /* */
		if(_remain_size >= 2  && *cur == '/' && cur[1] == '*')
		{
			const char *start = cur;
			size_t len = 0;

			_next_start += 2;
			_remain_size -= 2;
			cur += 2;
			len += 2;

			// Go until */
			while(_remain_size > 0)
			{
				if(_remain_size >= 2  && *cur == '*' && cur[1] == '/')
				{
					_next_start += 2;
					_remain_size -= 2;
					cur += 2;
					len += 2;

					break;
				}

				// Count lines
				if(*cur == '\n')
					_line++;

				_next_start++;
				_remain_size--;
				cur++;
				len++;
			}

			Token *token = new Token();
			token->type = TOKEN_COMMENT;
			token->chr = 0;
			token->wchr = 0;
			token->str = start;
			token->wstr = NULL;
			token->len = len;
			token->remain_size = _remain_size;

			_tokens.Add(token);
			
			exists = true;
			continue;
		}
		else
		// Informix comment {}
		if(_source == SQL_INFORMIX && *cur == '{')
		{
			// Use 3 tokens to represent the comment
			Token *start = new Token();
			start->type = TOKEN_COMMENT;
			start->chr = *cur;
			start->remain_size = _remain_size;

			// Change to /* if target not Informix
			if(_target != SQL_INFORMIX)
			{
				start->t_str = Str::GetCopy("/*", 2);
				start->t_len = 2;
			}

			_tokens.Add(start);

			_next_start++;
			_remain_size--;
			cur++;
		
			size_t len = 0;
			const char *text = cur;

			// Go until }
			while(_remain_size > 0)
			{
				if(*cur == '}')
					break;

				// Count lines
				if(*cur == '\n')
					_line++;

				_next_start++;
				_remain_size--;
				cur++;
				len++;
			}

			Token *token = new Token();
			token->type = TOKEN_COMMENT;
			token->str = text;
			token->len = len;
			token->remain_size = _remain_size;

			_tokens.Add(token);

			Token *end = new Token();
			end->type = TOKEN_COMMENT;
			end->chr = *cur;
			end->remain_size = _remain_size;

			// Change to */ if target not Informix
			if(_target != SQL_INFORMIX)
			{
				end->t_str = Str::GetCopy("*/", 2);
				end->t_len = 2;
			}

			_tokens.Add(end);

			_next_start++;
			_remain_size--;

			exists = true;
			continue;
		}
		else
		// Sybase ASA, Sybase ADS C++ style comment //
		if(Source(SQL_SYBASE_ASA, SQL_SYBASE_ADS) && _remain_size >= 2  && *cur == '/' && cur[1] == '/')
		{
			// Use 2 tokens to represent the comment
			Token *start = new Token();
			start->type = TOKEN_COMMENT;
			start->str = cur;
			start->len = 2;
			start->remain_size = _remain_size;

			// Change to -- if target not Sybase ASA, Sybase ADS
			if(!Target(SQL_SYBASE_ASA, SQL_SYBASE_ADS))
			{
				start->t_str = Str::GetCopy("--", 2);
				start->t_len = 2;
			}

			_tokens.Add(start);

			_next_start += 2;
			_remain_size -= 2;
			cur += 2;
		
			size_t len = 0;
			const char *text = cur;

			// Go until the end of line
			while(_remain_size > 0)
			{
				if(*cur == '\r' || *cur == '\n')
					break;

				_next_start++;
				_remain_size--;
				cur++;
				len++;
			}	

			Token *token = new Token();
			token->type = TOKEN_COMMENT;
			token->str = text;
			token->len = len;
			token->remain_size = _remain_size;

			_tokens.Add(token);

			exists = true;
			continue;
		}
		else
		// MySQL # single line comment 
		if(_source == SQL_MYSQL && _remain_size >= 1  && *cur == '#')
		{
			// Use 2 tokens to represent the comment
			Token *start = new Token();
			start->type = TOKEN_COMMENT;
			start->chr = *cur;
			start->remain_size = _remain_size;

			// Change to -- for other databases
			if(_target != SQL_MYSQL)
			{
				start->t_str = Str::GetCopy("--", 2);
				start->t_len = 2;
			}

			_tokens.Add(start);

			_next_start++;
			_remain_size--;
			cur++;
		
			size_t len = 0;
			const char *text = cur;

			// Go until the end of line
			while(_remain_size > 0)
			{
				if(*cur == '\r' || *cur == '\n')
					break;

				_next_start++;
				_remain_size--;
				cur++;
				len++;
			}

			Token *token = new Token();
			token->type = TOKEN_COMMENT;
			token->str = text;
			token->len = len;
			token->remain_size = _remain_size;

			_tokens.Add(token);

			exists = true;
			continue;
		}
		else
		// COBOL * single line comment (* in 7 column)
		if(_source_app == APP_COBOL && _remain_size >= 1  && *cur == '*')
		{
			// Use 2 tokens to represent the comment
			Token *start = new Token();
			start->type = TOKEN_COMMENT;
			start->chr = *cur;
			start->remain_size = _remain_size;

			// Change to -- if the target application is not COBOL
			if(_target_app != APP_COBOL)
			{
				start->t_str = Str::GetCopy("-- ", 3);
				start->t_len = 3;
			}

			_tokens.Add(start);

			_next_start++;
			_remain_size--;
			cur++;
		
			size_t len = 0;
			const char *text = cur;

			// Go until the end of line
			while(_remain_size > 0)
			{
				if(*cur == '\r' || *cur == '\n')
					break;

				_next_start++;
				_remain_size--;
				cur++;
				len++;
			}

			Token *token = new Token();
			token->type = TOKEN_COMMENT;
			token->str = text;
			token->len = len;
			token->remain_size = _remain_size;

			_tokens.Add(token);

			exists = true;
			continue;
		}

		// Not a comment 
		break;
	}	

	// Skip spaces following the comment
	if(exists == true)
		SkipSpaceTokens();

	return exists;
}

// Get a word token
bool SqlParser::GetWordToken(Token *token)
{
	if(token == NULL)
		return false;

	size_t len = 0;

	const char *cur = _next_start;

	// Check for a sign for numbers in the first position (sign can go before variable name -num i.e)
	if(_remain_size > 1 && (*cur == '+' || *cur == '-') /*&& cur[1] >= '0' && cur[1] <= '9'*/ &&
		// Skip comment --
		cur[1] != '-') 
	{
		char sign = *cur;

		_remain_size--;
		len++;
		cur++;

		// Allow spaces follow the sign
		while(sign == '-' && _remain_size > 1 && *cur == ' ')
		{
			_remain_size--;
			len++;
			cur++;
		}
	}

	// Identifiers starts as a word but then there is quoted part SCHEMA."TABLE".COL i.e.
	bool partially_quoted_identifier = false;

	// Calculate the length
	while(_remain_size > 0)
	{
		// Check for a comment
		if(len == 0 && ParseComment() == true)
		{
			cur = _next_start;
			continue;
		}

		// Check whether we meet a special character allowed in identifiers
		if(strchr(g_symbols, *cur) != NULL)
		{
			// @variable in SQL Server and MySQL, :new in Oracle trigger, #temp table name in SQL Server
			// * meaning all columns, - in COBOL identifier, label : label name in DB2
            // $ or $$ often used as replacement markers, $ is also allowed in Oracle identifiers
			if(*cur != '_' && *cur != '.' && *cur != '@' && *cur != ':' && *cur != '#' && *cur != '*' && 
					*cur != '-' && *cur != '"' && *cur != '[' && *cur != ' ' && *cur != '&' && *cur != '$')
				break;

			// Spaces are allowed between identifier parts: table . name 
			if(*cur == ' ')
			{
				int ident_len = 0;

				for(int i = 0; i < _remain_size - 1; i++)
				{
					if(cur[i] == ' ' || cur[i] == '\t' || cur[i] == '\n' || cur[i] == '\r')
						continue;

					if(cur[i] == '.')
						ident_len = i;

					break;
				}

				// Not a multi-part identifier
				if(len == 0 || ident_len == 0)
					break;

				_remain_size -= ident_len;
				cur += ident_len;
				len += ident_len;

				continue;
			}

			// .. in numeric range 1..10 (FOR loop i.e)
			if(!Source(SQL_SQL_SERVER, SQL_SYBASE) && *cur == '.')
			{
				// first . followed by second
				if(_remain_size > 1 && cur[1] == '.')
					break;
				else
				// second . 
				if(cur > _start && cur[-1] == '.')
					break;
			}

			// * must be after . to not confuse with multiplication operator
			if(*cur == '*' && (len == 0 || (len > 0 && cur > _start && cur[-1] != '.')))
				break;

			// Check for partially quoted identifier that starts as a word then quoted part follows
			if(*cur == '"' || *cur == '[')
			{
				if(len > 0 && cur > _start && cur[-1] == '.')
					partially_quoted_identifier = true;
				
				break;
			}

			if(*cur == ':')
			{
				// But := also means assigment in Oracle (space is not allowed between : and =)
				if((_remain_size > 1 && cur[1] == '=') ||
				  // In DB2, Teradata, MySQL : used in label, and label:BEGIN (without spaces) or 
				  // label :BEGIN is correct, but :param can be also used in scripts
				  (Source(SQL_DB2, SQL_TERADATA, SQL_MYSQL) == true && 
						IsScope(SQL_SCOPE_SELECT_STMT) == false) ||
				  // In Informix, PostgreSQL :: is data type cast operator
				  (_remain_size > 1 && cur[1] == ':') || (cur > _start && cur[-1] == ':'))
				break;
			}

			// : can follow after label in DB2
			//if(*cur == ':' && len == 0 && _remain_size > 1 && Str::IsSpace(cur[1]) == true)
			//	break;

			// & used as parameter marker in scripts i.e. SQL*Plus, must be at the first position
			if(*cur == '&' && len != 0)
				break;

			// Allow - in COBOL only
			if(*cur == '-' && _source_app != APP_COBOL && _level != LEVEL_APP)
				break;

			bool right = true;

			// @ must not be followed by a blank or delimiter
			if(*cur == '@')
			{
				// Remain size not decremented yet
				if(_remain_size == 1 || 
					(_remain_size > 1 && (cur[1] == ' ' || cur[1] == '\r' || cur[1] == '\n' || cur[1] == '\t')))
					right = false;
			}
			else
			// . is the statement delimiter in COBOL
			if(_source_app == APP_COBOL && _level == LEVEL_APP && *cur == '.')
				right = false;

			if(right == false)
				break;
		}

		_remain_size--;
		cur++;
		len++;
	}

	if(partially_quoted_identifier == true)
	{
		_remain_size += len;

		GetQuotedIdentifier(token, true);
	}
	else			
	if(len > 0)
	{
		// If a single special character was selected in the right position, but no more characters followed
		// do not return as word
		if(len == 1 && (strchr(g_symbols, *_next_start) != NULL ||
			// Also skip N'literal' in SQL Server
			(*_next_start == 'N' && _remain_size > 1 && *cur == '\'')))
		{
			_remain_size++;
			return false;
		}

		token->type = TOKEN_WORD;
		token->chr = 0;
		token->wchr = 0;
		token->str = _next_start;
		token->wstr = 0;
		token->len = len;
		token->line = _line;
		token->remain_size = _remain_size;
		token->next_start = _next_start + len;

		_tokens.Add(token);

		_next_start = cur;

		// Check for non-ASCII 7-bit characters
		if(_stats)
		{
			bool non_ascii = false;

			for(unsigned int i = 0; i < len; i++)
			{
				if(((unsigned char)token->str[i]) > 127)
				{
					non_ascii = true;
					break;
				}
			}

			if(non_ascii)
				_stats->Non7BitAsciiIdents(token);
		}
	}

	return (len > 0) ? true : false;
}

// Return the last fetched token to the input
void SqlParser::PushBack(Token *token)
{
	if(token == NULL)
		return;

	_push_back_token = token;
}

// Check next token for the specific value but do not fecth it from the input
Token* SqlParser::LookNext(const char *str, const wchar_t *wstr, size_t len)
{
	Token *token = GetNext(str, wstr, len);

	if(token != NULL)
		PushBack(token);

	return token;
}

// Append the token with the specified value
Token* SqlParser::Append(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format)
{
	if(token == NULL)
		return NULL;

	Token *append = new Token();

	if(format == NULL)
		*append = *token;
	else
		*append = *format;

	append->prev = NULL;
	append->next = NULL;

	append->t_str = NULL;
	append->t_wstr = NULL;
	append->t_len = 0;
	
	// If token starts with newline, add the same number of spaces before appended token
	if(str != NULL && str[0] == '\n')
	{
		// Add newline token first
		Token newline;

#ifdef WIN32
		newline.type = TOKEN_SYMBOL;
		newline.chr = '\r';

	    AppendCopy(token, &newline);
#endif

		newline.type = TOKEN_SYMBOL;
		newline.chr = '\n';
		
	    AppendCopy(token, &newline);

		Token *prev = (format != NULL) ? format->prev : token->prev;

		// Copy all blankes 
		while(prev != NULL && (prev->Compare(' ', L' ') == true || prev->Compare('\t', L'\t') == true))
		{
			AppendCopy(token, prev);
			prev = prev->prev;
		}

		// Skip new line in the token
		Token::Change(append, str + 1, wstr + 1, len - 1);
	}
	else
		Token::Change(append, str, wstr, len);

    Append(token, append);

	return append;
}

// Append token and make sure it followed by a space
Token* SqlParser::AppendWithSpaceAfter(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format)
{
    if(token->next != NULL && !token->next->IsBlank())
        PrependNoFormat(token->next, " ", L" ", 1);

    return Append(token, str, wstr, len, format);
}

// Append an integer value
void SqlParser::Append(Token *token, int value)
{
	if(token == NULL)
		return;

	Token *append = new Token();

	append->t_str = new char[11];
	sprintf((char*)append->t_str, "%d", value);

	append->t_wstr = NULL;
	append->t_len = strlen(append->t_str);

	Append(token, append);
}

void SqlParser::Append(Token *token, TokenStr *str, Token *format)
{
	if(token == NULL || str == NULL)
		return;

	Append(token, str->str.c_str(), str->wstr.c_str(), str->len, format);
}

// Append the token with formatting immediately after the specified token
void SqlParser::AppendFirst(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format)
{
	if(token == NULL)
		return;

	Token *append = new Token();

    if(format == NULL)
		*append = *token;
	else
		*append = *format;

	append->prev = NULL;
	append->next = NULL;

	append->t_str = NULL;
	append->t_wstr = NULL;
	append->t_len = 0;

    Token::Change(append, str, wstr, len);

	append->flags = TOKEN_INSERTED;

	_tokens.Append(token, append);
}	

// Append the token without formatting by appended token style
Token* SqlParser::AppendNoFormat(Token *token, const char *str, const wchar_t * /*wstr*/, size_t len)
{
	if(token == NULL)
		return NULL;

	Token *append = new Token();

	// Initalize source values in the token
	*append = *token; 
	append->prev = NULL;
	append->next = NULL;

	append->t_str = Str::GetCopy(str, len);
	append->t_wstr = NULL;
	append->t_len = len;

	Append(token, append);

	return append;
}	

void SqlParser::AppendNoFormat(Token *token, TokenStr *str)
{
	if(token == NULL || str == NULL)
		return;

	AppendNoFormat(token, str->str.c_str(), str->wstr.c_str(), str->len);
}

// Append the token without formatting immediately after specified token
void SqlParser::AppendFirstNoFormat(Token *token, const char *str, const wchar_t * /*wstr*/, size_t len)
{
	if(token == NULL)
		return;

	Token *append = new Token();

	// Initalize source values in the token
	*append = *token; 
	append->prev = NULL;
	append->next = NULL;

	append->t_str = Str::GetCopy(str, len);
	append->t_wstr = NULL;
	append->t_len = len;

	append->flags = TOKEN_INSERTED;

	_tokens.Append(token, append);
}	

// Append a copy of the token
Token* SqlParser::AppendCopy(Token *token, Token *app)
{
	if(token == NULL || app == NULL)
		return NULL;

	Token *append = new Token();
	*append = *app;

	// Reallocate source if space is not in place as the original token can be removed later
	if(app->source_allocated)
	{
		if(app->str != NULL)
			append->str = Str::GetCopy(app->str, app->len);
	}

	// Reallocate target values to separate buffers if set
	if(app->t_len != 0)
	{
		if(app->t_str != NULL)
			append->t_str = Str::GetCopy(app->t_str, app->t_len);
	}
	
	Append(token, append);

	return append;
}

// Append the range of tokens (inclusively)
Token* SqlParser::AppendCopy(Token *token, Token *first, Token *last, bool append_removed)
{
	if(token == NULL || first == NULL)
		return NULL;

	Token *cur = first;
	Token *new_first = NULL;
    int cnt = 1000;

	while(cur != NULL && cnt != 0)
	{
		bool removed = cur->IsRemoved();

		// Check whether we need to skip already removed tokens
		if(removed == false || (removed == true && append_removed == true))
		{
			Token *append = AppendCopy(token, cur);

			if(new_first == NULL)
				new_first = append;
		}

		// Append single token only
		if(first == last || last == NULL)
			break;

		// Tokens appended inclusively
		if(cur == last)
			break;

		cur = cur->next;
        cnt--;
	}

	return new_first;
}

// Append a space and specified token
void SqlParser::AppendSpaceCopy(Token *token, Token *append)
{
	if(token == NULL || append == NULL)
		return;

	AppendNoFormat(token, " ", L" ", 1);
	AppendCopy(token, append);
}

void SqlParser::AppendSpaceCopy(Token *token, Token *first, Token *last, bool append_removed)
{
	if(token == NULL)
		return;

	AppendNoFormat(token, " ", L" ", 1);
	AppendCopy(token, first, last, append_removed);
}

void SqlParser::AppendNewlineCopy(Token *token, Token *first, Token *last, size_t newlines, bool append_removed)
{
	if(token == NULL && newlines > 3)
		return;

    char c[3] = {'\n', '\n', '\n' };
    wchar_t w[3] = {L'\n', L'\n', L'\n' };

    AppendNoFormat(token, c, w, newlines);
	AppendCopy(token, first, last, append_removed);
}

// Append the token
void SqlParser::Append(Token *token, Token *append)
{
	if(token == NULL || append == NULL)
		return;

	append->flags = TOKEN_INSERTED;

	Token *cur = token;

	// Append to the end of already appended tokens
	while(cur != NULL)
	{
		if(cur->next == NULL || (cur->next->flags & TOKEN_INSERTED) == 0)
			break;

		cur = cur->next;
	}

	_tokens.Append(cur, append);
}

// Prepend the token with the specified value
Token* SqlParser::Prepend(Token *token, const char *str, const wchar_t *wstr, size_t len, Token *format)
{
	if(token == NULL)
		return NULL;

	Token *prepend = new Token();

	// Define how to format the token
	if(format == NULL)
		*prepend = *token;
	else
		*prepend = *format;

	prepend->prev = NULL;
	prepend->next = NULL;
	prepend->t_str = NULL;
	prepend->t_wstr = NULL;
	prepend->t_len = 0;

	prepend->flags = TOKEN_INSERTED;

	Token::Change(prepend, str, wstr, len);
	Prepend(token, prepend);

	// If token ends with newline, add the same number of spaces after prepended token
	if(str != NULL && str[len - 1] == '\n')
	{
		Token *prev = token->prev;

		// Skip previously appended tokens
		while(prev != NULL && prev->flags & TOKEN_INSERTED)
			prev = prev->prev;

		// Copy all blanks and tabs before appended token after new line
		while(prev != NULL && (prev->Compare(' ', L' ') == true || prev->Compare('\t', L'\t') == true))
		{
			AppendCopy(prepend, prev);
			prev = prev->prev;
		}
	}

	return prepend;
}

Token* SqlParser::Prepend(Token *token, TokenStr *str, Token *format)
{
	if(token == NULL || str == NULL)
		return NULL;

	return Prepend(token, str->str.c_str(), str->wstr.c_str(), str->len, format);
}

// Prepend the token without formatting
Token* SqlParser::PrependNoFormat(Token *token, const char *str, const wchar_t * /*wstr*/, size_t len)
{
	if(token == NULL)
		return NULL;

	Token *prepend = new Token();

	// Initalize source values in the token
	*prepend = *token; 
	prepend->prev = NULL;
	prepend->next = NULL;

	prepend->t_str = Str::GetCopy(str, len);
	prepend->t_wstr = NULL;
	prepend->t_len = len;

	Prepend(token, prepend);

	return prepend;
}

Token* SqlParser::PrependNoFormat(Token *token, TokenStr *str)
{
	if(token == NULL || str == NULL)
		return NULL;

	return PrependNoFormat(token, str->str.c_str(), str->wstr.c_str(), str->len);
}

// Prepend a copy of the token
Token* SqlParser::PrependCopy(Token *token, Token *pre)
{
	if(token == NULL || pre == NULL)
		return NULL;

	Token *prepend = new Token();
	*prepend = *pre;

	prepend->flags = TOKEN_INSERTED;

	// Reallocate source values to separate buffers if set
	if(pre->source_allocated == true)
	{
		prepend->str = Str::GetCopy(pre->str, pre->len);
	}

	// Reallocate target values to separate buffers if set
	if(pre->t_len != 0)
	{
		if(pre->t_str != NULL)
			prepend->t_str = Str::GetCopy(pre->t_str, pre->t_len);
	}
	
	_tokens.Prepend(token, prepend);

	return prepend;
}

// Prepend the range of tokens (inclusively)
Token* SqlParser::PrependCopy(Token *token, Token *first, Token *last, bool prepend_removed)
{
	if(token == NULL || first == NULL)
		return NULL;

	Token *cur = first;
	Token *last_added = NULL;

	while(cur != NULL)
	{
		bool removed = cur->IsRemoved();

		// Check whether we need to skip already removed tokens
		if(removed == false || (removed == true && prepend_removed == true))
			last_added = PrependCopy(token, cur);

		// Prepend single token only
		if(first == last || last == NULL)
			break;

		// Tokens prepended inclusively
		if(cur == last)
			break;

		cur = cur->next;
	}

	return last_added;
}

void SqlParser::PrependSpaceCopy(Token *token, Token *first, Token *last, bool prepend_removed)
{
	if(token == NULL)
		return;

	PrependCopy(token, first, last, prepend_removed);
	PrependNoFormat(token, " ", L" ", 1);
}

// Prepend the token
void SqlParser::Prepend(Token *token, Token *prepend)
{
	if(token == NULL || prepend == NULL)
		return;

	prepend->flags = TOKEN_INSERTED;

	_tokens.Prepend(token, prepend);
}

// Prepend the token without formatting immediately before the specified token
void SqlParser::PrependFirstNoFormat(Token *token, const char *str, const wchar_t * /*wstr*/, size_t len)
{
	if(token == NULL)
		return;

	Token *prepend = new Token();

	// Initalize source values in the token
	*prepend = *token; 
	prepend->prev = NULL;
	prepend->next = NULL;

	prepend->t_str = Str::GetCopy(str, len);
	prepend->t_wstr = NULL;
	prepend->t_len = len;

	prepend->flags = TOKEN_INSERTED;

	_tokens.Prepend(token, prepend);
}	

// Change the token and add spaces around it if they do not exist
void SqlParser::ChangeWithSpacesAround(Token *token, const char *new_str, const wchar_t *new_wstr, size_t len, Token *format)
{
	if(token == NULL)
		return;

	if(token->prev != NULL && token->prev->IsBlank() == false)
		PrependNoFormat(token, " ", L" ", 1);

	if(token->next != NULL && token->next->IsBlank() == false)
		AppendNoFormat(token, " ", L" ", 1);

	Token::Change(token, new_str, new_wstr, len, format);
}

// Check whether the token is valid column/table alias
bool SqlParser::IsValidAlias(Token *token)
{
	if(token == NULL)
		return false;

	bool alias = true;

	// DB2 allows almost any keyword as alias including WHERE, ORDER, SELECT 
	if(Source(SQL_ORACLE, SQL_SQL_SERVER, SQL_DB2, SQL_INFORMIX, SQL_SYBASE) == false)
	{
		// List of not valid aliases for all databases
		const char **a_array = g_no_alias;
		const wchar_t **w_array = g_no_alias_w;
		size_t *size = g_no_alias_size;

		int i = 0;

		// Check is the word is valid for alias
		while(size[i] != 0)
		{
			if(token->Compare(a_array[i], w_array[i], size[i]) == true)
			{
				alias = false;
				break;
			}

			i++;
		}
	}

	// Check Oracle list
	if(_source == SQL_ORACLE)
	{
		// Words not allowed as alias in Oracle
		if(Token::Compare(token, "CONNECT", L"CONNECT", 7) == true || 
			Token::Compare(token, "END", L"END", 3) == true || 
			Token::Compare(token, "GROUP", L"GROUP", 5) == true || 
			Token::Compare(token, "MINUS", L"MINUS", 5) == true || 
			Token::Compare(token, "ORDER", L"ORDER", 5) == true || 
			Token::Compare(token, "RETURN", L"RETURN", 6) == true || 
			Token::Compare(token, "SELECT", L"SELECT", 6) == true || 
			Token::Compare(token, "START", L"START", 5) == true /* START WITH after FROM */ ||  
			Token::Compare(token, "UNION", L"UNION", 5) == true ||
			Token::Compare(token, "UPDATE", L"UPDATE", 6) == true ||
			Token::Compare(token, "WHERE", L"WHERE", 5) == true ||
			Token::Compare(token, "WITH", L"WITH", 4) == true)
			alias = false;
	}
	else
	// Check SQL Server and Sybase ASE list
	if(_source == SQL_SQL_SERVER || _source == SQL_SYBASE)
	{
		// Words not allowed as alias in SQL Server
		if(TOKEN_CMP(token, "ALTER") || TOKEN_CMP(token, "CLOSE") || TOKEN_CMP(token, "DECLARE") ||
			TOKEN_CMP(token, "ELSE") || 
			TOKEN_CMP(token, "END") || 
			TOKEN_CMP(token, "EXEC") || 
			TOKEN_CMP(token, "FETCH") ||
			TOKEN_CMP(token, "IF") ||
			TOKEN_CMP(token, "INSERT") ||
			TOKEN_CMP(token, "GO") ||
			TOKEN_CMP(token, "GROUP") ||
			TOKEN_CMP(token, "ORDER") || 
			TOKEN_CMP(token, "RETURN") || 
			TOKEN_CMP(token, "SELECT") || 
			TOKEN_CMP(token, "SET") ||
			TOKEN_CMP(token, "UNION") ||
			TOKEN_CMP(token, "UPDATE") ||
			TOKEN_CMP(token, "WHERE"))
			alias = false;
	}
	else
	// For DB2 check the next word
	if(_source == SQL_DB2)
	{
		Token *next = NULL;

		// DB2 allows WHERE as alias, but does not allow for example WHERE ORDER BY
		// Our parser currently does not allow WHERE as alias
		if(token->Compare("WHERE", L"WHERE", 5) == true)
			alias = false;
		else
		// GROUP BY or ORDER BY
		if(token->Compare("GROUP", L"GROUP", 5) == true || token->Compare("ORDER", L"ORDER", 5) == true)
			next = GetNextWordToken("BY", L"BY", 2);
		else
		// FETCH FIRST
		if(token->Compare("FETCH", L"FETCH", 5) == true)
			next = GetNextWordToken("FIRST", L"FIRST", 5);
		else
		// WITH UR, CS, RS, RR
		if(token->Compare("WITH", L"WITH", 4) == true)
		{
			next = GetNextWordToken("UR", L"UR", 2);

			if(next == NULL) 
				next = GetNextWordToken("CS", L"CS", 2);

			if(next == NULL) 
				next = GetNextWordToken("RS", L"RS", 2);

			if(next == NULL) 
				next = GetNextWordToken("RR", L"RR", 2);
		}

		if(next != NULL)
		{
			PushBack(next);
			alias = false;
		}
	}
	else
	// Check Informix list
	if(_source == SQL_INFORMIX)
	{
		// Words not allowed as alias in Informix
		if(Token::Compare(token, "END", L"END", 3) == true || 
			Token::Compare(token, "INTO", L"INTO", 4) == true ||
			Token::Compare(token, "GROUP", L"GROUP", 5) == true ||
			Token::Compare(token, "ORDER", L"ORDER", 5) == true || 
			Token::Compare(token, "RETURN", L"RETURN", 6) == true || 
			Token::Compare(token, "SELECT", L"SELECT", 6) == true || 
			Token::Compare(token, "UNION", L"UNION", 5) == true ||
			Token::Compare(token, "UPDATE", L"UPDATE", 6) == true ||
			Token::Compare(token, "WHERE", L"WHERE", 5) == true)
			alias = false;
	}
	else
	// Check Teradata list
	if(_source == SQL_TERADATA)
	{
		// Words not allowed as alias in Teradata
		if(Token::Compare(token, "DO", L"DO", 2) == true || 
			Token::Compare(token, "END", L"END", 3) == true || 
			Token::Compare(token, "INTO", L"INTO", 4) == true ||
			Token::Compare(token, "GROUP", L"GROUP", 5) == true ||
			Token::Compare(token, "ORDER", L"ORDER", 5) == true || 
			Token::Compare(token, "RETURN", L"RETURN", 6) == true || 
			Token::Compare(token, "SELECT", L"SELECT", 6) == true || 
			Token::Compare(token, "UNION", L"UNION", 5) == true ||
			Token::Compare(token, "UPDATE", L"UPDATE", 6) == true ||
			Token::Compare(token, "WHERE", L"WHERE", 5) == true)
			alias = false;
	}
	else
	// MySQL list
	if(_source == SQL_MYSQL)
	{
		// Words not allowed as alias in MySQL
		if(Token::Compare(token, "END", L"END", 3) == true || 
			Token::Compare(token, "LIMIT", L"LIMIT", 5) == true || 
			Token::Compare(token, "ORDER", L"ORDER", 5) == true || 
			Token::Compare(token, "RETURN", L"RETURN", 6) == true || 
			Token::Compare(token, "SELECT", L"SELECT", 6) == true || 
			Token::Compare(token, "UNION", L"UNION", 5) == true ||
			Token::Compare(token, "UPDATE", L"UPDATE", 6) == true ||
			Token::Compare(token, "WHERE", L"WHERE", 5) == true)
			alias = false;
	}
	else
	// Check Sybase ASE list
	if(_source == SQL_SYBASE)
	{
		// Words not allowed as alias in Sybase
		if(Token::Compare(token, "DECLARE", L"DECLARE", 7) == true || 
			Token::Compare(token, "END", L"END", 3) == true || 
			Token::Compare(token, "EXEC", L"EXEC", 4) == true || 
			Token::Compare(token, "FETCH", L"FETCH", 5) == true ||
			Token::Compare(token, "GO", L"GO", 2) == true ||
			Token::Compare(token, "ORDER", L"ORDER", 5) == true || 
			Token::Compare(token, "RETURN", L"RETURN", 6) == true || 
			Token::Compare(token, "SELECT", L"SELECT", 6) == true || 
			Token::Compare(token, "UNION", L"UNION", 5) == true ||
			Token::Compare(token, "UPDATE", L"UPDATE", 6) == true ||
			Token::Compare(token, "WHERE", L"WHERE", 5) == true)
			alias = false;
	}

	return alias;
}

// Comment text
void SqlParser::Comment(Token *first, Token *last)
{
	PrependNoFormat(first, "/* ", L"/* ", 3);
	AppendNoFormat(Nvl(last, first), " */", L" */", 3); 
}

void SqlParser::CommentNoSpaces(Token *first, Token *last)
{
	PrependNoFormat(first, "/*", L"/*", 2);
	AppendNoFormat(Nvl(last, first), "*/", L"*/", 2); 
}

void SqlParser::Comment(const char *word, const wchar_t *w_word, size_t len, Token *first, Token *last)
{
	PrependNoFormat(first, "/* ", L"/* ", 3);
	PrependNoFormat(first, word, w_word, len);
	AppendNoFormat(Nvl(last, first), " */", L" */", 3);
}

// Parse Unicode byte order mark
Token* SqlParser::GetBomToken()
{
	Token *token = NULL;

	const unsigned char *cur = (const unsigned char *)_next_start;

	// UTF-8 EF BB BF
	if(_remain_size >= 3 && cur[0] == 0xEF && cur[1] == 0xBB && cur[2] == 0xBF)
	{
		token = new Token();

		token->type = TOKEN_BOM;
		token->str = _next_start;
		token->len = 3;
		token->remain_size = _remain_size - 3;

		_tokens.Add(token);

		_next_start += 3;
		_remain_size -= 3;
	}

	return token;
}

// Create report file
int SqlParser::CreateReport(const char *summary)
{
    if(_report != NULL)
    {
        _report->CreateReport(_stats, _source, _target, summary);
        return 0;
    }

    return -1;
}
