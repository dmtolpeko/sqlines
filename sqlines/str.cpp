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

	return (char*)cur;
}

// Skip comments, spaces, new lines and tabs
char* Str::SkipComments(const char *input)
{
	if(input == NULL)
		return NULL;

	char *cur = (char*)input;

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

	return cur;
}

// Trim trailing spaces
void Str::TrimTrailingSpaces(std::string &input)
{
	// Get the string length
	int len = input.length();
	int nlen = len;

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

	int len = strlen(input);
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

	return (char*)cur;
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

// Get substring until the specified char is met
void Str::GetSubtringUntil(const char *input, char ch, int len, std::string &output)
{
	unsigned int pos = 0;

	do
	{
		if(input[pos] == ch)
		{
			output.assign(input, pos);
			break;
		}

		pos++;

	} while(pos < len);
}

// Get copy of the input string
char* Str::GetCopy(const char *input, int size)
{
	if(input == NULL)
		return NULL;

	char *output = new char[size + 1];

	// Use memcpy as data can contain '\x0' for Unicode
	memcpy(output, input, size);
	output[size] = '\x0';

	return output;
}

char* Str::GetCopy(const char *input)
{
	if(input == NULL)
		return NULL;
	
	int size = strlen(input);

	return Str::GetCopy(input, size);
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

#ifdef WIN32
	_itoa_s(int_value, output, 11, 10);
#else
	sprintf(output, "%d", int_value);
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
char* Str::FormatTime(int time_ms, char *output)
{
	if(output == NULL)
		return NULL;

	if(time_ms < 1000)
		sprintf(output, "%d ms", time_ms);
	else
	if(time_ms < 1000 * 60)
		sprintf(output, "%0.1lf sec", ((double)time_ms)/1000);
	else
		sprintf(output, "%d min %d sec", time_ms/60/1000, (time_ms % (60 * 1000))/1000);

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

	ch[0] = '0' + f;
	ch[1] = '0' + s;
}

// _strupr() and _strlwr() functions are not available on Linux
#ifndef WIN32

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
