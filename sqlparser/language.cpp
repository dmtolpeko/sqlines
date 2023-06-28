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

// SQLParser for language elements

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "str.h"
#include "cobol.h"

// Convert an identifier
void SqlParser::ConvertIdentifier(Token *token, int type, int scope)
{
	if(token == NULL || token->len == 0)
		return;
	
	token->type = TOKEN_IDENT;

	// Check for NEW or OLD column reference in trigger
	if(ConvertTriggerNewOldColumn(token) == true)
		return;

	// Convert table, view, procedure, function etc. name
	if(type == SQL_IDENT_OBJECT)
	{
		ConvertObjectIdentifier(token, scope);
		return;
	}
	else
	// Convert a parameter 
	if(type == SQL_IDENT_PARAM)
	{
		ConvertParameterIdentifier(token);
		return;
	}
	else
	// Convert a local variable
	if(type == SQL_IDENT_VAR)
	{
		ConvertVariableIdentifier(token);
		return;
	}

	// Check for a cursor parameter reference NOW PROCESSED AS LOCAL BLOCK VAR
	//if(_spl_current_declaring_cursor != NULL && ConvertCursorParameter(token) == true)
	//	return;

	// Check for record variable reference
	if(ConvertRecordVariable(token) == true)
		return;

	// Check for implicit record variable reference
	if(ConvertImplicitRecordVariable(token))
		return;

	// Check for implicit field reference (without record name)
	if(ConvertImplicitForRecordVariable(token) == true)
		return;

	// Check for local temporary table column reference
	if(ConvertLocalTableColumn(token) == true)
		return;

	// Convert column name
	if((type == SQL_IDENT_COLUMN || type == SQL_IDENT_COLUMN_SINGLE) && Source(SQL_ORACLE) == true && 
        Target(SQL_MYSQL) == true)
	{
		ConvertColumnIdentifier(token, scope);
		return;
	}
    else
    if(type == SQL_IDENT_COLUMN_SINGLE && Source(SQL_TERADATA) == true && Target(SQL_ESGYNDB) == true)
	{
		ConvertColumnIdentifierSingle(token, scope);
		return;
	}

	// Column, variable or parameter
	if(type == SQL_IDENT_COLVAR)
	{
		if(_source == SQL_DB2)
			ConvertDb2Identifier(token, type);
		if(_source == SQL_MYSQL)
			ConvertMysqlIdentifier(token, type);
	}
	else
	if(_source == SQL_POSTGRESQL)
	{
		TokenStr ident;
		size_t len = 0;

		// Get number of parts in quailified identifier
		int parts = GetIdentPartsCount(token);

		// Object containing schema
		if(type == SQL_IDENT_OBJECT && parts > 1)
			ConvertSchemaName(token, ident, &len);
	}
	else
	{
	// OLD CODE BELOW

	const char *cur = token->str;

	if(cur == NULL)
		return;

	// SQL Server, Sybase variable or parameter starting with @
	//if(ConvertTsqlVariable(token) == true)
	//	return;
	//else
	// Oracle pseudo-column (nextval i.e)
	if(ConvertOraclePseudoColumn(token) == true)
		return;

	// For SQL Server change ` to []
	if(_target == SQL_SQL_SERVER)
	{
		if(*cur == '`')
		{
			char *ident = new char[token->len + 1];

			*ident = '[';

			// Copy the value within quotes
			size_t i = 1;
			for(i = 1; i < token->len - 1; i++)
				ident[i] = cur[i];

			ident[i] = ']';
			ident[i + 1] = 0;

			token->t_str = ident;
			token->t_len = token->len;
		}
	}		
	else
	// Remove brace and backtick 
	if(Target(SQL_ORACLE, SQL_DB2, SQL_POSTGRESQL, SQL_GREENPLUM) == true)
	{
		if(*cur == '[' || *cur == '`')
		{
			char *ident = new char[token->len + 1];

			size_t i = 0;

			// Copy the value within quotes
			for(size_t k = 1; k < token->len - 1; k++)
			{
				if(cur[k] != '[' && cur[k] != ']' && cur[k] != '`')
				{
					ident[i] = cur[k];
					i++;
				}
			}

			ident[i] = 0;

			token->t_str = ident;
			token->t_len = i;
		}
	}		
	else
	// Change quote to backtick for MySQL (" is not suppored by default in MySQL)
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		if(*cur == '"' || *cur == '[')
		{
			char *ident = new char[token->len + 1];

			*ident = '`';

			char open = *cur;
			char close = (*cur == '[') ? ']' : *cur;

			// Copy the value within quotes
			size_t i = 1;
			for(i = 1; i < token->len - 1; i++)
			{
				// Open or close quote is faced inside, can be qualified identifier
				if(cur[i] == open || cur[i] == close)
					ident[i] = '`';
				else
					ident[i] = cur[i];
			}

			ident[i] = '`';
			ident[i + 1] = 0;

			token->t_str = ident;
			token->t_len = token->len;
		}
	}		
	}
}

// Convert table, view, procedure, function etc. name
void SqlParser::ConvertObjectIdentifier(Token *token, int scope)
{
	if(token == NULL)
		return;

	// Check for local table variable
	if(ConvertTableVariable(token) == true)
		return;

	// Check for session temporary table
	if(ConvertSessionTemporaryTable(token) == true)
		return;

	// Check if object name mapping is set
	if(MapObjectName(token) == true)
		return;

	TokenStr ident;
	size_t len = 0;

	// Package function or procedure, add a prefix
	if(_spl_package != NULL && _target != SQL_ORACLE && (scope == SQL_SCOPE_FUNC || scope == SQL_SCOPE_PROC))
		PrefixPackageName(ident);

	// Get the number of parts in quailified identifier
	int parts = GetIdentPartsCount(token);

	// Object contains a schema
	if(parts > 1)
		ConvertSchemaName(token, ident, &len);
	else
	// Set explicit schema if defined by the option
	if(!_option_set_explicit_schema.empty())
	{
		TokenStr schema;
		schema.Append(_option_set_explicit_schema.c_str(), L"", _option_set_explicit_schema.length());
		schema.Append(".", L".", 1);
		schema.Append(ident);
		ident.Set(schema);
	}

	ConvertObjectName(token, ident, &len);

	// If identifier was modified set the target value
	if(Token::Compare(token, &ident) == false)
		Token::ChangeNoFormat(token, ident);
}

// Convert column name
void SqlParser::ConvertColumnIdentifier(Token *token, int /*scope*/)
{
	if(token == NULL)
		return;

	TokenStr ident;
	size_t len = 0;

	// Get the number of parts in quailified identifier
	int parts = GetIdentPartsCount(token);

	// Identifier contains a schema
	if(parts > 2)
		ConvertSchemaName(token, ident, &len);

	// Identifier contains a table
	if(parts > 1)
	{
		ConvertObjectName(token, ident, &len);

		ident.Append(token, len, 1);
		len = len + 1;
	}

	ConvertObjectName(token, ident, &len);

	// If identifier was modified set the target value
	if(Token::Compare(token, &ident) == false)
		Token::ChangeNoFormat(token, ident);
}

// Convert single (not qualified) column identifier
void SqlParser::ConvertColumnIdentifierSingle(Token *token, int /*scope*/)
{
    if(token == NULL || token->str == NULL)
		return;

	TokenStr ident;
    bool changed = false;

    for(size_t i = 0; i < token->len; i++)
    {
        // $ allowed in Teradata but not allowed in EsgynDB
        if(TOKEN_CMPCP(token, '$', i) == true)
        {
            if(Target(SQL_ESGYNDB))
            {
                ident.Append("_", L"_", 1);
                changed = true;
            }            
        }
        else
            ident.Append(token, i, 1);
    }

    if(changed)
        Token::ChangeNoFormat(token, ident); 
}

// Oracle PL/SQL cursor parameter reference NOW PROCESSED AS LOCAL BLOCK VAR
bool SqlParser::ConvertCursorParameter(Token *token)
{
	if(token == NULL || _spl_current_declaring_cursor == NULL)
		return false;

	bool exists = false;

	// Use variables instead of cursor parameters 
	if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true && _spl_cursor_params.GetCount() > 0)
	{
		for(ListwmItem *i = _spl_cursor_params.GetFirst(); i != NULL; i = i->next)
		{
			Token *cursor_name = (Token*)i->value2;

			if(Token::Compare(cursor_name, _spl_current_declaring_cursor) == true)
			{
				Token *param = (Token*)i->value3;

				if(Token::Compare(param, token) == true)
				{
					// The target value of parameter already contains variable name
					Token::Change(token, param);

					exists = true;
					break;
				}
			}			
		}
	}

	return exists;
}

// %ROWTYPE record variable reference rec.field 
bool SqlParser::ConvertRecordVariable(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	ListwItem *item = _spl_rowtype_vars.GetFirst();

	// Compare identifier prefix with existing record name
	while(item != NULL)
	{
		Token *rec = (Token*)item->value;

		// Check for rec. prefix
		if(rec != NULL && Token::Compare(rec, token, rec->len) == true && 
			token->Compare(".", L".", rec->len, 1) == true)
		{
			// Change to @rec_field in SQL Server, rec_field in MySQL
			if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
			{
				TokenStr ident;

				if(_target == SQL_SQL_SERVER)
					ident.Append("@", L"@", 1);

				// Append record name _ and field name 
				ident.Append(token, 0, rec->len);
				ident.Append("_", L"_", 1);
				ident.Append(token, rec->len + 1, token->len - rec->len - 1);

				Token::ChangeNoFormat(token, ident);
			}

			// Save referenced record fields (once only)
			if(Find(_spl_rowtype_fields, rec, token) == NULL)
				_spl_rowtype_fields.Add(rec, token);

			exists = true;
			break;
		}

		item = item->next;
	}

	return exists;
}

// Implicit record (cursor name) variable reference rec.field 
bool SqlParser::ConvertImplicitRecordVariable(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	ListwmItem *item = _spl_implicit_rowtype_vars.GetFirst();

	// Compare identifier prefix with existing record name
	while(item != NULL)
	{
		exists = ConvertImplicitRecordVariable(token, (Token*)item->value);

		if(exists)
			break;

		item = item->next;
	}

	// Sybase ADS allows referencing field of previously declared cursor in declaration of another cursor
	if(!exists && Source(SQL_SYBASE_ADS) && _spl_current_declaring_cursor != NULL)
	{
		ListwItem *item = _spl_declared_cursors.GetFirst();

		while(item != NULL)
		{
			exists = ConvertImplicitRecordVariable(token, (Token*)item->value);

			if(exists)
			{
				_spl_current_declaring_cursor_uses_vars = true;
				break;
			}

			item = item->next;
		}
	}

	return exists;
}

bool SqlParser::ConvertImplicitRecordVariable(Token *token, Token *rec)
{
	// Check for rec. prefix
	if(rec != NULL && Token::Compare(rec, token, rec->len) && token->Compare(".", L".", rec->len, 1))
	{
		// Change to @rec_field in SQL Server, rec_field in MySQL
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL))
		{
			TokenStr ident;

			if(_target == SQL_SQL_SERVER)
				ident.Append("@", L"@", 1);

			// Append record name _ and field name 
			ident.Append(token, 0, rec->len);
			ident.Append("_", L"_", 1);
			ident.Append(token, rec->len + 1, token->len - rec->len - 1);

			Token::ChangeNoFormat(token, ident);
		}

		// Save referenced record fields (once only)
		if(Find(_spl_implicit_rowtype_fields, rec, token) == NULL)
			_spl_implicit_rowtype_fields.Add(rec, token);

		return true;
	}

	return false;
}

// Check for implicit field reference (without record name)
bool SqlParser::ConvertImplicitForRecordVariable(Token *token)
{
	// DB2 allows referencing implicit record fields without qualified name
	if(token == NULL || _source != SQL_DB2 || _spl_implicit_record_fields.GetCount() == 0)
		return false;

	bool exists = false;

	ListwmItem *i = Find(_spl_implicit_record_fields, NULL, token);

	// Field found
	if(i != NULL && i->value != NULL && i->value2 != NULL)
	{
		Token *rec = (Token*)i->value;

		// Make sure name is not qualified
		if(Token::Compare(token, rec, rec->len) == false)
		{
			// There can be conflicts with column names, check the current scope
			if(IsScope(SQL_SCOPE_INSERT_VALUES) == true ||
				// Assignment but not SET var = (SELECT ...)
				(IsScope(SQL_SCOPE_ASSIGNMENT_RIGHT_SIDE) == true && IsScope(SQL_SCOPE_SELECT_STMT) == false))
			{
				TokenStr new_name(rec);
				new_name.Append(".", L".", 1);
				new_name.Append(token);

				Token::ChangeNoFormat(token, new_name);
			}
		}

		exists = true;
	}

	return exists;
}

