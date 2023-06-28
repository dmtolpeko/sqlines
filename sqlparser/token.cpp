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

// Token class

#include <stdio.h>
#include <string.h>
#include "token.h"
#include "str.h"

// Constructor/destructor
Token::Token()
{
	type = 0;
	subtype = 0;
	t_type = 0;
	data_type = 0;
	data_subtype = 0;
	datatype_meta = NULL;
	table = NULL;
	nullable = true;

	chr = 0;
	wchr = 0; 
	str = NULL;
	wstr = NULL;

	t_str = NULL;
	t_wstr = NULL;
	t_len = 0;

	len = 0;
	flags = 0;

    notes_str = NULL;

	remain_size = 0;
	next_start = NULL;
	line = 0;
	source_allocated = false;

	open = NULL;
	close = NULL;

	prev = NULL;
	next = NULL;
}

Token::~Token()
{
	// Delete source values only if allocated flag is set
	if(source_allocated == true)
	{
		delete str;
		delete wstr;
		str = NULL;
		wstr = NULL;
	}

	// Delete target values
	delete t_str;
	delete t_wstr;
	t_str = NULL;
	t_wstr = NULL;
}

// Get a copy of token
Token* Token::GetCopy(Token *source)
{
	if(source == NULL)
		return NULL;

	Token *token = new Token();
	*token = *source;

	token->prev = NULL;
	token->next = NULL;

	// Reallocate source if space is not in place as the original token can be removed later
	if(source->source_allocated)
	{
		if(source->str != NULL)
			token->str = Str::GetCopy(source->str, source->len);
	}

	// Reallocate target values to separate buffers if set
	if(source->t_len != 0)
	{
		if(source->t_str != NULL)
			token->t_str = Str::GetCopy(source->t_str, source->t_len);
	}

	return token;
}

// Compare token value with the specified word
bool Token::Compare(const char *word, const wchar_t *w_word, size_t len)
{
	return Token::Compare(this, word, w_word, len);
}

bool Token::Compare(const char *word, const wchar_t *w_word, size_t start, size_t len)
{
	return Token::Compare(this, word, w_word, start, len);
}

bool Token::Compare(Token *token, const char *word, const wchar_t * /*w_word*/, size_t len)
{
	if(token == NULL)
		return false;

	if(token->str != NULL && token->len == len && !_strnicmp(token->str, word, len))
		return true;

	return false;
}

// Compare token fragment
bool Token::Compare(Token *token, const char *word, const wchar_t * /*w_word*/, size_t start, size_t len)
{
	if(token == NULL)
		return false;

	if(token->str != NULL && token->len >= start + len && !_strnicmp(token->str + start, word, len))
		return true;

	return false;
}

// Compare token fragment (case sensitive)
bool Token::CompareCS(Token *token, const char *word, const wchar_t * /*w_word*/, size_t start, size_t len)
{
	if(token == NULL)
		return false;

	if(token->str != NULL && !strncmp(token->str + start, word, len))
		return true;

	return false;
}

// Compare single character token value with the specified character
bool Token::Compare(const char ch, const wchar_t wch)
{
	return Token::Compare(this, ch, wch);
}

bool Token::Compare(const char ch, const wchar_t wch, size_t start)
{
	return Token::Compare(this, ch, wch, start);
}

bool Token::Compare(Token *token, const char ch, const wchar_t wch)
{
	if(token == NULL)
		return false;

	if(token->chr == ch || token->wchr == wch)
		return true;

	return false;
}

bool Token::Compare(Token *token, const char ch, const wchar_t wch, size_t start)
{
	if(token == NULL || token->len <= start)
		return false;

	if(token->str != NULL && token->str[start] == ch)
		return true;

	if(token->wstr != NULL && token->wstr[start] == wch)
		return true;

	return false;
}

