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

// SQLParser for datatypes

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Parse data type definition
bool SqlParser::ParseDataType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    if(ParseAutoIncType(name) == true)
        return true;

    if(ParseBfileType(name) == true)
        return true;

    if(ParseBigdatetimeType(name) == true)
        return true;

    if(ParseBigintType(name, clause_scope) == true)
        return true;

    if(ParseBigserialType(name) == true)
        return true;

    if(ParseBigtimeType(name) == true)
        return true;

    if(ParseBinaryType(name) == true)
        return true;

    if(ParseBinaryDoubleType(name) == true)
        return true;

    if(ParseBinaryFloatType(name) == true)
        return true;

    if(ParseBitType(name) == true)
        return true;

    if(ParseBlobType(name) == true)
        return true;

    if(ParseBooleanType(name) == true)
        return true;

    if(ParseByteaType(name) == true)
        return true;

    if(ParseByteType(name) == true)
        return true;

    if(ParseByteintType(name) == true)
        return true;

    if(ParseCharacterType(name, clause_scope) == true)
        return true;

    if(ParseCharType(name, clause_scope) == true)
        return true;

    if(ParseClobType(name) == true)
        return true;

    if(ParseCurDoubleType(name) == true)
        return true;

    if(ParseDatetimeType(name) == true)
        return true;

    if(ParseDatetime2Type(name) == true)
        return true;

    if(ParseDatetimeoffsetType(name) == true)
        return true;

    if(ParseDateType(name) == true)
        return true;

    if(ParseDbclobType(name) == true)
        return true;

    if(ParseDecfloatType(name) == true)
        return true;

    if(ParseDecimalType(name, clause_scope) == true)
        return true;

    if(ParseDoubleType(name) == true)
        return true;

    if(ParseFixedType(name) == true)
        return true;

    if(ParseFloatType(name) == true)
        return true;

    if(ParseFloat4Type(name) == true)
        return true;

    if(ParseFloat8Type(name) == true)
        return true;

    if(ParseGraphicType(name) == true)
        return true;

    if(ParseImageType(name) == true)
        return true;

    if(ParseIntervalType(name) == true)
        return true;

    if(ParseIntType(name, clause_scope) == true)
        return true;

    if(ParseInt1Type(name) == true)
        return true;

    if(ParseInt2Type(name) == true)
        return true;

    if(ParseInt3Type(name) == true)
        return true;

    if(ParseInt4Type(name) == true)
        return true;

    if(ParseInt8Type(name) == true)
        return true;

    if(ParseLogicalType(name) == true)
        return true;

    if(ParseLongblobType(name) == true)
        return true;

    if(ParseLongType(name) == true)
        return true;

    if(ParseLongtextType(name) == true)
        return true;

    if(ParseLvarcharType(name) == true)
        return true;

    if(ParseMediumblobType(name) == true)
        return true;

    if(ParseMediumintType(name) == true)
        return true;

    if(ParseMediumtextType(name) == true)
        return true;

    if(ParseMemoType(name) == true)
        return true;

    if(ParseMiddleintType(name) == true)
        return true;

    if(ParseMoneyType(name) == true)
        return true;

    if(ParseNationalType(name) == true)
        return true;

    if(ParseNcharType(name) == true)
        return true;

    if(ParseNclobType(name) == true)
        return true;

    if(ParseNmemoType(name) == true)
        return true;

    if(ParseNtextType(name) == true)
        return true;

    if(ParseNumberType(name, clause_scope) == true)
        return true;

    if(ParseNumericType(name) == true)
        return true;

    if(ParseNvarcharType(name) == true)
        return true;

    if(ParseNvarchar2Type(name, clause_scope) == true)
        return true;

    if(ParseRawType(name) == true)
        return true;

    if(ParseRealType(name) == true)
        return true;

    if(ParseRefcursor(name) == true)
        return true;

    if(ParseRowidType(name) == true)
        return true;

    if(ParseRowversionType(name) == true)
        return true;

    if(ParseSerialType(name) == true)
        return true;

    if(ParseSerial2Type(name) == true)
        return true;

    if(ParseSerial4Type(name) == true)
        return true;

    if(ParseSerial8Type(name) == true)
        return true;

    if(ParseShortType(name) == true)
        return true;

    if(ParseSmalldatetimeType(name) == true)
        return true;

    if(ParseSmallfloatType(name) == true)
        return true;

    if(ParseSmallintType(name, clause_scope) == true)
        return true;

    if(ParseSmallmoneyType(name) == true)
        return true;

    if(ParseSmallserialType(name) == true)
        return true;

    if(ParseStringType(name) == true)
        return true;

    if(ParseTextType(name) == true)
        return true;

    if(ParseTimeType(name) == true)
        return true;

    if(ParseTimetzType(name) == true)
        return true;

    if(ParseTimestampType(name) == true)
        return true;

    if(ParseTimestamptzType(name) == true)
        return true;

    if(ParseTinyblobType(name) == true)
        return true;

    if(ParseTinyintType(name, clause_scope) == true)
        return true;

    if(ParseTinytextType(name) == true)
        return true;

    if(ParseUnicodeType(name) == true)
        return true;

    if(ParseUnicharType(name) == true)
        return true;

    if(ParseUniqueidentifierType(name) == true)
        return true;

    if(ParseUniqueidentifierstrType(name) == true)
        return true;

    if(ParseUnitextType(name) == true)
        return true;

    if(ParseUnivarcharType(name) == true)
        return true;

    if(ParseUnsignedType(name) == true)
        return true;

    if(ParseUrowidType(name) == true)
        return true;

    if(ParseUuidType(name) == true)
        return true;

    if(ParseVarbinaryType(name) == true)
        return true;

    if(ParseVarbitType(name) == true)
        return true;

    if(ParseVarcharType(name, clause_scope) == true)
        return true;

    if(ParseVarchar2Type(name, clause_scope) == true)
        return true;

    if(ParseVargraphicType(name) == true)
        return true;

    if(ParseXmlType(name) == true)
        return true;

    if(ParseXmltypeType(name) == true)
        return true;

    if(ParseYearType(name) == true)
        return true;

    // Parse a user-defined data type
    ParseUdt(name, clause_scope);

    return false;
}

// Oracle, PostgreSQL, Netezza %TYPE and %ROWTYPE variable; Informix LIKE
bool SqlParser::ParseTypedVariable(Token *var, Token *ref_type)
{
    if(var == NULL || ref_type == NULL)
        return false;

	STATS_DECL
	STATS_DTL_DECL

    // %TYPE and %ROWTYPE in Oracle
    Token *cent = GetNextCharToken('%', L'%');

    Token *type = NULL; 
    Token *rowtype = NULL;

    Token *like = NULL;

    // LIKE table.column in Informix
    if(_source == SQL_INFORMIX && ref_type->Compare("LIKE", L"LIKE", 4) == true)
        like = ref_type;

    if(cent == NULL && like == NULL)
        return false;

    if(cent != NULL)
    {
        type = GetNextWordToken("TYPE", L"TYPE", 4); 

        if(type == NULL)
            rowtype = GetNextWordToken("ROWTYPE", L"ROWTYPE", 7); 

        if(type == NULL && rowtype == NULL)
            return false;

		if(type != NULL)
		{
			STATS_SET_DESC(SQL_USER_DATATYPE_TYPE_DESC)
			STATS_SET_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA, SQL_POSTGRESQL, SQL_NETEZZA))

			UDTYPE_STATS_V("%TYPE", var)
		}
		else
		{
			STATS_SET_DESC(SQL_USER_DATATYPE_ROWTYPE_DESC)
			STATS_SET_CONV_NO_NEED(Target(SQL_ORACLE, SQL_MARIADB_ORA, SQL_POSTGRESQL, SQL_NETEZZA))

			UDTYPE_STATS_V("%ROWTYPE", var)
		}
    }

    UDTYPE_DTL_STATS_L(ref_type)

	if(_target_app == APP_JAVA)
	{
		// %TYPE
		if(type != NULL)
		{
			// Read the data type from available meta information
			const char *meta_type = GetMetaType(ref_type);

			if(meta_type != NULL)
			{
			   TokenStr java_type(_java->MapSqlDataType(meta_type));
			   Token::ChangeNoFormat(ref_type, java_type);
			}
			else
				TOKEN_CHANGE_NOFMT(ref_type, "Object /* Use -meta option */");
		}
		else
			// Save variable
			_spl_rowtype_vars.Add(var);

		Token::Remove(cent, Nvl(type, rowtype));
	}
	else
    // No conversion required
    if(Source(SQL_ORACLE, SQL_MARIADB_ORA, SQL_POSTGRESQL, SQL_NETEZZA) && 
		Target(SQL_ORACLE, SQL_MARIADB_ORA, SQL_POSTGRESQL, SQL_NETEZZA))
        return true;
	else
    // Convert %TYPE
    if(type != NULL)
    {
        // Read the data type from available meta information
        const char *meta_type = GetMetaType(ref_type);

        if(meta_type != NULL)
        {
            Token::Change(ref_type, meta_type, NULL, strlen(meta_type), type);
            Token::Remove(cent, type);
            return true;
        }

        // Guess functions
        TokenStr type_str(" ", L" ", 1);
        char data_type = GuessType(ref_type, type_str);

        if(data_type != 0)
            Append(var, &type_str, type);
        else
            Append(var, " VARCHAR(4000)", L" VARCHAR(4000)", 14, type);

        Comment("Use -meta option ", L"Use -meta option ", 17, ref_type, type);
    }
    else
    // Convert LIKE
    if(like != NULL)
    {
        // Get table.column
        Token *col = GetNextToken();

        // table.column%TYPE in Oracle
        if(_target == SQL_ORACLE)
        {
            Prepend(col, "%TYPE", L"%TYPE", 5, like);
            Token::Remove(like);
        }
        else
        if(Target(SQL_SQL_SERVER) == true)
        {
            TokenStr type(" ", L" ", 1);
            GuessType(col, type);

            Append(var, &type, like);
            Comment(ref_type, col);
        }
    }
    else
    // %ROWTYPE
    if(rowtype != NULL)
    {
        // Save variable
        _spl_rowtype_vars.Add(var);

        Token::Remove(var, rowtype);

        Token *semi = GetNextCharToken(';', L';');

        if(semi != NULL)
            Token::Remove(semi);

        // Return ; to the input to allow correct declaration parsing
        PushBack(semi);
    }

    return true;
}

// AutoInc in Sybase ADS
bool SqlParser::ParseAutoIncType(Token *name)
{
    if(name == NULL)
        return false;

	STATS_DECL

	if(!TOKEN_CMP(name, "AUTOINC"))
        return false;

    if(_target == SQL_SQL_SERVER)
		TOKEN_CHANGE(name, "INT IDENTITY");

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BFILE in Oracle
bool SqlParser::ParseBfileType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BFILE", L"BFILE", 5) == false)
        return false;

	STATS_DECL
	STATS_ITM_DECL

    // Convet to VARCHAR(255) in other databases
    if(_target != SQL_ORACLE)
    {
        Token::Change(name, "VARCHAR(255)", L"VARCHAR(255)", 12);
        STATS_ITM_CONV_ERROR
    }

    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BIGDATETIME in Sybase ASE
bool SqlParser::ParseBigdatetimeType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BIGDATETIME", L"BIGDATETIME", 11) == false)
        return false;

	STATS_DECL

	// Convert to DATETIME2(6) in SQL Server
    if(_target == SQL_SQL_SERVER)
        Token:: Change(name, "DATETIME2(6)", L"DATETIME2(6)", 12);
    else
        // Convet to TIMESTAMP in Oracle, PostgreSQL
        if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
            Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);

    STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BIGINT in SQL Server, DB2, MySQL, PostreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseBigintType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    bool bigint = false;
    bool bigint_in_braces = false;

	STATS_DECL

	if(name->Compare("BIGINT", L"BIGINT", 6) == true)
        bigint = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[BIGINT]", L"[BIGINT]", 8) == true)
        bigint_in_braces = true;

    if(bigint == false && bigint_in_braces == false)
        return false;

    Token *open = GetNextCharToken('(', L'(');

    // MySQL BIGINT can include display size
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for BIGINT
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(19) in Oracle
    if(_target == SQL_ORACLE)
    {
        // Change to NUMBER in parameters
        if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS)
            Token::Change(name, "NUMBER", L"NUMBER", 6);
        else
            Token::Change(name, "NUMBER(19)", L"NUMBER(19)", 10);
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && bigint_in_braces == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(bigint_in_braces)
	{
		DTYPE_STATS_V("BIGINT", name)
		DTYPE_DTL_STATS_ST("BIGINT", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_0(name)
	}
	
    return true;
}

// BIGSERIAL in PostgreSQL, Informix
bool SqlParser::ParseBigserialType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BIGSERIAL", L"BIGSERIAL", 9) == false)
        return false;

	STATS_DECL
    Token *open = GetNextCharToken('(', L'(');

    // Start is optional in Informix
    if(open != NULL)
    {
        /*Token *start */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not Informix remove start for BIGSERIAL
        if(_target != SQL_INFORMIX)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(19) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(19)", L"NUMBER(19)", 10);
    else
        // Convert to BIGINT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "BIGINT", L"BIGINT", 6);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BIGTIME in Sybase ASE
bool SqlParser::ParseBigtimeType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BIGTIME", L"BIGTIME", 7) == false)
        return false;

	STATS_DECL

	// Convert to TIMESTAMP in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);
    else
        // Convert to TIME in other databases
        if(_target != SQL_SYBASE)
        {
            Token::Change(name, "TIME", L"TIME", 4);

            if(_target == SQL_SQL_SERVER)
                Append(name, "(6)", L"(6)", 3);
        }

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)
    
    return true;
}

// BINARY in SQL Server, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseBinaryType(Token *name)
{
    if(name == NULL)
        return false;

    bool binary = false;
    bool binary_in_braces = false;

	STATS_DECL

    if(name->Compare("BINARY", L"BINARY", 6) == true)
        binary = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[BINARY]", L"[BINARY]", 8) == true)
            binary_in_braces = true;

    if(binary == false && binary_in_braces == false)
        return false;

    // VARYING keyword in SQL Server, Sybase ASA
    Token *varying = GetNextWordToken("VARYING", L"VARYING", 7);

    Token *open = GetNextCharToken('(', L'(');
    Token *size = NULL;
    Token *close = NULL;

    int size_int = -1;

    // Num is optional
    if(open != NULL)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();
        size_int = size->GetInt();
        close = GetNextCharToken(')', L')');
    }

    // Check for BINARY VARYING
    if(varying != NULL)
    {
        // If MAX is specified, convert to CLOB types
        if(size->Compare("MAX", L"MAX", 3) == true)
        {
            // Change to LONGBLOB in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL))
            {
                Token::Remove(name);
                Token::Change(varying, "LONGBLOB", L"LONGBLOB", 8);
                Token::Remove(open, close);
            }
            else
                // Change to BYTEA in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                {
                    Token::Remove(name);
                    Token::Change(varying, "BYTEA", L"BYTEA", 5);
                    Token::Remove(open, close);
                }
        }
        else
            // Convert to RAW in Oracle
            if(_target == SQL_ORACLE)
            {
                Token::Remove(name);
                Token::Change(varying, "RAW", L"RAW", 3);

                // Size is mandatory in Oracle
                if(open == NULL)
                    Append(varying, "(1)", L"(1)", 3);
            }
            else
                // Convert to VARBINARY in MySQL
                if(Target(SQL_MARIADB, SQL_MYSQL))
                {
                    Token::Remove(name);
                    Token::Change(varying, "VARBINARY", L"VARBINARY", 9);

                    // Size is mandatory in MySQL
                    if(open == NULL)
                        Append(varying, "(1)", L"(1)", 3);
                }
                else
                    // Convert to BYTEA in PostgreSQL
                    if(_target == SQL_POSTGRESQL)
                    {
                        Token::Remove(name);
                        Token::Change(varying, "BYTEA", L"BYTEA", 5);

                        if(open != NULL)
                            Token::Remove(open, close);
                    }
    }
    else
        // Check for BINARY
        if(varying == NULL)
        {
            // Convert to RAW in Oracle
            if(_target == SQL_ORACLE)
            {
                Token::Change(name, "RAW", L"RAW", 3);

                // Size is mandatory in Oracle
                if(open == NULL)
                    Append(name, "(1)", L"(1)", 3);
                else
                    if(size_int == 0)
                        Token::Change(size, "1", L"1", 1);
            }
            else
                // Convert to VARBINARY in SQL Server
                if(_target == SQL_SQL_SERVER)
                {
                    // BINARY is semantically equivalent to VARBINARY in Sybase ASA
                    if(_source == SQL_SYBASE_ASA)
                        Token::Change(name, "VARBINARY", L"VARBINARY", 9);

                    if(size_int == 0)
                        Token::Change(size, "1", L"1", 1);
                }
                else
                    // Convert to BYTEA in PostgreSQL
                    if(_target == SQL_POSTGRESQL)
                    {
                        Token::Change(name, "BYTEA", L"BYTEA", 5);

                        if(open != NULL)
                            Token::Remove(open, close);
                    }
                    else
                        // Remove [] for other databases
                        if(_target != SQL_SQL_SERVER && binary_in_braces == true)
                            Token::ChangeNoFormat(name, name, 1, name->len - 2);
        }

	STATS_ITM_DECL 

	if(binary_in_braces)
	{
		DTYPE_STATS_V("BINARY", name)
		DTYPE_DTL_STATS_ST("BINARY", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// BINARY_DOUBLE in Oracle
bool SqlParser::ParseBinaryDoubleType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BINARY_DOUBLE", L"BINARY_DOUBLE", 13) == false)
        return false;

	STATS_DECL

    // Convert to DOUBLE in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL))
        Token::Change(name, "DOUBLE", L"DOUBLE", 6);
    else
        // Convert to DOUBLE PRECISION in other databases
        if(_target != SQL_ORACLE)
            Token::Change(name, "DOUBLE PRECISION", L"DOUBLE PRECISION", 16);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BINARY_FLOAT in Oracle
bool SqlParser::ParseBinaryFloatType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BINARY_FLOAT", L"BINARY_FLOAT", 12) == false)
        return false;

	STATS_DECL

    // Convert to FLOAT in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL))
        Token::Change(name, "FLOAT", L"FLOAT", 5);
    else
        // Convert to REAL in other databases
        if(_target != SQL_ORACLE)
            Token::Change(name, "REAL", L"REAL", 4);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BIT in SQL Server, MySQL, PostgreSQL, Sybase ASE, Sybase ASA;
// BIT VARYING in PostgreSQL, Sybase ASA
bool SqlParser::ParseBitType(Token *name)
{
    if(name == NULL)
        return false;

    bool bit = false;
    bool bit_in_braces = false;

	STATS_DECL

    if(name->Compare("BIT", L"BIT", 3) == true)
        bit = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[BIT]", L"[BIT]", 5) == true)
        bit_in_braces = true;

    if(bit == false && bit_in_braces == false)
        return false;

    // VARYING keyword in PostgreSQL, Sybase ASA
    Token *varying = GetNextWordToken("VARYING", L"VARYING", 7);

    Token *open = GetNextCharToken('(', L'(');
    Token *num = NULL;

    // Num is optional in MySQL, PostgreSQL BIT and in BIT VARYING
    if(open != NULL)
    {
        num = GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Calculating RAW and BINARY size in bytes
    if(open != NULL && Target(SQL_MARIADB, SQL_MYSQL, SQL_POSTGRESQL, SQL_SYBASE_ASA) == false)
    {
        int size;
        int rem;

        size = num->GetInt();
        rem = size % 8;

        size = size/8;

        if(rem > 0)
            size++;

        Token::Change(num, size);
    }

    // Check for BIT VARYING
    if(varying != NULL)
    {
        DTYPE_STATS(name)

        // Convert to RAW in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(varying, "RAW", L"RAW", 3);

            if(open == NULL)
            {
                // BIT VARYING is unlimited by default in PostgreSQL
                if(_source == SQL_POSTGRESQL)
                    Append(varying, "(2000)", L"(2000)", 6);
                else
                    // BIT VARYING is 1 bit by default in Sybase ASA
                    if(_source == SQL_SYBASE_ASA)
                        Append(varying, "(1)", L"(1)", 3);
            }
        }
        else
        // Convert to BINARY in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Remove(name);
            Token::Change(varying, "BINARY", L"BINARY", 6);

            if(open == NULL)
            {
                // BIT VARYING is 1 bit by default in Sybase ASA
                if(_source == SQL_SYBASE_ASA)
                    Append(varying, "(1)", L"(1)", 3);
            }
        }
    }
    else
    // Check for BIT
    if(varying == NULL)
    {
        DTYPE_STATS(name)

        if(_target == SQL_ORACLE)
        {
            // In MySQL and PostgreSQL BIT is a bit array type
            if(Source(SQL_MYSQL, SQL_POSTGRESQL) == true)
            {
                Token::Change(name, "RAW", L"RAW", 3);

                // BIT is 1 bit by default in MySQL and PostgreSQL
                if(open == NULL)
                    Append(name, "(1)", L"(1)", 3);
            }
            // In SQL Server BIT holds 0, 1 and NULL
            else
                Token::Change(name, "NUMBER(1)", L"NUMBER(1)", 9);
        }
        else
        // Convert MySQL BIT to BINARY in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            if(_source == SQL_MYSQL)
                Token::Change(name, "BINARY", L"BINARY", 6);
        }
        else
        // Convert to BOOLEAN in PostgreSQL
        if(_target == SQL_POSTGRESQL && _source != SQL_POSTGRESQL)
            Token::Change(name, "BOOLEAN", L"BOOLEAN", 7);
        else
        // Convert to TINYINT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_MYSQL)
            Token::Change(name, "TINYINT", L"TINYINT", 7);
        else
        // Convert to SMALLINT in Redshift
        if(_target == SQL_REDSHIFT)
            Token::Change(name, "SMALLINT", L"SMALLINT", 8);
        else
        // Remove [] for other databases
        if(_target != SQL_SQL_SERVER && bit_in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);
    }

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)
    return true;
}