// Check for NEW or OLD column reference in trigger
bool SqlParser::ConvertTriggerNewOldColumn(Token *token)
{
	if(token == NULL || _spl_scope != SQL_SCOPE_TRIGGER)
		return false;

	bool exists = false;

	// Check for new. prefix in DB2
	if(_spl_new_correlation_name != NULL && 
		Token::Compare(_spl_new_correlation_name, token, _spl_new_correlation_name->len) == true && 
			token->Compare(".", L".", _spl_new_correlation_name->len, 1) == true)
	{
		// :new.column in Oracle, but not in WHEN condition
		if(_target == SQL_ORACLE && IsScope(SQL_SCOPE_TRG_WHEN_CONDITION) == false)
		{
			TokenStr ident(":", L":", 1);
			ident.Append(token);

			Token::ChangeNoFormat(token, ident);
		}

		exists = true; 
	}
	else
	// Check for default new. prefix in DB2
	if(_spl_new_correlation_name == NULL && Token::Compare(token, "NEW", L"NEW", 1, 3) == true && 
			token->Compare(".", L".", 4, 1) == true)
	{
		// :new.column in Oracle, but not in WHEN condition
		if(_target == SQL_ORACLE && IsScope(SQL_SCOPE_TRG_WHEN_CONDITION) == false)
		{
			TokenStr ident(":", L":", 1);
			ident.Append(token);

			Token::ChangeNoFormat(token, ident);
		}

		exists = true; 
	}
	else
	// Check for old. prefix in DB2
	if(_spl_old_correlation_name != NULL && 
		Token::Compare(_spl_old_correlation_name, token, _spl_old_correlation_name->len) == true && 
			token->Compare(".", L".", _spl_old_correlation_name->len, 1) == true) 
	{
		// Change to @old_field in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			TokenStr ident("@", L"@", 1);

			size_t old_len = _spl_old_correlation_name->len;

			// Append OLD name _ and column name 
			ident.Append(token, 0, old_len);
			ident.Append("_", L"_", 1);
			ident.Append(token, old_len + 1, token->len - old_len - 1);

			Token::ChangeNoFormat(token, ident);
		}
		else
		// :old.column in Oracle, but not in WHEN condition
		if(_target == SQL_ORACLE && IsScope(SQL_SCOPE_TRG_WHEN_CONDITION) == false)
		{
			TokenStr ident(":", L":", 1);
			ident.Append(token);

			Token::ChangeNoFormat(token, ident);
		}

		// Save referenced old columns (once only)
		if(Find(_spl_tr_old_columns, token) == NULL)
			_spl_tr_old_columns.Add(token);
		
		exists = true;
	}
	else
	if(_spl_old_correlation_name == NULL && Token::Compare(token, "OLD", L"OLD", 1, 3) == true && 
			token->Compare(".", L".", 4, 1) == true)
	{
		// :old.column in Oracle, but not in WHEN condition
		if(_target == SQL_ORACLE && IsScope(SQL_SCOPE_TRG_WHEN_CONDITION) == false)
		{
			TokenStr ident(":", L":", 1);
			ident.Append(token);

			Token::ChangeNoFormat(token, ident);
		}

		// Save referenced old columns (once only)
		if(Find(_spl_tr_old_columns, token) == NULL)
			_spl_tr_old_columns.Add(token);

		exists = true;
	}		

	return exists;
}

// Convert declared local temporary table (Sybase ASA), table variable (SQL Server) 
bool SqlParser::ConvertTableVariable(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	ListwItem *item = _spl_declared_local_tables.GetFirst();

	// Compare identifier with existing declared local table (table variable)
	while(item != NULL)
	{
		Token *table = (Token*)item->value;

		if(table != NULL && Token::Compare(table, token) == true)
		{
			// If the variable was changed, set new name to all references
			if(table->IsTargetSet() == true)
				Token::Change(token, table);	

			exists = true;
			break;
		}

		item = item->next;
	}

	return exists;
}

// Check for session temporary table
bool SqlParser::ConvertSessionTemporaryTable(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	// In DB2 temporary table reference includes SESSION. qualifier
	if((_source == SQL_DB2 && _target != SQL_DB2 && token->Compare("SESSION.", L"SESSION.", 0, 8)) ||
		// In Sybase ASE temporary table reference includes TEMPDB.. qualifier
		(_source == SQL_SYBASE && _target != SQL_SYBASE && token->Compare("TEMPDB..", L"TEMPDB..", 0, 8)))
	{
		TokenStr name;

		// For SQL Server replace SESSION. with #
		if(_target == SQL_SQL_SERVER)
		{
			name.Append("#", L"#", 1);
			name.Append(token, 8, token->len - 8);
		}
		// Remove SESSION. for other databases
		else
		if(_source == SQL_DB2)
			name.Append(token, 8, token->len - 8);
		else
		// Add temp_ prefix to avoid name conflicts with regular tables
		if(_source == SQL_SYBASE)
		{
			name.Append("temp_", L"temp_", 5);
			name.Append(token, 8, token->len - 8);
		}

		Token::ChangeNoFormat(token, name);
		exists = true;
	}
	
	return exists;
}

// Check for local temporary table column reference (Sybase ASA)
bool SqlParser::ConvertLocalTableColumn(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	ListwItem *item = _spl_declared_local_tables.GetFirst();

	// Compare identifier prefix with existing local temporary name
	while(item != NULL)
	{
		Token *table = (Token*)item->value;

		// Check for table. prefix
		if(table != NULL && Token::Compare(table, token, table->len) == true && 
			token->Compare(".", L".", table->len, 1) == true)
		{
			// If the table variable was changed, set new name to all references
			if(table->IsTargetSet() == true)
			{
				TokenStr ident(table);

				// Append referenced column name
				ident.Append(token, table->len, token->len - table->len);

				Token::ChangeNoFormat(token, ident);
			}

			exists = true;
			break;
		}

		item = item->next;
	}

	return exists;
}

// Convert a procedure, function parameter
void SqlParser::ConvertParameterIdentifier(Token *token)
{
	if(token == NULL && token->len > 0)
		return;

	// Convert @ to p_ for SQL Server, Sybase
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
	{
		if(Token::Compare(token, '@', L'@', 0) == true)
		{
			TokenStr name2("p_", L"p_", 2);
			name2.Append(token, 1, token->len - 1);

			Token::ChangeNoFormat(token, name2);
		}
	}
	else
	// Remove `` in MySQL
	if(_source == SQL_MYSQL && _target != SQL_MYSQL)
	{
		if(Token::Compare(token, '`', L'`', 0) == true)
		{
			TokenStr name2;
			name2.Append(token, 1, token->len - 2);

			Token::ChangeNoFormat(token, name2);
		}
	}
}

void SqlParser::ConvertParameterIdentifier(Token *ref, Token *decl)
{
	if(ref == NULL || decl == NULL || decl->t_len == 0)
		return;

	// Set value of the referenced parameter to the value changed at the declaration
	Token::Change(ref, decl);

	// Propagate attributes
	ref->type = decl->type;
	ref->data_type = decl->data_type;
	ref->data_subtype = decl->data_subtype;
}

// Convert a local variable
void SqlParser::ConvertVariableIdentifier(Token *token)
{
	if(token == NULL && token->len > 0)
		return;

	// Check if a variable with this name already declared
	Token *var = GetVariable(token);

	if(var != NULL && var->t_len != 0)
	{
		ConvertVariableIdentifier(token, var);
		return;
	}
	
	// Convert @ to v_ for SQL Server, Sybase
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
	{
		if(Token::Compare(token, '@', L'@', 0) == true)
		{
			TokenStr name2("v_", L"v_", 2);
			name2.Append(token, 1, token->len - 1);

			Token::ChangeNoFormat(token, name2);
		}
	}
}

void SqlParser::ConvertVariableIdentifier(Token *ref, Token *decl)
{
	if(ref == NULL || decl == NULL || decl->t_len == 0)
		return;

	// Set value of the referenced variable to the value changed at the declaration
	Token::Change(ref, decl);

	// Propagate attributes
	ref->type = decl->type;
	ref->data_type = decl->data_type;
	ref->data_subtype = decl->data_subtype;
}

