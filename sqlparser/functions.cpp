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

// SQLParser for functions

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "listw.h"

// Parse function 
bool SqlParser::ParseFunction(Token *name)
{
	if(name == NULL)
		return false;

	Token *open = GetNextCharToken('(', L'(');

	if(open == NULL)
		return false;

	bool exists = false;
    bool udt_exists = false;

	if(name->Compare("ABS", L"ABS", 3) == true)
		exists = ParseFunctionAbs(name, open);
	else
	if(name->Compare("ABSVAL", L"ABSVAL", 6) == true)
		exists = ParseFunctionAbsval(name, open);
	else
	if(name->Compare("ACOS", L"ACOS", 4) == true)
		exists = ParseFunctionAcos(name, open);
	else
	if(name->Compare("ADDDATE", L"ADDDATE", 7) == true)
		exists = ParseFunctionAddDate(name, open);
	else
	if(name->Compare("ADD_MONTHS", L"ADD_MONTHS", 10) == true)
		exists = ParseFunctionAddMonths(name, open);
	else
	if(name->Compare("ARGN", L"ARGN", 4) == true)
		exists = ParseFunctionArgn(name, open);
	else
	if(name->Compare("ASCII", L"ASCII", 5) == true)
		exists = ParseFunctionAscii(name, open);
	else
	if(name->Compare("ASCIISTR", L"ASCIISTR", 8) == true)
		exists = ParseFunctionAsciistr(name, open);
	else
	if(name->Compare("ASEHOSTNAME", L"ASEHOSTNAME", 11) == true)
		exists = ParseFunctionAsehostname(name, open);
	else
	if(name->Compare("ASIN", L"ASIN", 4) == true)
		exists = ParseFunctionAsin(name, open);
	else
	if(name->Compare("ATAN", L"ATAN", 4) == true)
		exists = ParseFunctionAtan(name, open);
	else
	if(name->Compare("ATAN2", L"ATAN2", 5) == true)
		exists = ParseFunctionAtan2(name, open);
	else
	if(name->Compare("ATANH", L"ATANH", 5) == true)
		exists = ParseFunctionAtanh(name, open);
	else
	if(name->Compare("ATN2", L"ATN2", 4) == true)
		exists = ParseFunctionAtn2(name, open);
	else
	if(name->Compare("AVG", L"AVG", 3) == true)
		exists = ParseFunctionAvg(name, open);
	else
	if(name->Compare("BASE64_DECODE", L"BASE64_DECODE", 13) == true)
		exists = ParseFunctionBase64Decode(name, open);
	else
	if(name->Compare("BASE64_ENCODE", L"BASE64_ENCODE", 13) == true)
		exists = ParseFunctionBase64Encode(name, open);
	else
	if(name->Compare("BIGINT", L"BIGINT", 6) == true)
		exists = ParseFunctionBigint(name, open);
	else
	if(name->Compare("BIGINTTOHEX", L"BIGINTTOHEX", 11) == true)
		exists = ParseFunctionBiginttohex(name, open);
	else
	if(name->Compare("BIN_TO_NUM", L"BIN_TO_NUM", 10) == true)
		exists = ParseFunctionBinToNum(name, open);
	else
	if(name->Compare("BINTOSTR", L"BINTOSTR", 8) == true)
		exists = ParseFunctionBintostr(name, open);
	else
	if(name->Compare("BITAND", L"BITAND", 6) == true)
		exists = ParseFunctionBitand(name, open);
	else
	if(name->Compare("BITANDNOT", L"BITANDNOT", 9) == true)
		exists = ParseFunctionBitandnot(name, open);
	else
	if(name->Compare("BIT_LENGTH", L"BIT_LENGTH", 10) == true)
		exists = ParseFunctionBitLength(name, open);
	else
	if(name->Compare("BITNOT", L"BITNOT", 6) == true)
		exists = ParseFunctionBitnot(name, open);
	else
	if(name->Compare("BITOR", L"BITOR", 5) == true)
		exists = ParseFunctionBitor(name, open);
	else
	if(name->Compare("BIT_SUBSTR", L"BIT_SUBSTR", 10) == true)
		exists = ParseFunctionBitSubstr(name, open);
	else
	if(name->Compare("BITXOR", L"BITXOR", 6) == true)
		exists = ParseFunctionBitxor(name, open);
	else
	if(name->Compare("BLOB", L"BLOB", 4) == true)
		exists = ParseFunctionBlob(name, open);
	else
	if(name->Compare("BYTE_LENGTH", L"BYTE_LENGTH", 11) == true)
		exists = ParseFunctionByteLength(name, open);
	else
	if(name->Compare("BYTE_SUBSTR", L"BYTE_SUBSTR", 11) == true)
		exists = ParseFunctionByteSubstr(name, open);
	else
	if(name->Compare("CAST", L"CAST", 4) == true)
		exists = ParseFunctionCast(name, open);
	else
	if(name->Compare("CEIL", L"CEIL", 4) == true)
		exists = ParseFunctionCeil(name, open);
	else
	if(name->Compare("CEILING", L"CEILING", 7) == true)
		exists = ParseFunctionCeiling(name, open);
	else
	if(name->Compare("CHAR", L"CHAR", 4) == true)
		exists = ParseFunctionChar(name, open);
	else
	if(name->Compare("CHARACTER_LENGTH", L"CHARACTER_LENGTH", 16) == true ||
		name->Compare("CHAR_LENGTH", L"CHAR_LENGTH", 11) == true)
		exists = ParseFunctionCharacterLength(name, open);
	else
	if(name->Compare("CHARINDEX", L"CHARINDEX", 9) == true)
		exists = ParseFunctionCharindex(name, open);
	else
	if(name->Compare("CHR", L"CHR", 3) == true)
		exists = ParseFunctionChr(name, open);
	else
	if(name->Compare("CLOB", L"CLOB", 4) == true)
		exists = ParseFunctionClob(name, open);
	else
	if(name->Compare("COALESCE", L"COALESCE", 8) == true)
		exists = ParseFunctionCoalesce(name, open);
	else
	if(name->Compare("COL_LENGTH", L"COL_LENGTH", 10) == true)
		exists = ParseFunctionColLength(name, open);
	else
	if(name->Compare("COL_NAME", L"COL_NAME", 8) == true)
		exists = ParseFunctionColName(name, open);
	else
	if(name->Compare("COMPARE", L"COMPARE", 7) == true)
		exists = ParseFunctionCompare(name, open);
	else
	if(name->Compare("CONCAT", L"CONCAT", 6) == true)
		exists = ParseFunctionConcat(name, open);
	else
	if(name->Compare("CONVERT", L"CONVERT", 7) == true)
		exists = ParseFunctionConvert(name, open);
	else
	if(name->Compare("COS", L"COS", 3) == true)
		exists = ParseFunctionCos(name, open);
	else
	if(name->Compare("COSH", L"COSH", 4) == true)
		exists = ParseFunctionCosh(name, open);
	else
	if(name->Compare("COT", L"COT", 3) == true)
		exists = ParseFunctionCot(name, open);
	else
	if(name->Compare("COUNT", L"COUNT", 5) == true)
		exists = ParseFunctionCount(name, open);
	else
	if(name->Compare("CSCONVERT", L"CSCONVERT", 9) == true)
		exists = ParseFunctionCsconvert(name, open);
	else
	if(name->Compare("CURDATE", L"CURDATE", 7) == true)
		exists = ParseFunctionCurdate(name, open);
	else
	if(name->Compare("CURRENT_BIGDATETIME", L"CURRENT_BIGDATETIME", 19) == true)
		exists = ParseFunctionCurrentBigdatetime(name, open);
	else
	if(name->Compare("CURRENT_BIGTIME", L"CURRENT_BIGTIME", 15) == true)
		exists = ParseFunctionCurrentBigtime(name, open);
	else
	if(name->Compare("CURRENT_DATE", L"CURRENT_DATE", 12) == true)
		exists = ParseFunctionCurrentDate(name, open);
	else
	if(name->Compare("CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17) == true)
		exists = ParseFunctionCurrentTimestamp(name, open);
	else
	if(name->Compare("CURSOR_ROWCOUNT", L"CURSOR_ROWCOUNT", 15) == true)
		exists = ParseFunctionCursorRowcount(name, open);
	else
	if(name->Compare("DATALENGTH", L"DATALENGTH", 10) == true)
		exists = ParseFunctionDatalength(name, open);
	else
	if(name->Compare("DATE", L"DATE", 4) == true)
		exists = ParseFunctionDate(name, open);
	else
	if(name->Compare("DATEADD", L"DATEADD", 7) == true)
		exists = ParseFunctionDateadd(name, open);
	else
	if(name->Compare("DATEDIFF", L"DATEDIFF", 8) == true)
		exists = ParseFunctionDatediff(name, open);
	else
	if(name->Compare("DATE_FORMAT", L"DATE_FORMAT", 11) == true)
		exists = ParseFunctionDateFormat(name, open);
	else
	if(name->Compare("DATEFORMAT", L"DATEFORMAT", 10) == true)
		exists = ParseFunctionDateformatASA(name, open);
	else
	if(name->Compare("DATENAME", L"DATENAME", 8) == true)
		exists = ParseFunctionDatename(name, open);
	else
	if(name->Compare("DATEPART", L"DATEPART", 8) == true)
		exists = ParseFunctionDatepart(name, open);
	else
	if(name->Compare("DATE_SUB", L"DATE_SUB", 8) == true)
		exists = ParseFunctionDatesub(name, open);
	else
	if(name->Compare("DATETIME", L"DATETIME", 8) == true)
		exists = ParseFunctionDatetime(name, open);
	else
	if(name->Compare("DAY", L"DAY", 3) == true)
		exists = ParseFunctionDay(name, open);
	else
	if(name->Compare("DAYNAME", L"DAYNAME", 7) == true)
		exists = ParseFunctionDayname(name, open);
	else
	if(name->Compare("DAYOFMONTH", L"DAYOFMONTH", 10) == true)
		exists = ParseFunctionDayofmonth(name, open);
	else
	if(name->Compare("DAYOFWEEK", L"DAYOFWEEK", 9) == true)
		exists = ParseFunctionDayofweek(name, open);
	else
	if(name->Compare("DAYOFWEEK_ISO", L"DAYOFWEEK_ISO", 13) == true)
		exists = ParseFunctionDayofweekIso(name, open);
	else
	if(name->Compare("DAYOFYEAR", L"DAYOFYEAR", 9) == true)
		exists = ParseFunctionDayofyear(name, open);
	else
	if(name->Compare("DAYS", L"DAYS", 4) == true)
		exists = ParseFunctionDays(name, open);
	else
	if(name->Compare("DENSE_RANK", L"DENSE_RANK", 10) == true)
		exists = ParseFunctionDenseRank(name, open);
	else
	if(name->Compare("DBCLOB", L"DBCLOB", 6) == true)
		exists = ParseFunctionDbclob(name, open);
	else
	if(name->Compare("DB_ID", L"DB_ID", 5) == true)
		exists = ParseFunctionDbId(name, open);
	else
	if(name->Compare("DBINFO", L"DBINFO", 6) == true)
		exists = ParseFunctionDbinfo(name, open);
	else
	if(name->Compare("DB_INSTANCEID", L"DB_INSTANCEID", 13) == true)
		exists = ParseFunctionDbInstanceid(name, open);
	else
	if(name->Compare("DB_NAME", L"DB_NAME", 7) == true)
		exists = ParseFunctionDbName(name, open);
	else
	if(name->Compare("DECFLOAT", L"DECFLOAT", 8) == true)
		exists = ParseFunctionDecfloat(name, open);
	else
	if(name->Compare("DECFLOAT_FORMAT", L"DECFLOAT_FORMAT", 15) == true)
		exists = ParseFunctionDecfloatFormat(name, open);
	else
	if(name->Compare("DECIMAL", L"DECIMAL", 7) == true ||
		name->Compare("DEC", L"DEC", 3) == true)
		exists = ParseFunctionDecimal(name, open);
	else
	if(name->Compare("DECODE", L"DECODE", 6) == true)
		exists = ParseFunctionDecode(name, open);
	else
	if(name->Compare("DEGREES", L"DEGREES", 7) == true)
		exists = ParseFunctionDegrees(name, open);
	else
	if(name->Compare("DEREF", L"DEREF", 5) == true)
		exists = ParseFunctionDeref(name, open);
	else
	if(name->Compare("DIGITS", L"DIGITS", 6) == true)
		exists = ParseFunctionDigits(name, open);
	else
	if(name->Compare("DOUBLE", L"DOUBLE", 6) == true ||
		name->Compare("DOUBLE_PRECISION", L"DOUBLE_PRECISION", 16) == true)
		exists = ParseFunctionDouble(name, open);
	else
	if(name->Compare("DOW", L"DOW", 3) == true)
		exists = ParseFunctionDow(name, open);
	else
	if(name->Compare("EMPTY_BLOB", L"EMPTY_BLOB", 10) == true)
		exists = ParseFunctionEmptyBlob(name, open);
	else
	if(name->Compare("EMPTY_CLOB", L"EMPTY_CLOB", 10) == true)
		exists = ParseFunctionEmptyClob(name, open);
	else
	if(name->Compare("EMPTY_DBCLOB", L"EMPTY_DBCLOB", 12) == true)
		exists = ParseFunctionEmptyDbclob(name, open);
	else
	if(name->Compare("EMPTY_NCLOB", L"EMPTY_NCLOB", 11) == true)
		exists = ParseFunctionEmptyNclob(name, open);
	else
	if(name->Compare("ERRORMSG", L"ERRORMSG", 8) == true)
		exists = ParseFunctionErrormsg(name, open);
	else
	if(name->Compare("EXP", L"EXP", 3) == true)
		exists = ParseFunctionExp(name, open);
	else
	if(name->Compare("EXPRTYPE", L"EXPRTYPE", 3) == true)
		exists = ParseFunctionExprtype(name, open);
	else
	if(name->Compare("EXTRACT", L"EXTRACT", 7) == true)
		exists = ParseFunctionExtract(name, open);
	else
	if(name->Compare("FIRST", L"FIRST", 5) == true)
		exists = ParseFunctionFirst(name, open);
	else
	if(name->Compare("FLOAT", L"FLOAT", 5) == true)
		exists = ParseFunctionFloat(name, open);
	else
	if(name->Compare("FLOOR", L"FLOOR", 5) == true)
		exists = ParseFunctionFloor(name, open);
	else
	if(name->Compare("GET_BIT", L"GET_BIT", 7) == true)
		exists = ParseFunctionGetBit(name, open);
	else
	if(name->Compare("GETDATE", L"GETDATE", 7) == true)
		exists = ParseFunctionGetdate(name, open);
	else
	if(name->Compare("GETUTCDATE", L"GETUTCDATE", 10) == true)
		exists = ParseFunctionGetutcdate(name, open);
	else
	if(name->Compare("GREATER", L"GREATER", 7) == true)
		exists = ParseFunctionGreater(name, open);
	else
	if(name->Compare("GREATEST", L"GREATEST", 8) == true)
		exists = ParseFunctionGreatest(name, open);
	else
	if(name->Compare("HASH", L"HASH", 4) == true)
		exists = ParseFunctionHash(name, open);
	else
	if(name->Compare("HEX", L"HEX", 3) == true)
		exists = ParseFunctionHex(name, open);
	else
	if(name->Compare("HEXTOBIGINT", L"HEXTOBIGINT", 11) == true)
		exists = ParseFunctionHextobigint(name, open);
	else
	if(name->Compare("HEXTOINT", L"HEXTOINT", 8) == true)
		exists = ParseFunctionHextoint(name, open);
	else
	if(name->Compare("HEXTORAW", L"HEXTORAW", 8) == true)
		exists = ParseFunctionHextoraw(name, open);
	else
	if(name->Compare("HOST_ID", L"HOST_ID", 7) == true)
		exists = ParseFunctionHostId(name, open);
	else
	if(name->Compare("HOST_NAME", L"HOST_NAME", 9) == true)
		exists = ParseFunctionHostName(name, open);
	else
	if(name->Compare("HOUR", L"HOUR", 4) == true)
		exists = ParseFunctionHour(name, open);
	else
	if(name->Compare("HTML_DECODE", L"HTML_DECODE", 11) == true)
		exists = ParseFunctionHtmlDecode(name, open);
	else
	if(name->Compare("HTML_ENCODE", L"HTML_ENCODE", 11) == true)
		exists = ParseFunctionHtmlEncode(name, open);
	else
	if(name->Compare("HTTP_DECODE", L"HTTP_DECODE", 11) == true)
		exists = ParseFunctionHttpDecode(name, open);
	else
	if(name->Compare("HTTP_ENCODE", L"HTTP_ENCODE", 11) == true)
		exists = ParseFunctionHttpEncode(name, open);

	if(exists)
	{
		name->type = TOKEN_FUNCTION;
		name->open= open;
		name->close = GetLastToken();

		if(_stats != NULL)
			_stats->LogFuncCall(name, GetLastToken(), _option_cur_file);
		
		return exists;
	}

	if(name->Compare("IDENTITY", L"IDENTITY", 8) == true)
		exists = ParseFunctionIdentity(name, open);
	else
	if(name->Compare("IFNULL", L"IFNULL", 6) == true)
		exists = ParseFunctionIfnull(name, open);
	else
	if(name->Compare("IIF", L"IIF", 3) == true)
		exists = ParseFunctionIif(name, open);
	else
	if(name->Compare("INDEX_COL", L"INDEX_COL", 9) == true)
		exists = ParseFunctionIndexCol(name, open);
	else
	if(name->Compare("INDEX_COLORDER", L"INDEX_COLORDER", 14) == true)
		exists = ParseFunctionIndexColorder(name, open);
	else
	if(name->Compare("INDEX_NAME", L"INDEX_NAME", 10) == true)
		exists = ParseFunctionIndexName(name, open);
	else
	if(name->Compare("INITCAP", L"INITCAP", 4) == true)
		exists = ParseFunctionInitcap(name, open);
	else
	if(name->Compare("INSERT", L"INSERT", 6) == true)
		exists = ParseFunctionInsert(name, open);
	else
	if(name->Compare("INSERTSTR", L"INSERTSTR", 9) == true)
		exists = ParseFunctionInsertstr(name, open);
	else
	if(name->Compare("INSTANCE_ID", L"INSTANCE_ID", 11) == true)
		exists = ParseFunctionInstanceId(name, open);
	else
	if(name->Compare("INSTANCE_NAME", L"INSTANCE_NAME", 13) == true)
		exists = ParseFunctionInstanceName(name, open);
	else
	if(name->Compare("INSTR", L"INSTR", 5) == true)
		exists = ParseFunctionInstr(name, open);
	else
	if(name->Compare("INSTRB", L"INSTRB", 6) == true)
		exists = ParseFunctionInstrb(name, open);
	else
	if(name->Compare("INTEGER", L"INTEGER", 7) == true ||
		name->Compare("INT", L"INT", 3) == true)
		exists = ParseFunctionInteger(name, open);
	else
	if(name->Compare("INTTOHEX", L"INTTOHEX", 8) == true)
		exists = ParseFunctionInttohex(name, open);
	else
	if(name->Compare("ISDATE", L"ISDATE", 6) == true)
		exists = ParseFunctionIsdate(name, open);
	else
	if(name->Compare("ISNULL", L"ISNULL", 6) == true)
		exists = ParseFunctionIsnull(name, open);
	else
	if(name->Compare("ISNUMERIC", L"ISNUMERIC", 9) == true)
		exists = ParseFunctionIsnumeric(name, open);
	else
	if(name->Compare("IS_SINGLEUSERMODE", L"IS_SINGLEUSERMODE", 17) == true)
		exists = ParseFunctionIsSingleusermode(name, open);
	else
	if(name->Compare("JULIAN_DAY", L"JULIAN_DAY", 10) == true)
		exists = ParseFunctionJulianDay(name, open);
	else
	if(name->Compare("LAST_DAY", L"LAST_DAY", 8) == true)
		exists = ParseFunctionLastDay(name, open);
	else
	if(name->Compare("LASTAUTOINC", L"LASTAUTOINC", 11) == true)
		exists = ParseFunctionLastAutoInc(name, open);
	else
	if(name->Compare("LCASE", L"LCASE", 5) == true)
		exists = ParseFunctionLcase(name, open);
	else
	if(name->Compare("LEAST", L"LEAST", 5) == true)
		exists = ParseFunctionLeast(name, open);
	else
	if(name->Compare("LEFT", L"LEFT", 4) == true)
		exists = ParseFunctionLeft(name, open);
	else
	if(name->Compare("LEN", L"LEN", 3) == true)
		exists = ParseFunctionLen(name, open);
	else
	if(name->Compare("LENGTH", L"LENGTH", 6) == true)
		exists = ParseFunctionLength(name, open);
	else
	if(name->Compare("LENGTHB", L"LENGTHB", 7) == true)
		exists = ParseFunctionLengthb(name, open);
	else
	if(name->Compare("LESSER", L"LESSER", 6) == true)
		exists = ParseFunctionLesser(name, open);
	else
	if(name->Compare("LIST", L"LIST", 4) == true)
		exists = ParseFunctionList(name, open);
	else
	if(name->Compare("LN", L"LN", 2) == true)
		exists = ParseFunctionLn(name, open);
	else
	if(name->Compare("LOCATE", L"LOCATE", 6) == true)
		exists = ParseFunctionLocate(name, open);
	else
	if(name->Compare("LOCATE_IN_STRING", L"LOCATE_IN_STRING", 16) == true)
		exists = ParseFunctionLocateInString(name, open);
	else
	if(name->Compare("LOG", L"LOG", 3) == true)
		exists = ParseFunctionLog(name, open);
	else
	if(name->Compare("LOG10", L"LOG10", 5) == true)
		exists = ParseFunctionLog10(name, open);
	else
	if(name->Compare("LONG_VARCHAR", L"LONG_VARCHAR", 12) == true)
		exists = ParseFunctionLongVarchar(name, open);
	else
	if(name->Compare("LONG_VARGRAPHIC", L"LONG_VARGRAPHIC", 15) == true)
		exists = ParseFunctionLongVargraphic(name, open);
	else
	if(name->Compare("LOWER", L"LOWER", 5) == true)
		exists = ParseFunctionLower(name, open);
	else
	if(name->Compare("LPAD", L"LPAD", 4) == true)
		exists = ParseFunctionLpad(name, open);
	else
	if(name->Compare("LTRIM", L"LTRIM", 5) == true)
		exists = ParseFunctionLtrim(name, open);
	else
	if(name->Compare("MAX", L"MAX", 3) == true)
		exists = ParseFunctionMax(name, open);
	else
	if(name->Compare("MDY", L"MDY", 3) == true)
		exists = ParseFunctionMdy(name, open);
	else
	if(name->Compare("MICROSECOND", L"MICROSECOND", 11) == true)
		exists = ParseFunctionMicrosecond(name, open);
	else
	if(name->Compare("MIDNIGHT_SECONDS", L"MIDNIGHT_SECONDS", 16) == true)
		exists = ParseFunctionMidnightSeconds(name, open);
	else
	if(name->Compare("MIN", L"MIN", 3) == true)
		exists = ParseFunctionMin(name, open);
	else
	if(name->Compare("MINUTE", L"MINUTE", 6) == true)
		exists = ParseFunctionMinute(name, open);
	else
	if(name->Compare("MOD", L"MOD", 3) == true)
		exists = ParseFunctionMod(name, open);
	else
	if(name->Compare("MONTH", L"MONTH", 5) == true)
		exists = ParseFunctionMonth(name, open);
	else
	if(name->Compare("MONTHNAME", L"MONTHNAME", 9) == true)
		exists = ParseFunctionMonthname(name, open);
	else
	if(name->Compare("MONTHS_BETWEEN", L"MONTHS_BETWEEN", 14) == true)
		exists = ParseFunctionMonthsBetween(name, open);
	else
	if(name->Compare("MULTIPLY_ALT", L"MULTIPLY_ALT", 12) == true)
		exists = ParseFunctionMultiplyAlt(name, open);
	else
	if(name->Compare("NCHAR", L"NCHAR", 5) == true)
		exists = ParseFunctionNchar(name, open);
	else
	if(name->Compare("NCLOB", L"NCLOB", 5) == true)
		exists = ParseFunctionNclob(name, open);
	else
	if(name->Compare("NEWID", L"NEWID", 5) == true)
		exists = ParseFunctionNewid(name, open);
	else
	if(name->Compare("NEWIDSTRING", L"NEWIDSTRING", 11) == true)
		exists = ParseFunctionNewidstring(name, open);
	else
	if(name->Compare("NEXT_DAY", L"NEXT_DAY", 8) == true)
		exists = ParseFunctionNextDay(name, open);
	else
	if(name->Compare("NEXT_IDENTITY", L"NEXT_IDENTITY", 13) == true)
		exists = ParseFunctionNextIdentity(name, open);
	else
	if(name->Compare("NOW", L"NOW", 3) == true)
		exists = ParseFunctionNow(name, open);
	else
	if(name->Compare("NULLIF", L"NULLIF", 6) == true)
		exists = ParseFunctionNullif(name, open);
	else
	if(name->Compare("NUMBER", L"NUMBER", 6) == true)
		exists = ParseFunctionNumber(name, open);
	else
	if(name->Compare("NVARCHAR", L"NVARCHAR", 8) == true)
		exists = ParseFunctionNvarchar(name, open);
	else
	if(name->Compare("NVL", L"NVL", 3) == true)
		exists = ParseFunctionNvl(name, open);
	else
	if(name->Compare("NVL2", L"NVL2", 4) == true)
		exists = ParseFunctionNvl2(name, open);
	else
	if(name->Compare("OBJECT_ID", L"OBJECT_ID", 9) == true)
		exists = ParseFunctionObjectId(name, open);
	else
	if(name->Compare("OBJECT_NAME", L"OBJECT_NAME", 11) == true)
		exists = ParseFunctionObjectName(name, open);
	else
	if(name->Compare("OBJECT_OWNER_ID", L"OBJECT_OWNER_ID", 15) == true)
		exists = ParseFunctionObjectOwnerId(name, open);
	else
	if(name->Compare("OCTET_LENGTH", L"OCTET_LENGTH", 12) == true)
		exists = ParseFunctionOctetLength(name, open);
	else
	if(name->Compare("OVERLAY", L"OVERLAY", 7) == true)
		exists = ParseFunctionOverlay(name, open);
	else
	if(name->Compare("PARTITION_ID", L"PARTITION_ID", 12) == true)
		exists = ParseFunctionPartitionId(name, open);
	else
	if(name->Compare("PARTITION_NAME", L"PARTITION_NAME", 14) == true)
		exists = ParseFunctionPartitionName(name, open);
	else
	if(name->Compare("PARTITION_OBJECT_ID", L"PARTITION_OBJECT_ID", 19) == true)
		exists = ParseFunctionPartitionObjectId(name, open);
	else
	if(name->Compare("PASSWORD_RANDOM", L"PASSWORD_RANDOM", 15) == true)
		exists = ParseFunctionPasswordRandom(name, open);
	else
	if(name->Compare("PATINDEX", L"PATINDEX", 8) == true)
		exists = ParseFunctionPatindex(name, open);
	else
	if(name->Compare("PI", L"PI", 2) == true)
		exists = ParseFunctionPi(name, open);
	else
	if(name->Compare("POSITION", L"POSITION", 8) == true)
		exists = ParseFunctionPosition(name, open);
	else
	if(name->Compare("POSSTR", L"POSSTR", 6) == true)
		exists = ParseFunctionPosstr(name, open);
	else
	if(name->Compare("POWER", L"POWER", 5) == true)
		exists = ParseFunctionPower(name, open);
	else
	if(name->Compare("QUARTER", L"QUARTER", 7) == true)
		exists = ParseFunctionQuarter(name, open);
	else
	if(name->Compare("RADIANS", L"RADIANS", 7) == true)
		exists = ParseFunctionRadians(name, open);
	else
	if(name->Compare("RAISE_ERROR", L"RAISE_ERROR", 11) == true)
		exists = ParseFunctionRaiseError(name, open);
	else
	if(name->Compare("RAISERROR", L"RAISERROR", 9) == true)
		exists = ParseFunctionRaiserror(name, open);
	else
	if(name->Compare("RANK", L"RANK", 4) == true)
		exists = ParseFunctionRank(name, open);
	else
	if(name->Compare("RAND", L"RAND", 4) == true)
		exists = ParseFunctionRand(name, open);
	else
	if(name->Compare("RAND2", L"RAND2", 5) == true)
		exists = ParseFunctionRand2(name, open);
	else
	if(name->Compare("REAL", L"REAL", 4) == true)
		exists = ParseFunctionReal(name, open);
	else
	if(name->Compare("REGEXP_LIKE", L"REGEXP_LIKE", 11) == true)
		exists = ParseFunctionRegexpLike(name, open);
	else
	if(name->Compare("REGEXP_SUBSTR", L"REGEXP_SUBSTR", 13) == true)
		exists = ParseFunctionRegexpSubstr(name, open);
	else
	if(name->Compare("REMAINDER", L"REMAINDER", 9) == true)
		exists = ParseFunctionRemainder(name, open);
	else
	if(name->Compare("REPEAT", L"REPEAT", 6) == true)
		exists = ParseFunctionRepeat(name, open);
	else
	if(name->Compare("REPLACE", L"REPLACE", 7) == true)
		exists = ParseFunctionReplace(name, open);
	else
	if(name->Compare("REPLICATE", L"REPLICATE", 9) == true)
		exists = ParseFunctionReplicate(name, open);
	else
	if(name->Compare("RESERVE_IDENTITY", L"RESERVE_IDENTITY", 16) == true)
		exists = ParseFunctionReserveIdentity(name, open);
	else
	if(name->Compare("REVERSE", L"REVERSE", 7) == true)
		exists = ParseFunctionReverse(name, open);
	else
	if(name->Compare("RIGHT", L"RIGHT", 5) == true)
		exists = ParseFunctionRight(name, open);
	else
	if(name->Compare("ROUND", L"ROUND", 5) == true)
		exists = ParseFunctionRound(name, open);
	else
	if(name->Compare("ROUND_TIMESTAMP", L"ROUND_TIMESTAMP", 15) == true)
		exists = ParseFunctionRoundTimestamp(name, open);
    else
	if(name->Compare("ROW_NUMBER", L"ROW_NUMBER", 10) == true)
		exists = ParseFunctionRowNumber(name, open);
	else
	if(name->Compare("RPAD", L"RPAD", 4) == true)
		exists = ParseFunctionRpad(name, open);
	else
	if(name->Compare("RTRIM", L"RTRIM", 5) == true)
		exists = ParseFunctionRtrim(name, open);

	if(exists)
	{
		name->type = TOKEN_FUNCTION;
		name->open= open;
		name->close = GetLastToken();

		if(_stats != NULL)
			_stats->LogFuncCall(name, GetLastToken(), _option_cur_file);

		return exists;
	}

	if(name->Compare("SCOPE_IDENTITY", L"SCOPE_IDENTITY", 14) == true)
		exists = ParseFunctionScopeIdentity(name, open);
	else
	if(name->Compare("SECOND", L"SECOND", 6) == true)
		exists = ParseFunctionSecond(name, open);
	else
	if(name->Compare("SIGN", L"SIGN", 4) == true)
		exists = ParseFunctionSign(name, open);
	else
	if(name->Compare("SIN", L"SIN", 3) == true)
		exists = ParseFunctionSin(name, open);
	else
	if(name->Compare("SINH", L"SINH", 4) == true)
		exists = ParseFunctionSinh(name, open);
	else
	if(name->Compare("SMALLINT", L"SMALLINT", 8) == true)
		exists = ParseFunctionSmallint(name, open);
	else
	if(name->Compare("SOUNDEX", L"SOUNDEX", 7) == true)
		exists = ParseFunctionSoundex(name, open);
	else
	if(name->Compare("SPACE", L"SPACE", 5) == true)
		exists = ParseFunctionSpace(name, open);
	else
	if(name->Compare("SPID_INSTANCE_ID", L"SPID_INSTANCE_ID", 16) == true)
		exists = ParseFunctionSpidInstanceId(name, open);
	else
	if(name->Compare("SQLERRM", L"SQLERRM", 7) == true)
		exists = ParseFunctionSqlerrm(name, open);
	else
	if(name->Compare("SQRT", L"SQRT", 4) == true)
		exists = ParseFunctionSqrt(name, open);
	else
	if(name->Compare("SQUARE", L"SQUARE", 6) == true)
		exists = ParseFunctionSquare(name, open);
	else
	if(name->Compare("STR", L"STR", 3) == true)
		exists = ParseFunctionStr(name, open);
	else
	if(name->Compare("STRING", L"STRING", 6) == true)
		exists = ParseFunctionString(name, open);
	else
	if(name->Compare("STRIP", L"STRIP", 5) == true)
		exists = ParseFunctionStrip(name, open);
	else
	if(name->Compare("STR_REPLACE", L"STR_REPLACE", 11) == true)
		exists = ParseFunctionStrReplace(name, open);
	else
	if(name->Compare("STRTOBIN", L"STRTOBIN", 11) == true)
		exists = ParseFunctionStrtobin(name, open);
	else
	if(name->Compare("STUFF", L"STUFF", 5) == true)
		exists = ParseFunctionStuff(name, open);
	else
	if(name->Compare("SUBDATE", L"SUBDATE", 7) == true)
		exists = ParseFunctionSubdate(name, open);
	else
	if(name->Compare("SUBSTR", L"SUBSTR", 6) == true)
		exists = ParseFunctionSubstr(name, open);
	else
	if(name->Compare("SUBSTR2", L"SUBSTR2", 7) == true)
		exists = ParseFunctionSubstr2(name, open);
	else
	if(name->Compare("SUBSTRB", L"SUBSTRB", 7) == true)
		exists = ParseFunctionSubstrb(name, open);
	else
	if(name->Compare("SUBSTRING", L"SUBSTRING", 9) == true)
		exists = ParseFunctionSubstring(name, open);
	else
	if(name->Compare("SUM", L"SUM", 3) == true)
		exists = ParseFunctionSum(name, open);
	else
	if(name->Compare("SUSER_ID", L"SUSER_ID", 8) == true)
		exists = ParseFunctionSuserId(name, open);
	else
	if(name->Compare("SUSER_NAME", L"SUSER_NAME", 10) == true)
		exists = ParseFunctionSuserName(name, open);
	else
	if(name->Compare("SWITCHOFFSET", L"SWITCHOFFSET", 12) == true)
		exists = ParseFunctionSwitchoffset(name, open);
	else
	if(name->Compare("SYSDATETIMEOFFSET", L"SYSDATETIMEOFFSET", 17) == true)
		exists = ParseFunctionSysdatetimeoffset(name, open);
	else
	if(name->Compare("SYS_CONTEXT", L"SYS_CONTEXT", 11) == true)
		exists = ParseFunctionSysContext(name, open);
	else
	if(name->Compare("SYS_GUID", L"SYS_GUID", 8) == true)
		exists = ParseFunctionSysGuid(name, open);
	else
	if(name->Compare("TAN", L"TAN", 3) == true)
		exists = ParseFunctionTan(name, open);
	else
	if(name->Compare("TANH", L"TANH", 4) == true)
		exists = ParseFunctionTanh(name, open);
	else
	if(name->Compare("TEXTPTR", L"TEXTPTR", 7) == true)
		exists = ParseFunctionTextptr(name, open);
	else
	if(name->Compare("TEXTVALID", L"TEXTVALID", 9) == true)
		exists = ParseFunctionTextvalid(name, open);
	else
	if(name->Compare("TIME", L"TIME", 4) == true)
		exists = ParseFunctionTime(name, open);
	else
	if(name->Compare("TIMESTAMP", L"TIMESTAMP", 9) == true)
		exists = ParseFunctionTimestamp(name, open);
	else
	if(name->Compare("TIMESTAMPADD", L"TIMESTAMPADD", 12) == true)
		exists = ParseFunctionTimestampadd(name, open);
	else
	if(name->Compare("TIMESTAMPDIFF", L"TIMESTAMPDIFF", 13) == true)
		exists = ParseFunctionTimestampdiff(name, open);
	else
	if(name->Compare("TIMESTAMP_FORMAT", L"TIMESTAMP_FORMAT", 16) == true)
		exists = ParseFunctionTimestampFormat(name, open);
	else
	if(name->Compare("TIMESTAMP_ISO", L"TIMESTAMP_ISO", 13) == true)
		exists = ParseFunctionTimestampIso(name, open);
	else
	if(name->Compare("TO_CHAR", L"TO_CHAR", 7) == true)
		exists = ParseFunctionToChar(name, open);
	else
	if(name->Compare("TO_CLOB", L"TO_CLOB", 7) == true)
		exists = ParseFunctionToClob(name, open);
	else
	if(name->Compare("TO_DATE", L"TO_DATE", 7) == true)
		exists = ParseFunctionToDate(name, open);
	else
	if(name->Compare("TODATETIMEOFFSET", L"TODATETIMEOFFSET", 16) == true)
		exists = ParseFunctionTodatetimeoffset(name, open);
	else
	if(name->Compare("TODAY", L"TODAY", 5) == true)
		exists = ParseFunctionToday(name, open);
	else
	if(name->Compare("TO_LOB", L"TO_LOB", 6) == true)
		exists = ParseFunctionToLob(name, open);
	else
	if(name->Compare("TO_NCHAR", L"TO_NCHAR", 8) == true)
		exists = ParseFunctionToNchar(name, open);
	else
	if(name->Compare("TO_NCLOB", L"TO_NCLOB", 8) == true)
		exists = ParseFunctionToNclob(name, open);
	else
	if(name->Compare("TO_NUMBER", L"TO_NUMBER", 9) == true)
		exists = ParseFunctionToNumber(name, open);
	else
	if(name->Compare("TO_SINGLE_BYTE", L"TO_SINGLE_BYTE", 14) == true)
		exists = ParseFunctionToSingleByte(name, open);
	else
	if(name->Compare("TO_TIMESTAMP", L"TO_TIMESTAMP", 12) == true)
		exists = ParseFunctionToTimestamp(name, open);
	else
	if(name->Compare("TO_TIMESTAMP_TZ", L"TO_TIMESTAMP_TZ", 15) == true)
		exists = ParseFunctionToTimestampTz(name, open);
	else
	if(name->Compare("TO_UNICHAR", L"TO_UNICHAR", 10) == true)
		exists = ParseFunctionToUnichar(name, open);
	else
	if(name->Compare("TRANSLATE", L"TRANSLATE", 9) == true)
		exists = ParseFunctionTranslate(name, open);
	else
	if(name->Compare("TRIM", L"TRIM", 4) == true)
		exists = ParseFunctionTrim(name, open);	
	else
	if(name->Compare("TRUNC", L"TRUNC", 5) == true)
		exists = ParseFunctionTrunc(name, open);	
	else
	if(name->Compare("TRUNCATE", L"TRUNCATE", 8) == true)
		exists = ParseFunctionTruncate(name, open);	
	else
	if(name->Compare("TRUNCNUM", L"TRUNCNUM", 8) == true)
		exists = ParseFunctionTruncnum(name, open);	
	else
	if(name->Compare("TRUNC_TIMESTAMP", L"TRUNC_TIMESTAMP", 15) == true)
		exists = ParseFunctionTruncTimestamp(name, open);	
	else
	if(name->Compare("TSEQUAL", L"TSEQUAL", 7) == true)
		exists = ParseFunctionTsequal(name, open);	
	else
	if(name->Compare("UCASE", L"UCASE", 5) == true)
		exists = ParseFunctionUcase(name, open);	
	else
	if(name->Compare("UHIGHSURR", L"UHIGHSURR", 9) == true)
		exists = ParseFunctionUhighsurr(name, open);	
	else
	if(name->Compare("ULOWSURR", L"ULOWSURR", 8) == true)
		exists = ParseFunctionUlowsurr(name, open);	
	else
	if(name->Compare("UNICODE", L"UNICODE", 7) == true)
		exists = ParseFunctionUnicode(name, open);	
	else
	if(name->Compare("UNISTR", L"UNISTR", 6) == true)
		exists = ParseFunctionUnistr(name, open);	
	else
	if(name->Compare("UPPER", L"UPPER", 5) == true)
		exists = ParseFunctionUpper(name, open);	
	else
	if(name->Compare("USCALAR", L"USCALAR", 7) == true)
		exists = ParseFunctionUscalar(name, open);	
	else
	if(name->Compare("USER_ID", L"USER_ID", 7) == true)
		exists = ParseFunctionUserId(name, open);	
	else
	if(name->Compare("USERENV", L"USERENV", 7) == true)
		exists = ParseFunctionUserenv(name, open);	
	else
	if(name->Compare("USER_NAME", L"USER_NAME", 9) == true)
		exists = ParseFunctionUserName(name, open);	
	else
	if(name->Compare("VALUE", L"VALUE", 5) == true)
		exists = ParseFunctionValue(name, open);	
	else
	if(name->Compare("VARCHAR", L"VARCHAR", 7) == true)
		exists = ParseFunctionVarchar(name, open);	
	else
	if(name->Compare("VARCHAR_BIT_FORMAT", L"VARCHAR_BIT_FORMAT", 18) == true)
		exists = ParseFunctionVarcharBitFormat(name, open);	
	else
	if(name->Compare("VARCHAR_FORMAT", L"VARCHAR_FORMAT", 14) == true)
		exists = ParseFunctionVarcharFormat(name, open);	
	else
	if(name->Compare("VARCHAR_FORMAT_BIT", L"VARCHAR_FORMAT_BIT", 18) == true)
		exists = ParseFunctionVarcharFormatBit(name, open);	
	else
	if(name->Compare("VARGRAPHIC", L"VARGRAPHIC", 10) == true)
		exists = ParseFunctionVargraphic(name, open);	
	else
	if(name->Compare("VSIZE", L"VSIZE", 5) == true)
		exists = ParseFunctionVsize(name, open);	
	else
	if(name->Compare("WEEK", L"WEEK", 4) == true)
		exists = ParseFunctionWeek(name, open);	
	else
	if(name->Compare("WEEK_ISO", L"WEEK_ISO", 8) == true)
		exists = ParseFunctionWeekIso(name, open);	
	else
	if(name->Compare("XMLAGG", L"XMLAGG", 6) == true)
		exists = ParseFunctionXmlagg(name, open);	
	else
	if(name->Compare("XMLATTRIBUTES", L"XMLATTRIBUTES", 13) == true)
		exists = ParseFunctionXmlattributes(name, open);	
	else
	if(name->Compare("XMLCAST", L"XMLCAST", 7) == true)
		exists = ParseFunctionXmlcast(name, open);	
	else
	if(name->Compare("XMLCDATA", L"XMLCDATA", 8) == true)
		exists = ParseFunctionXmlcdata(name, open);	
	else
	if(name->Compare("XMLCOMMENT", L"XMLCOMMENT", 10) == true)
		exists = ParseFunctionXmlcomment(name, open);	
	else
	if(name->Compare("XMLCONCAT", L"XMLCONCAT", 9) == true)
		exists = ParseFunctionXmlconcat(name, open);	
	else
	if(name->Compare("XMLDIFF", L"XMLDIFF", 7) == true)
		exists = ParseFunctionXmldiff(name, open);	
	else
	if(name->Compare("XMLDOCUMENT", L"XMLDOCUMENT", 11) == true)
		exists = ParseFunctionXmldocument(name, open);	
	else
	if(name->Compare("XMLELEMENT", L"XMLELEMENT", 10) == true)
		exists = ParseFunctionXmlelement(name, open);	
	else
	if(name->Compare("XMLEXTRACT", L"XMLEXTRACT", 10) == true)
		exists = ParseFunctionXmlextract(name, open);	
	else
	if(name->Compare("XMLFOREST", L"XMLFOREST", 9) == true)
		exists = ParseFunctionXmlforest(name, open);	
	else
	if(name->Compare("XMLGEN", L"XMLGEN", 6) == true)
		exists = ParseFunctionXmlgen(name, open);	
	else
	if(name->Compare("XMLISVALID", L"XMLISVALID", 10) == true)
		exists = ParseFunctionXmlisvalid(name, open);	
	else
	if(name->Compare("XMLNAMESPACES", L"XMLNAMESPACES", 13) == true)
		exists = ParseFunctionXmlnamespaces(name, open);	
	else
	if(name->Compare("XMLPARSE", L"XMLPARSE", 8) == true)
		exists = ParseFunctionXmlparse(name, open);	
	else
	if(name->Compare("XMLPATCH", L"XMLPATCH", 8) == true)
		exists = ParseFunctionXmlpatch(name, open);	
	else
	if(name->Compare("XMLPI", L"XMLPI", 5) == true)
		exists = ParseFunctionXmlpi(name, open);	
	else
	if(name->Compare("XMLQUERY", L"XMLQUERY", 8) == true)
		exists = ParseFunctionXmlquery(name, open);	
	else
	if(name->Compare("XMLREPRESENTATION", L"XMLREPRESENTATION", 17) == true)
		exists = ParseFunctionXmlrepresentation(name, open);	
	else
	if(name->Compare("XMLROOT", L"XMLROOT", 7) == true)
		exists = ParseFunctionXmlroot(name, open);	
	else
	if(name->Compare("XMLROW", L"XMLROW", 6) == true)
		exists = ParseFunctionXmlrow(name, open);	
	else
	if(name->Compare("XMLSEQUENCE", L"XMLSEQUENCE", 11) == true)
		exists = ParseFunctionXmlsequence(name, open);	
	else
	if(name->Compare("XMLSERIALIZE", L"XMLSERIALIZE", 12) == true)
		exists = ParseFunctionXmlserialize(name, open);	
	else
	if(name->Compare("XMLTEXT", L"XMLTEXT", 7) == true)
		exists = ParseFunctionXmltext(name, open);	
	else
	if(name->Compare("XMLTRANSFORM", L"XMLTRANSFORM", 12) == true)
		exists = ParseFunctionXmltransform(name, open);	
	else
	if(name->Compare("XMLVALIDATE", L"XMLVALIDATE", 11) == true)
		exists = ParseFunctionXmlvalidate(name, open);	
	else
	if(name->Compare("XMLXSROBJECTID", L"XMLXSROBJECTID", 14) == true)
		exists = ParseFunctionXmlxsrobjectid(name, open);	
	else
	if(name->Compare("XSLTRANSFORM", L"XSLTRANSFORM", 12) == true)
		exists = ParseFunctionXsltransform(name, open);	
	else
	if(name->Compare("YEAR", L"YEAR", 4) == true)
		exists = ParseFunctionYear(name, open);
	else
	if(name->Compare("ZEROIFNULL", L"ZEROIFNULL", 10) == true)
		exists = ParseFunctionZeroifnull(name, open);
	else
		udt_exists = ParseUnknownFunction(name, open);

	if(exists || udt_exists)
    {
		name->type = TOKEN_FUNCTION;
		name->open= open;
		name->close = GetLastToken();

		if(udt_exists)
        {
			if(_target == SQL_SQL_SERVER)
				SqlServerConvertUdfIdentifier(name);

			bool obj_ref = false;

			// Check for possible collection item reference in Oracle
			if(_source == SQL_ORACLE && GetVariableOrParameter(name) != NULL)
				obj_ref = true;

			if(!obj_ref)
			{
				STATS_DECL
				UDF_FUNC_STATS(name);
			}
            
			exists = true;
        }
        else
		{
			if(_stats != NULL)
				_stats->LogFuncCall(name, GetLastToken(), _option_cur_file);
		}
    }
	
	return exists;
}

// Unknown function (probably use-defined)
bool SqlParser::ParseUnknownFunction(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	// Handle comma separated expressions until )
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL || next->Compare(')', L')') == true)
			break;

		ParseExpression(next);

		// Must be comma
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	// Must be close (
	Token *close = GetNextCharToken(')', L')');

	return (close != NULL) ? true : false;
}

// Parse functional constant
bool SqlParser::ParseFunctionConstant(Token *name)
{
	if(name == NULL)
		return false;

	bool exists = false;

	if(name->Compare("NULL", L"NULL", 4) == true)
		exists = ParseFunctionNull(name);

	return exists;
}

// Parse function without parameters
bool SqlParser::ParseFunctionWithoutParameters(Token *name)
{
	if(name == NULL)
		return false;

	bool exists = false;

	if(name->Compare("ACTIVITY_COUNT", L"ACTIVITY_COUNT", 14) == true)
		exists = ParseFunctionActivityCount(name);
	else
	if(name->Compare("CURRENT", L"CURRENT", 7) == true)
		exists = ParseFunctionCurrent(name);
	else
	if(name->Compare("CURRENT_DATE", L"CURRENT_DATE", 12) == true)
		exists = ParseFunctionCurrentDate(name);
	else
	if(name->Compare("CURRENT_SCHEMA", L"CURRENT_SCHEMA", 14) == true)
		exists = ParseFunctionCurrentSchema(name);
	else
	if(name->Compare("CURRENT_SQLID", L"CURRENT_SQLID", 13) == true)
		exists = ParseFunctionCurrentSqlid(name);
	else
	if(name->Compare("CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17) == true)
		exists = ParseFunctionCurrentTimestamp(name);
	else
	if(name->Compare("CURRENT_TIME", L"CURRENT_TIME", 12) == true)
		exists = ParseFunctionCurrentTime(name);
	else
	if(name->Compare("CURRENT_USER", L"CURRENT_USER", 12) == true)
		exists = ParseFunctionCurrentUser(name);
	else
	if(name->Compare("DATE", L"DATE", 4) == true)
		exists = ParseFunctionDate(name);
	else
	if(name->Compare("@@IDENTITY", L"@@IDENTITY", 10) == true)
		exists = ParseFunctionIdentity(name);
	else
	if(name->Compare("INTERVAL", L"INTERVAL", 8) == true)
		exists = ParseFunctionInterval(name);
	else
	if(name->Compare("LOCALTIMESTAMP", L"LOCALTIMESTAMP", 14) == true)
		exists = ParseFunctionLocaltimestamp(name);
	else
	if(name->Compare("NEXTVAL", L"NEXTVAL", 7) == true)
		exists = ParseFunctionNextval(name);
	else
	if(name->Compare("@@ROWCOUNT", L"@@ROWCOUNT", 10) == true)
		exists = ParseFunctionRowcount(name);	
	else
	if(name->Compare("SQLCODE", L"SQLCODE", 7) == true)
		exists = ParseFunctionSqlcode(name);
	else
	if(name->Compare("SQLERRM", L"SQLERRM", 7) == true)
		exists = ParseFunctionSqlerrm(name, NULL);
	else
	if(name->Compare("SQLSTATE", L"SQLSTATE", 8) == true)
		exists = ParseFunctionSqlstate(name);
	else
	// SQL%ROWCOUNT
	if(name->Compare("SQL", L"SQL", 3) == true)
		exists = ParseFunctionSqlPercent(name);
	else
	if(name->Compare("SYSDATE", L"SYSDATE", 7) == true)
		exists = ParseFunctionSysdate(name);
	else
	if(name->Compare("SYSTEM_USER", L"SYSTEM_USER", 11) == true)
		exists = ParseFunctionSystemUser(name);
	else
	if(name->Compare("SYSTIMESTAMP", L"SYSTIMESTAMP", 12) == true)
		exists = ParseFunctionSystimestamp(name);	
	else
	if(name->Compare("TIME", L"TIME", 4) == true)
		exists = ParseFunctionTime(name);	
	else
	if(name->Compare("TODAY", L"TODAY", 5) == true)
		exists = ParseFunctionToday(name);	
	else
	if(name->Compare("USER", L"USER", 4) == true)
		exists = ParseFunctionUser(name);
	else
	if(name->Compare("_UTF8", L"_UTF8", 5) == true)
		exists = ParseFunctionUtf8(name);

    if(exists)
		name->type = TOKEN_FUNCTION;

	return exists;
}

// ABS in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionAbs(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_ABS_DESC)

	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ GetNextCharToken(')', L')');

	STATS_SET_CONV_NO_NEED(true)
	FUNC_STATS(name);
	return true;
}

// ABSVAL in DB2
bool SqlParser::ParseFunctionAbsval(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to ABS in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "ABS", L"ABS", 3);

	FUNC_STATS(name);
	return true;
}

// ACOS in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionAcos(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// ADDDATE in MySQL
bool SqlParser::ParseFunctionAddDate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse first datetime expression
	ParseExpression(date);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *interval = GetNextToken();

	// Parse interval
	ParseExpression(interval);

	Token *close = GetNextCharToken(')', L')');

	// date + interval in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Remove(name);

		Token::Change(comma, " +", L" +", 2);

		Token::Remove(open);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;

}