// BLOB in Oracle, DB2, MySQL, Informix
bool SqlParser::ParseBlobType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BLOB", L"BLOB", 4) == false)
        return false;

	STATS_DECL
	STATS_ITM_DECL 
	STATS_SET_DESC(SQL_DATATYPE_BLOB_DESC)

    Token *open = GetNextCharToken('(', L'(');

    // Num and semantics are optional in DB2, MySQL
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextToken();
        Token *semantics = GetNextToken();

        // DB2 allows specifying size (1024, 1M etc.)
        if(Token::Compare(semantics, "K", L"K", 1) == false &&
            Token::Compare(semantics, "M", L"M", 1) == false &&
            Token::Compare(semantics, "G", L"G", 1) == false)
        {
            PushBack(semantics);
            semantics = NULL;
        }

        Token *close = GetNextCharToken(')', L')');

        // If target is not DB2, MySQL remove semantics attribute
        if(Target(SQL_DB2, SQL_MYSQL) == false)
            Token::Remove(open, close);
    }

    // Convert to VARBINARY(max) in SQL Server
    if(_target == SQL_SQL_SERVER)
    {
        Token::Change(name, "VARBINARY", L"VARBINARY", 9);
        AppendNoFormat(name, "(max)", L"(max)", 5);
    }
    else
    // Convert to LONGBLOB in MySQL
    if(Target(SQL_MARIADB, SQL_MARIADB_ORA, SQL_MYSQL) && _source != SQL_MYSQL)
		Token::Change(name, "LONGBLOB", L"LONGBLOB", 8);
    else
    // Convert to BYTEA in PostgreSQL, Greenplum
    if(Target(SQL_POSTGRESQL, SQL_GREENPLUM) == true)
		Token::Change(name, "BYTEA", L"BYTEA", 5);

	STATS_SET_CONV_OK(true)
	STATS_SET_COMPLEXITY(true, STATS_CONV_LOW)

	DTYPE_STATS(name)
    DTYPE_DTL_STATS_L(name)

    return true;
}

// BOOLEAN in MySQL, PostgreSQL, Informix;
// BOOLEAN in Oracle PL/SQL only
// BOOL in MySQL, PostgreSQL
bool SqlParser::ParseBooleanType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BOOLEAN", L"BOOLEAN", 7) == false && name->Compare("BOOL", L"BOOL", 4) == false)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_BOOLEAN_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL 

    if(_target == SQL_MARIADB_ORA)
	{
		STATS_DTL_CONV_NO_NEED(Target(SQL_MARIADB_ORA))
		STATS_ITM_CONV_NO_NEED    
	}
    else
    // Convert to CHAR(1) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "CHAR(1)", L"CHAR(1)", 7);
    else
    // Convert to BIT in SQL Server
    if(_target == SQL_SQL_SERVER)
        Token::Change(name, "BIT", L"BIT", 3);

	STATS_UPDATE_STATUS
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BYTEA in PostreSQL
bool SqlParser::ParseByteaType(Token *name)
{
    if(name == NULL)
        return false;

	STATS_DECL

    if(name->Compare("BYTEA", L"BYTEA", 5) == false)
        return false;

    // Convet to BLOB in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "BLOB", L"BLOB", 4);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BYTE in Informix
bool SqlParser::ParseByteType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BYTE", L"BYTE", 4) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // Optional IN TABLE | IN lobspace
    Token *in = GetNextWordToken("IN", L"IN", 2);
    Token *space = NULL;

    if(in != NULL)
    {
        space = GetNextToken();

        // If target is not Informix remove IN TABLE | IN lobspace attributes
        if(_target != SQL_INFORMIX)
            Token::Remove(in, space);
    }

    // Convert to BLOB in Oracle and DB2
    if(_target == SQL_ORACLE || _target == SQL_DB2)
    {
        Token::Change(name, "BLOB", L"BLOB", 4);
    }
    else
        // Convert to VARBINARY(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "VARBINARY", L"VARBINARY", 9);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
        else
            // Convert to LONGBLOB in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL))
            {
                Token::Change(name, "LONGBLOB", L"LONGBLOB", 8);
            }
            else
                // Convert to BYTEA in PostgreSQL and Greenplum
                if(_target == SQL_POSTGRESQL || _target == SQL_GREENPLUM)
                {
                    Token::Change(name, "BYTEA", L"BYTEA", 5);
                }

				STATS_ITM_DECL 
                DTYPE_DTL_STATS_L(name)

                return true;
}

// BYTEINT in Teradata, 8-bit integer, -127 to 127
bool SqlParser::ParseByteintType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("BYTEINT", L"BYTEINT", 7) == false)
        return false;

	STATS_DECL

    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(3)", L"NUMBER(3)", 9);
    else
        Token::Change(name, "INT", L"INT", 3);

    name->data_type = TOKEN_DT_NUMBER;
    name->data_subtype = TOKEN_DT2_INT;
        
	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// CHARACTER & CHARACTER VARYING in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseCharacterType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    bool character = false;
    bool character_in_braces = false;

	STATS_DECL

    if(name->Compare("CHARACTER", L"CHARACTER", 9) == true)
        character = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[CHARACTER]", L"[CHARACTER]", 11) == true)
            character_in_braces = true;

    if(character == false && character_in_braces == false)
        return false;

    Token *varying = GetNextWordToken("VARYING", L"VARYING", 7);

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');
    Token *size = NULL;
    Token *semantics = NULL;
    Token *close = NULL;

    // Size is optional
    if(open != NULL)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();

        // For Oracle, size can be followed by length semantics CHAR or BYTE; for DB2 only BYTE
        semantics = GetNextWordToken("BYTE", L"BYTE", 4);

        if(semantics == NULL)
        {
            semantics = GetNextWordToken("CHAR", L"CHAR", 4);

            // For Sybase ASA, size can be followed by length semantics CHAR or CHARACTER
            if(semantics == NULL)
                semantics = GetNextWordToken("CHARACTER", L"CHARACTER", 9);
        }

        // If target is not Oracle, DB2 and Sybase ASA remove length semantics attribute
        if(semantics != NULL && _target != SQL_ORACLE && _target != SQL_DB2 && _target != SQL_SYBASE_ASA)
            Token::Remove(semantics);
        // Convert semantic CHARATER in Sybase ASA to CHAR in Oracle
        else
            if(Token::Compare(semantics, "CHARACTER", L"CHARACTER", 9) == true && _target == SQL_ORACLE)
                Token::Change(semantics, "CHAR", L"CHAR", 4);

        Token *comma = GetNextCharToken(',', L',');

        // For Informix, Reserve can be specified
        if(comma != NULL)
        {
            Token *reserve = GetNextNumberToken();

            // If target is not Informix remove reserve attribute
            if(_target != SQL_INFORMIX)
                Token::Remove(comma, reserve);
        }

        close = GetNextCharToken(')', L')');
    }

    bool binary = false;

    Token *for_ = GetNextWordToken("FOR", L"FOR", 3);

    // Check for DB2 FOR BIT DATA clause
    if(for_ != NULL)
    {
        /*Token *bit */ (void) GetNextWordToken("BIT", L"BIT", 3);
        Token *data = GetNextWordToken("DATA", L"DATA", 4);

        // Remove for other databases
        if(_target != SQL_DB2)
            Token::Remove(for_, data);

        binary = true;
    }

    // If MAX is specified, convert to CLOB types
    if(Token::Compare(size, "MAX", L"MAX", 3) == true)
    {
        // Change to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Remove(name);
            Token::Change(varying, "LONGTEXT", L"LONGTEXT", 8);
            Token::Remove(open, close);
        }
        else
            // Change to TEXT in PostgreSQL
            if(_target == SQL_POSTGRESQL)
            {
                Token::Remove(name);
                Token::Change(varying, "TEXT", L"TEXT", 4);
                Token::Remove(open, close);
            }
    }
    else
        // Check for CHARACTER VARYING FOR BIT DATA
        if(binary == true && varying != NULL)
        {
            // Convert DB2 CHARACTER VARYING FOR BIT DATA to RAW in Oracle
            if(_target == SQL_ORACLE)
            {
                Token::Remove(name);
                Token::Change(varying, "RAW", L"RAW", 3);
                Token::Remove(semantics);
            }
            else
                // Convert DB2 CHARACTER VARYING FOR BIT DATA to VARBINARY in SQL Server
                if(_target == SQL_SQL_SERVER)
                {
                    Token::Remove(name);
                    Token::Change(varying, "VARBINARY", L"VARBINARY", 9);
                }
                else
                    // Convert DB2 CHARACTER VARYING FOR BIT DATA to BYTEA in PostgreSQL
                    if(_target == SQL_POSTGRESQL)
                    {
                        Token::Remove(name);
                        Token::Change(varying, "BYTEA", L"BYTEA", 5);

                        if(open != NULL)
                            Token::Remove(open, close);
                    }
        }
        else
            // Check for CHARACTER FOR BIT DATA
            if(binary == true && varying == NULL)
            {
                // Convert DB2 CHARACTER FOR BIT DATA to RAW in SQL Server
                if(_target == SQL_ORACLE)
                {
                    Token::Change(name, "RAW", L"RAW", 3);
                    Token::Remove(semantics);

                    // Size is mandatory in Oracle RAW
                    if(open == NULL)
                        Append(name, "(1)", L"(1)", 3);
                }
                else
                    // Convert DB2 CHARACTER FOR BIT DATA to BINARY in SQL Server
                    if(_target == SQL_SQL_SERVER)
                        Token::Change(name, "BINARY", L"BINARY", 6);
                    else
                        // Convert DB2 CHARACTER FOR BIT DATA to BYTEA in PostgreSQL
                        if(_target == SQL_POSTGRESQL)
                        {
                            Token::Change(name, "BYTEA", L"BYTEA", 5);

                            if(open != NULL)
                                Token::Remove(open, close);
                        }
            }
            else
                // Check for CHARACTER VARYING
                if(varying != NULL)
                {
                    // Convert CHARACTER VARYING to VARCHAR2 in Oracle
                    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
                    {
                        Token::Remove(name);
                        Token::Change(varying, "VARCHAR2", L"VARCHAR2", 8);

                        // Default size
                        if(open == NULL)
                        {
                            // PostgreSQL allows not specify size
                            if(_source == SQL_POSTGRESQL)
                                Append(varying, "(4000)", L"(4000)", 6);
                            else
                                // Size is 1 by default in SQL Server, Informix, Sybase ASE, Sybase ASA
                                if(Source(SQL_SQL_SERVER, SQL_INFORMIX, SQL_SYBASE, SQL_SYBASE_ASA) == true)
                                    Append(varying, "(1)", L"(1)", 3);
                        }
                    }
                    else
                        // VARCHAR is unlimited by default in PostgreSQL
                        if(_target == SQL_POSTGRESQL && open == NULL && _source != SQL_POSTGRESQL)
                            Append(varying, "(1)", L"(1)", 3);
                        else
                            // Size is mandatory in MySQL
                            if(Target(SQL_MARIADB, SQL_MYSQL) && open == NULL)
                                Append(varying, "(1)", L"(1)", 3);
                }
                else
                    // Check for CHAR
                    if(varying == NULL && Target(SQL_MARIADB, SQL_MYSQL))
                    {
                        int num = 1;

                        if(size != NULL)
                            num = size->GetInt();

                        // If size is greater than 255 convert to VARCHAR in MySQL
                        if(num > 255)
                            Token::Change(name, "VARCHAR", L"VARCHAR", 7);

                        // Remove []
                        if(character_in_braces == true)
                            Token::ChangeNoFormat(name, name, 1, name->len - 2);
                    }
                    else
                        // Remove [] for other databases
                        if(_target != SQL_SQL_SERVER && character_in_braces == true)
                            Token::ChangeNoFormat(name, name, 1, name->len - 2);

    // If target is Oracle, and used in parameter list, remove length specification
    if(_target == SQL_ORACLE && 
        (clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS))
        Token::Remove(open, close);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// CHAR and CHAR VARYING in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseCharType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_CHAR_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    bool char_ = false;
    bool char_in_braces = false;

    if(name->Compare("CHAR", L"CHAR", 4) == true)
        char_ = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[CHAR]", L"[CHAR]", 6) == true)
        char_in_braces = true;

    if(char_ == false && char_in_braces == false)
        return false;

    Token *varying = GetNextWordToken("VARYING", L"VARYING", 7);

    Token *open = GetNextCharToken('(', L'(');
    Token *size = NULL;
    Token *semantics = NULL;
    Token *close = NULL;

    // Size is optional
    if(open != NULL)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();

        // For Oracle, size can be followed by length semantics CHAR or BYTE; for DB2 only BYTE
        semantics = GetNextWordToken("BYTE", L"BYTE", 4);

        if(semantics == NULL)
        {
            semantics = GetNextWordToken("CHAR", L"CHAR", 4);

            // For Sybase ASA, size can be followed by length semantics CHAR or CHARACTER
            if(semantics == NULL)
                semantics = GetNextWordToken("CHARACTER", L"CHARACTER", 9);
        }

        // If target is not Oracle, DB2 and Sybase ASA remove length semantics attribute
        if(semantics != NULL && _target != SQL_ORACLE && _target != SQL_DB2 && _target != SQL_SYBASE_ASA)
            Token::Remove(semantics);
        // Convert semantic CHARATER in Sybase ASA to CHAR in Oracle
        else
        if(Token::Compare(semantics, "CHARACTER", L"CHARACTER", 9) == true && _target == SQL_ORACLE)
            Token::Change(semantics, "CHAR", L"CHAR", 4);

        Token *comma = GetNextCharToken(',', L',');

        // For Informix, Reserve can be specified
        if(comma != NULL)
        {
            Token *reserve = GetNextNumberToken();

            // If target is not Informix remove reserve attribute
            if(_target != SQL_INFORMIX)
                Token::Remove(comma, reserve);
        }

        close = GetNextCharToken(')', L')');
    }

    bool binary = false;

    Token *for_ = GetNextWordToken("FOR", L"FOR", 3);

    // Check for DB2 FOR BIT DATA clause
    if(for_ != NULL)
    {
        Token *bit = GetNextWordToken("BIT", L"BIT", 3);
        Token *data = GetNextWordToken(bit, "DATA", L"DATA", 4);

        if(data != NULL)
        {

            // Remove for other databases
            if(_target != SQL_DB2)
                Token::Remove(for_, data);

            binary = true;
        }
        else
            PushBack(for_);
    }

    // If target is Oracle, and used in parameter list, remove length specification
    if(_target == SQL_ORACLE && (clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS))
        Token::Remove(open, close);

    // If MAX is specified, convert to CLOB types
    if(size != NULL && size->Compare("MAX", L"MAX", 3) == true)
    {
        // Change to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Remove(name);
            Token::Change(varying, "LONGTEXT", L"LONGTEXT", 8);
            Token::Remove(open, close);
        }
        else
        // Change to TEXT in PostgreSQL
        if(_target == SQL_POSTGRESQL)
        {
            Token::Remove(name);
            Token::Change(varying, "TEXT", L"TEXT", 4);
            Token::Remove(open, close);
        }
    }
    else
    // Check for CHAR VARYING FOR BIT DATA
    if(binary == true && varying != NULL)
    {
        // Convert DB2 CHAR VARYING FOR BIT DATA to RAW in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(varying, "RAW", L"RAW", 3);
            Token::Remove(semantics);
        }
        else
        // Convert DB2 CHAR VARYING FOR BIT DATA to VARBINARY in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Remove(name);
            Token::Change(varying, "VARBINARY", L"VARBINARY", 9);
        }
        else
        // Convert DB2 CHAR VARYING FOR BIT DATA to BYTEA in PostgreSQL
        if(_target == SQL_POSTGRESQL)
        {
            Token::Remove(name);
            Token::Change(varying, "BYTEA", L"BYTEA", 5);

            if(open != NULL)
                Token::Remove(open, close);
        }
    }
    else
    // Check for CHAR FOR BIT DATA
    if(binary == true && varying == NULL)
    {
        // Convert DB2 CHAR FOR BIT DATA to RAW in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Change(name, "RAW", L"RAW", 3);
            Token::Remove(semantics);

            // Size is mandatory in Oracle RAW
            if(open == NULL)
                Append(name, "(1)", L"(1)", 3);
        }
        else
        // Convert DB2 CHAR FOR BIT DATA to BINARY in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "BINARY", L"BINARY", 6);
        else
        // Convert DB2 CHAR FOR BIT DATA to BYTEA in PostgreSQL
        if(_target == SQL_POSTGRESQL)
        {
            Token::Change(name, "BYTEA", L"BYTEA", 5);

            if(open != NULL)
                Token::Remove(open, close);
        }
    }
    else
    // Check for CHAR VARYING
    if(varying != NULL)
    {
        // Convert CHAR VARYING to VARCHAR2 in Oracle
        if(_target == SQL_ORACLE && _source != SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(varying, "VARCHAR2", L"VARCHAR2", 8);

            // Default size
            if(open == NULL)
            {
                // PostgreSQL allows not specify size
                if(_source == SQL_POSTGRESQL)
                    Append(varying, "(4000)", L"(4000)", 6);
                else
                // Size is 1 by default in SQL Server, Informix, Sybase ASE, Sybase ASA
                if(Source(SQL_SQL_SERVER, SQL_INFORMIX, SQL_SYBASE, SQL_SYBASE_ASA) == true)
                    Append(varying, "(1)", L"(1)", 3);
            }
        }
        else
        // VARCHAR is unlimited by default in PostgreSQL
        if(_target == SQL_POSTGRESQL && open == NULL && _source != SQL_POSTGRESQL)
            Append(varying, "(1)", L"(1)", 3);
        else
        // Size is mandatory in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL) && open == NULL)
            Append(varying, "(1)", L"(1)", 3);
    }
    else
    // Check for CHAR
    if(varying == NULL)
    {
        int num = 1;

        if(size != NULL)
            num = size->GetInt();

        // If size is greater than 255 convert to VARCHAR in MySQL
        if(num > 255 && Target(SQL_MARIADB, SQL_MYSQL))
            Token::Change(name, "VARCHAR", L"VARCHAR", 7);
        else
        // STRING in Hive
        if(_source != SQL_HIVE && _target == SQL_HIVE)
        {
            TOKEN_CHANGE(name, "STRING");
            Token::Remove(open, close);
        }
        else
        // Remove []
        if(char_in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);
        else
		{
            STATS_ITM_CONV_NO_NEED
            STATS_DTL_CONV_NO_NEED(true)
		}
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && char_in_braces == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_UPDATE_STATUS

	// Collapse reporting items for small CHARs
	if(size != NULL && size->GetInt() <= 128 && si.conv_status == STATS_CONV_NO_NEED)
	{
		DTYPE_DTL_STATS_ST("CHAR(<=128)", NULL);
	}
	else
	{
		if(char_in_braces)
			DTYPE_DTL_STATS_ST("CHAR", NULL)
		else
			DTYPE_DTL_STATS_L(name)
	}
    
	if(char_in_braces)
		DTYPE_STATS_V("CHAR", name)
	else
		DTYPE_STATS(name)

    return true;
}

// CLOB in Oracle, DB2, Informix
bool SqlParser::ParseClobType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("CLOB", L"CLOB", 4) == false)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_CLOB_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL 

    Token *open = GetNextCharToken('(', L'(');

    // Num and semantics are optional in DB2
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextToken();
        Token *semantics = GetNextToken();

        // DB2 allows specifying size (1024, 1M etc.)
        if(Token::Compare(semantics, "K", L"K", 1) == false &&
            Token::Compare(semantics, "M", L"M", 1) == false &&
            Token::Compare(semantics, "G", L"G", 1) == false)
        {
            PushBack(semantics);
            semantics = NULL;
        }

        Token *close = GetNextCharToken(')', L')');

        // If target is not DB2 remove semantics attribute
        if(_target != SQL_DB2)
            Token::Remove(open, close);
    }

    // Convert to VARCHAR(max) in SQL Server
    if(_target == SQL_SQL_SERVER)
    {
        Token::Change(name, "VARCHAR", L"VARCHAR", 7);
        AppendNoFormat(name, "(max)", L"(max)", 5);
    }
    else
    // Convert to LONGTEXT in MySQL
    if(Target(SQL_MARIADB, SQL_MARIADB_ORA, SQL_MYSQL))
        Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
    else
    // Convert to TEXT in PostgreSQL, Greenplum
    if(Target(SQL_POSTGRESQL, SQL_GREENPLUM) == true)
        Token::Change(name, "TEXT", L"TEXT", 4);

	STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
	STATS_UPDATE_STATUS

    DTYPE_STATS(name)
    DTYPE_DTL_STATS_L(name)

    return true;
}

// CURDOUBLE in Sybase ADS
bool SqlParser::ParseCurDoubleType(Token *name)
{
    if(name == NULL && _source != SQL_SYBASE_ADS)
        return false;

    if(!TOKEN_CMP(name, "CURDOUBLE"))
        return false;

	STATS_DECL
	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_L(name)

	Token *open = TOKEN_GETNEXT('(');
	Token *close = NULL;
	    
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        close = TOKEN_GETNEXT(')');
    }

    // Convert to VARCHAR(max) in SQL Server
    if(_target == SQL_SQL_SERVER)
	{
        TOKEN_CHANGE(name, "FLOAT");

		if(open != NULL)
			Token::Remove(open, close);
	}
    
    return true;
}

