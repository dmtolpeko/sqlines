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

// Str - String operations 

#ifndef sqlines_str_h
#define sqlines_str_h

#if !defined(WIN32) && !defined(WIN64)

#include <string.h>
#include <strings.h>

#define _stricmp strcasecmp
#define _strnicmp strncasecmp

char *_strupr(char *input);
char *_strlwr(char *input);
#endif

#include <string>

class Str
{
public:
	// Skip spaces, new lines and tabs
	static char* SkipSpaces(const char *input);
	// Skip comments, spaces, new lines and tabs
	static char *SkipComments(const char *input);
	
	// Trim trailing spaces
	static void TrimTrailingSpaces(std::string &input);
	static char* TrimTrailingSpaces(char *input);

	// Skip until the specified char is met
	static const char* SkipUntil(const char *input, char ch);

	// Get copy of the input string
	static const char* GetCopy(const char *input, size_t size);
	static const char* GetCopy(const char *input);
	
	// Replace character in string (returns the same string)
	static char* ReplaceChar(char *input, const char what, const char with);

	// Is non-empty value set
	static bool IsSet(const char *in) { if(in != NULL && *in != '\x0') return true; else return false; }

	// Convert int to string
	static char* IntToString(int int_value, char *output);

	// Convert long to string
	static char* LongToString(long long_value, char *output);

	// Replace the first occurrence of a substring
	static void ReplaceFirst(std::string &str, std::string what, std::string with);

	// Quote string with single quoted, diplicate single quotes inside the string
	static void Quote(std::string &source, std::string &target);

	// Unquote identifier
	static std::string UnquoteIdent(std::string &source);

	// Get next item in list
	static char* GetNextInList(const char *input, std::string &output);

	// Convert size in bytes to string with MB, GB
	static char* FormatByteSize(double bytes, char *output);

	// Convert time in milliseconds to string with ms, sec
	static char* FormatTime(size_t time_ms, char *output);

	// Compare string representation of numbers (.5 and 0.50 etc)
	static bool	CompareNumberAsString(const char *first, int flen, const char *second, int slen);

	// Convert 2 digit date time part (century, 2 digit year, month, day, hour, minute, second) to 2 chars
	static void Dt2Ch(int dt, char *ch);
	// Convert timestamp units to string
	static void SqlTs2Str(short year, short month, short day, short hour, short minute, short second, long fraction, char *out);
	// Convert 7-byte packed Oracle DATE to string (non-null terminated, exactly 19 characters)
	static void OraDate2Str(unsigned char *ora_date, char *out);

	// Get integer or fractional part of number in string
	static const char* GetIntegerPart(const char *str, int slen, char *out, int *out_len);
	static void GetFractionPartLen(const char *str, int slen, int *out_len);

	// Trim string to the specified length by replacing words with abbreviations
	static void TrimToAbbr(std::string &input, std::string &output, int max_len);
};

#endif // sqlines_str_h