// ADD_MONTHS in Oracle, DB2
bool SqlParser::ParseFunctionAddMonths(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_ADD_MONTHS_DESC)

	Token *date = GetNextToken();

	// Parse date
	ParseExpression(date);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *num = GetNextToken();

	// Parse number of months
	ParseExpression(num);

	Token *end_num = GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to DATEADD in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "DATEADD", L"DATEADD", 7);

		Append(open, "MONTH, ", L"MONTH, ", 7, name);
		AppendCopy(open, num, end_num);
		Append(open, ", ", L", ", 2);
		
		Token::Remove(comma, end_num);
	}
	else
	// Convert to TIMESTAMPADD in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "TIMESTAMPADD", L"TIMESTAMPADD", 12);

		Append(open, "MONTH, ", L"MONTH, ", 7, name);
		AppendCopy(open, num, end_num);
		Append(open, ", ", L", ", 2);
		
		Token::Remove(comma, end_num);
	}

	FUNC_STATS(name);
	return true;
}

// ARGN in Sybase ASA
bool SqlParser::ParseFunctionArgn(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *position = GetNextToken();

	// Parse position
	ParseExpression(position);

	Token *end_pos = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	int num = 1;

	// Variable number of parameters
	while(true)
	{
		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);

		Token *comman = GetNextCharToken(',', L',');

		// Convert to CASE expression in other databases
		if(_target != SQL_SYBASE_ASA)
		{
			Token *when = Prepend(expn, " WHEN ", L" WHEN ", 6, name);
			AppendCopy(when, position, end_pos);
			Prepend(expn, " = ", L" = ", 3);

			Token *numt = Prepend(expn, "0", L"0", 1);
			Token::Change(numt, num);
			num++;

			Prepend(expn, " THEN ", L" THEN ", 6, name);
			Token::Remove(comman);
		}

		if(comman == NULL)
			break;
	}

	Token *close = GetNextCharToken(')', L')');

	// Convert to CASE expression in other databases
	if(_target != SQL_SYBASE_ASA)
	{
		Token::Change(name, "CASE", L"CASE", 4);
		Token::Remove(open, comma);

		Prepend(close, " END", L" END", 4, name);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// ASCII in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionAscii(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_ASCII_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	name->data_type = TOKEN_DT_STRING;
	
	FUNC_STATS(name);
	return true;
}

// ASCIISTR in Oracle
bool SqlParser::ParseFunctionAsciistr(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// ASEHOSTNAME in Sybase ASE
bool SqlParser::ParseFunctionAsehostname(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to HOST_NAME() in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "HOST_NAME", L"HOST_NAME", 9);
	else
	// Convert to SYS_CONTEXT('USERENV', 'SERVER_HOST') in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SYS_CONTEXT('USERENV', 'SERVER_HOST'", L"SYS_CONTEXT('USERENV', 'SERVER_HOST'", 36);
		Token::Remove(open);
	}
	else
	if(Target(SQL_MYSQL, SQL_MARIADB))
	{
		TOKEN_CHANGE(name, "@@hostname");
		Token::Remove(open, close);
	}

	FUNC_STATS(name);
	return true;
}

// ASIN in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionAsin(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// ATAN in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionAtan(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// ATAN2 in Oracle, DB2, MySQL, Sybase ASA
bool SqlParser::ParseFunctionAtan2(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num1 = GetNextToken();

	// Parse first numeric expression
	ParseExpression(num1);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *num2 = GetNextToken();

	// Parse second numeric expression
	ParseExpression(num2);

	Token *end_num2 = GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// DB2 and Oracle (Sybase ASA) have different parameter order
	if(_source == SQL_DB2 && Target(SQL_ORACLE, SQL_SYBASE_ASA) == true)
	{
		AppendCopy(open, num2, end_num2);
		Append(open, ", ", L", ", 2);

		Token::Remove(comma, end_num2);
	}
	else
	// Convert to ATN2 in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "ATN2", L"ATN2", 4);

	FUNC_STATS(name);
	return true;
}

// DB2 ATANH
bool SqlParser::ParseFunctionAtanh(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// ATN2 in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionAtn2(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num1 = GetNextToken();

	// Parse first numeric expression
	ParseExpression(num1);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *num2 = GetNextToken();

	// Parse second numeric expression
	ParseExpression(num2);

	/*Token *close */ GetNextCharToken(')', L')');

	// Convert to ATAN2 in Oracle, MariaDB, MySQL
	if(Target(SQL_ORACLE, SQL_MYSQL, SQL_MARIADB))
		TOKEN_CHANGE(name, "ATAN2");

	FUNC_STATS(name);
	return true;
}

// BASE64_DECODE in Sybase ASA
bool SqlParser::ParseFunctionBase64Decode(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *base64_string = GetNextToken();

	if(base64_string == NULL)
		return false;

	// Parse base64_string
	ParseExpression(base64_string);

	Token *close = GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		Prepend(name, "UTL_RAW.CAST_TO_VARCHAR2(UTL_ENCODE.", L"UTL_RAW.CAST_TO_VARCHAR2(UTL_ENCODE.", 36, name);
		Append(name, "(UTL_RAW.CAST_TO_RAW", L"(UTL_RAW.CAST_TO_RAW", 20, name);
		Append(close, "))", L"))", 2);
	}

	FUNC_STATS(name);
	return true;
}

// BASE64_ENCODE in Sybase ASA
bool SqlParser::ParseFunctionBase64Encode(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	Token *close = GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		Prepend(name, "UTL_RAW.CAST_TO_VARCHAR2(UTL_ENCODE.", L"UTL_RAW.CAST_TO_VARCHAR2(UTL_ENCODE.", 36, name);
		Append(name, "(UTL_RAW.CAST_TO_RAW", L"(UTL_RAW.CAST_TO_RAW", 20, name);
		Append(close, "))", L"))", 2);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 BIGINT
bool SqlParser::ParseFunctionBigint(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRUNC(TO_NUMBER", L"TRUNC(TO_NUMBER", 15);
		Append(close, ")", L")", 1);
	}

	FUNC_STATS(name);
	return true;
}

// BIGINTTOHEX in Sybase ASE
bool SqlParser::ParseFunctionBiginttohex(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	// Convert to CONVERT in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CONVERT(VARBINARY(8), CAST(", L"CONVERT(VARBINARY(8), CAST(", 26);
		Prepend(close, " AS BIGINT)", L" AS BIGINT)", 11);
	}
	else
	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CHAR(ABS", L"TO_CHAR(ABS", 11);
		Prepend(close, "), 'XXXXXXXXXXXXXXXXXXX'", L"), 'XXXXXXXXXXXXXXXXXXX'", 24);
	}
	else
	if(Target(SQL_MYSQL, SQL_MARIADB))
	{
		TOKEN_CHANGE(name, "LPAD(HEX");
		PREPEND_NOFMT(close, "), 16, '0'");
	}

	FUNC_STATS(name);
	return true;
}

// BIN_TO_NUM in Oracle
bool SqlParser::ParseFunctionBinToNum(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *bit = GetNextToken();

	if(bit == NULL)
		return false;

	// Parse first bit expression
	ParseExpression(bit);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *bitn = GetNextToken();

		if(bitn == NULL)
			break;

		// Parse next bit expression
		ParseExpression(bitn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// BINTOSTR in Sybase ASE
bool SqlParser::ParseFunctionBintostr(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(Target(SQL_MARIADB, SQL_MYSQL))
		TOKEN_CHANGE(name, "HEX");

	FUNC_STATS(name);
	return true;
}

// BITAND in Oracle, DB2
bool SqlParser::ParseFunctionBitand(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_BITAND_DESC)

	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// User & function in MySQL
	if(Target(SQL_MARIADB, SQL_MARIADB_ORA, SQL_MYSQL))
	{
		Token::Remove(name);
		Token::Change(comma, " &", L" &", 2);

		STATS_SET_CONV_OK(true)
		STATS_SET_COMPLEXITY(true, STATS_CONV_LOW)
	}

	FUNC_STATS(name);
	return true;
}

// DB2 BITANDNOT
bool SqlParser::ParseFunctionBitandnot(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	/*Token *comma */ (void) GetNextCharToken(',', L',');

	Token *exp2 = GetNextToken();

	if(exp2 == NULL)
		return false;

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// BIT_LENGTH in Sybase ASA
bool SqlParser::ParseFunctionBitLength(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *bit_string = GetNextToken();

	if(bit_string == NULL)
		return false;

	// Parse bit_string
	ParseExpression(bit_string);

	Token *close = GetNextCharToken(')', L')');

	// Convert to LENGTH * 8 in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "LENGTH", L"LENGTH", 6);
		Append(close, " * 8", L" * 8", 4);
	}
	else
	// Convert to LEN * 8 in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "LEN", L"LEN", 3);
		Append(close, " * 8", L" * 8", 4);
	}

	FUNC_STATS(name);
	return true;
}

// Oracle, DB2 BITNOT
bool SqlParser::ParseFunctionBitnot(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	/*Token *comma */ (void) GetNextCharToken(',', L',');

	Token *exp2 = GetNextToken();

	if(exp2 == NULL)
		return false;

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 BITOR
bool SqlParser::ParseFunctionBitor(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	/*Token *comma */ (void) GetNextCharToken(',', L',');

	Token *exp2 = GetNextToken();

	if(exp2 == NULL)
		return false;

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// BIT_SUBSTR in Sybase ASA
bool SqlParser::ParseFunctionBitSubstr(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *bit_string = GetNextToken();

	// Parse bit_string
	ParseExpression(bit_string);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *start = GetNextToken();

	// Parse start
	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');
	Token *length = NULL;

	// Length is optional
	if(comma2 != NULL)
	{
		length = GetNextToken();

		// Parse length
		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to SUBSTR in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "SUBSTR", L"SUBSTR", 6);
	else
	// Convert to SUBSTRING in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "SUBSTRING", L"SUBSTRING", 9);

	FUNC_STATS(name);
	return true;
}

// DB2 BITXOR
bool SqlParser::ParseFunctionBitxor(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	/*Token *comma */ (void) GetNextCharToken(',', L',');

	Token *exp2 = GetNextToken();

	if(exp2 == NULL)
		return false;

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 BLOB
bool SqlParser::ParseFunctionBlob(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_BLOB in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_BLOB", L"TO_BLOB", 7);

		// Second parameter is removed
		Token::Remove(comma);
		Token::Remove(exp2);
	}

	FUNC_STATS(name);
	return true;
}

// BYTE_LENGTH in Sybase ASA
bool SqlParser::ParseFunctionByteLength(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LENGTHB in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "LENGTHB", L"LENGTHB", 7);
	else
	// Convert to DATALENGTH in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "DATALENGTH", L"DATALENGTH", 10);

	FUNC_STATS(name);
	return true;
}

// BYTE_SUBSTR in Sybase ASA
bool SqlParser::ParseFunctionByteSubstr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *start = GetNextToken();

	// Parse start
	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');
	Token *length = NULL;

	// Length is optional
	if(comma2 != NULL)
	{
		length = GetNextToken();

		// Parse length
		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to SUBSTRB in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "SUBSTRB", L"SUBSTRB", 7);
	else
	// Convert to SUBSTRING in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "SUBSTRING", L"SUBSTRING", 9);

	FUNC_STATS(name);
	return true;
}

// ACTIVITY_COUNT in Teradata
bool SqlParser::ParseFunctionActivityCount(Token *activity_count)
{
	if(activity_count == NULL)
		return false;

	STATS_DECL
	// SQL%ROWCOUNT in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(activity_count, "SQL%ROWCOUNT", L"SQL%ROWCOUNT", 12);

	FUNC_STATS(activity_count);
	return true;
}

// CAST function
bool SqlParser::ParseFunctionCast(Token *cast, Token *open)
{
	if(cast == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Check for CAST(TIMEOFDAY() AS TIMESTAMP) pattern in PostgreSQL
	if(ParseCastTimeofdayAsTimestamp(cast, open, exp) == true)
        return true;

	// Parse expression
	ParseExpression(exp);

	Token *as = GetNextWordToken("AS", L"AS", 2);

	if(as == NULL)
		return false;

	Token *type = GetNextToken();

	// Parse cast data type
	ParseDataType(type, SQL_SCOPE_CASE_FUNC);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// CAST AS VARCHAR
	if(type->Compare("VARCHAR", L"VARCHAR", 7) == true)
	{
		// Convert to TO_CHAR in Oracle
		if(_source != SQL_ORACLE && _target == SQL_ORACLE)
		{
			Token::Change(cast, "TO_CHAR", L"TO_CHAR", 7);
			Token::Remove(as, type);
		}
	}
	else
	// CAST AS DATE
	if(type->Compare("DATE", L"DATE", 4) == true)
	{
		if(_source != SQL_ORACLE && _target == SQL_ORACLE)
		{
			TokenStr format(", ", L", ", 2);

			// If string litral is specified try to recognize format, and convert to TO_DATE
			if(exp->type == TOKEN_STRING && RecognizeOracleDateFormat(exp, format) == true)
			{
				Token::Change(cast, "TO_DATE", L"TO_DATE", 7);

				AppendNoFormat(exp, &format);

				Token::Remove(as, type);
			}
		}
	}

	FUNC_STATS(cast);
	return true;
}

// CEIL in Oracle, DB2, MySQL, Sybase ASA
bool SqlParser::ParseFunctionCeil(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_CEIL_DESC)

	// Get dollar sign if exists
	Token *sign = GetNextCharToken('$', L'$');
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Remove dollar sign for Oracle CEIL
	if(_target == SQL_ORACLE && sign != NULL)
		Token::Remove(sign);
	else
	// Convert to CEILING in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "CEILING", L"CEILING", 7);

	STATS_SET_CONV_NO_NEED(Target(SQL_MARIADB, SQL_MARIADB_ORA))  
	FUNC_STATS(name);

	return true;
}

// CEILING in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionCeiling(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Get dollar sign if exists
	Token *sign = GetNextCharToken('$', L'$');
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to CEIL in Oracle
	if(_target == SQL_ORACLE)
	{
		// Remove dollar sign for Oracle CEIL
		if(sign != NULL)
			Token::Remove(sign);

		Token::Change(name, "CEIL", L"CEIL", 4);
	}

	FUNC_STATS(name);
	return true;
}

// CHAR in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionChar(Token *name, Token* open)
{
	if(name == NULL)
		return false;

	// Possible conflict with data type, so check the source
	// Do not check that previous is a word as SELECT CHAR(), THEN CHAR etc. are possible
	if(Source(SQL_SQL_SERVER, SQL_DB2, SQL_SYBASE, SQL_SYBASE_ASA) == false)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *exp_end */ (void) GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	Token *format = NULL;
	
	// Format is optional in DB2
	if(comma != NULL)
	{
		format = GetNextToken();

		if(format == NULL)
			return false;

		// Parse format
		ParseExpression(format);
	}

	Token *close = GetNextCharToken(')', L')');

	if(_source == SQL_DB2)
	{
		if(format != NULL)
		{
			// If a number constant it is CHAR(string, num) - truncate or right pad string to num
			if(format->IsNumeric() == true)
			{
				// Convert to RPAD(SUBSTR(string, 1, num), num) in Oracle
				if(_target == SQL_ORACLE)
				{
					Token::Change(name, "RPAD(SUBSTR", L"RPAD(SUBSTR", 11);

					AppendNoFormat(comma, " 1,", L" 1,", 3);

					AppendNoFormat(close, ", ", L", ", 2);
					AppendCopy(close, format);
					AppendNoFormat(close, ")", L")", 1);
				}
			}
			else
			// ISO or JIS datetime format YYYY-MM-DD
			if(format->Compare("ISO", L"ISO", 3) || format->Compare("JIS", L"JIS", 3))
			{
				if(_target == SQL_SQL_SERVER)
				{
					TOKEN_CHANGE(name, "CONVERT");
					APPEND_FIRST_FMT(open, "VARCHAR, ", name);
					TOKEN_CHANGE(format, "120");
				}
			}
			else
			// USA datetime format MM/DD/YYYY
			if(format->Compare("USA", L"USA", 3) == true)
			{
				if(_target == SQL_ORACLE)
				{
					Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);
					Token::Change(format, "'MM/DD/YYYY'", L"'MM/DD/YYYY'", 12);
				}
				else
				if(_target == SQL_SQL_SERVER)
				{
					TOKEN_CHANGE(name, "CONVERT");
					APPEND_FIRST_FMT(open, "VARCHAR, ", name);
					TOKEN_CHANGE(format, "101");
				}
			}
			else
			// EUR datetime format DD.MM.YYYY
			if(format->Compare("EUR", L"EUR", 3) == true)
			{
				if(_target == SQL_SQL_SERVER)
				{
					TOKEN_CHANGE(name, "CONVERT");
					APPEND_FIRST_FMT(open, "VARCHAR, ", name);
					TOKEN_CHANGE(format, "104");
				}
			}
		}
		// No format specified
		else
		{
			if(_target == SQL_ORACLE)
				Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);
		}
	}
	else
		Token::Change(name, "CHR", L"CHR", 3);

	FUNC_STATS(name);
	return true;
}

// CHAR_LENGTH in DB2, Sybase ASE, Sybase ASA
// CHARACTER_LENGTH in DB2
bool SqlParser::ParseFunctionCharacterLength(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *unit = NULL;

	// Second parameter is optional in DB2
	if(comma != NULL)
		// Second parameter specifies code units CODEUNITS16, CODEUNITS32, OCTETS	
		unit = GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LENGTH in Oracle
	if(_target == SQL_ORACLE)
	{
		if(unit != NULL)
		{
			// UTF-16
			if(Token::Compare(unit, "CODEUNITS16", L"CODEUNITS16", 11))
				Token::Change(name, "LENGTH2", L"LENGTH2", 7);
			else
			// UTF-32
			if(Token::Compare(unit, "CODEUNITS32", L"CODEUNITS32", 11))
				Token::Change(name, "LENGTH4", L"LENGTH4", 7);
			else
			// In bytes
			if(Token::Compare(unit, "OCTETS", L"OCTETS", 6))
				Token::Change(name, "LENGTHB", L"LENGTHB", 7);

			// Unit is removed
			Token::Remove(comma, unit);
		}
		else
			Token::Change(name, "LENGTH", L"LENGTH", 6);
	}
	else
	// Convert to LEN in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "LEN", L"LEN", 3);

	FUNC_STATS(name);
	return true;
}

// CHARINDEX in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionCharindex(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *substring = GetNextToken();

	// Parse substring
	ParseExpression(substring);

	Token *substring_end = GetLastToken();

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma2 = GetNextCharToken(',', L',');

	// Start is optional in SQL Server, Sybase ASE
	if(comma2 != NULL)
	{
		Token *start = GetNextToken();

		if(start == NULL)
			return false;

		// Parse start
		ParseExpression(start);		
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to INSTR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "INSTR", L"INSTR", 5);
		AppendCopy(open, string);
		Append(open, ", ", L", ", 2);

		Token::Remove(comma, string);
	}
	else
	if(Target(SQL_MYSQL, SQL_MARIADB))
		TOKEN_CHANGE(name, "LOCATE"); 
	else
	// Convert to POSITION in PostgreSQL
	if(_target == SQL_POSTGRESQL)
	{
		Token::Change(name, "POSITION", L"POSITION", 8);

		// Replace comma with IN
		Append(substring_end, " IN", L" IN", 3, name);
		Token::Change(comma, " ", L" ", 1);
	}

	FUNC_STATS(name);
	return true;
}

// CHR in Oracle, DB2
bool SqlParser::ParseFunctionChr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_CHR_DESC)

	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	Token *usng = GetNextWordToken("USING", L"USING", 5);
	Token *nchar = NULL;

	// USING NCHAR_CS in optional in Oracle
	if(usng != NULL)
		nchar = GetNextWordToken("NCHAR_CS", L"NCHAR_CS", 8);	

	Token *close = GetNextCharToken(')', L')');

	// Convert to CHAR in SQL Server, MySQL
	if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "CHAR", L"CHAR", 4);

		if(usng == NULL && Target(SQL_MARIADB, SQL_MYSQL))
			Prepend(close, " USING ASCII", L" USING ASCII", 12, name);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 CLOB
bool SqlParser::ParseFunctionClob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	// Possible conflict with data type, so check the source
	if(_source != SQL_DB2)
		return false;

	STATS_DECL
	Token *prev = GetPrevToken(name);

	// For DB2 make sure the previous token is not a column name
	if(prev != NULL && (prev->type == TOKEN_WORD || prev->type == TOKEN_IDENT))
		return false;

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CLOB in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CLOB", L"TO_CLOB", 7);

		// Second parameter is removed
		Token::Remove(comma);
		Token::Remove(exp2);
	}

	FUNC_STATS(name);
	return true;
}

// COALESCE in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionCoalesce(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_COALESCE_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	STATS_SET_CONV_NO_NEED(true)  
	FUNC_STATS(name);

	return true;
}