// SQL Server, Sybase variable or parameter starting with @
bool SqlParser::ConvertTsqlVariable(Token *token)
{
	if(token == NULL || Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
		return false;

	const char *cur = token->str;

	if(cur == NULL || *cur != '@')
		return false;

	char *ident = new char[token->len + 1];

	*ident = 'v';

	// Copy content except @
	for(size_t i = 1; i < token->len; i++)
		ident[i] = cur[i];

	ident[token->len] = 0;

	token->t_str = ident;
	token->t_len = token->len;

	return true;
}

// Add @ for SQL Server, Sybase variable or parameter
bool SqlParser::ConvertToTsqlVariable(Token *name)
{
	if(name == NULL)
		return false;

	const char *cur = name->str;

	// Check if there is already starting @
	if(cur == NULL || *cur == '@')
		return false;

	// Allocate space for @ and terminating 0
	char *ident = new char[name->len + 2];

	*ident = '@';

	size_t len = name->len + 1;
	size_t pos = 1;

	// Copy content after @
	for(size_t i = 0; i < name->len; i++)
	{
		// Check for a special character to be replaced by @ at first position rather than appended
		if(i == 0 && cur[i] == '#')
			len--;
		else
		{
			ident[pos] = cur[i];
			pos++;
		}
	}

	ident[len] = 0;

	name->t_str = ident;
	name->t_len = len;

	return true;
}

// Oracle pseudo-column (nextval i.e.)
bool SqlParser::ConvertOraclePseudoColumn(Token *token)
{
	if(token == NULL)
		return false;

	STATS_DTL_DECL

	size_t len = token->len;

	// .nextval reference
	if(len > 8 && token->Compare(".nextval", L".nextval", len - 8, 8) == true)
	{
		STATS_DTL_DESC(SEQUENCE_NEXTVAL_DESC)

		if(Target(SQL_MARIADB))
		{
			PREPEND(token, "NEXTVAL(");
			Token::Change(token, token->str, token->wstr, len - 8);
			APPEND_NOFMT(token, ")");
			
			STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
		}
		else
		// Supported in Oracle compatibility mode for MariaDB
		if(Target(SQL_MARIADB_ORA))
		{
			STATS_DTL_CONV_NO_NEED(Target(SQL_MARIADB_ORA))
		}
		else
		// NextVal('seqname') user-defined function in MySQL
		if(Target(SQL_MYSQL))
		{
			Prepend(token, "NextVal('", L"NextVal('", 9); 
			Token::Change(token, token->str, token->wstr, len - 8);
			Append(token, "')", L"')", 2);
		}
		
		token->subtype = TOKEN_SUB_IDENT_SEQNEXTVAL;
		
		SEQ_REF_STATS(".NEXTVAL", token);
		SEQ_REF_DTL_STATS(token);
		
		return true;
	}
	else
	// .currval reference
	if(len > 8 && token->Compare(".currval", L".currval", len - 8, 8) == true)
	{
		STATS_DTL_DESC(SEQUENCE_CURRVAL_DESC)

		if(Target(SQL_MARIADB))
		{
			PREPEND(token, "LASTVAL(");
			Token::Change(token, token->str, token->wstr, len - 8);
			APPEND_NOFMT(token, ")");

			STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
		}
		else
		// Supported in Oracle compatibility mode for MariaDB
		if(Target(SQL_MARIADB_ORA))
		{
			STATS_DTL_CONV_NO_NEED(Target(SQL_MARIADB_ORA)) 
		}
		else
		// LastVal('seqname') user-defined function in MySQL
		if(Target(SQL_MYSQL))
		{
			Prepend(token, "LastVal('", L"LastVal('", 9); 
			Token::Change(token, token->str, token->wstr, len - 8);
			Append(token, "')", L"')", 2);
		}

		SEQ_REF_STATS(".CURRVAL", token);
		SEQ_REF_DTL_STATS(token);
		return true;
	}

	return false;
}

// Column constraint or propertirs
bool SqlParser::ParseColumnConstraints(Token *create, Token *table_name, Token *column, Token *type, Token *type_end,
										Token **id_col, Token **id_start, Token **id_inc, bool *id_default)
{
	if(column == NULL)
		return false;

	bool exists = false;

	int num = 0;

	Token *null = NULL;

	// Parse until , or )
	while(true)
	{
		Token *cns = GetNextToken();

		if(cns == NULL)
			break;

		if(cns->Compare(',', L',') == true || cns->Compare(';', L';') == true || 
			cns->Compare(')', L')') == true)
		{
			PushBack(cns);
			break;
		}
		else
		// Inline named constraint can be specified in SQL Server, Oracle
		if(ParseInlineColumnConstraint(type, type_end, cns, num) == true)
		{
			num++;
		}
		else
		// NULL constraint
		if(cns->Compare("NULL", L"NULL", 4) == true)
		{
			null = cns;
			num++;
		}
		else
		// Check for NOT NULL constraint, NOT LOGGED/COMPACT
		if(cns->Compare("NOT", L"NOT", 3) == true)
		{
			Token *next = GetNextToken();

            if(TOKEN_CMP(next, "NULL") == true)
            {
                if(_obj_scope == SQL_SCOPE_TABLE)
                    CREATE_TAB_STMS_STATS("NOT NULL constraints")

                if(_target == SQL_HIVE)
                    Token::Remove(cns, next);
            }
            else
            // DB2 NOT LOGGED and NOT COMPACT 
            if(TOKEN_CMP(next, "LOGGED") == true || TOKEN_CMP(next, "COMPACT") == true)
            {
                if(_target != SQL_DB2)
    				Token::Remove(cns, next);
            }
            else
            // NOT CASESPECIFIC in Teradata, EsgynDB 
            if(TOKEN_CMP(next, "CASESPECIFIC") == true)
            {
                if(Target(SQL_TERADATA, SQL_ESGYNDB) == false)
                    Token::Remove(cns, next);
                else
                // NOT CASESPECIFIC must go right after data type before NOT NULL/DEFAULT in EsgynDB
                if(_source != SQL_ESGYNDB && _target == SQL_ESGYNDB)
                {
                    AppendFirst(type_end, " NOT CASESPECIFIC", L" NOT CASESPECIFIC", 17, next);
                    Token::Remove(cns, next); 
                }
            }

			num++;
		}
		else
		// Inline CHECK constraint
		if(cns->Compare("CHECK", L"CHECK", 5) == true)
		{
			ParseCheckConstraint(cns);
			num++;
		}
		else
		// SQL Server IDENTITY(start, inc)
		if(cns->Compare("IDENTITY", L"IDENTITY", 8) == true)
		{
			if(id_col != NULL)
				*id_col = column;

			// Sequence and trigger will be used in Oracle
			if(_target == SQL_ORACLE)
				Token::Remove(cns);
			else
			if(_target == SQL_POSTGRESQL)
			{
				Token *seq_name = AppendIdentifier(table_name, "_seq", L"_seq", 4);
		
				// Generate CREATE SEQUENCE before CREATE TABLE
				Prepend(create, "CREATE SEQUENCE ", L"CREATE SEQUENCE ", 16);
				PrependCopy(create, seq_name);
				Prepend(create, ";\n\n", L";\n\n", 3);

				// Generate DEFAULT nextval('tablename_seq') clause
				Token::Change(cns, "DEFAULT ", L"DEFAULT ", 8);
				Append(cns, "nextval ('", L"nextval ('", 10);

				AppendCopy(cns, seq_name);
				Append(cns, "')", L"')", 2);

				delete seq_name;
			}
			else
			if(Target(SQL_MARIADB, SQL_MYSQL))
				Token::Change(cns, "AUTO_INCREMENT", L"AUTO_INCREMENT", 14);

			// Optional start and increment
			Token *open = GetNextCharToken('(', L'(');

			if(open != NULL)
			{
				// If start is specified, increment is mandatory
				Token *start = GetNextToken();
				/*Token *comma */ (void) GetNextCharToken(',', L',');
				Token *inc = GetNextToken();
				Token *close = GetNextCharToken(')', L')');

				if(Target(SQL_ORACLE, SQL_POSTGRESQL, SQL_MYSQL) == true)
					Token::Remove(open, close);

				if(id_start != NULL)
					*id_start = start;

				if(id_inc != NULL)
					*id_inc = inc; 
			}

			num++;
		}
		else
		// SQL Server ROWGUIDCOL property
		if(cns->Compare("ROWGUIDCOL", L"ROWGUIDCOL", 10) == true)
		{
			// GUIDs are not automatically generated, so remove the keyword
			if(_target != SQL_SQL_SERVER)
				Token::Remove(cns);

			num++;
		}
		else
		// Check for DEFAULT or DB2 WITH DEFAULT clause
		if(cns->Compare("DEFAULT", L"DEFAULT", 7) == true || cns->Compare("WITH", L"WITH", 4) == true)
		{
			ParseDefaultExpression(type, type_end, cns, num);
			num++;
		}
		// Oracle ENABLE contraint state
		else
		if(cns->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			// Remove for other databases
			if(_target != SQL_ORACLE)
				Token::Remove(cns);

			num++;
		}
		// DB2 GENERATED ALWAYS | BY DEFAULT AS IDENTITY 
		else
		if(cns->Compare("GENERATED", L"GENERATED", 9) == true)
		{
			ParseDb2GeneratedClause(create, table_name, column, cns, id_col, id_start, id_inc, id_default);

			num++;
		}
		else
		// DB2 FOR BIT | SBCS | MIXED DATA 
		if(cns->Compare("FOR", L"FOR", 3) == true)
		{
			Token *type = GetNextToken();
			Token *data = NULL;

			if(Token::Compare(type, "BIT", L"BIT", 3) == true || Token::Compare(type, "SBCS", L"SBCS", 4) == true ||
				Token::Compare(type, "MIXED", L"MIXED", 5) == true)
				data = GetNextWordToken("DATA", L"DATA", 4);

			if(_target != SQL_DB2)
				Token::Remove(cns, data);

			num++;
		}
		else
		// CCSID num in DB2 for OS/400
		if(cns->Compare("CCSID", L"CCSID", 5) == true)
		{
			Token *encoding = GetNextToken();
			
			if(_target != SQL_DB2)
				Token::Remove(cns, encoding);

			num++;
		}
		else
		// DB2 LOGGED clause for LOB data types
		if(cns->Compare("LOGGED", L"LOGGED", 6) == true)
		{
			// Remove for other databases
			if(_target != SQL_DB2)
				Token::Remove(cns);

			num++;
		}
		// DB2 COMPACT clause for LOB data types
		else
		if(cns->Compare("COMPACT", L"COMPACT", 7) == true)
		{
			// Remove for other databases
			if(_target != SQL_DB2)
				Token::Remove(cns);

			num++;
		}
		// DB2 INLINE LENGTH clause for LOB data types; Sybase ASA INLINE 
		else
		if(cns->Compare("INLINE", L"INLINE", 6) == true)
		{
			/*Token *length */ (void) GetNextWordToken("LENGTH", L"LENGTH", 6);
			Token *size = GetNextNumberToken();

			// Remove for other databases
			if(Target(SQL_DB2, SQL_SYBASE_ASA) == false)
				Token::Remove(cns, size);

			num++;
		}
		else
		// Sybase ASA PREFIX num 
		if(cns->Compare("PREFIX", L"PREFIX", 6) == true)
		{
			Token *size = GetNextNumberToken();

			// Remove for other databases
			if(_target != SQL_SYBASE_ASA)
				Token::Remove(cns, size);

			num++;
		}
		else
		// Sybase ASA COMPUTE (expr) 
		if(cns->Compare("COMPUTE", L"COMPUTE", 7) == true)
		{
			Token *open = GetNext('(', L'(');

			if(open != NULL)
			{			
				Token *exp = GetNextToken();
				ParseExpression(exp);
				/*Token *close */ (void) GetNext(')', L')');

				// Compute columns in SQL Server does not include data type and NULL/NOT NULL
				if(_target == SQL_SQL_SERVER)
				{
					Token::Change(cns, "AS", L"AS", 2);
					Token::Remove(type, type_end);
					Token::Remove(null);
				}
			}

			num++;
		}
		// Check for AUTO_INCREMENT clause in MySQL
		else
		if(cns->Compare("AUTO_INCREMENT", L"AUTO_INCREMENT", 14) == true)
		{
			// Change to IDENTITY for SQL Server
			if(_target == SQL_SQL_SERVER)
				Token::Change(cns, "IDENTITY", L"IDENTITY", 8);
			else
			// Remove for Oracle
			if(_target == SQL_ORACLE)
				Token::Remove(cns);
			else
			// For PostgreSQL and Greenplum, add DEFAULT nextval
			if(_target == SQL_POSTGRESQL || _target == SQL_GREENPLUM)
			{
				Token *seq_name = AppendIdentifier(table_name, "_seq", L"_seq", 4);
		
				// Generate CREATE SEQUENCE before CREATE TABLE
				Prepend(create, "CREATE SEQUENCE ", L"CREATE SEQUENCE ", 16);
				PrependCopy(create, seq_name);
				Prepend(create, ";\n\n", L";\n\n", 3);

				// Generate DEFAULT nextval('tablename_seq') clause
				Token::Change(cns, "DEFAULT ", L"DEFAULT ", 8);
				Append(cns, "nextval ('", L"nextval ('", 10);

				AppendCopy(cns, seq_name);
				Append(cns, "')", L"')", 2);

				delete seq_name;
			}

			if(id_col != NULL)
				*id_col = column;

			num++;
		}
		else
		// UNSIGNED in MySQL
		if(cns->Compare("UNSIGNED", L"UNSIGNED", 8) == true)
		{
			// Replace with CHECK constraint 
			if(_target != SQL_MYSQL)
			{
				Token::Change(cns, "CHECK (", L"CHECK (", 7);
				AppendCopy(cns, column); 
				AppendNoFormat(cns, " > 0)", L" > 0)", 5); 
			}

			num++;
		}
		else
		// COLLATE in MySQL
		if(cns->Compare("COLLATE", L"COLLATE", 7) == true)
		{
			// Name can be identifier or string literal
			Token *name = GetNextToken();

			// Remove for other databases 
			if(_target != SQL_MYSQL)
				Token::Remove(cns, name);

			num++;
		}
		else
		// CHARACTER SET in MySQL, Teradata
		if(cns->Compare("CHARACTER", L"CHARACTER", 9) == true)
		{
			Token *set = GetNextWordToken("SET", L"SET", 3);

			if(set == NULL)
				break;

			// Name can be identifier or string literal
			Token *name = GetNextToken();

			// Remove for SQL Server, Oracle, Hive 
			if(Target(SQL_SQL_SERVER, SQL_ORACLE, SQL_HIVE) == true)
				Token::Remove(cns, name);
            else
            // EsgynDB does not support LATIN charset
            if(_target == SQL_ESGYNDB && TOKEN_CMP(name, "LATIN") == true)
                Token::Remove(cns, name);

			num++;
		}
		else
		// COMMENT in MySQL
		if(cns->Compare("COMMENT", L"COMMENT", 7) == true)
		{
			// Get string literal
			Token *string = GetNextToken();

			// Remove if not MySQL 
			if(_target != SQL_MYSQL)
			{
				Token::Remove(cns);
				Token::Remove(string);
			}

			num++;
		}
		else
		// ON UPDATE CURRENT_TIMESTAMP clause in MySQL
		if(cns->Compare("ON", L"ON", 2) == true)
		{
			Token *update = GetNextWordToken("UPDATE", L"UPDATE", 6);
			Token *current = GetNextWordToken("CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);

			// Remove if not MySQL 
			if(_target != SQL_MYSQL)
			{
				Token::Remove(cns);
				Token::Remove(update);
				Token::Remove(current);
			}

			num++;
		}
		else
		// CASESPECIFIC Teradata
		if(cns->Compare("CASESPECIFIC", L"CASESPECIFIC", 12) == true)
		{
			if(_target != SQL_TERADATA)
				Token::Remove(cns);

			num++;
		}
		else
		// COMPRESS values clause in Teradata
		if(cns->Compare("COMPRESS", L"COMPRESS", 8) == true)
		{
			ParseTeradataCompressClauseDefaultExpression(cns);
			num++;
		}
        else
		// TITLE 'text' clause in Teradata
		if(TOKEN_CMP(cns, "TITLE") == true)
		{
            Token *value = GetNextStringToken();

			if(_target != SQL_TERADATA && value != NULL)
				Token::Remove(cns, value);

			num++;
		}
        else
		// FORMAT 'format' clause in Teradata
		if(TOKEN_CMP(cns, "FORMAT") == true)
		{
            Token *value = GetNextStringToken();

			if(_target != SQL_TERADATA && value != NULL)
				Token::Remove(cns, value);

			num++;
		}
		else
		// ENCRYPT in Sybase ASE
		if(TOKEN_CMP(cns, "ENCRYPT"))
		{
			if(_target != SQL_SYBASE)
				Token::Remove(cns);

			num++;
		}
	}

	return exists;
}

// Data type attribute for variable or parameter UNSIGNED i.e
bool SqlParser::ParseVarDataTypeAttribute()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// UNSIGNED in MySQL
		if(next->Compare("UNSIGNED", L"UNSIGNED", 8) == true)
		{
			// Remove 
			if(_target != SQL_MYSQL)
				Token::Remove(next);

			exists = true;
		}
		else
		// Not an attribute
		{
			PushBack(next);
			break;
		}
	}

	return exists;
}