// DATETIME in SQL Server, MySQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseDatetimeType(Token *name)
{
    if(name == NULL)
        return false;

    bool datetime = false;
    bool datetime_in_braces = false;
    bool datetime_in_quotes = false;

    if(name->Compare("DATETIME", L"DATETIME", 8) == true)
        datetime = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[DATETIME]", L"[DATETIME]", 10) == true)
        datetime_in_braces = true;
    else
    // Can be enclosed in "" for Sybase ASA
    if(name->Compare("\"DATETIME\"", L"\"DATETIME\"", 10) == true)
        datetime_in_quotes = true;

    if(datetime == false && datetime_in_braces == false && datetime_in_quotes == false)
        return false;

	STATS_DECL

    // Informix DATETIME can include start and end field
    Token *first_unit = GetNextToken();
    Token *first_year = NULL;
    Token *first_month = NULL;
    Token *first_day = NULL;
    Token *first_hour = NULL;
    Token *first_minute = NULL;
    Token *first_second = NULL;
    Token *first_fraction = NULL;

    Token *to = NULL;

    Token *second_unit = NULL;
    Token *second_year = NULL;
    Token *second_month = NULL;
    Token *second_day = NULL;
    Token *second_hour = NULL;
    Token *second_minute = NULL;
    Token *second_second = NULL;
    Token *second_fraction = NULL;

    if(Token::Compare(first_unit, "YEAR", L"YEAR", 4) == true)
        first_year = first_unit;
    else
    if(Token::Compare(first_unit, "MONTH", L"MONTH", 5) == true)
        first_month = first_unit;
    else
    if(Token::Compare(first_unit, "DAY", L"DAY", 3) == true)
        first_day = first_unit;
    else
    if(Token::Compare(first_unit, "HOUR", L"HOUR", 4) == true)
        first_hour = first_unit;
    else
    if(Token::Compare(first_unit, "MINUTE", L"MINUTE", 6) == true)
        first_minute = first_unit;
    else
    if(Token::Compare(first_unit, "SECOND", L"SECOND", 6) == true)
        first_second = first_unit;
    else
    if(Token::Compare(first_unit, "FRACTION", L"FRACTION", 8) == true)
        first_fraction = first_unit;
    else
    {
        PushBack(first_unit);
        first_unit = NULL;
    }

    if(first_unit != NULL)
    {
        to = GetNextWordToken("TO", L"TO", 2);

        second_unit = GetNextToken(to);

        if(Token::Compare(second_unit, "YEAR", L"YEAR", 4) == true)
            second_year = second_unit;
        else
        if(Token::Compare(second_unit, "MONTH", L"MONTH", 5) == true)
            second_month = second_unit;
        else
        if(Token::Compare(second_unit, "DAY", L"DAY", 3) == true)
            second_day = second_unit;
        else
        if(Token::Compare(second_unit, "HOUR", L"HOUR", 4) == true)
            second_hour = second_unit;
        else
        if(Token::Compare(second_unit, "MINUTE", L"MINUTE", 6) == true)
            second_minute = second_unit;
        else
        if(Token::Compare(second_unit, "SECOND", L"SECOND", 6) == true)
            second_second = second_unit;
        else
        if(Token::Compare(second_unit, "FRACTION", L"FRACTION", 8) == true)
            second_fraction = second_unit;
        else
        {
            PushBack(second_unit);
            second_unit = NULL;
        }
    }

    // MySQL and Informix DATETIME can include fraction
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *fraction */ (void) GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to TIMESTAMP in Oracle, PostgreSQL
    if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
    {
        if(_source != SQL_INFORMIX)
            Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);

        // Fraction is 3 by default in SQL Server, Sybase ASE
        if(Source(SQL_SQL_SERVER, SQL_SYBASE) && open == NULL)
            Append(name, "(3)", L"(3)", 3);
        else
        // Fraction is 0 by default in MySQL
        if(_source == SQL_MYSQL && open == NULL)
            Append(name, "(0)", L"(0)", 3);
        else
        // Units can specify range and precious in Informix
        if(_source == SQL_INFORMIX)
        {
            // YEAR TO ...
            if(first_year != NULL)
            {
                // YEAR TO FRACTION(n)
                if(second_fraction != NULL)
                {
                    Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);
                    Token::Remove(first_unit, second_unit);
                }
                else
                // YEAR TO SECOND, MINUTE, HOUR
                if(second_second != NULL || second_minute != NULL || second_hour != NULL)
                {
                    Token::Change(name, "TIMESTAMP(0)", L"TIMESTAMP(0)", 12);
                    Token::Remove(first_unit, second_unit);
                }
                else
                // YEAR TO DAY
                if(second_day != NULL)
                {
                    Token::Change(name, "DATE", L"DATE", 4);
                    Token::Remove(first_unit, second_unit);
                }
            }
        }		
    }
    else
    // Convert to DATETIME2(6) in SQL Server
    if(_target == SQL_SQL_SERVER && _source != SQL_SQL_SERVER)
    {
        Token::Change(name, "DATETIME2", L"DATETIME2", 9);

        // Remove units
        if(_source == SQL_INFORMIX)
            Token::Remove(first_unit, second_unit);

        if(open == NULL)
        {
            // Fraction is 0 by default in MySQL
            if(_source == SQL_MYSQL)
                Append(name, "(0)", L"(0)", 3);
            // Fraction is 3 by default in Informix
            if(_source == SQL_INFORMIX)
                Append(name, "(3)", L"(3)", 3);
            else
            // Fraction is 6 by default in Sybase ASE, Sybase ASA
            if(Source(SQL_SYBASE, SQL_SYBASE_ASA) == true)
                Append(name, "(6)", L"(6)", 3);
        }
    }
    else
    // Append fraction in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL))
    {
        // Remove []
        if(datetime_in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);

        if(open == NULL)
        {
            // Fraction is 3 in SQL Server, Sybase ASE
            if(Source(SQL_SQL_SERVER, SQL_SYBASE))
                Append(name, "(3)", L"(3)", 3);
        }
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && datetime_in_braces == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);
    else
    // Remove "" for other databases
    if(_target != SQL_SYBASE_ASA && datetime_in_quotes == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);

    name->data_type = TOKEN_DT_DATETIME;
    name->data_subtype = TOKEN_DT2_DATETIME;

	STATS_ITM_DECL 

	if(datetime_in_braces)
	{
		DTYPE_STATS_V("DATETIME", name)
		DTYPE_DTL_STATS_ST("DATETIME", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// DATETIME2 in SQL Server
bool SqlParser::ParseDatetime2Type(Token *name)
{
    if(name == NULL)
        return false;

    bool datetime2 = false;
    bool datetime2_in_braces = false;

    if(name->Compare("DATETIME2", L"DATETIME2", 9) == true)
        datetime2 = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[DATETIME2]", L"[DATETIME2]", 11) == true)
        datetime2_in_braces = true;

    if(datetime2 == false && datetime2_in_braces == false)
        return false;

	STATS_DECL

    Token *open = GetNextCharToken('(', L'(');
    Token *fraction = NULL;

    int fraction_int = 0;

    // Fraction is optional in SQL Server
    if(open != NULL)
    {
        fraction = GetNextNumberToken();
        fraction_int = fraction->GetInt();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to DATETIME in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL))
    {
        Token::Change(name, "DATETIME", L"DATETIME", 8);

        // Fraction is 7 by default in SQL Server, but max fraction is 6 in MySQL
        if(open == NULL)
            Append(name, "(6)", L"(6)", 3);
        else
            if(fraction_int > 6)
                Token::Change(fraction, "6", L"6", 1);
    }
    else
    // Convert to TIMESTAMP in PostgreSQL, Redshift
    if(Target(SQL_POSTGRESQL, SQL_REDSHIFT))
    {
        Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);

        // Fraction is 7 by default in SQL Server, but max fraction is 6 in PostgreSQL
        if(open == NULL)
            Append(name, "(6)", L"(6)", 3);
        else
        if(fraction_int > 6)
            Token::Change(fraction, "6", L"6", 1);
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && datetime2_in_braces == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(datetime2_in_braces)
	{
		DTYPE_STATS_V("DATETIME2", name)
		DTYPE_DTL_STATS_ST("DATETIME2", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// DATETIMEOFFSET in SQL Server, Sybase ASA
bool SqlParser::ParseDatetimeoffsetType(Token *name)
{
    if(name == NULL)
        return false;

    bool datetimeoffset = false;
    bool datetimeoffset_in_braces = false;

    if(name->Compare("DATETIMEOFFSET", L"DATETIMEOFFSET", 14) == true)
        datetimeoffset = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[DATETIMEOFFSET]", L"[DATETIMEOFFSET]", 16) == true)
            datetimeoffset_in_braces = true;

    if(datetimeoffset == false && datetimeoffset_in_braces == false)
        return false;

	STATS_DECL

    int fraction_int = 0;

    Token *open = GetNextCharToken('(', L'(');
    Token *fraction = NULL;
    Token *close = NULL;

    // Fraction is optional in SQL Server
    if(open != NULL)
    {
        fraction = GetNextNumberToken();
        fraction_int = fraction->GetInt();
        close = GetNextCharToken(')', L')');
    }

    // Convert to TIMESTAMP WITH TIME ZONE in Oracle
    if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
    {
        Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);

        if(open == NULL)
        {
            // Fraction is 7 by default in SQL Server, but max fraction is 6 in PostgreSQL
            if(_source == SQL_SQL_SERVER)
                Append(name, "(6)", L"(6)", 3);
        }
        else
            if(fraction_int > 6)
                Token::Change(fraction, "6", L"6", 1);

        if(open != NULL)
            Append(close, " WITH TIME ZONE", L" WITH TIME ZONE", 15);
        else
            Append(name, " WITH TIME ZONE", L" WITH TIME ZONE", 15);
    }
    else
        // Fraction is 6 by default in Sybase ASA
        if(_target == SQL_SQL_SERVER && _source == SQL_SYBASE_ASA)
            Append(name, "(6)", L"(6)", 3);
        else
            // Convert to DATETIME in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL))
            {
                Token::Change(name, "DATETIME", L"DATETIME", 8);

                if(open == NULL)
                {
                    // Fraction is 7 by default in SQL Server, but max fraction is 6 in MySQL
                    if(_source == SQL_SQL_SERVER)
                        Append(name, "(6)", L"(6)", 3);
                }
                else
                    // Max fraction is 6 in MySQL
                    if(fraction_int > 6)
                        Token::Change(fraction, "6", L"6", 1);
            }
            else
                // Remove [] for other databases
                if(_target != SQL_SQL_SERVER && datetimeoffset_in_braces == true)
                    Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(datetimeoffset_in_braces)
	{
		DTYPE_STATS_V("DATETIMEOFFSET", name)
		DTYPE_DTL_STATS_ST("DATETIMEOFFSET", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// DATE in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseDateType(Token *name)
{
    if(name == NULL)
        return false;

	STATS_DECL

	if(_source != SQL_ORACLE)
		STATS_SET_DESC(SQL_DATATYPE_DATE_DESC)
	else
		STATS_SET_DESC(SQL_DATATYPE_DATE_ORA_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL 

    bool date = false;
    bool date_in_braces = false;

    if(name->Compare("DATE", L"DATE", 4) == true)
        date = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[DATE]", L"[DATE]", 6) == true)
        date_in_braces = true;

    if(date == false && date_in_braces == false)
        return false;

    // Check for Oracle DATE
    if(_source == SQL_ORACLE)
    {
        if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL, SQL_SYBASE))
            Token::Change(name, "DATETIME", L"DATETIME", 8);
		else
		if(_target == SQL_MARIADB_ORA)
		{
			STATS_DTL_CONV_NO_NEED(true)
			STATS_ITM_CONV_NO_NEED 
		}
        else
        if(Target(SQL_DB2, SQL_POSTGRESQL, SQL_GREENPLUM, SQL_ESGYNDB))
            Token::Change(name, "TIMESTAMP(0)", L"TIMESTAMP(0)", 12);
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && date_in_braces == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);
        
	name->data_type = TOKEN_DT_DATETIME;
	name->data_subtype = (_source == SQL_ORACLE) ? TOKEN_DT2_DATETIME_SEC : TOKEN_DT2_DATE;

	STATS_UPDATE_STATUS

	if(date_in_braces)
	{
		DTYPE_STATS_V("DATE", name)
		DTYPE_DTL_STATS_ST("DATE", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// DBCLOB in DB2
bool SqlParser::ParseDbclobType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("DBCLOB", L"DBCLOB", 6) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Num and semantics are optional in DB2
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextToken();
        Token *semantics = GetNextToken();

        // DB2 allows specifying size (1024, 1M etc.)
        if(Token::Compare(semantics, "K", L"K", 1) == false &&
            Token::Compare(semantics, "M", L"M", 1) == false &&
            Token::Compare(semantics, "G", L"G", 1) == false)
        {
            PushBack(semantics);
            semantics = NULL;
        }

        Token *close = GetNextCharToken(')', L')');

        // If target is not DB2 remove semantics attribute
        if(_target != SQL_DB2)
            Token::Remove(open, close);
    }

    // Convert to NCLOB in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NCLOB", L"NCLOB", 5);
    else
        // Convert to NVARCHAR(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
        else
            // Convert to TEXT in PostgreSQL
            if(_target == SQL_POSTGRESQL)
                Token::Change(name, "TEXT", L"TEXT", 4);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// DECFLOAT in DB2
bool SqlParser::ParseDecfloatType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("DECFLOAT", L"DECFLOAT", 8) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');
    Token *precision = NULL;
    Token *close = NULL;

    if(open != NULL)
    {
        precision = GetNextNumberToken();
        close = GetNextCharToken(')', L')');
    }

    // Convert to NUMBER in Oracle
    if(_target == SQL_ORACLE)
    {
        Token::Change(name, "NUMBER", L"NUMBER", 6);

        if(open != NULL)
            Token::Remove(open, close);
    }
    else
        // Convert to FLOAT in SQL Server, PostgreSQL
        if(Target(SQL_SQL_SERVER, SQL_POSTGRESQL) == true)
        {
            Token::Change(name, "FLOAT", L"FLOAT", 5);

            if(open != NULL)
                Token::Remove(open, close);
        }

		STATS_ITM_DECL 
        DTYPE_DTL_STATS_L(name)

        return true;
}

// DECIMAL and DEC in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA, Teradata
bool SqlParser::ParseDecimalType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_DECIMAL_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    bool decimal = false;
    bool decimal_in_braces = false;

    if(name->Compare("DECIMAL", L"DECIMAL", 7) == true || name->Compare("DEC", L"DEC", 3) == true)
        decimal = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[DECIMAL]", L"[DECIMAL]", 9) == true || name->Compare("[DEC]", L"[DEC]", 5) == true)
        decimal_in_braces = true;

    if(decimal == false && decimal_in_braces == false)
        return false;

    Token *open = GetNextCharToken('(', L'(');
    Token *close = NULL;
    bool conv = false;

    // Precision and scale are optional
    if(open != NULL)
    {
        Token *precision = GetNextNumberToken();

        // Scale is optional
        Token *comma = GetNextCharToken(precision, ',', L',');
        Token *scale = GetNextNumberToken(comma);

        close = GetNextCharToken(')', L')');

        if(scale == NULL)
        {
            // Scale is 6 by default in Sybase ASA
            if(_source == SQL_SYBASE_ASA && _target != SQL_SYBASE_ASA)
            {
                Append(precision, ",6", L",6", 2);
                conv = true;
            }
        }

        int p = (precision != NULL) ? precision->GetInt() : 0;

        // If precision > 18, convert to NUMERIC for EsgynDB
        if(p > 18 && _target == SQL_ESGYNDB)
        {
            TOKEN_CHANGE(name, "NUMERIC");
            conv = true;
        }
    }

    // Default precision and scale
    if(open == NULL)
    {
        // NUMERIC(38,0) is default in Oracle
        if(_source == SQL_ORACLE && _target != SQL_ORACLE)
        {
            Append(name, "(38,0)", L"(38,0)", 6);
            conv = true;
        }
        else
        // DECIMAL(18,0) is default in SQL Server and Sybase ASE
        if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
        {
            Append(name, "(18,0)", L"(18,0)", 6);
            conv = true;
        }
        else
        // DECIMAL(5,0) is default in DB2
        if(_source == SQL_DB2 && _target != SQL_DB2)
        {
            Append(name, "(5,0)", L"(5,0)", 5);
            conv = true;
        }
        else
        // DECIMAL(10,0) is default in MySQL
        if(_source == SQL_MYSQL && _target != SQL_MYSQL)
        {
            Append(name, "(10,0)", L"(10,0)", 6);
            conv = true;
        }
        else
        // DECIMAL(16,0) is default in Informix
        if(_source == SQL_INFORMIX && _target != SQL_INFORMIX)
        {
            Append(name, "(16,0)", L"(16,0)", 6);
            conv = true;
        }
        else
        // DECIMAL(30,6) is default in Sybase ASA
        if(_source == SQL_SYBASE_ASA && _target != SQL_SYBASE_ASA)
        {
            Append(name, "(30,6)", L"(30,6)", 6);
            conv = true;
        }
    }

    // Convert to NUMBER in Oracle
    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
    {
        Token::Change(name, "NUMBER", L"NUMBER", 6);
        conv = true;

        // Remove precision and scale in parameters
        if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS)
            Token::Remove(open, close);
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && decimal_in_braces == true)
    {
        Token::ChangeNoFormat(name, name, 1, name->len - 2);
        conv = true;
    }

    if(!conv)
	{
		STATS_DTL_CONV_NO_NEED(true)
        STATS_ITM_CONV_NO_NEED
	}

	STATS_UPDATE_STATUS

	if(decimal_in_braces)
	{
		DTYPE_STATS_V("DECIMAL", name)
		DTYPE_DTL_STATS_ST("DECIMAL", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// DOUBLE PRECISION in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseDoubleType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("DOUBLE", L"DOUBLE", 6) == false)
        return false;

	STATS_DECL

	STATS_ITM_DECL
    DTYPE_STATS(name)

    // DB2 and MySQL can omit PRECISION keyword
    Token *prec = GetNextWordToken("PRECISION", L"PRECISION", 9);
    Token *open = GetNextCharToken('(', L'(');

    bool conv = false;

    // MySQL DOUBLE PRECISION can include display size
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        /*Token *comma */ (void) GetNextCharToken(',', L',');
        /*Token *scale */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove size for DOUBLE PRECISION
        if(_target != SQL_MYSQL)
        {
            Token::Remove(open, close);
            conv = true;
        }
    }

    // Convert to BINARY_DOUBLE in Oracle
    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
    {
        if(prec != NULL)
        {
            Token::Remove(name);
            Token::Change(prec, "BINARY_DOUBLE", L"BINARY_DOUBLE", 13);
        }
        else
            Token::Change(name, "BINARY_DOUBLE", L"BINARY_DOUBLE", 13);

        conv = true;
    }
    else
    // Convert to FLOAT in SQL Server
    if(_target == SQL_SQL_SERVER && _source != SQL_SQL_SERVER)
    {
        if(prec != NULL)
        {
            Token::Remove(name);
            Token::Change(prec, "FLOAT", L"FLOAT", 5);
        }
        else
            Token::Change(name, "FLOAT", L"FLOAT", 5);

        conv = true;
    }

    // Append PRECISION keyword in PostgreSQL
    if(_target == SQL_POSTGRESQL)
    {
        if(prec == NULL)
        {
            Append(name, " PRECISION", L" PRESICION", 10);
            conv = true;
        }
    }

    if(!conv)
        STATS_ITM_CONV_NO_NEED

    DTYPE_DTL_STATS_L(name)

    return true;
}

// FIXED in MySQL
bool SqlParser::ParseFixedType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("FIXED", L"FIXED", 5) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Precision and scale are optional in MySQL
    if(open != NULL)
    {
        /*Token *precision */ (void) GetNextNumberToken();
        Token *comma = GetNextCharToken(',', L',');	

        if(comma != NULL)
            /*Token *scale */ (void) GetNextNumberToken(); 

        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to NUMBER in Oracle
    if(_target == SQL_ORACLE)
    {
        Token::Change(name, "NUMBER", L"NUMBER", 6);

        if(open == NULL)
            Append(name, "(10,0)", L"(10,0)", 6);
    }
    else
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "DECIMAL", L"DECIMAL", 7);

            if(open == NULL)
                Append(name, "(10,0)", L"(10,0)", 6);
        }

		STATS_ITM_DECL 
        DTYPE_DTL_STATS_L(name)

        return true;
}

// FLOAT in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseFloatType(Token *name)
{
    if(name == NULL)
        return false;

    bool float_ = false;
    bool float_in_braces = false;
    bool conv = false;

    if(name->Compare("FLOAT", L"FLOAT", 5) == true)
        float_ = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[FLOAT]", L"[FLOAT]", 7) == true)
        float_in_braces = true;

    if(float_ == false && float_in_braces == false)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_FLOAT_DESC)

	STATS_ITM_DECL

    Token *open = GetNextCharToken('(', L'(');
    Token *close = NULL;

    // Precision is optional
    if(open != NULL)
    {
        /*Token *precision */ (void) GetNextToken();
        Token *comma = GetNextCharToken(',', L',');

        // Scale can be specified for MySQL
        if(comma != NULL)
        {
            Token *scale = GetNextToken();

            // Remove scale if target is not MySQL
            if(_target != SQL_MYSQL)
            {
                Token::Remove(comma, scale);
                conv = true;
            }
        }

        close = GetNextCharToken(')', L')');
    }

    // Convert to BINARY_DOUBLE in Oracle
    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
    {
        Token::Change(name, "BINARY_DOUBLE", L"BINARY_DOUBLE", 13);
        conv = true;

        if(open != NULL)
            Token::Remove(open, close);
    }
    else
    // Remove precision for SQL Server
    if(_target == SQL_SQL_SERVER && Source(SQL_SYBASE_ASA, SQL_SQL_SERVER) == false && open != NULL)
    {
        Token::Remove(open, close);
        conv = true;
    }
    else
    // Convert to DOUBLE in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_MYSQL)
    {
        Token::Change(name, "DOUBLE", L"DOUBLE", 6);
        conv = true;

        if(open != NULL)
            Token::Remove(open, close);
    }
    else
    // Convert to DOUBLE PRECISION in PostgreSQL
    if(_target == SQL_POSTGRESQL && _source != SQL_POSTGRESQL)
    {
        Token::Change(name, "DOUBLE PRECISION", L"DOUBLE PRECISION", 16);
        conv = true;

        if(open != NULL)
            Token::Remove(open, close);
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && float_in_braces == true)
    {
        Token::ChangeNoFormat(name, name, 1, name->len - 2);
        conv = true;
    }

    if(!conv)
	{
		STATS_SET_CONV_NO_NEED(true)
        STATS_ITM_CONV_NO_NEED
	}

    if(float_in_braces)
	{
		DTYPE_STATS_V("FLOAT", name)
		DTYPE_DTL_STATS_ST("FLOAT", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// FLOAT4 in MySQL, PostgreSQL
bool SqlParser::ParseFloat4Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("FLOAT4", L"FLOAT4", 6) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Precision and scale are optional in MySQL
    if(open != NULL)
    {
        /*Token *precision */ GetNextToken();
        Token *comma = GetNextCharToken(',', L',');

        if(comma != NULL)
            /*Token *scale */ GetNextToken();

        Token *close = GetNextCharToken(')', L')');

        // Remove precision and scale if target is not MySQL
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to BINARY_FLOAT in Oracle
    if(_target == SQL_ORACLE)
    {
        if(_source == SQL_MYSQL)
            Token::Change(name, "BINARY_DOUBLE", L"BINARY_DOUBLE", 13);
        else
            Token::Change(name, "BINARY_FLOAT", L"BINARY_FLOAT", 12);
    }
    else
        // Convert to FLOAT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "FLOAT", L"FLOAT", 5);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// FLOAT8 in MySQL, PostgreSQL
bool SqlParser::ParseFloat8Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("FLOAT8", L"FLOAT8", 6) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // MySQL FLOAT8 can include display size
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        /*Token *comma */ (void) GetNextCharToken(',', L',');
        /*Token *scale */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove size for FLOAT8
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to BINARY_DOUBLE in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "BINARY_DOUBLE", L"BINARY_DOUBLE", 13);
    else
        // Convert to FLOAT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "FLOAT", L"FLOAT", 5);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// GRAPHIC in DB2
bool SqlParser::ParseGraphicType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("GRAPHIC", L"GRAPHIC", 7) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Num is optional
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to NCHAR in Oracle, SQL Server
    if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
        Token::Change(name, "NCHAR", L"NCHAR", 5);
    else
        // Convert to CHAR in PostgreSQL
        if(_target == SQL_POSTGRESQL)
            Token::Change(name, "CHAR", L"CHAR", 4);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// IMAGE in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseImageType(Token *name)
{
    if(name == NULL)
        return false;

    bool image = false;
    bool image_in_braces = false;
    bool image_in_quotes = false;

	STATS_DECL

    if(name->Compare("IMAGE", L"IMAGE", 5) == true)
        image = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[IMAGE]", L"[IMAGE]", 7) == true)
            image_in_braces = true;
        else
            // Can be enclosed in "" for Sybase ASA
            if(name->Compare("\"IMAGE\"", L"\"IMAGE\"", 7) == true)
                image_in_quotes = true;

    if(image == false && image_in_braces == false && image_in_quotes == false)
        return false;

    // Convert to BLOB in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "BLOB", L"BLOB", 4);
    else
        // Convert to VARBINARY(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "VARBINARY", L"VARBINARY", 9);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
        else
            // Convert to LONGBLOB in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL))
                Token::Change(name, "LONGBLOB", L"LONGBLOB", 8);
            else
                // Convert to BYTEA in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                    Token::Change(name, "BYTEA", L"BYTEA", 5);
                else
                    // Remove [] for other databases
                    if(_target != SQL_SQL_SERVER && image_in_braces == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);
                    else
                        // Remove "" for other databases
                        if(_target != SQL_SYBASE_ASA && image_in_quotes == true)
                            Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(image_in_braces)
	{
		DTYPE_STATS_V("IMAGE", name)
		DTYPE_DTL_STATS_ST("IMAGE", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// INTERVAL in Oracle, PostgreSQL, Informix, Netezza
bool SqlParser::ParseIntervalType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("INTERVAL", L"INTERVAL", 8) == false)
        return false;

	STATS_DECL

 	STATS_ITM_DECL
    DTYPE_STATS(name)

    bool start = false;

    Token *first_unit = GetNextToken();
    Token *first_year = NULL;
    Token *first_month = NULL;
    Token *first_day = NULL;
    Token *first_hour = NULL;
    Token *first_minute = NULL;
    Token *first_second = NULL;
    Token *first_fraction = NULL;
    Token *to = NULL;
    Token *second_unit = NULL;

    if(Token::Compare(first_unit, "YEAR", L"YEAR", 4) == true)
    {
        first_year = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "MONTH", L"MONTH", 5) == true)
    {
        first_month = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "DAY", L"DAY", 3) == true)
    {
        first_day = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "HOUR", L"HOUR", 4) == true)
    {
        first_hour = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "MINUTE", L"MINUTE", 6) == true)
    {
        first_minute = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "SECOND", L"SECOND", 6) == true)
    {
        first_second = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "FRACTION", L"FRACTION", 8) == true)
    {
        first_fraction = first_unit;
        start = true;
    }
    else
        PushBack(first_unit);

    Token *open = NULL;
    Token *close = NULL;

    if(start == true)
    {
        open = GetNextCharToken('(', L'(');

        // Precision is optional in Oracle and Informix
        if(open != NULL)
        {
            /*Token *precision */ (void) GetNextNumberToken();
            close = GetNextCharToken(')', L')');
        }

        to = GetNextWordToken("TO", L"TO", 2);

        // Second unit is optional in PostgreSQL
        if(to != NULL)
            second_unit = GetNextToken();
    }

    Token *open2 = GetNextCharToken('(', L'(');
    Token *scale = NULL;
    Token *close2 = NULL;

    // Scale is optional
    if(open2 != NULL)
    {
        scale = GetNextNumberToken();
        close2 = GetNextCharToken(')', L')');
    }

    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
    {
        // Convert INTERVAL in PostgreSQL to INTERVAL DAY(5) TO SECOND in Oracle
        if(start == false)
            Append(name, " DAY(5) TO SECOND", L" DAY(5) TO SECOND", 17);
        else
        // When second unit is not specified (for PostgreSQL) 
        // or first unit is equal to second unit (for Informix) convert to NUMBER(5) in Oracle
        if(to == NULL || Token::Compare(first_unit, second_unit) == true)
        {
            Token::Remove(name);
            Token::Remove(to, second_unit);
            Token::Change(first_unit, "NUMBER", L"NUMBER", 6);

            if(open == NULL)
            {		
                if(open2 != NULL)
                    Prepend(scale, "5,", L"5,", 2);
                else
                    Append(first_unit, "(5)", L"(5)", 3);
            }
        }
        else
        // If second unit is specified convert to INTERVAL DAY TO SECOND in Oracle, except INTERVAL YEAR TO MONTH
        if(to != NULL && first_year == NULL)
        {
            Token::Change(first_unit, "DAY", L"DAY", 3);

            // If precision is not specified for Informix
            if(open == NULL)
                Append(first_unit, "(5)", L"(5)", 3);

            Token::Change(second_unit, "SECOND", L"SECOND", 6);
        }
    }
    else
    // Convert to VARHCAR(30) in SQL Server, MySQL
    if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
    {
        Token::Remove(first_unit, second_unit);
        Token::Remove(open2, close2);
        Token::Change(name, "VARCHAR(30)", L"VARCHAR(30)", 11);

        STATS_ITM_CONV_ERROR 
    }
    else
    if(_target == SQL_POSTGRESQL && _source != SQL_POSTGRESQL)
    {
        // Remove precision for PostgreSQL
        if(open != NULL)
            Token::Remove(open, close);

        // When first unit is equal to second unit (for Informix)
        // or first unit is SECOND and second unit is FRACTION (for Informix) remove second unit in PostgreSQL
        if(Token::Compare(first_unit, second_unit) == true ||
            (Token::Compare(first_unit, "SECOND", L"SECOND", 6) == true &&
            Token::Compare(second_unit, "FRACTION", L"FRACTION", 8) == true))
            Token::Remove(to, second_unit);

        // Change FRACTION in Informix to SECOND in PostgreSQL 
        if(Token::Compare(first_unit, "FRACTION", L"FRACTION", 8) == true)
            Token::Change(first_unit, "SECOND", L"SECOND", 6);
        else
            if(Token::Compare(second_unit, "FRACTION", L"FRACTION", 8) == true)
                Token::Change(second_unit, "SECOND", L"SECOND", 6);
    }
    else
    // Netezza parser units but throws runtime errors
    if(_target == SQL_NETEZZA && _source != SQL_NETEZZA)
    {
        Token::Remove(first_unit, close);
        Token::Remove(to);
        Token::Remove(second_unit);
    }
    else
    // EsgynDB supports Oracle INTERVAL syntax
    if(_target == SQL_ESGYNDB && _source != SQL_ESGYNDB)
        STATS_ITM_CONV_NO_NEED

    DTYPE_DTL_STATS_L(name)

    return true;
}

