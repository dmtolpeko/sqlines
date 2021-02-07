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

// SQLParser Library entry point

#ifdef WIN32
#include <windows.h>
#endif

#include "sqlparser.h"

void* CreateParserObject()
{
	return new SqlParser();
}

void SetParserTypes(void *parser, short source, short target)
{
	if(parser == NULL)
		return;

	SqlParser *sql_parser = (SqlParser*)parser;

	// Run conversion
	sql_parser->SetTypes(source, target);
}

int ConvertSql(void *parser, const char *input, int size, const char **output, int *out_size, int *lines)
{
	if(parser == NULL)
		return -1;

	SqlParser *sql_parser = (SqlParser*)parser;

	// Run conversion
	sql_parser->Convert(input, size, output, out_size, lines);

	return 0;
}

int SetParserOption(void *parser, const char *option, const char *value)
{
	if(parser == NULL)
		return -1;

	SqlParser *sql_parser = (SqlParser*)parser;

	sql_parser->SetOption(option, value);

	return 0;
}

// Create report file
int CreateAssessmentReport(void *parser, const char *summary)
{
	if(parser == NULL)
		return -1;

	SqlParser *sql_parser = (SqlParser*)parser;

	return sql_parser->CreateReport(summary);
}

// Free allocated result
void FreeOutput(const char *output)
{
	delete [] output;
}

#ifdef WIN32

BOOL APIENTRY DllMain( HMODULE /*hModule*/, DWORD  ul_reason_for_call, LPVOID /*lpReserved*/)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}

#endif