// COL_LENGTH in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionColLength(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *table = GetNextToken();

	if(table == NULL)
		return false;

	// Parse table
	ParseExpression(table);

	/*Token *comma */ (void) GetNextCharToken(',', L',');
	Token *column = GetNextToken();

	if(column == NULL)
		return false;

	// Parse column
	ParseExpression(column);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// COL_NAME in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionColName(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *table_id = GetNextToken();

	if(table_id == NULL)
		return false;

	// Parse table_id
	ParseExpression(table_id);

	/*Token *comma */ (void) GetNextCharToken(',', L',');
	Token *column_id = GetNextToken();

	if(column_id == NULL)
		return false;

	// Parse column_id
	ParseExpression(column_id);

	Token *comma2 = GetNextCharToken(',', L',');
	Token *database_id = NULL;

	// Database_id is optional in Sybase ASE
	if(comma2 != NULL)
	{
		database_id = GetNextToken();

		if(database_id == NULL)
			return false;

		// Parse database_id
		ParseExpression(database_id);

		// If target type is not Sybase ASE remove database_id
		if(_target != SQL_SYBASE)
			Token::Remove(comma2, database_id);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// COMPARE in Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionCompare(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *end_exp = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	Token *end_exp2 = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	Token *collation = NULL;

	// Collation is optional
	if(comma2 != NULL)
	{
		collation = GetNextToken();

		if(collation == NULL)
			return false;
	}

	Token *close = GetNextCharToken(')', L')');

	// Convert to CASE expression in other databases
	if(Target(SQL_SYBASE, SQL_SYBASE_ASA) == false)
	{
		if(collation == NULL)
		{
			Prepend(exp, " WHEN ", L" WHEN ", 6, name);
			Append(end_exp, " = ", L" = ", 3);
			AppendCopy(end_exp, exp2, end_exp2);
			Append(end_exp, " THEN 0", L" THEN 0", 7, name);

			Append(comma, " WHEN ", L" WHEN ", 6, name);
			AppendCopy(comma, exp, end_exp);
			Append(comma, " <", L" <", 2);
			Append(end_exp2, " THEN -1 ELSE 1 END", L" THEN -1 ELSE 1 END", 19, name);

			Token::Change(name, "CASE", L"CASE", 4);
			Token::Remove(open);
			Token::Remove(comma);
			Token::Remove(close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// CONCAT in Oracle, DB2, MySQL
bool SqlParser::ParseFunctionConcat(Token *concat, Token *open)
{
	if(concat == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_CONCAT_DESC)

	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	Token *comma1 = GetNextCharToken(',', L',');

	if(comma1 == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	int params = 2;

	// Optional varying list of expressions in MySQL
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);

		// Oracle supports only 2 parameters in CONCAT, replace function with || operator
		if(_target == SQL_ORACLE)
			Token::Change(comma, " ||", L" ||", 3);

		params++;
	}

	Token *close = GetNextCharToken(')', L')');

	// Replace function with || 
	if(_target == SQL_ORACLE && params > 2)
	{
		Token::Remove(concat, open);
		Token::Remove(close);

		Token::Change(comma1, " ||", L" ||", 3);
	}

	STATS_SET_CONV_NO_NEED(Target(SQL_MARIADB, SQL_MARIADB_ORA)) 

	FUNC_STATS(concat);
	return true;
}

// CONVERT in Oracle, SQL Server, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionConvert(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	// MySQL, Sybase ADS use different order CONVERT(exp, type)
	if(Source(SQL_MYSQL, SQL_SYBASE_ADS))
		return ParseFunctionConvertMySql(name, open);

	STATS_DECL
	// First is a keyword specifying data type in SQL Server, Sybase ASE, Sybase ASA
	Token *datatype = GetNextToken();

	if(datatype == NULL)
		return false;

	if(TOKEN_CMP(datatype, "DATETIME") || TOKEN_CMP(datatype, "SMALLDATETIME"))
		ParseFunctionConvertDatetime(name, open, datatype);
	else
	if(TOKEN_CMP(datatype, "IMAGE"))
		ParseFunctionConvertImage(name, open, datatype);
	else
	if(TOKEN_CMP(datatype, "INTEGER"))
		ParseFunctionConvertInteger(name, open, datatype);
	else
	if(TOKEN_CMP(datatype, "TEXT"))
		ParseFunctionConvertText(name, open, datatype);
	else
	if(TOKEN_CMP(datatype, "VARCHAR") || TOKEN_CMP(datatype, "CHAR"))
		ParseFunctionConvertVarchar(name, open, datatype);
	else
	// Oracle CONVERT has a different meaning than in other databases
	if(_source == SQL_ORACLE)
	{
		Token *comma1 = GetNextCharToken(',', L',');

		if(comma1 == NULL)
			return false;

		/*Token *dest_charset */ (void) GetNextToken();
		Token *comma2 = GetNextCharToken(',', L',');
		Token *source_charset = NULL;

		if(comma2 != NULL)
			source_charset = GetNextToken();

		/*Token *close */ (void) GetNextCharToken(')', L')');

		// Change syntax in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			if(comma2 == NULL)
			{
				Token::Change(comma1, " USING", L" USING", 6);
			}
		}
	}

	FUNC_STATS(name);
	return true;
}

// CONVERT function in MySQL, Sybase ADS
bool SqlParser::ParseFunctionConvertMySql(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Expression goes first
	Token *exp = GetNextToken();

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	// Data type
	Token *datatype = GetNextToken();

	// Sybase ADS has specific constants for target type
	if(_source == SQL_SYBASE_ADS)
	{
		if(TOKEN_CMP(datatype, "SQL_CHAR"))
			TOKEN_CHANGE(datatype, "VARCHAR");
		else
		if(TOKEN_CMP(datatype, "SQL_DATE"))
			TOKEN_CHANGE(datatype, "DATE");
		else
		if(TOKEN_CMP(datatype, "SQL_INTEGER"))
			TOKEN_CHANGE(datatype, "INTEGER");
		else
		if(TOKEN_CMP(datatype, "SQL_TIME"))
		{
			if(_target == SQL_SQL_SERVER)
				TOKEN_CHANGE(datatype, "TIME(3)");
		}
		else
		// Datetime with milliseconds
		if(TOKEN_CMP(datatype, "SQL_TIMESTAMP"))
		{
			if(_target == SQL_SQL_SERVER)
				TOKEN_CHANGE(datatype, "DATETIME2(3)");
			else
				TOKEN_CHANGE(datatype, "TIMESTAMP");
		}
	}
	else
		ParseDataType(datatype, SQL_SCOPE_CASE_FUNC);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Other databases use different order in CONVERT; Oracle does not support such conversion function
	if(!Target(SQL_MYSQL, SQL_SYBASE_ADS))
	{
		Token::Change(name, "CAST", L"CAST", 4);
		Token::Change(comma, " AS ", L" AS ", 4, name);
	}

	FUNC_STATS(name);
	return true;
}

// CONVERT to DATETIME in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionConvertDatetime(Token *name, Token *open, Token *datatype)
{
	STATS_DECL
	Token *comma = GetNextCharToken(',', L',');

	// String expression
	Token *string = GetNextToken();
	ParseExpression(string);

	/*Token *comma2 */ (void) GetNextCharToken(',', L',');

	// Style
	Token *style = GetNextNumberToken();
	Token *close = GetNextCharToken(')', L')');

	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		// Map style to format
		if(style != NULL)
		{
       		Token::Change(name, "STR_TO_DATE", L"STR_TO_DATE", 11);

			if(_source == SQL_SQL_SERVER)
			{
				if(style->Compare("101", L"101", 3) == true)
					Token::Change(style, "'%m/%d/%Y'", L"'%m/%d/%Y'", 10);
				else
				if(style->Compare("121", L"121", 3) == true)
					Token::Change(style, "'%Y-%m-%d %T.%f'", L"'%Y-%m-%d %T.%f'", 16);
			}
			else
			if(_source == SQL_SYBASE)
				SybaseMapDatetimeStyle(style);
		}
        // Use the default format for datetime and CONVERT(exp, datatype) function
        else
        {
            // SMALLDATETIME has always 00 seconds
            if(TOKEN_CMP(datatype, "SMALLDATETIME"))
            {
                APPEND_FMT(open, "DATE_FORMAT(", name);
                PREPEND(close, ", '%Y-%m-%d %H-%i-00')");
            }

            // Specify datatype explicitly as the source can be SMALLDATETIME or other datetime type
            PREPEND_FMT(close, ", DATETIME", datatype);
        }

   		Token::Remove(datatype);
		Token::Remove(comma, false);
	}

    name->data_type = TOKEN_DT_DATETIME;

	FUNC_STATS(name);
	return true;
}

// CONVERT for IMAGE in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionConvertImage(Token *name, Token *open, Token *datatype)
{
	STATS_DECL
	// Comma after the data type
	Token *comma = GetNextCharToken(',', L',');
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert IMAGE datatype to VARBINARY(max) in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(datatype, "VARBINARY", L"VARBINARY", 9);
		AppendNoFormat(datatype, "(max)", L"(max)", 5);
	}
	else
	// Convert to TO_BLOB in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_BLOB", L"TO_BLOB", 7);
		Prepend(exp, "(", L"(", 1);
		Token::Remove(open, comma);
	}

	FUNC_STATS(name);
	return true;
}

// CONVERT for INTEGER in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionConvertInteger(Token *name, Token *open, Token * /*datatype*/)
{
	STATS_DECL
	// Comma after the data type
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	// Convert expression
	Token *exp = GetNextToken();
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TRUNC in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRUNC", L"TRUNC", 5);
		Prepend(exp, "(", L"(", 1);
		Token::Remove(open, comma);
	}

	FUNC_STATS(name);
	return true;
}

// CONVERT for TEXT in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionConvertText(Token *name, Token *open, Token *datatype)
{
	STATS_DECL
	// Comma after the data type
	Token *comma = GetNextCharToken(',', L',');
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert TEXT datatype to VARCHAR(max) in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(datatype, "VARCHAR", L"VARCHAR", 7);
		AppendNoFormat(datatype, "(max)", L"(max)", 5);
	}
	else
	// Convert to TO_CLOB in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CLOB", L"TO_CLOB", 7);
		Prepend(exp, "(", L"(", 1);
		Token::Remove(open, comma);
	}

	FUNC_STATS(name);
	return true;
}

// CONVERT to VARCHAR and CHAR in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionConvertVarchar(Token *name, Token *open, Token *datatype)
{
	STATS_DECL
	// Length of the target data type
	Token *open_bracket = GetNextCharToken('(', L'(');
	Token *length = NULL;
	Token *close_bracket = NULL;

	// Length is optional
	if(open_bracket != NULL)
	{
		length = GetNextToken();

		// Parse length
		ParseExpression(length);

		close_bracket = GetNextCharToken(')', ')');
	}

	// Comma after the data type
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	// Convert expression
	Token *exp = GetNextToken();
	ParseExpression(exp);
	Token *exp_end = GetLastToken();

	Token *comma2 = GetNextCharToken(',', L',');
	Token *style = NULL;
	
	// Style is optional
	if(comma2 != NULL)
		style = GetNextNumberToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle, PostgreSQL
	if(Target(SQL_ORACLE, SQL_POSTGRESQL) == true)
	{
		Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);
		Prepend(exp, "(", L"(", 1);
		Token::Remove(open, comma);

		// Convert map style to format
		if(style != NULL)
		{
			if(style->Compare("104", L"104", 3) == true)
				Token::Change(style, "'dd.mm.yyyy'", L"'dd.mm.yyyy'", 12);
			else
			if(style->Compare("7", L"7", 1) == true)
				Token::Change(style, "'Mon dd, yy'", L"'Mon dd, yy'", 12);
		}
	}
	else
	// Convert to DATE_FORMAT in MySQL and MariaDB if expression is datetime
	if(Target(SQL_MARIADB, SQL_MYSQL) && (exp->data_type == TOKEN_DT_DATETIME || style != NULL))
	{
		Token::Change(name, "DATE_FORMAT", L"DATE_FORMAT", 11);
		Prepend(exp, "(", L"(", 1);
		Token::Remove(open, comma);

		// Convert SQL Server style to format
		if(style != NULL && _source == SQL_SQL_SERVER)
		{
			if(style->Compare("101", L"101", 3) == true)
				Token::Change(style, "'%m/%d/%Y'", L"'%m/%d/%Y'", 10);
			else
			if(style->Compare("103", L"103", 3) == true)
				Token::Change(style, "'%d/%m/%Y'", L"'%d/%m/%Y'", 10);
			else
			if(style->Compare("108", L"108", 3) == true)
				Token::Change(style, "'%T'", L"'%T'", 4);
			else
			if(style->Compare("112", L"112", 3) == true)
				Token::Change(style, "'%Y%m%d'", L"'%Y%m%d'", 8);
			else
			if(style->Compare("121", L"121", 3) == true)
				Token::Change(style, "'%Y-%m-%d %T.%f'", L"'%Y-%m-%d %T.%f'", 16);
			else
			if(style->Compare("20", L"20", 2) == true)
				Token::Change(style, "'%Y-%m-%d %T'", L"'%Y-%m-%d %T'", 13);
		}
		else
		// Convert Sybase ASE style to format
		if(style != NULL && _source == SQL_SYBASE)
			SybaseMapDatetimeStyle(style);
	}
	else
	// If the expression is string then substring is taken
	if(exp->data_type == TOKEN_DT_STRING)
	{
		// CAST(exp AS CHAR(length)) for MariaDB, MySQL; they do not support CAST AS VARCHAR
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			TOKEN_CHANGE(name, "CAST");
			APPEND_FMT(exp_end, " AS CHAR(", datatype);
			AppendCopy(exp_end, length);
			APPEND(exp_end, ")");

			Token::Remove(datatype, comma);
		}
	}

	name->data_type = TOKEN_DT_STRING;

	FUNC_STATS(name);
	return true;
}

// COS in SQL Server, Oracle, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionCos(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// COSH in Oracle, DB2
bool SqlParser::ParseFunctionCosh(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	Token *end_num = GetLastToken();
	Token *close = GetNextCharToken(')', L')');

	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Prepend(name, "(", L"(", 1);
		Token::Change(name, "EXP", L"EXP", 3);
		Append(close, " + EXP(-(", L" + EXP(-(", 9, name);
		AppendCopy(close, num, end_num);
		Append(close, ")))/2", L")))/2", 5);
	}

	FUNC_STATS(name);
	return true;
}

// COT in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionCot(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *numeric = GetNextToken();

	if(numeric == NULL)
		return false;

	// Parse numeric
	ParseExpression(numeric);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to COS/SIN in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "COS(", L"COS(", 4);
		AppendCopy(name, numeric);
		Append(name, ")/SIN", L")/SIN", 5);
	}

	FUNC_STATS(name);
	return true;
}

// AVG aggregate and analytic function
bool SqlParser::ParseFunctionAvg(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_AVG_DESC)

	// Optional DISTINCT keyword
	/*Token *distinct */ (void) GetNext("DISTINCT", L"DISTINCT", 8);

	// Column, constant or expression
	Token *col = GetNextToken();

	if(col == NULL)
		return false;

	ParseExpression(col);

	/*Token *close */ (void) GetNextCharToken(')', ')');

	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

	if(over != NULL)
		ParseAnalyticFunctionOverClause(over);

	STATS_SET_CONV_NO_NEED(true)
	FUNC_STATS(name);
	return true;
}

// COUNT aggregate and analytic function
bool SqlParser::ParseFunctionCount(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_COUNT_DESC)

	// Optional DISTINCT keyword
	/*Token *distinct */ (void) GetNext("DISTINCT", L"DISTINCT", 8);

	// Column, constant, expression or *
	Token *col = GetNextToken();

	if(col == NULL)
		return false;

	ParseExpression(col);

	/*Token *close */ (void) GetNextCharToken(')', ')');

	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

	if(over != NULL)
		ParseAnalyticFunctionOverClause(over);

	STATS_SET_CONV_NO_NEED(true)
	FUNC_STATS(name);
	return true;
}

// SUM aggregate and analytic function
bool SqlParser::ParseFunctionSum(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_SUM_DESC)

	// Optional DISTINCT keyword
	/*Token *distinct */ (void) GetNext("DISTINCT", L"DISTINCT", 8);

	// Column, constant or expression
	Token *col = GetNextToken();

	if(col == NULL)
		return false;

	ParseExpression(col);

	/*Token *close */ (void) GetNextCharToken(')', ')');

	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

	if(over != NULL)
		ParseAnalyticFunctionOverClause(over);

	STATS_SET_CONV_NO_NEED(true)
	FUNC_STATS(name);
	return true;
}

// CSCONVERT in Sybase ASA
bool SqlParser::ParseFunctionCsconvert(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	/*Token *target_charset */ (void) GetNextToken();
	Token *comma2 = GetNextCharToken(',', L',');
	Token *source_charset = NULL;

	// Source_charset is optional
	if(comma2 != NULL)
		source_charset = GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to COS/SIN in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "CONVERT", L"CONVERT", 7);

	FUNC_STATS(name);
	return true;
}

// CURDATE() in MySQL
bool SqlParser::ParseFunctionCurdate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// TRUNC(SYSDATE) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRUNC(SYSDATE", L"TRUNC(SYSDATE", 13);
		Token::Remove(open);
	}

	FUNC_STATS(name);
	return true;
}

// CURRENT DATE, CURRENT SERVER, CURRENT SCHEMA, CURRENT SQLID, CURRENT TIME, CURRENT TIMESTAMP, CURRENT USER,
// Informix CURRENT YEAR TO SECOND
bool SqlParser::ParseFunctionCurrent(Token *name)
{
	if(name == NULL)
		return false;

	Token *second = GetNextToken();

	if(second == NULL)
		return false;

	bool exists = false;

	// CURRENT DATE in DB2, Sybase ASA
	if(second->Compare("DATE", L"DATE", 4) == true)
	{
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "CONVERT(DATE, GETDATE())", L"CONVERT(DATE, GETDATE())", 24);
			Token::Remove(second);
		}
		else
		if(_target == SQL_ORACLE)
		{
			Token::Change(name, "TRUNC(SYSDATE)", L"TRUNC(SYSDATE)", 14);
			Token::Remove(second);
		}
		else
		if(Target(SQL_MARIADB, SQL_MYSQL) || _target == SQL_POSTGRESQL || _target == SQL_GREENPLUM)
		{
			Token::Change(name, "CURRENT_DATE", L"CURRENT_DATE", 12);
			Token::Remove(second);
		}

		exists = true;
	}
	else
	// CURRENT SCHEMA
	if(second->Compare("SCHEMA", L"SCHEMA", 6) == true)
	{
		if(_target == SQL_ORACLE)
		{
			Token::Change(name, "CURRENT_SCHEMA", L"CURRENT_SCHEMA", 14);
			Token::Remove(second);
		}

		exists = true;
	}
	else
	// CURRENT SERVER (current database name in DB2)
	if(second->Compare("SERVER", L"SERVER", 6) == true)
	{
		if(_target == SQL_ORACLE)
		{
			Token::Change(name, "SYS_CONTEXT('USERENV', 'DB_NAME')", L"SYS_CONTEXT('USERENV', 'DB_NAME')", 33);
			Token::Remove(second);
		}

		exists = true;
	}
	else
	// CURRENT SQLID
	if(second->Compare("SQLID", L"SQLID", 5) == true)
	{
		if(_target == SQL_ORACLE)
		{
			Token::Change(name, "CURRENT_SQLID", L"CURRENT_SQLID", 13);
			Token::Remove(second);
		}

		exists = true;
	}
	else
	// CURRENT TIMESTAMP
	if(second->Compare("TIMESTAMP", L"TIMESTAMP", 9) == true)
	{
		// Convert to SYSTIMESTAMP in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Change(second, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);
			Token::Remove(name);
		}
		else
		// Convert to GETDATE() in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(second, "GETDATE()", L"GETDATE()", 9);
			Token::Remove(name);
		}
		else
		// Convert to CURRENT_TIMESTAMP in MySQL, PostgreSQL, Greenplum
		if(Target(SQL_MARIADB, SQL_MYSQL, SQL_POSTGRESQL, SQL_GREENPLUM))
		{
			Token::Change(second, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);
			Token::Remove(name);
		}

		name->data_type = TOKEN_DT_DATETIME;
		name->nullable = false;

		exists = true;
	}
	else
	// CURRENT TIME
	if(second->Compare("TIME", L"TIME", 4) == true)
	{
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "CONVERT(TIME, GETDATE())", L"CONVERT(TIME, GETDATE())", 24);
			Token::Remove(second);
		}
		else
		if(_target == SQL_ORACLE)
		{
			Token::Change(name, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);
			Token::Remove(second);
		}

		exists = true;
	}
	else
	// CURRENT USER
	if(second->Compare("USER", L"USER", 4) == true)
	{
		// SQL Server has CURRENT_USER but it returns dbo i.e
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "SYSTEM_USER", L"SYSTEM_USER", 11);
			Token::Remove(second);
		}
		else
		if(_target == SQL_ORACLE)
			Token::Remove(name);

		exists = true;
	}
	else
	// CURRENT YEAR TO part in Informix
	if(second->Compare("YEAR", L"YEAR", 4) == true)
	{
		Token *to = GetNextWordToken("TO", L"TO", 2);
		Token *third = (to != NULL) ? GetNextToken() : NULL;

		// CURRENT YEAR TO SECOND (fraction is set to zero)
		if(Token::Compare(third, "SECOND", L"SECOND", 6) == true)
		{
			// SYSDATE in Oracle
			if(_target == SQL_ORACLE)
			{
				Token::Change(name, "SYSDATE", L"SYSDATE", 7);
				Token::Remove(second, third);
			}
		}

		if(third != NULL)
			exists = true;
	}
	else
	{
		// CURRENT defaults to CURRENT YEAR TO FRACTION(3) in Informix
		if(_target == SQL_ORACLE)
			Token::Change(name, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);
		else
		if(_target == SQL_SQL_SERVER)
			Token::Change(name, "GETDATE()", L"GETDATE()", 9);

		PushBack(second);
	}

	return exists;
}

// CURRENT_BIGDATETIME in Sybase ASE
bool SqlParser::ParseFunctionCurrentBigdatetime(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = GetNextCharToken(')', L')');

	// Convert to GETDATE() in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "GETDATE", L"GETDATE", 7);
	else
	// Convert to SYSTIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);
		Token::Remove(open, close);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		TOKEN_CHANGE(name, "NOW");
		APPEND_NOFMT(open, "6");
	}

	FUNC_STATS(name);
	return true;
}

// CURRENT_BIGTIME in Sybase ASE
bool SqlParser::ParseFunctionCurrentBigtime(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = GetNextCharToken(')', L')');

	// Convert to GETDATE() in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "GETDATE", L"GETDATE", 7);
	else
	// Convert to SYSTIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);
		Token::Remove(open, close);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		TOKEN_CHANGE(name, "CURTIME");
		APPEND_NOFMT(open, "6");
	}

	FUNC_STATS(name);
	return true;
}

// CURRENT_DATE with () in Sybase ASE, Sybase ADS, MariaDB, MySQL
bool SqlParser::ParseFunctionCurrentDate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = TOKEN_GETNEXT(')');

	// If target type is not MariaDB, MySQL, Sybase ASE, Sybase ADS remove parentheses
	if(!Target(SQL_MARIADB, SQL_MYSQL, SQL_SYBASE, SQL_SYBASE_ADS))
		Token::Remove(open, close);

	// Convert to TRUNC(SYSDATE) in Oracle
	if(_target == SQL_ORACLE && _source != SQL_ORACLE)
		Token::Change(name, "TRUNC(SYSDATE)", L"TRUNC(SYSDATE)", 14);
	else
	// Convert to GETDATE in SQL Server
	if(_target == SQL_SQL_SERVER && _source != SQL_ORACLE)
		Token::Change(name, "CONVERT(DATE, GETDATE())", L"CONVERT(DATE, GETDATE())", 24);

	name->data_type = TOKEN_DT_DATETIME;
	name->nullable = false;

	FUNC_STATS(name);
	return true;
}

// CURRENT_DATE in Oracle, DB2, MySQL, Sybase ASE, Sybase ADS
bool SqlParser::ParseFunctionCurrentDate(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *open = NULL;
	Token *close = NULL;

	// Get parentheses for Sybase ASE, Sybase ADS
	if(Source(SQL_SYBASE, SQL_SYBASE_ADS))
	{
		open = GetNextCharToken('(', L'(');
		close = GetNextCharToken(')', L')');
	}

	// If target type is not Sybase ASE remove parentheses
	if(_target != SQL_SYBASE)
		Token::Remove(open, close);

	// Convert to TRUNC(SYSDATE) in Oracle
	if(_target == SQL_ORACLE && _source != SQL_ORACLE)
		Token::Change(name, "TRUNC(SYSDATE)", L"TRUNC(SYSDATE)", 14);
	else
	// Convert to GETDATE in SQL Server
	if(_target == SQL_SQL_SERVER && _source != SQL_ORACLE)
		Token::Change(name, "CONVERT(DATE, GETDATE())", L"CONVERT(DATE, GETDATE())", 24);
	else
	// Convert Oracle CURRENT_DATE to NOW() in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL) && _source == SQL_ORACLE)
		Token::Change(name, "NOW()", L"NOW()", 5);

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle CURRENT_SCHEMA
bool SqlParser::ParseFunctionCurrentSchema(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	FUNC_STATS(name);
	return true;
}

// DB2 CURRENT_SQLID
bool SqlParser::ParseFunctionCurrentSqlid(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "CURRENT_SCHEMA", L"CURRENT_SCHEMA", 14);

	FUNC_STATS(name);
	return true;
}

// CURRENT_TIMESTAMP in Oracle, DB2, MySQL, PostgreSQL
bool SqlParser::ParseFunctionCurrentTimestamp(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_CURRENT_TIMESTAMP_DESC)

	// Convert to SYSTIMESTAMP in Oracle
	if(_target == SQL_ORACLE && _source != SQL_ORACLE)
		Token::Change(name, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);
	else
	// Convert to GETDATE() in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "GETDATE()", L"GETDATE()", 9);
	else
	// Convert Oracle CURRENT_DATE to NOW() in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL) && _source == SQL_ORACLE)
		Token::Change(name, "NOW()", L"NOW()", 5);

	name->data_type = TOKEN_DT_DATETIME;
	name->nullable = false;

	FUNC_STATS(name);
	return true;
}

// CURRENT_TIMESTAMP(n) in Teradata; CURRENT_TIMESTAMP() in Sybase ADS
bool SqlParser::ParseFunctionCurrentTimestamp(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Optional precision
	/*Token *num */ (void) GetNextNumberToken();

	Token *close = TOKEN_GETNEXT(')');

	// CURRENT_TIMESTAMP in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Remove(open, close);

	name->data_type = TOKEN_DT_DATETIME;
	name->nullable = false;

	FUNC_STATS(name);
	return true;
}

// CURRENT_TIME in DB2, Sybase ASE
bool SqlParser::ParseFunctionCurrentTime(Token *name)
{
	if(name == NULL)
		return false;

	// Oracle you can declare a variable CURRENT_TIME, there is no such built-in function
	if(_source == SQL_ORACLE)
		return false;

	STATS_DECL
	Token *open = NULL;
	Token *close = NULL;

	// Get parentheses for Sybase ASE
	if(_source == SQL_SYBASE)
	{
		open = GetNextCharToken('(', L'(');
		close = GetNextCharToken(')', L')');
	}

	// If target type is not Sybase ASE remove parentheses
	if(_target != SQL_SYBASE)
		Token::Remove(open, close);

	// Convert to Getdate in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "GETDATE()", L"GETDATE()", 9);
	else
	// Convert to CURRENT_TIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);

	FUNC_STATS(name);
	return true;
}

// SQL Server, DB2, Sybase ASA CURRENT_USER
bool SqlParser::ParseFunctionCurrentUser(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// SQL Server has CURRENT_USER but it returns dbo i.e
	if(_source != SQL_SQL_SERVER && _target == SQL_SQL_SERVER)
		Token::Change(name, "SYSTEM_USER", L"SYSTEM_USER", 11);
	else
	if(_target == SQL_ORACLE)
		Token::Change(name, "USER", L"USER", 4);

	FUNC_STATS(name);
	return true;
}

// DATE without parameters in Teradata
bool SqlParser::ParseFunctionDate(Token *name)
{
	if(name == NULL || _source != SQL_TERADATA)
		return false;

	STATS_DECL
	// SYSDATE in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "SYSDATE", L"SYSDATE", 7);

	name->nullable = false;

	name->data_type = TOKEN_DT_DATETIME;
	name->data_subtype = TOKEN_DT2_DATE;

	FUNC_STATS(name);
	return true;
}