bool Token::Compare(Token *first, Token *second)
{
	if(first == NULL || second == NULL)
		return false;

	if(first->chr != 0 && first->chr == second->chr)
		return true;

	if(first->wchr != 0 && first->wchr == second->wchr)
		return true;

	if(first->str != NULL && second->str != NULL && first->len == second->len && 
		!_strnicmp(first->str, second->str, first->len))
		return true;

	return false;
}

bool Token::Compare(Token *first, Token *second, size_t len)
{
	if(first == NULL || second == NULL)
		return false;

	if(first->str != NULL && second->str != NULL && first->len >= len && second->len >= len && 
		!_strnicmp(first->str, second->str, len))
		return true;

	return false;
}

bool Token::Compare(Token *first, TokenStr *second)
{
	if(first == NULL || second == NULL)
		return false;

	const char *str = NULL;
	const wchar_t *wstr = NULL;

	// Use NULL pointers for empty strings for correct comparison (since only str or wstr can be set at a time)
	if(second->str.size() > 0)
		str = second->str.c_str();

	if(second->wstr.size() > 0)
		wstr = second->wstr.c_str();

	return Token::Compare(first, str, wstr, second->len);
}

// Compare with the target value
bool Token::CompareTarget(Token *token, const char *word, const wchar_t * /*w_word*/, size_t len)
{
	if(token == NULL || token->t_len != len)
		return false;

	if(token->t_str != NULL && word != NULL && _strnicmp(token->t_str, word, len) == 0)
		return true;

	return false;
}

// Check whether the token contains a number
bool Token::IsNumeric()
{
	if(str == NULL)
		return false;

	for(size_t i = 0; i < len; i++)
	{
		// Sign is allowed in the first position
		if(i == 0 && (str[i] == '-' || str[i] == '+'))
			continue;

		if((str[i] < '0' || str[i] > '9') && str[i] != '.')
			return false;
	}

	return true;
}

// Check if string literal contain a number
bool Token::IsNumericInString()
{
	if(type != TOKEN_STRING || len == 0)
		return false;

	// Empty string
	if(len == 2)
		return false;

	for(size_t i = 1; i < len - 1; i++)
	{
		// Sign is allowed in the first position after '
		if(i == 1 && (str[i] == '-' || str[i] == '+'))
			continue;

		if((str[i] < '0' || str[i] > '9') && str[i] != '.')
			return false;
	}

	return true;
}

// Test if the token is a single char token
bool Token::IsSingleChar()
{
	if(chr != 0 || wchr != 0)
		return true;

	return false;
}

// Test if the token contains space, newline or tab
bool Token::IsBlank()
{
	return Token::IsBlank(this);
}

bool Token::IsBlank(Token *token)
{
	if(token == NULL)
		return false;

	if(token->chr == ' ' || token->chr == '\t' || token->chr == '\r' || token->chr == '\n' ||
		token->wchr == L' ' || token->wchr == L'\t' || token->wchr == L'\r' || token->wchr == L'\n')
		return true;

	return false;
}

void Token::Change(Token *token, const char *new_str, const wchar_t * /*new_wstr*/, size_t len, Token *format)
{
	if(token == NULL)
		return;

	Token::ClearTarget(token);

	// Target is ASCII value
	if(new_str != NULL)
	{
#ifdef WIN32
		char *newline = strchr((char*)new_str, '\n');

		// Replace new line with 2 byte sequence for Windows
		if(newline != NULL)
		{
			char *newline_win = new char[len*2 + 1];
			size_t len_win = 0;

			for(size_t i = 0; i < len; i++)
			{
				if(new_str[i] == '\n')
				{
					newline_win[len_win] = '\r';
					len_win++;
				}

				newline_win[len_win] = new_str[i];
				len_win++;
			}

			newline_win[len_win] = 0;
			len = len_win;

			token->t_str = newline_win;
		}
		else
			token->t_str = Str::GetCopy(new_str, len);
#else
		token->t_str = Str::GetCopy(new_str, len);
#endif

		FormatTargetValue(token, format);
	}

	token->t_len = len;
}

