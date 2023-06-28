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

#ifndef WIN32

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
    
    // Get substring until the specified char is met
	static void GetSubtringUntil(const char *input, char ch, int len, std::string &output);

	// Get copy of the input string
	static char* GetCopy(const char *input, int size);
	static char* GetCopy(const char *input);

	// Replace character in string (returns the same string)
	static char* ReplaceChar(char *input, const char what, const char with);

	// Convert int to string
	static char* IntToString(int int_value, char *output);

	// Replace the first occurrence of a substring
	static void ReplaceFirst(std::string &str, std::string what, std::string with);

	// Get next item in list
	static char* GetNextInList(const char *input, std::string &output);

	// Convert size in bytes to string with MB, GB
	static char* FormatByteSize(double bytes, char *output);

	// Convert time in milliseconds to string with ms, sec
	static char* FormatTime(int time_ms, char *output);

	// Convert 2 digit date time part (century, 2 digit year, month, day, hour, minute, second) to 2 chars
	static void Dt2Ch(int dt, char *ch);
};

#endif // sqlines_str_h