// INTERVAL in Oracle, MySQL 
bool SqlParser::ParseFunctionInterval(Token *name)
{
	if(name == NULL)
		return false;
	
	STATS_DECL
	// Number in MySQL; 'num' in Oracle; variable or expression in MySQL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// DAY i.e.
	Token *unit = GetNextToken();

	bool exp_num = exp->IsNumeric();

	// INTERVAL 'num' DAY, or NUMTODSINTERVAL(var, 'DAY') in Oracle
	if(_source != SQL_ORACLE && _target == SQL_ORACLE)
	{
		// Number constant must be quoted
		if(exp_num == true)
		{
			PrependNoFormat(exp, "'", L"'", 1);
			AppendNoFormat(exp, "'", L"'", 1);
		}
		// Variable or expression
		else
		{
			Prepend(exp, "NUMTODSINTERVAL(", L"NUMTODSINTERVAL(", 16, name);
			
			AppendNoFormat(exp, ",", L",", 1);
			PrependNoFormat(unit, "'", L"'", 1);
			AppendNoFormat(unit, "')", L"')", 2);

			Token::Remove(name);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DB2 NEXTVAL
bool SqlParser::ParseFunctionNextval(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// FOR keyword
	Token *for_ = GetNextWordToken("FOR", L"FOR", 3);

	if(for_ == NULL)
		return false;

	// Sequence name
	Token *seq_name = GetNextToken();

	if(seq_name == NULL)
		return false;

	// seq.NEXTVAL in Oracle
	if(_target == SQL_ORACLE)
	{
		PrependCopy(name, seq_name);
		PrependNoFormat(name, ".", L".", 1);

		Token::Remove(for_, seq_name);
	}

	FUNC_STATS(name);
	return true;
}

// NULL value
bool SqlParser::ParseFunctionNull(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	if(_target_app == APP_JAVA)
		TOKEN_CHANGE_NOFMT(name, "null");

	// Don't include NULL to function stats - FUNC_STATS(name);
	return true;
}

// @@ROWCOUNT in SQL Server
bool SqlParser::ParseFunctionRowcount(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "FOUND_ROWS()", L"FOUND_ROWS()", 12);

	FUNC_STATS(name);
	return true;
}

// DB2 CURSOR_ROWCOUNT
bool SqlParser::ParseFunctionCursorRowcount(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Cursor variable
	Token *cur = GetNextToken();

	if(cur == NULL)
		return false;

	Token *close = GetNextCharToken(')', L')');

	// Convert to cur%ROWCOUNT in Oracle
	if(_target == SQL_ORACLE)
	{
		Append(cur, "%ROWCOUNT", L"%ROWCOUNT", 9, name);
		
		Token::Remove(name);
		Token::Remove(open);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// DATALENGTH in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDatalength(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LENGTHB in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "LENGTHB", L"LENGTHB", 7);
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
		TOKEN_CHANGE(name, "LENGTH");

	FUNC_STATS(name);
	return true;
}

// DATE in DB2, PostgreSQL, Sybase ASA
bool SqlParser::ParseFunctionDate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	// Recognize format in string literal
	if(exp->type == TOKEN_STRING)
	{
		// Convert to TO_DATE in Oracle
		if(_target == SQL_ORACLE)
		{
			TokenStr format(", ", L", ", 2);

			Token::Change(name, "TO_DATE", L"TO_DATE", 7);

			if(RecognizeOracleDateFormat(exp, format) == true)
				AppendNoFormat(exp, &format);
		}
		else
		// CONVERT(DATE, exp) in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "CONVERT", L"CONVERT", 7);
			AppendFirst(open, "DATE, ", L"DATE, ", 6, name);
		}
	}
	else
	// DB2 allows a number (day from 01.01.0001)
	if(exp->IsNumeric())
	{
		// Convert to DATE '0001-01-01' + num - 1 in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Change(open, " '0001-01-01' + ", L" '0001-01-01' + ", 16);
			Append(exp, " - 1", L" - 1", 4);

			Token::Remove(close);
		}
	}
	// Variable column or expression
	else
	{
		if(_target == SQL_ORACLE)
		{
			// If TIMESTAMP is converted to DATE use TRUNC()
			if(exp->data_type == TOKEN_DT_DATETIME)
				Token::Change(name, "TRUNC", L"TRUNC", 5);
			else
				Token::Change(name, "TO_DATE", L"TO_DATE", 7);
		}
		else
		// CONVERT(DATE, exp) in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "CONVERT", L"CONVERT", 7);
			AppendFirst(open, "DATE, ", L"DATE, ", 6, name);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DATEADD in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDateadd(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Datepart is a keyword, not expression (day, dd i.e.)
	Token *datepart = GetNextIdentToken();
	Token *comma1 = GetNextCharToken(',', L',');

	if(comma1 == NULL)
		return false;

	Token *number = GetNextToken();

	bool number_const = false;

	// Check if number constant is specified
	if(number->IsNumeric() == true)
		number_const = true;
	else
		ParseExpression(number);

	Token *number_end = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');

	if(comma2 == NULL)
		return false;

	Token *datetime = GetNextToken();

	// Parse datetime
	ParseExpression(datetime);

	Token *close = GetNextCharToken(')', L')');

	// PostgreSQL
	if(_target == SQL_POSTGRESQL)
	{
		// If a numeric constant is specified, convert to "INTERVAL 'number part' + datetime" 
		if(number_const == true)
		{
			Token::Change(name, "INTERVAL '", L"INTERVAL '", 10);
			Token::Remove(open);
			Token::Change(comma1, "'", L"'", 1);
			Token::Change(comma2, " +", L" +", 2);
			Token::Remove(close);
					
			AppendCopy(name, number);
			Append(name, " ", L" ", 1);
			Token::Remove(number);
		}
		else
		// If an expression is specified, convert to "exp * INTERVAL '1 part' + datetime" 
		{
			PrependCopy(name, number);
			Token::Change(name, " * INTERVAL '1 ", L" * INTERVAL '1 ", 15);
			Token::Remove(open);
			Token::Remove(comma1, number);
			Token::Change(comma2, "' +", L"' +", 3);
			Token::Remove(close);
		}

		// Change DD to DAY
		if(Token::Compare(datepart, "DD", L"DD", 2) == true)
			Token::Change(datepart, "DAY", L"DAY", 3);
	}
	else
	// Oracle
	if(_target == SQL_ORACLE)
	{
		// Append precision for DAY
		if(Token::Compare(datepart, "DAY", L"DAY", 3) == true || 
			Token::Compare(datepart, "DD", L"DD", 2) == true)
			Append(datepart, "(5)", L"(5)", 3);

		// If a numeric constant is specified, convert to "INTERVAL 'number' part + datetime" 
		if(number_const == true)
		{
			Token::Change(name, "INTERVAL '", L"INTERVAL '", 10);
			Token::Remove(open);
			Token::Remove(comma1);
			Token::Change(comma2, " +", L" +", 2);
			Token::Remove(close);
					
			AppendCopy(name, number);
			Append(name, "' ", L"' ", 2);
			Token::Remove(number);
		}
		else
		// If an expression is specified, convert to "exp * INTERVAL '1' part + datetime" 
		{
			PrependCopy(name, number);
			Token::Change(name, " * INTERVAL '1' ", L" * INTERVAL '1' ", 16);
			Token::Remove(open);
			Token::Remove(comma1, number);
			Token::Change(comma2, " +", L" +", 2);
			Token::Remove(close);
		}
	}
    else
	// TIMESTAMPADD in MySQL, MariaDB
	if(Target(SQL_MYSQL, SQL_MARIADB))
	{
        TOKEN_CHANGE(name, "TIMESTAMPADD");

		if(TOKEN_CMP(datepart, "yy"))
			TOKEN_CHANGE(datepart, "year");
		else
		if(TOKEN_CMP(datepart, "qq"))
			TOKEN_CHANGE(datepart, "quarter");
		else
		if(TOKEN_CMP(datepart, "mm"))
			TOKEN_CHANGE(datepart, "month");
		else
		if(TOKEN_CMP(datepart, "wk"))
			TOKEN_CHANGE(datepart, "week");
		else
		if(TOKEN_CMP(datepart, "dd") || TOKEN_CMP(datepart, "dy") || TOKEN_CMP(datepart, "dw") ||
			TOKEN_CMP(datepart, "dayofyear") || TOKEN_CMP(datepart, "weekday"))
			TOKEN_CHANGE(datepart, "day");
		else
		if(TOKEN_CMP(datepart, "hh"))
			TOKEN_CHANGE(datepart, "hour");
		else
		if(TOKEN_CMP(datepart, "mi"))
			TOKEN_CHANGE(datepart, "minute");
		else
		if(TOKEN_CMP(datepart, "ss"))
			TOKEN_CHANGE(datepart, "second");
		else
		if(TOKEN_CMP(datepart, "ms") || TOKEN_CMP(datepart, "millisecond"))
		{
			TOKEN_CHANGE(datepart, "microsecond");
			
			// Wrap multitoken expression with ()
			if(number != number_end)
			{
				PREPEND_NOFMT(number, "(");
				APPEND_NOFMT(number_end, ")");
			}

			APPEND_NOFMT(number_end, " * 1000");
		}
		else
		if(TOKEN_CMP(datepart, "us"))
			TOKEN_CHANGE(datepart, "microsecond");
	}

    name->data_type = TOKEN_DT_DATETIME;

	FUNC_STATS(name);
	return true;
}

// DATEDIFF in SQL Server, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDatediff(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// In SQL Server, Sybase ASE and Sybase ASA first parameter specifies unit, in MySQL difference always in days
	Token *datepart = NULL;
	Token *comma1 = NULL;

	// Parse SQL Server, Sybase ASE and Sybase ASA syntax by default
	if(_source != SQL_MYSQL)
	{
		// Datepart is a keyword, not expression (day, dd i.e.)
		datepart = GetNextIdentToken();
		comma1 = GetNextCharToken(',', L',');

		if(comma1 == NULL)
			return false;
	}

	// Start date
	Token *start = GetNextToken();
	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');

	if(comma2 == NULL)
		return false;

	// End date
	Token *end = GetNextToken();
	ParseExpression(end);

	Token *close = GetNextCharToken(')', L')');

	// If SQL Server, Sybase ASE or Sybase ASA is source, parameters are datetime 
	if(_source != SQL_MYSQL)
	{
		if(_target == SQL_ORACLE)
		{
			// If datepart is MONTH convert to MONTHS_BETWEEN in Oracle
			if(Token::Compare(datepart, "MONTH", L"MONTH", 5) || 
				Token::Compare(datepart, "MM", L"MM", 2))
			{
				Token::Change(name, "MONTHS_BETWEEN", L"MONTHS_BETWEEN", 14);

				// If start and end are date string literals enclose with TO_DATE
				if(end->type == TOKEN_STRING)
				{
					Append(open, "TO_DATE(", L"TO_DATE(", 8, name);
					AppendCopy(open, end);
					Append(open, ")", L")", 1);
				}
				else
					AppendCopy(open, end);


				if(start->type == TOKEN_STRING)
				{
					Prepend(start, "TO_DATE(", L"TO_DATE(", 8, name);
					Append(start, ")", L")", 1);
				}

				Token::Remove(datepart);
				Token::Remove(comma2, end);
			}
			else
			// If datepart is YEAR convert to MONTHS_BETWEEN/12 in Oracle
			if(Token::Compare(datepart, "YEAR", L"YEAR", 4) || 
				Token::Compare(datepart, "YY", L"YY", 2))
			{
				Token::Change(name, "MONTHS_BETWEEN", L"MONTHS_BETWEEN", 14);

				// If start and end are date string literals enclose with TO_DATE
				if(end->type == TOKEN_STRING)
				{
					Append(open, "TO_DATE(", L"TO_DATE(", 8, name);
					AppendCopy(open, end);
					Append(open, ")", L")", 1);
				}
				else
					AppendCopy(open, end);


				if(start->type == TOKEN_STRING)
				{
					Prepend(start, "TO_DATE(", L"TO_DATE(", 8, name);
					Append(start, ")", L")", 1);
				}

				Token::Remove(datepart);
				Token::Remove(comma2, end);
				Append(close, " / 12", L" / 12", 5);
			}
			else
			// If datepart is DAY convert to enddate - startdate in Oracle
			if(Token::Compare(datepart, "DAY", L"DAY", 3) || 
				Token::Compare(datepart, "DD", L"DD", 2))
			{
				// If start and end are date string literals enclose with TO_DATE
				if(end->type == TOKEN_STRING)
				{
					Append(open, "TO_DATE(", L"TO_DATE(", 8, name);
					AppendCopy(open, end);
					Append(open, ")", L")", 1);
				}
				else
					AppendCopy(open, end);


				if(start->type == TOKEN_STRING)
				{
					Prepend(start, "TO_DATE(", L"TO_DATE(", 8, name);
					Append(start, ")", L")", 1);
				}

				Token::Remove(datepart);
				Token::Remove(name, open);
				Token::Change(comma1, " -", L" -", 2);
				Token::Remove(comma2, close);
			}
			else
			// If datepart is HOUR convert to (enddate - startdate) * 24 in Oracle
			if(Token::Compare(datepart, "HOUR", L"HOUR", 4) || 
				Token::Compare(datepart, "HH", L"HH", 2))
			{
				// If start and end are date string literals enclose with TO_DATE
				if(end->type == TOKEN_STRING)
				{
					Append(open, "TO_DATE(", L"TO_DATE(", 8, name);
					AppendCopy(open, end);
					Append(open, ")", L")", 1);
				}
				else
					AppendCopy(open, end);


				if(start->type == TOKEN_STRING)
				{
					Prepend(start, "TO_DATE(", L"TO_DATE(", 8, name);
					Append(start, ")", L")", 1);
				}

				Token::Remove(datepart);
				Token::Remove(name);
				Token::Change(comma1, " -", L" -", 2);
				Token::Remove(comma2, end);
				Append(close, " * 24", L" * 24", 5);
			}
			else
			// If datepart is MINUTE convert to (enddate - startdate) * 1440 in Oracle
			if(Token::Compare(datepart, "MINUTE", L"MINUTE", 6) || 
				Token::Compare(datepart, "MI", L"MI", 2))
			{
				// If start and end are date string literals enclose with TO_DATE
				if(end->type == TOKEN_STRING)
				{
					Append(open, "TO_DATE(", L"TO_DATE(", 8, name);
					AppendCopy(open, end);
					Append(open, ")", L")", 1);
				}
				else
					AppendCopy(open, end);


				if(start->type == TOKEN_STRING)
				{
					Prepend(start, "TO_DATE(", L"TO_DATE(", 8, name);
					Append(start, ")", L")", 1);
				}

				Token::Remove(datepart);
				Token::Remove(name);
				Token::Change(comma1, " -", L" -", 2);
				Token::Remove(comma2, end);
				Append(close, " * 1440", L" * 1440", 7);
			}
			else
			// If datepart is SECOND convert to (enddate - startdate) * 86400 in Oracle
			if(Token::Compare(datepart, "SECOND", L"SECOND", 6) || 
				Token::Compare(datepart, "SS", L"SS", 2))
			{
				// If start and end are date string literals enclose with TO_DATE
				if(end->type == TOKEN_STRING)
				{
					Append(open, "TO_DATE(", L"TO_DATE(", 8, name);
					AppendCopy(open, end);
					Append(open, ")", L")", 1);
				}
				else
					AppendCopy(open, end);


				if(start->type == TOKEN_STRING)
				{
					Prepend(start, "TO_DATE(", L"TO_DATE(", 8, name);
					Append(start, ")", L")", 1);
				}

				Token::Remove(datepart);
				Token::Remove(name);
				Token::Change(comma1, " -", L" -", 2);
				Token::Remove(comma2, end);
				Append(close, " * 86400", L" * 86400", 8);
			}
		}
		else
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			TOKEN_CHANGE(name, "TIMESTAMPDIFF");

			if(TOKEN_CMP(datepart, "yy"))
				TOKEN_CHANGE(datepart, "year");
			else
			if(TOKEN_CMP(datepart, "qq"))
				TOKEN_CHANGE(datepart, "quarter");
			else
			if(TOKEN_CMP(datepart, "mm"))
				TOKEN_CHANGE(datepart, "month");
			else
			if(TOKEN_CMP(datepart, "wk"))
				TOKEN_CHANGE(datepart, "week");
			else
			if(TOKEN_CMP(datepart, "dd"))
				TOKEN_CHANGE(datepart, "day");
			else
			if(TOKEN_CMP(datepart, "hh"))
				TOKEN_CHANGE(datepart, "hour");
			else
			if(TOKEN_CMP(datepart, "mi"))
				TOKEN_CHANGE(datepart, "minute");
			else
			if(TOKEN_CMP(datepart, "ss"))
				TOKEN_CHANGE(datepart, "second");
			else
			if(TOKEN_CMP(datepart, "ms") || TOKEN_CMP(datepart, "millisecond"))
			{
				TOKEN_CHANGE(datepart, "microsecond");
				APPEND_NOFMT(close, "/1000");
			}
			else
			if(TOKEN_CMP(datepart, "us"))
				TOKEN_CHANGE(datepart, "microsecond");
		}
	}
	else
	// Source is MySQL
	{
		// Convert to date1 - date2 in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Remove(name, open);
			
			// If start and end are date string literals enclose with TO_DATE
			if(start->type == TOKEN_STRING)
			{
				Prepend(start, "TO_DATE(", L"TO_DATE(", 8, name);
				AppendNoFormat(start, ")", L")", 1);
			}

			if(end->type == TOKEN_STRING)
			{
				Prepend(end, "TO_DATE(", L"TO_DATE(", 8, name);
				AppendNoFormat(end, ")", L")", 1);
			}

			Token::Change(comma2, "-", L"-", 1);
			Token::Remove(close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// MySQL DATE_FORMAT
bool SqlParser::ParseFunctionDateFormat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// First expression
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	// Format is mandatory in MySQL
	if(comma == NULL)
		return false;

	Token *format = GetNextToken();

	if(format == NULL)
		return false;

	ParseExpression(format);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Format is a string literal
	if(format->type == TOKEN_STRING)
	{
		TokenStr out;
		size_t i = 0;

		while(i < format->len)
		{
			// %Y (4-digit year)
			if(Token::CompareCS(format, "%Y", L"%Y", i, 2) == true)
			{
				if(Target(SQL_ORACLE, SQL_DB2) == true)
					out.Append("YYYY", L"YYYY", 4);

				i += 2;
				continue;
			}
			else
			// %y (2-digit year rounded)
			if(Token::CompareCS(format, "%y", L"%y", i, 2) == true)
			{
				if(Target(SQL_ORACLE, SQL_DB2) == true)
					out.Append("RR", L"RR", 2);

				i += 2;
				continue;
			}
			else
			// %m month (01-12)
			if(Token::CompareCS(format, "%m", L"%m", i, 2) == true)
			{
				if(Target(SQL_ORACLE, SQL_DB2) == true)
					out.Append("MM", L"MM", 2);

				i += 2;
				continue;
			}
			else
			// %M full month name (January-December)
			if(Token::CompareCS(format, "%M", L"%M", i, 2) == true)
			{
				if(Target(SQL_ORACLE, SQL_DB2) == true)
					out.Append("MONTH", L"MONTH", 5);

				i += 2;
				continue;
			}
			else
			// %d day (01-31)
			if(Token::CompareCS(format, "%d", L"%d", i, 2) == true)
			{
				if(Target(SQL_ORACLE, SQL_DB2) == true)
					out.Append("DD", L"DD", 2);

				i += 2;
				continue;
			}
			else
			// %T time (HH24:MI:SS)
			if(Token::CompareCS(format, "%T", L"%T", i, 2) == true)
			{
				if(Target(SQL_ORACLE, SQL_DB2) == true)
					out.Append("HH24:MI:SS", L"HH24:MI:SS", 10);

				i += 2;
				continue;
			}

			// No format matched just copy
			out.Append(format, i, 1);
			i++;
		}

		// Set target format
		Token::ChangeNoFormat(format, out);

	}

	// TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);
	else
	// VARCHAR_FORMAT in DB2
	if(_target == SQL_DB2)
		Token::Change(name, "VARCHAR_FORMAT", L"VARCHAR_FORMAT", 14);

	name->data_type = TOKEN_DT_STRING;

	FUNC_STATS(name);
	return true;
}

// DATEFORMAT in Sybase ASA
bool SqlParser::ParseFunctionDateformatASA(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *datetime = GetNextToken();

	if(datetime == NULL)
		return false;
	
	// Parse datetime
	ParseExpression(datetime);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *format = GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Default format
	if(format == NULL)
	{
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "CONVERT(VARCHAR, ", L"CONVERT(VARCHAR, ", 17);
			Token::Remove(open);
		}
		else
		if(_target == SQL_ORACLE)
			Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);
	}
	else
	// Format is specified and it is a string literal
	if(format != NULL && format->type == TOKEN_STRING)
	{
		// Convert to TO_CHAR in Oracle
		if(_target == SQL_ORACLE)
			Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);
		else
		// Define a style for SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			// YYYY-MM-DD
			if(format->Compare("'YYYY-MM-DD'", L"'YYYY-MM-DD'", 12) == true)
			{
				Token::Change(name, "CONVERT(VARCHAR(10), ", L"CONVERT(VARCHAR(10), ", 21);
				Token::Remove(open);
				Token::Change(format, "120", L"120", 3);
			}
			else
			// YYYYMMDD
			if(format->Compare("'YYYYMMDD'", L"'YYYYMMDD'", 10) == true)
			{
				Token::Change(name, "CONVERT(VARCHAR(8), ", L"CONVERT(VARCHAR(8), ", 20);
				Token::Remove(open);
				Token::Change(format, "112", L"112", 3);
			}
			else
			// YYYY/MM/DD
			if(format->Compare("'YYYY/MM/DD'", L"'YYYY/MM/DD'", 12) == true)
			{
				Token::Change(name, "CONVERT(VARCHAR(10), ", L"CONVERT(VARCHAR(10), ", 21);
				Token::Remove(open);
				Token::Change(format, "111", L"111", 3);
			}
		}
	}

	name->data_type = TOKEN_DT_STRING;

	// If first argument is non-nullable, set the function result non-nullable
	if(datetime->nullable == false)
		name->nullable = false;

	FUNC_STATS(name);
	return true;
}

// DATENAME in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDatename(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Datepart is a keyword, not expression (day, dd i.e.)
	Token *datepart = GetNextIdentToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *date = GetNextToken();

	// Parse date
	ParseExpression(date);

	Token *close = GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);

		// If date expression is date string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Append(open, "TO_DATE(", L"TO_DATE(", 8, name);
			AppendCopy(open, date);
			Append(open, ")", L")", 1);
		}
		else
			AppendCopy(open, date);

		Append(open, ", ", L", ", 2);
		Prepend(datepart, "'", L"'", 1);
		Append(datepart, "'", L"'", 1);

		Token::Remove(comma, date);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Remove(datepart, comma);

		if(TOKEN_CMP(datepart, "yy") || TOKEN_CMP(datepart, "year"))
			TOKEN_CHANGE(name, "YEAR");
		else
		if(TOKEN_CMP(datepart, "qq") || TOKEN_CMP(datepart, "quarter"))
			TOKEN_CHANGE(name, "QUARTER");
		else
		if(TOKEN_CMP(datepart, "mm") || TOKEN_CMP(datepart, "month"))
			TOKEN_CHANGE(name, "MONTHNAME");
		else
		if(TOKEN_CMP(datepart, "wk") || TOKEN_CMP(datepart, "week"))
			TOKEN_CHANGE(name, "WEEK");
		else
		if(TOKEN_CMP(datepart, "dd") || TOKEN_CMP(datepart, "day"))
			TOKEN_CHANGE(name, "DAY");
		else
		if(TOKEN_CMP(datepart, "dy") || TOKEN_CMP(datepart, "dayofyear"))
			TOKEN_CHANGE(name, "DAYOFYEAR");
		else
		if(TOKEN_CMP(datepart, "dw") || TOKEN_CMP(datepart, "weekday"))
			TOKEN_CHANGE(name, "DAYNAME");
		else
		if(TOKEN_CMP(datepart, "hh") || TOKEN_CMP(datepart, "hour"))
			TOKEN_CHANGE(name, "HOUR");
		else
		if(TOKEN_CMP(datepart, "mi") || TOKEN_CMP(datepart, "minute"))
			TOKEN_CHANGE(name, "MINUTE");
		else
		if(TOKEN_CMP(datepart, "ss") || TOKEN_CMP(datepart, "second"))
			TOKEN_CHANGE(name, "SECOND");
		else
		if(TOKEN_CMP(datepart, "ms") || TOKEN_CMP(datepart, "millisecond"))
		{
			TOKEN_CHANGE(name, "MICROSECOND");
			PREPEND(name, "CAST(");
			APPEND_FMT(close, "/1000 AS INT)", name);
		}
		else
		if(TOKEN_CMP(datepart, "us") || TOKEN_CMP(datepart, "microsecond"))
			TOKEN_CHANGE(name, "MICROSECOND");
	}

	FUNC_STATS(name);
	return true;
}

// DATEPART in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDatepart(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Datepart is a keyword, not expression (day, dd i.e.)
	Token *datepart = GetNextIdentToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *date = GetNextToken();

	// Parse date
	ParseExpression(date);

	Token *close = GetNextCharToken(')', L')');

	// Convert to TO_NUMBER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);

		// If date expression is date string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Append(open, "TO_DATE(", L"TO_DATE(", 8, name);
			AppendCopy(open, date);
			Append(open, ")", L")", 1);
		}
		else
			AppendCopy(open, date);

		Append(open, ", ", L", ", 2);
		Prepend(datepart, "'", L"'", 1);
		Append(datepart, "'", L"'", 1);

		Token::Remove(comma, date);
		Append(close, ")", L")", 1);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Remove(datepart, comma);

		if(TOKEN_CMP(datepart, "yy") || TOKEN_CMP(datepart, "year"))
			TOKEN_CHANGE(name, "YEAR");
		else
		if(TOKEN_CMP(datepart, "qq") || TOKEN_CMP(datepart, "quarter"))
			TOKEN_CHANGE(name, "QUARTER");
		else
		if(TOKEN_CMP(datepart, "mm") || TOKEN_CMP(datepart, "month"))
			TOKEN_CHANGE(name, "MONTH");
		else
		if(TOKEN_CMP(datepart, "wk") || TOKEN_CMP(datepart, "week"))
			TOKEN_CHANGE(name, "WEEK");
		else
		if(TOKEN_CMP(datepart, "dd") || TOKEN_CMP(datepart, "day"))
			TOKEN_CHANGE(name, "DAY");
		else
		if(TOKEN_CMP(datepart, "dy") || TOKEN_CMP(datepart, "dayofyear"))
			TOKEN_CHANGE(name, "DAYOFYEAR");
		else
		if(TOKEN_CMP(datepart, "dw") || TOKEN_CMP(datepart, "weekday"))
			TOKEN_CHANGE(name, "DAYOFWEEK");
		else
		if(TOKEN_CMP(datepart, "hh") || TOKEN_CMP(datepart, "hour"))
			TOKEN_CHANGE(name, "HOUR");
		else
		if(TOKEN_CMP(datepart, "mi") || TOKEN_CMP(datepart, "minute"))
			TOKEN_CHANGE(name, "MINUTE");
		else
		if(TOKEN_CMP(datepart, "ss") || TOKEN_CMP(datepart, "second"))
			TOKEN_CHANGE(name, "SECOND");
		else
		if(TOKEN_CMP(datepart, "ms") || TOKEN_CMP(datepart, "millisecond"))
		{
			TOKEN_CHANGE(name, "MICROSECOND");
			PREPEND(name, "CAST(");
			APPEND_FMT(close, "/1000 AS INT)", name);
		}
		else
		if(TOKEN_CMP(datepart, "us") || TOKEN_CMP(datepart, "microsecond"))
			TOKEN_CHANGE(name, "MICROSECOND");
	}

	FUNC_STATS(name);
	return true;
}

// DATE_SUB in MySQL
bool SqlParser::ParseFunctionDatesub(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse first datetime expression
	ParseExpression(date);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *interval = GetNextToken();

	// Parse interval
	ParseExpression(interval);

	Token *close = GetNextCharToken(')', L')');

	// date - interval in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Remove(name);

		Token::Change(comma, " -", L" -", 2);

		Token::Remove(open);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// DATETIME in Informix and Sybase ASA
bool SqlParser::ParseFunctionDatetime(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// DATETIME (literal) field qualifier in Informix
	if(_source == SQL_INFORMIX)
		return ParseFunctionDatetimeInformix(name, open);

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_TIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_TIMESTAMP", L"TO_TIMESTAMP", 12);
	else
	// CONVERT(DATETIME, exp) in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CONVERT", L"CONVERT", 7);
		Append(open, "DATETIME, ", L"DATETIME, ", 10, name);
	}

	FUNC_STATS(name);
	return true;
}

// DATETIME (literal) field TO filed qualifier in Informix
bool SqlParser::ParseFunctionDatetimeInformix(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *first = NULL;
	Token *last = NULL;

	int num = 0;

	// Literal is specified without quotes, 1899-12-31 00:00:00.000 i.e.
	while(true)
	{
		Token *next = GetNextToken();

		if(next == NULL)
			break;

		if(Token::Compare(next, ')', L')') == true || num > 13)
		{
			PushBack(next);
			break;
		}

		if(num == 0)
			first = next;
		else
			last = next;

		num++;
	}

	Token *close = GetNextCharToken(')', L')');

	// Start and end fields
	Token *field1 = GetNextToken();

	Token *to = GetNextWordToken(field1, "TO", L"TO", 2);

	Token *field2 = GetNextToken(to);

	// YEAR TO FRACTION (full datetime value)
	if(Token::Compare(field1, "YEAR", L"YEAR", 4) == true && 
		Token::Compare(field2, "FRACTION", L"FRACTION", 8) == true)
	{
		// TIMESTAMP 'literal' allow missing fraction, or specify any number of digits in fraction, seconds must be specified
		if(_target == SQL_ORACLE)
		{

			Token::Change(name, "TIMESTAMP", L"TIMESTAMP", 9);

			PrependNoFormat(first, "'", L"'", 1);
			AppendNoFormat(last, "'", L"'", 1);

			Token::Remove(open);
			Token::Remove(close);
			Token::Remove(field1, field2);
		}
	}
	else
	// Starts with year
	if(Token::Compare(field1, "YEAR", L"YEAR", 4) == true)
	{
		// YEAR TO DAY
		if(Token::Compare(field2, "DAY", L"DAY", 3) == true)
		{
			// DATE 'literal' can be used in Oracle
			if(_target == SQL_ORACLE)
			{
				Token::Change(name, "DATE", L"DATE", 4);

				PrependNoFormat(first, "'", L"'", 1);
				AppendNoFormat(last, "'", L"'", 1);

				Token::Remove(open);
				Token::Remove(close);
				Token::Remove(field1, field2);
			}
		}
	}

	FUNC_STATS(name);
	return true;
}

// DAY in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDay(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to EXTRACT in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "EXTRACT(DAY FROM ", L"EXTRACT(DAY FROM ", 17);
		Token::Remove(open);

		// If date expression is date string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Prepend(date, "TO_DATE(", L"TO_DATE(", 8, name);
			Append(date, ")", L")", 1);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DAYNAME in DB2, Sybase ASA
bool SqlParser::ParseFunctionDayname(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	Token *comma = GetNextCharToken(',', L',');
	Token *locale = NULL;
	
	// Locale is optional in DB2
	if(comma != NULL)
		locale = GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);

		// If date expression is date string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Prepend(date, "TO_DATE(", L"TO_DATE(", 8, name);
			Append(date, ")", L")", 1);
		}

		Append(date, ", 'Day'", L", 'Day'", 7);

		if(locale != NULL)
		{
			if(Token::Compare(locale, "'CLDR181_en_US'", L"'CLDR181_en_US'", 15))
				Token::Change(locale, "'NLS_DATE_LANGUAGE = AMERICAN'", L"'NLS_DATE_LANGUAGE = AMERICAN'", 30);
			else
			if(Token::Compare(locale, "'CLDR181_de_DE'", L"'CLDR181_de_DE'", 15))
				Token::Change(locale, "'NLS_DATE_LANGUAGE = GERMAN'", L"'NLS_DATE_LANGUAGE = GERMAN'", 28);
			else
			if(Token::Compare(locale, "'CLDR181_fr_FR'", L"'CLDR181_fr_FR'", 15))
				Token::Change(locale, "'NLS_DATE_LANGUAGE = FRENCH'", L"'NLS_DATE_LANGUAGE = FRENCH'", 28);
		}
	}
	else
	// Convert to DATENAME in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "DATENAME", L"DATENAME", 8);
		Prepend(date, "DW, ", L"DW, ", 4, name);
	}

	FUNC_STATS(name);
	return true;
}

// DAYOFMONTH in Sybase ADS
bool SqlParser::ParseFunctionDayofmonth(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_SQL_SERVER)
		TOKEN_CHANGE(name, "DAY");

	FUNC_STATS(name);
	return true;
}

// DAYOFWEEK in DB2, Sybase ADS
bool SqlParser::ParseFunctionDayofweek(Token *name, Token* open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER(TO_CHAR()) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);
		Append(exp, ", 'D')", L", 'D')", 6);
	}
	else
	if(_target == SQL_SQL_SERVER)
	{
		TOKEN_CHANGE(name, "DATEPART");
		APPEND_NOFMT(open, "dw, ");
	}

	FUNC_STATS(name);
	return true;
}

// DB2 DAYOFWEEK_ISO
bool SqlParser::ParseFunctionDayofweekIso(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER(TO_CHAR()) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);
		Append(exp, ", 'D')", L", 'D')", 6);
	}
	else
	if(_target == SQL_SQL_SERVER)
	{
		// Note. SET DATEFIRST 1 must be executed in SQL Server as the start day is Sunday by default in US (1 in Europe i.e.), while ISO requires Monday
		// SET DATEFIRST 1 is not allowed inside CREATE FUNCTION(!) 
		TOKEN_CHANGE(name, "DATEPART");
		APPEND_FIRST_NOFMT(open, "dw, "); 

		_spl_monday_1 = true;
	}

	FUNC_STATS(name);
	return true;
}

// DAYOFYEAR in DB2, Sybase ADS
bool SqlParser::ParseFunctionDayofyear(Token *name, Token* open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER(TO_CHAR()) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);
		Append(exp, ", 'DDD')", L", 'DDD')", 8);
	}
	else
	if(_target == SQL_SQL_SERVER)
	{
		TOKEN_CHANGE(name, "DATEPART");
		APPEND_NOFMT(open, "dy, ");
	}

	FUNC_STATS(name);
	return true;
}

// DAYS in DB2, Sybase ASA
bool SqlParser::ParseFunctionDays(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	// Sybase ASA allows second parameter: timestamp or integer
	Token *comma = GetNextCharToken(',', L',');

	Token *second = (comma != NULL) ? GetNextToken() : NULL;
	Token *second_end = NULL;

	if(second != NULL)
	{
		ParseExpression(second);
		second_end = GetLastToken();
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_SQL_SERVER)
	{
		// Second parameter is specified
		if(second != NULL)
		{
			// An integer constant means to add the specified number of days in Sybase ASA 
			if(second->IsNumeric() == true)
			{
				// DATEADD(DAY, num, date) in SQL Server
				Token::Change(name, "DATEADD", L"DATEADD", 7);
				Append(open, "DAY, ", L"DAY, ", 5, name);

				// Move number of days expression to second parameter
				AppendCopy(open, second, second_end);
				AppendNoFormat(open, ", ", L", ", 2);

				Token::Remove(comma, second_end);
			}
		}
	}
	else
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRUNC", L"TRUNC", 5);

		// If date expression is date string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Prepend(date, "TO_DATE(", L"TO_DATE(", 8, name);
			Append(date, ")", L")", 1);
		}

		if(_source == SQL_DB2)
			Append(date, " - DATE '0001-01-02'", L" - DATE '0001-01-02'", 20, name);
		else
		if(_source == SQL_SYBASE_ASA)
			Append(date, " - DATE '0000-03-02'", L" - DATE '0000-03-02'", 20, name);
	}

	FUNC_STATS(name);
	return true;
}

// Oracle, DB2 DENSE_RANK function
bool SqlParser::ParseFunctionDenseRank(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

	if(over != NULL)
		ParseAnalyticFunctionOverClause(over);
	
	FUNC_STATS(name);
	return true;
}

// DB2 DBCLOB
bool SqlParser::ParseFunctionDbclob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CLOB in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CLOB", L"TO_CLOB", 7);

		// Second parameter is removed
		Token::Remove(comma);
		Token::Remove(exp2);
	}

	FUNC_STATS(name);
	return true;
}

// DB_ID in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDbId(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *database_name = GetNextToken();

	// Parse database_name
	if(Token::Compare(database_name, ')', L')') == true)
	{
		PushBack(database_name);
		database_name = NULL;
	}
	else
		ParseExpression(database_name);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to SYS_CONTEXT('USERENV', 'CURRENT_SCHEMAID') in Oracle
	if(_target == SQL_ORACLE)
	{
		if(database_name == NULL)
		{
			Token::Change(name, "SYS_CONTEXT('USERENV', 'CURRENT_SCHEMAID'", 
				L"SYS_CONTEXT('USERENV', 'CURRENT_SCHEMAID'", 41);
			Token::Remove(open);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DBINFO in Informix
bool SqlParser::ParseFunctionDbinfo(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	Token *close = GetNextCharToken(')', ')');

	// "sqlca.sqlerrd1" or 'sqlca.sqlerrd1' mean the last SERIAL value (see also LET var = DBINFO conversion pattern)
	if(exp->Compare("\"sqlca.sqlerrd1\"", L"\"sqlca.sqlerrd1\"", 16) == true ||
		exp->Compare("'sqlca.sqlerrd1'", L"'sqlca.sqlerrd1'", 16) == true)
	{
		// table_name_seq.CURRVAL in Oracle
		if(_target == SQL_ORACLE)
		{
			// Use the sequence for the table from last INSERT statement
			PrependCopy(name, _spl_last_insert_table_name);
			PrependNoFormat(name, "_seq", L"_seq", 4);

			Token::Change(name, ".CURRVAL", L".CURRVAL", 8);
			Token::Remove(open, close);
		}
	}
	else
	// "sqlca.sqlerrd2" or 'sqlca.sqlerrd2' mean the number of affected rows by SELECT, UPDATE, DELETE, INSERT
	if(exp->Compare("\"sqlca.sqlerrd2\"", L"\"sqlca.sqlerrd2\"", 16) == true ||
		exp->Compare("'sqlca.sqlerrd2'", L"'sqlca.sqlerrd2'", 16) == true)
	{
		// SQL%ROWCOUNT in Oracle
		if(_target == SQL_ORACLE)
		{
			Token::Change(name, "SQL%ROWCOUNT", L"SQL%ROWCOUNT", 12);
			Token::Remove(open, close);
		}
		else
		// @@ROWCOUNT in SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "@@ROWCOUNT", L"@@ROWCOUNT", 10);
			Token::Remove(open, close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DB_INSTANCEID in Sybase ASE
bool SqlParser::ParseFunctionDbInstanceid(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	if(Token::Compare(exp, ')', L')') == true)
	{
		PushBack(exp);
		exp = NULL;
	}
	else
		ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to SYS_CONTEXT('USERENV', 'CURRENT_SCHEMAID') in Oracle
	if(_target == SQL_ORACLE)
	{
		if(exp == NULL)
		{
			Token::Change(name, "SYS_CONTEXT('USERENV', 'INSTANCE'", L"SYS_CONTEXT('USERENV', 'INSTANCE'", 33);
			Token::Remove(open);
		}
	}
	else
	// Convert to @@SERVICENAME in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(exp == NULL)
		{
			Token::Change(name, "@@SERVICENAME", L"@@SERVICENAME", 13);
			Token::Remove(open, close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DB_NAME in SQL, Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDbName(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *database_id = GetNextToken();

	// Parse database_id
	if(Token::Compare(database_id, ')', L')') == true)
	{
		PushBack(database_id);
		database_id = NULL;
	}
	else
		ParseExpression(database_id);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to SYS_CONTEXT('USERENV', 'CURRENT_SCHEMAID') in Oracle
	if(_target == SQL_ORACLE)
	{
		if(database_id == NULL)
		{
			Token::Change(name, "SYS_CONTEXT('USERENV', 'CURRENT_SCHEMA'", 
				L"SYS_CONTEXT('USERENV', 'CURRENT_SCHEMA'", 39);
			Token::Remove(open);
		}
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
		TOKEN_CHANGE(name, "DATABASE");

	FUNC_STATS(name);
	return true;
}

// DB2 DECFLOAT
bool SqlParser::ParseFunctionDecfloat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER", L"TO_NUMBER", 9);

		// Second parameter is removed
		Token::Remove(comma);
		Token::Remove(exp2);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 DECFLOAT_FORMAT
bool SqlParser::ParseFunctionDecfloatFormat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Format DB2 and Oracle the same
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_NUMBER", L"TO_NUMBER", 9);

	FUNC_STATS(name);
	return true;
}

// DB2 DECIMAL
bool SqlParser::ParseFunctionDecimal(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	// Possible conflict with data type, so check the source
	if(_source != SQL_DB2)
		return false;

	STATS_DECL
	Token *prev = GetPrevToken(name);

	// For DB2 make sure the previous token is not a column name
	if(prev != NULL && (prev->type == TOKEN_WORD || prev->type == TOKEN_IDENT))
		return false;
	
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *comma2 = NULL;
	Token *comma3 = NULL;
	
	Token *prec = NULL;
	Token *scale = NULL;
	Token *dec = NULL;
	
	// Precision is optional
	if(comma != NULL)
	{
		prec = GetNextToken();

		if(prec == NULL)
			return false;

		ParseExpression(prec);
	}

	if(prec != NULL)
	{
		comma2 = GetNextCharToken(',', L',');
		
		// Scale is optional
		if(comma2 != NULL)
		{
			scale = GetNextToken();

			if(scale == NULL)
				return false;

			ParseExpression(scale);
		}
	}

	if(scale != NULL)
	{
		comma3 = GetNextCharToken(',', L',');
		
		// Decimal character is optional
		if(comma3 != NULL)
		{
			dec = GetNextToken();

			if(dec == NULL)
				return false;

			ParseExpression(dec);
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_NUMBER", L"TO_NUMBER", 9);

	FUNC_STATS(name);
	return true;
}

// DECODE in Oracle, DB2, Informix
bool SqlParser::ParseFunctionDecode(Token *decode, Token *open)
{
	if(decode == NULL || open == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_DECODE_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Evaluate expression
	ParseExpression(exp);

	// If target is not Oracle, DB2 or Informix replace with CASE
	if(Target(SQL_ORACLE, SQL_DB2, SQL_INFORMIX) == false)
	{
		Token::Change(decode, "CASE ", L"CASE ", 5);
		Token::Remove(open);
	}

	ListW whens;

	bool null_exists = false;

	// Handle conditions
	while(true)
	{
		Token *comma1 = GetNextCharToken(',', L',');

		if(comma1 == NULL)
			break;

		// WHEN condition (or ELSE if there is no corresponding THEN)
		Token *when = GetNextToken();

		if(when == NULL)
			break;

		ParseExpression(when);

		Token *comma2 = GetNextCharToken(',', L',');

		// It is ELSE condition
		if(comma2 == NULL)
		{
			if(_target != SQL_ORACLE && _target != SQL_INFORMIX)
            {
                Prepend(when, " ELSE ", L" ELSE ", 6, decode);
                Token::Remove(comma1);
            }
			
			break;
		}

		// Check is WHEN condition contain NULL in DECODE
		if(when->Compare("NULL", L"NULL", 4) == true)
			null_exists = true;

		whens.Add(when);

		// Change commas to WHEN THEN
		if(_target != SQL_ORACLE && _target != SQL_INFORMIX)
		{
            Prepend(when, " WHEN ", L" WHEN ", 6, decode);
            Token::Remove(comma1);
			Token::Change(comma2, " THEN ", L" THEN ", 6, decode);
		}

		Token *then = GetNextToken();

		if(then == NULL)
			break;

		ParseExpression(then);
	} 

	Token *close = GetNextCharToken(')', L')');

	if(_target != SQL_ORACLE && _target != SQL_INFORMIX)
		Token::Change(close, " END", L" END", 4, decode);	

	// Check if we need to use searched form
	if(null_exists == true)
	{
		// Remove expression after case
		Token::Remove(exp);

		ListwItem *cur = whens.GetFirst();

		// Go through all WHEN conditions again
		while(cur != NULL)
		{
			Token *when = (Token*)cur->value;

			PrependCopy(when, exp);

			// add 'exp IS' before NULL, and exp = in other cases
			if(when->Compare("NULL", L"NULL", 4) == true)
				Prepend(when, " IS ", L" IS ", 4);
			else
				Prepend(when, " = ", L" = ", 3);

			cur = cur->next;
		}
	}

	FUNC_STATS(decode);
	return true;
}

// DEGREES in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionDegrees(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	// Convert to (EXP) * 180/3.1415926535 in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Remove(name);
		Append(close, " * 180/3.1415926535", L" * 180/3.1415926535", 19);
	}

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle DEREF
bool SqlParser::ParseFunctionDeref(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 DIGITS
bool SqlParser::ParseFunctionDigits(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TRANSLATE in Oracle, if 3rd parameter is '' then everything removed
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRANSLATE", L"TRANSLATE", 9);
		Append(exp, ", '0-+.,', '0'", L", '0-+.,', '0'", 14);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 DOUBLE
bool SqlParser::ParseFunctionDouble(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER", L"TO_NUMBER", 9);
	}

	FUNC_STATS(name);
	return true;
}

// DOW in Sybase ASA
bool SqlParser::ParseFunctionDow(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);

		// If date expression is date string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Prepend(date, "TO_DATE(", L"TO_DATE(", 8, name);
			Append(date, ")", L")", 1);
		}

		Append(date, ", 'D')", L", 'D')", 6);
	}
	else
	// Convert to DATEPART in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "DATEPART", L"DATEPART", 8);
		Prepend(date, "DW, ", L"DW, ", 4, name);
	}

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle EMPTY_BLOB()
bool SqlParser::ParseFunctionEmptyBlob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	FUNC_STATS(name);
	return true;	
}

// DB2, Oracle EMPTY_CLOB()
bool SqlParser::ParseFunctionEmptyClob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	FUNC_STATS(name);
	return true;	
}

// DB2, Oracle EMPTY_DBCLOB()
bool SqlParser::ParseFunctionEmptyDbclob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// Convert to EMPTY_CLOB() in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "EMPTY_CLOB", L"EMPTY_CLOB", 10);
	}

	FUNC_STATS(name);
	return true;	
}

// DB2, Oracle EMPTY_NCLOB()
bool SqlParser::ParseFunctionEmptyNclob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// Convert to EMPTY_CLOB() in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "EMPTY_CLOB", L"EMPTY_CLOB", 10);
	}

	FUNC_STATS(name);
	return true;	
}

// ERRORMSG in Sybase ASA
bool SqlParser::ParseFunctionErrormsg(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(Token::Compare(exp, ')', L')') == true)
	{
		PushBack(exp);
		exp = NULL;
	}
	else
		ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	// Convert to SQLERRM in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SQLERRM", L"SQLERRM", 7);

		if(exp == NULL)
			Token::Remove(open, close);
	}
	else
	// Convert to STR(@@ERROR) in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(exp == NULL)
		{
			Token::Change(name, "STR", L"STR", 3);
			Append(open, "@@ERROR", L"@@ERROR", 7, name);
		}
	}

	FUNC_STATS(name);
	return true;	
}