// DEFAULT or DB2 WITH DEFAULT clause
bool SqlParser::ParseDefaultExpression(Token *type, Token *type_end, Token *token, int pos)
{
	if(token == NULL)
		return false;

	// Optional WITH in DB2
	if(token->Compare("WITH", L"WITH", 4) == true)
	{
		// Remove for other databases
		if(_target != SQL_DB2)
			Token::Remove(token);

		// DEFAULT keyword
		token = GetNextToken();
	}
	
	Token *first = GetNextToken();

	if(first == NULL)
		return false;

	Token *append_default_value = token;
	Token *default_end = NULL;

	// DB2 DEFAULT maybe without the expression and terminated by , ) or CHECK () or FOR BIT | SBCS | MIXED DATA
	if(first->Compare(',', L',') == true || first->Compare(')', L')') == true || 
		first->Compare("CHECK", L"CHECK", 5) == true || first->Compare("FOR", L"FOR", 3) == true)
	{
		PushBack(first);
		first = NULL;
	}
	else
	// Sybase ASA DEFAULT GLOBAL AUTOINCREMENT(partition_size)
	if(first->Compare("GLOBAL", L"GLOBAL", 6) == true)
	{
		Token *autoincrement = GetNext("AUTOINCREMENT", L"AUTOINCREMENT", 13);
		if(autoincrement != NULL)
		{
			/*Token *open */ (void) GetNext('(', L'(');
			/*Token *partition_size */ (void) GetNextToken();
			Token *close = GetNext(')', L')');

			if(_target == SQL_SQL_SERVER)
			{
				Token::Change(token, "IDENTITY", L"IDENTITY", 8);
				Token::Remove(first, close);
			}
		}
	}
	else
	{
		ParseExpression(first);
		default_end = GetLastToken();
	}

    bool commented = false;

    // Not all databases supports functions in DEFAULT
    if(first != NULL && first->type == TOKEN_FUNCTION)
    {
        // Only CURRENT_DATE/TIME/TIMESTAMP allowed in EsgynDB
        if(_target == SQL_ESGYNDB && TOKEN_CMP(first, "CURRENT_DATE") == false && 
            TOKEN_CMP(first, "CURRENT_TIMESTAMP") == false && TOKEN_CMP(first, "CURRENT_TIME") == false)
        {
            Comment(token, default_end);
            commented = true;
        }
    }

	Token *new_default = token;
	Token *new_default_first = first;

	// In Oracle and EsgynDB unlike DB2, MySQL DEFAULT must go right after data type
	if(pos > 0 && Target(SQL_ORACLE, SQL_ESGYNDB) && commented == false)
	{
		new_default = Append(type_end, " DEFAULT ", L" DEFAULT ", 9, token);
		new_default_first = AppendCopy(type_end, first, default_end, false);

		append_default_value = type_end;

		Token::Remove(token);
		Token::Remove(first, default_end);
	}

	// Oracle requires TO_TIMESTAMP around datetime literals i.e.
	FormatDefaultExpression(type, new_default_first, new_default);

	// In DB2 default value can be omitted, add the value explicitly depending on the data type
	if(first == NULL && _target != SQL_DB2)
	{
		// Specify DEFAULT '' for CHAR, VARCHAR and CLOB
		if(type->Compare("CHAR", L"CHAR", 4) == true || type->Compare("CHAR", L"CHAR", 4))
			Append(append_default_value, " ''", L" ''", 3);
		else
		// Specify DEFAULT 0 for INTEGER, DECIMAL and NUMERIC
		if(type->Compare("INTEGER", L"INTEGER", 7) == true || type->Compare("DECIMAL", L"DECIMAL", 7) == true || 
			type->Compare("NUMERIC", L"NUMERIC", 7) == true)
			Append(append_default_value, " 0", L" 0", 2);
		else
		// Specify current timestamp for TIMESTAMP
		if(type->Compare("TIMESTAMP", L"TIMESTAMP", 9) == true)
		{
			if(_target == SQL_SQL_SERVER)
				Append(append_default_value, " GETDATE()", L" GETDATE()", 10, token);
			else
			if(_target == SQL_ORACLE)
				Append(append_default_value, " SYSTIMESTAMP", L" SYSTIMESTAMP", 13, token);
			else
				Append(append_default_value, " CURRENT_TIMESTAMP", L" CURRENT_TIMESTAMP", 18, token);
		}
		else
		// Specify current date for DATE
		if(type->Compare("DATE", L"DATE", 4) == true)
		{
			if(_target == SQL_SQL_SERVER)
				Append(append_default_value, " GETDATE()", L" GETDATE()", 10, token);
			else
			if(_target == SQL_ORACLE)
				Append(append_default_value, " SYSDATE", L" SYSDATE", 8, token);
			else
				Append(append_default_value, " CURRENT_DATE", L" CURRENT_DATE", 13, token);
		}
		else
		// Default for CLOB
		if(type->Compare("CLOB", L"CLOB", 4) == true)
		{
			if(_target == SQL_ORACLE)
				Append(append_default_value, " EMPTY_CLOB()", L" EMPTY_CLOB()", 13, token);
			else
				Append(append_default_value, " ''", L" ''", 3);
		}
	}

    // Hive does not support DEFAULT
    if(_target == SQL_HIVE)
        Token::Remove(token, default_end);

	return true;
}

// Format DEFAULT expression
bool SqlParser::FormatDefaultExpression(Token *datatype, Token *exp, Token *default_)
{
	if(datatype == NULL || exp == NULL)
		return false;

	// Oracle requires TO_TIMESTAMP around datetime literals i.e.
	if(_source != SQL_ORACLE && _target == SQL_ORACLE)
	{
		// A datetime source data type with default string literal
		if(datatype->data_type == TOKEN_DT_DATETIME && exp->type == TOKEN_STRING)
		{
			TokenStr format;

			if(RecognizeOracleDateFormat(exp, format) == true)
			{
				// Use TO_DATE for dates, and TO_TIMESTAMP for datetimes
				if(datatype->data_subtype == TOKEN_DT2_DATE)
					Prepend(exp, "TO_DATE(", L"TO_DATE(", 8, default_);
				else
					Prepend(exp, "TO_TIMESTAMP(", L"TO_TIMESTAMP(", 13, default_);

				AppendNoFormat(exp, ", ", L", ", 2);
				Append(exp, &format);
				AppendNoFormat(exp, ")", L")", 1);
			}
		}
	}

	return true;
}

// Standalone column constraint
bool SqlParser::ParseStandaloneColumnConstraints(Token *alter, Token *table_name, ListW &pkcols,
												 ListWM *inline_indexes)
{
	bool exists = false;

	// In Oracle constraint can be enclosed with () i.e. ALTER TABLE ADD (PRIMARY KEY (c1,c2))
	Token *open = GetNext('(', L'(');

	if(open != NULL)
	{
		if(_target != SQL_ORACLE)
			Token::Remove(open);
	}

	while(true)
	{
		bool exists_now = false;

		Token *cns = GetNextToken();

		if(cns == NULL)
			break;

		// Check for constraint name
		if(cns->Compare("CONSTRAINT", L"CONSTRAINT", 10) == true)
		{
			Token *cns_name = GetNextIdentToken();

			// Constraint name is not supported in Redshift
			if(_target == SQL_REDSHIFT)
				Comment(cns, cns_name);

			// Now get constraint type keyword
			cns = GetNextToken();
		}

		// Parse PRIMARY KEY, UNIQUE in database and INDEX in MySQL 
		if(ParseKeyConstraint(alter, table_name, cns, pkcols, false, inline_indexes) == true)
			exists_now = true;
		else
		// Check for FOREIGN KEY
		if(ParseForeignKey(cns) == true)
			exists_now = true;
		else
		// CHECK constraint
		if(ParseCheckConstraint(cns) == true)
			exists_now = true;

		// Check for constraint option
		if(exists_now == true)
		{
			ParseConstraintOption();
			exists = true;

            // Hive does not support constraints
            if(_target == SQL_HIVE)
                Token::Remove(cns, GetLastToken());

			continue;
		}

		// Not a constraint
		PushBack(cns);
		break;
	}

	if(open != NULL)
	{
		Token *close = GetNext(')', L')');

		if(close != NULL)
		{
			if(_target != SQL_ORACLE)
				Token::Remove(close);
		}
	}

	return exists;
}

// Inline named constraint can be specified in SQL Server, Oracle
bool SqlParser::ParseInlineColumnConstraint(Token *type, Token *type_end, Token *constraint, int num)
{
	// CONSTRAINT keyword before the name
	if(constraint == NULL || Token::Compare(constraint, "CONSTRAINT", L"CONSTRAINT", 10) == false)
		return false;

	// Constraint name
	Token *name = GetNextToken();

	if(name == NULL)
		return false;

	// Constraint name is not supported in Redshift
	if(_target == SQL_REDSHIFT)
		Comment(constraint, name);

	// Constraint type keyword
	Token *cns = GetNextToken();

	if(cns == NULL)
		return false;

	bool exists = false;

	ListW pkcols;

	// Parse PRIMARY KEY, UNIQUE in database and INDEX in MySQL 
	if(ParseKeyConstraint(NULL, NULL, cns, pkcols, true, NULL) == true)
		exists = true;
	else
	// Check for FOREIGN KEY
	if(ParseForeignKey(cns) == true)
		exists = true;
	else
	// CHECK constraint
	if(ParseCheckConstraint(cns) == true)
		exists = true;
	else
	if(cns->Compare("DEFAULT", L"DEFAULT", 7) == true)
	{
		exists = ParseDefaultExpression(type, type_end, cns, num);

		// Named DEFAULT allowed in SQL Server, but not allowed in Oracle
		if(exists == true && _target == SQL_ORACLE)
			Token::Remove(constraint, name);
	}

	// Check for constraint option
	if(exists == true)
		ParseConstraintOption();

	return exists;
}

// Parse PRIMARY KEY, UNIQUE constraint clause, and INDEX/KEY in MySQL 
bool SqlParser::ParseKeyConstraint(Token * /*alter*/, Token *table_name, Token *cns, ListW &pkcols, 
									bool inline_cns, ListWM *inline_indexes)
{
	if(cns == NULL)
		return false;

	bool primary = false;
	bool unique = false; 

	// MySQL UNIQUE INDEX clause
	bool unique_index = false;
	bool index = false;

	Token *index_name = NULL;

	// PRIMARY KEY
	if(cns->Compare("PRIMARY", L"PRIMARY", 7) == true)
	{
		/*Token *key */ (void) GetNextWordToken("KEY", L"KEY", 3);
		primary = true;

        if(_stmt_scope == SQL_STMT_ALTER_TABLE)
            ALTER_TAB_STMS_STATS("Add PRIMARY KEY")
	}
	else
	// UNIQUE
	if(cns->Compare("UNIQUE", L"UNIQUE", 6) == true)
	{
		unique = true;

		// Check for MySQL UNIQUE KEY or UNIQUE INDEX
		Token *index = GetNextWordToken("INDEX", L"INDEX", 5);

		if(index == NULL)
			index = GetNextWordToken("KEY", L"KEY", 3);

		if(index != NULL)
		{
			unique_index = true;
			index_name = GetNextIdentToken();

			if(_target != SQL_MYSQL)
			{
				// if index/key name is specified use it as CONSTRAINT name
				if(index_name != NULL)
				{
					Prepend(cns, "CONSTRAINT ", L"CONSTRAINT ", 11);
					PrependCopy(cns, index_name);
					PrependNoFormat(cns, " ", L" ", 1);

					Token::Remove(index_name);
				}

				// Remove INDEX or KEY keyword for other databases (UNIQUE constraint will be used)
				Token::Remove(index);
			}
		}

        if(_stmt_scope == SQL_STMT_ALTER_TABLE)
            ALTER_TAB_STMS_STATS("Add UNIQUE constraint")
	}
	else
	// MySQL INDEX or KEY for inline non-unique index
	if(cns->Compare("INDEX", L"INDEX", 5) == true || cns->Compare("KEY", L"KEY", 3) == true)
	{
		index = true;
		index_name = GetNextIdentToken();

		// Standalone CREATE TABLE will be generated for other databases
		if(_target != SQL_MYSQL)
		{
			Token::Change(cns, "CREATE INDEX", L"CREATE INDEX", 12);

			Append(index_name, " ON ", L" ON ", 4, cns);
			AppendCopy(index_name, table_name);
		}
	}
	else
		return false;

	// In SQL Server PRIMARY and UNIQUE key can be CLUSTERED or NONCLUSTERED
	if(primary == true || unique == true)
	{
		// CLUSTERED or NONCLUSTERED keyword can go for SQL Server
		Token *clustered = GetNext("CLUSTERED", L"CLUSTERED", 9);
		Token *nonclustered = NULL;
		
		if(clustered == NULL)
			nonclustered = GetNext("NONCLUSTERED", L"NONCLUSTERED", 12);

		// Remove for other databases
		if(_target != SQL_SQL_SERVER)
		{
			Token::Remove(clustered);
			Token::Remove(nonclustered);
		}
	}

	Token *open = GetNextCharToken('(', L'(');

	if(open == NULL)
		return false;

	Book *cr_table_start = NULL;
	Book *cr_table_end = NULL;

	bool greenplum_distributed = false;

	// Get bookmarks to CREATE TABLE
	if(_target == SQL_GREENPLUM)
	{
		cr_table_start = GetBookmark(BOOK_CT_START, table_name);
		cr_table_end = GetBookmark(BOOK_CTC_ALL_END, table_name);

		// Add DISTRIBUTED BY to CREATE TABLE for Greenplum
		if(cr_table_start != NULL && cr_table_end != NULL && cr_table_end->distributed_by == false)
		{
			Append(cr_table_end->book, " DISTRIBUTED BY (", L" DISTRIBUTED BY (", 17, cr_table_start->book);
			greenplum_distributed = true;
		}
	}

	int count = 0;

	while(true)
	{
		Token *col = GetNextIdentToken();

		if(col == NULL)
			break;

		if(primary == true)
			pkcols.Add(col);

		// For Greenplum, add column to DISTRIBUTED BY in CREATE TABLE
		if(greenplum_distributed == true)
		{
			if(count > 0)
				Append(cr_table_end->book, ", ", L", ", 2);

			AppendCopy(cr_table_end->book, col);
		}

		count++;

		// For SQL Server PRIMARY KEY, for MySQL index next can be ASC DESC 
		Token *asc = GetNext("ASC", L"ASC", 3); 
		Token *desc = NULL;

		if(asc == NULL)
			desc = GetNext("DESC", L"DESC", 4);

		// Oracle does not allow specifying ASC, DESC in key constraint, but if it is index it will be
		// moved to a separate CREATE INDEX, so ASC and DESC can be used
		if(_target == SQL_ORACLE && index == false)
		{
			// ASC is the default in index so just remove it
			if(asc != NULL)
				Token::Remove(asc);

			if(desc != NULL)
				Comment(desc);
		}

		Token *next = GetNextToken();

		if(next == NULL)
			break;

		if(next->Compare(',', L',') == true)
			continue;

		// Leave if not comma
		PushBack(next);
		break;
	}

	Token *close = GetNextCharToken(')', L')');

	// Parse index options
	ParseKeyIndexOptions();

	// In inline constraint Oracle does not allow specifying column name again
	if(inline_cns == true && primary == true && _target == SQL_ORACLE)
		Token::Remove(open, close);

	// Check for inline non-unique index in MySQL
	if(unique == false && index == true && inline_indexes != NULL)
	{
		inline_indexes->Add(cns, GetLastToken());

		// Inline indexes will be moved, so delete comma at the end of previous column
		Token *prev = GetPrevToken(cns);

		if(Token::Compare(prev, ',', L',') == true)
			Token::Remove(prev);
	}

	// Close DISTRIBUTED BY clause for Greenplum
	if(greenplum_distributed == true)
	{
		Append(cr_table_end->book, ")", L")", 1);
		cr_table_end->distributed_by = true;
	}

	return true;
}

