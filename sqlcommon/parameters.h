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

#ifndef sqlines_parameters_h
#define sqlines_parameters_h

#include <map>

#define PARAMETER_START_TOKEN	'-'				

// Option to print help on available parameters (? is also suported)
#define HELP_PARAMETER			"-?"				

typedef std::map<std::string, std::string> ParametersMap;
typedef std::pair<std::string, std::string> ParametersPair;

#define CFG_ERROR_LEN		1024

//////////////////////////////////////////////////////////////////////

class Parameters
{
	// Key-value map of parameters
	ParametersMap _map;

	// Default configuration file
	const char *_cfg_file;

	// Status for configuration file reading
	bool _cfg_read_ok;
	char _cfg_read_err[CFG_ERROR_LEN];

public:
	Parameters();

	// Return the value of the parameter if it is set to True/Yes, or False/No
	const char* GetTrue(const char *name);
	const char* GetFalse(const char *name);

	// Get integer value or the default
	int GetInt(const char *name, int def);

	// Load parameters from command line 
	int Load(int argc, char** argv);
	// Load parameters from a string and configuration file
	int LoadStr(const char *input);

	// Get the value by key
	const char* Get(const char *key);

	// Set the default configuration file
	void SetConfigFile(const char *file) { _cfg_file = file; }
	const char *GetConfigFile() { return _cfg_file; }

	// Get status for configuration file reading
	bool IsCfgReadOk() { return _cfg_read_ok; }
	const char* GetCfgReadError() { return _cfg_read_err; }

	ParametersMap& GetMap() { return _map; }

private:
	// Load command line parameters
	int LoadCommandLine(int argc, char** argv);
	// Load parameters from a string only
	int LoadStrItem(const char *input);
	// Load parameters from a configuration file
	int LoadConfigFile();
};

#endif // sqlines_parameters_h