// EXP in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionExp(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// EXPRTYPE in Sybase ASA
bool SqlParser::ParseFunctionExprtype(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *string */ (void) GetNextToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	/*Token *integer */ (void) GetNextToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// EXTRACT in Oracle, DB2
bool SqlParser::ParseFunctionExtract(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_EXTRACT_DATETIME_DESC)

	Token *unit = GetNextToken();

	// FROM keyword after unit
	Token *from = GetNextWordToken("FROM", L"FROM", 4);
	
	if(from == NULL)
		return false;

	Token *datetime = GetNextToken();

	// Parse datetime
	ParseExpression(datetime);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(Token::Compare(unit, "YEAR", L"YEAR", 4) == true)
	{
		// Convert to YEAR function in SQL Server, MySQL
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
		{
			Token::Change(name, "YEAR", L"YEAR", 4);
			
			Token::Remove(unit, false);
			Token::Remove(from, false);	
		}
	}
	else
	if(Token::Compare(unit, "MONTH", L"MONTH", 5) == true)
	{
		// Convert to MONTH function in SQL Server, MySQL
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
		{
			Token::Change(name, "MONTH", L"MONTH", 5);
			
			Token::Remove(unit, false);
			Token::Remove(from, false);	
		}
	}
	else
	if(Token::Compare(unit, "DAY", L"DAY", 3) == true)
	{
		// Convert to DAY function in SQL Server, MySQL
		if(Target(SQL_SQL_SERVER, SQL_MARIADB, SQL_MYSQL) == true)
		{
			Token::Change(name, "DAY", L"DAY", 3);
			
			Token::Remove(unit, false);
			Token::Remove(from, false);	
		}
	}
	else
	if(Token::Compare(unit, "HOUR", L"HOUR", 4) == true)
	{
		// Convert to HOUR function in MySQL
		if(Target(SQL_MYSQL) == true)
		{
			Token::Change(name, "HOUR", L"HOUR", 4);
			
			Token::Remove(unit, false);
			Token::Remove(from, false);	
		}
	}
	else
	if(Token::Compare(unit, "MINUTE", L"MINUTE", 6) == true)
	{
		// Convert to MINUTE function in MySQL
		if(Target(SQL_MYSQL) == true)
		{
			Token::Change(name, "MINUTE", L"MINUTE", 6);
			
			Token::Remove(unit, false);
			Token::Remove(from, false);	
		}
	}
	else
	if(Token::Compare(unit, "SECOND", L"SECOND", 6) == true)
	{
		// Convert to SECOND function in MySQL
		if(Target(SQL_MYSQL) == true)
		{
			Token::Change(name, "SECOND", L"SECOND", 6);
			
			Token::Remove(unit, false);
			Token::Remove(from, false);	
		}
	}

	FUNC_STATS(name);
	return true;
}

// FIRST in Sybase ASA
bool SqlParser::ParseFunctionFirst(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	// TOP 1 exp in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "TOP 1", L"TOP 1", 5);
		Token::Change(open, " ", L" ", 1);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 FLOAT
bool SqlParser::ParseFunctionFloat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER", L"TO_NUMBER", 9);
	}

	FUNC_STATS(name);
	return true;
}

// FLOOR in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionFloor(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_FLOOR_DESC)

	// Get dollar sign if exists
	Token *sign = GetNextCharToken('$', L'$');
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(Target(SQL_ORACLE, SQL_MYSQL) == true)
	{
		// Remove dollar sign for Oracle, MySQL FLOOR
		if(sign != NULL)
			Token::Remove(sign);
	}

	STATS_SET_CONV_NO_NEED(true)  
	FUNC_STATS(name);

	return true;
}

// GET_BIT in Sybase ASA
bool SqlParser::ParseFunctionGetBit(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *bit_array = GetNextToken();

	// Parse bit_array
	ParseExpression(bit_array);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *position = GetNextToken();

	// Parse position
	ParseExpression(position);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// GETDATE in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionGetdate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Next must be close parentesis
	Token *close = GetNextCharToken(')', L')');

	// Convert to SYSTIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);
		Token::Remove(open, close);
	}
	else
	if(Target(SQL_MYSQL, SQL_MARIADB))
	{
		TOKEN_CHANGE(name, "NOW");
		APPEND_NOFMT(open, "3");
	}
	else
	if(Target(SQL_POSTGRESQL))
		TOKEN_CHANGE(name, "NOW");

	FUNC_STATS(name);
	return true;
}

// GETUTCDATE in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionGetutcdate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Next is parentesis
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to SYS_EXTRACT_UTC(SYSTIMESTAMP) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SYS_EXTRACT_UTC(SYSTIMESTAMP", L"SYS_EXTRACT_UTC(SYSTIMESTAMP", 28);
		Token::Remove(open);
	}
	else
	// Convert to UTC_TIMESTAMP in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		TOKEN_CHANGE(name, "UTC_TIMESTAMP");
		APPEND_NOFMT(open, "3");
	}

	FUNC_STATS(name);
	return true;
}

// GREATER in Sybase ASA
bool SqlParser::ParseFunctionGreater(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	Token *close = GetNextCharToken(')', L')');

	// Convert to GREATEST in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "GREATEST", L"GREATEST", 8);
	else
	// Convert to CASE expression in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CASE WHEN ", L"CASE WHEN ", 10);
		Token::Change(comma, " >", L" >", 2);

		Append(close, " THEN ", L" THEN ", 6, name);
		AppendCopy(close, exp);
		Append(close, " ELSE ", L" ELSE ", 6, name);
		AppendCopy(close, exp2);
		Append(close, " END", L" END", 4, name);

		Token::Remove(open);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// GREATEST in Oracle, DB2, MySQL
bool SqlParser::ParseFunctionGreatest(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_GREATEST_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	STATS_SET_CONV_NO_NEED(true)
	FUNC_STATS(name);
	return true;
}

// HASH in Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionHash(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	Token *algorithm = NULL;

	// Algorithm is optional
	if(comma != NULL)
		algorithm = GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to HASHBYTES in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "HASHBYTES", L"HASHBYTES", 9);

		// Algorithm is mandatory in SQL Server
		if(algorithm != NULL)
		{
			if(Token::Compare(algorithm, "'SHA256'", L"'SHA256'", 8) == true)
				Token::Change(algorithm, "'SHA2_256'", L"'SHA2_256'", 10);

			AppendCopy(open, algorithm);
			Append(open, ", ", L", ", 2);
			Token::Remove(comma, algorithm);
		}
		else
			// MD5 algorithm is used by default
			Append(open, "'MD5', ", L"'MD5', ", 7);
	}
	else
	// Convert to DBMS_CRYPTO.HASH in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "DBMS_CRYPTO.HASH(UTL_RAW.CAST_TO_RAW", L"DBMS_CRYPTO.HASH(UTL_RAW.CAST_TO_RAW", 36);
		Append(string, ")", L")", 1);

		// Algorithm is mandatory in Oracle
		if(algorithm != NULL)
		{
			if(Token::Compare(algorithm, "'MD5'", L"'MD5'", 5) == true)
				Token::Change(algorithm, "2", L"2", 1);
			else
			if(Token::Compare(algorithm, "'SHA1'", L"'SHA1'", 6) == true)
				Token::Change(algorithm, "3", L"3", 1);
		}
		else
			// MD5 algorithm is used by default, that corresponds 2 in Oracle
			Append(string, ", 2", L", 2", 3);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		if(algorithm == NULL || TOKEN_CMP(algorithm, "'MD5'"))
		{
			TOKEN_CHANGE(name, "MD5");
			Token::Remove(comma, algorithm);
		}
		else
		if(TOKEN_CMP(algorithm, "'SHA1'"))
		{
			TOKEN_CHANGE(name, "SHA1");
			Token::Remove(comma, algorithm);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DB2 HEX
bool SqlParser::ParseFunctionHex(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// HEXTOBIGINT in Sybase ASE
bool SqlParser::ParseFunctionHextobigint(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	// Convert to CONVERT in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CONVERT(BIGINT, CONVERT(VARBINARY(8), ", 
			L"CONVERT(BIGINT, CONVERT(VARBINARY(8), ", 38);
		Token::Remove(open);
		Prepend(close, ", 1)", L", 1)", 4);
	}
	else
	// Convert to TO_NUMBER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER", L"TO_NUMBER", 9);
		Prepend(close, ", 'XXXXXXXXXXXXXXXXXXX'", L", 'XXXXXXXXXXXXXXXXXXX'", 23);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		TOKEN_CHANGE(name, "CAST(CONV");
		PREPEND_FMT(close, ",16,10) AS UNSIGNED", name);
	}

	FUNC_STATS(name);
	return true;
}

// HEXTOINT in Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionHextoint(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *hexadecimal = GetNextToken();

	if(hexadecimal == NULL)
		return false;

	// Parse hexadecimal
	ParseExpression(hexadecimal);

	Token *close = GetNextCharToken(')', L')');

	// Convert to CONVERT in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CONVERT(INT, CONVERT(VARBINARY(8), ", L"CONVERT(INT, CONVERT(VARBINARY(8), ", 35);
		Token::Remove(open);
		Prepend(close, ", 1)", L", 1)", 4);
	}
	else
	// Convert to TO_NUMBER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER", L"TO_NUMBER", 9);
		Prepend(close, ", 'XXXXXXXXXXXXXXXXXXX'", L", 'XXXXXXXXXXXXXXXXXXX'", 23);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		TOKEN_CHANGE(name, "CAST(CONV");
		PREPEND_FMT(close, ",16,10) AS UNSIGNED", name);
	}

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle HEXTORAW
bool SqlParser::ParseFunctionHextoraw(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_HEXTORAW_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// HOST_ID in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionHostId(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// HOST_NAME in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionHostName(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to SYS_CONTEXT('USERENV', 'SERVER_HOST') in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SYS_CONTEXT('USERENV', 'SERVER_HOST'", L"SYS_CONTEXT('USERENV', 'SERVER_HOST'", 36);
		Token::Remove(open);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "@@HOSTNAME", L"@@HOSTNAME", 10);
		Token::Remove(open);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// HOUR in DB2, Sybase ASA
bool SqlParser::ParseFunctionHour(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *datetime = GetNextToken();

	if(datetime == NULL)
		return false;

	// Parse datetime
	ParseExpression(datetime);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to EXTRACT in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "EXTRACT(HOUR FROM ", L"EXTRACT(HOUR FROM ", 18);
		Token::Remove(open);

		// If datetime expression is string literal enclose with TO_TIMESTAMP
		if(datetime->type == TOKEN_STRING)
		{
			Prepend(datetime, "TO_TIMESTAMP(", L"TO_TIMESTAMP(", 13, name);
			Append(datetime, ")", L")", 1);
		}
	}
	else
	// Convert to DATEPART in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "DATEPART", L"DATEPART", 8);
		Append(open, "HH, ", L"HH, ", 4, name);
	}

	FUNC_STATS(name);
	return true;
}

// HTML_DECODE in Sybase ASA
bool SqlParser::ParseFunctionHtmlDecode(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// HTML_ENCODE in Sybase ASA
bool SqlParser::ParseFunctionHtmlEncode(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to HTF.ESCAPE_SC in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "HTF.ESCAPE_SC", L"HTF.ESCAPE_SC", 13);
	else
	if(_target == SQL_SQL_SERVER)
	{
		Token::Remove(name);
		Prepend(string, "SELECT ", L"SELECT ", 7, name);
		Append(string, " FOR XML PATH('')", L" FOR XML PATH('')", 17, name);
	}

	FUNC_STATS(name);
	return true;
}

// HTTP_DECODE in Sybase ASA
bool SqlParser::ParseFunctionHttpDecode(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// HTTP_ENCODE in Sybase ASA
bool SqlParser::ParseFunctionHttpEncode(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// IDENTITY in Sybase ASA
bool SqlParser::ParseFunctionIdentity(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	// Convert to ROWNUM in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "ROWNUM", L"ROWNUM", 6);
		Token::Remove(open, close);
	}
	else
	// Convert to ROW_NUMBER in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "ROW_NUMBER", L"ROW_NUMBER", 10);
		Token::Remove(exp);
		Append(close, " OVER(ORDER BY (SELECT 1))", L" OVER(ORDER BY (SELECT 1))", 26, name);
	}

	FUNC_STATS(name);
	return true;
}

// @@IDENTITY in SQL Server
bool SqlParser::ParseFunctionIdentity(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		TOKEN_CHANGE(name, "LAST_INSERT_ID()");

	FUNC_STATS(name);
	return true;
}

// IFNULL in MySQL, Sybase ASA, Sybase ADS
bool SqlParser::ParseFunctionIfnull(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse first expression
	ParseExpression(exp);

	Token *end_exp = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	Token *end_exp2 = GetLastToken();

	Token *comma2 = GetNextCharToken(',', L',');

	Token *exp3 = NULL;
	Token *end_exp3 = NULL;

	// Third expression is optional in Sybase ASA; only 2 expressions allowed in MySQL and Sybase ADS
	if(comma2 != NULL)
	{
		exp3 = GetNextToken();

		// Parse third expression
		ParseExpression(exp3);

		end_exp3 = GetLastToken();
	}

	Token *close = GetNextCharToken(')', L')');

	// Convert to NVL or NVL2 in Oracle
	if(_target == SQL_ORACLE)
	{
		if(_source == SQL_MYSQL)
			Token::Change(name, "NVL", L"NVL", 3);
		else
		if(_source == SQL_SYBASE_ASA)
		{
			Token::Change(name, "NVL2", L"NVL2", 4);

			// If 3 parameters are specified, the order of 2nd and 3rd parameters is different in NVL2
			if(exp3 != NULL)
			{
				AppendNoFormat(comma, " ", L" ", 1);
				AppendCopy(comma, exp3, end_exp3);
				AppendNoFormat(comma, ", ", L", ", 2);

				Token::Remove(comma2, end_exp3);
			}
			// If 2 parameters specified, second must be NULL in NVL2 (see Sybase IFNULL docs)
			else
				Prepend(exp2, "NULL, ", L"NULL, ", 6, name);
		}
	}
	else
	// Convert to CASE expression in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		// Sybase ADS support 2 arguments only
		if(_source == SQL_SYBASE_ADS)
			TOKEN_CHANGE(name, "ISNULL");
		else
		{
			Token::Change(name, "CASE WHEN ", L"CASE WHEN ", 10);
			Token::Remove(open);
			Append(end_exp, " IS NULL THEN ", L" IS NULL THEN ", 14, name);
			Token::Remove(comma);

			if(exp3 != NULL)
			{
				Append(end_exp2, " ELSE ", L" ELSE ", 6, name);
				Token::Remove(comma2);
				Append(end_exp3, " END", L" END", 4, name);
			}
			else
				Append(end_exp2, " ELSE NULL END", L" ELSE NULL END", 14, name);

			Token::Remove(close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// IIF in SQL Server, Sybase ADS
bool SqlParser::ParseFunctionIif(Token *name, Token * /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	ParseBooleanExpression(SQL_BOOL_IF_EXP);

	Token *comma = TOKEN_GETNEXT(',');

	if(comma == NULL)
		return false;

	Token *true_exp = GetNextToken();

	// Parse the TRUE condition expression
	ParseExpression(true_exp);

	Token *comma2 = TOKEN_GETNEXT(',');

	if(comma2 == NULL)
		return false;

	Token *false_exp = GetNextToken();

	// Parse the FALSE condition expression
	ParseExpression(false_exp);

	/*Token *close */ (void) TOKEN_GETNEXT(')');	

	FUNC_STATS(name);
	return true;
}

// INDEX_COL in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionIndexCol(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *object = GetNextToken();

	if(object == NULL)
		return false;

	// Parse object
	ParseExpression(object);

	/*Token *comma */ (void) GetNextCharToken(',', L',');
	Token *index = GetNextToken();

	if(index == NULL)
		return false;

	// Parse index
	ParseExpression(index);

	/*Token *comma2 */ (void) GetNextCharToken(',', L',');
	Token *key = GetNextToken();

	if(key == NULL)
		return false;

	// Parse key
	ParseExpression(key);

	Token *comma3 = GetNextCharToken(',', L',');

	// User is optional in Sybase ASE
	if(comma3 != NULL)
	{
		Token *user = GetNextToken();

		if(user == NULL)
			return false;

		// Parse user
		ParseExpression(user);

		// If target type is not Sybase ASE remove user for INDEX_COL
		if(_target != SQL_SYBASE)
			Token::Remove(comma3, user);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// INDEX_COLORDER in Sybase ASE
bool SqlParser::ParseFunctionIndexColorder(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *object = GetNextToken();

	if(object == NULL)
		return false;

	// Parse object
	ParseExpression(object);

	/*Token *comma */ (void) GetNextCharToken(',', L',');
	Token *index = GetNextToken();

	if(index == NULL)
		return false;

	// Parse index
	ParseExpression(index);

	/*Token *comma2 */ (void) GetNextCharToken(',', L',');
	Token *key = GetNextToken();

	if(key == NULL)
		return false;

	// Parse key
	ParseExpression(key);

	Token *comma3 = GetNextCharToken(',', L',');

	// User is optional in Sybase ASE
	if(comma3 != NULL)
	{
		Token *user = GetNextToken();

		if(user == NULL)
			return false;

		// Parse user
		ParseExpression(user);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// INDEX_NAME in Sybase ASE
bool SqlParser::ParseFunctionIndexName(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *dbid = GetNextToken();

	if(dbid == NULL)
		return false;

	// Parse dbid
	ParseExpression(dbid);

	/*Token *comma */ (void) GetNextCharToken(',', L',');
	Token *objid = GetNextToken();

	if(objid == NULL)
		return false;

	// Parse objid
	ParseExpression(objid);

	/*Token *comma2 */ (void) GetNextCharToken(',', L',');
	Token *indid = GetNextToken();

	if(indid == NULL)
		return false;

	// Parse indid
	ParseExpression(indid);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// INITCAP in Oracle, DB2
bool SqlParser::ParseFunctionInitcap(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// INSERT in DB2, MySQL, MariaDB, Sybase ADS
bool SqlParser::ParseFunctionInsert(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *source = GetNextToken();

	if(source == NULL)
		return false;

	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *start = GetNextToken();

	if(start == NULL)
		return false;

	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');
	
	if(comma2 == NULL)
		return false;

	Token *length = GetNextToken();

	if(length == NULL)
		return false;

	ParseExpression(length);

	Token *comma3 = GetNextCharToken(',', L',');
	
	if(comma3 == NULL)
		return false;

	Token *insert_string = GetNextToken();

	if(insert_string == NULL)
		return false;

	ParseExpression(insert_string);

	Token *comma4 = GetNextCharToken(',', L',');

	// Character set is optional in DB2
	if(comma4 != NULL)
		/*Token *unit */ (void) GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(Target(SQL_SQL_SERVER))
		TOKEN_CHANGE(name, "STUFF");

	FUNC_STATS(name);
	return true;
}

// INSERTSTR in Sybase ASA
bool SqlParser::ParseFunctionInsertstr(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *position = GetNextToken();

	// Parse position
	ParseExpression(position);

	Token *end_pos = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *end_str = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	
	if(comma2 == NULL)
		return false;

	Token *string2 = GetNextToken();

	// Parse string2
	ParseExpression(string2);

	Token *end_str2 = GetLastToken();
	Token *close = GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		int num = position->GetInt();

		if(num == 0)
		{
			AppendCopy(close, string2);
			Append(close, " || SUBSTR(", L" || SUBSTR(", 11, name);
			AppendCopy(close, string, end_str);
			Append(close, ", 1)", L", 1)", 4);

			Token::Remove(name, close);
		}
		else
		{
			Token::Change(name, "SUBSTR", L"SUBSTR", 6);

			Append(close, " || ", L" || ", 4);
			AppendCopy(close, string2, end_str2);
			Append(close, " || SUBSTR(", L" || SUBSTR(", 11, name);
			AppendCopy(close, string, end_str);
			Append(close, ", ", L", ", 2);
			AppendCopy(close, position, end_pos);
			Append(close, "+1)", L"+1)", 3, name);

			AppendCopy(open, string, end_str);
			Append(open, ", 1, ", L", 1, ", 5);
			Token::Remove(comma, end_str2);
		}
	}
	else
	if(_target == SQL_SQL_SERVER)
	{
		int num = position->GetInt();

		if(num == 0)
		{
			AppendCopy(close, string2, end_str2);
			Append(close, " + SUBSTRING(", L" + SUBSTRING(", 13, name);
			AppendCopy(close, string, end_str);
			Append(close, ", 1, LEN(", L", 1, LEN(", 9, name);
			AppendCopy(close, string, end_str);
			Append(close, "))", L"))", 2);

			Token::Remove(name, close);
		}
		else
		{
			Token::Change(name, "SUBSTRING", L"SUBSTRING", 9);

			Append(close, " + ", L" + ", 3);
			AppendCopy(close, string2, end_str2);
			Append(close, " + SUBSTRING(", L" + SUBSTRING(", 13, name);
			AppendCopy(close, string, end_str);
			Append(close, ", ", L", ", 2);
			AppendCopy(close, position, end_pos);
			Append(close, "+1, LEN(", L"+1, LEN(", 8, name);
			AppendCopy(close, string, end_str);
			Append(close, ")-", L")-", 2);
			AppendCopy(close, position, end_pos);
			Append(close, ")", L")", 1);

			AppendCopy(open, string, end_str);
			Append(open, ", 1, ", L", 1, ", 5);
			Token::Remove(comma, end_str2);
		}
	}

	FUNC_STATS(name);
	return true;
}

// INSTANCE_ID in Sybase ASE
bool SqlParser::ParseFunctionInstanceId(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(Token::Compare(exp, ')', L')') == true)
	{
		PushBack(exp);
		exp = NULL;
	}
	else
		// Parse expression
		ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to SYS_CONTEXT('USERENV', 'INSTANCE') in Oracle
	if(_target == SQL_ORACLE)
	{
		if(exp == NULL)
		{
			Token::Change(name, "SYS_CONTEXT('USERENV', 'INSTANCE'", L"SYS_CONTEXT('USERENV', 'INSTANCE'", 33);
			Token::Remove(open);
		}
	}
	else
	// Convert to @@SERVICENAME in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(exp == NULL)
		{
			Token::Change(name, "@@SERVICENAME", L"@@SERVICENAME", 13);
			Token::Remove(open, close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// INSTANCE_NAME in Sybase ASE
bool SqlParser::ParseFunctionInstanceName(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(Token::Compare(exp, ')', L')') == true)
	{
		PushBack(exp);
		exp = NULL;
	}
	else
		// Parse expression
		ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to SYS_CONTEXT('USERENV', 'INSTANCE_NAME') in Oracle
	if(_target == SQL_ORACLE)
	{
		if(exp == NULL)
		{
			Token::Change(name, "SYS_CONTEXT('USERENV', 'INSTANCE_NAME'", 
				L"SYS_CONTEXT('USERENV', 'INSTANCE_NAME'", 38);
			Token::Remove(open);
		}
	}
	else
	// Convert to @@SERVICENAME in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(exp == NULL)
		{
			Token::Change(name, "@@SERVICENAME", L"@@SERVICENAME", 13);
			Token::Remove(open, close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// INSTR in Oracle, DB2, MySQL
bool SqlParser::ParseFunctionInstr(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_INSTR_DESC)

	Token *source = GetNextToken();

	if(source == NULL)
		return false;

	// Parse source
	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *search = GetNextToken();

	if(search == NULL)
		return false;

	// Parse search
	ParseExpression(search);

	Token *end_search = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	Token *start = NULL;
	Token *comma3 = NULL;
	Token *instance = NULL;
	
	// Start is optional in Oracle, DB2
	if(comma2 != NULL)
	{
		start = GetNextToken();

		if(start == NULL)
			return false;

		// Parse start
		ParseExpression(start);

		comma3 = GetNextCharToken(',', L',');
	
		// Instance is optional in Oracle, DB2
		if(comma3 != NULL)
		{
			instance = GetNextToken();

			if(instance == NULL)
				return false;

			// Parse instance
			ParseExpression(instance);
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(Target(SQL_SQL_SERVER))
	{
		// For 2 and 3 parameters, convert to CHARINDEX
		if(instance == NULL)
		{
			TOKEN_CHANGE(name, "CHARINDEX");

			// Different order of parameters
			AppendCopy(open, search, end_search);
			APPEND_NOFMT(open, ", ");
			Token::Remove(comma, end_search);
		}
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		// For 3 parameters, convert to LOCATE in MySQL
		if(start != NULL && instance == NULL)
		{
			Token::Change(name, "LOCATE", L"LOCATE", 6);
			AppendCopy(open, search, end_search);
			Append(open, ", ", L", ", 2);

			Token::Remove(comma, end_search);
		}
	}

	name->subtype = TOKEN_SUB_FUNC_STRING;

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle INSTRB
bool SqlParser::ParseFunctionInstrb(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_INSTR_DESC)

	Token *source = GetNextToken();

	if(source == NULL)
		return false;

	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *search = GetNextToken();

	if(search == NULL)
		return false;

	ParseExpression(search);

	Token *comma2 = GetNextCharToken(',', L',');
	
	if(comma2 == NULL)
		return false;

	Token *start = GetNextToken();

	if(start == NULL)
		return false;

	ParseExpression(start);

	Token *comma3 = GetNextCharToken(',', L',');
	
	if(comma3 == NULL)
		return false;

	Token *instance = GetNextToken();

	if(instance == NULL)
		return false;

	ParseExpression(instance);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 INTEGER, INT function
bool SqlParser::ParseFunctionInteger(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	STATS_DECL
	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	// Convert to TO_NUMBER in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRUNC(TO_NUMBER", L"TRUNC(TO_NUMBER", 15);
		Append(close, ")", L")", 1);
	}
	else
	if(!Target(SQL_DB2))
	{
		TOKEN_CHANGE(name, "CAST");
		PREPEND_FMT(close, " AS INT", name);
	}

	FUNC_STATS(name);
	return true;
}

// INTTOHEX in Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionInttohex(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *integer = GetNextToken();
	
	if(integer == NULL)
		return false;

	// Parse integer
	ParseExpression(integer);

	Token *close = GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CHAR(TO_CHAR", L"TO_CHAR(TO_CHAR", 15);
		Append(close, ", 'XXXXXXXX')", L", 'XXXXXXXX')", 13);
	}
	else
	// Convert to CONVERT in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CONVERT(VARBINARY(8), ", L"CONVERT(VARBINARY(8), ", 22);
		Token::Remove(open);
	}
	else
	if(Target(SQL_MYSQL, SQL_MARIADB))
	{
		TOKEN_CHANGE(name, "LPAD(HEX");
		PREPEND_NOFMT(close, "), 8, '0'");
	}
	
	FUNC_STATS(name);
	return true;	
}

// ISDATE in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionIsdate(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	FUNC_STATS(name);
	return true;	
}

// ISNULL in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionIsnull(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *replace = GetNextToken();

	if(replace == NULL)
		return false;

	// Parse replace
	ParseExpression(replace);

	int num = 2;

	// Variable number of parameters in Sybase ASA
	while(true)
	{
		Token *comman = GetNextCharToken(',', L',');

		if(comman == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);

		num++;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// In SQL Server, use ISNULL if there are 2 parameters, otherwise COALESCE
	if(_target == SQL_SQL_SERVER && num > 2)
		Token::Change(name, "COALESCE", L"COALESCE", 8);
	else
	// Convert to NVL in Oracle
	if(_target == SQL_ORACLE)
	{
		if(num > 2)
			Token::Change(name, "COALESCE", L"COALESCE", 8);
		else
			Token::Change(name, "NVL", L"NVL", 3);
	}
	else
	// Convert to IFNULL in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "IFNULL", L"IFNULL", 6);
	else
	// Convert to COALESCE in PostgreSQL
	if(_target == SQL_POSTGRESQL)
		Token::Change(name, "COALESCE", L"COALESCE", 8);
	
	// Set string datatype for ISNULL(exp, 'string');
	if(replace->type == TOKEN_STRING && replace->IsNumericInString() == false)
		name->data_type = TOKEN_DT_STRING;

	FUNC_STATS(name);
	return true;	
}

// ISNUMERIC in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionIsnumeric(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	FUNC_STATS(name);
	return true;	
}

// IS_SINGLEUSERMODE in Sybase ASE
bool SqlParser::ParseFunctionIsSingleusermode(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;
	
	FUNC_STATS(name);
	return true;	
}

// DB2 JULIAN_DAY
bool SqlParser::ParseFunctionJulianDay(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER(TO_CHAR()) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);
		Append(exp, ", 'J')", L", 'J')", 6);
	}

	FUNC_STATS(name);
	return true;
}

// LAST_DAY in Oracle, DB2, MySQL
bool SqlParser::ParseFunctionLastDay(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_LAST_DAY_DESC)

	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// LASTAUTOINC in Sybase ADS
bool SqlParser::ParseFunctionLastAutoInc(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *option = GetNextToken();
	Token *close = (option !=  NULL) ? TOKEN_GETNEXT(')') : NULL;

	// Statement level identity value (the same scope)
	if(TOKEN_CMP(option, "STATEMENT"))
	{
		if(_target == SQL_SQL_SERVER)
		{
			TOKEN_CHANGE(name, "SCOPE_IDENTITY");
			Token::Remove(option);
		}
	}
	else
	// Connection level identity value (all scopes)
	if(TOKEN_CMP(option, "CONNECTION"))
	{
		if(_target == SQL_SQL_SERVER)
		{
			TOKEN_CHANGE(name, "@@IDENTITY");
			Token::Remove(open, close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// LCASE in DB2, Sybase ASA
bool SqlParser::ParseFunctionLcase(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LOWER in Oracle, SQL Server
	if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
		Token::Change(name, "LOWER", L"LOWER", 5);

	FUNC_STATS(name);
	return true;
}

// LEAST in Oracle, DB2, MySQL
bool SqlParser::ParseFunctionLeast(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_LEAST_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	STATS_SET_CONV_NO_NEED(true)
	FUNC_STATS(name);
	return true;
}

// LEFT in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionLeft(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *length = GetNextToken();

	// Parse length
	ParseExpression(length);

	Token *comma2 = GetNextCharToken(',', L',');

	// String unit is optional in DB2
	if(comma2 != NULL)
		/*Token *unit */ (void) GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to SUBSTR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SUBSTR", L"SUBSTR", 6);
		Append(comma, " 1,", L" 1,", 3);
	}

	FUNC_STATS(name);
	return true;
}

// LEN in SQL Server, Sybase ASE, Sybase ASA, Informix
bool SqlParser::ParseFunctionLen(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	Token *close = GetNextCharToken(')', L')');

	bool spaces_included = true;

	// SQL Server LEN excludes trailing spaces
	if(_source == SQL_SQL_SERVER)
		spaces_included = false;

	// Convert to LENGTH in Oracle
	if(_target == SQL_ORACLE)
	{
		if(spaces_included == false)
		{
			Token::Change(name, "LENGTH(RTRIM", L"LENGTH(RTRIM", 12);
			AppendNoFormat(close, ")", L")", 1);
		}
		else
			Token::Change(name, "LENGTH", L"LENGTH", 6);
	}
	else
	// Convert to CHAR_LENGTH in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		if(spaces_included == false)
		{
			Token::Change(name, "CHAR_LENGTH(RTRIM", L"CHAR_LENGTH(RTRIM", 17);
			AppendNoFormat(close, ")", L")", 1);
		}
		else
			Token::Change(name, "CHAR_LENGTH", L"CHAR_LENGTH", 11);
	}

	FUNC_STATS(name);
	return true;
}

// LENGTH in Oracle, DB2, Informix, Sybase ASA
bool SqlParser::ParseFunctionLength(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_LENGTH_DESC)

	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LEN in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "LEN", L"LEN", 3);
	else
	// Convert to CHAR_LENGTH in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "CHAR_LENGTH", L"CHAR_LENGTH", 11);

	FUNC_STATS(name);
	return true;
}

// LENGTHB in Oracle
bool SqlParser::ParseFunctionLengthb(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_LENGTH_DESC)

	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LENGTH in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "LENGTH", L"LENGTH", 6);

	FUNC_STATS(name);
	return true;
}

// LESSER in Sybase ASA
bool SqlParser::ParseFunctionLesser(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse first expression
	ParseExpression(exp);

	Token *end_exp = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	Token *end_exp2 = GetLastToken();
	Token *close = GetNextCharToken(')', L')');

	// Convert to LEAST in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "LEAST", L"LEAST", 5);
	else
	// Convert to CASE expression in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CASE WHEN ", L"CASE WHEN ", 10);
		Token::Change(comma, " <", L" <", 2);

		Append(close, " THEN ", L" THEN ", 6, name);
		AppendCopy(close, exp, end_exp);
		Append(close, " ELSE ", L" ELSE ", 6, name);
		AppendCopy(close, exp2, end_exp2);
		Append(close, " END", L" END", 4, name);

		Token::Remove(open);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// LIST in Sybase ASA
bool SqlParser::ParseFunctionList(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	// Optional ALL or DISTINCT keyword
	STATS_DECL
	Token *all = GetNextWordToken("ALL", L"ALL", 3);

	Token *distinct = (all == NULL) ? GetNextWordToken("DISTINCT", L"DISTINCT", 8) : NULL;

	Token *exp = GetNextToken();

	// Parse first expression
	ParseExpression(exp);

	// Optional delimiter (default is ',' in Sybase ASA)
	Token *comma = GetNextCharToken(',', L',');

	Token *del = (comma != NULL) ? GetNextToken() : NULL;

	Token *close = GetNextCharToken(')', L')');

	// STUFF and FOR XML XPATH in SQL Server (this is only the first part of conversion; it will be completed in 
	// the end of SELECT
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "STUFF", L"STUFF", 5);
		Append(open, "(SELECT ", L"(SELECT ", 8, name);

		// Token after which to append delimiter
		Token *del_append = open;

		if(distinct != NULL)
			del_append = distinct;
		else
		if(all != NULL)
			del_append = all;

		if(del_append != open)
			Append(del_append, " ", L" ", 1);

		// If delimiter is specified append it before expression, otherwise use default ',' delimiter
		if(del != NULL)
		{
			AppendCopy(del_append, del);
			Token::Remove(comma, del);
		}
		else
			Append(del_append, "','", L"','", 3);

		Append(del_append, " + ", L" + ", 3);

		// Close will be added after SELECT statement
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// LN in Oracle, DB2, MySQL
bool SqlParser::ParseFunctionLn(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// LOCALTIMESTAMP in Oracle, MySQL
bool SqlParser::ParseFunctionLocaltimestamp(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *open = GetNextCharToken('(', L'(');
	Token *prec = NULL;

	// Prec is optional
	if(open != NULL)
	{
		prec = GetNextToken();

		if(prec == NULL)
			return false;

		// Parse prec
		ParseExpression(prec);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Remove prec in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		if(prec != NULL)
			Token::Remove(prec);
	}

	FUNC_STATS(name);
	return true;
}

// LOCATE in DB2, Sybase ASA
bool SqlParser::ParseFunctionLocate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string1 = GetNextToken();

	// Parse first string
	ParseExpression(string1);

	Token *comma1 = GetNextCharToken(',', L',');
	
	if(comma1 == NULL)
		return false;

	Token *string2 = GetNextToken();

	// Parse second string
	ParseExpression(string2);

	Token *end_str2 = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	Token *start = NULL;
	Token *comma3 = NULL;
	Token *unit = NULL;

	// Start is optional
	if(comma2 != NULL)
	{
		start = GetNextToken();

		// For DB2, if start paramter points to string unit this means start parameter is skipped 
		if(Token::Compare(start, "CODEUNITS16", L"CODEUNITS16", 11) == true ||
			Token::Compare(start, "CODEUNITS32", L"CODEUNITS32", 11) == true ||
			Token::Compare(start, "OCTETS", L"OCTETS", 6) == true)
		{
			unit = start;
			comma3 = comma2;
			start = NULL;
			comma2 = NULL;
		}
		else
			// Parse start
			ParseExpression(start);
		
		// String unit is optional in DB2
		if(comma3 == NULL)
		{
			comma3 = GetNextCharToken(',', L',');

			if(comma3 != NULL)
				unit = GetNextToken();
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to INSTR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "INSTR", L"INSTR", 5);

		// In Oracle change the order of search and source parameters for DB2 LOCATE
		if(_source == SQL_DB2)
		{
			AppendCopy(open, string2, end_str2);
			Append(open, ", ", L", ", 2);

			Token::Remove(comma1, end_str2);
		}
	}
	else
	// Convert to CHARINDEX in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CHARINDEX", L"CHARINDEX", 9);

		// In SQL Server change the order of search and source parameters for Sybase ASA LOCATE
		if(_source == SQL_SYBASE_ASA)
		{
			AppendCopy(open, string2, end_str2);
			Append(open, ", ", L", ", 2);

			Token::Remove(comma1, end_str2);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DB2 LOCATE_IN_STRING
bool SqlParser::ParseFunctionLocateInString(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *source = GetNextToken();

	if(source == NULL)
		return false;

	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *search = GetNextToken();

	if(search == NULL)
		return false;

	ParseExpression(search);

	Token *comma2 = GetNextCharToken(',', L',');
	
	if(comma2 == NULL)
		return false;

	Token *start = GetNextToken();

	if(start == NULL)
		return false;

	ParseExpression(start);

	Token *comma3 = GetNextCharToken(',', L',');
	
	if(comma3 == NULL)
		return false;

	Token *instance = GetNextToken();

	if(instance == NULL)
		return false;

	ParseExpression(instance);

	Token *comma4 = GetNextCharToken(',', L',');

	// Character set is optional
	if(comma4 != NULL)
		/*Token *unit */ (void) GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to INSTR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "INSTR", L"INSTR", 5);
	}

	FUNC_STATS(name);
	return true;
}

// LOG in Oracle, SQL Server, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionLog(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_LOG_DESC)

	Token *num1 = GetNextToken();

	if(num1 == NULL)
		return false;

	// Parse num1
	ParseExpression(num1);

	Token *comma = GetNextCharToken(',', L',');
	Token *num2 = NULL;

	// Num2 is optional in Oracle, MySQL
	if(comma != NULL)
	{
		num2 = GetNextToken();

		if(num2 == NULL)
			return false;

		// Parse num2
		ParseExpression(num2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	STATS_SET_CONV_NO_NEED(true) 
	FUNC_STATS(name);

	return true;
}

// LOG10 in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionLog10(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *numeric = GetNextToken();

	if(numeric == NULL)
		return false;

	// Parse numeric
	ParseExpression(numeric);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LOG in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "LOG", L"LOG", 3);
		Append(open, "10, ", L"10, ", 4);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 LONG_VARCHAR
bool SqlParser::ParseFunctionLongVarchar(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LOG in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CLOB", L"TO_CLOB", 7);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 LONG_VARGRAPHIC
bool SqlParser::ParseFunctionLongVargraphic(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LOG in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CLOB", L"TO_CLOB", 7);
	}

	FUNC_STATS(name);
	return true;
}

// LOWER in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionLower(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_LOWER_DESC)

	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	STATS_SET_CONV_NO_NEED(true) 
	FUNC_STATS(name);

	return true;
}

// LPAD in Oracle, DB2, MySQL
bool SqlParser::ParseFunctionLpad(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_LPAD_DESC)

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *len = GetNextToken();

	// Parse len
	ParseExpression(len);

	Token *end_len = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	Token *pad = NULL;
	
	// Pad is optional in Oracle, DB2
	if(comma2 != NULL)
	{
		pad = GetNextToken();

		// Parse pad
		ParseExpression(pad);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Add third parameter in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_MYSQL)
	{
		if(pad == NULL)
			Append(end_len, ", ' '", L", ' '", 5);
	}

	FUNC_STATS(name);
	return true;
}

// LTRIM in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionLtrim(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_LTRIM_DESC)

	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	Token *set = NULL;
	Token *end_set = NULL;
	
	// Set is optional in Oracle, DB2
	if(comma != NULL)
	{
		set = GetNextToken();

		// Parse set
		ParseExpression(set);

		end_set = GetLastToken();
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TRIM in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_MYSQL)
	{
		if(set != NULL)
		{
			Token::Change(name, "TRIM", L"TRIM", 4);
			Append(open, "LEADING ", L"LEADING ", 8, name);
			AppendCopy(open, set, end_set);
			Append(open, " FROM ", L"FROM ", 6, name);

			Token::Remove(comma, end_set);
		}
	}

	name->data_type = TOKEN_DT_STRING;

	FUNC_STATS(name);
	return true;
}

// DB2 MAX
bool SqlParser::ParseFunctionMax(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_MAX_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	int num = 1;

	// DB2 allows variable number of parameters (synonym of GREATEST)
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);

		num++;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// If num > 1 it is not aggregate function
	if(num > 1)
	{
		// Convert to GREATEST in Oracle
		if(_target == SQL_ORACLE)
			Token::Change(name, "GREATEST", L"GREATEST", 8);
	}
	else
		STATS_SET_CONV_NO_NEED(true)

	FUNC_STATS(name);
	return true;
}

// Informix MDY function (build DATE from 3 integers: months, day and year)
bool SqlParser::ParseFunctionMdy(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Month expression
	Token *month = GetNextToken();

	if(month == NULL)
		return false;

	ParseExpression(month);

	Token *month_end = GetLastToken();

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	// Day expression
	Token *day = GetNextToken();

	if(day == NULL)
		return false;

	ParseExpression(day);

	Token *day_end = GetLastToken();

	Token *comma2 = GetNextCharToken(',', L',');

	if(comma2 == NULL)
		return false;

	// Year expression
	Token *year = GetNextToken();

	if(year == NULL)
		return false;

	ParseExpression(year);

	Token *year_end = GetLastToken();

	Token *close = GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		// All parts are specified as integer constants, use DATE '2013-09-27' syntax
		if(month->IsNumeric() == true && day->IsNumeric() == true && year->IsNumeric() == true)
		{
			Token::Change(name, "DATE", L"DATE", 4);

			TokenStr date(" '", L" '", 2);
			date.Append(year);
			date.Append("-", L"-", 1);
			date.Append(month);
			date.Append("-", L"-", 1);
			date.Append(day);
			date.Append("'", L"'", 1);

			AppendNoFormat(name, &date);

			Token::Remove(open, close);
		}
		// TO_DATE(TO_CHAR(month, 'FM09') || TO_CHAR(day, 'FM09') || TO_CHAR(year, 'FM0009'), 'MMDDYYYY') in Oracle
		else
		{
			Token::Change(name, "TO_DATE", L"TO_DATE", 7);
			Token::Change(comma, " ", L" ", 1);
			Token::Change(comma2, " ", L" ", 1);

			Prepend(month, "TO_CHAR(", L"TO_CHAR(", 8, name);
			AppendNoFormat(month_end, ", 'FM09') ||", L", 'FM09') ||", 12);

			Prepend(day, "TO_CHAR(", L"TO_CHAR(", 8, name);
			AppendNoFormat(day_end, ", 'FM09') ||", L", 'FM09') ||", 12);

			Prepend(year, "TO_CHAR(", L"TO_CHAR(", 8, name);
			AppendNoFormat(year_end, ", 'FM0009')", L", 'FM0009')", 11);

			PrependNoFormat(close, ", 'MMDDYYYY'", L", 'MMDDYYYY'", 12);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DB2 MICROSECOND
bool SqlParser::ParseFunctionMicrosecond(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER(TO_CHAR()) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);
		Append(exp, ", 'FF6')", L", 'FF6')", 8);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 MIDNIGHT_SECONDS
bool SqlParser::ParseFunctionMidnightSeconds(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER(TO_CHAR()) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);
		Append(exp, ", 'SSSSS')", L", 'SSSSS')", 10);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 MIN
bool SqlParser::ParseFunctionMin(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_MIN_DESC)
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	int num = 1;

	// DB2 allows variable number of parameters (synonym of LEAST)
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);

		num++;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// If num > 1 it is not aggregate function
	if(num > 1)
	{
		// Convert to LEAST in Oracle
		if(_target == SQL_ORACLE)
			Token::Change(name, "LEAST", L"LEAST", 5);
	}
	else
		STATS_SET_CONV_NO_NEED(true)

	FUNC_STATS(name);
	return true;
}

// MINUTE in DB2, Sybase ASA
bool SqlParser::ParseFunctionMinute(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *datetime = GetNextToken();

	if(datetime == NULL)
		return false;

	// Parse datetime
	ParseExpression(datetime);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to EXTRACT in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "EXTRACT(MINUTE FROM ", L"EXTRACT(MINUTE FROM ", 20);
		Token::Remove(open);

		// If datetime parameter is string literal enclose with TO_TIMESTAMP
		if(datetime->type == TOKEN_STRING)
		{
			Prepend(datetime, "TO_TIMESTAMP(", L"TO_TIMESTAMP(", 13, name);
			Append(datetime, ")", L")", 1);
		}
	}
	else
	// Convert to DATEPART in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "DATEPART", L"DATEPART", 8);
		Append(open, "MI, ", L"MI, ", 4, name);
	}

	FUNC_STATS(name);
	return true;
}