// Parse index options
bool SqlParser::ParseKeyIndexOptions()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Oracle USING INDEX clause, MySQL USING BTREE | HASH
		if(next->Compare("USING", L"USING", 5) == true)
		{
			Token *key_option = GetNextToken();

			// Oracle USING INDEX
			if(Token::Compare(key_option, "INDEX", L"INDEX", 5))
			{
				Token *name = GetNextToken();

				ParseCreateIndexOptions();

				// Remove USING INDEX for other databases
				if(_target != SQL_ORACLE)
					Token::Remove(next, name);
			}
			else
			// MySQL USING BTREE | HASH
			if(Token::Compare(key_option, "BTREE", L"BTREE", 5) || Token::Compare(key_option, "HASH", L"HASH", 4))
			{
				// Remove for other databases
				if(_target != SQL_MYSQL)
					Token::Remove(next, key_option);
			}

			exists = true;
			continue;
		}
		else
		// Oracle ENABLE
		if(next->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			// Remove for other databases
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// Oracle NOVALIDATE
		if(next->Compare("NOVALIDATE", L"NOVALIDATE", 10) == true)
		{
			// Remove for other databases
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// SQL Server WITH (PAD_INDEX = OFF, ...)
		if(ParseSqlServerIndexOptions(next) == true)
		{
			exists = true;
			continue;
		}

		PushBack(next);
		break;
	}

	return exists;
}

// Parse FOREIGN KEY constraint clause
bool SqlParser::ParseForeignKey(Token *foreign)
{
	if(foreign == NULL || foreign->Compare("FOREIGN", L"FOREIGN", 7) == false)
		return false;

	/*Token *key */ (void) GetNextWordToken("KEY", L"KEY", 3);
	Token *open = GetNextCharToken('(', L'(');

	// Get foreign key columns
	if(open != NULL)
	{
		// Get columns until )
		while(true)
		{			
			// Column name
			Token *next = GetNextIdentToken();

			if(next == NULL)
				break;

			Token *comma = GetNext(',', L',');

			if(comma == NULL)
				break;
		}

		/*Token *close */ (void) GetNext(')', L')');
	}

	Token *references = GetNextWordToken("REFERENCES", L"REFERENCES", 10);

	if(references != NULL)
	{
		Token *pk_table = GetNextIdentToken(SQL_IDENT_OBJECT);
		Token *open = GetNextCharToken('(', L'(');

		// Get primary/unique key columns
		if(pk_table != NULL && open != NULL)
		{
			// Get columns until )
			while(true)
			{			
				// Column name
				Token *next = GetNextIdentToken();

				if(next == NULL)
					break;

				Token *comma = GetNext(',', L',');

				if(comma == NULL)
					break;
			}

			/*Token *close */ (void) GetNext(')', L')');
		}
	}

	// Parse actions
	while(true)
	{
		Token *on = GetNextWordToken("ON", L"ON", 2);

		if(on == NULL)
			break;

		Token *action = GetNextToken();

		if(action == NULL)
			break;

		bool on_delete = false;
		bool on_update = false;

		// ON DELETE
		if(action->Compare("DELETE", L"DELETE", 6) == true)
			on_delete = true;
		else
		if(action->Compare("UPDATE", L"UPDATE", 6) == true)
			on_update = true;

		if(on_delete == true || on_update == true)
		{
			Token *what = GetNextToken();
			
			// NO ACTION, SET NULL and CASCADE
			if(what == NULL)
				break;

			// NO ACTION
			if(what->Compare("NO", L"NO", 2) == true)
			{
				Token *action = GetNextToken();

				// Oracle does not support ON DELETE NO ACTION and ON UPDATE NO ACTION clause, but this 
				// is the default behavior (do not allow to delete parent rows if child rows exist)
				if(_target == SQL_ORACLE)
					Token::Remove(on, action);
			}
			else
			// SET NULL
			if(what->Compare("SET", L"SET", 3) == true)
				/*Token *null */ (void) GetNextToken();

			continue;
		}

		PushBack(action);
		break;
	}

	// Parse foreign key options
	ParseKeyIndexOptions();

    if(_stmt_scope == SQL_STMT_ALTER_TABLE)
        ALTER_TAB_STMS_STATS("Add FOREIGN KEY")

	return true;
}

// CHECK constraint
bool SqlParser::ParseCheckConstraint(Token *check)
{
	if(check == NULL || check->Compare("CHECK", L"CHECK", 5) == false)
		return false;

	/*Token *open */ (void) GetNextCharToken('(', L'(');

	// CHECK expression
	ParseBooleanExpression(SQL_BOOL_CHECK);

	/*Token *close*/ (void) GetNextCharToken(')', L')');

	// CHECK constraint is not supported in Redshift
	if(_target == SQL_REDSHIFT)
		Comment(check, GetLastToken());

    if(_stmt_scope == SQL_STMT_ALTER_TABLE)
        ALTER_TAB_STMS_STATS("Add CHECK constraint")

	return true;
}

