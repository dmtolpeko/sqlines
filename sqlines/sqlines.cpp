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

// SQLines Class

#ifdef WIN32
#include <windows.h>
#include <conio.h>
#endif

#include <stdio.h>
#include <string.h>
#include "sqlines.h"
#include "filelist.h"
#include "file.h"
#include "sqlparserexp.h"
#include "os.h"
#include "str.h"

// Constructor/destructor
Sqlines::Sqlines()
{
	_parser = CreateParserObject();
	_total_files = 0;

    _a = false;
	_stdin = false;
    
	_exe = NULL;
}

// Run the tool with command line parameters
int Sqlines::Run(int argc, char** argv)
{
	_total_files = 0;

	_license.Set(argv[0]);

	// Read and validate parameters
	int rc = SetParameters(argc, argv);

	// Command line or parameters error
	if(rc == -1)
		return rc;

    if(_stdin == false)
	    rc = ProcessFiles();
	else
	    rc = ProcessStdin();

	return rc;
}

// Perform operations on files
int Sqlines::ProcessFiles()
{
	FileList fileList;

	PrintCurrentTimestamp();

	// Load the list of files
	int rc = fileList.Load(_in.c_str());

	char total_size_fmt[21];
	Str::FormatByteSize(fileList.GetSize(), total_size_fmt);

	_total_files = fileList.Get().size();

	_log.Log("\n\nSource file%s: %d (%s)\n", SUFFIX(_total_files), _total_files, total_size_fmt);

	int num = 1;
	int total_lines = 0;

	int all_start = Os::GetTickCount();

	// Handle each file
	for(std::list<std::string>::iterator i = fileList.Get().begin(); i != fileList.Get().end(); i++, num++)
	{
		int start = Os::GetTickCount();

		std::string current = *i;
		std::string relative_name = File::GetRelativeName(_in.c_str(), current.c_str()); 

		_log.Log("\n%5d. %s", num, relative_name.c_str()); 

		std::string out_name = GetOutFileName(*i, relative_name);

		int in_size = 0;
		int in_lines = 0;

		SetParserOption(_parser, SQLINES_CURRENT_FILE, relative_name.c_str());

		// Convert the current file
	    rc = ProcessFile(current, out_name, &in_size, &in_lines);

		total_lines += in_lines;

		int end = Os::GetTickCount() - start;

		char time_fmt[21];
		char size_fmt[21];

		Str::FormatTime(end, time_fmt);
		Str::FormatByteSize(in_size, size_fmt);

		_log.Log("...Ok (%s, %d line%s, %s)", size_fmt, in_lines, SUFFIX(in_lines), time_fmt); 
	}

	char total_time_fmt[21];
	Str::FormatTime(Os::GetTickCount() - all_start, total_time_fmt);

    if(_total_files > 0)
    {
        char summary[1024];
        sprintf(summary, "\n\nTotal: %d file%s, %s, %d line%s, %s", _total_files, SUFFIX(_total_files), 
            total_size_fmt, total_lines, SUFFIX(total_lines), total_time_fmt);

		_log.Log("%s", summary);

        if(_a)
        {
			std::string report_summary = SQLINES_VERSION;
			report_summary += "<br><br>"; 
			report_summary += summary;

            _log.Log("\n\nCreating assessment report");
			CreateAssessmentReport(_parser, report_summary.c_str());
        }
    }

	PrintCurrentTimestamp();

	return rc;
}

// Get output name of the file
std::string Sqlines::GetOutFileName(std::string &input, std::string &relative_name)
{
	std::string output;

	// If -out option not set, add "_out" to the file name
	if(_out.empty() == true)
	{
		std::string dir;
		std::string file;

		// Get the input directory and file name
		File::SplitDirectoryAndFile(input.c_str(), dir, file);

		if(dir.empty() == false)
		{
			output = dir;
			output += DIR_SEPARATOR_CHAR;
		}

		// When there is no extension pos points to '\x0'
		int pos = File::GetExtensionPosition(file.c_str());

		std::string out_file;

		if(pos > 0)
			out_file.assign(file, 0, pos);

		// Add default postfix
		out_file += "_out";

		// Extension exists (.at(pos) throws an exception when pos point to 0)
		if(pos > 0 && (file.c_str())[pos] != '\x0')
			out_file.append(file.c_str() + pos);

		output += out_file;
	}
	// -out option is set
	else
	{
		int len = _out.size();

		// If multiple files are converted, -out must specify the directory; also if path terminates with \ the directory is specified
		if(_total_files > 1 || (len > 0 && _out[len-1] == DIR_SEPARATOR_CHAR))
		{
			// Get file name (relative name can include a subdirectory)
			File::GetPathFromDirectoryAndFile(output, _out.c_str(), relative_name.c_str());

			std::string dir;
			std::string file;

			File::SplitDirectoryAndFile(output.c_str(), dir, file);

			// Create target directories
			File::CreateDirectories(dir.c_str());
		}
		// If single file is converted, -out must specify the file name (with or without directory)
		else
			output = _out;
	}

	return output;
}

