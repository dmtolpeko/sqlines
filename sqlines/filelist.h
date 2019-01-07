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

// FileList - List of files by template

#ifndef sqlines_filelist_h
#define sqlines_filelist_h

#include <string>
#include <list>

class FileList
{
	// List of files
	std::list<std::string> _files;
	// Total size of all files in the list in bytes
	int _size;  
public:
	FileList();

	// Load the list of files
	int Load(const char *inputPath);	

	// Get the directory list matching the specified wildcard (A*\B*\C\D* i.e.)
	int GetDirectoriesByWildcard(const char *wildcard, std::list<std::string> &dirs);

	// Get list
	std::list<std::string>& Get() { return _files; }
	// Get total size of all files in the list in bytes
	int GetSize() { return _size; }

private:
	// Find files in the specified directory matching the file wildcard
	int	FindFiles(std::string dir, std::string file, std::list<std::string> &dirs, std::list<std::string> &files); 
};

#endif // sqlines_filelist_h
