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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "str.h"

// Skip spaces, new lines and tabs
char* Str::SkipSpaces(const char *input)
{
	if(input == NULL)
		return NULL;

	const char *cur = input;

	while(*cur)
	{
		if(*cur != L' ' && *cur != L'\r' && *cur != L'\n' && *cur != L'\t')
			break;

		cur++;
	}

	return const_cast<char*>(cur);
}

// Skip comments, spaces, new lines and tabs
char* Str::SkipComments(const char *input)
{
	if(input == NULL)
		return NULL;

	const char *cur = input;

	while(*cur)
	{
		cur = Str::SkipSpaces(cur);

		// Single line comment
		if((*cur == '-' && cur[1] == '-') || (*cur == '/' && cur[1] == '/'))
		{
			while(*cur && *cur != '\n')
				cur++;
		}
		else
			break;
	}

	return const_cast<char*>(cur);
}

// Trim trailing spaces
void Str::TrimTrailingSpaces(std::string &input)
{
	// Get the string length
	size_t len = input.length();
	size_t nlen = len;

	if(len <= 0)
		return;

	// Calculate the number of spaces at the end
	while(nlen && input[nlen-1] == L' ')
		nlen--;

	if(nlen < len)
		input.erase(nlen);
}

// Trim trailing spaces
char* Str::TrimTrailingSpaces(char *input)
{
	if(!input)
		return NULL;

	size_t len = strlen(input);
	char *cur = input;

	while(len > 0)
	{
		if(cur[len-1] == ' ')
			cur[len-1] = '\x0';
		else
			break;

		len--;
	}

	return input;
}

// Get next item in list
char* Str::GetNextInList(const char *input, std::string &output)
{
	if(input == NULL)
		return NULL;

	const char *cur = input;

	cur = Str::SkipSpaces(cur);

	// Remove list delimiters
	while(*cur && (*cur == L',' || *cur == L';'))
		cur++;

	cur = Str::SkipSpaces(cur);

	// Copy data until the next delimiter
	while(*cur && *cur != L',' && *cur != L';' && *cur != L')')
	{
		output += *cur;
		cur++;
	}

	Str::TrimTrailingSpaces(output);

	return const_cast<char*>(cur);
}

// Skip until the specified char is met
const char* Str::SkipUntil(const char *input, char ch)
{
	if(input == NULL)
		return NULL;

	const char *cur = input;

	while(*cur && *cur != ch)
		cur++;

	return cur;
}

// Get copy of the input string
const char* Str::GetCopy(const char *input, size_t size)
{
	if(input == NULL)
		return NULL;

	char *output = new char[(size_t)size + 1];

	// Use memcpy as data can contain '\x0' for Unicode
	memcpy(output, input, size);
	output[size] = '\x0';

	return output;
}

// Get copy of the input string
const char* Str::GetCopy(const char *input)
{
	if(input == NULL)
		return NULL;

	return Str::GetCopy(input, strlen(input));
}

// Replace character in string (returns the same string)
char* Str::ReplaceChar(char *input, const char what, const char with)
{
	if(!input)
		return NULL;

	char *cur = input;

	while(*cur)
	{
		if(*cur == what)
			*cur = with;

		cur++;
	}

	return input;
}

// Convert int to string
char* Str::IntToString(int int_value, char *output)
{
	if(output == NULL)
		return NULL;

	*output = '\x0';

#if defined(WIN32) || defined(_WIN64)
	_itoa_s(int_value, output, 11, 10);
#else
	snprintf(output, 11, "%d", int_value);
#endif

	return output;
}

// Convert long to string
char* Str::LongToString(long long_value, char *output)
{
	if(output == NULL)
		return NULL;

	*output = '\x0';

#if defined(WIN32) || defined(_WIN64)
	_ltoa_s(long_value, output, 11, 10);
#else
	snprintf(output, 11, "%ld", long_value);
#endif

	return output;
}

