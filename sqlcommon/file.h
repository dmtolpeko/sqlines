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

#ifndef sqlines_file_h
#define sqlines_file_h

#include <string>

#if defined(WIN32) || defined(WIN64)
#define DIR_SEPARATOR_CHAR '\\'
#define DIR_SEPARATOR_STR  "\\"

struct _finddata_t;

#else
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_STR  "/"
#endif

class File
{
public:
	// Check if path includes a directory, not just file name
	static bool IsDirectoryInPath(const char *path);

	// Check if the path points to an existing directory
	static bool IsDirectory(const char *path);	
    // Check if the path points to an existing file
	static bool IsFile(const char *path, size_t *size);	

#if defined(WIN32) || defined(WIN64)
    // Check if the file is directory
	static bool IsDirectory(_finddata_t *fileInfo);

	// Check if the file is found
	static bool IsFile(_finddata_t *fileInfo);
#endif

	// Find directory by a template in the last path item
	static void FindDir(const char *dir_template, std::string &dir); 

	// Split directory and file parts from a path
	static void SplitDirectoryAndFile(const char* path, std::string &dir, std::string &file);

	// Get file path from directory and file name
	static void GetPathFromDirectoryAndFile(std::string &path, const char *dir, const char *file);

	// Get last directory separator position
	static int GetLastDirSeparatorPos(const char *path);
    
    // Get the position of file extension
	static int GetExtensionPosition(const char *path);

	// Get the size of the file
	#if defined(WIN32) || defined(WIN64)
	static __int64 GetFileSize(const char* file);
	#else
	static int64_t GetFileSize(const char* file);
	#endif

	// Get content of the file (without terminating with 'x0')
	static int GetContent(const char* file, void *input, int64_t len);

	// Get relative name
	static std::string GetRelativeName(const char* base, const char *file);
    
   	// Write the buffer to the file
    static long Write(const char *file, const char* content, int64_t size);

	// Create directories (supports nested directories)
	static void CreateDirectories(const char *path);
    
   	// Truncate the file (empty file will be created)
	static int Truncate(const char *file);
	// Append data to the existing file
	static long Append(const char *file, const char *data, int64_t len);
};

#endif // sqlines_file_h
