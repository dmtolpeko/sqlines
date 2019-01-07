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
#include "str.h"
#include "file.h"
#include "os.h"

// Constructor
Parameters::Parameters()
{
	_cfg_file = NULL;
	_cfg_read_ok = false;
	*_cfg_read_err = '\x0';
}

// Return the value of the parameter if it is set to True or Yes
const char* Parameters::GetTrue(const char *name)
{
	const char *value = Get(name);

	if(value != NULL && *value != '\x0' && 
		(_stricmp(value, "yes") == 0 || _stricmp(value, "true") == 0))
		return value;

	return NULL;
}

// Return the value of the parameter if it is set to False or No
const char* Parameters::GetFalse(const char *name)
{
	const char *value = Get(name);

	if(value != NULL && *value != '\x0' && 
		(_stricmp(value, "no") == 0 || _stricmp(value, "false") == 0))
		return value;

	return NULL;
}

// Get integer value or the default
int Parameters::GetInt(const char *name, int def)
{
	const char *value = Get(name);

	if(Str::IsSet(value))
	{
		int num = def;
        sscanf(value, "%d", &num);

        if(num != def)
			return num;
	}

	return def;
}

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
	rc = LoadStr(comline.c_str());

	return rc;
}

// Load parameters from string and configuration file
int Parameters::LoadStr(const char *input)
{
	// NULL input is allowed to read from configuration file only

	// Delete all current parameters
	_map.clear();

	// Load from string
	if(input != NULL)
		LoadStrItem(input);

	// Load from configuration file
	LoadConfigFile();

	return 0;
}

// Load parameters from a string only
int Parameters::LoadStrItem(const char *input)
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
		while(*cur && *cur != '=' && *cur != '\r' && *cur != '\n' && *cur != '\t')
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
			// if - is faced, previuos must be space, next mustn't be space (to allow - in directory path, host names etc.)
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

// Load parameters from a configuration file
int Parameters::LoadConfigFile()
{
	const char *file = Get("-cfg");

	_cfg_read_ok = false;
	*_cfg_read_err = '\x0';

	// Use the default file name
	if(file != NULL)
		_cfg_file = file;

	if(_cfg_file == NULL)
		return -1;

	// Configuration file size
	int64_t size = File::GetFileSize(_cfg_file);

	if(size == -1)
	{
		Os::GetLastErrorText(NULL, _cfg_read_err, CFG_ERROR_LEN);
		return -1;
	}
 
	char *input = new char[size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(_cfg_file, input, static_cast<unsigned int>(size)) == -1)
	{
		Os::GetLastErrorText(NULL, _cfg_read_err, CFG_ERROR_LEN);
		delete[] input;
		return -1;
	}

	input[size] = '\x0';

	char *cur = input;

	// Process input
	while(*cur)
	{
		cur = Str::SkipComments(cur);

		if(*cur == '\x0')
			break;

		std::string name;
		std::string value;

		// Get the parameter name until = or space
		while(*cur && *cur != '=' && *cur != ' ' && *cur != '\t' && *cur != '\n')
		{
			name += *cur;
			cur++;
		}

		cur = Str::SkipSpaces(cur);

		if(*cur == '=')
			cur++;

		// Skip only spaces and tabs (newline means no value set)
		while(*cur && (*cur == ' ' || *cur == '\t'))
			cur++;

		// Get the parameter value until new line or tab
		while(*cur && *cur != '\r' && *cur != '\n' && *cur != '\t')
		{
			value += *cur;
			cur++;
		}

		Str::TrimTrailingSpaces(value);

		// In configuration file, if the value is not set skip the parameter
		if(value.empty())
			continue;

		// Parameter names are case-insensitive, convert to lower case
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);

		// Don't add parameter if it already exists (give priority to command line and parameters defined first)
		if(_map.count(name) == 0)
		{
			_map.insert(ParametersPair(name, value));
		}
	}

	_cfg_read_ok = true;
	delete[] input;

	return 0;
}

// Get the value by key
const char* Parameters::Get(const char *key)
{
	if(key == NULL)
		return NULL;

	std::string skey = key;

	// Find the value
	ParametersMap::iterator i = _map.find(skey);

	// Key not found
	if(i == _map.end())
		return NULL;

	return i->second.c_str();
}
