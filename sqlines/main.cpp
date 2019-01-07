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

// SQLines entry point

#include <stdio.h>
#include "sqlines.h"

// Entry point
int main(int argc, char** argv)
{
	ssize_t rc = 0;

	Sqlines sqlines;

	try
	{
		// Run SQLines in command line
		rc = sqlines.Run(argc, argv);		
	}
	catch(...)
	{
		return -2;
	}

	printf("\n");

	return static_cast<int>(rc);
}
