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

// SQLParser helper functions

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "sqlparser.h"
#include "str.h"
#include "file.h"
#include "cobol.h"

// Check for the source type
bool SqlParser::Source(short s1, short s2, short s3, short s4, short s5, short s6)
{
	if(_source == s1 || _source == s2 || _source == s3 ||_source == s4 || _source == s5 || _source == s6)
		return true;

	return false;
}

// Check for the target type
bool SqlParser::Target(short t1, short t2, short t3, short t4, short t5, short t6)
{
	if(_target == t1 || _target == t2 || _target == t3 ||_target == t4 || _target == t5 || _target == t6)
		return true;

	return false;
}

// Save bookmark 
Book* SqlParser::Bookmark(int type, Token *name, Token *book)
{
	Book *bookmark = new Book();
	bookmark->type = type;
	bookmark->name = name;
	bookmark->book = book;

	_bookmarks.Add(bookmark);

	return bookmark;
}

Book* SqlParser::Bookmark(int type, Token *name, Token *name2, Token *book)
{
	Book *bookmark = new Book();
	bookmark->type = type;
	bookmark->name = name;
	bookmark->name2 = name2;
	bookmark->book = book;

	_bookmarks.Add(bookmark);

	return bookmark;
}

// Get bookmark
Book* SqlParser::GetBookmark(int type, Token *name)
{
	return GetBookmark(type, name, NULL);
}

Book* SqlParser::GetBookmark(int type, Token *name, Token *name2)
{
	Book *bookmark = _bookmarks.GetFirstNoCurrent();

	while(bookmark != NULL)
	{
		if(bookmark->type == type && Token::Compare(bookmark->name, name) == true)
		{
			// Check second name if set
			if(bookmark->name2 != NULL)
			{
				if(Token::Compare(bookmark->name2, name2) == true)
					break;
			}
			else
				break;
		}

		bookmark = bookmark->next;
	}

	return bookmark;
}

// Get bookmark by composite name name.name2
Book* SqlParser::GetBookmark2(int type, Token *nm)
{
	if(nm == NULL)
		return NULL;

	Token name;
	Token name2;

	// Split qualified name to 2 parts
	SplitIdentifier(nm, &name, &name2); 

	Book* book = GetBookmark(type, &name, &name2);

	return book;
}

// Return first not NULL
Token* SqlParser::Nvl(Token *first, Token *second, Token *third, Token *fourth)
{
	if(first != NULL)
		return first;

	if(second != NULL)
		return second;

	if(third != NULL)
		return third;

	return fourth;
}

Token* SqlParser::NvlLast(Token *first)
{
    if(first != NULL)
	    return first;

    return GetLastToken();
}
	
// Create a new identifier by appending the specified word (handles delimiters)
Token *SqlParser::AppendIdentifier(Token *source, const char *word, const wchar_t * /*w_word*/, size_t len)
{
	if(source == NULL || source->len <= 0)
		return NULL;

	Token *token = new Token();
	*token = *source;

	token->prev = NULL;
	token->next = NULL;

	token->t_str = NULL;
	token->t_wstr = NULL;
	token->t_len = 0;

	// Append to the target value if set
	const char *s = (source->t_str == NULL) ? source->str : source->t_str;

	size_t s_len = (source->t_len == 0) ? source->len : source->t_len; 
	size_t new_len = s_len + len;

	char *new_str = new char[new_len+1];
	strncpy(new_str, s, s_len);

	char quote = s[s_len-1];

	// If the source identifiers ends with a quote, append before the quote
	if(quote == '"')
	{
		strncpy(new_str + s_len - 1, word, len);
		new_str[s_len - 1] = quote;
	}
	else
		strncpy(new_str + s_len, word, len);

	new_str[new_len] = '\x0';

	token->str = new_str;
	token->len = new_len;
	token->source_allocated = true;

	return token;
}

// Append a word to identifier (handles delimiters)
void SqlParser::AppendIdentifier(TokenStr &source, const char *word, const wchar_t *w_word, size_t len)
{
	size_t slen = source.str.size();
	size_t wlen = source.wstr.size();

	// Identifier is empty, just append the value
	if(slen == 0 && wlen == 0)
	{
		source.Append(word, w_word, len);
		return;
	}

	if(slen > 0)
	{
		char last = source.str.at(slen-1);

		// If last character is a quote, insert before the quote
		if(last == '"' || last == ']' || last == '`')
			source.str.insert(slen-1, word);
		else
			source.str += word;

		source.len += len;
	}
	else
	if(wlen > 0)
	{
		wchar_t last = source.wstr.at(slen-1);

		// If last character is a quote, insert before the quote
		if(last == L'"' || last == L']' || last == L'`')
			source.wstr.insert(slen-1, w_word);
		else
			source.wstr += w_word;

		source.len += len;
	}	
}

// Append a token string to identifier (handles delimiters)
void SqlParser::AppendIdentifier(TokenStr &source, TokenStr &append)
{
	AppendIdentifier(source, append.str.c_str(), append.wstr.c_str(), append.len);
}