// Process a file
int Sqlines::ProcessFile(std::string &file, std::string &out_file, int *in_size, int *in_lines)
{
	if(_parser == NULL)
		return -1;

	int size = File::GetFileSize(file.c_str());

	if(size <= 0)
		return -1;

	// Allocate a buffer for the file content
	char *input = new char[size];

	// Get content of the file (without terminating 'x0')
	int rc = File::GetContent(file.c_str(), input, size);

	if(rc == -1)
	{
		delete input;
		return -1;
	}

	const char *output = NULL;
	int out_size = 0;
	int lines;

	// Convert the file
    rc = ConvertSql(_parser, input, size, &output, &out_size, &lines);

    // Write the target content to the file
    rc = File::Write(out_file.c_str(), output, out_size);

    FreeOutput(output);

	if(in_size != NULL)
		*in_size = size;

	if(in_lines != NULL)
		*in_lines = lines;

	delete input;

	return rc;
}

// Convert input from STDIN
int Sqlines::ProcessStdin()
{
	if(_parser == NULL)
		return -1;

	std::string in;

	char c = '\x0';

	// Read STDIN
	while((c = getchar()) != EOF)
		in += c;

	const char *output = NULL;
	int out_size = 0;
	int lines;

	// Convert the input
	int rc = ConvertSql(_parser, in.c_str(), in.size(), &output, &out_size, &lines);

	// Write the target content to STDOUT
	if(output != NULL)
	{
		printf("%s", output);
		fflush(stdout);
	}

	FreeOutput(output);
	
	return rc;
}

// Read and validate parameters
int Sqlines::SetParameters(int argc, char **argv)
{
	if(argv != NULL)
		_exe = argv[0];

	// Fill parameters map
	int rc = _parameters.Load(argc, argv);

	// Get -stdin option
	char *value = _parameters.Get(STDIN_OPTION);

	if(value != NULL)
		_stdin = true;

	// Get -log option
	value = _parameters.Get(LOG_OPTION);

	if(value != NULL)
		_logfile = value;
	else
		_logfile = SQLINES_LOGFILE;

	// Set the directory and name of log file
	_log.SetLogfile(_logfile.c_str());

	if(_stdin == false)
	{
		_log.Log("\n%s\n%s", SQLINES_VERSION, SQLINES_COPYRIGHT);

		// Show the license message if required
		if(_license.IsLicenseCheckRequired())
		{
			if(_license.IsEmpty())
			{
				_log.Log("\n\nThe product is FOR EVALUATION USE ONLY.");
				SetParserOption(_parser, SQLINES_EVAL_MODE, "TRUE");
			}
			else
				_log.Log("\n\nThe product is licensed to %s.", _license.GetName().c_str());
		}
	}

	// Get -s option
	value = _parameters.Get(S_OPTION);

	if(value != NULL)
		_s = value;

	// Get -t option
	value = _parameters.Get(T_OPTION);

	if(value != NULL)
		_t = value;

	// Get -in option
	value = _parameters.Get(IN_OPTION);

	if(value != NULL)
		_in = value;

	// Get -out option
	value = _parameters.Get(OUT_OPTION);

	if(value != NULL)
		_out = value;

    // Get -a option
	value = _parameters.Get(A_OPTION);

	if(value != NULL)
		_a = true;

	if(_parameters.Get(HELP_PARAMETER))
	{
		PrintHowToUse();
		return -1;
	}

	// Check mandatory options
	if(_s.empty() == true || _t.empty() == true || (_in.empty() == true && _stdin == false))
	{
		_log.Log("\n\nCommand line error:\n");

		if(_s.empty() == true)
			_log.Log("\n   -s option not set");

		if(_t.empty() == true)
			_log.Log("\n   -t option not set");

		if(_in.empty() == true)
			_log.Log("\n   -in option not set");

		PrintHowToUse();
		return -1;
	}

	SetTypes();
	SetOptions();

	return rc;
}

