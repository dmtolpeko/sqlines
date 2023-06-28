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

#ifndef sqlines_sqlines_h
#define sqlines_sqlines_h

#include <string>
#include "applog.h"
#include "parameters.h"
#include "license.h"

#define SQLINES_VERSION             "SQLines 3.1.330 - SQL Assessment and Conversion Tool."
#define SQLINES_COPYRIGHT           "Copyright (c) 2020 SQLines. All Rights Reserved."

#define S_OPTION                    "-s"        // Source type 
#define T_OPTION                    "-t"        // Target type
#define IN_OPTION                   "-in"       // Input files/directory
#define STDIN_OPTION                "-stdin"    // Read input from STDIN and output to STDOUT
#define OUT_OPTION                  "-out"      // Output files/directory
#define A_OPTION                    "-a"        // Assessment 
#define SL_OPTION                   "-sl"       // Source programming language (COBOL i.e.)
#define TL_OPTION                   "-tl"       // Target programming language (Java i.e.)
#define LOG_OPTION                  "-log"      // Log file

#define SQLINES_CURRENT_FILE        "__cur_file__"    // Relative path for the current file
#define SQLINES_EVAL_MODE           "__eval_mode__"   // Evaluation mode

// Default log file name
#define SQLINES_LOGFILE             "sqlines.log"

#define SUFFIX(int_value)           ((int_value == 1) ? "" : "s")

class Sqlines
{
    // Options
    std::string _s;
    std::string _t;
    std::string _in;
    std::string _out;
	std::string _logfile;

    bool _a;
    bool _stdin;

    // Current executable file
    const char *_exe;

    // SQL parser handle 
    void *_parser;

    // Total number of files
    int _total_files;

    // Command line parameters
    Parameters _parameters;
    // Logger
    AppLog _log;

	// License information
	License _license;

public:
    Sqlines();

    // Run the tool with command line parameters
    int Run(int argc, char** argv);

private:
    // Read and validate parameters
    int SetParameters(int argc, char **argv);

    int ProcessFiles();
    int ProcessStdin();
    int ProcessFile(std::string &file, std::string &out_file, int *in_size, int *in_lines);

    // Get output name of the file
    std::string GetOutFileName(std::string &input, std::string &relative_name);

    // Set source and target types
    void SetTypes();
    short DefineType(const char *name);

    // Set conversion options
    void SetOptions();

    // Output how to use the tool if /? or incorrect parameters are specified
    void PrintHowToUse();
    // Output the current date and time
    void PrintCurrentTimestamp();
};

#endif // sqlines_sqlparser_h