// Constraint option 
bool SqlParser::ParseConstraintOption()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		// Oracle ENABLE
		if(next->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			// Remove if not Oracle
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// Oracle DISABLE
		if(next->Compare("DISABLE", L"DISABLE", 7) == true)
		{
			// Remove if not Oracle
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// Oracle VALIDATE
		if(next->Compare("VALIDATE", L"VALIDATE", 8) == true)
		{
			// Remove for other databases
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// Oracle NOVALIDATE
		if(next->Compare("NOVALIDATE", L"NOVALIDATE", 10) == true)
		{
			// Remove for other databases
			if(_target != SQL_ORACLE)
				Token::Remove(next);

			exists = true;
			continue;
		}

		// Not a constraint option
		PushBack(next);
		break;
	}

	return exists;
}

// Get and parse the next token as an expression
Token* SqlParser::ParseExpression()
{
    Token *first = GetNext();

    if(ParseExpression(first))
        return first;

    PushBack(first);
    return NULL;
}

// Parse an expression
bool SqlParser::ParseExpression(Token *first, int prev_operator)
{
	if(first == NULL)
		return false;

	bool exists = false;

	Token *_not = NULL;

	// Some databases (Sybase ADS i.e.) support boolean values in expressions and their negation like SELECT NOT True
	if(Source(SQL_SYBASE_ADS) && TOKEN_CMP(first, "NOT"))
	{
		_not = first;
		first = GetNextToken();

		if(first == NULL)
			return false;

		// Logical expressions (without operand) converted to BIT in SQL Server, but NOT 1 is not allowed
		if(Target(SQL_SQL_SERVER))
			TOKEN_CHANGE(_not, "CASE WHEN");
	}

	// If expression starts with open (, parse recursively except a subquery
	if(first->Compare('(', L'(') == true)
	{
		Token *next = GetNextToken();

		// Subquery
		if(Token::Compare(next, "SELECT", L"SELECT", 6) == true)
		{
			bool select_pattern = ParseSelectExpressionPattern(first, next); 

			if(!select_pattern)
				ParseSelectStatement(next, 0, SQL_SEL_EXP, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			
			_exp_select++;
		}
		else
		{
			ParseExpression(next);

			// Propagate data type from expression to (
			first->data_type = next->data_type;			
		}

		// Next should be )
		/*Token *close*/ (void) GetNextCharToken(')', L')');

		exists = true;
	}
	else
	// Datetime literals
	if(ParseDatetimeLiteral(first) == true)
		exists = true;
    else
    // Named variable and expression: param => expr (Oracle)
    if(ParseNamedVarExpression(first) == true)
        exists = true;
	else
	// CASE expression (check CASE and IF before function as it can contain ( before first expression)
	if(ParseCaseExpression(first) == true)
		exists = true;
	else
	// IF expression (Sybase ASA)
	if(ParseIfExpression(first) == true)
		exists = true;
	else
	// Oracle built-in package
	if(ParseOraclePackage(first) == true)
		exists = true;
	else
	// Function with ()
	if(ParseFunction(first) == true)
		exists = true;
	else
	// Functional constant
	if(ParseFunctionConstant(first) == true)
		exists = true;
	else
	// Function without parentheses
	if(ParseFunctionWithoutParameters(first) == true)
		exists = true;
	else
	// Boolean literal True/False
	if(ParseBooleanLiteral(first))
		exists = true;
	else
	// This can be an identifier 
	if(first->type == TOKEN_WORD || first->type == TOKEN_IDENT)
	{
		// In COBOL EXEC SQL block all variables/parameters are prefixed with :
		if(_source_app == APP_COBOL && _level == LEVEL_SQL && _cobol != NULL)
			_cobol->ConvertDeclaredIdentifierInSql(first);
		else
		{
			// Check if a variable exists with this name
			Token *var = GetVariable(first);

			// Change only if it was changed at the declaration
			if(var != NULL && var->t_len != 0)
				ConvertVariableIdentifier(first, var);

			Token *param = NULL;

			// Check if a parameter exists with this name
			if(var == NULL)
				param = GetParameter(first);

			// Change only if it was changed at the declaration
			if(param != NULL && param->t_len != 0)
				ConvertParameterIdentifier(first, param);

			if(var == NULL && param == NULL)
				ConvertIdentifier(first, SQL_IDENT_COLUMN);
		}

		exists = true;
	}
	else
	// Other tokens that can start expression
	if(first->type == TOKEN_IDENT || first->type == TOKEN_STRING)
		exists = true;

	if(exists == false)
		return false;

	// Parse operators (they will be processed recursively by calling ParseExpression on the second operand)
	bool op_exists = false;

	// String concatenation || operator in Oracle
	if(ParseStringConcatenation(first, prev_operator) == true)
		op_exists = true;

	// String concatenation CONCAT operator in DB2
	if(ParseConcatOperator(first) == true)
		op_exists = true;

	// Addition operator + 
	if(op_exists == false && ParseAdditionOperator(first, prev_operator) == true)
		op_exists = true;

	// Subtraction operator - 
	if(op_exists == false && ParseSubtractionOperator(first) == true)
		op_exists = true;

	// Multiplication operator * 
	if(op_exists == false && ParseMultiplicationOperator(first) == true)
		op_exists = true;

	// Division operator / 
	if(op_exists == false && ParseDivisionOperator(first) == true)
		op_exists = true;

	// Percent operator % 
	if(op_exists == false && ParsePercentOperator(first) == true)
		op_exists = true;

	// Substring operator [] in Informix 
	if(op_exists == false && ParseIndexOperator(first) == true)
		op_exists = true;

	// Data type casting operator :: in Informix, PostgreSQL 
	if(op_exists == false && ParseCastOperator(first) == true)
		op_exists = true;

	// Modulo MOD operator in Teradata 
	if(op_exists == false && ParseModOperator(first) == true)
		op_exists = true;

	// UNITS num DAY operator in Informix (interval in days)
	if(op_exists == false && ParseUnitsOperator(first) == true)
		op_exists = true;

	// End of expression with negation of boolean value (without operator i.e. NOT TRUE, not NOT 1=2)
	if(_not != NULL)
	{
		if(Target(SQL_SQL_SERVER))
			APPEND_FMT(GetLastToken(), " = 1 THEN 0 ELSE 1 END", _not);
	}

	return true;
}

// Parse datetime literal
bool SqlParser::ParseDatetimeLiteral(Token *token)
{
	if(token == NULL)
		return false;

	bool exists = false;

	// TIMESTAMP '2013-01-01 11:11:10' or DATE '2016-07-11'
	if(TOKEN_CMP(token, "TIMESTAMP") == true || TOKEN_CMP(token, "DATE") == true)
	{
		Token *string = GetNextStringToken();

		if(string != NULL)
			exists = true;
	}

	return exists;
}

// Boolean literal True/False in Sybase ADS
bool SqlParser::ParseBooleanLiteral(Token *token)
{
	if(!Source(SQL_SYBASE_ADS))
		return false;

	bool exists = false;

	if(TOKEN_CMP(token, "TRUE"))
	{
		if(Target(SQL_SQL_SERVER))
			TOKEN_CHANGE(token, "1");

		exists = true;
	}
	else
	if(TOKEN_CMP(token, "FALSE"))
	{
		if(Target(SQL_SQL_SERVER))
			TOKEN_CHANGE(token, "0");

		exists = true;
	}

	return exists;
}

// Named variable and expression: param => expr (Oracle), param := expr (PostgreSQL)
bool SqlParser::ParseNamedVarExpression(Token *token)
{
	if(token == NULL)
		return false;

    Token *equal = GetNext('=', L'=');
    Token *arrow = GetNext(equal, '>', L'>');

    if(arrow == NULL)
    {
        PushBack(equal);
        return false;
    }
	else
	{
		// := in PostgreSQL
		if(Target(SQL_POSTGRESQL))
		{
			TOKEN_CHANGE(equal, ":");
			TOKEN_CHANGE(arrow, "=");
		}
	}

    ParseExpression();

	return true;
}

// Parse a boolean expression
bool SqlParser::ParseBooleanExpression(int scope, Token *stmt_start, int *conditions_count, int *rowlimit,
										Token *prev_open, bool *bool_operator_no_exists)
{
	int count = 0;

	// Typical error check boolean expressions involving = operator
	if(ParseBooleanErrorCheckPattern() == true)
		return true;

	// Typical cursor or fetch check 
	if(ParseBooleanCursorCheckPattern() == true)
		return true;

	// Boolean expression can be optionally in braces
	Token *open = GetNextCharToken('(', L'(');

	bool spec_and = false;
	bool bool_op_not_exists = false;

	if(open != NULL)
	{
		Token *select = GetNextWordToken("SELECT", L"SELECT", 6);
		Token *select_list_end = NULL;

		// Subquery
		if(select != NULL)
		{
			bool select_pattern = ParseSelectExpressionPattern(open, select); 

			if(!select_pattern)
				ParseSelectStatement(select, 0, SQL_SEL_EXP, NULL, &select_list_end, NULL, NULL, NULL, NULL, NULL, NULL);
		}
		else
		{
			// Check for specific DB2 AND syntax: (c1, c2, ...) = (v1, v2, ...)
			spec_and = ParseDb2AndBooleanExpression(open);

			// Use recursion as multiple open ( can be specified
			if(spec_and == false)
				ParseBooleanExpression(scope, stmt_start, conditions_count, rowlimit, open, &bool_op_not_exists);

			// Possible AND or OR between nested () conditions
			ParseBooleanAndOr(scope, stmt_start, &count, rowlimit);
		}

		Token *close = GetNextCharToken(')', L')');
		
		// Oracle does not allow SELECT in the IF statement
		if(select != NULL && scope == SQL_BOOL_IF && _target == SQL_ORACLE)
			OracleMoveSelectOutofIf(stmt_start, select, select_list_end, open, close);
		
		// Do not exit now as AND or OR condition can follow, or it parsed (exp) > exp2 
	}

	Token *pre_first = GetLastToken();
	Token *first = NULL;

	// First can be already parsed
	if(open == NULL)
	{
		first = GetNextToken();

		if(first == NULL)
			return false;

		// Check EXISTS () and NOT EXISTS expression
		if(first->Compare("EXISTS", L"EXISTS", 6) == true || first->Compare("NOT", L"NOT", 3) == true)
		{
			PushBack(first);
			first = NULL;
		}
		else
		{
			bool ora_join = false;

			// Check for (+) rigth join condition
			if(_source == SQL_ORACLE && scope == SQL_BOOL_WHERE)
				ora_join = ParseOracleOuterJoin(first, NULL);

			if(!ora_join)
				ParseExpression(first);
		}
	}

	Token *op = NULL;
	Token *not_ = NULL;

	bool unary = false;

	if(spec_and == false)
	{
		// Get boolean operator
		op = GetNextToken();

		// Check for ) than means subexpression, not boolean expression - (exp) > exp2 i.e.
		if(Token::Compare(op, ')', L')') == true ||
			// Expression ended
			Token::Compare(op, ';', L';') == true) 
		{
			if(bool_operator_no_exists != NULL)
				*bool_operator_no_exists = true;

			PushBack(op);
			return true;
		}

		if(Token::Compare(op, ',', L',') == true)
		{
			// List of columns in the left part of (a, b, c) IN ... Open ( is taken at higher level
			if(prev_open != NULL)
			{
				while(Token::Compare(op, ',', L',') == true)
				{
					Token *exp = GetNextToken();

					ParseExpression(exp);

					op = GetNextToken();

					if(Token::Compare(op, ')', L')') == true)
					{
						PushBack(op);
						return true;
					}
				}
			}
			// End of boolean condition
			else
			{
				PushBack(op);
				return true;
			}
		}

		// Negation operator
		if(Token::Compare(op, "NOT", L"NOT", 3) == true)
		{
			not_ = op;
			op = GetNextToken();

			// NOT can be complex, for example, NOT (SELECT c1 ...) IS NULL
			if(op != NULL && op->Compare('(', L'('))
			{
				Token *select = TOKEN_GETNEXTW("SELECT");

				// Subquery
				if(select != NULL)
					ParseSelectStatement(select, 0, SQL_SEL_EXP, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				else
					ParseBooleanExpression(scope, stmt_start, conditions_count, rowlimit);

				/*Token *close*/ (void) GetNextCharToken(')', L')');
				op = GetNextToken();
			}
			else
			{
				if(!TOKEN_CMP(op, "LIKE") && !TOKEN_CMP(op, "IN") && !TOKEN_CMP(op, "BETWEEN") && !TOKEN_CMP(op, "EXISTS") && !TOKEN_CMP(op, "MATCHES"))
				{
					ParseExpression(op);
					op = GetNextToken();
				}
			}
		}

		if(op == NULL)
			return false;
	}

	if(spec_and == true || TOKEN_CMPC(op, ')'))
		unary = true;
	else
	// Possible boolean function or expression without following operator (check for terminating word)
	if(scope == SQL_BOOL_IF && op->Compare("THEN", L"THEN", 4) == true)
	{
		PushBack(op);

		unary = true;
		count++;		
	}
	else
	// IS NULL or IS NOT NULL
	if(op->Compare("IS", L"IS", 2) == true)
	{
		Token *not_ = GetNextWordToken("NOT", L"NOT", 3);
		Token *null = GetNextWordToken("NULL", L"NULL", 4); 

		if(not_ != NULL || null != NULL)
		{
			if(_target_app == APP_JAVA)
			{
				if(not_ != NULL)
					TOKEN_CHANGE_NOFMT(op, "!=");
				else
					TOKEN_CHANGE_NOFMT(op, "==");

				TOKEN_CHANGE_NOFMT(null, "null");
				Token::Remove(not_);
			}

			unary = true;
			count++;
		}
	}
	else
	// BETWEEN a AND b
	if(op->Compare("BETWEEN", L"BETWEEN", 7) == true)
	{
		Token *exp1 = GetNextToken();

		ParseExpression(exp1);

		Token *and_ = GetNextWordToken("AND", L"AND", 3);
		
		Token *exp2 = GetNextToken();

		ParseExpression(exp2);

		if(exp1 != NULL && and_ != NULL && exp2 != NULL)
		{
			unary = true;
			count++;
		}
	}
	else
	// IN predicate
	if(op->Compare("IN", L"IN", 2) == true)
	{
		if(ParseInPredicate(op) == true)
		{
			unary = true;
			count++;
		}
	}
	else
	// EXISTS predicate
	if(op->Compare("EXISTS", L"EXISTS", 6) == true)
	{
		// Parentheses () are required for EXISTS expression
		Token *open = GetNextCharToken('(', L'(');

		if(open != NULL)
		{
			Token *next = GetNextToken();

			// Subquery
			if(Token::Compare(next, "SELECT", L"SELECT", 6) == true)
				ParseSelectStatement(next, 0, SQL_SEL_EXISTS_PREDICATE, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

			/*Token *close*/ (void) GetNextCharToken(')', L')');
		}

		unary = true;
		count++;
	}
	else
	// LIKE predicate
	if(op->Compare("LIKE", L"LIKE", 4) == true)
	{
		// Optionally enclosed in parentheses () 
		Token *open = GetNextCharToken('(', L'(');

		Token *pattern = GetNextToken();
		ParseExpression(pattern);

		/*Token *close*/ (void) GetNextCharToken(open, ')', L')');

		unary = true;
		count++;
	}
	else
	// MATCHES predicate in Informix
	if(op->Compare("MATCHES", L"MATCHES", 7) == true)
	{
		ParseInformixMatchesPredicate(pre_first, first, not_, op);

		unary = true;
		count++;
	}

	Token *op2 = NULL;

	// Check for <= <> != >= 2 symbol operators
	if(unary == false)
	{
		if(op->chr != 0 || op->wchr != 0)
		{
			op2 = GetNextCharToken('=', L'=');

			if(op2 == NULL)
				op2 = GetNextCharToken('>', L'>');
		}
		else
		// NE, EQ etc. operator in Teradata
		if(_source == SQL_TERADATA && ParseTeradataComparisonOperator(op) == true)
		{
		}
		// Unknown or no operator
		else
		{
			PushBack(op);
			unary = true;

			// Sybase ADS supports boolean (true/false) data type, so boolean expression can be without operator (i.e. it can be just AND TRUE)
			if((open == NULL || bool_op_not_exists) && Source(SQL_SYBASE_ADS))
			{
				// Boolean is converted to 1/0 in SQL Server that cannot go without boolean operator, so we create a boolean expression by adding = 1
				if(Target(SQL_SQL_SERVER))
					APPEND(GetPrevToken(op), " = 1");
			}
		}
	}
	
	// Second operand
	if(unary == false)
	{
		Token *second = GetNextToken();

		if(second == NULL)
			return false;

		bool ora_join = false;

		// Check for (+) join condition
		if(_source == SQL_ORACLE && scope == SQL_BOOL_WHERE)
			ora_join = ParseOracleOuterJoin(first, second);

		if(!ora_join)
			ParseExpression(second);

		count++;

		// Oracle rownum condition in WHERE clause removed for other databases
		if(ParseOracleRownumCondition(first, op, second, rowlimit) == true && _target != SQL_ORACLE)
			count--;
	}

	// Possible AND or OR 
	ParseBooleanAndOr(scope, stmt_start, &count, rowlimit);

	if(conditions_count != NULL)
		*conditions_count = count;

	return true;
}

// AND or OR in boolean expression
bool SqlParser::ParseBooleanAndOr(int scope, Token *stmt_start, int *conditions_count, int *rowlimit)
{
	bool exists = false;

	while(true)
	{
		// Possible AND or OR 
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		if(Token::Compare(next, "AND", L"AND", 3) == false && Token::Compare(next, "OR", L"OR", 2) == false)
		{
			PushBack(next);
			break;
		}

		// Option PRIOR before next condition in CONNECT BY
		if(scope == SQL_BOOL_CONNECT_BY)
			TOKEN_GETNEXTW("PRIOR");
		
		int count = 0;

		// Process recursively
		ParseBooleanExpression(scope, stmt_start, &count, rowlimit);

		if(conditions_count != NULL)
			*conditions_count = (*conditions_count + count);

		exists = true;
	}

	return exists;
}

// IN predicate
bool SqlParser::ParseInPredicate(Token *in)
{
	if(in == NULL)
		return false;

	Token *open = TOKEN_GETNEXT('(');

	if(open == NULL)
		return false;

	// SELECT statement or list of expressions can be specified
	Token *select = GetNextSelectStartKeyword();

	if(select != NULL)
		ParseSelectStatement(select, 0, SQL_SEL_IN_PREDICATE, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	else
	// Comma separated list of expressions
	{
		while(true)
		{
			Token *exp = GetNextToken();

			if(exp == NULL)
				break;

			ParseExpression(exp);

			Token *comma = GetNextCharToken(',', ',');

			if(comma == NULL)
				break;
		}
	}

	/*Token *close */ TOKEN_GETNEXT(')');

    // Return true if at least IN ( parsed. Other inside can be complex select and ) can be missed
	// so returning false if ) not found will mean double parsing
	return true;
}

// Parser a procedural block
bool SqlParser::ParseBlock(int type, bool frontier, int scope, int *result_sets)
{
	// Process tokens until the end of the block
	while(true)
	{
		Token *token = GetNextToken();

		if(token == NULL)
			break;

		// Entered a nested block
		if(token->Compare("BEGIN", L"BEGIN", 5) == true)
		{
			Token *next = GetNext();

			bool block = true;

			// Make sure it is not BEGIN WORK, BEGIN TRANSACTION or BEGIN;
			if(Token::Compare(next, "WORK", L"WORK", 4 )== true ||
				Token::Compare(next, "TRANSACTION", L"TRANSACTION", 11) == true || 
				Token::Compare(next, "TRAN", L"TRAN", 4) == true || 
				Token::Compare(next, ';', L';') == true)
				block = false;

			PushBack(next);

			if(block == true)
			{
                _spl_begin_blocks.Add(token);

				ParseBlock(SQL_BLOCK_BEGIN, true, scope, result_sets);

                _spl_begin_blocks.DeleteLast();

				Token *end = GetNext("END", L"END", 3);

				if(end != NULL)
				{
					Token *semi = GetNext(';', L';');

					if(semi == NULL)
					{
						// In SQL Server, Sybase ASE BEGIN END can go without ; while MySQL, MariaDB require BEGIN END;
						if(Target(SQL_MYSQL, SQL_MARIADB))
							APPEND_NOFMT(end, ";");
					}
					else
						PushBack(semi);
				}

				continue;
			}
		}
		else
		// Entered a TRY ... CATCH block in Sybase ADS
		if(_source == SQL_SYBASE_ADS && TOKEN_CMP(token, "TRY"))
		{
			ParseBlock(SQL_BLOCK_TRY, true, scope, result_sets);

			if(_target == SQL_SQL_SERVER)
				TOKEN_CHANGE(token, "BEGIN TRY");

			Token *catch_ = TOKEN_GETNEXTW("CATCH");

			if(catch_ != NULL)
			{
				Token *all = TOKEN_GETNEXTW("ALL");

				ParseBlock(SQL_BLOCK_CATCH, true, scope, result_sets);

				Token *end = TOKEN_GETNEXTW("END");

				if(_target == SQL_SQL_SERVER)
				{
					PREPEND(catch_, "END TRY\n");
					TOKEN_CHANGE(catch_, "BEGIN CATCH");
					TOKEN_CHANGE(end, "END CATCH");
					Token::Remove(all);
				}
			}

			continue;			
		}

		// If frontier was set, check the closing sequence for the block
		if(frontier == true)
		{
			// END keyword
			if(token->Compare("END", L"END", 3) == true)
			{
				PushBack(token);
				break;
			}
			else
			// For procedural block check for next CREATE PROCEDURE, FUNCTION, TRIGGER and
			// exit to avoid stack overflow in case of parser issues and deep nested calls
			if(type == SQL_BLOCK_PROC && token->Compare("CREATE", L"CREATE", 6) == true)
			{
				Token *next = GetNextToken();

				if(Token::Compare(next, "PROCEDURE", L"PROCEDURE", 9) || Token::Compare(next, "FUNCTION", L"FUNCTION", 8) ||
					Token::Compare(next, "TRIGGER", L"TRIGGER", 7))
				{
					PushBack(token);
					break;
				}

				PushBack(next);
			}
			else
			// IF block can be terminated with ELSEIF, ELSIF, ELSE or ENDIF keyword
			if(type == SQL_BLOCK_IF && (token->Compare("ELSEIF", L"ELSEIF", 6) || 
				token->Compare("ELSIF", L"ELSIF", 5) || token->Compare("ELSE", L"ELSE", 4) || token->Compare("ENDIF", L"ENDIF", 5)))
			{
				PushBack(token);
				break;
			}
			else
			// CASE block can be terminated with WHEN or ELSE keyword
			if(type == SQL_BLOCK_CASE && (token->Compare("WHEN", L"WHEN", 4) || token->Compare("ELSE", L"ELSE", 4)))
			{
				PushBack(token);
				break;
			}
			else
			// REPEAT block terminated with UNTIL
			if(type == SQL_BLOCK_REPEAT && token->Compare("UNTIL", L"UNTIL", 5) == true)
			{
				PushBack(token);
				break;
			}
			else 
			// Oracle EXCEPTION block terminated by WHEN for next condition
			if(type == SQL_BLOCK_EXCEPTION  && token->Compare("WHEN", L"WHEN", 4) == true)
			{
				PushBack(token);
				break;
			}
			else
			// In SQL Server, Sybase procedure (not function) body can be terminated by GO without BEGIN-END block
			if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && token->Compare("GO", L"GO", 2) == true)
			{
				PushBack(token);
				break;
			}
			else 
			// Sybase ADS TRY ... CATCH
			if((type == SQL_BLOCK_TRY  && TOKEN_CMP(token, "CATCH")) || (type == SQL_BLOCK_CATCH  && TOKEN_CMP(token, "END")))
			{
				PushBack(token);
				break;
			}

			// Parse high-level token
			Parse(token, scope, result_sets);
			continue;
		}
		else
		{
			// In SQL Server, Sybase procedure (not function) body can be terminated by GO without BEGIN-END block
			if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && token->Compare("GO", L"GO", 2) == true)
			{
				PushBack(token);
				break;
			}

			// Parse the first statement and exit
			Parse(token, scope, result_sets);
			break;
		}
	}

	return true;
}

// Oracle PL/SQL assignment statement variable := exp, in PostgreSQL var = exp can be used
bool SqlParser::ParseAssignmentStatement(Token *variable)
{
	if(variable == NULL)
		return false;

	// Must be a word or identifier
	if(variable->type != TOKEN_WORD && variable->type != TOKEN_IDENT)
		return false;

	Token *colon = GetNextCharToken(':', L':');
	Token *equal = GetNextCharToken('=', L'=');

	if(equal == NULL)
		return false;

	// For PostgreSQL assignment can be without :
	if(colon == NULL)
	{
		Token *prev = GetPrevToken(variable);

		if(prev == NULL)
			return false;

		// Previous word must be ; or keyword that starts a block
		if(prev->Compare(';', L';') == false && prev->Compare("BEGIN", L"BEGIN", 5) == false && 
			prev->Compare("THEN", L"THEN", 4) == false && prev->Compare("ELSE", L"ELSE", 4) == false &&
			!TOKEN_CMP(prev, "DO"))
			return false;
	}

	// Try to find variable declaration
	Token *decl_var = GetVariableOrParameter(variable);

	if(_target_app == APP_JAVA)
		Token::Remove(colon);
	else
	// SET @var = value in SQL Server, SET var = value in MySQL
	if(Target(SQL_SQL_SERVER, SQL_MYSQL, SQL_SYBASE) == true)
	{
		Prepend(variable, "SET ", L"SET ", 4, _declare_format);
		
		// Add @ before the name
		if(Target(SQL_SQL_SERVER, SQL_SYBASE) == true)
			ConvertToTsqlVariable(variable);

		Token::Remove(colon);
	}
	else
	// For Oracle add : if not set
	if(_target == SQL_ORACLE && colon == NULL)
		Prepend(equal, ":", L":", 1);

	// Sybase ADS supports assignment of boolean expression to logical value i.e a = 1=2;
	if(Source(SQL_SYBASE_ADS) && decl_var != NULL && decl_var->data_subtype == TOKEN_DT2_BOOL)
	{
		bool bool_op_not_exists = false;

		ParseBooleanExpression(SQL_BOOL_ASSIGN, NULL, NULL, NULL, NULL, &bool_op_not_exists);

		// SQL Server does not allow boolean expression if assignment (= <> etc.) but simple assignment a = true is fine (converted to a = 1 using BIT data type)
		if(!bool_op_not_exists && Target(SQL_SQL_SERVER))
		{
			APPEND(equal, " CASE WHEN ");
			APPEND(GetLastToken(), " THEN 1 ELSE 0 END");
		}

		return true;
	}

	Token *next = GetNextToken();

	// Assignment expression
	ParseExpression(next);

	return true;
}

// String concatenation operator || in Oracle
bool SqlParser::ParseStringConcatenation(Token *first, int prev_operator)
{
	if(first == NULL)
		return false;
	
	Token *first_end = GetLastToken();

	Token *bar = GetNextCharToken('|', L'|');

	if(bar == NULL)
		return false;

	Token *bar2 = GetNextCharToken('|', L'|');

	if(bar2 == NULL)
		return false;

	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second, SQL_OPERATOR_CONCAT);

	// Change to + in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(bar, "+", L"+", 1);
		Token::Remove(bar2);

		// If the first expression is nullable, around with ISNULL
		if(first->nullable == true)
		{
			Prepend(first, "ISNULL(", L"ISNULL(", 7);
			Append(first_end, ", '')", L", '')", 5);

			// Set not null to first token to avoid nested ISNULL when multiple concatenations specified
			first->nullable = false;
		}

		// If the second expression is nullable, around with ISNULL
		if(second->nullable == true)
		{
			Prepend(second, "ISNULL(", L"ISNULL(", 7);
			Append(GetLastToken(), ", '')", L", '')", 5);
		}
	}
	else
	// Change to CONCAT function in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		// If it is first expression add CONCAT( before 
		if(prev_operator != SQL_OPERATOR_CONCAT)
			Prepend(first, "CONCAT(", L"CONCAT(", 7);

		Token::Change(bar, ",", L",", 1);
		Token::Remove(bar2);

		// If the first expression is nullable, around with IFNULL
		if(first->nullable == true)
		{
			Prepend(first, "IFNULL(", L"IFNULL(", 7);
			Append(first_end, ", '')", L", '')", 5);

			// Set not null to first token to avoid nested IFNULL when multiple concatenations specified
			first->nullable = false;
		}

		// If the second expression is nullable, around with IFNULL
		if(second->nullable == true)
		{
			Prepend(second, "IFNULL(", L"IFNULL(", 7);
			Append(GetLastToken(), ", '')", L", '')", 5);
		}

		// If it is first expression add ) to close CONCAT function
		if(prev_operator != SQL_OPERATOR_CONCAT)
			Append(GetLastToken(), ")", L")", 1);
	}

	return true;
}