// Replace the token with integer value
void Token::Change(Token *token, int value)
{
	if(token == NULL)
		return;

	Token::ClearTarget(token);

	token->t_str = new char[11];
	sprintf((char*)token->t_str, "%d", value);

	token->t_len = strlen(token->t_str);
}

void Token::Change(Token *token, Token *values)
{
	if(token == NULL || values == NULL)
		return;

	Token::ClearTarget(token);
	
	// Target value has priority over source value
	if(values->t_str != NULL)
	{
		token->t_str = Str::GetCopy(values->t_str, values->t_len);
		token->t_len = values->t_len;
	}
	else
	if(values->str != NULL)
	{
		token->t_str = Str::GetCopy(values->str, values->len);
		token->t_len = values->len;
	}
}

void Token::ChangeNoFormat(Token *token, const char *new_str, const wchar_t * /*new_wstr*/, size_t len)
{
	if(token == NULL)
		return;

	Token::ClearTarget(token);

	if(new_str != NULL)
		token->t_str = Str::GetCopy(new_str, len);

	token->t_len = len;
}

void Token::ChangeNoFormat(Token *token, TokenStr &tstr)
{
	if(token == NULL)
		return;

	Token::ClearTarget(token);

	if(tstr.str.empty() == false)
		token->t_str = Str::GetCopy(tstr.str.c_str(), tstr.len);

	token->t_len = tstr.len;
}

void Token::ChangeNoFormat(Token *token, Token *source, size_t start, size_t len)
{
	if(token == NULL || source == NULL)
		return;

	if(source->len <= len)
		return;

	Token::ClearTarget(token);

	if(source->str != NULL)
		token->t_str = Str::GetCopy(source->str + start, len);

	token->t_len = len;
}

// Format target value case
void Token::FormatTargetValue(Token *token, Token *format)
{
	if(token == NULL || token->t_str == NULL)
		return;

	bool all_upper = true;
	bool all_lower = true;
	bool first_upper = true;
	bool others_lower = true;

	size_t len = token->len;

	const char *src = token->str;
	char *tgt = (char*)token->t_str;

	if(format != NULL && format->str != NULL)
	{
		src = format->str;
		len = format->len;
	}

	if(src == NULL)
		return;

	// Define whether the source value contains all chars in upper or lowercase
	for(size_t i = 0; i < len; i++)
	{
		// Check for a letter (English ASCII only), src is signed so check for negative values
		if(src[i] > 0 && src[i] < 128 && isalpha(src[i]) != 0)
		{
			// Check for lower case letter
			if(src[i] >= 'a' && src[i] <= 'z')
			{
				all_upper = false;

				if(i == 0)
					first_upper = false;
			}
			else
			// Check for upper case letter
			if(src[i] >= 'A' && src[i] <= 'Z')
			{
				all_lower = false;

				if(i > 0)
					others_lower = false;
			}
		}
	}

	// Single char value
	if(first_upper == true && len == 1)
		others_lower = false;

	// Check for mixed case
	if(first_upper == true && others_lower == true)
	{
		if(tgt[0] >= 'A' && tgt[0] <= 'Z')
		{
			_strlwr(tgt);
			tgt[0] = 'A' + (tgt[0] - 'a');
		}
		else
		// Special case when first is the space
		if(tgt[0] == ' ' && tgt[1] >= 'A' && tgt[1] <= 'Z')
		{
			_strlwr(tgt);
			tgt[1] = 'A' + (tgt[1] - 'a');
		}
	}
	else
	// If source value is upper case only, convert target value to upper case
	if(all_upper == true)
		_strupr(tgt);
	else
	// If source value is lower case only, convert target value to lowe case
	if(all_lower == true)
		_strlwr(tgt);
}

