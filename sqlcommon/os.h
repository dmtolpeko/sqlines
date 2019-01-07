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

// OS Class - Operating system related functions

#ifndef sqlines_os_h
#define sqlines_os_h

#include <stdlib.h>
#include <time.h>

#ifndef PATH_MAX
#define PATH_MAX	260
#endif

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#include <direct.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define _getcwd getcwd

// POSIX synchronization event
struct Event
{
	pthread_cond_t _event;
	pthread_mutex_t _mutex;
	bool _state;
};

long long GetTickCount(void);
#endif

class Os
{
public:
	// Enter and leave critical section
	static void EnterCriticalSection(void *section);
	static void LeaveCriticalSection(void *section);

#if !defined(WIN32) && !defined(_WIN64)
	// Events
	static void CreateEvent(Event *event);
	static void SetEvent(Event *event);
	static void ResetEvent(Event *event);
	static void WaitForEvent(Event *event);
#endif

	// Load dynamic library
#if defined(WIN32) || defined(_WIN64)
	static HMODULE LoadLibrary(const char *name);
#else
    static void* LoadLibrary(const char *name);
#endif
	static char* LoadLibraryError();

	// Get procedure address
#if defined(WIN32) || defined(_WIN64)
	static void* GetProcAddress(HMODULE module, const char *name);
#else
	static void* GetProcAddress(void* module, const char *name);
#endif

	// Get current time in milliseconds
	static size_t GetTickCount();

	// Get error message of the last system error
	static void GetLastErrorText(const char *prefix, char *output, int len);

	// Sleep the specified number of seconds
	static void Sleep (unsigned int sec);

	// Get the full path of the loaded library
#if defined(WIN32) || defined(_WIN64)
	static void GetModuleFileName(HMODULE module, char *out_path, int len);
#else
	static void GetModuleFileName(void* module, char *out_path, int len);
#endif

	// Get the current timestamp string
	static const char* CurrentTimestamp();

	// Get the current working directory
	static const char* CurrentWorkingDirectory();

	// Check if DDL is 64-bit
	static bool Is64Bit(const char *filename);
};

#endif // sqlines_os_h