// String concatenation CONCAT operator in DB2
bool SqlParser::ParseConcatOperator(Token *first)
{
	if(first == NULL)
		return false;
	
	// CONCAT keyword
	Token *concat = GetNextWordToken("CONCAT", L"CONCAT", 6);

	if(concat == NULL)
		return false;

	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second, SQL_OPERATOR_CONCAT);

	// Change to + operator in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(concat, "+", L"+", 1);
	else
	// Change to || operator in Oracle 
	if(_target == SQL_ORACLE)
		Token::Change(concat, "||", L"||", 2);

	return true;
}

// Multiplication operator *
bool SqlParser::ParseMultiplicationOperator(Token *first)
{
	if(first == NULL)
		return false;
	
	Token *asterisk = GetNextCharToken('*', L'*');

	if(asterisk == NULL)
		return false;

	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second);

	return true;
}

// Addition operator +
bool SqlParser::ParseAdditionOperator(Token *first, int prev_operator)
{
	if(first == NULL)
		return false;
	
	Token *first_end = GetLastToken();

	Token *plus = GetNextPlusMinusAsOperatorToken('+', L'+');

	if(plus == NULL)
		return false;

	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second, SQL_OPERATOR_PLUS);

	Token *second_end = GetLastToken();

	// In SQL Server, Sybase + is also used to concatenate strings
	if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true)
	{
		// Number always has priority in SQL Server: both '5'+ 5 and 5 + '5' give result 10, but
		// if at least one string literal contains not a number ',' i.e. we can consider + as
		// string concatenation

		bool not_num1 = false;
		bool not_num2 = false;

		bool not_num_in_string1 = false;
		bool not_num_in_string2 = false;

		if(first->type == TOKEN_STRING && first->IsNumericInString() == false) 
		{
			not_num_in_string1 = true;
			not_num1 = true;
		}

		if(first->data_type == TOKEN_DT_STRING)
			not_num1 = true;

		if(second->type == TOKEN_STRING && second->IsNumericInString() == false) 
		{
			not_num_in_string2 = true;
			not_num2 = true;
		}

		if(second->data_type == TOKEN_DT_STRING)
			not_num2 = true;

		bool string_concat = false;

		if(not_num_in_string1 == true || not_num_in_string2 == true)
			string_concat = true;

		if(not_num1 == true && not_num2 == true)
			string_concat = true;

		// String concatentation
		if(string_concat == true)
		{
			// Use || in Oracle and PostgreSQL
			if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
			{
				Token::Change(plus, "||", L"||", 2);
			}
			else
			// CONCAT function in MySQL
			if(Target(SQL_MARIADB, SQL_MYSQL))
			{
				// If it is first expression add CONCAT( before, and ) after last expression 
				if(prev_operator != SQL_OPERATOR_PLUS)
					Prepend(first, "CONCAT(", L"CONCAT(", 7);

				// Sybase ASE treats NULLs in + operator as empty string (unlike SQL Server)
				// MySQL, MariaDB CONCAT gives NULL if any expression is NULL
				if(_source == SQL_SYBASE)
				{
					// Enclose first expression with IFNULL if it allows NULLs
					if(first->type != TOKEN_STRING && first->nullable)
					{
						PREPEND(first, "IFNULL(");
						APPEND(first_end, ", '')");
					}

					// Enclose second expression with IFNULL if it allows NULLs
					if(second->type != TOKEN_STRING && second->nullable)
					{
						PREPEND(second, "IFNULL(");
						APPEND(second_end, ", '')");
					}
				}

				if(prev_operator != SQL_OPERATOR_PLUS)
					Append(GetLastToken(), ")", L")", 1);

				Token::Change(plus, ",", L",", 1);
			}
		}
	}
	else
	// Check for DB2 interval +/- expressions
	if(_source == SQL_DB2 && second->data_type == TOKEN_DT_INTERVAL)
	{
		if(_target == SQL_SQL_SERVER)
			SqlServerToDateAdd(plus, first, first_end, second, second_end);

		// Check for a +/- chain of interval expressions: for example, + expr MONTH - expr DAYS + ...
		ParseAddSubIntervalChain(first, SQL_OPERATOR_PLUS);
	}
	else
	// Sybase ADS adding number of days to date
	if(_source == SQL_SYBASE_ADS && first->data_type == TOKEN_DT_DATETIME)
	{
		if(_target == SQL_SQL_SERVER)
			SqlServerToDateAdd(plus, first, first_end, second, second_end);
	}

	return true;
}

// Check for a +/- chain of interval expressions: for example, + expr MONTH - expr DAYS + ...
bool SqlParser::ParseAddSubIntervalChain(Token *first, int prev_operator)
{
	bool exists = false;

	// Get + or -
	Token *plus = GetNextPlusMinusAsOperatorToken('+', L'+');
	Token *minus = NULL;

	if(plus == NULL)
		minus = GetNextPlusMinusAsOperatorToken('-', L'-');

	if(plus != NULL)
	{
		PushBack(plus);
		ParseAdditionOperator(first, prev_operator);
		exists = true;
	}
	else
	if(minus != NULL)
	{
		PushBack(minus);
		ParseSubtractionOperator(first);
		exists = true;
	}

	return exists;
}

// Division operator /
bool SqlParser::ParseDivisionOperator(Token *first)
{
	if(first == NULL)
		return false;
	
	/*Token *first_end */ (void) GetLastToken();

	Token *div = GetNextCharToken('/', L'/');

	if(div == NULL)
		return false;

	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second);

	return true;
}

// Parse % operator or attribute 
bool SqlParser::ParsePercentOperator(Token *first)
{
	if(first == NULL)
		return false;
	
	Token *cent = GetNextCharToken('%', L'%');

	if(cent == NULL)
		return false;

	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	// cursor%NOTFOUND in Oracle
	if(TOKEN_CMP(second, "NOTFOUND") == true)
	{
		if(_target_app == APP_JAVA)
		{
			PREPEND_NOFMT(first, "!");
			APPEND_NOFMT(first, "_found");
			Token::Remove(cent, second);
		}
		else
		// NOT_FOUND = 1 in MariaDB and MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			TOKEN_CHANGE(second, "NOT_FOUND = 1");
			Token::Remove(first, cent);
		}
        else
        // @@FETCH_STATUS = 0 in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			TOKEN_CHANGE(second, "@@FETCH_STATUS = 0");
			Token::Remove(first, cent);
		}
	}
    else
    // cursor%FOUND in Oracle
	if(TOKEN_CMP(second, "FOUND") == true)
	{
		if(_target_app == APP_JAVA)
		{
			APPEND_NOFMT(first, "_found");
			Token::Remove(cent, second);
		}
		else
		// NOT_FOUND = 0 in MariaDB and MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			TOKEN_CHANGE(second, "NOT_FOUND = 0");
			Token::Remove(first, cent);
		}
	}

	return true;
}