// Set Removed flag for the token
void Token::Remove(Token *token, bool remove_spaces_before)
{
	if(token == NULL)
		return;

	token->flags |= TOKEN_REMOVED;

	Token *prev = token->prev;
	Token *next = token->next;

	if(prev == NULL)
		return;

	if(remove_spaces_before == true)
	{
		// Remove spaces token before the removed word (recursively), but only if there is a blank or ) , ;
		// after removed token (otherwise tokens can be merged)
		if(prev->Compare(' ', L' ') == true || prev->Compare('\t', L'\t') == true)
		{
			if(next != NULL)
			{
				if(next->IsBlank() == true || next->Compare(')', L')') == true || 
					next->Compare(',', L',') == true || next->Compare(';', L';') == true)
					Remove(prev);
			}
			else
			// Next token can be not selected yet
			if(token->next_start != NULL && token->remain_size > 1)
			{
				const char *cur = token->next_start;

				if(*cur == ' ' || *cur == ')' || *cur == ',' || *cur == ';')
					Remove(prev);
			}
		}
	}
	// In other cases better to remove spaces after: "SET var = 1" to "var := 1" when SET is removed 
	else
	{
		if(next != NULL && next->Compare(' ', L' ') == true)
			Remove(next);
	}
}

// Remove the specified range of tokens
void Token::Remove(Token *from, Token *last)
{
	if(from == NULL)
		return;

	Token *cur = from;

	while(true) 
	{
		Token::Remove(cur);

		if(cur == last || last == NULL)
			break;

		cur = cur->next;

		if(cur == NULL)
			break;
	}
}

// Replace the token value with spaces to preserve indent
void Token::ReplaceWithSpaces(Token *token)
{
	if(token == NULL)
		return;

	TokenStr str;

	for(size_t i = 0; i < token->len; i++)
		str.Append(" ", L" ", 1);

	ChangeNoFormat(token, str);
}

// Get the target length of the token
size_t Token::GetTargetLength()
{
	if(flags & TOKEN_REMOVED)
		return 0;

	// Firstly check for target values
	if(t_str != NULL)
		return t_len;

	if(chr != 0)
		return 1;
	else
	if(wchr != 0)
		return 2;

	return len;
}

// Remove the current target value
void Token::ClearTarget(Token *token)
{
	if(token == NULL)
		return;

	// Remove the current target value if set
	if(token->t_str != NULL)
	{
		delete token->t_str;

		token->t_str = NULL;
		token->t_len = 0;
	}
}

// Append data to the string
void Token::AppendTarget(char *string, int *cur_len)
{
	if(string == NULL || cur_len == NULL)
		return;

	int clen = *cur_len;

	// Token was deleted
	if(flags & TOKEN_REMOVED)
		return;

	// Check is target value is set
	if(t_str != NULL)
	{
		strncpy(string + clen, t_str, t_len);
		clen += t_len;

		*cur_len = clen;
		return;
	}

	if(chr != 0)
	{
		string[clen] = chr;
		clen++;
	}
	else
	if(str != NULL)
	{
		strncpy(string + clen, str, len);
		clen += len;
	}

	*cur_len = clen;
}

// Skip blanks, tabs and newlines
Token* Token::SkipSpaces(Token *token)
{
	if(token == NULL)
		return NULL;

	Token *cur = token;

	// Skip spaces
	while(cur != NULL)
	{
		if(cur->IsBlank() == false)
			break;

		cur = cur->next;
	}

	return cur;
}

// Get value as int
int Token::GetInt()
{
	if(str == NULL && wstr == NULL)
		return -1;

	return GetInt(0, len);
}

int Token::GetInt(size_t start, size_t l)
{
	int out = -1;

	if(str == NULL && wstr == NULL)
		return -1;

	if(start >= len || start + l > len || l > 10)
		return -1;

	// Convert string value to int
	if(str != NULL)
	{
		char in[21]; *in = 0;

		if(l < 20)
		{
			strncpy(in, str + start, l);
			in[l] = 0;
		}
		
		sscanf(in, "%d", &out);
	}

	return out;
}

TokenStr::TokenStr(Token *token)
{
	if(token == NULL)
		return;

	// Target value has priority
	if(token->t_str != NULL || token->t_wstr != NULL)
		Set(token->t_str, token->t_wstr, token->t_len);
	else
		Set(token->str, token->wstr, token->len);

	prev = NULL; 
	next = NULL;
}