// INT in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseIntType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    bool int_ = false;
    bool int_in_braces = false;
    bool conv = false;

    if(name->Compare("INT", L"INT", 3) == true || name->Compare("INTEGER", L"INTEGER", 7) == true)
        int_ = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[INT]", L"[INT]", 5) == true || name->Compare("[INTEGER]", L"[INTEGER]", 9) == true)
        int_in_braces = true;

    if(int_ == false && int_in_braces == false)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_INT_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    Token *open = GetNextCharToken('(', L'(');

    // MySQL INT can include display size
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for INT
        if(_target != SQL_MYSQL)
        {
            Token::Remove(open, close);
            conv = true;
        }
    }

	// Use Integer class instead of primitive int type in Java to handle NULLs
	if(_target_app == APP_JAVA && (clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS ||
			clause_scope == SQL_SCOPE_VAR_DECL))
	{
		TOKEN_CHANGE_NOFMT(name, "Integer");
		conv = true;
	}
	else
    // Convert to NUMBER(10) in Oracle
    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
    {
        Token::Change(name, "NUMBER", L"NUMBER", 6);
        conv = true;

        // Not in parameters
        if(clause_scope != SQL_SCOPE_FUNC_PARAMS && clause_scope != SQL_SCOPE_PROC_PARAMS)
        {
            // Add size as separate tokens as it may be modified later
            AppendNoFormat(name, "(", L"(", 1);
            AppendNoFormat(name, "10", L"10", 2);
            AppendNoFormat(name, ")", L")", 1);
        }
    }
    else
    // Convert Oracle INTEGER to DECIMAL(38) in other databases
    if(_source == SQL_ORACLE && _target != SQL_ORACLE)
    {
        //Token::Change(name, "DECIMAL(38)", L"DECIMAL(38)", 11);
    }
    // Remove [] for other databases
    else
    if(_target != SQL_SQL_SERVER && int_in_braces == true)
    {
        Token::ChangeNoFormat(name, name, 1, name->len - 2);
        conv = true;
    }

    if(!conv)
	{
		STATS_DTL_CONV_NO_NEED(true)
        STATS_ITM_CONV_NO_NEED
	}

	name->data_type = TOKEN_DT_NUMBER;
	name->data_subtype = TOKEN_DT2_INT;

	STATS_UPDATE_STATUS

	if(int_in_braces)
	{
		DTYPE_STATS_V("INT", name)
		DTYPE_DTL_STATS_ST("INT", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// INT1 in MySQL
bool SqlParser::ParseInt1Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("INT1", L"INT1", 4) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // MySQL INT1 can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for INT1
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(3) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(3)", L"NUMBER(3)", 9);
    else
        // Convert to SMALLINT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "SMALLINT", L"SMALLINT", 8);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// INT2 in MySQL, PostreSQL
bool SqlParser::ParseInt2Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("INT2", L"INT2", 4) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // MySQL INT2 can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for INT2
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(5) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(5)", L"NUMBER(5)", 9);
    else
        // Convert to SMALLINT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "SMALLINT", L"SMALLINT", 8);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// INT3 in MySQL
bool SqlParser::ParseInt3Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("INT3", L"INT3", 4) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // MySQL INT3 can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for INT3
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(7) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(7)", L"NUMBER(7)", 9);
    else
        // Convert to INT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "INT", L"INT", 3);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// INT4 in MySQL, PostreSQL
bool SqlParser::ParseInt4Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("INT4", L"INT4", 4) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // MySQL INT4 can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for INT4
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(10) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(10)", L"NUMBER(10)", 10);
    else
        // Convert to INT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "INT", L"INT", 3);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// INT8 in MySQL, PostreSQL, Informix
bool SqlParser::ParseInt8Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("INT8", L"INT8", 4) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // MySQL INT8 can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for INT8
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(19) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(19)", L"NUMBER(19)", 10);
    else
        // Convert to BIGINT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "BIGINT", L"BIGINT", 6);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// LONG in Oracle;
// LONG BINARY in Sybase ASA;
// LONG BIT VARYING in Sybase ASA;
// LONG NVARCHAR in Sybase ASA;
// LONG RAW in Oracle;
// LONG VARBINARY in MySQL;
// LONG VARBIT in Sybase ASA;
// LONG VARCHAR in MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseLongType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("LONG", L"LONG", 4) == false)
        return false;

	STATS_DECL
	STATS_DTL_DECL
	STATS_ITM_DECL 

	bool conv = false;

    Token *unit = GetNextToken();

    Token *binary = NULL;
    Token *bit = NULL;
    Token *nvarchar = NULL;
    Token *raw = NULL;
    Token *varbinary = NULL;
    Token *varbit = NULL;
    Token *varchar = NULL;
    Token *varying = NULL;

    if(Token::Compare(unit, "BINARY", L"BINARY", 6) == true)
        binary = unit;
    else
    if(Token::Compare(unit, "BIT", L"BIT", 3) == true)
    {
        bit = unit;
        varying = GetNextWordToken("VARYING", L"VARYING", 7);
    }
    else
    if(Token::Compare(unit, "NVARCHAR", L"NVARCHAR", 8) == true)
        nvarchar = unit;
    else
    if(Token::Compare(unit, "RAW", L"RAW", 3) == true)
        raw = unit;
    else
    if(Token::Compare(unit, "VARBINARY", L"VARBINARY", 9) == true)
        varbinary = unit;
    else
    if(Token::Compare(unit, "VARBIT", L"VARBIT", 6) == true)
        varbit = unit;
    else
    if(Token::Compare(unit, "VARCHAR", L"VARCHAR", 7) == true)
        varchar = unit;
    else
    {
        PushBack(unit);
        unit = NULL;
    }
    
    // LONG in Oracle
    if(unit == NULL)
    {
		STATS_SET_DESC(SQL_DATATYPE_LONG_DESC)

		// Convert to CLOB in Oracle
        if(_target == SQL_ORACLE && _source != SQL_ORACLE)
			Token::Change(name, "CLOB", L"CLOB", 4);
        else
        // Convert to VARCHAR(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "VARCHAR", L"VARCHAR", 7);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
        else
        // Convert to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MARIADB_ORA, SQL_MYSQL) && _source != SQL_MYSQL)
            Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
        else
        // Convert to TEXT in PostgreSQL, Greenplum
        if(Target(SQL_POSTGRESQL, SQL_GREENPLUM) == true)
            Token::Change(name, "TEXT", L"TEXT", 4);

		conv = true;
    }
    else
    // LONG BINARY in Sybase ASA
    if(binary != NULL)
    {
        // Convert to BLOB in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(unit, "BLOB", L"BLOB", 4);
        }
        else
            // Convert to VARCHAR(max) in SQL Server
            if(_target == SQL_SQL_SERVER)
            {
                Token::Remove(name);
                Token::Change(unit, "VARBINARY", L"VARBINARY", 9);
                AppendNoFormat(unit, "(max)", L"(max)", 5);
            }
            else
                // Convert to BYTEA in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                {
                    Token::Remove(name);
                    Token::Change(unit, "BYTEA", L"BYTEA", 5);
                }
    }
    else
    // LONG BIT VARYING in Sybase ASA
    if(bit != NULL && varying != NULL)
    {
        // Convert to BLOB in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name, unit);
            Token::Change(varying, "BLOB", L"BLOB", 4);
        }
        else
            // Convert to VARBINARY(max) in SQL Server
            if(_target == SQL_SQL_SERVER)
            {
                Token::Remove(name, unit);
                Token::Change(varying, "VARBINARY", L"VARBINARY", 9);
                AppendNoFormat(varying, "(max)", L"(max)", 5);
            }
            else
                // Convert to BYTEA in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                {
                    Token::Remove(name, unit);
                    Token::Change(varying, "BYTEA", L"BYTEA", 5);
                }
    }
    else
    // LONG NVARCHAR in Sybase ASA
    if(nvarchar != NULL)
    {
        // Convert to NCLOB in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(unit, "NCLOB", L"NCLOB", 5);
        }
        else
            // Convert to NVARCHAR(max) in SQL Server
            if(_target == SQL_SQL_SERVER)
            {
                Token::Remove(name);
                Token::Change(unit, "NVARCHAR", L"NVARCHAR", 8);
                AppendNoFormat(unit, "(max)", L"(max)", 5);
            }
            else
                // Convert to TEXT in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                {
                    Token::Remove(name);
                    Token::Change(unit, "TEXT", L"TEXT", 4);
                }
    }
    else
    // LONG RAW in Oracle
    if(raw != NULL)
    {
		STATS_SET_DESC(SQL_DATATYPE_LONG_RAW_DESC)

		Token *open = TOKEN_GETNEXT('(');

		// Size is supported in LONG RAW(n) in PL/SQL, but not in CREATE TABLE DDL in Oracle
		Token *size = GetNext(open);

		Token *close = TOKEN_GETNEXTP(size, ')');

        // Convert to VARBINARY(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Remove(name);
            Token::Change(unit, "VARBINARY", L"VARBINARY", 9);
            AppendNoFormat(unit, "(max)", L"(max)", 5);

			conv = true;
        }
        else
        // Convert to LONGBLOB in MySQL
        if(Target(SQL_MARIADB, SQL_MARIADB_ORA, SQL_MYSQL))
        {
			TOKEN_CHANGE(name, "LONGBLOB");

            Token::Remove(unit);
			Token::Remove(open, close);

			conv = true;
        }
        else
        // Convert to BYTEA in PostgreSQL
        if(_target == SQL_POSTGRESQL)
        {
            Token::Remove(name);
            Token::Change(unit, "BYTEA", L"BYTEA", 5);

			conv = true;
        }
    }
    else
    // LONG VARBINARY in MySQL
    if(varbinary != NULL)
    {
        // Convert to BLOB in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(unit, "BLOB", L"BLOB", 4);
        }
        else
            // Convert to VARBINARY(max) in SQL Server
            if(_target == SQL_SQL_SERVER)
            {
                Token::Remove(name);
                Token::Change(unit, "VARBINARY", L"VARBINARY", 9);
                AppendNoFormat(unit, "(max)", L"(max)", 5);
            }
    }
    else
    // LONG VARBIT in Sybase ASA
    if(varbit != NULL)
    {
        // Convert to BLOB in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(varbit, "BLOB", L"BLOB", 4);
        }
        else
            // Convert to VARBINARY(max) in SQL Server
            if(_target == SQL_SQL_SERVER)
            {
                Token::Remove(name);
                Token::Change(unit, "VARBINARY", L"VARBINARY", 9);
                AppendNoFormat(unit, "(max)", L"(max)", 5);
            }
            else
                // Convert to BYTEA in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                {
                    Token::Remove(name);
                    Token::Change(unit, "BYTEA", L"BYTEA", 5);
                }
    }
    else
        // LONG VARCHAR in MySQL, Sybase ASE, Sybase ASA
        if(varchar != NULL)
        {
            // Convert to CLOB in Oracle
            if(_target == SQL_ORACLE)
            {
                Token::Remove(name);
                Token::Change(unit, "CLOB", L"CLOB", 4);
            }
            else
                // Convert to VARCHAR(max) in SQL Server
                if(_target == SQL_SQL_SERVER)
                {
                    Token::Remove(name);
                    Token::Change(unit, "VARCHAR", L"VARCHAR", 7);
                    AppendNoFormat(unit, "(max)", L"(max)", 5);
                }
                else
                    // Convert to TEXT in PostgreSQL
                    if(_target == SQL_POSTGRESQL)
                    {
                        Token::Remove(name);
                        Token::Change(unit, "TEXT", L"TEXT", 4);
                    }
        }


	if(conv)
	{
		STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
		STATS_ITM_CONV_OK
	}

	STATS_UPDATE_STATUS

	if(unit != NULL)
	{
		TokenStr dt;
		dt.Append(name, true);
		APPENDSTR(dt, " ");
		dt.Append(unit, true);

		DTYPE_STATS_V(dt.GetCStr(), name)
	}
	else
		DTYPE_STATS(name)	

    DTYPE_DTL_STATS_L(name)

    return true;
}

// LOGICAL in Sybase ADS
bool SqlParser::ParseLogicalType(Token *name)
{
    if(name == NULL || _source != SQL_SYBASE_ADS)
        return false;

    if(!TOKEN_CMP(name, "LOGICAL"))
        return false;

    if(_target == SQL_SQL_SERVER)
        TOKEN_CHANGE(name, "BIT");
	else
    if(_target != SQL_SYBASE_ADS)
        TOKEN_CHANGE(name, "CHAR(1)");
                
	STATS_DECL

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

	name->data_subtype = TOKEN_DT2_BOOL;

    return true;
}

// LONGBLOB in MySQL
bool SqlParser::ParseLongblobType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("LONGBLOB", L"LONGBLOB", 8) == false)
        return false;

	STATS_DECL

    // Convert to BLOB in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "BLOB", L"BLOB", 4);
    else
        // Convert to VARBINARY(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "VARBINARY", L"VARBINARY", 9);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
                
    STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// LVARCHAR in Informix
bool SqlParser::ParseLvarcharType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("LVARCHAR", L"LVARCHAR", 8) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Num is optional in Informix
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to VARCHAR2 in Oracle
    if(_target == SQL_ORACLE)
    {
        // Default size 2048 is allowed in Informix
        if(open == NULL)
            Token::Change(name, "VARCHAR2(2048)", L"VARCHAR2(2048)", 14);
        else
            Token::Change(name, "VARCHAR2", L"VARCHAR2", 8);
    }
    else
        // Convert to VARCHAR in other databases
        if(_target != SQL_INFORMIX)
        {
            // Default size 2048 is allowed in Informix
            if(open == NULL)
                Token::Change(name, "VARCHAR(2048)", L"VARCHAR(2048)", 13);
            else
                Token::Change(name, "VARCHAR", L"VARCHAR", 7);
        }

		STATS_ITM_DECL 
        DTYPE_DTL_STATS_L(name)

        return true;
}

