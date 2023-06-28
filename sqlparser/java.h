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

// Java Conversion Class

#ifndef sqlines_java_h
#define sqlines_java_h

#define JAVA_PREPEND(token, string) _sqlparser->PrependNoFormat(token, string, L##string, sizeof(string) - 1)

class SqlParser;

class Java
{
	SqlParser *_sqlparser;

public:
	Java();

	// Set data type for procedure OUT parameters
	void SetOutDataType(Token *data_type);

	// Map SQL data type name (without length, precision, scale) to Java object type
	const char *MapSqlDataType(const char *name);

	// Create a string literal from a set of tokens (support multi-line strings)
	void MakeStringLiteral(Token *start, Token *end);

	void SetParser(SqlParser *parser) { _sqlparser = parser; }
};

#endif // sqlines_java_h