void TokenStr::Set(const char *s, const wchar_t *w, size_t l)
{
	// It is possible that only one part is set (ASCII or Unicode)
	if(s != NULL)
		str.assign(s, l);

	if(w != NULL)
		wstr.assign(w, l);

	len = l;
}

void TokenStr::Set(TokenStr &src)
{
	str.assign(src.str);
	wstr.assign(src.wstr);

	len = src.len;
}

// Append a string
void TokenStr::Append(const char *s, const wchar_t *w, size_t l)
{
	if(s != NULL)
		str += s;

	if(w != NULL)
		wstr += w;

	len += l;
}

void TokenStr::Append(Token *token, size_t start, size_t l)
{
	if(token == NULL || start >= token->len)
		return;

	// Target value has priority
	if(token->t_str != NULL)
		str.append(token->t_str + start, l); 
	else
	if(token->str != NULL)
		str.append(token->str + start, l); 
	else
	if(token->wstr != NULL)
		wstr.append(token->wstr + start, l); 

	len += l;
}

void TokenStr::Append(Token *token, bool appendSource)
{
	if(token == NULL)
		return;

	// Append the target value if set
	if(token->t_str != NULL && !appendSource)
	{
		str.append(token->t_str, token->t_len); 
		len += token->t_len;
	}
	else
	if(token->str != NULL)
	{
		str.append(token->str, token->len); 
		len += token->len;
	}
    else
    if(token->chr != 0)
    {
        str.append(1, token->chr);
        len++;
    }

	if(token->wstr != NULL)
	{
		wstr.append(token->wstr, token->len); 
		len += token->len;
	}
}

void TokenStr::Append(TokenStr &in_str)
{
	size_t ln = in_str.str.size();

	if(ln > 0)
	{
		str += in_str.str;
		len += ln;
	}
	else
	{
		ln = in_str.wstr.size();
		wstr += in_str.wstr;
		len += ln;
	}
}

void TokenStr::Append(TokenStr &tokenstr, size_t start, size_t l)
{
	if(start >= tokenstr.len)
		return;

	if(tokenstr.str.size() > start + l)
		str.append(tokenstr.str.c_str() + start, l); 
	else
	if(tokenstr.wstr.size() > start + l)
		wstr.append(tokenstr.wstr.c_str() + start, l);

	len += l;
}

void TokenStr::Append(int num)
{
	char s[11]; *s = '\x0';

	sprintf(s, "%d", num);

	size_t l = strlen(s);

	if(l > 0)
	{
		str += s;
		len += l;
	}
}

bool TokenStr::Compare(const char *word, const wchar_t *w_word, size_t len)
{
	return TokenStr::Compare(this, word, w_word, len);
}

bool TokenStr::Compare(const char ch, const wchar_t wch, size_t pos)
{
	return TokenStr::Compare(this, ch, wch, pos);
}

bool TokenStr::Compare(TokenStr &first, TokenStr &second)
{
	if(first.len != second.len)
		return false;

	const char *s1 = first.str.c_str();
	const char *s2 = second.str.c_str();

	// ASCII value is set
	if(*s1 != '\x0' && _stricmp(s1, s2) == 0)
		return true;

	return false;
}

bool TokenStr::Compare(TokenStr *tokenstr, const char *word, const wchar_t * /*w_word*/, size_t len)
{
	if(tokenstr == NULL || tokenstr->len != len)
		return false;

	if(_strnicmp(tokenstr->str.c_str(), word, len) == 0)
		return true;

	return false;
}

bool TokenStr::Compare(TokenStr *tokenstr, const char ch, const wchar_t /*wch*/, size_t pos)
{
	if(tokenstr == NULL || tokenstr->len <= pos)
		return false;

	if((tokenstr->str.c_str())[pos] == ch)
		return true;

	return false;
}