// MEDIUMBLOB in MySQL
bool SqlParser::ParseMediumblobType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("MEDIUMBLOB", L"MEDIUMBLOB", 10) == false)
        return false;

	STATS_DECL

    // Convert to BLOB in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "BLOB", L"BLOB", 4);
    else
        // Convert to VARBIANRY(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "VARBINARY", L"VARBINARY", 9);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
    
	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// LONGTEXT in MySQL
bool SqlParser::ParseLongtextType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("LONGTEXT", L"LONGTEXT", 8) == false)
        return false;

	STATS_DECL

    // Convert to VARCHAR(max) in SQL Server
    if(_target == SQL_SQL_SERVER)
    {
        Token::Change(name, "VARCHAR", L"VARCHAR", 7);
        AppendNoFormat(name, "(max)", L"(max)", 5);
    }
    else
        // Convert to CLOB in Oracle and DB2
        if(_target == SQL_ORACLE || _target == SQL_DB2)
            Token::Change(name, "CLOB", L"CLOB", 4);
        else
            // Convert to LONGTEXT in MySQL
            if(_source != SQL_MYSQL && Target(SQL_MARIADB, SQL_MYSQL))
                Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
    
	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// MEDIUMINT in MySQL
bool SqlParser::ParseMediumintType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("MEDIUMINT", L"MEDIUMINT", 9) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // MySQL MEDIUMINT can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *number */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for MEDIUMINT
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(7) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(7)", L"NUMBER(7)", 9);
    else
        // Convert to INT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "INT", L"INT", 3);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// MEDIUMTEXT in MySQL
bool SqlParser::ParseMediumtextType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("MEDIUMTEXT", L"MEDIUMTEXT", 10) == false)
        return false;

	STATS_DECL

    // Convert to VARCHAR(max) in SQL Server
    if(_target == SQL_SQL_SERVER)
    {
        Token::Change(name, "VARCHAR", L"VARCHAR", 7);
        AppendNoFormat(name, "(max)", L"(max)", 5);
    }
    else
        // Convert to CLOB in Oracle and DB2
        if(_target == SQL_ORACLE || _target == SQL_DB2)
            Token::Change(name, "CLOB", L"CLOB", 4);
        else
            // Convert to LONGTEXT in MySQL
            if(_source != SQL_MYSQL && Target(SQL_MARIADB, SQL_MYSQL))
                Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
        
	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// MIDDLEINT in MySQL
bool SqlParser::ParseMiddleintType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("MIDDLEINT", L"MIDDLEINT", 9) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // MySQL MIDDLEINT can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for MIDDLEINT
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(7) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(7)", L"NUMBER(7)", 9);
    else
        // Convert to INT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "INT", L"INT", 3);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// MONEY in SQL Server, PostreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseMoneyType(Token *name)
{
    if(name == NULL)
        return false;

    bool money = false;
    bool money_in_braces = false;
    bool money_in_quotes = false;

	STATS_DECL

    if(name->Compare("MONEY", L"MONEY", 5) == true)
        money = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[MONEY]", L"[MONEY]", 7) == true)
            money_in_braces = true;
        else
            // Can be enclosed in "" for Sybase ASA
            if(name->Compare("\"MONEY\"", L"\"MONEY\"", 7) == true)
                money_in_quotes = true;

    if(money == false && money_in_braces == false && money_in_quotes == false)
        return false;

    Token *open = GetNextCharToken('(', L'(');
    Token *close = NULL;

    // Precision and scale are optional in Informix
    if(open != NULL)
    {
        Token *precision = GetNextNumberToken();
        Token *comma = GetNextCharToken(',', L',');

        if(comma != NULL)
            /*Token *scale */ (void) GetNextNumberToken();
        else
            // Scale is 2 by default in Informix
            if(_source == SQL_INFORMIX && _target != SQL_INFORMIX)
                Append(precision, ",2", L",2", 2);

        close = GetNextCharToken(')', L')');
    }

    // Convert to NUMBER in Oracle
    if(_target == SQL_ORACLE)
    {
        Token::Change(name, "NUMBER", L"NUMBER", 6);

        // Range (17,2) is allowed in PostgreSQL
        if(_source == SQL_POSTGRESQL)
            Append(name, "(17,2)", L"(17,2)", 6);
        else
            // Range (16,2) is allowed in Informix
            if(_source == SQL_INFORMIX && open == NULL)
                Append(name, "(16,2)", L"(16,2)", 6);
            else
                // Range (15,4) is allowed in Sybase ASE
                if(_source == SQL_SYBASE)
                    Append(name, "(15,4)", L"(15,4)", 6);
                else
                    // Range (19,2) is allowed in Sybase ASA
                    if(_source == SQL_SYBASE_ASA)
                        Append(name, "(19,4)", L"(19,4)", 6);
    }
    else
        if(Target(SQL_SQL_SERVER, SQL_POSTGRESQL) == true)
        {
            // Remove ""
            if(money_in_quotes == true)
                Token::ChangeNoFormat(name, name, 1, name->len - 2);

            if(_source == SQL_INFORMIX)
                Token::Remove(open, close);
        }
        else
            // Convert to DECIMAL in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL))
            {
                Token::Change(name, "DECIMAL", L"DECIMAL", 7);

                // Range (15,4) is allowed in SQL Server
                if(_source == SQL_SQL_SERVER)
                    Append(name, "(15,4)", L"(15,4)", 6);
            }
            else
                // Remove [] for other databases
                if(_target != SQL_SQL_SERVER && money_in_braces == true)
                    Token::ChangeNoFormat(name, name, 1, name->len - 2);
                else
                    // Remove "" for other databases
                    if(_target != SQL_SYBASE_ASA && money_in_quotes == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(money_in_braces)
	{
		DTYPE_STATS_V("MONEY", name)
		DTYPE_DTL_STATS_ST("MONEY", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// NATIONAL CHAR, NATIONAL CHARACTER in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA;
// NATIONAL CHAR VARYING, NATIONAL CHARACTER VARYING in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA;
// NATIONAL TEXT in SQL Server;
// NATIONAL VARCHAR in MySQL
bool SqlParser::ParseNationalType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("NATIONAL", L"NATIONAL", 8) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *unit = GetNextToken();
    Token *character = NULL;
    Token *text = NULL;
    Token *varying = NULL;
    Token *varchar = NULL;

    if(Token::Compare(unit, "CHAR", L"CHAR", 4) == true ||
        Token::Compare(unit, "CHARACTER", L"CHARACTER", 9) == true)
    {
        character = unit;
        varying = GetNextWordToken("VARYING", L"VARYING", 7);
    }
    else
        if(Token::Compare(unit, "TEXT", L"TEXT", 4) == true)
            text = unit;
        else
            if(Token::Compare(unit, "VARCHAR", L"VARCHAR", 7) == true)
                varchar = unit;

    Token *open = GetNextCharToken('(', L'(');
    Token *size = NULL;
    Token *close = NULL;

    // Size is optional
    if(open != NULL)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();
        close = GetNextCharToken(')', L')');
    }

    // Check for NATIONAL TEXT
    if(text != NULL)
    {
        // Convert to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Remove(name);
            Token::Change(unit, "LONGTEXT", L"LONGTEXT", 8);
        }
        else
            // Convert to TEXT in PostgreSQL
            if(_target == SQL_POSTGRESQL)
            {
                Token::Remove(name);
                Token::Change(unit, "TEXT", L"TEXT", 4);
            }
            else
                // Convert to NVARCHAR(max) is SQL Server
                if(_target == SQL_SQL_SERVER)
                {
                    Token::Remove(name);
                    Token::Change(unit, "NVARCHAR", L"NVARCHAR", 8);
                    AppendNoFormat(unit, "(max)", L"(max)", 5);
                }
    }
    else
        // Check for NATIONAL CHARACTER VARYING and NATIONAL CHAR VARYING
        if(varying != NULL)
        {
            // If MAX is specified, convert to CLOB types
            if(size->Compare("MAX", L"MAX", 3) == true)
            {
                // Change to LONGTEXT in MySQL
                if(Target(SQL_MARIADB, SQL_MYSQL))
                {
                    Token::Remove(name, character);
                    Token::Change(varying, "LONGTEXT", L"LONGTEXT", 8);
                    Token::Remove(open, close);
                }
                else
                    // Change to TEXT in PostgreSQL
                    if(_target == SQL_POSTGRESQL)
                    {
                        Token::Remove(name, character);
                        Token::Change(varying, "TEXT", L"TEXT", 4);
                        Token::Remove(open, close);
                    }
            }
            else
                // Convert NATIONAL CHARACTER VARYING and NATIONAL CHAR VARYING to NVARCHAR2 in Oracle
                if(_target == SQL_ORACLE && _source != SQL_ORACLE)
                {
                    Token::Remove(name, character);
                    Token::Change(varying, "NVARCHAR2", L"NVARCHAR2", 9);

                    // Default size
                    if(open == NULL)
                    {
                        // Size is 1 by default in SQL Server, Informix, Sybase ASE, Sybase ASA
                        if(Source(SQL_SQL_SERVER, SQL_INFORMIX, SQL_SYBASE, SQL_SYBASE_ASA) == true)
                            Append(varying, "(1)", L"(1)", 3);
                    }
                }
                else
                    // Convert to CHARACTER VARYING and CHAR VARYING in PostgreSQL
                    if(_target == SQL_POSTGRESQL)
                    {
                        Token::Remove(name);

                        // CHAR[ACTER] VARYING is unlimited by default in PostgreSQL
                        if(open == NULL)
                            Append(varying, "(1)", L"(1)", 3);
                    }
                    else
                        // Size is mandatory in MySQL
                        if(Target(SQL_MARIADB, SQL_MYSQL) && open == NULL)		
                            Append(varying, "(1)", L"(1)", 3);
        }
        else
            // Check for NATIONAL CHARACTER and NATIONAL CHAR
            if(varying == NULL && varchar == NULL)
            {
                // Convert to CHARACTER and CHAR in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                    Token::Remove(name);
                else
                    // If size is greater than 255 convert to NVARCHAR in MySQL
                    if(Target(SQL_MARIADB, SQL_MYSQL))
                    {
                        int num = 1;

                        if(size != NULL)
                            num = size->GetInt();

                        if(num > 255)
                        {
                            Token::Change(unit, "NVARCHAR", L"NVARCHAR", 8);
                            Token::Remove(name);
                        }
                    }
            }
            else
                // Check for NATIONAL VARCHAR
                if(varchar != NULL)
                {
                    // Convert to NVARCHAR2 in Oracle
                    if(_target == SQL_ORACLE)
                    {
                        Token::Remove(name);
                        Token::Change(unit, "NVARCHAR2", L"NVARCHAR2", 9);
                    }
                    else
                        // Convert to NVARCHAR in SQL Server
                        if(_target == SQL_SQL_SERVER)
                        {
                            Token::Remove(name);
                            Token::Change(unit, "NVARCHAR", L"NVARCHAR", 8);
                        }
                }

				STATS_ITM_DECL 
                DTYPE_DTL_STATS_L(name)

                return true;
}

// NCHAR and NCHAR VARYING in Oracle, SQL Server, DB2, MySQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseNcharType(Token *name)
{
    if(name == NULL)
        return false;

    bool nchar = false;
    bool nchar_in_braces = false;

	bool conv_no_need = false;
	bool conv = false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_NCHAR_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    if(name->Compare("NCHAR", L"NCHAR", 5) == true)
        nchar = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[NCHAR]", L"[NCHAR]", 7) == true)
        nchar_in_braces = true;

    if(nchar == false && nchar_in_braces == false)
        return false;

    Token *varying = GetNextWordToken("VARYING", L"VARYING", 7);

    Token *open = GetNextCharToken('(', L'(');
    Token *size = NULL;
    Token *close = NULL;

    // Num is optional
    if(open != NULL)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();
        close = GetNextCharToken(')', L')');
    }

    // If MAX is specified, convert to CLOB types
    if(TOKEN_CMP(size, "MAX"))
    {
        // Change to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Remove(name);
            Token::Change(varying, "LONGTEXT", L"LONGTEXT", 8);
            Token::Remove(open, close);
        }
        else
        // Change to TEXT in PostgreSQL
        if(_target == SQL_POSTGRESQL)
        {
            Token::Remove(name);
            Token::Change(varying, "TEXT", L"TEXT", 4);
            Token::Remove(open, close);
        }
    }
    else
    // Check for NCHAR VARYING
    if(varying != NULL)
    {
        // Convert NCHAR VARYING to NVARCHAR2 in Oracle
        if(_target == SQL_ORACLE && _source != SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(varying, "NVARCHAR2", L"NVARCHAR2", 9);

            // Default size
            if(open == NULL)
            {
                // Size is 1 by default in SQL Server, Informix, Sybase ASE, Sybase ASA
                if(Source(SQL_SQL_SERVER, SQL_INFORMIX, SQL_SYBASE, SQL_SYBASE_ASA) == true)
                    Append(varying, "(1)", L"(1)", 3);
            }
        }
        else
        // Convert NCHAR VARYING to VARCHAR in PostgreSQL
        if(_target == SQL_POSTGRESQL)
        {
            Token::Remove(name);
            Token::Change(varying, "VARCHAR", L"VARCHAR", 7);

            // VARCHAR is unlimited by default in PostgreSQL
            if(open == NULL)
                Append(varying, "(1)", L"(1)", 3);
        }
        else
        // Size is mandatory in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL) && open == NULL)		
            Append(varying, "(1)", L"(1)", 3);
    }
    else
    // Check for NCHAR
    if(varying == NULL && _target != SQL_SQL_SERVER)
    {
        // Remove []
        if(nchar_in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);

        // Convert to CHAR in PostgreSQL
        if(_target == SQL_POSTGRESQL)
            Token::Change(name, "CHAR", L"CHAR", 4);
        else
        // If size is greater than 255 convert to NVARCHAR in MySQL and MariaDB
        if(Target(SQL_MARIADB, SQL_MARIADB_ORA, SQL_MYSQL))
        {
            int num = 1;

            if(size != NULL)
                num = size->GetInt();

            if(num > 255)
			{
                TOKEN_CHANGE(name, "NVARCHAR");
				conv = true;
			}
			else
				conv_no_need = true;
        }
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && nchar_in_braces == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	if(conv)
	{
		STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
	}
	else
	if(conv_no_need)
	{
		STATS_DTL_CONV_NO_NEED(true)  
		STATS_ITM_CONV_NO_NEED
	}

	STATS_UPDATE_STATUS

	if(nchar_in_braces)
	{
		DTYPE_STATS_V("NCHAR", name)
		DTYPE_DTL_STATS_ST("NCHAR", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// NCLOB in Oracle, DB2
bool SqlParser::ParseNclobType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("NCLOB", L"NCLOB", 5) == false)
        return false;

	bool conv_no_need = false;
	bool conv = false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_NCLOB_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    Token *open = GetNextCharToken('(', L'(');

    // Num and semantics are optional in DB2
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextToken();
        Token *semantics = GetNextToken();

        // DB2 allows specifying size (1024, 1M etc.)
        if(Token::Compare(semantics, "K", L"K", 1) == false &&
            Token::Compare(semantics, "M", L"M", 1) == false &&
            Token::Compare(semantics, "G", L"G", 1) == false)
        {
            PushBack(semantics);
            semantics = NULL;
        }

        Token *close = GetNextCharToken(')', L')');

        // If target is not DB2 remove semantics attribute
        if(_target != SQL_DB2)
            Token::Remove(open, close);
    }

    // Convert to NVARCHAR(max) in SQL Server
    if(_target == SQL_SQL_SERVER)
    {
        Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);
        AppendNoFormat(name, "(max)", L"(max)", 5);
		conv = true;
    }
    else
    // Convert to LONGTEXT in MySQL
    if(Target(SQL_MARIADB, SQL_MARIADB_ORA, SQL_MYSQL))
	{
        Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
		conv = true;
	}
    else
    // Convert to TEXT in PostgreSQL
    if(_target == SQL_POSTGRESQL)
	{
        Token::Change(name, "TEXT", L"TEXT", 4);
		conv = true;
	}

	if(conv)
	{
		STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
	}
	else
	if(conv_no_need)
	{
		STATS_DTL_CONV_NO_NEED(true)  
		STATS_ITM_CONV_NO_NEED
	}

	STATS_UPDATE_STATUS

	DTYPE_STATS(name)
	DTYPE_DTL_STATS_L(name)

    return true;
}

// NMEMO in Sybase ADS
bool SqlParser::ParseNmemoType(Token *name)
{
    if(name == NULL || _source != SQL_SYBASE_ADS)
        return false;

    if(!TOKEN_CMP(name, "NMEMO"))
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // Convert to NVARCHAR(max) in SQL Server
    if(_target == SQL_SQL_SERVER)
    {
        Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);
        AppendNoFormat(name, "(max)", L"(max)", 5);
    }
    else
    // Convert to LONGTEXT in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
    else
    // Convert to TEXT in PostgreSQL
    if(_target == SQL_POSTGRESQL)
        Token::Change(name, "TEXT", L"TEXT", 4);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// MEMO in Sybase ADS
bool SqlParser::ParseMemoType(Token *name)
{
    if(name == NULL || _source != SQL_SYBASE_ADS)
        return false;

    if(!TOKEN_CMP(name, "MEMO"))
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // Convert to NVARCHAR(max) in SQL Server
    if(_target == SQL_SQL_SERVER)
    {
        Token::Change(name, "VARCHAR", L"VARCHAR", 7);
        AppendNoFormat(name, "(max)", L"(max)", 5);
    }
    else
    // Convert to LONGTEXT in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
    else
    // Convert to TEXT in PostgreSQL
    if(_target == SQL_POSTGRESQL)
        Token::Change(name, "TEXT", L"TEXT", 4);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// NTEXT in SQL Server, Sybase ASA
bool SqlParser::ParseNtextType(Token *name)
{
    if(name == NULL)
        return false;

    bool ntext = false;
    bool ntext_in_braces = false;

	STATS_DECL

    if(name->Compare("NTEXT", L"NTEXT", 5) == true)
        ntext = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[NTEXT]", L"[NTEXT]", 7) == true)
            ntext_in_braces = true;

    if(ntext == false && ntext_in_braces == false)
        return false;

    // Convert to NCLOB in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NCLOB", L"NCLOB", 5);
    else
        // Convert to NVARCHAR(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
        // Convert to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
            Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
        else
            // Convert to TEXT in PostgreSQL
            if(_target == SQL_POSTGRESQL)
                Token::Change(name, "TEXT", L"TEXT", 4);
            else
                // Remove [] for other database
                if(_target != SQL_SQL_SERVER && ntext_in_braces == true)
                    Token::ChangeNoFormat(name, name, 1, name->len - 2);
            
	STATS_ITM_DECL 

	if(ntext_in_braces)
	{
		DTYPE_STATS_V("NTEXT", name)
		DTYPE_DTL_STATS_ST("NTEXT", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}
    
    return true;
}

// NUMBER in Oracle
bool SqlParser::ParseNumberType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    if(name->Compare("NUMBER", L"NUMBER", 6) == false)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_NUMBER_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    // Precision and scale are optional
    Token *open = GetNextCharToken('(', L'(');
    Token *precision = NULL;
    Token *comma = NULL;
    Token *scale = NULL;
    Token *close = NULL;

    // Get precision and scale;
    if(open != NULL)
    {
        // Precision can be specified as a number or *
        precision = GetNextToken();
        comma = GetNextCharToken(',', L',');

        if(comma != NULL)
            scale = GetNextNumberToken(); 

        close = GetNextCharToken(')', L')');
    }

    // Floating-point number
    if(open == NULL || (precision->Compare('*', L'*') == true && comma == NULL))
    {
        // NUMBER without () in parameters and variables
        if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS ||
            clause_scope == SQL_SCOPE_VAR_DECL)
        {
			if(!_option_oracle_plsql_number_mapping.empty())
			{
				TokenStr number(_option_oracle_plsql_number_mapping);
				Token::ChangeNoFormat(name, number);
			}
			else
			// Use Double class instead of primitive double type in Java to handle NULLs
			if(_target_app == APP_JAVA)
				TOKEN_CHANGE_NOFMT(name, "Double");
			else 
            if(_target == SQL_SQL_SERVER)
                Token::Change(name, "FLOAT", L"FLOAT", 5);
            else
            if(Target(SQL_MARIADB, SQL_MYSQL))
                Token::Change(name, "DOUBLE", L"DOUBLE", 6);
            else
			if(_target != SQL_ORACLE)
                Token::Change(name, "INT", L"INT", 3);
        }
        else
        // Convert to FLOAT in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "FLOAT", L"FLOAT", 5);

            if(open != NULL)
                Token::Remove(open, close);
        }
        else
        // Convert to DOUBLE PRECISION in PostgreSQL
        if(_target == SQL_POSTGRESQL)
        {
            Token::Change(name, "DOUBLE PRECISION", L"DOUBLE PRECISION", 16);

            if(open != NULL)
                Token::Remove(open, close);
        }
        else
        // Convert to DOUBLE for other databases
        if(_target != SQL_ORACLE)
        {
            Token::Change(name, "DOUBLE", L"DOUBLE", 6);

            if(open != NULL)
                Token::Remove(open, close);
        }
    }
    else
    {
        int prec = -1;
        int sc = 0;

        bool prec_asterisk = false;

        if(precision->Compare('*', L'*') == true)
        {
            prec = 38;
            prec_asterisk = true;
        }
        else
            prec = precision->GetInt();

        if(scale != NULL)
            sc = scale->GetInt();

		// Use Integer class instead of primitive int type in Java to handle NULLs
		if(_target_app == APP_JAVA)
			TOKEN_CHANGE_NOFMT(name, "Integer");
		else
        // Convert to other databases
        if(_target != SQL_ORACLE)
        {
            // Check for TINYINT range
            if(prec != -1 && prec < 3 && sc == 0)
            {
                // SQL Server TINYINT is 0 to 255, PostgreSQL does not support TINYINT
                if(Target(SQL_SQL_SERVER, SQL_POSTGRESQL, SQL_GREENPLUM))
                    Token::Change(name, "SMALLINT", L"SMALLINT", 8);
                else
                    Token::Change(name, "TINYINT", L"TINYINT", 7);

                Token::Remove(open, close);
            }
            else
                // Check for SMALLINT range
                if(prec >= 3 && prec < 5 && sc == 0)
                {
                    Token::Change(name, "SMALLINT", L"SMALLINT", 8);
                    Token::Remove(open, close);
                }
                else
                    // INT range
                    if(prec >= 5 && prec < 9 && sc == 0)
                    {
                        Token::Change(name, "INT", L"INT", 3);
                        Token::Remove(open, close);
                    }
                    else
                        if(prec >= 9 && prec < 19 && sc == 0)
                        {
                            // NUMERIC in Netezza
                            if(_target == SQL_NETEZZA)
                                Token::Change(name, "NUMERIC", L"NUMERIC", 7);
                            else
                            {
                                Token::Change(name, "BIGINT", L"BIGINT", 6);
                                Token::Remove(open, close);
                            }
                        }
                        else
                            // DECIMAL
                        {
                            Token::Change(name, "DECIMAL", L"DECIMAL", 7);

                            if(prec_asterisk == true)
                                Token::Change(precision, "38", L"38", 2);
                        }
        }
    }

	name->data_type = TOKEN_DT_NUMBER;

	STATS_DTL_CONV_OK(true, STATS_CONV_MEDIUM, "", "")
	STATS_UPDATE_STATUS

    DTYPE_STATS(name)
    DTYPE_DTL_STATS_L(name)

	return true;
}