// Replace the first occurrence of a substring
void Str::ReplaceFirst(std::string &str, std::string what, std::string with)
{
	// Find the substring starting from the first position of the original string
	size_t pos = str.find(what, 0);

	if(pos != std::string::npos)
		str.replace(pos, what.size(), with);
}

// Quote string with single quoted, diplicate single quotes inside the string
void Str::Quote(std::string &source, std::string &target)
{
	target = '\'';

	for(std::string::iterator i = source.begin(); i != source.end(); i++) 
	{
		target += (*i);

		// Duplicate single quote
		if((*i) == '\'')
			target += (*i);
	}

	target += '\'';
}

// Unquote identifier
std::string Str::UnquoteIdent(std::string &source) 
{
	size_t cnt = source.size();

	if(cnt <= 2)
		return source;

	char s = source.at(0);

	if(s == '"' || s == '[' || s == '[')
		return source.substr(1, cnt - 2);

	return source;
}

// Convert size in bytes to string with MB, GB
char* Str::FormatByteSize(double bytes, char *output)
{
	if(output == NULL)
		return NULL;

	// Output size in bytes
	if(bytes <= 1024)
		sprintf(output, "%0.0lf bytes", bytes);
	else
	// Output size in KB
	if(bytes > 1024 && bytes <= 1024 * 1024)
		sprintf(output, "%0.1lf KB", ((double)bytes)/1024.0);
	else
	// Output size in MB
	if(bytes > 1024 * 1024 && bytes <= 1024 * 1024 * 1024)
		sprintf(output, "%0.1lf MB", ((double)bytes)/1024.0/1024.0);
	else
	// Output size in GB
	sprintf(output, "%0.1lf GB", ((double)bytes)/1024.0/1024.0/1024.0);

	return output;
}

// Convert time in milliseconds to string with ms, sec
char* Str::FormatTime(size_t time_ms, char *output)
{
	if(output == NULL)
		return NULL;

	if(time_ms < 1000)
		sprintf(output, "%zu ms", time_ms);
	else
	if(time_ms < 1000 * 60)
		sprintf(output, "%0.1lf sec", ((double)time_ms)/1000);
	else
		sprintf(output, "%lu min %lu sec", time_ms/60/1000, (time_ms % (60 * 1000))/1000);

	return output;
}

// Convert 2 digit date time part (century, 2 digit year, month, day, hour, minute, second) to 2 chars
void Str::Dt2Ch(int dt, char *ch)
{
	if(ch == NULL)
		return;

	// Get the first and second digits
	int f = dt/10;
	int s = dt%10;

	ch[0] = (char)('0' + f);
	ch[1] = (char)('0' + s);
}

// Convert SQL_TIMESTAMP_STRUCT to string 'YYYY-MM-DD HH:MI:SS.FFFFFF'
void Str::SqlTs2Str(short year, short month, short day, short hour, short minute, short second, 
						long fraction, char *out)
{
	if(out == NULL)
		return;

	short y1 = year/1000;
	short y2 = (year%1000)/100;
	short y3 = (year%100)/10;
	short y4 = year%10;

	short m1 = month/10;
	short m2 = month%10;

	short d1 = day/10;
	short d2 = day%10;

	short h1 = hour/10;
	short h2 = hour%10;

	short mi1 = minute/10;
	short mi2 = minute%10;

	short s1 = second/10;
	short s2 = second%10;

	out[0] = char('0' + y1);
	out[1] = char('0' + y2);
	out[2] = char('0' + y3);
	out[3] = char('0' + y4);
	out[4] = '-';
    out[5] = char('0' + m1);
	out[6] = char('0' + m2);
	out[7] = '-';
    out[8] = char('0' + d1);
	out[9] = char('0' + d2);
	out[10] = ' ';
    out[11] = char('0' + h1);
	out[12] = char('0' + h2);
	out[13] = ':';
    out[14] = char('0' + mi1);
	out[15] = char('0' + mi2);
	out[16] = ':';
    out[17] = char('0' + s1);
	out[18] = char('0' + s2);
	out[19] = '.';

	if(fraction == 0 || fraction >= 1000000)
	{
		out[20] = out[21] = out[22] = out[23] = out[24] = out[25] = '0';
		out[26] = '\x0';
	}
	else
		sprintf(out + 20, "%06ld", fraction);		// puts terminating 0
}