// MOD in Oracle, DB2, Informix, MySQL, Sybase ASA, Sybase ADS
bool SqlParser::ParseFunctionMod(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_MOD_DESC)

	Token *dividend = GetNextToken();

	if(dividend == NULL)
		return false;

	// Parse dividend
	ParseExpression(dividend);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *divisor = GetNextToken();

	if(divisor == NULL)
		return false;

	// Parse divisor
	ParseExpression(divisor);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_SQL_SERVER)
	{
		Token::Remove(name);
		Token::Change(comma, " %", L" %", 2);
	}
	else
		STATS_SET_CONV_NO_NEED(true)

	FUNC_STATS(name);
	return true;
}

// MONTH in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionMonth(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	// Parse date
	ParseExpression(date);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to EXTRACT in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "EXTRACT(MONTH FROM ", L"EXTRACT(MONTH FROM ", 19);
		Token::Remove(open);

		// If date parameter is string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Prepend(date, "TO_DATE(", L"TO_DATE(", 8, name);
			Append(date, ")", L")", 1);
		}
	}

	FUNC_STATS(name);
	return true;
}

// MONTHNAME in DB2, Sybase ASA
bool SqlParser::ParseFunctionMonthname(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	Token *end_date = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	Token *locale = NULL;
	
	// Locale is optional in DB2
	if(comma != NULL)
		locale = GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);

		// If date parameter is string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Prepend(date, "TO_DATE(", L"TO_DATE(", 8, name);
			Append(date, ")", L")", 1);
		}

		Append(end_date, ", 'Month'", L", 'Month'", 9);

		if(locale != NULL)
		{
			if(Token::Compare(locale, "'CLDR181_en_US'", L"'CLDR181_en_US'", 15))
				Token::Change(locale, "'NLS_DATE_LANGUAGE = AMERICAN'", L"'NLS_DATE_LANGUAGE = AMERICAN'", 30);
			else
			if(Token::Compare(locale, "'CLDR181_de_DE'", L"'CLDR181_de_DE'", 15))
				Token::Change(locale, "'NLS_DATE_LANGUAGE = GERMAN'", L"'NLS_DATE_LANGUAGE = GERMAN'", 28);
			else
			if(Token::Compare(locale, "'CLDR181_fr_FR'", L"'CLDR181_fr_FR'", 15))
				Token::Change(locale, "'NLS_DATE_LANGUAGE = FRENCH'", L"'NLS_DATE_LANGUAGE = FRENCH'", 28);
		}
	}
	else
	// Convert to DATENAME in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "DATENAME", L"DATENAME", 8);
		Append(open, "MONTH, ", L"MONTH, ", 7, name);
	}

	FUNC_STATS(name);
	return true;
}

// MONTHS_BETWEEN in Oracle, DB2
bool SqlParser::ParseFunctionMonthsBetween(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_MONTHS_BETWEEN_DESC)

	Token *date1 = GetNextToken();

	if(date1 == NULL)
		return false;

	// Parse first date
	ParseExpression(date1);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *date2 = GetNextToken();

	if(date2 == NULL)
		return false;

	// Parse second date
	ParseExpression(date2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// dbo.MONTHS_BETWEEN UDF in SQL Server
	if(_target == SQL_SQL_SERVER)
		PrependNoFormat(name, "dbo.", L"dbo.", 4);
		
	FUNC_STATS(name);
	return true;	
}

// DB2 MULTIPLY_ALT
bool SqlParser::ParseFunctionMultiplyAlt(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// First expression
	Token *exp1 = GetNextToken();

	if(exp1 == NULL)
		return false;

	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');

	// Second expression
	Token *exp2 = GetNextToken();

	if(exp2 == NULL)
		return false;

	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to prduct in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Remove(name);
		Token::Change(comma, " * ", L" * ", 3);
	}
		
	FUNC_STATS(name);
	return true;	
}

// NCHAR in SQL Server, DB2, Sybase ASA
bool SqlParser::ParseFunctionNchar(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp1 = GetNextToken();

	if(exp1 == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional in DB2
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		// Convert DB2 NCHAR to TO_NCHAR in Oracle if format is not specified
		if(_source == SQL_DB2)
		{
			if(exp2 == NULL)
				Token::Change(name, "TO_NCHAR", L"TO_NCHAR", 8);
		}
		else
		// Convert Sybase ASA NCHAR to NCHR in Oracle
		if(_source == SQL_SYBASE_ASA)
			Token::Change(name, "NCHR", L"NCHR", 4);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 NCLOB
bool SqlParser::ParseFunctionNclob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CLOB in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NCLOB", L"TO_NCLOB", 8);

		// Second parameter is removed
		Token::Remove(comma);
		Token::Remove(exp2);
	}

	FUNC_STATS(name);
	return true;
}

// NEWID in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionNewid(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Flag is optional in Sybase ASE
	Token *flag = GetNextToken();

	if(Token::Compare(flag, ')', L')') == true)
	{
		PushBack(flag);
		flag = NULL;
	}
	else
		// Parse flag
		ParseExpression(flag);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// If target type is not Sybase ASE remove optional flag
	if(flag != NULL && _target != SQL_SYBASE)
		Token::Remove(flag);

	// Convert to SYS_GUID in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "SYS_GUID", L"SYS_GUID", 8);
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "UUID", L"UUID", 4);

	FUNC_STATS(name);
	return true;
}

// NEWIDSTRING in Sybase ADS
bool SqlParser::ParseFunctionNewidstring(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_SQL_SERVER)
		TOKEN_CHANGE(name, "NEWID");
	else
	if(_target == SQL_ORACLE)
		TOKEN_CHANGE(name, "SYS_GUID");
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
		TOKEN_CHANGE(name, "UUID");

	FUNC_STATS(name);
	return true;
}

// NEXT_DAY in Oracle, DB2
bool SqlParser::ParseFunctionNextDay(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_NEXT_DAY_DESC)

	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *weekday = GetNextToken();

	if(weekday == NULL)
		return false;

	// Parse weekday
	ParseExpression(weekday);

	/*Token *close */ (void) GetNextCharToken(')', L')');
		
	FUNC_STATS(name);
	return true;	
}

// NEXT_IDENTITY in Sybase ASE
bool SqlParser::ParseFunctionNextIdentity(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to IDENT_CURRENT + IDENT_INCR in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "IDENT_CURRENT", L"IDENT_CURRENT", 13);
		Append(close, " + IDENT_INCR(", L" + IDENT_INCR(", 14);
		AppendCopy(close, exp);
		Append(close, ")", L")", 1);
	}
		
	FUNC_STATS(name);
	return true;	
}

// NOW in MySQL, Sybase ASA
bool SqlParser::ParseFunctionNow(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Asterisk is optional in Sybase ASA
	Token *asterisk = GetNextCharToken('*', L'*');
	Token *close = GetNextCharToken(')', L')');

	// If target type is not Sybase ASA remove asterisk
	if(asterisk != NULL && _target != SQL_SYBASE_ASA)
		Token::Remove(asterisk);

	// Convert to SYSTIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);
		Token::Remove(open, close);
	}
	else
	// Convert to GETDATE in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "GETDATE", L"GETDATE", 7);
	
	FUNC_STATS(name);
	return true;	
}

// NULLIF in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionNullif(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// NUMBER in Sybase ASA
bool SqlParser::ParseFunctionNumber(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Asterisk is optional
	Token *asterisk = GetNextCharToken('*', L'*');	
	Token *close = GetNextCharToken(')', L')');

	// Convert to ROWNUM in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "ROWNUM", L"ROWNUM", 6);
		Token::Remove(open, close);
	}
	else
	// Convert to ROW_NUMBER in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "ROW_NUMBER", L"ROW_NUMBER", 10);
		
		if(asterisk != NULL)
			Token::Remove(asterisk);

		Append(close, " OVER(ORDER BY (SELECT 1))", L" OVER(ORDER BY (SELECT 1))", 26, name);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 NVARCHAR
bool SqlParser::ParseFunctionNvarchar(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		// Convert to TO_NCHAR if format is not specified
		if(exp2 == NULL)
			Token::Change(name, "TO_NCHAR", L"TO_NCHAR", 8);
	}

	FUNC_STATS(name);
	return true;
}

// NVL in Oracle, DB2
bool SqlParser::ParseFunctionNvl(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_NVL_DESC)

	// First expression
	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	int num = 2;

	// DB2 allows more than 2 parameters for NVL
	while(true)
	{
		Token *comman = GetNextCharToken(',', L',');

		if(comman == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);

		num++;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// 2 parameters in Oracle
	if(num == 2)
	{
		// Convert to ISNULL in SQL Server
		if(_target == SQL_SQL_SERVER)
			Token::Change(name, "ISNULL", L"ISNULL", 6);
		else
		// Convert to IFNULL in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
			Token::Change(name, "IFNULL", L"IFNULL", 6);
		else
		// Convert to COALESCE in PostgreSQL
		if(_target == SQL_POSTGRESQL)
			Token::Change(name, "COALESCE", L"COALESCE", 8);
	}
	// 3 and more parameters in DB2
	else
	{
		// Convert to COALESCE in Oracle
		if(_target == SQL_ORACLE)
			Token::Change(name, "COALESCE", L"COALESCE", 8);
	}

	FUNC_STATS(name);
	return true;	
}

// NVL2 in Oracle, DB2
bool SqlParser::ParseFunctionNvl2(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_NVL_DESC)

	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	/*Token *end_exp1 */ (void) GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	Token *comma2 = GetNextCharToken(',', L',');
	
	if(comma2 == NULL)
		return false;

	Token *exp3 = GetNextToken();

	// Parse third expression
	ParseExpression(exp3);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// OBJECT_ID in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionObjectId(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *object_name = GetNextToken();

	// Parse object_name
	ParseExpression(object_name);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// OBJECT_NAME in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionObjectName(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *object_id = GetNextToken();

	// Parse object_id
	ParseExpression(object_id);

	Token *comma = GetNextCharToken(',', L',');
	Token *database_id = NULL;

	// Database_id is optional in SQL Server, Sybase ASE
	if(comma != NULL)
	{
		database_id = GetNextToken();

		// Parse database_id_id
		ParseExpression(database_id);
	}

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// OBJECT_OWNER_ID in Sybase ASE
bool SqlParser::ParseFunctionObjectOwnerId(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *object_id = GetNextToken();

	// Parse object_id
	ParseExpression(object_id);

	Token *comma = GetNextCharToken(',', L',');
	Token *database_id = NULL;

	// Database_id is optional in Sybase ASE
	if(comma != NULL)
	{
		database_id = GetNextToken();

		// Parse database_id_id
		ParseExpression(database_id);
	}

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to OBJECTPROPERTY in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(database_id == NULL)
		{
			Token::Change(name, "OBJECTPROPERTY", L"OBJECTPROPERTY", 14);
			Append(object_id, ", 'OWNERID'", L", 'OWNERID'", 11);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DB2 OCTET_LENGTH
bool SqlParser::ParseFunctionOctetLength(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to LENGTHB in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "LENGTHB", L"LENGTHB", 7);

	FUNC_STATS(name);
	return true;
}

// DB2 OVERLAY
bool SqlParser::ParseFunctionOverlay(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *source = GetNextToken();

	if(source == NULL)
		return false;

	// Parse source expression
	ParseExpression(source);

	Token *del = GetNextToken();
	
	if(del == NULL)
		return false;

	Token *insert = GetNextToken();

	if(insert == NULL)
		return false;

	// Parse insert expression
	ParseExpression(insert);

	Token *del2 = GetNextToken();
	
	if(del2 == NULL)
		return false;

	Token *start = GetNextToken();

	if(start == NULL)
		return false;

	// Parse start expression
	ParseExpression(start);

	Token *del3 = GetNextToken();

	if(del3 == NULL)
		return false;

	// If FOR syntax is used it is optional and while skipped still can be followed by not optional USING
	if(Token::Compare(del3, "FOR", L"FOR", 3) || Token::Compare(del3, ",", L",", 1))
	{
		Token *length = GetNextToken();

		if(length == NULL)
			return false;

		ParseExpression(length);
	}
	else
		PushBack(del3);
		
	Token *del4 = GetNextToken();

	if(del4 == NULL)
		return false;

	Token *unit = GetNextToken();

	if(unit == NULL)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// User-defined function in Oracle
	if(_target == SQL_ORACLE)
	{
		if(Token::Compare(del, "PLACING", L"PLACING", 7))
			Token::Change(del, ",", L",", 1);

		if(Token::Compare(del2, "FROM", L"FROM", 4))
			Token::Change(del2, ",", L",", 1);

		if(Token::Compare(del3, "FOR", L"FOR", 3))
			Token::Change(del3, ",", L",", 1);

		if(Token::Compare(del4, "USING", L"USING", 5))
			Token::Change(del4, ",", L",", 1);
	}

	FUNC_STATS(name);
	return true;
}

// PARTITION_ID in Sybase ASE
bool SqlParser::ParseFunctionPartitionId(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *table_name = GetNextToken();

	// Parse table_name
	ParseExpression(table_name);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *partition_name = GetNextToken();

	// Parse partition_name
	ParseExpression(partition_name);

	Token *comma2 = GetNextCharToken(',', L',');
	Token *index_name = NULL;

	// Index_name is optional in Sybase ASE
	if(comma2 != NULL)
	{
		index_name = GetNextToken();

		// Parse index_name
		ParseExpression(index_name);
	}

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// PARTITION_NAME in Sybase ASE
bool SqlParser::ParseFunctionPartitionName(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *index_id = GetNextToken();

	// Parse index_id
	ParseExpression(index_id);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *partition_id = GetNextToken();

	// Parse partition_id
	ParseExpression(partition_id);

	Token *comma2 = GetNextCharToken(',', L',');
	Token *database_id = NULL;

	// Database_id is optional in Sybase ASE
	if(comma2 != NULL)
	{
		database_id = GetNextToken();

		// Parse database_id
		ParseExpression(database_id);
	}

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// PARTITION_OBJECT_ID in Sybase ASE
bool SqlParser::ParseFunctionPartitionObjectId(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *partition_id = GetNextToken();

	// Parse partition_id
	ParseExpression(partition_id);

	Token *comma = GetNextCharToken(',', L',');
	Token *database_id = NULL;

	// Database_id is optional in Sybase ASE
	if(comma != NULL)
	{
		database_id = GetNextToken();

		// Parse database_id_id
		ParseExpression(database_id);
	}

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// PASSWORD_RANDOM in Sybase ASE
bool SqlParser::ParseFunctionPasswordRandom(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Len is optional in Sybase ASE
	Token *len = GetNextToken();

	if(Token::Compare(len, ')', L')') == true)
	{
		PushBack(len);
		len = NULL;
	}
	else
		// Parse len
		ParseExpression(len);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;
	
	// Convert to DBMS_RANDOM.STRING in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "DBMS_RANDOM.STRING", L"DBMS_RANDOM.STRING", 18);
		Append(open, "'P', ", L"'P', ", 5);

		// Len is 6 by default in Sybase ASE
		if(len == NULL)
			Prepend(close, "6", L"6", 1);
	}

	FUNC_STATS(name);
	return true;
}

// PATINDEX in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionPatindex(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *pattern = GetNextToken();

	// Parse pattern
	ParseExpression(pattern);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *end_str = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	Token *usng = NULL;
	Token *semantics = NULL;

	// USING BYTES, USING CHARACTERS or USING CHARS is optional in Sybase ASE
	if(comma2 != NULL)
	{
		usng = GetNextWordToken("USING", L"USING", 5);
		semantics = GetNextToken();

		// If target is not Sybase ASE remove semantics attribute
		if(_target != SQL_SYBASE)
			Token::Remove(comma2, semantics);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to INSTR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "REGEXP_INSTR", L"REGEXP_INSTR", 12);
		AppendCopy(open, string, end_str);
		Append(open, ", ", L", ", 2);

		Token::Remove(comma, end_str);
	}

	FUNC_STATS(name);
	return true;
}

// PI in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionPi(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Asterisk is optional
	Token *asterisk = GetNextCharToken('*', L'*');
	Token *close = GetNextCharToken(')', L')');

	// If target type is not Sybase ASA remove asterisk
	if(asterisk != NULL && _target != SQL_SYBASE_ASA)
		Token::Remove(asterisk);

	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "3.1415926535897931", L"3.1415926535897931", 18);
		Token::Remove(open, close);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 POSITION
bool SqlParser::ParseFunctionPosition(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *search = GetNextToken();

	if(search == NULL)
		return false;

	// Parse search expression
	ParseExpression(search);

	Token *del = GetNextToken();
	
	if(del == NULL)
		return false;

	Token *source = GetNextToken();

	if(source == NULL)
		return false;

	// Parse source expression
	ParseExpression(source);
		
	Token *del2 = GetNextToken();

	if(del2 == NULL)
		return false;

	Token *unit = GetNextToken();

	if(unit == NULL)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to INSTR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "INSTR", L"INSTR", 5);
		AppendCopy(open, source);
		Append(open, ", ", L", ", 2);

		Token::Remove(del, source);
		Token::Remove(del2, unit);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 POSSTR
bool SqlParser::ParseFunctionPosstr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *source = GetNextToken();

	if(source == NULL)
		return false;

	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *search = GetNextToken();

	if(search == NULL)
		return false;

	ParseExpression(search);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to INSTR in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "INSTR", L"INSTR", 5);

	FUNC_STATS(name);
	return true;
}

// POWER in Oracle, SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionPower(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_POWER_DESC)

	Token *value = GetNextToken();

	// Parse value
	ParseExpression(value);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *power = GetNextToken();

	// Parse power
	ParseExpression(power);

	/*Token *close */ (void) GetNextCharToken(')', L')');
		
	FUNC_STATS(name);
	return true;	
}

// QUARTER in DB2, Sybase ASA
bool SqlParser::ParseFunctionQuarter(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	Token *close = GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);

		// If date parameter is string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Prepend(date, "TO_DATE(", L"TO_DATE(", 8, name);
			Append(date, ")", L")", 1);
		}

		Prepend(close, ", 'Q')", L", 'Q')", 6, name);
	}
	else
	// Convert to DATEPART in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "DATEPART", L"DATEPART", 8);
		Append(open, "Q, ", L"Q, ", 3, name);
	}

	FUNC_STATS(name);
	return true;
}

// RADIANS in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionRadians(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *numeric = GetNextToken();

	// Parse numeric
	ParseExpression(numeric);

	Token *close = GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		Token::Remove(name);
		Append(close, " * 3.1415926535/180", L" * 3.1415926535/180", 19);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 RAISE_ERROR
bool SqlParser::ParseFunctionRaiseError(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *sqlstate = GetNextToken();

	if(sqlstate == NULL)
		return false;

	ParseExpression(sqlstate);

	/*Token *comma */ (void) GetNextCharToken(',', L',');

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	if(_target == SQL_ORACLE)
		Token::Change(name, "RAISE_APPLICATION_ERROR", L"RAISE_APPLICATION_ERROR", 23);

	FUNC_STATS(name);
	return true;	
}

// SQL Server RAISERROR
bool SqlParser::ParseFunctionRaiserror(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *text = GetNextToken();

	if(text == NULL)
		return false;

	ParseExpression(text);

	Token *comma = GetNextCharToken(',', L',');

	Token *severity = GetNextToken();

	if(severity == NULL)
		return false;

	ParseExpression(severity);

	/*Token *comma2 */ (void) GetNextCharToken(',', L',');

	Token *state = GetNextToken();

	if(state == NULL)
		return false;

	ParseExpression(state);

	Token *close = GetNextCharToken(')', L')');
	
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "SIGNAL", L"SIGNAL", 6);
		Token::Change(open, " SQLSTATE '02000' SET MESSAGE_TEXT = ", L" SQLSTATE '02000' SET MESSAGE_TEXT = ", 37);
		Token::Remove(comma, close);
	}

	FUNC_STATS(name);
	return true;	
}

// RANK analytic function in DB2, Oracle
bool SqlParser::ParseFunctionRank(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_RANK_DESC)

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

	if(over != NULL)
		ParseAnalyticFunctionOverClause(over);

	FUNC_STATS(name);
	return true;
}

// ROW_NUMBER analytic function in DB2, Oracle, MariaDB
bool SqlParser::ParseFunctionRowNumber(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_ROW_NUMBER_DESC)

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

	if(over != NULL)
		ParseAnalyticFunctionOverClause(over);

	STATS_SET_CONV_NO_NEED(Target(SQL_MARIADB, SQL_MARIADB_ORA))  
	FUNC_STATS(name);

	return true;
}

// RAND in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionRand(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Integer is optional
	Token *integer = GetNextToken();

	if(Token::Compare(integer, ')', L')') == true)
	{
		PushBack(integer);
		integer = NULL;
	}
	else
		// Parse integer
		ParseExpression(integer);

	Token *close = GetNextCharToken(')', L')');
	
	// Convert to DBMS_RANDOM.VALUE in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "DBMS_RANDOM.VALUE", L"DBMS_RANDOM.VALUE", 17);
		Token::Remove(open, close);
	}

	FUNC_STATS(name);
	return true;
}

// RAND2 in Sybase ASE
bool SqlParser::ParseFunctionRand2(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Expression is optional in Sybase ASE
	Token *exp = GetNextToken();

	if(Token::Compare(exp, ')', L')') == true)
	{
		PushBack(exp);
		exp = NULL;
	}
	else
		// Parse expression
		ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;
	
	// Convert to DBMS_RANDOM.VALUE in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "DBMS_RANDOM.VALUE", L"DBMS_RANDOM.VALUE", 17);
		Token::Remove(open, close);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 REAL
bool SqlParser::ParseFunctionReal(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_NUMBER", L"TO_NUMBER", 9);

	FUNC_STATS(name);
	return true;
}

// REGEXP_LIKE in Oracle, Vertica, Snowflake
bool SqlParser::ParseFunctionRegexpLike(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_REGEXP_LIKE_DESC)

	Token *string = GetNextToken();

	// Parse input string
	ParseExpression(string);

	Token *comma1 = TOKEN_GETNEXT(',');

	if(comma1 == NULL)
		return false;

	// Regexp pattern
	Token *pattern = GetNextToken();
	ParseExpression(pattern);

	Token *comma2 = TOKEN_GETNEXT(',');

	// match parameter (3rd operand) is optional
	if(comma2 != NULL)
	{
		Token *match = GetNextToken();

		// Parse the match parameter
		ParseExpression(match);
	}

	/*Token *close */ (void) TOKEN_GETNEXT(')');

	FUNC_STATS(name);
	return true;
}

// REGEXP_SUBSTR in Oracle, Sybase ASA
bool SqlParser::ParseFunctionRegexpSubstr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_REGEXP_SUBSTR_DESC)

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma1 = GetNextCharToken(',', L',');

	if(comma1 == NULL)
		return false;

	Token *pattern = GetNextToken();

	// Parse pattern
	ParseExpression(pattern);

	Token *comma2 = GetNextCharToken(',', L',');
	Token *start = NULL;
	Token *comma3 = NULL;
	Token *occurrence = NULL;
	Token *comma4 = NULL;
	Token *escape = NULL;

	// Start is optional
	if(comma2 != NULL)
	{
		start = GetNextToken();

		// Parse start
		ParseExpression(start);

		comma3 = GetNextCharToken(',', L',');

		// If start is specified, optional occurrence can be specified
		if(comma3 != NULL)
		{
			occurrence = GetNextToken();

			// Parse occurence
			ParseExpression(occurrence);

			comma4 = GetNextCharToken(',', L',');

			// If occurrence is specified, optional escape can be specified
			if(comma4 != NULL)
			{
				escape = GetNextToken();

				// Parse escape
				ParseExpression(escape);
			}
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// REMAINDER in Oracle, Sybase ASA
bool SqlParser::ParseFunctionRemainder(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *dividend = GetNextToken();

	// Parse dividend
	ParseExpression(dividend);

	/*Token *end_dividend */ (void) GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *divisor = GetNextToken();

	// Parse divisor
	ParseExpression(divisor);

	/*Token *end_divisor */ (void) GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to MOD in Oracle
	if(_target == SQL_ORACLE && _source != SQL_ORACLE)
		Token::Change(name, "MOD", L"MOD", 3);
	else
	if(_target == SQL_SQL_SERVER)
	{
		Token::Remove(name);
		Token::Change(comma, " %", L" %", 2);
	}

	FUNC_STATS(name);
	return true;
}

// REPEAT in DB2, Sybase ASA
bool SqlParser::ParseFunctionRepeat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *end_str = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *num = GetNextToken();

	// Parse num
	ParseExpression(num);

	Token *end_num = GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// Convert to RPAD in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "RPAD", L"RPAD", 4);
		Append(comma, " LENGTH(", L" LENGTH(", 8, name);
		AppendCopy(comma, string, end_str);
		Append(comma, ") *", L") *", 3);
		Append(end_num, ", ", L", ", 2);
		AppendCopy(end_num, string, end_str);
	}
	else
	// Convert to REPLICATE in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "REPLICATE", L"REPLICATE", 9);

	FUNC_STATS(name);
	return true;
}

// REPLACE in Oracle, SQL Server, DB2, MySQL, Sybase ASA
bool SqlParser::ParseFunctionReplace(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_REPLACE_DESC)
	
	Token *source = GetNextToken();

	// Parse source
	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *search = GetNextToken();

	// Parse search
	ParseExpression(search);

	Token *end_search = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	Token *replace = NULL;

	// Replace is optional in Oracle, DB2
	if(comma2 != NULL)
	{
		replace = GetNextToken();

		// Parse replace
		ParseExpression(replace);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Add third parameter in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_MYSQL)
	{
		if(replace == NULL)
			Append(end_search, ", ''", L", ''", 4);
	}

	FUNC_STATS(name);
	return true;
}

// REPLICATE in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionReplicate(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *end_str = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *num = GetNextToken();

	// Parse num
	ParseExpression(num);

	Token *end_num = GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// Convert to RPAD in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "RPAD", L"RPAD", 4);
		Append(comma, " LENGTH(", L" LENGTH(", 8, name);
		AppendCopy(comma, string, end_str);
		Append(comma, ") *", L") *", 3);
		Append(end_num, ", ", L", ", 2);
		AppendCopy(end_num, string, end_str);
	}
	else
	if(Target(SQL_MYSQL, SQL_MARIADB))
		TOKEN_CHANGE(name, "REPEAT");

	FUNC_STATS(name);
	return true;
}

// RESERVE_IDENTITY in Sybase ASE
bool SqlParser::ParseFunctionReserveIdentity(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *table = GetNextToken();

	// Parse table
	ParseExpression(table);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *value = GetNextToken();

	// Parse value
	ParseExpression(value);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// REVERSE in Oracle, SQL Server, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionReverse(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// RIGHT in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionRight(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *end_str = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *length = GetNextToken();

	// Parse length
	ParseExpression(length);

	Token *comma2 = GetNextCharToken(',', L',');

	// Character set is optional in DB2
	if(comma2 != NULL)
		/*Token *unit */ (void) GetNextToken();

	Token *close = GetNextCharToken(')', L')');

	// Convert to SUBSTR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SUBSTR", L"SUBSTR", 6);
		Append(comma, " GREATEST(-LENGTH(", L" GREATEST(-LENGTH(", 18, name);
		AppendCopy(comma, string, end_str);
		Append(comma, "),", L"),", 2);
		Prepend(length, "-", L"-", 1);
		Append(close, ")", L")", 1);
	}

	FUNC_STATS(name);
	return true;
}

// ROUND in Oracle, SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionRound(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_ROUND_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional in Oracle, DB2, MySQL
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 ROUND_TIMESTAMP
bool SqlParser::ParseFunctionRoundTimestamp(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Format expression is optional
	if(comma != NULL)
	{
		Token *format = GetNextToken();

		if(format == NULL)
			return false;

		ParseExpression(format);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to ROUND in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "ROUND", L"ROUND", 5);

	FUNC_STATS(name);
	return true;
}

// RPAD in Oracle, DB2, MySQL
bool SqlParser::ParseFunctionRpad(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_RPAD_DESC)

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *len = GetNextToken();

	// Parse len
	ParseExpression(len);

	Token *end_len = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	Token *pad = NULL;
	
	// Pad is optional in Oracle, DB2
	if(comma2 != NULL)
	{
		pad = GetNextToken();

		// Parse pad
		ParseExpression(pad);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Add third parameter in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_MYSQL)
	{
		if(pad == NULL)
			Append(end_len, ", ' '", L", ' '", 5);
	}

	FUNC_STATS(name);
	return true;
}

// RTRIM in Oracle, SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionRtrim(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_RTRIM_DESC)

	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	Token *set = NULL;
	Token *end_set = NULL;
	
	// Set is optional in Oracle, DB2
	if(comma != NULL)
	{
		set = GetNextToken();

		// Parse set
		ParseExpression(set);

		end_set = GetLastToken();
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TRIM in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL) && _source != SQL_MYSQL)
	{
		if(set != NULL)
		{
			Token::Change(name, "TRIM", L"TRIM", 4);
			Append(open, "TRAILING ", L"TRAILING ", 9, name);
			AppendCopy(open, set, end_set);
			Append(open, " FROM ", L"FROM ", 6, name);

			Token::Remove(comma, end_set);
		}
	}

	name->data_type = TOKEN_DT_STRING;

	FUNC_STATS(name);
	return true;
}