// Split identifier to 2 parts
void SqlParser::SplitIdentifier(Token *source, Token *first, Token *second)
{
	if(source == NULL || first == NULL || second == NULL)
		return;

	size_t dot = 0;

	// Find the position of the last dot
	for(size_t i = source->len - 1; i > 0; i--)
	{
		if(source->str[i] == '.')
		{
			dot = i;
			break;
		}
	}

	// Delimiter found
	if(dot > 0)
	{
		// First part
		first->str = Str::GetCopy(source->str, dot);
		first->len = dot;
		first->source_allocated = true;

		// Second part
		second->str = Str::GetCopy(source->str + dot + 1, source->len - dot - 1);
		second->len = source->len - dot - 1;
		second->source_allocated = true;
	}
}

// Define database object name mapping from file
void SqlParser::SetObjectMappingFromFile(const char *file)
{
	if(file == NULL)
		return;

	// Mapping file size
	int size = File::GetFileSize(file);

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file, input, (size_t)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';

	char *cur = input;

	// Process input
	while(*cur)
	{
		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		std::string source;
		std::string target;

		// Get the source name until ,
		while(*cur && *cur != ',')
		{
			source += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(source);

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

		// Get the target name until new line
		while(*cur && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			target += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(target);

		_object_map.insert(StringMapPair(source, target));
	}
}

// Meta information about tables, columns
void SqlParser::SetMetaFromFile(const char *file)
{
	if(file == NULL)
		return;

	// Meta file size
	int size = File::GetFileSize(file);

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file, input, (size_t)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';

	char *cur = input;

	// Process input
	while(*cur)
	{
		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		std::string object;
		std::string column;
        std::string dtype;

		// Get the object name until ,
		while(*cur && *cur != ',')
		{
			object += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(object);

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

        // Get the column name until ,
		while(*cur && *cur != ',')
		{
			column += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(column);

		if(*cur == ',')
			cur++;
		else
			break;

		cur = Str::SkipSpaces(cur);

		// Get the data type until new line
		while(*cur && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			dtype += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(dtype);

        std::transform(object.begin(), object.end(), object.begin(), ::tolower);

        // Check if any metadata for this object already exists
        ListT<Meta> *m = NULL;
        std::map<std::string, ListT<Meta>*>::iterator i = _meta.find(object);
        
        if(i == _meta.end())
        {
            m = new ListT<Meta>();
            _meta.insert(std::make_pair(object, m));
        }
        else 
            m = i->second;

        m->Add(new Meta(object, column, dtype));
	}
}

// Functions mapped to stored procedures
void SqlParser::SetFuncToSpMappingFromFile(const char *file)
{
	if(file == NULL)
		return;

	// Mapping file size
	int size = File::GetFileSize(file);

	if(size == -1)
		return;
 
	char *input = new char[(size_t)size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(file, input, (size_t)size) == -1)
	{
		delete input;
		return;
	}

	input[size] = '\x0';

	char *cur = input;

	// Process input
	while(*cur)
	{
		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		std::string source;
		
		// Get the name until new line
		while(*cur && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			source += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(source);

		std::transform(source.begin(), source.end(), source.begin(), ::tolower);
		_func_to_sp_map.insert(StringMapPair(source, ""));
	}
}

// Read the data type from available meta information
const char* SqlParser::GetMetaType(Token *object, Token *column)
{
    if(object == NULL)
        return NULL;

    TokenStr obj;
    TokenStr col;

    // Separate object and column names
	if(column == NULL)
		SplitIdentifierByLastPart(object, obj, col, 2);
	else
	{
		obj.Append(object);
		col.Append(column);
	}

    std::transform(obj.str.begin(), obj.str.end(), obj.str.begin(), ::tolower);
    std::map<std::string, ListT<Meta>*>::iterator i = _meta.find(obj.str);

     if(i != _meta.end())
     {
         Meta *meta = i->second->GetFirstNoCurrent();

	     while(meta != NULL)
	     {
             if(_stricmp(meta->column.c_str(), col.str.c_str()) == 0)
                 return meta->dtype.c_str();

		     meta = meta->next;
         }
     }

     return NULL;
}

// Schema name mapping in format s1:t1, s2:t2, s3, ...
void SqlParser::SetSchemaMapping(const char *mapping)
{
	if(mapping == NULL)
		return;

	char *cur = (char*)mapping;

	// Process input
	while(*cur)
	{
		cur = Str::SkipSpaces(cur);

		if(*cur == '\x0')
			break;

		std::string source;
		std::string target;

		// Get the source name until : or ,
		while(*cur && *cur != ':' && *cur != ',')
		{
			source += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(source);

		bool removed = false;

		// : is not specified when schema is removed
		if(*cur == ':')
			cur++;
		else
			removed = true;

		if(removed == false)
		{
			cur = Str::SkipSpaces(cur);

			// Get the target name until ,
			while(*cur && *cur != ',')
			{
				target += *cur;
				cur++;
			}

			Str::TrimTrailingSpaces(target);
		}

		_schema_map.insert(StringMapPair(source, target));

		if(*cur == ',')
			cur++;
	}
}

// Map object name for identifier
bool SqlParser::MapObjectName(Token *token)
{
	if(token == NULL)
		return false;

	// Find an identifier
	for(StringMap::iterator i = _object_map.begin(); i != _object_map.end(); i++)
	{
		if(CompareIdentifiers(token, i->first.c_str()) == true)
		{
			// Change name
			token->t_str = Str::GetCopy(i->second.c_str());
			token->t_len = strlen(token->t_str);

			return true;
		}
	}	

	return false;
}

// Compare identifiers
bool SqlParser::CompareIdentifiers(Token *token, const char *str)
{
	if(token == NULL || str == NULL)
		return false;

	// Exact comparison
	if(token->str != NULL && _strnicmp(token->str, str, token->len) == 0)
		return true;

	return false;
}

// Compare only existing parts in identifiers (schema.tab and tab i.e.)
bool SqlParser::CompareIdentifiersExistingParts(Token *first, Token *second)
{
	if(first == NULL || second == NULL)
		return false;

	// Define the number of parts in each identifier
	int num1 = GetIdentPartsCount(first);
	int num2 = GetIdentPartsCount(second);

	int num = 0;
	int equal_num = 0;

	bool equal = true;

	TokenStr part1;
	TokenStr part2;

	size_t len1 = 0;
	size_t len2 = 0;

	// Skip exceeding first identifier parts
	if(num1 > num2)
	{
		num = num2;

		for(int i = 0; i < num1 - num2; i++)
			GetNextIdentItem(first, part1, &len1);
	}
	else
	// Skip exceeding second identifier parts
	if(num2 > num1)
	{
		num = num1;

		for(int i = 0; i < num2 - num1; i++)
			GetNextIdentItem(second, part2, &len2);
	}

	// Single part identifier
	if(num == 0)
	{
		equal = Token::Compare(first, second);
		equal_num++;
	}
	else
	{
		for(int i = 0; i < num; i++)
		{
			part1.Clear();
			part2.Clear();

			GetNextIdentItem(first, part1, &len1);
			GetNextIdentItem(second, part2, &len2);

			// All parts must be equal
			if(CompareIdentifierPart(part1, part2) == false)
			{
				equal = false;
				break;
			}

			equal_num++;
		}
	}

	return (equal == true && equal_num > 0) ? true : false;
}

// Compare a single part of identifier
bool SqlParser::CompareIdentifierPart(TokenStr &first, TokenStr &second)
{
	size_t len1 = first.len;
	size_t len2 = second.len;

	const char *cur1 = first.str.c_str();
	const char *cur2 = second.str.c_str();

	// Skip quotes
	if(*cur1 == '"' && len1 > 2)
	{
		cur1++;
		len1 -= 2;
	}

	if(*cur2 == '"' && len2 > 2)
	{
		cur2++;
		len2 -= 2;
	}

	if(len1 != len2)
		return false;

	if(_strnicmp(cur1, cur2, len1) == 0)
		return true;

	return false;
}

// Convert schema name in identifier
void SqlParser::ConvertSchemaName(Token *token, TokenStr &ident, size_t *len)
{
	TokenStr schema;

	if(token == NULL || len == NULL)
		return;

	GetNextIdentItem(token, schema, len);

	// It is not schema name (must not be last item in identifier)
	if(*len >= token->len)
		return;

	// Get . from input
	schema.Append(token, *len, 1);
	
	*len = *len + 1;

	// Schema name is removed
	if(_option_rems == true)
		schema.Clear();
	else
	// dbo. in SQL Server, Sybase ASE
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) && !Target(SQL_SQL_SERVER, SQL_SYBASE) && 
		(schema.Compare("[dbo].", L"[dbo].", 6) == true || schema.Compare("dbo.", L"dbo.", 4) == true))
		schema.Clear();		
	else
	// Remove `` in MySQL
	if(_source == SQL_MYSQL && _target != SQL_MYSQL)
	{
		if(schema.len > 2 && schema.Compare('`', L'`', 0) == true)
		{
			TokenStr schema2;
			schema2.Append(schema, 1, schema.len - 3);
			schema2.Append(".", L".", 1);

			schema.Set(schema2);
		}
	}
	else
	// Change "" and [] to `` in MySQL
	if(_source != SQL_MYSQL && Target(SQL_MARIADB, SQL_MYSQL))
	{
		if(schema.len > 2 && 
			(schema.Compare('"', L'"', 0) == true || schema.Compare('[', L'[', 0) == true))
		{
			TokenStr schema2;

			schema2.Append("`", L"`", 1);
			schema2.Append(schema, 1, schema.len - 3);
			schema2.Append("`.", L"`.", 2);
			
			schema.Set(schema2);
		}
	}
	else
	// dba. in Sybase ASA
	if(_source == SQL_SYBASE_ASA && _target != SQL_SYBASE_ASA && schema.Compare("dba.", L"dba.", 4) == true)
		schema.Clear();		

	ident.Append(schema);
}

// Convert object name in identifier
void SqlParser::ConvertObjectName(Token *token, TokenStr &ident, size_t *len)
{
	TokenStr name;
	TokenStr name2;

	if(token == NULL || len == NULL)
		return;

	GetNextIdentItem(token, name, len);

	bool changed = false;

	// Remove [] in SQL Server, Sybase
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
	{
		if(name.len > 2 && name.Compare('[', L'[', 0) == true && 
				name.Compare(']', L']', name.len - 1) == true)
		{
			name2.Append(name, 1, name.len - 2);
			changed = true;
		}
	}
	else
	// Remove `` in MySQL
	if(_source == SQL_MYSQL && _target != SQL_MYSQL)
	{
		if(name.len > 2 && name.Compare('`', L'`', 0) == true && 
			name.Compare('`', L'`', name.len - 1) == true)
		{
			name2.Append(name, 1, name.len - 2);
			changed = true;
		}
	}
	else
	// Change "" to `` in MySQL
	if(_source != SQL_MYSQL && Target(SQL_MARIADB, SQL_MYSQL))
	{
		if(name.len > 2 && name.Compare('"', L'"', 0) == true)
		{
			name2.Append("`", L"`", 1);
			name2.Append(name, 1, name.len - 2);
			name2.Append("`", L"`", 1);
			
			changed = true;
		}
	}
	
	// Add object name to the target identifier
	if(changed == false)
		AppendIdentifier(ident, name);
	else
		AppendIdentifier(ident, name2);
}

// Convert next item in qualified identifier
void SqlParser::GetNextIdentItem(Token *token, TokenStr &ident, size_t *len)
{
	if(token == NULL || len == NULL)
		return;

	size_t i = *len;

	// Skip starting . from the previous item
	if(i > 0 && token->Compare('.', L'.', i) == true)
		i++;

	char end_del = 0;
	wchar_t end_delw = 0;

	// Check for delimited identifier
	if(token->Compare('"', L'"', i) == true)
	{
		end_del = '"';
		end_delw = L'"';
	}

	if(end_del != 0)
	{
		ident.Append(token, i, 1);
		i++;
	}

	// Go until dot (.), delimiter or end
	while(i < token->len)
	{
		if(end_del == 0)
		{
			if(token->Compare('.', L'.', i) == true)
				break;
		}
		else
		if(token->Compare(end_del, end_delw, i) == true)
		{
			ident.Append(token, i, 1);
			i++;

			break;
		}

		ident.Append(token, i, 1);
		i++;
	}

	*len = i;
}

// Separate leading parts and trail part
void SqlParser::SplitIdentifierByLastPart(Token *token, TokenStr &lead, TokenStr &trail, int parts)
{
	if(token == NULL)
		return;

	// Number of identifier parts in unknown yet
	if(parts == -1)
		parts = GetIdentPartsCount(token);

	size_t len = token->len;
	int c = parts - 1;

	for(size_t i = 0; i < len; i++)
	{
		if(token->Compare('.', L'.', i) == true)
		{
			c--;

			// Do not include . to the last part
			if(c == 0)
				continue;
		}

		if(c > 0)
			lead.Append(token, i, 1);
		else
			trail.Append(token, i, 1);
	}
}

// Get the number of parts in 
int SqlParser::GetIdentPartsCount(Token *token)
{
	if(token == NULL)
		return -1;

	int num = 1;

	if(token->str != NULL && token->len > 0)
	{
		// Count number of . 
		for(size_t i = 0; i < token->len; i++)
		{
			if(token->str[i] == '.')
				num++;
		}
	}

	return num;
}

// Remove leading part from qualified identifier if it exists
void SqlParser::ConvertIdentRemoveLeadingPart(Token *token)
{
	if(token == NULL || token->len == 0)
		return;

	size_t pos = 0;
	bool found = false;

	// Find the position of the first dot (.)
	while(true)
	{
		if(token->Compare(".", L".", pos, 1) == true)
		{
			// Skip dot
			pos++;

			found = true;
			break;
		}

		pos++;

		if(pos >= token->len)
			break;
	}

	// No (.) in the identifier
	if(found == false)
		return;

	const char *new_str = NULL;
	const wchar_t *new_wstr = NULL;
	
	size_t new_len = token->len - pos; 

	if(token->str != NULL)
		new_str = token->str + pos;

	if(token->wstr != NULL)
		new_wstr = token->wstr + pos;

	// Set the new identifier value
	Token::Change(token, new_str, new_wstr, new_len);
}

// Add package name as the prefix to identifier
void SqlParser::PrefixPackageName(TokenStr &ident)
{
	if(_spl_package == NULL)
		return;

	ident.Append(_spl_package);
	AppendIdentifier(ident, "_", L"_", 1);
}

// Replace PL/SQL records with variable list
void SqlParser::DiscloseRecordVariables(Token *format)
{
	ListwItem *item = _spl_rowtype_vars.GetFirst();

	// Convert record declaration to variable list declaration
	while(item != NULL)
	{
		Token *decl_rec = (Token*)item->value;
		bool found = false;

		ListwmItem *fields = _spl_rowtype_fields.GetFirst();

		while(fields != NULL)
		{
			Token *rec = (Token*)fields->value;
			Token *var = (Token*)fields->value2;

			// A recond field found, add a variable 
			if(Token::Compare(decl_rec, rec) == true)
			{
				// There is already DECLARE keyword before first variable
				if(found == true)
					Append(decl_rec, "\nDECLARE ", L"\nDECLARE ", 9, format);

				AppendCopy(decl_rec, var);
				Append(decl_rec, ";", L";", 1);

				found = true;
			}

			fields = fields->next;
		}

		item = item->next;
	}

	item = _spl_rowtype_fetches.GetFirst();

	// Convert FETCH INTO rec to FETCH INTO rec_field1, rec_field2 etc.
	while(item != NULL)
	{
		Token *fetch_rec = (Token*)item->value;
		bool found = false;

		ListwmItem *fields = _spl_rowtype_fields.GetFirst();

		while(fields != NULL)
		{
			Token *rec = (Token*)fields->value;
			Token *var = (Token*)fields->value2;

			// A recond field found, append variable name to the list
			if(Token::Compare(fetch_rec, rec) == true)
			{
				if(found == true)
					Append(fetch_rec, ", ", L", ", 2);

				AppendCopy(fetch_rec, var);
				found = true;
			}

			fields = fields->next;
		}

		// Remove record variable
		if(found == true)
			Token::Remove(fetch_rec);

		item = item->next;
	}
}

// Add declarations for implicit record fields (for cursor loops)
void SqlParser::DiscloseImplicitRecordVariables(Token *format)
{
	ListwmItem *item = _spl_implicit_rowtype_fields.GetFirst();

	Token *append = GetDeclarationAppend();
	Token *fmt = Nvl(_declare_format, format);

	// Add declaration for each variable
	while(item != NULL)
	{
		Token *cursor = (Token*)item->value;
		Token *var = (Token*)item->value2;

		Token var_last_part;
		var_last_part.len = var->len;
		var_last_part.str = var->str;

		TokenStr cur;
		TokenStr col;

		// Separate cursor and column names
		SplitIdentifierByLastPart(&var_last_part, cur, col, 2);

		APPEND_FMT(append, "\nDECLARE ", fmt);
		AppendCopy(append, var);

		ListwmItem *sel_item = _spl_declared_cursors_select_exp.GetFirst();
		const char *datatype_meta = NULL;

		// Try to define the data type from SELECT list expressions
		while(sel_item != NULL)
		{
			Token *sel_cursor = (Token*)sel_item->value;
			Token *sel_col = (Token*)sel_item->value2;

			if(Token::Compare(cursor, sel_cursor))
			{
				if(sel_col->datatype_meta != NULL && Token::Compare(sel_col, &col))
				{
					datatype_meta = sel_col->datatype_meta;
					break;
				}
				else
				// For SELECT * FROM single table, table name may be resolved
				if(sel_col->table != NULL && TOKEN_CMPC(sel_col, '*'))
				{
					Token tcol;
					tcol.str = col.str.c_str();
					tcol.len = col.len;

					datatype_meta = GetMetaType(sel_col->table, &tcol);
					break;
				}
			}

			sel_item = sel_item->next;
		}

		if(datatype_meta != NULL)
		{
			TokenStr dt;
			APPENDSTR(dt, " ");
			dt.Append(datatype_meta, NULL, strlen(datatype_meta));
			AppendNoFormat(append, &dt);			
			APPEND(append, ";");
		}
		else
		{
			APPEND_FMT(append, " VARCHAR(200)", fmt);
			APPEND(append, "; -- Use -meta option to resolve the data type");
		}

		item = item->next;
	}

	item = _spl_implicit_rowtype_fetches.GetFirst();

	// Add column list to all generated fetch statements
	while(item != NULL)
	{
		Token *cursor = (Token*)item->value;
		Token *fetch_stmt = (Token*)item->value2;
		Token *format = (Token*)item->value3;

		ListwmItem *col_item = _spl_implicit_rowtype_fields.GetFirst();

		TokenStr into_cols;
		int cnt = 0;

		// Find columns of this cursor only
		while(col_item != NULL)
		{
			Token *col_cursor = (Token*)col_item->value;
			Token *var = (Token*)col_item->value2;

			if(Token::Compare(cursor, col_cursor))
			{
				if(cnt > 0)
					APPENDSTR(into_cols, ", ");

				into_cols.Append(var);
				cnt++;
			}

			col_item = col_item->next;
		}

		AppendNoFormat(fetch_stmt, &into_cols);
		APPEND_NOFMT(fetch_stmt, ";");
		APPEND_FMT(fetch_stmt, "\n", format);

		item = item->next;
	}
		
	item = _spl_declared_cursors_select_first_exp.GetFirst();

	// Add column list to cursor declaration that use SELECT * FROM instead of explicit column list
	while(item != NULL)
	{
		Token *cursor = (Token*)item->value;
		Token *first_select_exp = (Token*)item->value2;

		if(first_select_exp == NULL || !TOKEN_CMPC(first_select_exp, '*')) 
		{
			item = item->next;
			continue;
		}
		
		ListwmItem *col_item = _spl_implicit_rowtype_fields.GetFirst();

		TokenStr cursor_cols;
		int cnt = 0;

		// Find columns of this cursor only
		while(col_item != NULL)
		{
			Token *col_cursor = (Token*)col_item->value;
			Token *var = (Token*)col_item->value2;

			if(Token::Compare(cursor, col_cursor))
			{
				if(cnt > 0)
					APPENDSTR(cursor_cols, ", ");

				TokenStr cur;
				TokenStr col;

				Token var_src;
				var_src.len = var->len;
				var_src.str = var->str;

				// Separate cursor and column names
				SplitIdentifierByLastPart(&var_src, cur, col, 2);

				cursor_cols.Append(col);
				cnt++;
			}

			col_item = col_item->next;
		}

		// Replace * with column list
		Token::ChangeNoFormat(first_select_exp, cursor_cols);

		item = item->next;
	}		
}

// Cut operation
void SqlParser::Cut(int scope, int type, Token *name, Token *start, Token *end)
{
	CopyPaste *copy = new CopyPaste();

	copy->scope = scope;
	copy->type = type;
	copy->name = name;

	Token *cur = start;

	while(cur != NULL)
	{
		// Skip already deleted tokens
		if(cur->IsRemoved() == false)
		{
			// Create a copy of the source token
			Token *token = Token::GetCopy(cur);

			copy->tokens.Add(token);
			Token::Remove(cur);
		}

		// Cut inclusive, start and end can be the same
		if(cur == end)
			break;

		cur = cur->next;
	}

	_copypaste.Add(copy);
}

// Remove Copy/Paste blocks
void SqlParser::ClearCopy(int scope)
{
	CopyPaste *cur = _copypaste.GetFirst();

	while(cur != NULL)
	{
		if(cur->scope == scope)
			_copypaste.DeleteCurrent(true);

		cur = _copypaste.GetNext();
	}
}

// Check for an aggregate function
bool SqlParser::IsAggregateFunction(Token *name)
{
	if(name == NULL || name->type != TOKEN_FUNCTION)
		return false;

	if(name->Compare("COUNT", L"COUNT", 5) || name->Compare("SUM", L"SUM", 3) ||
		name->Compare("MIN", L"MIN", 3) || name->Compare("MAX", L"MAX", 3) ||
		name->Compare("AVG", L"AVG", 5))
		return true;

	return false;
}

// Check for LIST aggregate function
bool SqlParser::IsListAggregateFunction(Token *name)			
{
	if(name == NULL || name->type != TOKEN_FUNCTION)
		return false;

	if(_source == SQL_SYBASE_ASA && name->Compare("LIST", L"LIST", 4) == true)
		return true;

	return false;
}

// Find a token in list
Token* SqlParser::Find(ListW &list, Token *what)
{
	if(what == NULL)
		return NULL;

	Token *token = NULL;

	ListwItem *item = list.GetFirst();

	// Iterate through all items
	while(item != NULL)
	{
		Token *cur = (Token*)item->value;

		if(Token::Compare(cur, what) == true)
		{
			token = cur;
			break;
		}

		item = item->next;
	}

	return token;
}

// Find an item in list
ListwmItem* SqlParser::Find(ListWM &list, Token *what, Token *what2)
{
	if(what == NULL && what2 == NULL)
		return NULL;

	ListwmItem *item = list.GetFirst();

	ListwmItem *found = NULL;

	// Iterate through all items
	while(item != NULL)
	{
		Token *cur = (Token*)item->value;

		// Compare first item
		if(what != NULL && Token::Compare(cur, what) == false)
		{
			item = item->next;
			continue;
		}

		// No more comparisons required
		if(what2 == NULL)
		{
			found = item;
			break;
		}

		cur = (Token*)item->value2;

		// Compare second item
		if(Token::Compare(cur, what2) == false)
		{
			item = item->next;
			continue;
		}

		found = item;
		break;
	}

	return found;
}

// Get position to append new generated declarations
Token* SqlParser::GetDeclarationAppend()
{
	Token *append = _spl_last_declare;

	if(!TOKEN_CMPC(append, ';'))
	{
		// Skip ; that can follow the last declaration
		while(append != NULL && append->next != NULL)
		{
			// Skip spaces
			if(append->next->IsBlank())
			{
				append = append->next;
				continue;
			}

			if(Token::Compare(append->next, ';', L';') == true)
				append = append->next;

			break;
		}
	}

	if(append == NULL)
		append = _spl_last_declare;

	return append;
}

// Add RETURN statements for REFCURSORs
void SqlParser::AddReturnRefcursors(Token *end)
{
	if(!Source(SQL_ORACLE) || !Target(SQL_POSTGRESQL) || _spl_refcursor_params_num == 0 || end == NULL)
		return;

	// Iterate REFCURSORS parameters and add RETURN statement
	for(ListwItem *i = _spl_refcursor_params.GetFirst(); i != NULL; i = i->next)
	{
		Token *cursor = (Token*)i->value;

		PREPEND(end, "RETURN ");

		if(_spl_refcursor_params_num > 1)
			PREPEND(end, "NEXT ");

		PrependCopy(end, cursor);
		PREPEND(end, ";\n");
	}
}

// Add variable declarations generated in the procedural block
void SqlParser::AddGeneratedVariables()
{
	// Add not hound handler that must go after all variables and cursors; and initialize not_found variable before second and subsequent OPEN cursors
	if(_spl_need_not_found_handler && Target(SQL_MYSQL, SQL_MARIADB))
	{
		MySQLAddNotFoundHandler();
		MySQLInitNotFoundBeforeOpen();
	}

	Token *format_ident = _declare_format;

	// Oracle uses DECLARE block with the keyword at the beginning so use the variable name to define the indention
	if(_source == SQL_ORACLE)
	{
		format_ident = _spl_last_outer_declare_varname;

		// DECLARE is already added before the variable, so use prev token
		if(_spl_last_outer_declare_varname != NULL && Target(SQL_SQL_SERVER))
			format_ident = _spl_last_outer_declare_varname->prev;
	}

	Token *format = Nvl(_declare_format, _spl_outer_begin, _spl_outer_as);

	// Dynamic SQL variable is used
	if(_spl_dyn_sql_var)
	{
		// @sql for sp_executesql that does not allow expressions
		if(_target == SQL_SQL_SERVER)
		{
			Token *append_var = _spl_last_outer_declare_var;

			// sp_executesql requires unicode string
			APPEND_FMT(append_var, "\n", format_ident);
			APPEND_FMT(append_var, "DECLARE ", format);
			APPEND_NOFMT(append_var, "@sql ");
			APPEND_FMT(append_var, "NVARCHAR(max);", format);
		}
	}

	// Row count variable needs to be added
	if(_spl_need_rowcount_var)
	{
		if(_target == SQL_POSTGRESQL)
		{
			Token *append_var = GetDeclarationAppend();

			APPEND_FMT(append_var, "\n", format_ident);
			APPEND_NOFMT(append_var, "v_sqlrowcount ");
			APPEND_FMT(append_var, "INT;", format);
		}
	}
}

// Clear all procedural lists, statuses
void SqlParser::ClearSplScope()
{
	_spl_scope = 0;
	_spl_name = NULL;
	_spl_start = NULL;
	_spl_external = false;

	_declare_format = NULL;
	
	// Delete variable and parameters
	_spl_variables.DeleteAll();
	_spl_parameters.DeleteAll();

	// Clear statements clause scope
	_scope.DeleteAll();

	_spl_outer_begin = NULL;
	_spl_outer_as = NULL;
    _spl_begin_blocks.DeleteAll();
	_spl_last_declare = NULL;
	_spl_first_non_declare = NULL;
    _spl_last_outer_declare_var = NULL;
	_spl_last_outer_declare_varname = NULL;
	_spl_current_stmt = NULL;
	_spl_last_stmt = NULL;

	_spl_new_correlation_name = NULL;
	_spl_old_correlation_name = NULL;

	_spl_result_sets = 0;
	_spl_result_set_cursors.DeleteAll();
	_spl_result_set_generated_cursors.DeleteAll();

	_spl_refcursor_params_num = 0;
	_spl_refcursor_params.DeleteAll();
	
	_spl_delimiter_set = false;
	_spl_user_exceptions.DeleteAll();
	_spl_outer_label = NULL;
	_spl_cursor_params.DeleteAll();
	_spl_cursor_vars.DeleteAll();
	_spl_declared_cursors.DeleteAll();
	_spl_declared_cursors_using_vars.DeleteAll();
	_spl_declared_cursors_stmts.DeleteAll();
	_spl_updatable_current_of_cursors.DeleteAll();
	_spl_declared_cursors_select.DeleteAll();
	_spl_declared_cursors_select_first_exp.DeleteAll();
	_spl_declared_cursors_select_exp.DeleteAll();

	_spl_current_declaring_cursor = NULL;
	_spl_current_declaring_cursor_uses_vars = false;

	_spl_rowtype_vars.DeleteAll();
	_spl_rowtype_fields.DeleteAll();
	_spl_rowtype_fetches.DeleteAll();

	_spl_implicit_rowtype_vars.DeleteAll();
	_spl_implicit_rowtype_fields.DeleteAll();
	_spl_implicit_rowtype_fetches.DeleteAll();

	_spl_declared_local_tables.DeleteAll();
	_spl_created_session_tables.DeleteAll();
	_spl_declared_tables_with_replace.DeleteAll();

	_spl_tr_new_columns.DeleteAll();
	_spl_tr_old_columns.DeleteAll();
	_spl_param_close = NULL;

	_spl_returns = NULL;
	_spl_returning_out_names.DeleteAll();
	_spl_returning_datatypes.DeleteAll();
	_spl_return_with_resume = 0;
	_spl_returning_end = NULL;
	_spl_return_int = false;

	_spl_foreach_num = 0;
	_spl_moved_if_select = 0;
	_spl_return_num = 0;

	_spl_last_insert_table_name = NULL;
	_spl_last_open_cursor_name = NULL;
	_spl_last_fetch_cursor_name = NULL;
	_spl_open_cursors.DeleteAll();

	_spl_prepared_stmts.DeleteAll();
	_spl_prepared_stmts_cursors.DeleteAll();
	_spl_prepared_stmts_cursors_with_return.DeleteAll();

	_spl_implicit_record_fields.DeleteAll();

	_spl_declared_rs_locators.DeleteAll();
	_spl_rs_locator_procedures.DeleteAll();
	_spl_rs_locator_cursors.DeleteAll();

    _spl_loops.DeleteAll();
    _spl_loop_labels = 0;

	_spl_sp_calls.DeleteAll();

	_spl_proc_to_func = false;
	_spl_func_to_proc = false;
    _spl_not_found_handler = false;
	_spl_need_not_found_handler = false;
	_spl_need_rowcount_var = false;
	_spl_dyn_sql_var = false;

	_spl_monday_1 = false;   // means unknown from context

	_exp_select = 0;
}

// Set application type (Java, C#, PowerBuilder, COBOL etc.)
void SqlParser::SetApplicationSource()
{
	// Set the target language
	if(_source_app == APP_COBOL)
	{
		_cobol = new Cobol();
		
		// PL/SQL in Oracle
		if(_target == SQL_ORACLE)
			_target_app = APP_PLSQL;

		_cobol->SetParser(this);
		_cobol->SetTypes(_source, _target, _target_app);
	}
}

// Enter the specified scope
void SqlParser::Enter(int scope)
{
	_scope.Add(scope);
}

// Leave the specified scope
void SqlParser::Leave(int scope)
{
	// No scope defined
	if(_scope.GetCount() == 0)
		return;

	// Search the first matching item from the end 
	for(ListwmItem *i = _scope.GetLast(); i != NULL; i = i->prev)
	{
		if(scope == i->ivalue)
		{
			ListwmItem *prev = NULL;

			// Delete all following scopes
			for(ListwmItem *j = _scope.GetLast(); j != NULL; j = prev)
			{
				if(j == i)
					break;

				// Assign previous before deletion
				prev = j->prev;

				_scope.DeleteItem(j);
			}

			_scope.DeleteItem(i);
			break;
		}
	}
}

// Check if the parser is in the specified scope
bool SqlParser::IsScope(int scope, int scope2)
{
	// No scope defined
	if(_scope.GetCount() == 0)
		return false;

	bool exists = false;

	// Search the first matching item from the end 
	for(ListwmItem *i = _scope.GetLast(); i != NULL; i = i->prev)
	{
		int s = i->ivalue;

		if(s == scope || s == scope2)
		{
			exists = true;
			break;
		}
	}

	return exists;
}

// Generate cursor name for standalone SELECT returning a result set
void SqlParser::GenerateResultSetCursorName(TokenStr *name)
{
	if(name == NULL)
		return;

	name->Append("cur", L"cur", 3);

	// Not incremented yet - cur, cur2, cur3, ...
	if(_spl_result_sets > 0)
		name->Append(_spl_result_sets + 1);
}

// Add FOR SELECT for WITH RETURN cursors
bool SqlParser::OpenWithReturnCursor(Token *name)
{
	// No WITH RETURN cursors
	if(_spl_result_sets == 0 || name == NULL)
		return false;

	bool exists = false;
	CopyPaste *cur = _copypaste.GetFirstNoCurrent();

	// Find the cursor declaration
	while(cur != NULL)
	{
		if(cur->type != COPY_CURSOR_WITH_RETURN || Token::Compare(name, cur->name) == false)
		{
			cur = cur->next;
			continue;
		}

		Token *token = cur->tokens.GetFirstNoCurrent();

		Append(name, " ", L" ", 1);

		// Copy tokens
		while(token != NULL)
		{
			AppendCopy(name, token);
			token = token->next;
		}

		exists = true;
		cur = cur->next;
	}

	return exists;
}

// Check if it is specified to convert function to procedure
bool SqlParser::IsFuncToProc(Token *name)
{
	if(name == NULL || name->str == NULL)
		return false;

	std::string sname(name->str, name->len);
	std::transform(sname.begin(), sname.end(), sname.begin(), ::tolower);

	StringMap::iterator i = _func_to_sp_map.find(sname);

	if(i != _func_to_sp_map.end())
     return true;

	return false;
}

// Check if the conversion running in evaluation mode and add comment
void SqlParser::AddEvalModeComment(Token *token)
{
	if(token == NULL || !_option_eval_mode)
		return;

	PREPEND_NOFMT(token, "-- SQLINES LICENSE FOR EVALUATION USE ONLY\n\n");
}