// Convert 7-byte packed Oracle DATE to string (non-null terminated, exactly 19 characters)
void Str::OraDate2Str(unsigned char *ora_date, char *out)
{
	if(ora_date == NULL || out == NULL)
		return;

	// Unsigned required for proper conversion to int
	int c = ((int)ora_date[0]) - 100;
	int y = ((int)ora_date[1]) - 100;
	int m = (int)ora_date[2];
	int d = (int)ora_date[3];
	int h = ((int)ora_date[4]) - 1;
	int mi = ((int)ora_date[5]) - 1;
	int s = ((int)ora_date[6]) - 1;

	// Get string representation
	Str::Dt2Ch(c, out);
	Str::Dt2Ch(y, out + 2);
	out[4] = '-';
	Str::Dt2Ch(m, out + 5);
	out[7] = '-';
	Str::Dt2Ch(d, out + 8);
	out[10] = ' ';
	Str::Dt2Ch(h, out + 11);
	out[13] = ':';
	Str::Dt2Ch(mi, out + 14);
	out[16] = ':';
	Str::Dt2Ch(s, out + 17);
}

// Compare string representation of numbers (.5 and 0.50 etc)
bool Str::CompareNumberAsString(const char *first, int flen, const char *second, int slen)
{
	if(first == NULL || second == NULL || flen > 127 || slen > 127)
		return false;

	char part1[128];
	char part2[128];

	int len1 = 0;
	int len2 = 0;

	// Get integer parts of the first and second number
	const char *cur1 = GetIntegerPart(first, flen, part1, &len1);
	const char *cur2 = GetIntegerPart(second, slen, part2, &len2);

	// Integer part not equal
	if(len1 != len2)
		return false;

	// Do not compare zero integers (0.123 or .123)
	if(len1 != 0 && len2 != 0 && memcmp(part1, part2, (size_t)len1) != 0)
		return false;

	len1 = 0;
	len2 = 0;

	// Get lengths of fractions of the first and second number
	GetFractionPartLen(cur1, (int)(flen - (cur1 - first)), &len1);
	GetFractionPartLen(cur2, (int)(slen - (cur2 - second)), &len2);

	// Fractions not equal
	if(len1 != len2)
		return false;

	// Do not compare zero fractions (123.0 or 123.)
	if(len1 != 0 && len2 != 0 && memcmp(cur1, cur2, (size_t)len1) != 0)
		return false;

	return true;
}

// Get integer part of number in string
const char* Str::GetIntegerPart(const char *str, int slen, char *out, int *out_len)
{
	if(str == NULL || out == NULL)
		return NULL;

	int i = 0;
	int len = 0;

	// Skip leading zeros
	while(i < slen && str[i] == '0')
		i++;

	// Copy digits until .
	while(i < slen && str[i] != '.')
	{
		out[len] = str[i];
		i++;
		len++;
	}

	// Skip .
	if(str[i] == '.')
		i++;

	if(out_len != NULL)
		*out_len = len;
		
	return str + i;
}

// Get fractional part of number in string
void Str::GetFractionPartLen(const char *str, int slen, int *out_len)
{
	if(str == NULL || slen == 0)
		return;

	int len = slen;

	// Skip trailing zeros
	while(len >= 1 && str[len - 1] == '0')
		len--;

	if(out_len != NULL)
		*out_len = len;
}

// Abbreviations
struct abbreviations
{
	const char *word;
	const char *word_up;
	int len;
	const char *abbr;
	const char *abbr_up;
};