// SCOPE_IDENTITY in SQL Server
bool SqlParser::ParseFunctionScopeIdentity(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "LAST_INSERT_ID", L"LAST_INSERT_ID", 14);

	FUNC_STATS(name);
	return true;
}

// SECOND in DB2, Sybase ASA
bool SqlParser::ParseFunctionSecond(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *datetime = GetNextToken();

	if(datetime == NULL)
		return false;

	// Parse datetime
	ParseExpression(datetime);

	Token *comma = GetNextCharToken(',', L',');
	Token *integer = NULL;

	// Integer is optional in DB2
	if(comma != NULL)
	{
		integer = GetNextToken();

		// Parse integer
		ParseExpression(integer);
	}

	Token *close = GetNextCharToken(')', L')');

	// Convert to EXTRACT in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRUNC(EXTRACT(SECOND FROM ", L"TRUNC(EXTRACT(SECOND FROM ", 26);
		Token::Remove(open);
		Append(close, ")", L")", 1);

		// If datetime parameter is string literal enclose with TO_TIMESTAMP
		if(datetime->type == TOKEN_STRING)
		{
			Prepend(datetime, "TO_TIMESTAMP(", L"TO_TIMESTAMP(", 13, name);
			Append(datetime, ")", L")", 1);
		}
	}
	else
	// Convert to DATEPART in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "DATEPART", L"DATEPART", 8);
		Append(open, "SS, ", L"SS, ", 4, name);
	}

	FUNC_STATS(name);
	return true;
}

// SIGN in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionSign(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_SIGN_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// SIN in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionSin(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// SINH in Oracle, DB2
bool SqlParser::ParseFunctionSinh(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	Token *end_num = GetLastToken();
	Token *close = GetNextCharToken(')', L')');

	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Remove(name);
		Append(open, "EXP(", L"EXP(", 4, name);
		Append(end_num, ") - EXP(-(", L") - EXP(-(", 10, name);
		AppendCopy(end_num, num, end_num);
		Append(close, "))/2", L"))/2", 4);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 SMALLINT
bool SqlParser::ParseFunctionSmallint(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRUNC(TO_NUMBER", L"TRUNC(TO_NUMBER", 15);
		Append(close, ")", L")", 1);
	}
	else
	if(!Target(SQL_DB2))
	{
		TOKEN_CHANGE(name, "CAST");
		PREPEND_FMT(close, " AS SMALLINT", name);
	}

	FUNC_STATS(name);
	return true;
}

// SOUNDEX in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionSoundex(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// SPACE in SQL Server, DB2, Sybase ASE, Sybase ASA, MySQL, MariaDB
bool SqlParser::ParseFunctionSpace(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *integer = GetNextToken();

	if(integer == NULL)
		return false;

	// Parse integer
	ParseExpression(integer);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to RPAD in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "RPAD(' ', ", L"RPAD(' ', ", 10);
		Token::Remove(open);
	}

	name->data_type = TOKEN_DT_STRING;

	FUNC_STATS(name);
	return true;
}

// SPID_INSTANCE_ID in Sybase ASE
bool SqlParser::ParseFunctionSpidInstanceId(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// Oracle SQLCODE
bool SqlParser::ParseFunctionSqlcode(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_SQLCODE_DESC)

	// Netezza does not support SQLCODE
	if(_target == SQL_NETEZZA)
	{
		PrependNoFormat(name, "0", L"0", 1);
		Comment(name);
	}
	
	FUNC_STATS(name);
	return true;
}

// Oracle SQLERRM (parameter is optional)
bool SqlParser::ParseFunctionSqlerrm(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_SQLERRM_DESC)

	// Parse optional parameter
	if(open != NULL)
	{
		Token *exp = GetNextToken();
		ParseExpression(exp);

		/*Token *close */ (void) TOKEN_GETNEXT(')');
	}

	FUNC_STATS(name);
	return true;
}

// DB2, PostgreSQL SQLSTATE
bool SqlParser::ParseFunctionSqlstate(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Oracle does not support SQLSTATE (see also ParseBooleanErrorCheckPattern)
	if(_target == SQL_ORACLE)
		Token::Change(name, "SQLCODE", L"SQLCODE", 7);
	
	FUNC_STATS(name);
	return true;
}

// Oracle SQL%ROWCOUNT
bool SqlParser::ParseFunctionSqlPercent(Token *name)
{
	if(name == NULL)
		return false;

	Token *cent = GetNext('%', L'%');

	if(cent == NULL)
		return false;

	STATS_DECL

	Token *rowcount = TOKEN_GETNEXTW("ROWCOUNT");
	Token *notfound = NULL;
	Token *found = NULL;

	if(rowcount == NULL)
		notfound = TOKEN_GETNEXTW("NOTFOUND");

	if(rowcount == NULL && notfound == NULL)
		found = TOKEN_GETNEXTW("FOUND");

	// Oracle SQL%ROWCOUNT
	if(rowcount != NULL)
	{
		STATS_SET_DESC(SQL_FUNC_SQLROWCOUNT_DESC)

		if(_target == SQL_SQL_SERVER)
		{
			TOKEN_CHANGE(cent, "@@");
			Token::Remove(name);
		}
		else
		// GET DIAGNOSTICS var = ROW_COUNT in PostgreSQL
		if(_target == SQL_POSTGRESQL)
		{
			PREPEND(_spl_current_stmt, "GET DIAGNOSTICS ");
			PREPEND_NOFMT(_spl_current_stmt, "v_sqlrowcount");
			PREPEND(_spl_current_stmt, " = ROW_COUNT;\n");

			TOKEN_CHANGE(cent, "v_sqlrowcount");
			Token::Remove(name);
			Token::Remove(rowcount);

			_spl_need_rowcount_var = true;
		}
		else
		if(_target == SQL_NETEZZA)
		{
			TOKEN_CHANGE(name, "ROW_COUNT");
			Token::Remove(cent, rowcount);
		}

		FUNC_STATS_V("SQL%ROWCOUNT", name);
	}
	else
	// Oracle SQL%NOTFOUND
	if(notfound != NULL)
	{
		// NOT FOUND in PostgreSQL
		if(_target == SQL_POSTGRESQL)
		{
			TOKEN_CHANGE(notfound, "NOT FOUND");
			Token::Remove(name);
			Token::Remove(cent);
		}

		STATS_SET_DESC(SQL_FUNC_SQLNOTFOUND_DESC)
		FUNC_STATS_V("SQL%NOTFOUND", name);
	}
	else
	// Oracle SQL%FOUND
	if(found != NULL)
	{
		// FOUND in PostgreSQL
		if(_target == SQL_POSTGRESQL)
		{
			Token::Remove(name);
			Token::Remove(cent);
		}

		STATS_SET_DESC(SQL_FUNC_SQLNOTFOUND_DESC)
		FUNC_STATS_V("SQL%NOTFOUND", name);
	}
		
	return true;
}

// SQRT in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionSqrt(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_SQRT_DESC)

	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	STATS_SET_CONV_NO_NEED(true)  
	FUNC_STATS(name);

	return true;
}

// SQUARE in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionSquare(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse Expression
	ParseExpression(exp);

	Token *exp_end = GetLastToken();
	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	if(Target(SQL_ORACLE, SQL_MARIADB, SQL_MYSQL))
	{
		TOKEN_CHANGE(name, "POWER");
		APPEND_NOFMT(exp_end, ", 2");
	}

	FUNC_STATS(name);
	return true;
}

// STR in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionStr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *numeric = GetNextToken();

	if(numeric == NULL)
		return false;

	// Parse numeric
	ParseExpression(numeric);

	Token *comma1 = GetNextCharToken(',', L',');
	Token *length = NULL;
	Token *comma2 = NULL;
	Token *decimal = NULL;

	// Length is optional
	if(comma1 != NULL)
	{
		length = GetNextToken();

		// Parse length
		ParseExpression(length);

		comma2 = GetNextCharToken(',', L',');

		// Decimal is optional
		if(comma2 != NULL)
		{
			decimal = GetNextToken();

			// Parse decimal
			ParseExpression(decimal);
		}
	}

	Token *close = GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);

		if(comma1 != NULL)
		{
			Token::Remove(comma1, length);

			if(comma2 != NULL)
				Token::Remove(comma2, decimal);
		}
	}
	else
	// Convert to CONVERT in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "CONVERT", L"CONVERT", 7);
		Prepend(close, ", CHAR", L", CHAR", 6, name);
	}

	FUNC_STATS(name);
	return true;
}

// STRING in Sybase ASA
bool SqlParser::ParseFunctionString(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		// Convert to || operator in Oracle
		if(_target == SQL_ORACLE)
			Token::Change(comma, " ||", L" ||", 3);

		Token *stringn = GetNextToken();

		if(stringn == NULL)
			break;

		// Parse next string
		ParseExpression(stringn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Remove name for Oracle
	if(_target == SQL_ORACLE)
		Token::Remove(name);
	else
	// Convert to CONCAT in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "CONCAT", L"CONCAT", 6);

	FUNC_STATS(name);
	return true;
}

// DB2 STRIP
bool SqlParser::ParseFunctionStrip(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *type = NULL;
		
	// Type is optional
	if(comma != NULL)
	{
		type = GetNextToken();

		if(type == NULL)
			return false;
	}

	Token *comma2 = NULL;
	Token *character = NULL;

	// Character can be specified with type only
	if(type != NULL)
		comma2 = GetNextCharToken(',', L',');

	if(comma2 != NULL)
	{
		character = GetNextToken();

		if(character == NULL)
			return false;

		ParseExpression(character);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRIM", L"TRIM", 4);

		if(type != NULL)
		{
			if(Token::Compare(type, "B", L"B", 1))
				Append(open, "BOTH", L"BOTH", 4, type);
			else
			if(Token::Compare(type, "L", L"L", 1))
				Append(open, "LEADING", L"LEADING", 7, type);
			else
			if(Token::Compare(type, "T", L"T", 1))
				Append(open, "TRAILING", L"TRAILING", 8, type);
			else
				AppendCopy(open, type);	

			Token::Remove(type);
			Token::Remove(comma);
		}

		if(character != NULL)
		{
			Append(open, " ", L" ", 1);
			AppendCopy(open, character);
	
			Token::Remove(character);
			Token::Remove(comma2);
		}	

		if(type != NULL)
			Prepend(exp, " FROM ", L" FROM ", 6, type);
	}

	FUNC_STATS(name);
	return true;
}

// STR_REPLACE in Sybase ASE
bool SqlParser::ParseFunctionStrReplace(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *source = GetNextToken();

	// Parse source
	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *length = GetNextToken();

	// Parse length
	ParseExpression(length);

	Token *comma2 = GetNextCharToken(',', L',');

	if(comma2 == NULL)
		return false;

	Token *replace = GetNextToken();

	// Parse replace
	ParseExpression(replace);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to REPLACE in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "REPLACE", L"REPLACE", 7);

		if(Token::Compare(replace, "NULL", L"NULL", 4) == true)
			Token::Remove(comma2, replace);
	}
	else
	// Convert to REPLACE in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "REPLACE", L"REPLACE", 7);

		if(Token::Compare(replace, "NULL", L"NULL", 4) == true)
			Token::Change(replace, "' '", L"' '", 3);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
		TOKEN_CHANGE(name, "REPLACE");

	FUNC_STATS(name);
	return true;
}

// STRTOBIN in Sybase ASE
bool SqlParser::ParseFunctionStrtobin(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// STUFF in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionStuff(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *start = GetNextToken();

	// Parse start
	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');

	if(comma2 == NULL)
		return false;

	Token *length = GetNextToken();

	// Parse length
	ParseExpression(length);

	Token *comma3 = GetNextCharToken(',', L',');

	if(comma3 == NULL)
		return false;

	Token *replace = GetNextToken();

	// Parse replace
	ParseExpression(replace);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// INSERT function in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "INSERT", L"INSERT", 6);

	FUNC_STATS(name);
	return true;
}

// SUBDATE in MySQL
bool SqlParser::ParseFunctionSubdate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse first datetime expression
	ParseExpression(date);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *interval = GetNextToken();

	// Parse interval
	ParseExpression(interval);

	Token *close = GetNextCharToken(')', L')');

	// date - interval in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Remove(name);

		Token::Change(comma, " -", L" -", 2);

		Token::Remove(open);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// SUBSTR in Oracle, DB2, Sybase ASA
bool SqlParser::ParseFunctionSubstr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_SUBSTR_DESC)

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *string_end = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *pos = GetNextToken();

	// Parse pos
	ParseExpression(pos);

	Token *pos_end = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	
	// Length expression is optional
	if(comma2 != NULL)
	{
		Token *length = GetNextToken();

		// Parse length
		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to SUBSTRING in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "SUBSTRING", L"SUBSTRING", 9);

		// In SQL Server length must be specified
		if(comma2 == NULL)
		{
			APPEND_FMT(pos_end, ", LEN(", name);
			AppendCopy(pos_end, string, string_end, false);
			APPEND(pos_end, ")");
		}
	}
	else
		STATS_SET_CONV_NO_NEED(true)

	name->data_type = TOKEN_DT_STRING;

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle SUBSTR2
bool SqlParser::ParseFunctionSubstr2(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *start = GetNextToken();

	if(start == NULL)
		return false;

	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');
	
	// Length expression is optional
	if(comma2 != NULL)
	{
		Token *length = GetNextToken();

		if(length == NULL)
			return false;

		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle SUBSTRB
bool SqlParser::ParseFunctionSubstrb(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_SUBSTRB_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *start = GetNextToken();

	if(start == NULL)
		return false;

	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');
	
	// Length expression is optional
	if(comma2 != NULL)
	{
		Token *length = GetNextToken();

		if(length == NULL)
			return false;

		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// SUBSTRING in SQL Server, DB2, Sybase ASE, Sybase ASA, Informix
bool SqlParser::ParseFunctionSubstring(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNext();

	// Parse expression
	ParseExpression(exp);

	// FROM can be used instead of comma before start position in DB2; Informix requires FROM
	Token *from = GetNext("FROM", L"FROM", 4);
	Token *comma = NULL;

	if(from == NULL)
		comma = GetNext(',', L',');

	Token *start = GetNext();

	// Parse start
	ParseExpression(start);

	// Length is optional, FOR can be used instead of comma in DB2; Informix requires FOR
	Token *for_ = GetNext("FOR", L"FOR", 3);

	Token *comma2 = NULL;
	Token *length = NULL;

	// Substring length
	if(for_ != NULL)
	{
		length = GetNext();

		ParseExpression(length);
	}
	else
		comma2 = GetNext(',', L',');

	Token *comma3 = NULL;
	Token *using_ = NULL;
	Token *unit = NULL;

	// Although 3rd length is optional, 4th parameter unit is mandatory (!) in DB2
	if(comma2 != NULL)
	{
		length = GetNext();

		if(_source == SQL_DB2)
		{
			// Make sure it is not unit
			if(Token::Compare(length, "CODEUNITS16", L"CODEUNITS16", 11) == true ||
				Token::Compare(length, "CODEUNITS32", L"CODEUNITS32", 11) == true ||
				Token::Compare(length, "OCTETS", L"OCTETS", 6) == true)
			{
				unit = length;
				comma3 = comma2;

				length = NULL;
				comma2 = NULL;
			}
			// Check unit now
			else
			{
				ParseExpression(length);

				comma3 = GetNext(',', L',');

				if(comma3 != NULL)
					unit = GetNext();
			}
		}
		else
			ParseExpression(length);
	}

	// USING unit in DB2
	if(from != NULL && _source == SQL_DB2)
	{
		using_ = GetNext("USING", L"USING", 5);

		if(using_ != NULL)
			unit = GetNext();
	}

	/*Token *close */ (void) GetNext(')', L')');

	// Convert to SUBSTR in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SUBSTR", L"SUBSTR", 6);

		// Replace FROM and FOR with commas
		Token::Change(from, ",", L",", 1);
		Token::Change(for_, ",", L",", 1);

		// Remove unit
		Token::Remove(using_);
		Token::Remove(comma3);
		Token::Remove(unit);
	}

	name->data_type = TOKEN_DT_STRING;

	FUNC_STATS(name);
	return true;
}

// SUSER_ID in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionSuserId(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Expression is optional
	Token *exp = GetNextToken();

	if(Token::Compare(exp, ')', L')') == true)
	{
		PushBack(exp);
		exp = NULL;
	}
	else
		// Parse expression
		ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;
	
	// Convert to UID in Oracle
	if(_target == SQL_ORACLE)
	{
		if(exp == NULL)
		{
			Token::Change(name, "UID", L"UID", 3);
			Token::Remove(open, close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// SUSER_NAME in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionSuserName(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Expression is optional
	Token *exp = GetNextToken();

	if(Token::Compare(exp, ')', L')') == true)
	{
		PushBack(exp);
		exp = NULL;
	}
	else
		// Parse expression
		ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;
	
	// Convert to SYS_CONTEXT('USERENV', 'OS_USER') in Oracle
	if(_target == SQL_ORACLE)
	{
		if(exp == NULL)
		{
			Token::Change(name, "SYS_CONTEXT('USERENV', 'OS_USER'", L"SYS_CONTEXT('USERENV', 'OS_USER'", 32);
			Token::Remove(open);
		}
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
		Token::Change(name, "CURRENT_USER", L"CURRENT_USER", 12);

	FUNC_STATS(name);
	return true;
}

// SWITCHOFFSET in SQL Server, Sybase ASA
bool SqlParser::ParseFunctionSwitchoffset(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *tmz = GetNextToken();

	// Parse tmz
	ParseExpression(tmz);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *offset = GetNextToken();

	// Parse offset
	ParseExpression(offset);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// SYSDATETIMEOFFSET in Sybase ASA
bool SqlParser::ParseFunctionSysdatetimeoffset(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = GetNextCharToken(')', L')');

	// Convert to SYSTIMESTAMP
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);
		Token::Remove(open, close);
	}

	FUNC_STATS(name);
	return true;
}

// SYS_CONTEXT in Oracle
bool SqlParser::ParseFunctionSysContext(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_SYS_CONTEXT_DESC)

	Token *namespace_ = GetNextToken();

	ParseExpression(namespace_);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *parameter = GetNextToken();

	// Parse offset
	ParseExpression(parameter);

	Token *comma2 = GetNextCharToken(',', L',');

	// Optional length
	if(comma2 != NULL)
	{
		Token *length = GetNextToken();
		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// SYS_GUID in Oracle returns GUID without -
bool SqlParser::ParseFunctionSysGuid(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *close = GetNextCharToken(')', L')');

	// Convert to UUID in MySQL, and trim -
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "REPLACE(UUID", L"REPLACE(UUID", 12);
		AppendNoFormat(close, ", '-', '')", L", '-', '')", 10);
	}

	FUNC_STATS(name);
	return true;
}

// SYSDATE in Oracle, MySQL
bool SqlParser::ParseFunctionSysdate(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_SYSDATE_DESC)

	Token *open = GetNextCharToken('(', L'(');
	Token *close = NULL;

	// SYSDATE() is only allowed in MySQL
	if(open != NULL)
		close = GetNextCharToken(')', L')');

	// Convert to GETDATE in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(open != NULL)
			Token::Change(name, "GETDATE", L"GETDATE", 7);
		else
			Token::Change(name, "GETDATE()", L"GETDATE()", 9);
	}
	else
	// Convert to SYSDATE in Oracle
	if(_target == SQL_ORACLE && open != NULL)
		Token::Remove(open, close);
	else
	// Convert to CURRENT_TIMESTAMP in DB2, PostgreSQL, Greenplum
	if(Target(SQL_DB2, SQL_POSTGRESQL, SQL_GREENPLUM) == true)
	{
		Token::Change(name, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);
		
		if(open != NULL)
			Token::Remove(open, close);
	}
	else
	// Convert to SYSDATE() in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL) && open == NULL)
		Append(name, "()", L"()", 2);

	name->data_type = TOKEN_DT_DATETIME;
	name->nullable = false;

	FUNC_STATS(name);
	return true;
}

// SYSTEM_USER in SQL Server (OS user)
bool SqlParser::ParseFunctionSystemUser(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	if(_target == SQL_ORACLE)
		Token::Change(name, "SYS_CONTEXT('USERENV','OS_USER')", L"SYS_CONTEXT('USERENV','OS_USER')", 32);

	FUNC_STATS(name);
	return true;
}

// SYSTIMESTAMP in Oracle
bool SqlParser::ParseFunctionSystimestamp(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_SYSTIMESTAMP_DESC)

	// Convert to GETDATE in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "GETDATE()", L"GETDATE()", 9);
	else
	// Convert to CURRENT_TIMESTAMP in DB2, MySQL, PostgreSQL, Greenplum
	if(Target(SQL_DB2, SQL_MYSQL, SQL_POSTGRESQL, SQL_GREENPLUM) == true)
		Token::Change(name, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);

	name->data_type = TOKEN_DT_DATETIME;

	FUNC_STATS(name);
	return true;
}

// TIME without parameters in Teradata
bool SqlParser::ParseFunctionTime(Token *name)
{
	if(name == NULL || _source != SQL_TERADATA)
		return false;

	STATS_DECL
	// SYSTIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);

	name->nullable = false;

	name->data_type = TOKEN_DT_DATETIME;
	name->data_subtype = TOKEN_DT2_TIME;

	FUNC_STATS(name);
	return true;
}

// Informix TODAY
bool SqlParser::ParseFunctionToday(Token *name)
{
	if(name == NULL || _source != SQL_INFORMIX)
		return false;

	STATS_DECL
	if(_target == SQL_ORACLE)
		Token::Change(name, "TRUNC(SYSDATE)", L"TRUNC(SYSDATE)", 14);

	name->data_type = TOKEN_DT_DATETIME;

	FUNC_STATS(name);
	return true;
}

// TAN in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionTan(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// TANH in Oracle, DB2
bool SqlParser::ParseFunctionTanh(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *num = GetNextToken();

	if(num == NULL)
		return false;

	ParseExpression(num);

	Token *close = GetNextCharToken(')', L')');

	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "(EXP(2*", L"(EXP(2*", 7);
		Append(close, ") - 1) / (EXP(2*", L") - 1) / (EXP(2*", 16, name);
		AppendCopy(close, open, close);
		Append(close, ") + 1)", L") + 1)", 6);
	}

	FUNC_STATS(name);
	return true;
}

// TEXTPTR in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionTextptr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// TEXTVALID in SQL Server, Sybase ASE
bool SqlParser::ParseFunctionTextvalid(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *pointer = GetNextToken();

	// Parse pointer
	ParseExpression(pointer);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	FUNC_STATS(name);
	return true;
}

// DB2 TIME
bool SqlParser::ParseFunctionTime(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_TIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_TIMESTAMP", L"TO_TIMESTAMP", 12);

	FUNC_STATS(name);
	return true;
}