// NUMERIC in Oracle, SQL Server, DB2, MySQL, PostreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseNumericType(Token *name)
{
    if(name == NULL)
        return false;

    bool numeric = false;
    bool numeric_in_braces = false;
    bool num = false;

    if(name->Compare("NUMERIC", L"NUMERIC", 7) == true)
        numeric = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[NUMERIC]", L"[NUMERIC]", 9) == true)
            numeric_in_braces = true;
        else
            // NUM in DB2
            if(name->Compare("NUM", L"NUM", 3) == true)
                num = true;

    if(numeric == false && numeric_in_braces == false && num == false)
        return false;

	STATS_DECL

    Token *open = GetNextCharToken('(', L'(');

    // Precision and scale are optional
    if(open != NULL)
    {
        Token *precision = GetNextNumberToken();
        Token *comma = GetNextCharToken(',', L',');
        Token *scale = NULL;

        if(comma != NULL)
            scale = GetNextNumberToken();

        if(scale == NULL)
        {
            // Scale is 6 by default in Sybase ASA
            if(_source == SQL_SYBASE_ASA && _target != SQL_SYBASE_ASA)
                Append(precision, ",6", L",6", 2);
        }

        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Default precision and scale
    if(open == NULL)
    {
        // NUMERIC(38,0) is default in Oracle
        if(_source == SQL_ORACLE && _target != SQL_ORACLE)
            Append(name, "(38,0)", L"(38,0)", 6);
        else
            // NUMERIC(18,0) is default in SQL Server and Sybase ASE
            if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true && Target(SQL_SQL_SERVER, SQL_SYBASE) == false)
                Append(name, "(18,0)", L"(18,0)", 6);
            else
                // NUMERIC(5,0) is default in DB2
                if(_source == SQL_DB2 && _target != SQL_DB2)
                    Append(name, "(5,0)", L"(5,0)", 5);
                else
                    // NUMERIC(10,0) is default in MySQL
                    if(_source == SQL_MYSQL && _target != SQL_MYSQL)
                        Append(name, "(10,0)", L"(10,0)", 6);
                    else
                        // NUMERIC(16,0) is default in Informix
                        if(_source == SQL_INFORMIX && _target != SQL_INFORMIX)
                            Append(name, "(16,0)", L"(16,0)", 6);
                        else
                            // NUMERIC(30,6) is default in Sybase ASA
                            if(_source == SQL_SYBASE_ASA && _target != SQL_SYBASE_ASA)
                                Append(name, "(30,6)", L"(30,6)", 6);
    }

    // Convert to NUMBER in Oracle
    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
        Token::Change(name, "NUMBER", L"NUMBER", 6);
    else
        // Remove [] for other databases
        if(_target != SQL_SQL_SERVER && numeric_in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);
        else
            // If target type is not DB2 change NUM to NUMERIC
            if(_target != SQL_DB2 && num == true)
                Token::Change(name, "NUMERIC", L"NUMERIC", 7);

	STATS_ITM_DECL 
    
	if(numeric_in_braces)
	{
		DTYPE_STATS_V("NUMERIC", name)
		DTYPE_DTL_STATS_ST("NUMERIC", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// NVARCHAR in SQL Server, DB2, MySQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseNvarcharType(Token *name)
{
    if(name == NULL)
        return false;

    bool nvarchar = false;
    bool nvarchar_in_braces = false;

    if(name->Compare("NVARCHAR", L"NVARCHAR", 8) == true)
        nvarchar = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[NVARCHAR]", L"[NVARCHAR]", 10) == true)
            nvarchar_in_braces = true;

    if(nvarchar == false && nvarchar_in_braces == false)
        return false;

	STATS_DECL

    Token *open = GetNextCharToken('(', L'(');
    Token *size = NULL;
    Token *close = NULL;

    // Size is optional for SQL Server, Informix, Sybase ASE, Sybase ASA
    if(open != NULL)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();
        Token *comma = GetNextCharToken(',', L',');

        // For Informix, Reserve can be specified
        if(comma != NULL)
        {
            Token *reserve = GetNextNumberToken();

            // If target is not Informix remove reserve attribute
            if(_target != SQL_INFORMIX)
                Token::Remove(comma, reserve);
        }

        close = GetNextCharToken(')', L')');
    }

    // If MAX is specified, convert to CLOB types
    if(size->Compare("MAX", L"MAX", 3) == true)
    {
        // Change to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
            Token::Remove(open, close);
        }
        else
            // Change to TEXT in PostgreSQL
            if(_target == SQL_POSTGRESQL)
            {
                Token::Change(name, "TEXT", L"TEXT", 4);
                Token::Remove(open, close);
            }
    }
    else
        // Convert to NVARCHAR2 in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Change(name, "NVARCHAR2", L"NVARCHAR2", 9);

            // Default size
            if(open == NULL)
            {
                // Size is 1 by default in SQL Server, Informix, Sybase ASE, Sybase ASA
                if(Source(SQL_SQL_SERVER, SQL_INFORMIX, SQL_SYBASE, SQL_SYBASE_ASA) == true)
                    Append(name, "(1)", L"(1)", 3);
            }
        }
        else
            // Convert to VARCHAR in PostgreSQL
            if(_target == SQL_POSTGRESQL)
            {
                Token::Change(name, "VARCHAR", L"VARCHAR", 7);

                // VARCHAR is unlimited by default in PostgreSQL
                if(open == NULL)
                    Append(name, "(1)", L"(1)", 3);
            }
            else
                // Size is mandatory in MySQL
                if(Target(SQL_MARIADB, SQL_MYSQL) && open == NULL)
                {
                    if(nvarchar_in_braces == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);

                    Append(name, "(1)", L"(1)", 3);
                }
                else
                    // Remove [] for other databases
                    if(_target != SQL_SQL_SERVER && nvarchar_in_braces == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);

    name->data_type = TOKEN_DT_STRING;

	STATS_ITM_DECL 

	if(nvarchar_in_braces)
	{
		DTYPE_STATS_V("NVARCHAR", name)
		DTYPE_DTL_STATS_ST("NVARCHAR", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// NVARCHAR2 in Oracle
bool SqlParser::ParseNvarchar2Type(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    if(name->Compare("NVARCHAR2", L"NVARCHAR2", 9) == false)
        return false;

    bool conv_no_need = false;
	bool conv = false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_NVARCHAR2_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    // Size is mandatory in Oracle
    /*Token *open */ (void) GetNextCharToken('(', L'(');
    /*Token *size */ (void) GetNextNumberToken();
    /*Token *close */ (void) GetNextCharToken(')', L')');

    if(_target != SQL_ORACLE)
    {
        // Convert to VARCHAR in PostgreSQL
        if(_target == SQL_POSTGRESQL)
            Token::Change(name, "VARCHAR", L"VARCHAR", 7);
        else
            // Convert to NVARCHAR in other databases
            Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);

		// When used in parameter list for a function or procedure add length
        if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS)
			APPEND_NOFMT(name, "(2000)");

		conv = true;
    }
	else
		conv_no_need = true;

	if(conv)
	{
		STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
	}
	else
	if(conv_no_need)
	{
		STATS_DTL_CONV_NO_NEED(true)  
		STATS_ITM_CONV_NO_NEED
	}

	STATS_UPDATE_STATUS

	DTYPE_STATS(name)
	DTYPE_DTL_STATS_L(name)

    return true;
}

// RAW in Oracle
bool SqlParser::ParseRawType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("RAW", L"RAW", 3) == false)
        return false;

	STATS_DECL
	STATS_DTL_DECL
	STATS_SET_DESC(SQL_DATATYPE_RAW_DESC)

	STATS_ITM_DECL 

    // Size is mandatory in Oracle, but can be skipped for PL/SQL parameters
    Token *open = TOKEN_GETNEXT('(');
    Token *size = (open != NULL) ? GetNextNumberToken() : NULL;
    Token *close = TOKEN_GETNEXTP(size, ')');

    if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL))
	{
		TOKEN_CHANGE(name, "VARBINARY");
		STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
	}
	else
	// MariaDB supports RAW in Oracle compatibility mode
	if(_target == SQL_MARIADB_ORA)
	{
		STATS_DTL_CONV_NO_NEED(true)
		STATS_ITM_CONV_NO_NEED
	}
    else
    // Convert to BYTEA in PostgreSQL and Greenplum
    if(_target == SQL_POSTGRESQL || _target == SQL_GREENPLUM)
    {
        Token::Change(name, "BYTEA", L"BYTEA", 5);
        Token::Remove(open, close);

		STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
    }

	STATS_UPDATE_STATUS
	DTYPE_STATS(name)
    DTYPE_DTL_STATS_L(name)

    return true;
}