// List must be ASC ordered for correct search
struct abbreviations abbr[] = 
{
	{ "active", "ACTIVE", 6, "act", "ACT" },
	{ "adjust", "ADJUST", 6, "adj", "ADJ" },
	{ "average", "AVERAGE", 7, "avg", "AVG" },
	{ "close", "CLOSE", 5, "cl", "CL" },
	{ "date", "DATE", 4, "dt", "DT" },
	{ "image", "IMAGE", 5, "img", "IMG" },
	{ "inactive", "INACTIVE", 8, "inact", "INACT" },
	{ "last", "LAST", 4, "lst", "LST" },
	{ "length", "LENGTH", 6, "len", "LEN" },
	{ "office", "OFFICE", 6, "offc", "offc" },
	{ "profile", "PROFILE", 7, "prof", "PROF" },
	{ "ratio", "RATIO", 5, "ro", "RO" },
	{ "require", "REQUIRE", 7, "req", "REQ" },
	{ "time", "TIME", 4, "tm", "TM" },
	{ "washington", "WASHINGTON", 10, "wa", "WA" },
	{ NULL, NULL, 0, NULL, NULL }
};

const char *words_remove[] = { "been", "by", "has", "of", "to", NULL };

// Trim string to the specified length by replacing words with abbreviations
// Abbreviations in PeopleSoft - http://www.acs.utah.edu/acs/qa_standards/psstd02a.htm

void Str::TrimToAbbr(std::string &input, std::string &output, int max_len)
{
	const char *cur = input.c_str();

	while(*cur)
	{
		const char *end = cur;

		// Select the next word
		while(*end && *end != '_')
			end++;

		int i = 0;
		bool removed = false;

		// Check is word needs to be removed
		while(words_remove[i] != NULL)
		{
			#ifdef WIN32
			if(_strnicmp(words_remove[i], cur, end - cur) == 0)
			#else
			if(strncasecmp(words_remove[i], cur, end - cur) == 0)
			#endif
			{
				// Skip following word delimiter
				if(*end)
					end++;

				removed = true;
				cur = end;

				break;
			}

			i++;
		}

		if(removed)
			continue;

		const char *ab = NULL;
		i = 0;

		// Find a abbreviation for the word
		while(abbr[i].word != NULL)
		{
			int c = strncmp(abbr[i].word, cur, (size_t)abbr[i].len);

			if(c == 0)
			{
				ab = abbr[i].abbr;
				break;
			}
			else
			if(c > 0)
				break;

			// Compare without case now (upper letters go before lower in ASCII table)
			#ifdef WIN32
			c = _strnicmp(abbr[i].word_up, cur, abbr[i].len);
			#else
			c = strncasecmp(abbr[i].word_up, cur, abbr[i].len);
			#endif

			if(c == 0)
			{
				ab = abbr[i].abbr_up;
				break;
			}
			else
			if(c > 0)
				break;

			i++;
		}

		// Use abbreviation if found, otherwise original word
		if(ab != NULL)
			output += ab;
		else
			output.append(cur, (size_t)(end - cur));

		// Add word delimiter
		if(*end)
		{
			output += *end;
			end++;
		}

		// Exit if the size is enough
		if(output.size() + strlen(end) <= (unsigned)max_len)
		{
			output += end;
			break;
		}

		cur = end;
	}

	// Truncate to the maximum length if still longer
	if(output.size() > (unsigned int)max_len)
		output.resize((size_t)max_len);
}

// _strupr() and _strlwr() functions are not available on Linux
#if !defined(WIN32) && !defined(_WIN64)

char *_strupr(char *input)
{
  if(!input)
   return NULL;

  char *cur = input;

  while(*cur)
  {
    *cur = (char)toupper((int)*cur);
	cur++;
  }

  return input;
}

char *_strlwr(char *input)
{
  if(!input)
   return NULL;

  char *cur = input;

  while(*cur)
  {
    *cur = (char)tolower((int)*cur);
	cur++;
  }

  return input;
}

#endif
