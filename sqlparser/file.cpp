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

// File - File operations

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>
#elif __APPLE__
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#define _read read
#define _write write
#define _close close
#else
#include <sys/stat.h>
#include <sys/io.h>
#include <unistd.h>

#define _read read
#define _write write
#define _close close

#endif

#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

#include "file.h"
#include "str.h"

// Check if the path points to an existing directory
bool File::IsDirectory(const char *path)
{
	if(path == NULL)
		return false;

#ifdef WIN32

	struct _finddata_t fileInfo;
	int findHandle = _findfirst(path, &fileInfo); 

	// Check if a file or directory exists with this name
	if(findHandle == -1)
	{
		_findclose(findHandle);
		return false;
	}

	// Check for a directory
	bool dir = IsDirectory(&fileInfo);

	_findclose(findHandle);
	return dir;

#else

	struct stat fileInfo;

	if(stat(path, &fileInfo) != -1)
	{
		// Check that this is a directory
		if(S_ISDIR(fileInfo.st_mode))
			return true;
	}

	return false;

#endif
}

// Check if the path points to an existing file
bool File::IsFile(const char *path, size_t *size)
{
#ifdef WIN32

	struct _finddata_t fileInfo;
	int findHandle = _findfirst(path, &fileInfo); 

	// check if a file or directory exists with this name
	if(findHandle == -1)
	{
		_findclose(findHandle);
		return false;
	}

	// check for file
	bool file = IsFile(&fileInfo);

	if(size != NULL)
		*size = fileInfo.size;

	_findclose(findHandle);

	return file;

#else

	struct stat fileInfo;

	if(stat(path, &fileInfo) != -1)
	{
		// check that this is a file
		if(S_ISREG(fileInfo.st_mode))
		{
			if(size != NULL)
				*size = fileInfo.st_size;

			return true;
		}
	}

	return false;

#endif
}

#ifdef WIN32

// Check if it is a directory
bool File::IsDirectory(_finddata_t *fileInfo)
{
	if(!fileInfo)
		return false;

	if((fileInfo->attrib & _A_SUBDIR) &&
		// skip current "." and parent ".." directory that are also returned
		strcmp(fileInfo->name, ".") && strcmp(fileInfo->name, ".."))
		return true;

	return false;
}

#endif

#ifdef WIN32

// Check if it is a file
bool File::IsFile(_finddata_t *fileInfo)
{
	if(!fileInfo)
		return false;

	if(fileInfo->attrib & _A_SUBDIR)
		return false;

	return true;
}

#endif

// Split directory and file parts from a path
void File::SplitDirectoryAndFile(const char* path, std::string &dir, std::string &file)
{
	if(path == NULL)
		return;

	int pos = GetLastDirSeparatorPos(path);

	// Split directory and file
	if(pos >= 0)
	{
		dir.assign(path, (size_t)pos);
		file.assign(path + pos + 1);
	}
	else
		file = path;
}

// Get last directory separator position
int File::GetLastDirSeparatorPos(const char *path)
{
	if(path == NULL)
		return -1;

	int pos = -1;
	int i = 0;

	// Find the position of last directory separator
	while(path[i])
	{
		if(path[i] == DIR_SEPARATOR_CHAR)
			pos = i;

		i++;
	}

	return pos;
}

// Get the position of file extension
int File::GetExtensionPosition(const char *path)
{
	if(path == NULL)
		return -1;

	int i = 0;
	int dot = -1;

	// find the position of the last dot
	while(path[i] != 0)
	{
		if(path[i] == '.')
			dot = i;

		i++;
	}

	if(dot != -1)
		return dot;

	// If not found, return the index of terminating 0
	return i;
}

// Get the size of the file
int File::GetFileSize(const char* file)
{
	int size = -1;

	if(file == NULL)
		return -1;

#ifdef WIN32

	struct _finddata_t fileData;

	// define the file size to allocate buffer
	int findHandle = _findfirst(file, &fileData); 

	if(findHandle == -1)
	{
		return -1;
	}

	size = (int)fileData.size;

	_findclose(findHandle);

#else

	struct stat info;
  
	if(stat(file, &info) != -1)
	{
		// Check that the file was found
		if(S_ISREG(info.st_mode))
 			size = info.st_size;
	}

#endif

	return size;
}

// Get content of the file (without terminating with 'x0')
int File::GetContent(const char *file, void *input, size_t len)
{
	if(file == NULL)
		return -1;

	int fileHandle = -1;

	// open the file
#ifdef WIN32
	fileHandle = _open(file, _O_RDONLY | _O_BINARY);	
#else
	fileHandle = open(file, O_RDONLY);
#endif

	if(fileHandle == -1)
	{
		return -1;
	}

	// Read the file content to the buffer
	int bytesRead = _read(fileHandle, input, len);
	if(bytesRead == -1)
	{
		_close(fileHandle);

		return -1;
	}

	_close(fileHandle);

	return 0;
}

// Get relative name
std::string File::GetRelativeName(const char* base, const char *file)
{
	std::string relative;

	if(base == NULL || file == NULL)
		return relative;

	// If base and file are equal return the file name
	if(_stricmp(base, file))
	{
		int pos = File::GetLastDirSeparatorPos(file);
		relative = (pos > 0) ? file + pos + 1 : file;

		return relative;
	}

	return relative;
}

// Write the buffer to the file
int File::Write(const char *file, const char* content, size_t size)
{
	 if(file == NULL || content == NULL)
		return -1;

#ifdef WIN32
  // open the file
  // if _S_IWRITE is not set, the Read Only file is created (at least on Windows)
  int fileh = _open(file, _O_CREAT | _O_RDWR | _O_BINARY | _O_TRUNC, _S_IREAD | _S_IWRITE);
#else
  // open the file
  int fileh = open(file, O_CREAT | O_RDWR, 0666);
#endif

   if(fileh == -1)
	   return -1;

   // write the content to the file 
   int rc = _write(fileh, content, size);
 
   _close(fileh);

   return rc;
}
