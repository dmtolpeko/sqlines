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

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <stdio.h>
#include "os.h"

#if !defined(WIN32) && !defined(_WIN64)
long long GetTickCount(void) 
{
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now))
    return 0;
  return (long long)(now.tv_sec * 1000 + now.tv_nsec/1000000);
}
#endif

// Enter the critical section
void Os::EnterCriticalSection(void *section)
{
#if defined(WIN32) || defined(_WIN64)
	::EnterCriticalSection((CRITICAL_SECTION*)section);
#else
	pthread_mutex_lock((pthread_mutex_t*)section);
#endif
}

// Leave the critical section
void Os::LeaveCriticalSection(void *section)
{
#if defined(WIN32) || defined(_WIN64)
	::LeaveCriticalSection((CRITICAL_SECTION*)section);
#else
	pthread_mutex_unlock((pthread_mutex_t*)section);
#endif
}

#if !defined(WIN32) && !defined(_WIN64)
// Initialize event
void Os::CreateEvent(Event *event)
{
	pthread_cond_init(&event->_event, NULL);
	pthread_mutex_init(&event->_mutex, NULL);
	event->_state = false;
}

// Set event (use state variable to deal with spurious wakeups)
void Os::SetEvent(Event *event)
{
	pthread_mutex_lock(&event->_mutex);
	event->_state = true;
	pthread_cond_signal(&event->_event);
	pthread_mutex_unlock(&event->_mutex);
}

// Reset event 
void Os::ResetEvent(Event *event)
{
	pthread_mutex_lock(&event->_mutex);
	event->_state = false;
	pthread_mutex_unlock(&event->_mutex);
}

// Wait for event to be set (handle spurious wakeups and missed signals)
void Os::WaitForEvent(Event *event)
{
	pthread_mutex_lock(&event->_mutex);
	while(!event->_state)
		pthread_cond_wait(&event->_event, &event->_mutex);
	event->_state = false;
	pthread_mutex_unlock(&event->_mutex);
}

#endif

// Load dynamic library
#if defined(WIN32) || defined(_WIN64)
HMODULE Os::LoadLibrary(const char *name)
{
	return ::LoadLibrary(name);
}
#else
void* Os::LoadLibrary(const char *name)
{
	return dlopen(name, RTLD_NOW);
}
#endif

// Error during the library load
char* Os::LoadLibraryError() 
{ 
#if defined(WIN32) || defined(_WIN64)
	return NULL;
#else
	return dlerror(); 
#endif
}

// Get procedure address
#if defined(WIN32) || defined(_WIN64)
void* Os::GetProcAddress(HMODULE module, const char *name)
{
	return ::GetProcAddress(module, name);
}
#else
void* Os::GetProcAddress(void* module, const char *name)
{
	return dlsym(module, name);
}
#endif

// Get the full path of the loaded library
#if defined(WIN32) || defined(_WIN64)
void Os::GetModuleFileName(HMODULE module, char *out_path, int len)
{
	::GetModuleFileName(module, out_path, (DWORD)len);
}
#else
void Os::GetModuleFileName(void* module, char *out_path, int len)
{
}
#endif

// Sleep the specified number of seconds
void Os::Sleep (unsigned int sec)
{
#if defined(WIN32) || defined(_WIN64)
	::Sleep(sec * 1000);
#else
	sleep(sec);
#endif
}

// Check if DDL is 64-bit (check runs on 32-bit Windows systems only)
#if defined(WIN32) && !defined(_WIN64)
bool Os::Is64Bit(const char *filename)
{
	bool is64 = false;
	HMODULE module = LoadLibraryEx(filename, NULL, LOAD_LIBRARY_AS_DATAFILE);
	char *cur = (char*)module;

	// For some reason memory starts from Z, not MZ
    if (cur == NULL || *cur != 'Z')
        return false;

	IMAGE_DOS_HEADER *dh = (IMAGE_DOS_HEADER*)(cur - 1);
	IMAGE_NT_HEADERS *nh = (IMAGE_NT_HEADERS*)(cur - 1 + dh->e_lfanew);

	// IMAGE_FILE_MACHINE_AMD64 (x64)
	if(nh->FileHeader.Machine == 0x8664)
		is64 = true;

	FreeLibrary(module);
	return is64;
}
#elif defined(_WIN64)
bool Os::Is64Bit(const char* /*filename*/)
{
	return false;
}
#endif

// Get current time in milliseconds
size_t Os::GetTickCount()
{
#ifdef WIN32
	return ::GetTickCount();
#else
	struct timeval  tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec * 1000 + tv.tv_usec/1000;
#endif
}

// Get error message of the last system error
void Os::GetLastErrorText(const char *prefix, char *output, int len)
{
	if(output == NULL || len <= 0)
		return;

#if defined(WIN32) || defined(WIN64)
	char error[1024];
	DWORD rc = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 
						1033, (LPSTR)error, 1024, NULL);
 
	// No error found
	if(rc == 0)
	{
		*output = '\x0';
		return;
	}

	// On Windows OS error messages terminated with 0D 0A 00, remove new lines
	if(rc > 2 && error[rc - 2] == '\r')
		error[rc - 2] = '\x0';

	if(prefix != NULL)
		strcpy(output, prefix);
	else
		*output = '\x0';

	strcat(output, error);

#endif
}

// Get the current timestamp string
const char *Os::CurrentTimestamp()
{
	static char ts[26];

#if defined(WIN32) || defined(_WIN64)
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	sprintf(ts, "%d-%02d-%02d %02d:%02d:%02d.%03d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
#else
	char t[26];
	struct tm* tm_info;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	tm_info = localtime(&tv.tv_sec);
	strftime(t, sizeof(t), "%Y:%m:%d %H:%M:%S", tm_info);
	sprintf(ts, "%s.%03d", t, (int)tv.tv_usec/1000);
#endif
	return ts;
}

// Get the current working directory
const char* Os::CurrentWorkingDirectory()
{
	static char cwd[PATH_MAX];

	if(_getcwd(cwd, PATH_MAX) == NULL)
		*cwd = '\x0';

	return cwd;
}

