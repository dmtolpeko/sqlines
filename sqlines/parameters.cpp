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

// Parameters class - Loads parameters from command line  

#include <stdio.h> 
#include <string.h>
#include <string>
#include <algorithm>
#include "parameters.h"
#include "../sqlcommon/str.h"

// Process parameters in command line
int Parameters::Load(int argc, char** argv)
{
	// Load command line parameters
	int rc = LoadCommandLine(argc, argv);

	return rc;
}

// Load command line parameters
int Parameters::LoadCommandLine(int argc, char** argv)
{
	int rc = 0;

	// Check if any parameters are specified
	if(argc == 1 || argv == NULL)
		return -1;

	std::string comline;

	// Merge all command line parameters into a single string
	for(int i = 1; i < argc; i++)
	{
		comline += argv[i];
		comline += " ";
	}

	// Load parameters from the command line string
	rc = LoadString(comline.c_str());

	return rc;
}

// Load parameters from string
int Parameters::LoadString(const char *input)
{
	if(input == NULL)
		return -1;

	const char *cur = input;

	// Process parameters
	while(*cur)
	{
		std::string name;
		std::string value;

		cur = Str::SkipSpaces(cur);

		// Check for help option specified as ? or /?
		if(*cur == '?' || (*cur == '-' && cur[1] == '?'))
		{
			// Add help option and ignore all remaining options
			_map.insert(ParametersPair(HELP_PARAMETER, "yes"));
			break;
		}

		// Get the parameter name until = or new line
		while(*cur && *cur != '=' && *cur && '\r' && *cur != '\n' && *cur != '\t')
		{
			// Check for parameter without value
			if(*cur == PARAMETER_START_TOKEN && name.empty() == false)
				break;

			name += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(name);

		if(*cur == '=')
			cur++;

		cur = Str::SkipSpaces(cur);

		// Get the parameter value until - or new line
		while(*cur && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			// if - is faced, previuos must be space, next mustn't be space (to allow - in directory path)
			if(*cur == PARAMETER_START_TOKEN && cur[-1] == ' ' && cur[1] != ' ')
				break;
			
			value += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(value);

		// Parameter names are case-insensitive, convert to lower case
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);

		// Don't add parameter if it already exists (give priority to command line and parameters defined first)
		if(_map.count(name) == 0)
		{
			_map.insert(ParametersPair(name, value));
		}
	}

	return 0;
}

// Get the value by key
char* Parameters::Get(const char *key)
{
	if(key == NULL)
		return NULL;

	std::string skey = key;

	// Find the value
	ParametersMap::iterator i = _map.find(skey);

	// Key not found
	if(i == _map.end())
		return NULL;

	return (char*)i->second.c_str();
}
