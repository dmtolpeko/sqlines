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

// AppLog - Application logger class
// Copyright (c) 2012 SQLines. All rights reserved

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "applog.h"
#include "file.h"

// Constructor
AppLog::AppLog()
{
	_filename = APPLOG_DEFAULT_FILE;
	_filename_trc = APPTRACE_DEFAULT_FILE;
	first_write = true;
	first_write_trc = true;

	_use_stderr = false;

	_console = NULL;
	_console_object = NULL;

#if defined(WIN32) || defined(_WIN64)
	InitializeCriticalSection(&_log_critical_section);
#else
	pthread_mutex_init(&_log_critical_section, NULL);
#endif
}

// Set log file name
void AppLog::SetLogfile(const char *name, const char *default_dir)
{
	if(name == NULL)
		return;

	// If directory is not set use the default directory
	if(File::IsDirectoryInPath(name) == false)
		File::GetPathFromDirectoryAndFile(_filename, default_dir, name); 
	else
		_filename = name;

	first_write = true;
}

// Set trace file name
void AppLog::SetTracefile(const char *name, const char *default_dir)
{
	if(name == NULL)
		return;

	// If directory is not set use the default directory
	if(File::IsDirectoryInPath(name) == false)
		File::GetPathFromDirectoryAndFile(_filename_trc, default_dir, name); 
	else
		_filename_trc = name;

	first_write_trc = true;
}

// Log message to console and log file
void AppLog::Log(const char *format, ...)
{
	Os::EnterCriticalSection(&_log_critical_section);

	va_list args;
	va_start(args, format);

	// log message to console
	if(_console == NULL)
	{
		if(!_use_stderr)
		{
			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wformat-nonliteral"
			vprintf(format, args);
			#pragma clang diagnostic pop
			fflush(stdout);
		}
		else
		{
			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wformat-nonliteral"
			vfprintf(stderr, format, args);
			#pragma clang diagnostic pop
			fflush(stderr);
		}
	}
	else
		_console(_console_object, format, args);

	va_end(args);
	va_start(args, format);

	// log message to file
	LogFileVaList(format, args);

	va_end(args);
	Os::LeaveCriticalSection(&_log_critical_section);
}

// Log messages to file if trace mode is set
void AppLog::Trace(const char *format, ...)
{
	Os::EnterCriticalSection(&_log_critical_section);

	va_list args;
	va_start(args, format);

	// log message into the file
	TraceFileVaList(format, args);

	va_end(args);
	Os::LeaveCriticalSection(&_log_critical_section);
}

// Write to console only
void AppLog::LogConsole(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	// log message to console
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wformat-nonliteral"
	vprintf(format, args);
	#pragma clang diagnostic pop

	fflush(stdout);

	va_end(args);
}

// Write to log file only
void AppLog::LogFile(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	// log message into the file
	LogFileVaList(format, args);

	va_end(args);
}

// Write to log file
void AppLog::LogFileVaList(const char *format, va_list args)
{
	const char *openMode = "a";

	// During the first call destroy the log file content that may exists from the previous runs
	if(first_write == true)
		openMode = "w";

	FILE *file = fopen(_filename.c_str(), openMode);

	// Log message to file
	if(file)
	{
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wformat-nonliteral"
		vfprintf(file, format, args);
		#pragma clang diagnostic pop
		fclose(file);
	}
	else 
	{  
		// Show error message during the first call only  
		if(first_write == true)
			printf("\n\nError:\n Opening log file %s - %s", _filename.c_str(), strerror(errno));
	}

	first_write = false;
}

// Write data buffer to log file
void AppLog::LogFile(const char *data, int64_t len)
{
	if(first_write)
	{
		File::Write(_filename.c_str(), data, len);
		first_write = false;
	}
	else
		File::Append(_filename.c_str(), data, len);
}

// Write to trace file
void AppLog::TraceFileVaList(const char *format, va_list args)
{
	const char *openMode = "a";

	// During the first call destroy the log file content that may exists from the previous runs
	if(first_write_trc == true)
		openMode = "w";

	FILE *file = fopen(_filename_trc.c_str(), openMode);

	// Log message to file
	if(file)
	{
		fprintf(file, "%s ", Os::CurrentTimestamp());
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wformat-nonliteral"
		vfprintf(file, format, args);
		#pragma clang diagnostic pop
		fprintf(file, "\n");
		fclose(file);
	}
	else 
	{  
		// Show error message during the first call only  
		if(first_write == true)
			printf("\n\nError:\n Opening trace file %s - %s", _filename_trc.c_str(), strerror(errno));
	}

	first_write_trc = false;
}