// Set source and target types
void Sqlines::SetTypes()
{
	int source = DefineType(_s.c_str());
	int target = DefineType(_t.c_str());

	SetParserTypes(_parser, source, target);
}

// Set conversion options
void Sqlines::SetOptions()
{
	ParametersMap &map = _parameters.GetMap();

	for(ParametersMap::iterator i = map.begin(); i != map.end(); ++i)
		SetParserOption(_parser, i->first.c_str(), i->second.c_str());
}

// Define SQL dialect type by name
short Sqlines::DefineType(const char *name)
{
	if(name == NULL)
		return -1;

	short type = 0;

	if(_stricmp(name, "sql") == 0)
		type = SQL_SQL_SERVER;
	else
	if(_stricmp(name, "oracle") == 0)
		type = SQL_ORACLE;
	else
	if(_stricmp(name, "db2") == 0)
		type = SQL_DB2;
	else
	if(_stricmp(name, "mysql") == 0)
		type = SQL_MYSQL;
	else
	if(_stricmp(name, "postgresql") == 0)
		type = SQL_POSTGRESQL;
	else
	if(_stricmp(name, "sybase") == 0)
		type = SQL_SYBASE;
	else
	if(_stricmp(name, "informix") == 0)
		type = SQL_INFORMIX;
	else
	if(_stricmp(name, "greenplum") == 0)
		type = SQL_GREENPLUM;
	else
	if(_stricmp(name, "asa") == 0)
		type = SQL_SYBASE_ASA;
	else
	if(_stricmp(name, "teradata") == 0)
		type = SQL_TERADATA;
	else
	if(_stricmp(name, "netezza") == 0)
		type = SQL_NETEZZA;
    else
	if(_stricmp(name, "mariadb") == 0)
		type = SQL_MARIADB;
    else
	if(_stricmp(name, "mariadb_ora") == 0)
		type = SQL_MARIADB_ORA;
    else
	if(_stricmp(name, "hive") == 0)
		type = SQL_HIVE;
    else
	if(_stricmp(name, "redshift") == 0)
		type = SQL_REDSHIFT;
    else
	if(_stricmp(name, "esgyndb") == 0 || _stricmp(name, "trafodion") == 0)
		type = SQL_ESGYNDB;
	else
	if(_stricmp(name, "ads") == 0)
		type = SQL_SYBASE_ADS;
	
	return type;
}

// Output how to use the tool if /? or incorrect parameters are specified
void Sqlines::PrintHowToUse()
{
	printf("\n\nHow to use:");
	printf("\n\n    sqlines -option=value [...n]");

	printf("\n\nOptions:\n");
	printf("\n   -s        - Source type");
	printf("\n   -t        - Target type");
	printf("\n   -in       - List of files (wildcards *.* are allowed)");
	printf("\n   -out      - Output directory (the current directory by default)");
	printf("\n   -log      - Log file (sqlines.log by default)");
	printf("\n   -?        - Print how to use");

	printf("\n\nExample:");
#ifdef WIN32
	printf("\n\nConvert script.sql file from Oracle to SQL Server");
	printf("\n\n   sqlines.exe -s=oracle -t=sql -in=script.sql");

	printf("\nPress any key to continue...\n");

	_getch();
#else
	printf("\n\nConvert script.sql file from Oracle to MySQL");
	printf("\n\n   ./sqlines -s=oracle -t=mysql -in=script.sql");
	printf("\n");
#endif
	printf("\n");
}

// Output the current date and time
void Sqlines::PrintCurrentTimestamp()
{
#ifdef WIN32
	SYSTEMTIME lt;
	GetLocalTime(&lt);

	// Write log record
	_log.LogFile("\n\nCurrent timestamp: %d-%02d-%02d %02d:%02d:%02d.%03d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond,
		lt.wMilliseconds);
#else
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);

	_log.LogFile("\n\nCurrent timestamp: %d-%02d-%02d %02d:%02d:%02d", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
#endif
}