// REAL in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseRealType(Token *name)
{
    if(name == NULL)
        return false;

    bool real = false;
    bool real_in_braces = false;

    if(name->Compare("REAL", L"REAL", 4) == true)
        real = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[REAL]", L"[REAL]", 6) == true)
            real_in_braces = true;

    if(real == false && real_in_braces == false)
        return false;

	STATS_DECL

    // MySQL REAL can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *number */ (void) GetNextNumberToken();
        /*Token *comma */ (void) GetNextCharToken(',', L',');
        /*Token *scale */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove size for REAL
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to BINARY_FLOAT in Oracle
    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
    {
        // By default, MySQL treats REAL as a synonym for DOUBLE
        if(_source == SQL_MYSQL)
            Token::Change(name, "BINARY_DOUBLE", L"BINARY_DOUBLE", 13);
        else
            Token::Change(name, "BINARY_FLOAT", L"BINARY_FLOAT", 12);
    }
    else
        // Convert Oracle REAL to FLOAT in other databases
        if(_source == SQL_ORACLE && _target != SQL_ORACLE)
        {
            // Convert to DOUBLE PRECISION in PostgreSQL
            if(_target == SQL_POSTGRESQL)
                Token::Change(name, "DOUBLE PRECISION", L"DOUBLE PRECISION", 16);
            else
                Token::Change(name, "FLOAT", L"FLOAT", 5);
        }
        else
            // Convert MySQL REAL to DOUBLE PRECISION in SQL Server
            if(_source == SQL_MYSQL && _target == SQL_SQL_SERVER)
                Token::Change(name, "DOUBLE PRECISION", L"DOUBLE PRECISION", 16);
            else
                // Remove [] for other databases
                if(_target != SQL_SQL_SERVER && real_in_braces == true)
                    Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(real_in_braces)
	{
		DTYPE_STATS_V("REAL", name)
		DTYPE_DTL_STATS_ST("REAL", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// Oracle SYS_REFCURSOR, PostgreSQL REFCURSOR type
bool SqlParser::ParseRefcursor(Token *name)
{
    if(name == NULL)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_REFCURSOR_DESC)

	// SYS_REFCURSOR in Oracle 
	if(TOKEN_CMP(name, "SYS_REFCURSOR"))
	{
		if(Target(SQL_POSTGRESQL))
			TOKEN_CHANGE(name, "REFCURSOR");
	}
	else
	// REFCURSOR in PostgreSQL
	if(TOKEN_CMP(name, "REFCURSOR"))
	{
	}
	else
		return false;
	
    // Netezza does not support cursors
    if(_target == SQL_NETEZZA)
        Token::Change(name, "RECORD", L"RECORD", 6);
        
	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// ROWID in Oracle
bool SqlParser::ParseRowidType(Token *name)
{
    if(name == NULL)
        return false;

    // ROWID 10 bytes
    if(name->Compare("ROWID", L"ROWID", 5) == false)
        return false;

	STATS_DECL
	STATS_DTL_DECL
	STATS_SET_DESC(SQL_DATATYPE_ROWID_DESC)

	STATS_ITM_DECL

    // Convert to CHAR(10) in other databases
    if(_target != SQL_ORACLE)
    {
        Token::Change(name, "CHAR(10)", L"CHAR(10)", 8);

		STATS_DTL_CONV_ERROR(true, STATS_CONV_HIGH, "", "")
        STATS_ITM_CONV_ERROR
    }
        
	STATS_UPDATE_STATUS
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// ROWVERSION in SQL Server
bool SqlParser::ParseRowversionType(Token *name)
{
    if(name == NULL)
        return false;

    bool rowversion = false;
    bool rowversion_in_braces = false;

	STATS_DECL

    if(name->Compare("ROWVERSION", L"ROWVERSION", 10) == true)
        rowversion = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[ROWVERSION]", L"[ROWVERSION]", 12) == true)
            rowversion_in_braces = true;

    if(rowversion == false && rowversion_in_braces == false)
        return false;

    // Convert to BINARY(8) in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL))
        Token::Change(name, "BINARY(8)", L"BINARY(8)", 9);
    else
        // Convert to BYTEA in PostgreSQL
        if(_target == SQL_POSTGRESQL)
            Token::Change(name, "BYTEA", L"BYTEA", 5);
        else
            // Remove [] for other databases
            if(_target != SQL_SQL_SERVER && rowversion_in_braces == true)
                Token::ChangeNoFormat(name, name, 1, name->len - 2);
    
	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// SERIAL in MySQL, PostgreSQL, Informix
bool SqlParser::ParseSerialType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("SERIAL", L"SERIAL", 6) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Start is optional in Informix
    if(open != NULL)
    {
        /*Token *start */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not Informix remove start for SERIAL8
        if(_target != SQL_INFORMIX)
            Token::Remove(open, close);
    }

    // Convert to NUMBER in Oracle
    if(_target == SQL_ORACLE)
    {
        if(_source == SQL_MYSQL)
            Token::Change(name, "NUMBER(19)", L"NUMBER(19)", 10);
        else
            Token::Change(name, "NUMBER(10)", L"NUMBER(10)", 10);
    }
    else
        // Convert to INT in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            if(_source == SQL_MYSQL)
                Token::Change(name, "NUMERIC(20)", L"NUMERIC(20)", 11);
            else
                Token::Change(name, "INT", L"INT", 3);
        }

		STATS_ITM_DECL 
        DTYPE_DTL_STATS_L(name)

        return true;
}

// SERIAL2 in PostreSQL
bool SqlParser::ParseSerial2Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("SERIAL2", L"SERIAL2", 7) == false)
        return false;

	STATS_DECL

    // Convet to NUMBER(5) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(5)", L"NUMBER(5)", 9);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// SERIAL4 in PostgreSQL
bool SqlParser::ParseSerial4Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("SERIAL4", L"SERIAL4", 7) == false)
        return false;

	STATS_DECL

    // Convert to NUMBER(10) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(10)", L"NUMBER(10)", 10);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// SERIAL8 in PostgreSQL, Informix
bool SqlParser::ParseSerial8Type(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("SERIAL8", L"SERIAL8", 7) == false)
        return false;

	STATS_DECL

    Token *open = GetNextCharToken('(', L'(');

    // Start is optional in Informix
    if(open != NULL)
    {
        /*Token *start */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not Informix remove start for SERIAL8
        if(_target != SQL_INFORMIX)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(19) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(19)", L"NUMBER(19)", 10);
    else
        // Convert to BIGINT in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "BIGINT", L"BIGINT", 6);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// SHORT in Sybase ADS
bool SqlParser::ParseShortType(Token *name)
{
    if(name == NULL || _source != SQL_SYBASE_ADS)
        return false;

    if(!TOKEN_CMP(name, "SHORT"))
        return false;

	STATS_DECL

    if(_target != SQL_SYBASE_ADS)
		TOKEN_CHANGE(name, "SMALLINT");

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// SMALLDATETIME in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseSmalldatetimeType(Token *name)
{
    if(name == NULL)
        return false;

    bool smalldatetime = false;
    bool smalldatetime_in_braces = false;
    bool smalldatetime_in_quotes = false;

	STATS_DECL

    if(name->Compare("SMALLDATETIME", L"SMALLDATETIME", 13) == true)
        smalldatetime = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[SMALLDATETIME]", L"[SMALLDATETIME]", 15) == true)
            smalldatetime_in_braces = true;
        else
            // Can be enclosed in "" for Sybase ASA
            if(name->Compare("\"SMALLDATETIME\"", L"\"SMALLDATETIME\"", 15) == true)
                smalldatetime_in_quotes = true;

    if(smalldatetime == false && smalldatetime_in_braces == false && smalldatetime_in_quotes == false)
        return false;

    // Convert to TIMESTAMP in Oracle, PostgreSQL
    if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
    {
        Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);

        // Fraction is 0 in SQL Server, Sybase ASE
        if(Source(SQL_SQL_SERVER, SQL_SYBASE) == true)
            Append(name, "(0)", L"(0)", 3);
    }
    else
        // Convert Sybase ASA SMALLDATETIME to DATETIME(6) in SQL Server
        if(_target == SQL_SQL_SERVER && _source == SQL_SYBASE_ASA)
            Token::Change(name, "DATETIME2(6)", L"DATETIME2(6)", 12);
        else
            // Convert to DATETIME in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL))
            {
                Token::Change(name, "DATETIME", L"DATETIME", 8);
            }
            else
                // Remove [] for other databases
                if(_target != SQL_SQL_SERVER && smalldatetime_in_braces == true)
                    Token::ChangeNoFormat(name, name, 1, name->len - 2);
                else
                    // Remove "" for other databases
                    if(_target != SQL_SYBASE_ASA && smalldatetime_in_quotes == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(smalldatetime_in_braces)
	{
		DTYPE_STATS_V("SMALLDATETIME", name)
		DTYPE_DTL_STATS_ST("SMALLDATETIME", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// SMALLFLOAT in Informix
bool SqlParser::ParseSmallfloatType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("SMALLFLOAT", L"SMALLFLOAT", 10) == false)
        return false;

	STATS_DECL

    // Convet to BINARY_FLOAT in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "BINARY_FLOAT", L"BINARY_FLOAT", 12);
    else
        // Convert to REAL in other databases
        if(_target != SQL_INFORMIX)
            Token::Change(name, "REAL", L"REAL", 4);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// SMALLINT in Oracle, SQL Server, DB2, MySQL, PostreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseSmallintType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    bool smallint = false;
    bool smallint_in_braces = false;

    if(name->Compare("SMALLINT", L"SMALLINT", 8) == true)
        smallint = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[SMALLINT]", L"[SMALLINT]", 10) == true)
            smallint_in_braces = true;

    if(smallint == false && smallint_in_braces == false)
        return false;

	STATS_DECL

    Token *open = GetNextCharToken('(', L'(');

    // MySQL SMALLINT can include display size
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for SMALLINT
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convet to NUMBER(5) in Oracle
    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
    {
        Token::Change(name, "NUMBER", L"NUMBER", 6);

        // Not in parameters
        if(clause_scope != SQL_SCOPE_FUNC_PARAMS && clause_scope != SQL_SCOPE_PROC_PARAMS)
        {
            // Add size as separate tokens as it may be modified later
            AppendNoFormat(name, "(", L"(", 1);
            AppendNoFormat(name, "5", L"5", 1);
            AppendNoFormat(name, ")", L")", 1);
        }
    }
    else
        // Convert Oracle INTEGER to DECIMAL(38) in other databases
        if(_source == SQL_ORACLE && _target != SQL_ORACLE)
            Token::Change(name, "DECIMAL(38)", L"DECIMAL(38)", 11);
        else
            // Remove [] for other databases
            if(_target != SQL_SQL_SERVER && smallint_in_braces == true)
                Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

    if(smallint_in_braces)
	{
		DTYPE_STATS_V("SMALLINT", name)
		DTYPE_DTL_STATS_ST("SMALLINT", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// SMALLMONEY in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseSmallmoneyType(Token *name)
{
    if(name == NULL)
        return false;

    bool smallmoney = false;
    bool smallmoney_in_braces = false;

	STATS_DECL

    if(name->Compare("SMALLMONEY", L"SMALLMONEY", 10) == true)
        smallmoney = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[SMALLMONEY]", L"[SMALLMONEY]", 12) == true)
            smallmoney_in_braces = true;

    if(smallmoney == false && smallmoney_in_braces == false)
        return false;

    // Convert to NUMBER in Oracle
    if(_target == SQL_ORACLE)
    {
        // Sybase ASA SMALLMONEY implemented as NUMBER(10,4)
        if(_source == SQL_SYBASE_ASA)
            Token::Change(name, "NUMBER(10,4)", L"NUMBER(10,4)", 12);
        else
            // Sybase ASE SMALLMONEY implemented as NUMBER(6,4)
            if(_source == SQL_SYBASE)
                Token::Change(name, "NUMBER(6,4)", L"NUMBER(6,4)", 11);
            else
                Token::Change(name, "NUMBER", L"NUMBER", 6);
    }
    else
        // Convert to DECIMAL in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            // SQL Server SMALLMONEY implemented as DECIMAL(6,4)
            if(_source == SQL_SQL_SERVER)
                Token::Change(name, "DECIMAL(6,4)", L"DECIMAL(6,4)", 12);
            else
                Token::Change(name, "DECIMAL", L"DECIMAL", 7);
        }
        else
            // Convet to MONEY in PostgreSQL
            if(_target == SQL_POSTGRESQL)
                Token::Change(name, "MONEY", L"MONEY", 5);
            else
                // Remove [] for other databases
                if(_target != SQL_SQL_SERVER && smallmoney_in_braces == true)
                    Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 
    
	if(smallmoney_in_braces)
	{
		DTYPE_STATS_V("SMALLMONEY", name)
		DTYPE_DTL_STATS_ST("SMALLMONEY", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// SMALLSERIAL in PostreSQL
bool SqlParser::ParseSmallserialType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("SMALLSERIAL", L"SMALLSERIAL", 11) == false)
        return false;

	STATS_DECL

    // Convet to NUMBER(5) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(5)", L"NUMBER(5)", 9);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// STRING in Sybase ADS (not documented)
bool SqlParser::ParseStringType(Token *name)
{
    if(name == NULL)
        return false;

    if(!TOKEN_CMP(name, "STRING"))
        return false;

	STATS_DECL

    if(Target(SQL_SQL_SERVER))
        TOKEN_CHANGE(name, "VARCHAR(1000)");

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// TEXT in SQL Server, MySQL, PostreSQL, Informix, Sybase ASE, Sybase ASA, Greenplum
bool SqlParser::ParseTextType(Token *name)
{
    if(name == NULL)
        return false;

    bool text = false;
    bool text_in_braces = false;
    bool text_in_quotes = false;

	STATS_DECL

    if(name->Compare("TEXT", L"TEXT", 4) == true)
        text = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[TEXT]", L"[TEXT]", 6) == true)
            text_in_braces = true;
        else
            // Can be enclosed in "" for Sybase ASA
            if(name->Compare("\"TEXT\"", L"\"TEXT\"", 6) == true)
                text_in_quotes = true;

    if(text == false && text_in_braces == false && text_in_quotes == false)
        return false;

    // Optional IN TABLE | IN lobspace for Informix TEXT
    Token *in = GetNextWordToken("IN", L"IN", 2);
    Token *space = NULL;

    if(in != NULL)
    {
        space = GetNextToken();

        // If target is not Informix remove IN TABLE | IN lobspace attributes
        if(_target != SQL_INFORMIX)
            Token::Remove(in, space);
    }

    // Num is optional in MySQL
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove num for TEXT
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to CLOB in Oracle, DB2
    if(Target(SQL_ORACLE, SQL_DB2) == true)
        Token::Change(name, "CLOB", L"CLOB", 4);
    else
        // Convert to VARCHAR(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "VARCHAR", L"VARCHAR", 7);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
        else
            // Convert to LONGTEXT in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL) && _source!= SQL_MYSQL)
                Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
            else
                // Remove [] for other databases
                if(_target != SQL_SQL_SERVER && text_in_braces == true)
                    Token::ChangeNoFormat(name, name, 1, name->len - 2);
                else
                    // Remove "" for other databases
                    if(_target != SQL_SYBASE_ASA && text_in_quotes == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(text_in_braces)
	{
		DTYPE_STATS_V("TEXT", name)
		DTYPE_DTL_STATS_ST("TEXT", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// TIME in SQL Server, DB2, MySQL, PostgreSQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseTimeType(Token *name)
{
    if(name == NULL)
        return false;

    bool time_ = false;
    bool time_in_braces = false;

	STATS_DECL

    if(name->Compare("TIME", L"TIME", 4) == true)
        time_ = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[TIME]", L"[TIME]", 6) == true)
            time_in_braces = true;

    if(time_ == false && time_in_braces == false)
        return false;

    Token *open = GetNextCharToken('(', L'(');
    Token *fraction = NULL;

    int fraction_int = 0;

    // Optional fractional precision for SQL Server, MySQL, PostgreSQL
    if(open != NULL)
    {
        fraction = GetNextToken();
        fraction_int = fraction->GetInt();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // WITHOUT TIME ZONE is optional for PostgreSQL
    Token *without = GetNextWordToken("WITHOUT", L"WITHOUT", 7);
    // WITH TIME ZONE is mandatory parameter for PostgreSQL
    Token *with = GetNextWordToken("WITH", L"WITH", 4);

    if(without != NULL || with != NULL)
    {
        /*Token *time */ (void) GetNextWordToken("TIME", L"TIME", 4);
        Token *zone = GetNextWordToken("ZONE", L"ZONE", 4);

        // If target is not PostgreSQL remove WITHOUT TIME ZONE
        if(_target != SQL_POSTGRESQL && without != NULL)
            Token::Remove(without, zone);
    }

    // Default fraction
    if(open == NULL)
    {
        // Fraction is 0 in DB2, MySQL
        if(Source(SQL_DB2, SQL_MYSQL) == true && Target(SQL_DB2, SQL_MYSQL) == false)
            Append(name, "(0)", L"(0)", 3);
    }

    // Convert to TIMESTAMP in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);
    else
        // Append fraction in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            // Fraction is 6 in Sybase ASE, Sybase ASA
            if(Source(SQL_SYBASE, SQL_SYBASE_ASA) == true)
                Append(name, "(6)", L"(6)", 3);
        }
        else
            // Append fraction in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL))
            {
                // Remove []
                if(time_in_braces == true)
                    Token::ChangeNoFormat(name, name, 1, name->len - 2);

                if(open == NULL)
                {
                    // Fraction is 7 by default in SQL Server, but max fraction is 6 in MySQL
                    if(_source == SQL_SQL_SERVER)
                        Append(name, "(6)", L"(6)", 3);
                }
                else
                    if(fraction_int > 6)
                        Token::Change(fraction, "6", L"6", 1);
            }
            else
                // Append fraction in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                {
                    if(open == NULL)
                    {
                        // Fraction is 7 by default in SQL Server, but max fraction is 6 in MySQL
                        if(_source == SQL_SQL_SERVER)
                            Append(name, "(6)", L"(6)", 3);
                    }
                    else
                        if(fraction_int > 6)
                            Token::Change(fraction, "6", L"6", 1);
                }
                else
                    // Remove [] for other databases
                    if(_target != SQL_SQL_SERVER && time_in_braces == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(time_in_braces)
	{
		DTYPE_STATS_V("TIME", name)
		DTYPE_DTL_STATS_ST("TIME", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// TIMETZ in PostgreSQL
bool SqlParser::ParseTimetzType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("TIMETZ", L"TIMETZ", 6) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');
    Token *close = NULL;

    // Precision is optional in PostgreSQL
    if(open != NULL)
    {
        /*Token *precision */ (void) GetNextNumberToken();
        close = GetNextCharToken(')', L')');
    }

    // Convert to TIMESTAMP WITH TIME ZONE in Oracle
    if(_target == SQL_ORACLE)
    {
        if(open != NULL)
        {
            Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);
            Append(close, " WITH TIME ZONE", L" WITH TIME ZONE", 15);
        }
        else
            Token::Change(name, "TIMESTAMP WITH TIME ZONE", L"TIMESTAMP WITH TIME ZONE", 24);
    }

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// TIMESTAMP in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Sybase ASA, Sybase ADS, Greenplum
bool SqlParser::ParseTimestampType(Token *name)
{
    if(name == NULL)
        return false;

    bool timestamp = false;
    bool timestamp_in_braces = false;
    bool conv = false;

    if(name->Compare("TIMESTAMP", L"TIMESTAMP", 9) == true)
        timestamp = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[TIMESTAMP]", L"[TIMESTAMP]", 11) == true)
        timestamp_in_braces = true;

    if(timestamp == false && timestamp_in_braces == false)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_TIMESTAMP_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    Token *open = GetNextCharToken('(', L'(');
    Token *close = NULL;

    // Optional fractional precision for SQL Server, MySQL, PostgreSQL
    if(open != NULL)
    {
        /*Token *fraction */ (void) GetNextToken();
        close = GetNextCharToken(')', L')');
    }

    // WITHOUT TIME ZONE is optional for PostgreSQL
    Token *without = GetNextWordToken("WITHOUT", L"WITHOUT", 7);
    // WITH TIME ZONE is mandatory parameter for PostgreSQL, Sybase ASA
    Token *with = GetNextWordToken("WITH", L"WITH", 4);

    // LOCAL is optional keyword in Oracle
    if(with != NULL)
    {
        Token *local = GetNextWordToken("LOCAL", L"LOCAL", 5);

        // If target is not Oracle remove LOCAL keyword
        if(_target != SQL_ORACLE)
        {
            Token::Remove(local);
            conv = true;
        }
    }

    Token *zone = NULL;

    if(without != NULL || with != NULL)
    {
        /*Token *time */ (void) GetNextWordToken("TIME", L"TIME", 4);
        zone = GetNextWordToken("ZONE", L"ZONE", 4);

        // If target is not PostgreSQL remove WITHOUT TIME ZONE
        if(_target != SQL_POSTGRESQL && without != NULL)
        {
            Token::Remove(without, zone);
            conv = true;
        }
    }

    // Default fraction
    if(open == NULL)
    {
        // TIMESTAMP(0) is default in MySQL
        if(_source == SQL_MYSQL && _target != SQL_MYSQL)
        {
            Append(name, "(0)", L"(0)", 3);
            conv = true;
        }
    }

    // Convert to DATETIME2 in SQL Server
    if(_target == SQL_SQL_SERVER && _source != SQL_SQL_SERVER)
    {
        if(with != NULL)
        {
            Token::Change(name, "DATETIMEOFFSET", L"DATETIMEOFFSET", 14);
            Token::Remove(with, zone);
            conv = true;
        }
        else
        {
            Token::Change(name, "DATETIME2", L"DATETIME2", 9);
            conv = true;
        }

        // TIMESTAMP(6) is default in Sybase ASA
        if(_source == SQL_SYBASE_ASA)
        {
            Append(name, "(6)", L"(6)", 3);
            conv = true;
        }
        else
        // TIMESTAMP(6) is default in Oracle
        if(Source(SQL_ORACLE, SQL_DB2) == true && open == NULL)
        {
            Append(name, "(6)", L"(6)", 3);
            conv = true;
        }
		else
		// TIMESTAMP with milliseconds in Sybase ADS
        if(_source == SQL_SYBASE_ADS)
        {
            APPEND_NOFMT(name, "(3)");
            conv = true;
        }
    }
    else
    // Convert to DATETIME in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_MYSQL)
    {
        if(_source == SQL_SQL_SERVER)
            Token::Change(name, "BINARY(8)", L"BINARY(8)", 9);
        else
            Token::Change(name, "DATETIME", L"DATETIME", 8);

        // TIMESTAMP(6) is default in Oracle
        if(_source == SQL_ORACLE && open == NULL)
            Append(name, "(6)", L"(6)", 3);

        if(with != NULL)
            Token::Remove(with, zone);

        conv = true;
    }
    else
    if(_target == SQL_POSTGRESQL)
    {
        // Convert SQL Server TIMESTAMP to BYTEA in PostgreSQL
        if(_source == SQL_SQL_SERVER)
        {
            Token::Change(name, "BYTEA", L"BYTEA", 5);
            conv = true;
        }
    }
    else
    // Remove precision for Hive
    if(_target == SQL_HIVE)
    {
        Token::Remove(open, close);
        conv = true;
    }
    else
    // Remove [] for other database
    if(_target != SQL_SQL_SERVER && timestamp_in_braces == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);

    if(!conv)
	{
		STATS_DTL_CONV_NO_NEED(true)
        STATS_ITM_CONV_NO_NEED
	}

	STATS_UPDATE_STATUS

    DTYPE_STATS(name)
    DTYPE_DTL_STATS_L(name)

    return true;
}

// TIMESTAMPTZ in PostgreSQL
bool SqlParser::ParseTimestamptzType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("TIMESTAMPTZ", L"TIMESTAMPTZ", 11) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');
    Token *close = NULL;

    // Precision is optional in PostgreSQL
    if(open != NULL)
    {
        /*Token *precision */ (void) GetNextNumberToken();
        close = GetNextCharToken(')', L')');
    }

    // Convert to TIMESTAMP WITH TIME ZONE in Oracle
    if(_target == SQL_ORACLE)
    {
        if(open != NULL)
        {
            Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);
            Append(close, " WITH TIME ZONE", L" WITH TIME ZONE", 15);
        }
        else
            Token::Change(name, "TIMESTAMP WITH TIME ZONE", L"TIMESTAMP WITH TIME ZONE", 24);
    }

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// TINYBLOB in MySQL
bool SqlParser::ParseTinyblobType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("TINYBLOB", L"TINYBLOB", 8) == false)
        return false;

    // Convert to RAW(255) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "RAW(255)", L"RAW(255)", 8);
    else
        // Convert to VARBINARY(255) in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "VARBINARY(255)", L"VARBINARY(255)", 14);

	STATS_DECL

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// TINYINT in SQL Server, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseTinyintType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    bool tinyint = false;
    bool tinyint_in_braces = false;

    if(name->Compare("TINYINT", L"TINYINT", 7) == true)
        tinyint = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[TINYINT]", L"[TINYINT]", 9) == true)
        tinyint_in_braces = true;

    if(tinyint == false && tinyint_in_braces == false)
        return false;

	STATS_DECL

    // MySQL TINYINT can include display size
    Token *open = GetNextCharToken('(', L'(');

    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove the display size for TINYINT
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(3) in Oracle
    if(_target == SQL_ORACLE)
    {
        Token::Change(name, "NUMBER", L"NUMBER", 6);

        // Not in parameters
        if(clause_scope != SQL_SCOPE_FUNC_PARAMS && clause_scope != SQL_SCOPE_PROC_PARAMS)
        {
            // Add size as separate tokens as it may be modified later
            AppendNoFormat(name, "(", L"(", 1);
            AppendNoFormat(name, "3", L"3", 1);
            AppendNoFormat(name, ")", L")", 1);
        }
    }
    else
    // Convert MySQL TINYINT to SMALLINT in SQL Server
    if(_source == SQL_MYSQL && _target == SQL_SQL_SERVER)
        Token::Change(name, "SMALLINT", L"SMALLINT", 8);
    else
    // SQL Server's TINYINT is unsigned
    if(_source == SQL_SQL_SERVER && Target(SQL_MARIADB, SQL_MYSQL))
    {
        // Remove []
        if(tinyint_in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);

        Append(name, " UNSIGNED", L" UNSIGNED", 9);
    }
    else
    // Convert to SMALLINT in PostgreSQL, Redshift
    if(Target(SQL_POSTGRESQL, SQL_REDSHIFT))
        Token::Change(name, "SMALLINT", L"SMALLINT", 8);
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && tinyint_in_braces == true)
        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 
    
	if(tinyint_in_braces)
	{
		DTYPE_STATS_V("TINYINT", name)
		DTYPE_DTL_STATS_ST("TINYINT", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// TINYTEXT in MySQL
bool SqlParser::ParseTinytextType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("TINYTEXT", L"TINYTEXT", 8) == false)
        return false;

	STATS_DECL

    // Convert to VARCHAR2(255) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "VARCHAR2(255)", L"VARCHAR2(255)", 13);
    else
        // Convert to VARCHAR(255) in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "VARCHAR(255)", L"VARCHAR(255)", 12);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// UNICODE CHARACTER, UNICODE CHAR, UNICODE CHARACTER VARYING, UNICODE CHAR VARYING in Sybase ASE
bool SqlParser::ParseUnicodeType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("UNICODE", L"UNICODE", 7) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *unit = GetNextToken();
    Token *character = NULL;
    Token *varying = NULL;

    if(Token::Compare(unit, "CHAR", L"CHAR", 4) == true ||
        Token::Compare(unit, "CHARACTER", L"CHARACTER", 9) == true)
    {
        character = unit;
        varying = GetNextWordToken("VARYING", L"VARYING", 7);
    }

    Token *open = GetNextCharToken('(', L'(');
    Token *size = NULL;
    Token *close = NULL;

    // Size is optional in Sybase ASE (default num is 1)
    if(open != NULL)
    {
        size = GetNextNumberToken();
        close = GetNextCharToken(')', L')');
    }

    // Check for UNICODE CHARACTER VARYING, UNICODE CHAR VARYING
    if(varying != NULL)
    {
        // Convert to NVARCHAR2 in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name, character);
            Token::Change(varying, "NVARCHAR2", L"NVARCHAR2", 9);

            // Size is 1 by default in Sybase ASE
            if(open == NULL)
                Append(varying, "(1)", L"(1)", 3);
        }
        else
            // Convert to NATIONAL CHARACTER VARYING and NATIONAL CHAR VARYING in SQL Server
            if(_target == SQL_SQL_SERVER)
                Token::Change(name, "NATIONAL", L"NATIONAL", 8);
            else
                // Convert to CHARACTER VARYING and CHAR VARYING in PostgreSQL
                if(_target == SQL_POSTGRESQL)
                {
                    Token::Remove(name);

                    // CHAR[ACTER] VARYING is unlimited by default in PostgreSQL
                    if(open == NULL)
                        Append(varying, "(1)", L"(1)", 3);
                }
    }
    else
        // Check for UNICODE CHARACTER, UNICODE CHAR
    {
        // Convert to NATIONAL CHARACTER and NATIONAL CHAR in Oracle, SQL Server
        if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
            Token::Change(name, "NATIONAL", L"NATIONAL", 8);
        else
            // Convert to CHARACTER and CHAR in PostgreSQL
            if(_target == SQL_POSTGRESQL)
                Token::Remove(name);
    }

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// UNICHAR in Sybase ASE
bool SqlParser::ParseUnicharType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("UNICHAR", L"UNICHAR", 7) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Num is optional in Sybase ASE (default num is 1)
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to NCHAR in Oracle, SQL Server
    if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
        Token::Change(name, "NCHAR", L"NCHAR", 5);
    else
        // Convert to CHAR in PostgreSQL
        if(_target == SQL_POSTGRESQL)
            Token::Change(name, "CHAR", L"CHAR", 4);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// UNIQUEIDENTIFIER in SQL Server, Sybase ASA
bool SqlParser::ParseUniqueidentifierType(Token *name)
{
    if(name == NULL)
        return false;

    bool uuid = false;
    bool in_braces = false;

	STATS_DECL

    // In SQL Server, UNIQUEIDENTIFIER is stored in 16 bytes, but on retrieval returns 36 characters (GUID with dashes)

    if(name->Compare("UNIQUEIDENTIFIER", L"UNIQUEIDENTIFIER", 16) == true)
        uuid = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[UNIQUEIDENTIFIER]", L"[UNIQUEIDENTIFIER]", 18) == true)
            in_braces = true;

    if(uuid == false && in_braces == false)
        return false;

    // Convert to CHAR(36) in Oracle, MySQL, PostgreSQL
    if(Target(SQL_ORACLE, SQL_MYSQL, SQL_POSTGRESQL) == true)
        Token::Change(name, "CHAR(36)", L"CHAR(16)", 8);
    else
        // Remove [] for other databases
        if(_target != SQL_SQL_SERVER && in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// UNIQUEIDENTIFIERSTR in Sybase ASA
bool SqlParser::ParseUniqueidentifierstrType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("UNIQUEIDENTIFIERSTR", L"UNIQUEIDENTIFIERSTR", 19) == false)
        return false;

	STATS_DECL

    // Convert to CHAR(36) in Oracle, SQL Server
    if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
        Token::Change(name, "CHAR(36)", L"CHAR(36)", 8);
    else
        // Convert to UUID in PostgreSQL
        if(_target == SQL_POSTGRESQL)
            Token::Change(name, "UUID", L"UUID", 4);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// UNITEXT in Sybase ASE
bool SqlParser::ParseUnitextType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("UNITEXT", L"UNITEXT", 7) == false)
        return false;

	STATS_DECL

    // Convert to NCLOB in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NCLOB", L"NCLOB", 5);
    else
        // Convert to NVARCHAR(max) in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);
            AppendNoFormat(name, "(max)", L"(max)", 5);
        }
        else
            // Convert to TEXT in PostgreSQL
            if(_target == SQL_POSTGRESQL)
                Token::Change(name, "TEXT", L"TEXT", 4);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// UNIVARCHAR in Sybase ASE
bool SqlParser::ParseUnivarcharType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("UNIVARCHAR", L"UNIVARCHAR", 10) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Num is optional in Sybase ASE (default num is 1)
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to NVARCHAR in Oracle
    if(_target == SQL_ORACLE)
    {
        Token::Change(name, "NVARCHAR2", L"NVARCHAR2", 9);

        // Size is 1 by default in Sybase ASE
        if(open == NULL)
            Append(name, "(1)", L"(1)", 3);
    }
    else
        // Convert to NVARCHAR in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);
        else
            // Convert to VARCHAR in PostgreSQL
            if(_target == SQL_POSTGRESQL)
            {
                Token::Change(name, "VARCHAR", L"VARCHAR", 7);

                // The default num is 1G in PostgreSQL
                if(open == NULL)
                    Append(name, "(1)", L"(1)", 3);
            }

			STATS_ITM_DECL 
            DTYPE_DTL_STATS_L(name)

            return true;
}

// UNSIGNED BIGINT, UNSIGNED INT, UNSIGNED SMALLINT in Sybase ASE, Sybase ASA;
// UNSIGNED TINYINT in Sybase ASA
bool SqlParser::ParseUnsignedType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("UNSIGNED", L"UNSIGNED", 8) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *unit = GetNextToken();
    Token *bigint = NULL;
    Token *int_ = NULL;
    Token *smallint = NULL;
    Token *tinyint = NULL;

    if(Token::Compare(unit, "BIGINT", L"BIGINT", 6) == true)
        bigint = unit;
    else
        if(Token::Compare(unit, "INT", L"INT", 3) == true || Token::Compare(unit, "INTEGER", L"INTEGER", 7) == true)
            int_ = unit;
        else
            if(Token::Compare(unit, "SMALLINT", L"SMALLINT", 8) == true)
                smallint = unit;
            else
                if(Token::Compare(unit, "TINYINT", L"TINYINT", 7) == true)
                    tinyint = unit;

    // Check for UNSIGNED BIGINT
    if(bigint != NULL)
    {
        // Convert UNSIGNED BIGINT to NUMBER(20) in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Remove(name);
            Token::Change(unit, "NUMBER(20)", L"NUMBER(20)", 10);
        }
        else
            // Convert UNSIGNED BIGINT to NUMERIC(20) in SQL Server, PostgreSQL
            if(Target(SQL_SQL_SERVER, SQL_POSTGRESQL) == true)
            {
                Token::Remove(name);
                Token::Change(unit, "NUMERIC(20)", L"NUMERIC(20)", 11);
            }
    }
    else
        // Check for UNSIGNED INT
        if(int_ != NULL)
        {
            // Convert UNSIGNED INT to NUMBER(10) in Oracle
            if(_target == SQL_ORACLE)
            {
                Token::Remove(name);
                Token::Change(unit, "NUMBER(10)", L"NUMBER(10)", 10);
            }
            else
                // Convert UNSIGNED INT to NUMERIC(10) in SQL Server, PostgreSQL
                if(Target(SQL_SQL_SERVER, SQL_POSTGRESQL) == true)
                {
                    Token::Remove(name);
                    Token::Change(unit, "NUMERIC(10)", L"NUMERIC(10)", 11);
                }
        }
        else
            // Check for UNSIGNED SMALLINT
            if(smallint != NULL)
            {
                // Convert UNSIGNED SMALLINT to NUMBER(5) in Oracle
                if(_target == SQL_ORACLE)
                {
                    Token::Remove(name);
                    Token::Change(unit, "NUMBER(5)", L"NUMBER(5)", 9);
                }
                else
                    // Convert UNSIGNED SMALLINT to NUMERIC(5) in SQL Server, PostgreSQL
                    if(Target(SQL_SQL_SERVER, SQL_POSTGRESQL) == true)
                    {
                        Token::Remove(name);
                        Token::Change(unit, "NUMERIC(5)", L"NUMERIC(5)", 10);
                    }
            }
            else
                if(tinyint != NULL)
                {
                    // Convert UNSIGNED TINYINT to NUMBER(3) in Oracle
                    if(_target == SQL_ORACLE)
                    {
                        Token::Remove(name);
                        Token::Change(unit, "NUMBER(3)", L"NUMBER(3)", 9);
                    }
                    else
                        // SQL Server's TINYINT is unsigned
                        if(_target == SQL_SQL_SERVER)
                            Token::Remove(name);
                        else
                            // Convert UNSIGNED TINYINT to NUMERIC(3) in PostgreSQL
                            if(_target == SQL_POSTGRESQL)
                            {
                                Token::Remove(name);
                                Token::Change(unit, "NUMERIC(3)", L"NUMERIC(3)", 10);
                            }
                }

				STATS_ITM_DECL 
                DTYPE_DTL_STATS_L(name)

                return true;
}

// UROWID in Oracle
bool SqlParser::ParseUrowidType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("UROWID", L"UROWID", 6) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Size is optional in Oracle
    if(open != NULL)
    {
        /*Token *size */ (void) GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to VARCHAR in other databases
    if(_target != SQL_ORACLE)
    {
        Token::Change(name, "VARCHAR", L"VARCHAR", 7);

        // Default size is 4000 in Oracle
        if(open == NULL)
            Append(name, "(4000)", L"(4000)", 6);
    }

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// UUID in PostgreSQL
bool SqlParser::ParseUuidType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("UUID", L"UUID", 4) == false)
        return false;

	STATS_DECL

    // Convert to CHAR(36) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "CHAR(36)", L"CHAR(36)", 8);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// VARBINARY in SQL Server, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseVarbinaryType(Token *name)
{
    if(name == NULL)
        return false;

    bool varbinary = false;
    bool varbinary_in_braces = false;

	STATS_DECL

    if(name->Compare("VARBINARY", L"VARBINARY", 9) == true)
        varbinary = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[VARBINARY]", L"[VARBINARY]", 11) == true)
            varbinary_in_braces = true;

    if(varbinary == false && varbinary_in_braces == false)
        return false;

    Token *open = GetNextCharToken('(', L'(');
    Token *size = NULL;
    Token *close = NULL;

    // Size is optional for SQL Server, Sybase ASE, Sybase ASA
    if(open != NULL)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();
        close = GetNextCharToken(')', L')');
    }

    // If MAX is specified, convert to CLOB types
    if(size->Compare("MAX", L"MAX", 3) == true)
    {
        // Change to LONGBLOB in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Change(name, "LONGBLOB", L"LONGBLOB", 8);
            Token::Remove(open, close);
        }
        else
            // Change to BYTEA in PostgreSQL
            if(_target == SQL_POSTGRESQL)
            {
                Token::Change(name, "BYTEA", L"BYTEA", 5);
                Token::Remove(open, close);
            }
    }
    else
        // Convert to RAW in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Change(name, "RAW", L"RAW", 3);

            // Size is mandatory in Oracle
            if(open == NULL)
                Append(name, "(1)", L"(1)", 3);
        }
        else
            // Convert to BYTEA in PostgreSQL
            if(_target == SQL_POSTGRESQL)
            {
                Token::Change(name, "BYTEA", L"BYTEA", 5);

                if(open != NULL)
                    Token::Remove(open, close);
            }
            else
                // Size is mandatory in MySQL
                if(Target(SQL_MARIADB, SQL_MYSQL) && open == NULL)
                {
                    Append(name, "(1)", L"(1)", 3);

                    // Remove [] for MySQL
                    if(varbinary_in_braces == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);
                }
                else
                    // Remove [] for other databases
                    if(_target != SQL_SQL_SERVER && varbinary_in_braces == true)
                        Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(varbinary_in_braces)
	{
		DTYPE_STATS_V("VARBINARY", name)
		DTYPE_DTL_STATS_ST("VARBINARY", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// VARBIT in PostgreSQL, Sybase ASA
bool SqlParser::ParseVarbitType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("VARBIT", L"VARBIT", 6) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');
    Token *num = NULL;

    // Num is optional in PostgreSQL, Sybase ASA
    if(open != NULL)
    {
        num = GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', ')');
    }

    // Calculating RAW and BINARY size in bytes
    if(open != NULL && Target(SQL_POSTGRESQL, SQL_SYBASE_ASA) == false)
    {
        int size;
        int rem;

        size = num->GetInt();
        rem = size % 8;

        size = size/8;

        if(rem > 0)
            size++;

        Token::Change(num, size);
    }

    // Convert to RAW in Oracle
    if(_target == SQL_ORACLE)
    {
        Token::Change(name, "RAW", L"RAW", 3);

        if(open == NULL)
        {
            // VARBIT is unlimited by default in PostgreSQL
            if(_source == SQL_POSTGRESQL)
                Append(name, "(2000)", L"(2000)", 6);
            else
                // VARBIT is 1 bit by default in Sybase ASA
                if(_source == SQL_SYBASE_ASA)
                    Append(name, "(1)", L"(1)", 3);
        }
    }
    else
        // Convert to BINARY in SQL Server
        if(_target == SQL_SQL_SERVER)
        {
            Token::Change(name, "BINARY", L"BINARY", 6);

            if(open == NULL)
            {
                // BIT VARYING is 1 bit by default in Sybase ASA
                if(_source == SQL_SYBASE_ASA)
                    Append(name, "(1)", L"(1)", 3);
            }
        }

		STATS_ITM_DECL 
        DTYPE_DTL_STATS_L(name)

        return true;
}

// VARCHAR in Oracle, SQL Server, DB2, MySQL, PostgreSQL, Informix, Sybase ASE, Sybase ASA
bool SqlParser::ParseVarcharType(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    bool varchar = false;
    bool varchar_in_braces = false;
	bool conv = false;
	bool req_eval = false;

    if(name->Compare("VARCHAR", L"VARCHAR", 7) == true)
        varchar = true;
    else
    // Can be enclosed in [] for SQL Server
    if(name->Compare("[VARCHAR]", L"[VARCHAR]", 9) == true)
        varchar_in_braces = true;

    if(varchar == false && varchar_in_braces == false)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_VARCHAR_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL

    Token *open = GetNextCharToken('(', L'(');

    Token *size = NULL;
    Token *close = NULL;

    // Size is optional for SQL Server, PostgreSQL, Informix, Sybase ASE, Sybase ASA
    if(open != NULL)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();

        // For Oracle, size can be followed by length semantics CHAR or BYTE; for DB2 BYTE, OCTETS (bytes)
        Token *byte_semantics = TOKEN_GETNEXTW("BYTE");
		Token *char_semantics = NULL;
		Token *character_semantics = NULL;
		Token *octets_semantics = NULL;

		char_semantics = (byte_semantics == NULL) ? TOKEN_GETNEXTW("CHAR") : NULL;

        // For Sybase ASA, size can be followed by length semantics CHAR or CHARACTER
        character_semantics = (char_semantics == NULL && byte_semantics == NULL) ? TOKEN_GETNEXTW("CHARACTER") : NULL;

		// OCTETS for DB2
		octets_semantics = (Nvl(character_semantics, char_semantics, byte_semantics) == NULL) ? TOKEN_GETNEXTW("OCTETS") : NULL;        

        // Convert semantic CHARACTER in Sybase ASA to CHAR in Oracle
        if(character_semantics != NULL)
		{
			if(_target == SQL_ORACLE)
			{
				TOKEN_CHANGE(character_semantics, "CHAR");
				conv = true;
			}
			else
				req_eval = true;
		}
		else
		if(octets_semantics != NULL && !Target(SQL_DB2))
			req_eval = true;

        Token *comma = GetNextCharToken(',', L',');

        // For Informix, Reserve can be specified
        if(comma != NULL)
        {
            Token *reserve = GetNextNumberToken();

            // If target is not Informix remove reserve attribute
            if(_target != SQL_INFORMIX)
			{
                Token::Remove(comma, reserve);
				conv = true;
			}
        }

        close = GetNextCharToken(')', L')');
    }
    // No length specified
    else
    {
        // CAST function
        if(clause_scope == SQL_SCOPE_CASE_FUNC)
        {
            // Netezza requires size
            if(_target == SQL_NETEZZA)
			{
                Append(name, "(30)", L"(30)", 4);
				conv = true;
			}
        }
    }

    bool binary = false;

    Token *for_ = GetNextWordToken("FOR", L"FOR", 3);

    // Check for DB2 FOR BIT DATA clause
    if(for_ != NULL)
    {
        Token *bit = GetNextWordToken("BIT", L"BIT", 3);
        Token *data = GetNextWordToken(bit, "DATA", L"DATA", 4);

        if(data != NULL)
        {
            // Remove for other databases
            if(_target != SQL_DB2)
			{
                Token::Remove(for_, data);
				conv = true;
			}

            binary = true;
        }
        else
            PushBack(for_);
    }

    if(_target == SQL_ORACLE)
    {
        // If in a parameter list, remove length specification for Oracle
        if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS)
		{
            Token::Remove(open, close);
			conv = true;
		}
    }

    // If MAX is specified, convert to CLOB types
    if(size->Compare("MAX", L"MAX", 3) == true)
    {
        // Change to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
        {
            Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
            Token::Remove(open, close);
			conv = true;
        }
        else
        // Change to TEXT in PostgreSQL, Redshift
        if(Target(SQL_POSTGRESQL, SQL_REDSHIFT))
        {
            Token::Change(name, "TEXT", L"TEXT", 4);
            Token::Remove(open, close);
			conv = true;
        }
    }
    else
    // Check for VARCHAR FOR BIT DATA
    if(binary == true)
    {
        // Convert DB2 VARCHAR FOR BIT DATA to RAW in Oracle
        if(_target == SQL_ORACLE)
        {
            Token::Change(name, "RAW", L"RAW", 3);
			conv = true;
        }
        else
        // Convert DB2 VARCHAR FOR BIT DATA to VARBINARY in SQL Server
        if(_target == SQL_SQL_SERVER)
		{
            Token::Change(name, "VARBINARY", L"VARBINARY", 9);
			conv = true;
		}
        else
        // Convert DB2 VARCHAR FOR BIT DATA to BYTEA in PostgreSQL, Greenplum
        if(Target(SQL_POSTGRESQL, SQL_GREENPLUM) == true)
        {
            Token::Change(name, "BYTEA", L"BYTEA", 5);
            Token::Remove(open, close);
			conv = true;
        }
    }
    else
    // Convert VARCHAR to VARCHAR2 in Oracle
    if(_target == SQL_ORACLE && _source != SQL_ORACLE)
    {
        Token::Change(name, "VARCHAR2", L"VARCHAR2", 8);
		conv = true;

        // Default size
        if(open == NULL && clause_scope != SQL_SCOPE_FUNC_PARAMS && clause_scope != SQL_SCOPE_PROC_PARAMS)
        {
            // PostgreSQL allows not specify size
            if(_source == SQL_POSTGRESQL)
                Append(name, "(4000)", L"(4000)", 6);
            else
                // Size is 1 by default in SQL Server, Informix, Sybase ASE, Sybase ASA
                Append(name, "(1)", L"(1)", 3);
        }
    }
    else
    // VARCHAR is unlimited by default in PostgreSQL
    if(_target == SQL_POSTGRESQL && open == NULL && _source != SQL_POSTGRESQL)
	{
        Append(name, "(1)", L"(1)", 3);
		conv = true;
	}
    else
    // Size is mandatory in MySQL
    if(Target(SQL_MARIADB, SQL_MYSQL) && open == NULL)
    {
        if(varchar_in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);
        
        // When used in parameter list for a function or procedure add length
        if(_source == SQL_ORACLE && (clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS))
            AppendNoFormat(name, "(4000)", L"(4000)", 6);
        else
            Append(name, "(1)", L"(1)", 3);

		conv = true;
    }
    else
    // STRING in Hive
    if(_source != SQL_HIVE && _target == SQL_HIVE)
    {
        TOKEN_CHANGE(name, "STRING");
        Token::Remove(open, close);
		conv = true;
    }
    else
    // Remove [] for other databases
    if(_target != SQL_SQL_SERVER && varchar_in_braces == true)
	{
        Token::ChangeNoFormat(name, name, 1, name->len - 2);
		conv = true;
	}

	if(!conv && !req_eval)
	{
		STATS_DTL_CONV_NO_NEED(true)
        STATS_ITM_CONV_NO_NEED
	}

    name->data_type = TOKEN_DT_STRING;

	STATS_UPDATE_STATUS

	if(varchar_in_braces)
	{
		DTYPE_STATS_V("VARCHAR", name)
		DTYPE_DTL_STATS_ST("VARCHAR", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// VARCHAR2 in Oracle
bool SqlParser::ParseVarchar2Type(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

    if(name->Compare("VARCHAR2", L"VARCHAR2", 8) == false)
        return false;

	STATS_DECL
	STATS_SET_DESC(SQL_DATATYPE_VARCHAR2_DESC)

	STATS_DTL_DECL
	STATS_ITM_DECL 

    // Size is mandatory in  unless it is a parameter
    Token *open = GetNextCharToken('(', L'(');

    Token *size = NULL;
    Token *semantics = NULL;
    Token *close = NULL;

    if(open != NULL)
        size = GetNextNumberToken();

    // For Oracle, size can be followed by length semantics CHAR or BYTE
    if(size != NULL)
        semantics = GetNextWordToken("BYTE", L"BYTE", 4);

    if(semantics == NULL)
        semantics = GetNextWordToken("CHAR", L"CHAR", 4);

    if(open != NULL)
        close = GetNextCharToken(')', L')');

	if(_target_app == APP_JAVA && (clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS ||
			clause_scope == SQL_SCOPE_VAR_DECL || clause_scope == SQL_SCOPE_OBJ_TYPE_DECL))
		TOKEN_CHANGE_NOFMT(name, "String");
	else
    // Convert to VARCHAR in other databases
    if(_target != SQL_ORACLE)
    {
        Token::Change(name, "VARCHAR", L"VARCHAR", 7);

        // Remove length semantics attribute
        if(semantics != NULL)
            Token::Remove(semantics);

        // When used in parameter list for a function or procedure add length
        if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS)
        {
            if(_target == SQL_NETEZZA)
                Append(name, "(ANY)", L"(ANY)", 5);
            else
                AppendNoFormat(name, "(4000)", L"(4000)", 6);
        }
        else
        // CAST function
        if(clause_scope == SQL_SCOPE_CASE_FUNC)
        {
            // Netezza requires size
            if(_target == SQL_NETEZZA)
                Append(name, "(30)", L"(30)", 4);
        }
		else
		// Inside PL/SQL VARCHAR2 can be up to 32767  even in Oracle 9i
		if(size != NULL)
		{
			int sz = size->GetInt();

			if(_target == SQL_SQL_SERVER && sz > 8000)
				TOKEN_CHANGE_FMT(size, "MAX", name);
		}
    }

    name->data_type = TOKEN_DT_STRING;

	STATS_DTL_CONV_OK(true, STATS_CONV_LOW, "", "")
	STATS_ITM_CONV_OK

	// Collapse reporting items for small VARCHAR2
	if(size != NULL && size->GetInt() <= 128)
	{
		TokenStr typeInfo("VARCHAR2(<=128");

		if(semantics != NULL)
		{
			APPENDSTR(typeInfo, " ");
			typeInfo.Append(semantics);
		}

		APPENDSTR(typeInfo, ")");

		DTYPE_DTL_STATS_ST(typeInfo.str, "VARCHAR(<=128)");
	}
	else
	    DTYPE_DTL_STATS_L(name)

	STATS_UPDATE_STATUS
    DTYPE_STATS(name)

    return true;
}

// VARGRAPHIC in DB2
bool SqlParser::ParseVargraphicType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("VARGRAPHIC", L"VARGRAPHIC", 10) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    // Num is mandatory
    /*Token *open */ (void) GetNextCharToken('(', L'(');
    /*Token *num */ (void) GetNextNumberToken();
    /*Token *close */ (void) GetNextCharToken(')', L')');

    // Convert to NVARCHAR2 in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NVARCHAR2", L"NVARCHAR2", 9);
    else
        // Convert to NVARCHAR in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);
        else
            // Convert to VARCHAR in PostgreSQL
            if(_target == SQL_POSTGRESQL)
                Token::Change(name, "VARCHAR", L"VARCHAR", 7);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// XML in SQL Server, DB2, PostgreSQL, Sybase ASA
bool SqlParser::ParseXmlType(Token *name)
{
    if(name == NULL)
        return false;

    bool xml = false;
    bool xml_in_braces = false;

	STATS_DECL

    if(name->Compare("XML", L"XML", 3) == true)
        xml = true;
    else
        // Can be enclosed in [] for SQL Server
        if(name->Compare("[XML]", L"[XML]", 5) == true)
            xml_in_braces = true;

    if(xml == false && xml_in_braces == false)
        return false;

    // Convert to XMLTYPE in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "XMLTYPE", L"XMLTYPE", 7);
    else
        // Convert to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
            Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
        else
            // Remove [] for other database
            if(_target != SQL_SQL_SERVER && xml_in_braces == true)
                Token::ChangeNoFormat(name, name, 1, name->len - 2);

	STATS_ITM_DECL 

	if(xml_in_braces)
	{
		DTYPE_STATS_V("XML", name)
		DTYPE_DTL_STATS_ST("XML", NULL)
	}
	else
	{
		DTYPE_STATS(name)
		DTYPE_DTL_STATS_L(name)
	}

    return true;
}

// XMLTYPE in Oracle
bool SqlParser::ParseXmltypeType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("XMLTYPE", L"XMLTYPE", 7) == false)
        return false;

	STATS_DECL

    // Convert to XML in SQL Server, DB2, PostgreSQL, Sybase ASA
    if(Target(SQL_SQL_SERVER, SQL_DB2, SQL_POSTGRESQL, SQL_SYBASE_ASA) == true)
        Token::Change(name, "XML", L"XML", 3);
    else
        // Convert to LONGTEXT in MySQL
        if(Target(SQL_MARIADB, SQL_MYSQL))
            Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// YEAR in MySQL
bool SqlParser::ParseYearType(Token *name)
{
    if(name == NULL)
        return false;

    if(name->Compare("YEAR", L"YEAR", 4) == false)
        return false;

	STATS_DECL

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Num is optional in MySQL
    if(open != NULL)
    {
        /*Token *num */ (void) GetNextNumberToken();
        Token *close = GetNextCharToken(')', L')');

        // If target type is not MySQL remove size for YEAR
        if(_target != SQL_MYSQL)
            Token::Remove(open, close);
    }

    // Convert to NUMBER(4) in Oracle
    if(_target == SQL_ORACLE)
        Token::Change(name, "NUMBER(4)", L"NUMBER(4)", 9);
    else
        // Convert to NUMERIC(4) in SQL Server
        if(_target == SQL_SQL_SERVER)
            Token::Change(name, "NUMERIC(4)", L"NUMERIC(4)", 10);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// Parse a user-defined data type
bool SqlParser::ParseUdt(Token *name, int clause_scope)
{
    if(name == NULL)
        return false;

	bool exists = false;

    // Convert to the base type for Oracle
    if(_target == SQL_ORACLE)
    {
        for(ListwmItem *i = _udt.GetFirst(); i != NULL; i = i->next)
        {
            Token *type = (Token*)i->value;
            Token *basetype = (Token*)i->value2;
            Token *basetype_end = (Token*)i->value3;

            if(Token::Compare(type, name) == true)
            {
                Token::Change(name, basetype);

                // Add sizes
                if(clause_scope != SQL_SCOPE_FUNC_PARAMS && clause_scope != SQL_SCOPE_PROC_PARAMS)
                {
                    if(basetype != basetype_end)
                        AppendCopy(name, basetype->next, basetype_end);
                }

				exists = true;
                break;
            }
        }
    }

	// Check for object table type
	if(!exists)
	{
		ListwmItem *i = Find(_spl_obj_type_table, name);

		if(i != NULL)
		{
			Token *data_type = (Token*)i->value2;

			// HashMap<Integer, data_type> in Java
			if(_target_app == APP_JAVA)
			{
				TokenStr dt("HashMap<Integer, ");
				dt.Append(data_type);
				APPENDSTR(dt, ">");

				Token::ChangeNoFormat(name, dt);
			}

			name->data_type = TOKEN_DT_UDT;
			name->data_subtype = TOKEN_DT2_UDT_TAB_SCALAR;

			exists = true;
		}
	}

    return exists;
}