// Parse substring [] operator in Informix 
bool SqlParser::ParseIndexOperator(Token *first)
{
	if(first == NULL)
		return false;
	
	Token *open = GetNextCharToken('[', L'[');

	if(open == NULL)
		return false;

	// Start position
	Token *start = GetNextToken();

	if(start == NULL)
		return false;

	// Optional comma and end position
	Token *comma = GetNextCharToken(',', L',');

	// End position (inclusive in Informix)
	Token *end = NULL;

	if(comma != NULL)
	{
		end = GetNextToken();

		if(end == NULL)
			return false;
	}

	Token *close = GetNextCharToken(']', L']');

	if(close == NULL)
		return false;

	// Get start and end as integer values
	int s = start->GetInt();
	int e = 0;
	int len = 1;
	
	if(end != NULL)
	{
		e = end->GetInt();

		// Define the substring length
		len = e - s + 1;
	}

	// SUBSTR in Oracle, SUBSTRING in SQL Server
	if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
	{
		if(_target == SQL_ORACLE)
			Prepend(first, "SUBSTR(", L"SUBSTR(", 7);
		else
		if(_target == SQL_SQL_SERVER)
			Prepend(first, "SUBSTRING(", L"SUBSTRING(", 10);
		
		Token::Change(open, ",", L",", 1);

		if(end != NULL)
			Token::Change(end, len);
		else
			AppendNoFormat(start, ", 1", L", 1", 3);

		Token::Change(close, ")", L")", 1);
	}

	return true;
}

// Data type casting operator :: in Informix, PostgreSQL 
bool SqlParser::ParseCastOperator(Token *first)
{
	if(first == NULL)
		return false;

	Token *colon1 = GetNext(':', L':');
	Token *colon2 = GetNext(colon1, ':', L':');

	if(colon1 == NULL || colon2 == NULL)
		return false;

	Token *datatype = GetNext();

	ParseDataType(datatype, SQL_SCOPE_CAST_FUNC);

	Token *end = GetLastToken();

	// Use CAST(exp AS datatype) in Oracle, SQL Server
	if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
	{
		Prepend(first, "CAST(", L"CAST(", 5, datatype);

		Token::Change(colon1, " AS ", L" AS ", 4, datatype);
		Token::Remove(colon2);

		AppendNoFormat(end, ")", L")", 1);
	}

	return true;
}

// Parse modulo (remainder) MOD operator in Teradata 
bool SqlParser::ParseModOperator(Token *first)
{
	if(first == NULL)
		return false;

	Token *first_end = GetLastToken();
	
	Token *mod = GetNext("MOD", L"MOD", 3);

	if(mod == NULL)
		return false;

	// Second expression
	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	ParseExpression(second);

	Token *second_end = GetLastToken();

	// MOD function in Oracle
	if(Target(SQL_ORACLE) == true)
	{
		Prepend(first, "MOD(", L"MOD(", 4, mod);
		AppendNoFormat(first_end, ",", L",", 1);
		AppendNoFormat(second_end, ")", L")", 1);

		Token::Remove(mod);
	}

	return true;
}

// num UNITS DAY | SECOND etc. operator in Informix (interval in days, seconds etc.)
// num DAY | SECOND etc. in DB2
bool SqlParser::ParseUnitsOperator(Token *first)
{
	if(first == NULL || Source(SQL_DB2, SQL_INFORMIX) == false)
		return false;

	Token *first_end = GetLastToken();

	// UNITS can be used with a number constant and expression (!) in Informix
	Token *units = GetNext("UNITS", L"UNITS", 5);

	if(_source == SQL_INFORMIX && units == NULL)
		return false;

	Token *month = GetNext("MONTH", L"MONTH", 5);
	Token *months = NULL;

	// MONTHS in DB2
	if(month == NULL)
		months = GetNext("MONTHS", L"MONTHS", 6);

	Token *field = Nvl(month, months);

	Token *day = NULL;
	Token *days = NULL;

	Token *minute = NULL;
	Token *minutes = NULL;
	
	Token *second = NULL;

	if(field == NULL)
	{
		day = GetNext("DAY", L"DAY", 3);

		if(day == NULL)
			days = GetNext("DAYS", L"DAYS", 4);

		field = Nvl(day, days);
	}

	if(field == NULL)
	{
		minute = GetNext("MINUTE", L"MINUTE", 6);

		if(minute == NULL)
			minutes = GetNext("MINUTES", L"MINUTES", 7);

		field = Nvl(minute, minutes);
	}

	if(field == NULL)
	{
		second = GetNextWordToken("SECOND", L"SECOND", 6);

		field = second;
	}

	// Not an interval field
	if(field == NULL)
		return false;

	bool first_num = first->IsNumeric();

	// INTERVAL 'num' DAY, or NUMTODSINTERVAL(var, 'DAY') in Oracle
	if(_target == SQL_ORACLE)
	{
		// Number constant
		if(first_num == true)
		{
			Prepend(first, "INTERVAL '", L"INTERVAL '", 10, field);
			AppendNoFormat(first, "'", L"'", 1);
		}
		// Variable or expression
		else
		{
			Prepend(first, "NUMTODSINTERVAL(", L"NUMTODSINTERVAL(", 16, field);
			
			AppendNoFormat(first_end, ",", L",", 1);
			PrependNoFormat(field, "'", L"'", 1);
			AppendNoFormat(field, "')", L"')", 2);
		}

		// Oracle allow only singular form DAY, not DAYS i.e
		if(days != NULL)
			Token::Change(days, "DAY", L"DAY", 3);

		if(minutes != NULL)
			Token::Change(minutes, "MINUTE", L"MINUTE", 6);
		
		Token::Remove(units);
	}
	else
	// For SQL Server, remove field keyword as it will replaced by DATEADD for +/- expressions
	if(_target == SQL_SQL_SERVER)
		Token::Remove(field);
	else
	// INTERVAL 'num' units, or var * INTERVAL '1' units in PostgreSQL
	if(_target == SQL_POSTGRESQL)
	{
		// Number constant
		if(first_num == true)
		{
			Prepend(first, "INTERVAL '", L"INTERVAL '", 10, field);
			AppendNoFormat(first, "'", L"'", 1);

			Token::Remove(units);
		}
		// Variable or expression
		else
			Token::Change(units, "* INTERVAL '1'", L"* INTERVAL '1'", 14);
	}

	first->data_type = TOKEN_DT_INTERVAL;

	if(day != NULL || days != NULL)
		first->data_subtype = TOKEN_DT2_INTVL_DAY;
	else
	if(month != NULL || months != NULL)
		first->data_subtype = TOKEN_DT2_INTVL_MON;
	else
	if(minute != NULL || minutes != NULL)
		first->data_subtype = TOKEN_DT2_INTVL_MIN;
	else
	if(second != NULL)
		first->data_subtype = TOKEN_DT2_INTVL_SEC;

	return true;
}

// Subtraction operator -
bool SqlParser::ParseSubtractionOperator(Token *first)
{
	if(first == NULL)
		return false;
	
	Token *first_end = GetLastToken();

	Token *minus = GetNextPlusMinusAsOperatorToken('-', L'-');

	if(minus == NULL)
		return false;

	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second);

	Token *second_end = GetLastToken();

	// Source is Oracle and operands are dates
	if(_source == SQL_ORACLE && first->data_type == TOKEN_DT_DATETIME && second->data_type == TOKEN_DT_DATETIME)
	{
		// Convert to FLOAT and subtract values
		if(_target == SQL_SQL_SERVER)
		{
			Prepend(first, "CONVERT(FLOAT, ", L"CONVERT(FLOAT, ", 15);
			Prepend(second, "CONVERT(FLOAT, ", L"CONVERT(FLOAT, ", 15);

			Append(first_end, ")", L")", 1);
			Append(second_end, ")", L")", 1);
		}
	}
	else
	// Check for DB2 interval +/- expressions
	if(_source == SQL_DB2 && second->data_type == TOKEN_DT_INTERVAL)
	{
		if(_target == SQL_SQL_SERVER)
			SqlServerToDateAdd(minus, first, first_end, second, second_end);

		// Check for a +/- chain of interval expressions: for example, + expr MONTH - expr DAYS + ...
		ParseAddSubIntervalChain(first, SQL_OPERATOR_PLUS);
	}

	return true;
}

// DB2, Teradata label declaration
bool SqlParser::ParseLabelDeclaration(Token *word, bool outer_sp_block)
{
	// Make sure it is DB2, Teradata, MySQL to avoid confusion with := assignment in Oracle, PostgreSQL i.e.
	if(Source(SQL_DB2, SQL_TERADATA, SQL_MYSQL) == false)
		return false;

	Token *name = (word != NULL) ? word : GetNextIdentToken();

	if(name == NULL)
		return false;

	// In DB2 label is specified as label: or label :
	Token *semi = GetNextCharToken(':', L':');

	bool exists = false;

	// Label
	if(semi != NULL || (name->len > 1 && name->Compare(':', L':', name->len-1) == true))
		exists = true;

	// Not a label
	if(exists == false)
	{
		// Name was selected, it is not an input parameter
		if(word == NULL)
			PushBack(name);

		return false;
	}

	// Outer label in procedure block in DB2
	if(outer_sp_block == true)
	{
		_spl_outer_label = name;

		// In Oracle, outer label is allowed only in anonymous blocks, remove it; also remove for other databases
		if(Target(SQL_DB2, SQL_TERADATA) == false)
		{
			Token::Remove(name);
			Token::Remove(semi);
		}
	}
	else
	{
		// <<label>> in Oracle
		if(_target == SQL_ORACLE)
		{
			PrependNoFormat(name, "<<", L"<<", 2);

			// label: without space between name and : is represented as single token
			if(semi == NULL)
				Token::ChangeNoFormat(name, name, 0, name->len-1);
			else
				Token::Remove(semi);
			
			AppendNoFormat(name, ">>", L">>", 2);
		}
	}

	return exists;
}

// CASE as expression
bool SqlParser::ParseCaseExpression(Token *first)
{
	if(first == NULL || Token::Compare(first, "CASE", L"CASE", 4) == false)
		return false;

	return ParseCaseStatement(first, false);
}

// IF expression in Sybase ASA, IF function in MySQL
bool SqlParser::ParseIfExpression(Token *first)
{
	if(first == NULL || Token::Compare(first, "IF", L"IF", 2) == false)
		return false;

	// ( in MySQL, do not take optional ( before boolean expression in Sybase ASA
	Token *open = NULL;
	
	if(_source == SQL_MYSQL)
		open = GetNext('(', L'(');

	ParseBooleanExpression(SQL_BOOL_IF_EXP);

	Token *then = GetNext("THEN", L"THEN", 4);
	Token *comma_then = NULL;

	// Comma in MySQL
	if(then == NULL)
		comma_then = GetNext(',', L',');

	if(then == NULL && comma_then == NULL)
		return false;

	// TRUE expression
	Token *true_exp = GetNextToken();

	if(true_exp == NULL)
		return false;

	ParseExpression(true_exp);

	// Optional ELSE expression
	Token *else_ = GetNextWordToken("ELSE", L"ELSE", 4);
	Token *comma_else = NULL;

	// ELSE expression is mandatory in IF function in MySQL
	if(else_ == NULL)
		comma_else = GetNext(',', L',');

	if(else_ != NULL || comma_else != NULL)
	{
		// FALSE expression
		Token *false_exp = GetNextToken();

		ParseExpression(false_exp);
	}

	// ENDIF in  Sybase ASA
	Token *endif = GetNextWordToken("ENDIF", L"ENDIF", 5);

	// ) in MySQL
	Token *close = GetNext(open, ')', L')');

	// CASE WHEN expression for other databases
	if(_target != SQL_SYBASE_ASA)
	{
		Token::Change(first, "CASE WHEN", L"CASE WHEN", 9);

		if(endif != NULL)
			Token::Change(endif, "END", L"END", 3);
		else
			Append(close, " END", L" END", 4, first);

		Token::Change(comma_then, " THEN ", L" THEN ", 6, first);
		Token::Change(comma_else, " ELSE ", L" ELSE ", 6, first);

		Token::Change(open, " ", L" ", 1);
		Token::Remove(close);
	}

	return true;
}

// Analytic function OVER (PARTITION part, ... ORDER BY c1 ASC, ...) clause
bool SqlParser::ParseAnalyticFunctionOverClause(Token *over)
{
	if(over == NULL)
		return false;

	Token *open = GetNextCharToken('(', L'(');

	if(open == NULL)
		return false;

	// Optional PARTITION clause
	Token *partition = GetNextWordToken("PARTITION", L"PARTITION", 9);
	/*Token *by */ (void) GetNextWordToken(partition, "BY", L"BY", 2);

	if(partition != NULL)
	{
		// Comma separated list of columns
		while(true)
		{
			Token *col = GetNextToken();

			if(col == NULL)
				break;

			Token *comma = GetNextCharToken(',', L',');

			if(comma == NULL)
				break;
		}
	}

	// ORDER BY clause
	ParseSelectOrderBy(NULL);

	/*Token *close*/ (void) GetNextCharToken(')', L')');

	return true;
}