// DB2 TIMESTAMP
bool SqlParser::ParseFunctionTimestamp(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	// Possible conflict with data type, so check the source
	if(_source != SQL_DB2)
		return false;

	STATS_DECL
	Token *prev = GetPrevToken(name);

	// For DB2 make sure the previous token is not a column name
	if(prev != NULL && (prev->type == TOKEN_WORD || prev->type == TOKEN_IDENT))
		return false;

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Second expression is optional
	if(comma != NULL)
	{
		Token *exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_TIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_TIMESTAMP", L"TO_TIMESTAMP", 12);

	FUNC_STATS(name);
	return true;
}

// TIMESTAMPADD in Sybase ADS
bool SqlParser::ParseFunctionTimestampadd(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *unit = GetNextToken();

	if(unit == NULL)
		return false;

	Token *comma = TOKEN_GETNEXT(',');

	if(comma == NULL)
		return false;

	Token *int_exp = GetNextToken();

	if(int_exp == NULL)
		return false;

	ParseExpression(int_exp);

	Token *comma2 = TOKEN_GETNEXT(',');

	if(comma2 == NULL)
		return false;

	Token *dt_exp = GetNextToken();

	if(dt_exp == NULL)
		return false;

	ParseExpression(dt_exp);

	/*Token *close */ (void) TOKEN_GETNEXT(')');

	if(Target(SQL_SQL_SERVER))
	{
		TOKEN_CHANGE(name, "DATEADD");

		// Sybase ADS units
		if(TOKEN_CMP(unit, "SQL_TSI_YEAR"))
			TOKEN_CHANGE_NOFMT(unit, "yy");
		else	
		if(TOKEN_CMP(unit, "SQL_TSI_QUARTER"))
			TOKEN_CHANGE_NOFMT(unit, "qq");
		else	
		if(TOKEN_CMP(unit, "SQL_TSI_MONTH"))
			TOKEN_CHANGE_NOFMT(unit, "mm");
		else	
		if(TOKEN_CMP(unit, "SQL_TSI_WEEK"))
			TOKEN_CHANGE_NOFMT(unit, "wk");
		else	
		if(TOKEN_CMP(unit, "SQL_TSI_DAY"))
			TOKEN_CHANGE_NOFMT(unit, "dd");
		else	
		if(TOKEN_CMP(unit, "SQL_TSI_HOUR"))
			TOKEN_CHANGE_NOFMT(unit, "hh");
		else	
		if(TOKEN_CMP(unit, "SQL_TSI_MINUTE"))
			TOKEN_CHANGE_NOFMT(unit, "mi");
		else	
		if(TOKEN_CMP(unit, "SQL_TSI_SECOND"))
			TOKEN_CHANGE_NOFMT(unit, "ss");
		else	
		if(TOKEN_CMP(unit, "SQL_TSI_FRAC_SECOND"))
			TOKEN_CHANGE_NOFMT(unit, "ss");
	}

	FUNC_STATS(name);
	return true;
}

// DB2 TIMESTAMPDIFF
bool SqlParser::ParseFunctionTimestampdiff(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *exp2 = GetNextToken();

	if(exp2 == NULL)
		return false;

	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 TIMESTAMP_FORMAT
bool SqlParser::ParseFunctionTimestampFormat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Second expression is optional
	if(comma != NULL)
	{
		Token *exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_TIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_TIMESTAMP", L"TO_TIMESTAMP", 12);

	FUNC_STATS(name);
	return true;
}

// DB2 TIMESTAMP_ISO
bool SqlParser::ParseFunctionTimestampIso(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_TIMESTAMP in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_TIMESTAMP", L"TO_TIMESTAMP", 12);

	FUNC_STATS(name);
	return true;
}

// TO_CHAR in DB2, Oracle, PostgreSQL, Informix, Sybase ASA, Netezza
bool SqlParser::ParseFunctionToChar(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_DTL_DECL
    STATS_SET_DESC(SQL_FUNC_TO_CHAR_DESC)

	// First expression
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *format = NULL;

	// Optional format
	if(comma != NULL)
	{
		format = GetNextToken();

		ParseExpression(format);
	}

	Token *close = GetNextCharToken(')', L')');

	// Default format
	if(format == NULL)
	{
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "CONVERT(VARCHAR, ", L"CONVERT(VARCHAR, ", 17);
			Token::Remove(open);
		}
		else
		// Netezza does not support TO_CHAR without format
		if(_target == SQL_NETEZZA)
		{
			Token::Change(name, "CAST", L"CAST", 4);
			Prepend(close, " AS VARCHAR(30)", L" AS VARCHAR(30)", 15, name);
		}
	}
	else
	// Format is specified and it is a string literal
	if(format != NULL && format->type == TOKEN_STRING)
	{
		// $ and number groups separated by , 
		if(format->Compare("'FM$9,999,999'", L"'FM$9,999,999'", 14) == true)
		{
			// CONCAT('$', FORMAT(num, 0)) in MySQL
			if(Target(SQL_MARIADB, SQL_MYSQL))
			{
				Token::Change(name, "CONCAT('$', FORMAT", L"CONCAT('$', FORMAT", 18);
				Token::Change(format, "0", L"0", 1);
				Append(close, ")", L")", 1);
			}
		}
		else
		// Number groups separated by , 
		if(format->Compare("'9,999,999'", L"'9,999,999'", 11) == true)
		{
			// FORMAT(num, 0) in MySQL
			if(Target(SQL_MARIADB, SQL_MYSQL))
			{
				Token::Change(name, "FORMAT", L"FORMAT", 6);
				Token::Change(format, "0", L"0", 1);
			}
		}
		else
		// Define a style for SQL Server
		if(_target == SQL_SQL_SERVER)
		{
			// YYYY-MM-DD
			if(format->Compare("'YYYY-MM-DD'", L"'YYYY-MM-DD'", 12) == true)
			{
				Token::Change(name, "CONVERT(VARCHAR(10), ", L"CONVERT(VARCHAR(10), ", 21);
				Token::Remove(open);
				Token::Change(format, "120", L"120", 3);
			}
			else
			// YYYYMMDD
			if(format->Compare("'YYYYMMDD'", L"'YYYYMMDD'", 10) == true)
			{
				Token::Change(name, "CONVERT(VARCHAR(8), ", L"CONVERT(VARCHAR(8), ", 20);
				Token::Remove(open);
				Token::Change(format, "112", L"112", 3);
			}
			else
			// YYYY/MM/DD
			if(format->Compare("'YYYY/MM/DD'", L"'YYYY/MM/DD'", 12) == true)
			{
				Token::Change(name, "CONVERT(VARCHAR(10), ", L"CONVERT(VARCHAR(10), ", 21);
				Token::Remove(open);
				Token::Change(format, "111", L"111", 3);
			}
			else
			// HH24:MI:SS
			if(format->Compare("'HH24:MI:SS'", L"'HH24:MI:SS'", 12) == true)
			{
				Token::Change(name, "CONVERT(VARCHAR(8), ", L"CONVERT(VARCHAR(8), ", 20);
				Token::Remove(open);
				Token::Change(format, "108", L"108", 3);
			}
		}
		else
		// Process each format specifier
		if(Target(SQL_MYSQL) == true)
		{
			TokenStr out;
			size_t i = 0;

			bool datetime = false;

			while(i < format->len)
			{
				// YYYY or RRRR part (4-digit year)
				if(format->Compare("YYYY", L"YYYY", i, 4) == true || 
					format->Compare("RRRR", L"RRRR", i, 4) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
						out.Append("%Y", L"%Y", 2);

					datetime = true;

					i += 4;
					continue;
				}
				else
				// RR part (2-digit year rounded)
				if(format->Compare("RR", L"RR", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
						out.Append("%y", L"%y", 2);

					datetime = true;

					i += 2;
					continue;
				}
				else
				// MM month (01-12)
				if(format->Compare("MM", L"MM", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
						out.Append("%m", L"%m", 2);

					datetime = true;

					i += 2;
					continue;
				}
				else
				// MON month (Jan-Dec)
				if(format->Compare("MON", L"MON", i, 3) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
						out.Append("%b", L"%b", 2);

					datetime = true;

					i += 3;
					continue;
				}
				else
				// DD month (01-31)
				if(format->Compare("DD", L"DD", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
					out.Append("%d", L"%d", 2);

					datetime = true;

					i += 2;
					continue;
				}
				else
				// HH24 hour 
				if(format->Compare("HH24", L"HH24", i, 4) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
					out.Append("%H", L"%H", 2);

					datetime = true;

					i += 4;
					continue;
				}
				else
				// MI minute 
				if(format->Compare("MI", L"MI", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
					out.Append("%i", L"%i", 2);

					datetime = true;

					i += 2;
					continue;
				}
				else
				// SS second 
				if(format->Compare("SS", L"SS", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
					out.Append("%s", L"%s", 2);

					datetime = true;

					i += 2;
					continue;
				}

				// No format matched just copy
				out.Append(format, i, 1);
				i++;
			}

			// A datetime format was specified
			if(datetime == true)
			{
				if(Target(SQL_MARIADB, SQL_MYSQL))
					Token::Change(name, "DATE_FORMAT", L"DATE_FORMAT", 11);
			}
			
			// Set target format
			Token::ChangeNoFormat(format, out);
		}
		else
		// Informix to Oracle format mapping
		if(_source == SQL_INFORMIX && Target(SQL_ORACLE, SQL_DB2) == true)
		{
			TokenStr out;
			size_t i = 0;

			while(i < format->len)
			{
				// %Y (4-digit year)
				if(Token::CompareCS(format, "%Y", L"%Y", i, 2) == true)
				{
					if(Target(SQL_ORACLE, SQL_DB2) == true)
						out.Append("YYYY", L"YYYY", 4);

					i += 2;
					continue;
				}
				else
				// %y (2-digit year rounded)
				if(Token::CompareCS(format, "%y", L"%y", i, 2) == true)
				{
					if(Target(SQL_ORACLE, SQL_DB2) == true)
						out.Append("RR", L"RR", 2);

					i += 2;
					continue;
				}
				else
				// %m month (01-12)
				if(Token::CompareCS(format, "%m", L"%m", i, 2) == true)
				{
					if(Target(SQL_ORACLE, SQL_DB2) == true)
						out.Append("MM", L"MM", 2);

					i += 2;
					continue;
				}
				else
				// %d day (01-31)
				if(Token::CompareCS(format, "%d", L"%d", i, 2) == true)
				{
					if(Target(SQL_ORACLE, SQL_DB2) == true)
						out.Append("DD", L"DD", 2);

					i += 2;
					continue;
				}

				// No format matched just copy
				out.Append(format, i, 1);
				i++;
			}

			// Set target format
			Token::ChangeNoFormat(format, out);
		}

        if(_stats != NULL)
        {
            TokenStr st;
            st.Append(name);
            st.Append(open);
            st.Append("value, ", L"value, ", 7);
            st.Append(format);
            st.Append(")", L")", 1);

			FUNC_DTL_STATS_V(st.str.c_str(), name)
        }
	}

	name->data_type = TOKEN_DT_STRING;

	// If first argument is non-nullable, set the function result non-nullable
	if(exp->nullable == false)
		name->nullable = false;

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle TO_CLOB
bool SqlParser::ParseFunctionToClob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		// Second parameter is removed
		Token::Remove(comma);
		Token::Remove(exp2);
	}

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle TO_DATE
bool SqlParser::ParseFunctionToDate(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_DTL_DECL
    STATS_SET_DESC(SQL_FUNC_TO_DATE_DESC)

	// First expression
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *format = NULL;

	// Optional format
	if(comma != NULL)
	{
		format = GetNextToken();

		ParseExpression(format);
	}

	Token *comma2 = GetNextCharToken(',', L',');
	Token *nls = NULL;

	// Optional NLS settings
	if(comma2 != NULL)
	{
		nls = GetNextToken();

		ParseExpression(nls);
	}

	Token *close = GetNextCharToken(')', L')');

	// CONVERT(DATETIME, exp, style) in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CONVERT", L"CONVERT", 7);
		Append(open, "DATETIME, ", L"DATETIME, ", 10);
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		if(format != NULL)
			TOKEN_CHANGE(name, "STR_TO_DATE");
		else
		{
			TOKEN_CHANGE(name, "CAST");
			PREPEND_NOFMT(close, " AS DATETIME");
		}
	}

	// Format is specified and it is a string literal, process each part
	if(format != NULL && format->type == TOKEN_STRING)
	{
		// For SQL Server, choose appropriate style
		if(_target == SQL_SQL_SERVER)
		{
			// YYYY-MM-DD, YYYY/MM/DD, DD-MON-YYYY recognized by default
			if(format->Compare("'YYYY-MM-DD'", L"'YYYY-MM-DD'", 12) == true ||
				format->Compare("'YYYY/MM/DD'", L"'YYYY/MM/DD'", 12) == true ||
				format->Compare("'DD-MON-YYYY'", L"'DD-MON-YYYY'", 13) == true)
				Token::Remove(comma, format);
		}
		else
		// Process each format specifier
		if(Target(SQL_MYSQL) == true)
		{
			TokenStr out;
			size_t i = 0;

			while(i < format->len)
			{
				// YYYY or RRRR part (4-digit year)
				if(format->Compare("YYYY", L"YYYY", i, 4) == true ||
					format->Compare("RRRR", L"RRRR", i, 4) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
						out.Append("%Y", L"%Y", 2);

					i += 4;
					continue;
				}
				else
				// RR part (2-digit year rounded)
				if(format->Compare("RR", L"RR", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
						out.Append("%y", L"%y", 2);

					i += 2;
					continue;
				}
				else
				// MM month (01-12)
				if(format->Compare("MM", L"MM", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
						out.Append("%m", L"%m", 2);

					i += 2;
					continue;
				}
				else
				// DD month (01-31)
				if(format->Compare("DD", L"DD", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
					out.Append("%d", L"%d", 2);

					i += 2;
					continue;
				}
				else
				// HH24 hour
				if(format->Compare("HH24", L"HH24", i, 4) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
					out.Append("%H", L"%H", 2);

					i += 4;
					continue;
				}
				else
				// MI minutes
				if(format->Compare("MI", L"MI", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
					out.Append("%i", L"%i", 2);

					i += 2;
					continue;
				}
				else
				// SS seconds
				if(format->Compare("SS", L"SS", i, 2) == true)
				{
					if(Target(SQL_MARIADB, SQL_MYSQL))
					out.Append("%s", L"%s", 2);

					i += 2;
					continue;
				}

				// No format matched just copy
				out.Append(format, i, 1);
				i++;
			}

			// Set target format
			Token::ChangeNoFormat(format, out);
		}

		if(_stats != NULL)
        {
            TokenStr st;
            st.Append(name);
            st.Append(open);
            st.Append("value, ", L"value, ", 7);
            st.Append(format);
            st.Append(")", L")", 1);

            FUNC_DTL_STATS_V(st.str.c_str(), name)
        }
	}

	name->data_type = TOKEN_DT_DATETIME;

	// Supported in MariaDB for Oracle compatibility mode
	STATS_SET_CONV_NO_NEED(Target(SQL_MARIADB_ORA))
		
	FUNC_STATS(name);
	return true;
}

// TODATETIMEOFFSET in SQL Server, Sybase ASA
bool SqlParser::ParseFunctionTodatetimeoffset(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *timestamp = GetNextToken();

	// Parse timestamp
	ParseExpression(timestamp);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *timezone = GetNextToken();

	// Parse timezone
	ParseExpression(timezone);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to FROM_TZ in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "FROM_TZ", L"FROM_TZ", 7);

	FUNC_STATS(name);
	return true;
}

// TODAY in Sybase ASA
bool SqlParser::ParseFunctionToday(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Asterisk is optional
	Token *asterisk = GetNextCharToken('*', L'*');
	Token *close = GetNextCharToken(')', L')');

	// If target type is not Sybase ASA remove asterisk
	if(asterisk != NULL && _target != SQL_SYBASE_ASA)
		Token::Remove(asterisk);

	// Convert to TRUNC(SYSDATE) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TRUNC(SYSDATE", L"TRUNC(SYSDATE", 13);
		Token::Remove(open);
	}
	else
	// Use GETDATE function in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CAST(GETDATE", L"CAST(GETDATE", 12);
		Append(close, " AS DATE)", L" AS DATE)", 9, name);
	}
	
	FUNC_STATS(name);
	return true;	
}

// TO_LOB in Oracle
bool SqlParser::ParseFunctionToLob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;	
}

// TO_NCHAR in Oracle, DB2, Sybase ASA
bool SqlParser::ParseFunctionToNchar(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp1 = GetNextToken();

	if(exp1 == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to CONVERT in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		if(comma == NULL)
		{
			Token::Change(name, "CONVERT", L"CONVERT", 7);
			Append(open, "NCHAR, ", L"NCHAR, ", 7, name);
		}
	}

	FUNC_STATS(name);
	return true;
}

// DB2 TO_NCLOB
bool SqlParser::ParseFunctionToNclob(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *exp2 = NULL;
	
	// Second expression is optional
	if(comma != NULL)
	{
		exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CLOB in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_CLOB", L"TO_CLOB", 7);

		// Second parameter is removed
		Token::Remove(comma);
		Token::Remove(exp2);
	}

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle TO_NUMBER
bool SqlParser::ParseFunctionToNumber(Token *name, Token* open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_DTL_DECL
    STATS_SET_DESC(SQL_FUNC_TO_NUMBER_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *format = NULL;
	
	// Format is optional
	if(comma != NULL)
	{
		format = GetNextToken();

		if(format == NULL)
			return false;

		// Parse second expression
		ParseExpression(format);

		if(_stats != NULL && format != NULL)
        {
            TokenStr st;
            st.Append(name);
            st.Append(open);
            st.Append("value, ", L"value, ", 7);
            st.Append(format);
            st.Append(")", L")", 1);

			FUNC_DTL_STATS_V(st.str.c_str(), name)
        }
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle TO_SINGLE_BYTE
bool SqlParser::ParseFunctionToSingleByte(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle TO_TIMESTAMP
bool SqlParser::ParseFunctionToTimestamp(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_DTL_DECL
	STATS_SET_DESC(SQL_FUNC_TO_TIMESTAMP_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Second expression is optional
	if(comma != NULL)
	{
		Token *exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		ParseExpression(exp2);

		if(_stats != NULL)
        {
            TokenStr st;
            st.Append(name);
            st.Append("(value, ", L"(value, ", 8);
            st.Append(exp2);
            st.Append(")", L")", 1);

            FUNC_DTL_STATS_V(st.str.c_str(), name)
        }
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// Oracle TO_TIMESTAMP_TZ
bool SqlParser::ParseFunctionToTimestampTz(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_TO_TIMESTAMP_TZ_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Second expression is optional
	if(comma != NULL)
	{
		Token *exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	STATS_SET_COMPLEXITY(Target(SQL_MYSQL, SQL_MARIADB, SQL_MARIADB_ORA), STATS_CONV_MEDIUM)

	FUNC_STATS(name);
	return true;
}

// TO_UNICHAR in Sybase ASE
bool SqlParser::ParseFunctionToUnichar(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

	if(close == NULL)
		return false;

	// Convert to TO_NCHAR in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_UNICHAR", L"TO_UNICHAR", 10);
	else
	// Convert to CONVERT in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "CONVERT(NVARCHAR, ", L"CONVERT(NVARCHAR, ", 18);
		Token::Remove(open);
	}

	FUNC_STATS(name);
	return true;
}

// TRANSLATE in Oracle, DB2
bool SqlParser::ParseFunctionTranslate(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_TRANSLATE_DESC)

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *to = GetNextToken();

	// Parse to
	ParseExpression(to);
		
	Token *comma2 = GetNextCharToken(',', L',');

	if(comma2 == NULL)
		return false;

	Token *from = GetNextToken();

	// Parse from
	ParseExpression(from);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// TRIM in Oracle, DB2, Sybase ASA
bool SqlParser::ParseFunctionTrim(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_TRIM_DESC)

	Token *type = GetNextToken();

	// Type is optional in Oracle, DB2
	if(Token::Compare(type, "LEADING", L"LEADING", 7) == false &&
		Token::Compare(type, "TRAILING", L"TRAILING", 8) == false &&
		Token::Compare(type, "BOTH", L"BOTH", 4) == false &&
		Token::Compare(type, "L", L"L", 1) == false &&
		Token::Compare(type, "T", L"T", 1) == false &&
		Token::Compare(type, "B", L"B", 1) == false)
	{
		PushBack(type);
		type = NULL;
	}

	Token *trim = GetNextToken();
	Token *from = NULL;

	if(Token::Compare(trim, "FROM", L"FROM", 4))
	{
		from = trim;
		trim = NULL;
	}
	else
		// Parse trim in Oracle, DB2
		ParseExpression(trim);

	if(from == NULL)
		from = GetNextWordToken("FROM", L"FROM", 4);
	
	Token *string = NULL;

	// If FROM is not set then trim is also not set in Oracle, DB2
	if(from == NULL)
	{
		string = trim;
		trim = NULL;
	}

	if(string == NULL)
		string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	Token *close = GetNextCharToken(')', L')');

	// Use LTRIM and RTRIM functions in SQL Server
	if(_target == SQL_SQL_SERVER)
	{
		Token::Change(name, "RTRIM(LTRIM", L"LTRIM(RTRIM", 11);
		Append(close, ")", L")", 1);
	}
	else
	// Oracle does not support L, T, and B types
	if(_target == SQL_ORACLE)
	{
		if(Token::Compare(type, "L", L"L", 1))
			Token::Change(type, "LEADING", L"LEADING", 7);
		else
		if(Token::Compare(type, "T", L"T", 1))
			Token::Change(type, "TRAILING", L"TRAILING", 8);
		else
		if(Token::Compare(type, "B", L"B", 1))
			Token::Change(type, "BOTH", L"BOTH", 4);
	}

	STATS_SET_CONV_NO_NEED(true)
	FUNC_STATS(name);

	return true;
}

// TRUNC in Oracle, DB2
bool SqlParser::ParseFunctionTrunc(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_TRUNC_DESC)

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *end_exp = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	Token *unit = NULL;

	// Truncation unit is optional
	if(comma != NULL)
	{
		unit = GetNextToken();

		// Parse trancation unit
		ParseExpression(unit);
	}

	Token *close = GetNextCharToken(')', L')');

	// Default datetime truncation
	if(unit == NULL && exp->data_type == TOKEN_DT_DATETIME)
	{
		if(_target == SQL_SQL_SERVER)
		{
			Token::Change(name, "CONVERT(DATETIME, CONVERT(DATE, ", L"CONVERT(DATETIME, CONVERT(DATE, ", 31);
			Token::Remove(open);
			Append(close, ")", L")", 1);

			name->data_type = TOKEN_DT_DATETIME;
		}
		else
		// Convert to DATE in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
			Token::Change(name, "DATE", L"DATE", 4);
	}
	else
	// Unit is specified and it is a string
	if(unit != NULL && unit->type == TOKEN_STRING)
	{
		// Set return data type 
		name->data_type = TOKEN_DT_DATETIME;

		// 'DD' truncate to day
		if(unit->Compare("'DD'", L"'DD'", 4) == true)
		{
			if(_target == SQL_SQL_SERVER)
			{
				Token::Change(name, "CONVERT(DATETIME, CONVERT(DATE, ", L"CONVERT(DATETIME, CONVERT(DATE, ", 31);
				Token::Remove(open);
				Append(close, ")", L")", 1);

				Token::Remove(comma);
				Token::Remove(unit);
			}
            else
            // Convert to DATE in MySQL
		    if(Target(SQL_MARIADB, SQL_MYSQL))
            {
			    Token::Change(name, "DATE", L"DATE", 4);

                Token::Remove(comma);
				Token::Remove(unit);
            }
		}
		else
		// 'MM' truncate to month
		if(unit->Compare("'MM'", L"'MM'", 4) == true)
		{
			if(_target == SQL_SQL_SERVER)
			{
				Token::Change(name, "CONVERT(DATETIME, CONVERT(VARCHAR(7), ", 
						L"CONVERT(DATETIME, CONVERT(VARCHAR(7), ", 38);
				Token::Remove(open);
				
				Token::Change(unit, "120) + '-01'", L"120) + '-01'", 12);
			}
            else
            // DATE_FORMAT in MySQL
            if(Target(SQL_MARIADB, SQL_MYSQL))
            {
                Token::Change(name, "DATE_FORMAT", L"DATE_FORMAT", 11);
                Token::ChangeNoFormat(unit, "'%Y-%m-01'", L"'%Y-%m-01'", 10);
            }
		}
	}
	else
	{
		// Convert to TRUNCATE in MySQL
		if(Target(SQL_MARIADB, SQL_MYSQL))
		{
			Token::Change(name, "TRUNCATE", L"TRUNCATE", 8);

			// Add second parameter
			if(unit == NULL)
				Append(end_exp, ", 0", L", 0", 3);
		}
	}

	FUNC_STATS(name);
	return true;
}

// TRUNCATE in DB2, Sybase ASA
bool SqlParser::ParseFunctionTruncate(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *unit = NULL;

	// Truncation unit is optional in DB2
	if(comma != NULL)
	{
		unit = GetNextToken();

		// Parse unit
		ParseExpression(unit);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TRUNC in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TRUNC", L"TRUNC", 5);

	FUNC_STATS(name);
	return true;
}

// TRUNCNUM in Sybase ASA
bool SqlParser::ParseFunctionTruncnum(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *numeric = GetNextToken();

	// Parse numeric
	ParseExpression(numeric);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *integer = GetNextToken();

	// Parse integer
	ParseExpression(integer);
	
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TRUNC in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TRUNC", L"TRUNC", 5);

	FUNC_STATS(name);
	return true;
}

// DB2 TRUNC_TIMESTAMP
bool SqlParser::ParseFunctionTruncTimestamp(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Format expression is optional
	if(comma != NULL)
	{
		Token *format = GetNextToken();

		if(format == NULL)
			return false;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
		Token::Change(name, "TRUNC", L"TRUNC", 5);

	FUNC_STATS(name);
	return true;
}

// TSEQUAL in Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionTsequal(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *timestamp1 = GetNextToken();

	// Parse first timestamp-expression
	ParseExpression(timestamp1);

	Token *comma = GetNextCharToken(',', L',');
	
	if(comma == NULL)
		return false;

	Token *timestamp2 = GetNextToken();

	// Parse second timestamp-expression
	ParseExpression(timestamp2);

	Token *close = GetNextCharToken(')', L')');

	// Convert to timestamp1 = timestamp2 in other databases
	if(Target(SQL_SYBASE, SQL_SYBASE_ASA) == false)
	{
		Token::Remove(name, open);
		Token::Change(comma, " =", L" =", 2);
		Token::Remove(close);
	}

	FUNC_STATS(name);
	return true;
}

// UCASE in DB2, Sybase ASA
bool SqlParser::ParseFunctionUcase(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to UPPER in Oracle, SQL Server
	if(Target(SQL_ORACLE, SQL_SQL_SERVER) == true)
		Token::Change(name, "UPPER", L"UPPER", 5);

	FUNC_STATS(name);
	return true;
}

// UHIGHSURR in Sybase ASE
bool SqlParser::ParseFunctionUhighsurr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *start = GetNextToken();

	// Parse start
	ParseExpression(start);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// ULOWSURR in Sybase ASE
bool SqlParser::ParseFunctionUlowsurr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *start = GetNextToken();

	// Parse start
	ParseExpression(start);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// UNICODE in SQL Server, Sybase ASA
bool SqlParser::ParseFunctionUnicode(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// UNISTR in Oracle, Sybase ASA
bool SqlParser::ParseFunctionUnistr(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	Token *end_string = GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to NCHAR in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "NCHAR", L"NCHAR", 5);
	else
	// Convert to CHAR in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "CHAR", L"CHAR", 4);
		Append(end_string, " USING UCS2", L" USING UCS2", 11, name);
	}

	FUNC_STATS(name);
	return true;
}

// UPPER in Oracle, SQL Server, DB2, MySQL, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionUpper(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
    STATS_SET_DESC(SQL_FUNC_UPPER_DESC)

	Token *string = GetNextToken();

	if(string == NULL)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	STATS_SET_CONV_NO_NEED(true)
	FUNC_STATS(name);
	return true;
}

// USCALAR in Sybase ASE
bool SqlParser::ParseFunctionUscalar(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to UNICODE in SQL Server
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "UNICODE", L"UNICODE", 7);

	FUNC_STATS(name);
	return true;
}

// USER in Oracle, SQL Server, DB2, Sybase ASE
bool SqlParser::ParseFunctionUser(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Convert to SYSTEM_USER in SQL Server (CURRENT_USER is also supported but returns 'dbo' i.e.)
	if(_target == SQL_SQL_SERVER)
		Token::Change(name, "SYSTEM_USER", L"SYSTEM_USER", 11);
	else
	// Add parentheses in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
		AppendNoFormat(name, "()", L"()", 2);
	else
	// Convert to CURRENT_USER in PostgreSQL, Greenplum
	if(Target(SQL_POSTGRESQL, SQL_GREENPLUM) == true)
		Token::Change(name, "CURRENT_USER", L"CURRENT_USER", 12);

	name->nullable = false;

	FUNC_STATS(name);
	return true;
}

// _UTF8'string' in MySQL
bool SqlParser::ParseFunctionUtf8(Token *name)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// String literal in MySQL
	/*Token *string */ (void) GetNextToken();

	FUNC_STATS(name);
	return true;
}

// USER_ID in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionUserId(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// User_name is optional
	Token *user_name = GetNextToken();

	if(Token::Compare(user_name, ')', L')') == true)
	{
		PushBack(user_name);
		user_name = NULL;
	}
	else
		// Parse user_name
		ParseExpression(user_name);

	Token *close = GetNextCharToken(')', L')');
	
	// Convert to UID in Oracle
	if(_target == SQL_ORACLE)
	{
		if(user_name == NULL)
		{
			Token::Change(name, "UID", L"UID", 3);
			Token::Remove(open, close);
		}
	}

	FUNC_STATS(name);
	return true;
}

// USERENV in Oracle
bool SqlParser::ParseFunctionUserenv(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *parameter = GetNextToken();

	if(parameter == NULL)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// USER_NAME in SQL Server, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionUserName(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// User_id is optional
	Token *user_id = GetNextToken();

	if(Token::Compare(user_id, ')', L')') == true)
	{
		PushBack(user_id);
		user_id = NULL;
	}
	else
		// Parse user_id
		ParseExpression(user_id);

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// Convert to SYS_CONTEXT('USERENV', 'OS_USER') in Oracle
	if(_target == SQL_ORACLE)
	{
		if(user_id == NULL)
		{
			Token::Change(name, "SYS_CONTEXT('USERENV', 'OS_USER'", L"SYS_CONTEXT('USERENV', 'OS_USER'", 32);
			Token::Remove(open);
		}
	}
	else
	if(Target(SQL_MARIADB, SQL_MYSQL))
		TOKEN_CHANGE(name, "USER");

	FUNC_STATS(name);
	return true;
}

// DB2 VALUE
bool SqlParser::ParseFunctionValue(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
		Token::Change(name, "COALESCE", L"COALESCE", 8);

	FUNC_STATS(name);
	return true;
}

// DB2 VARCHAR
bool SqlParser::ParseFunctionVarchar(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	// Possible conflict with data type, so check the source
	if(_source != SQL_DB2)
		return false;

	STATS_DECL
	Token *prev = GetPrevToken(name);

	// For DB2 make sure the previous token is not a column name
	if(prev != NULL && (prev->type == TOKEN_WORD || prev->type == TOKEN_IDENT))
		return false;

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Second expression is optional
	if(comma != NULL)
	{
		Token *exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);

	FUNC_STATS(name);
	return true;
}

// DB2 VARCHAR_BIT_FORMAT
bool SqlParser::ParseFunctionVarcharBitFormat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *format = NULL;
	
	// Format is optional
	if(comma != NULL)
	{
		format = GetNextToken();

		if(format == NULL)
			return false;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to HEXTORAW in Oracle
	if(_target == SQL_ORACLE)
    {
		Token::Change(name, "HEXTORAW", L"HEXTORAW", 8);

		// Format is removed
		Token::Remove(comma);
		Token::Remove(format);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 VARCHAR_FORMAT
bool SqlParser::ParseFunctionVarcharFormat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Format is optional
	if(comma != NULL)
	{
		Token *format = GetNextToken();

		if(format == NULL)
			return false;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_CHAR in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_CHAR", L"TO_CHAR", 7);

	FUNC_STATS(name);
	return true;
}

// DB2 VARCHAR_FORMAT_BIT
bool SqlParser::ParseFunctionVarcharFormatBit(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *format = NULL;
	
	// Format is optional
	if(comma != NULL)
	{
		format = GetNextToken();

		if(format == NULL)
			return false;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to RAWTOHEX in Oracle
	{
	if(_target == SQL_ORACLE)
		Token::Change(name, "RAWTOHEX", L"RAWTOHEX", 8);

		// Format is removed
		Token::Remove(comma);
		Token::Remove(format);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 VARGRAPHIC
bool SqlParser::ParseFunctionVargraphic(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Second expression is optional
	if(comma != NULL)
	{
		Token *exp2 = GetNextToken();

		if(exp2 == NULL)
			return false;

		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NCHAR in Oracle
	if(_target == SQL_ORACLE)
		Token::Change(name, "TO_NCHAR", L"TO_NCHAR", 8);

	FUNC_STATS(name);
	return true;
}

// VSIZE in Oracle
bool SqlParser::ParseFunctionVsize(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', ')');

	FUNC_STATS(name);
	return true;
}

// DB2 WEEK
bool SqlParser::ParseFunctionWeek(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER(TO_CHAR()) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);
		Append(exp, ", 'WW')", L", 'WW')", 7);
	}

	FUNC_STATS(name);
	return true;
}

// DB2 WEEK_ISO
bool SqlParser::ParseFunctionWeekIso(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to TO_NUMBER(TO_CHAR()) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "TO_NUMBER(TO_CHAR", L"TO_NUMBER(TO_CHAR", 17);
		Append(exp, ", 'IW')", L", 'IW')", 7);
	}

	FUNC_STATS(name);
	return true;
}

// Optional XMLTYPE methods chain - .extract() i.e.
bool SqlParser::ParseFunctionXmlChainedMethods()
{
	bool exists = false;

	Token *name = GetNextToken();

	// Method starts with .
	if(TOKEN_CMP_PART0(name, "."))
	{
		Token *open = TOKEN_GETNEXT('(');

		while(open != NULL)
		{
			exists = true;

			if(ParseExpression() == NULL)
				break;

			Token *comma = TOKEN_GETNEXT(',');

			if(comma == NULL)
				break;
		}

		/*Token *close */ TOKEN_GETNEXTP(open, ')');
	}
	else
		PushBack(name);

	return exists;
}

// XMLAGG in Oracle
bool SqlParser::ParseFunctionXmlagg(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_XMLAGG_DESC)

	Token *instance = GetNextToken();

	if(instance == NULL)
		return false;

	// Parse instance
	ParseExpression(instance);

	// Optional ORDER BY clause
	Token *order = TOKEN_GETNEXTW("ORDER");

	if(order != NULL)
	{
		Token *by = TOKEN_GETNEXTW("BY");
		/*Token *col */ GetNextToken(by);
	}

	/*Token *close */ GetNextCharToken(')', L')');

	// Optional XMLTYPE methods chain - .extract() i.e.
	ParseFunctionXmlChainedMethods();

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle XMLATTRIBUTES
bool SqlParser::ParseFunctionXmlattributes(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// Variable number of parameters
	while(true)
	{
		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		ParseExpression(expn);

		// Optional AS name
		Token *as = GetNextWordToken("AS", L"AS", 2);
	
		if(as != NULL)
		{
			Token *name = GetNextToken();

			if(name == NULL)
				return false;
		}

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLCAST in Oracle
bool SqlParser::ParseFunctionXmlcast(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *as = GetNextWordToken("AS", L"AS", 2);

	if(as == NULL)
		return false;

	/*Token *datatype */ (void) GetNextToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLCDATA in Oracle
bool SqlParser::ParseFunctionXmlcdata(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLCOMMENT in Oracle, DB2
bool SqlParser::ParseFunctionXmlcomment(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLCONCAT in Oracle, DB2, Sybase ASA
bool SqlParser::ParseFunctionXmlconcat(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLDIFF in Oracle
bool SqlParser::ParseFunctionXmldiff(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *doc = GetNextToken();

	// Parse first document
	ParseExpression(doc);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *doc2 = GetNextToken();

	// Parse second document
	ParseExpression(doc2);

	Token *comma2 = GetNextCharToken(',', L',');
	Token *integer = NULL;
	Token *comma3 = NULL;
	Token *string = NULL;

	// Integer and string are optional
	if(comma2 != NULL)
	{
		integer = GetNextToken();

		// Parse integer
		ParseExpression(integer);

		comma3 = GetNextCharToken(',', L',');

		if(comma3 == NULL)
			return false;

		string = GetNextToken();

		// Parse string
		ParseExpression(string);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 XMLDOCUMENT
bool SqlParser::ParseFunctionXmldocument(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLELEMENT in Oracle, DB2, Sybase ASA
bool SqlParser::ParseFunctionXmlelement(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	STATS_SET_DESC(SQL_FUNC_XMLELEMENT_DESC)

	// Optional NAME keyword
	/*Token *name_keyword */ TOKEN_GETNEXTW("NAME");

	Token *element = GetNextToken();

	// Parse element
	ParseExpression(element);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLEXTRACT in Sybase ASE
bool SqlParser::ParseFunctionXmlextract(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *xpath = GetNextToken();

	// Parse xpath
	ParseExpression(xpath);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *returns = GetNextWordToken("RETURNS", L"RETURNS", 7);

	// Returns is optional
	if(returns != NULL)
		/*Token *datatype */ (void) GetNextToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to EXTRACT in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "EXTRACT", L"EXTRACT", 7);
		AppendCopy(open, exp);
		Append(open, ", ", L", ", 2);
		
		Token::Remove(comma, exp);
	}

	FUNC_STATS(name);
	return true;
}

// XMLFOREST in Oracle, DB2, Sybase ASA
bool SqlParser::ParseFunctionXmlforest(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *as = GetNextWordToken("AS", L"AS", 2);
	Token *alias = NULL;

	// Alias is optional
	if(as != NULL)
		alias = GetNextToken();

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;

		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);

		Token *asn = GetNextWordToken("AS", L"AS", 2);

		// Alias is optional
		if(asn != NULL)
			/*Token *aliasn */ (void) GetNextToken();
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLGEN in Sybase ASA
bool SqlParser::ParseFunctionXmlgen(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *constructor = GetNextToken();

	if(constructor == NULL)
		return false;

	// Parse constructor
	ParseExpression(constructor);

	// Variable number of parameters
	while(true)
	{
		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		// Parse next expression
		ParseExpression(expn);

		Token *as = GetNextWordToken("AS", L"AS", 2);

		// AS is optional
		if(as != NULL)
			/*Token *aliasn */ (void) GetNextToken();

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLISVALID in Oracle
bool SqlParser::ParseFunctionXmlisvalid(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *instance = GetNextToken();

	if(instance == NULL)
		return false;

	// Parse instance
	ParseExpression(instance);

	Token *comma = GetNextCharToken(',', L',');
	Token *url = NULL;
	Token *comma2 = NULL;
	Token *element = NULL;

	// Url is optional
	if(comma != NULL)
	{
		url = GetNextToken();

		// Parse url
		ParseExpression(url);

		comma2 = GetNextCharToken(',', L',');

		// Element is optional
		if(comma2 != NULL)
		{
			element = GetNextToken();

			// Parse element
			ParseExpression(element);
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle XMLNAMESPACES
bool SqlParser::ParseFunctionXmlnamespaces(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;
	
	STATS_DECL
	// DEFAULT or NO DEFAULT can be specified as single parameter
	Token *no = GetNextWordToken("NO", L"NO", 2); 
	Token *def = GetNextWordToken("DEFAULT", L"DEFAULT", 7); 
	
	if(no == NULL && def != NULL)
	{
		Token *uri = GetNextToken();

		ParseExpression(uri);
	}

	while(true)
	{
		if(def != NULL)
			break;
		
		Token *uri = GetNextToken();

		if(uri == NULL)
			break;

		ParseExpression(uri);

		Token *as = GetNextWordToken("AS", L"AS", 2);

		if(as == NULL)
			break;

		Token *prefix = GetNextToken();

		if(prefix == NULL)
			break;

		ParseExpression(prefix);

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLPARSE in Oracle, DB2, Sybase ASE
bool SqlParser::ParseFunctionXmlparse(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// DOCUMENT is mandatory in Oracle, DB2
	/*Token *document */ (void) GetNextWordToken("DOCUMENT", L"DOCUMENT", 8);

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);
	
	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLPATCH in Oracle
bool SqlParser::ParseFunctionXmlpatch(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *doc = GetNextToken();

	// Parse first document
	ParseExpression(doc);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *doc2 = GetNextToken();

	// Parse second document
	ParseExpression(doc2);
	
	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLPI in Oracle, DB2
bool SqlParser::ParseFunctionXmlpi(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *keyword = GetNextWordToken("NAME", L"NAME", 4);

	if(keyword == NULL)
		return false;

	Token *identifier = GetNextToken();

	// Parse identifier
	ParseExpression(identifier);

	Token *comma = GetNextCharToken(',', L',');

	// Expression is optional
	if(comma != NULL)
	{
		Token *exp = GetNextToken();

		// Parse expression
		ParseExpression(exp);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2, Oracle XMLQUERY
bool SqlParser::ParseFunctionXmlquery(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp_cons = GetNextToken();

	if(exp_cons == NULL)
		return false;

	ParseExpression(exp_cons);

	// Passing is optional
	Token *passing = GetNextWordToken("PASSING", L"PASSING", 7);
	Token *by = NULL;
	Token *ref = NULL;
	Token *by2 = NULL;
	Token *ref2 = NULL;
	Token *returning = NULL;
	Token *sequence = NULL;
	Token *by3 = NULL;
	Token *ref3 = NULL;
	
	if(passing != NULL)
	{
		// By ref is optional
		by = GetNextWordToken("BY", L"BY", 2);
		if(by != NULL)
			ref = GetNextWordToken("REF", L"REF", 3);

		// Variable number of arguments
		while(true)
		{
			Token *expn = GetNextToken();

			if(expn == NULL)
				break;

			ParseExpression(expn);

			Token *as = GetNextWordToken("AS", L"AS", 2);

			if(as == NULL)
				break;

			Token *identifier = GetNextToken();

			if(identifier == NULL)
				break;

			// By ref is optional
			by2 = GetNextWordToken("BY", L"BY", 2);
			if(by2 != NULL)
				ref2 = GetNextWordToken("REF", L"REF", 3);

			Token *comma = GetNextCharToken(',', L',');

			if(comma == NULL)
				break;
		}
	}

	// Returning sequence is optional
	returning = GetNextWordToken("RETURNING", L"RETURNING", 9);
	if(returning != NULL)
	{
		sequence = GetNextWordToken("SEQUENCE", L"SEQUNCE", 7);

		// By ref is optional
		by3 = GetNextWordToken("BY", L"BY", 2);
		if(by3 != NULL)
			ref3 = GetNextWordToken("REF", L"REF", 3);
	}

	// Empty on empty is optional
	Token *empty = GetNextWordToken("EMPTY", L"EMPTY", 5);
	if(empty != NULL)
	{
		/*Token *on */ (void) GetNextWordToken("ON", L"ON", 2);
		/*Token *empty2 */ (void) GetNextWordToken("EMPTY", L"EMPTY", 5);
	}
	
	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		Token::Change(ref, "VALUE", L"VALUE", 5);
		Token::Remove(by2, ref2);
		Token::Change(sequence, "CONTENT", L"CONTENT", 7);
		Token::Remove(by3, ref3);
		Token::Change(empty, "NULL", L"NULL", 4);
	}

	FUNC_STATS(name);
	return true;
}

// XMLREPRESENTATION in Sybase ASE
bool SqlParser::ParseFunctionXmlrepresentation(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLROOT in Oracle
bool SqlParser::ParseFunctionXmlroot(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *version = GetNextWordToken("VERSION", L"VERSION", 7);

	if(version == NULL)
		return false;

	Token *no = GetNextWordToken("NO", L"NO", 2);
	Token *exp2 = NULL;
	Token *value = NULL;

	if(no != NULL)
	{
		value = GetNextWordToken("VALUE", L"VALUE", 5);

		if(value == NULL)
			return false;
	}
	else
	{
		exp2 = GetNextToken();

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 XMLROW
bool SqlParser::ParseFunctionXmlrow(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	while(true)
	{
		Token *expn = GetNextToken();

		if(expn == NULL)
			break;

		ParseExpression(expn);

		// As is optional
		Token *as = GetNextWordToken("AS", L"AS", 2);
		if(as != NULL)
			/*Token *identifier */ (void) GetNextToken();

		Token *comma = GetNextCharToken(',', L',');

		if(comma == NULL)
			break;
	}

	// Option is optional
	Token *option = GetNextWordToken("OPTION", L"OPTION", 6);
	if(option != NULL)
	{
		Token *as = GetNextWordToken("AS", L"AS", 2);
		/*Token *row */ (void) GetNextWordToken("ROW", L"ROW", 3);
		if(as != NULL)
			/*Token *attributes */ (void) GetNextWordToken("ATTRIBUTES", L"ATTRIBUTES", 10);
		else
			/*Token *row_name */ (void) GetNextToken();
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLSEQUENCE in Oracle
bool SqlParser::ParseFunctionXmlsequence(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	Token *fmt = NULL;

	// Fmt is optional
	if(comma != NULL)
	{
		fmt = GetNextToken();

		// Parse fmt
		ParseExpression(fmt);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLSERIALIZE in Oracle, DB2
bool SqlParser::ParseFunctionXmlserialize(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	// CONTENT is optional in DB2
	/*Token *content */ (void) GetNextWordToken("CONTENT", L"CONTENT", 7);

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	// Parse expression
	ParseExpression(exp);

	// AS must be specified in DB2, but optional in Oracle
	/*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

	Token *datatype = GetNextToken();

	// Parse datatype
	ParseDataType(datatype, SQL_SCOPE_XMLSERIALIZE_FUNC);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 XMLTEXT
bool SqlParser::ParseFunctionXmltext(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// XMLTRANSFORM in Oracle
bool SqlParser::ParseFunctionXmltransform(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *instance = GetNextToken();

	// Parse instance
	ParseExpression(instance);

	Token *comma = GetNextCharToken(',', L',');

	if(comma == NULL)
		return false;

	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 XMLVALIDATE
bool SqlParser::ParseFunctionXmlvalidate(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	/*Token *document */ (void) GetNextWordToken("DOCUMENT", L"DOCUMENT", 8);

	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	// Clause is optional
	Token *according = GetNextWordToken("ACCORDING", L"ACCORDING", 9);
	if(according != NULL)
	{
		/*Token *to */ (void) GetNextWordToken("TO", L"TO", 2);
		/*Token *xmlschema */ (void) GetNextWordToken("XMLSCHEMA", L"XMLSCHEMA", 9);

		Token *id = GetNextWordToken("ID", L"ID", 2);
		Token *uri = GetNextWordToken("URI", L"URI", 3);
		Token *no = GetNextWordToken("NO", L"NO", 2);
		if(id != NULL)
			/*Token *schema */ (void) GetNextToken();
		else
		if(uri != NULL)
		{
			/*Token *uri1 */ (void) GetNextToken();
			/*Token *uri2 */ (void) GetNextToken();
		}
		else
		if(no != NULL)
		{
			/*Token *namespace_ */ (void) GetNextWordToken("NAMESPACE", L"NAMESPACE", 9);
			/*Token *uri2 */ (void) GetNextToken();
		}

		Token *namespace2 = GetNextWordToken("NAMESPACE", L"NAMESPACE", 9);
		Token *no2 = GetNextWordToken("NO", L"NO", 2);
		if(namespace2 != NULL)
			/*Token *uri3 */ (void) GetNextToken();
		else
		if(no2 != NULL)
			/*Token *namespace3 */ (void) GetNextWordToken("NAMESPACE", L"NAMESPACE", 9);

		/*Token *element */ (void) GetNextWordToken("ELEMENT", L"ELEMENT", 7);

		Token *element_name = GetNextToken();

		if(element_name == NULL)
			return false;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to XMLISVALID
	if(_target == SQL_ORACLE)
		Token::Change(name, "XMLISVALID", L"XMLISVALID", 10);

	FUNC_STATS(name);
	return true;
}

// DB2 XMLXSROBJECTID
bool SqlParser::ParseFunctionXmlxsrobjectid(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	FUNC_STATS(name);
	return true;
}

// DB2 XSLTRANSFORM
bool SqlParser::ParseFunctionXsltransform(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *document = GetNextToken();

	if(document == NULL)
		return false;

	ParseExpression(document);

	Token *usng = GetNextWordToken("USING", L"USING", 5);

	if(usng == NULL)
		return false;

	Token *stylesheet = GetNextToken();

	if(stylesheet == NULL)
		return false;

	ParseExpression(stylesheet);

	Token *with = GetNextWordToken("WITH", L"WITH", 4);

	// WITH is optional
	if(with != NULL)
	{
		Token *param = GetNextToken();

		if(param == NULL)
		return false;

		ParseExpression(param);
	}

	// AS is optional
	Token *as = GetNextWordToken("AS", L"AS", 2);
	if(as != NULL)
	{
		Token *clob = GetNextWordToken("CLOB(2G)", L"CLOB(2G)", 8);

		if(clob == NULL)
		{
			Token *type = GetNextToken();
			ParseDataType(type, SQL_SCOPE_XMLSERIALIZE_FUNC);
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "XMLTRANSFORM", L"XMLTRANSFORM", 12);
		Token::Change(usng, ",", L",", 1);
	}

	FUNC_STATS(name);
	return true;
}

// YEAR in SQL Server, DB2, Sybase ASE, Sybase ASA
bool SqlParser::ParseFunctionYear(Token *name, Token *open)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *date = GetNextToken();

	if(date == NULL)
		return false;

	// Parse date
	ParseExpression(date);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// Convert to EXTRACT in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "EXTRACT(YEAR FROM ", L"EXTRACT(YEAR FROM ", 18);
		Token::Remove(open);

		// If date parameter is string literal enclose with TO_DATE
		if(date->type == TOKEN_STRING)
		{
			Prepend(date, "TO_DATE(", L"TO_DATE(", 8, name);
			Append(date, ")", L")", 1);
		}
	}

	FUNC_STATS(name);
	return true;
}

// Teradata ZEROIFNULL function
bool SqlParser::ParseFunctionZeroifnull(Token *name, Token* /*open*/)
{
	if(name == NULL)
		return false;

	STATS_DECL
	Token *exp = GetNextToken();

	if(exp == NULL)
		return false;

	ParseExpression(exp);

	Token *exp_end = GetLastToken();

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// NVL(exp, 0) in Oracle
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "NVL", L"NVL", 3);
		AppendNoFormat(exp_end, ", 0", L", 0", 3);
	}
	else
	// ISNULL(exp, 0) in SQL Server
	if(_target == SQL_ORACLE)
	{
		Token::Change(name, "ISNULL", L"ISNULL", 6);
		AppendNoFormat(exp_end, ", 0", L", 0", 3);
	}
	else
	// IFNULL(exp, 0) in MySQL
	if(Target(SQL_MARIADB, SQL_MYSQL))
	{
		Token::Change(name, "IFNULL", L"IFNULL", 6);
		AppendNoFormat(exp_end, ", 0", L", 0", 3);
	}
	else
	// COALESCE(exp, 0) in other databases
	if(_target != SQL_TERADATA)
	{
		Token::Change(name, "COALESCE", L"COALESCE", 8);
		AppendNoFormat(exp_end, ", 0", L", 0", 3);
	}

	name->nullable = false;

	FUNC_STATS(name);
	return true;
}