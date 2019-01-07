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

// Stats class

#ifndef sqlexec_stats_h
#define sqlexec_stats_h

#include <string>
#include <map>
#include "token.h"

#define SQLEXEC_STAT_FILE         "sqlines_func_calls.txt"

struct StatsItem
{
    // Number of occurrences
    int occurrences;

    // Conversion result (empty if no changes)
    std::string t_value;

    // Conversion is not required
    bool conv_not_required;
    // Conversion with warning
    bool warn;

    // Conversion notes
    std::string notes;

    StatsItem() { occurrences = 0; conv_not_required = false; warn = false; }
    StatsItem(int o, const char *tv, bool not_req, bool w, const char *nt) 
    { 
        occurrences = o; 
        conv_not_required = not_req;
        warn = w;
        
        if (tv != NULL) 
            t_value.assign(tv);

        if (nt != NULL) 
            notes.assign(nt);
    }
};

class Stats
{
public:
    std::map<std::string, int> _data_types;
    std::map<std::string, int> _udt_data_types;
    std::map<std::string, StatsItem> _data_types_dtl;
    std::map<std::string, int> _udt_data_types_dtl;
    std::map<std::string, int> _builtin_func;
    std::map<std::string, int> _builtin_func_dtl;
    std::map<std::string, int> _udf_func;
    std::map<std::string, int> _system_proc;
    std::map<std::string, int> _system_proc_dtl;
    std::map<std::string, int> _statements;
    std::map<std::string, int> _crtab_statements;
    std::map<std::string, int> _alttab_statements;
    std::map<std::string, int> _pl_statements;
    std::map<std::string, int> _quoted_idents;
    std::map<std::string, int> _strings;
    std::map<std::string, int> _numbers;
    std::map<std::string, int> _words;
    std::map<std::string, int> _delimiters;

    // Constructor/destructor
    Stats();
    ~Stats();

    // Collect statistics 
    void DataTypes(const char* value)  { Add(_data_types, value); }
    void DataTypes(Token *token)       { Add(_data_types, token); }
    void DataTypesDetail(Token *start)  { Add(_data_types_dtl, start); }
    void DataTypesDetail(Token *start, Token *end)  { Add(_data_types_dtl, start, end); }

    void UdtDataTypes(const char* value)  { Add(_udt_data_types, value); }
    void UdtDataTypesDetail(Token *start, Token *end)  { Add(_udt_data_types_dtl, start, end); }
    
    void Statements(const char* value)           { Add(_statements, value); }
    void Statements(Token *token)                { Add(_statements, token); }
    void CreateTabStatements(const char *value)  { Add(_crtab_statements, value, false); }
    void AlterTabStatements(const char *value)   { Add(_alttab_statements, value, false); }
    void ProceduralStatements(const char *value) { Add(_pl_statements, value); }
    void ProceduralStatements(Token *token)      { Add(_pl_statements, token); }

    void Functions(Token *token)          { Add(_builtin_func, token); }
    void FunctionsDetail(TokenStr *token) { Add(_builtin_func_dtl, token); }
    void UdfFunctions(Token *token)       { Add(_udf_func, token); }

    void Procedures(Token *token)           { Add(_system_proc, token); }
    void ProceduresDetail(TokenStr *token)  { Add(_system_proc_dtl, token); }
    
    void Quoted(Token *token)      { Add(_quoted_idents, token, false); }
    void Strings(Token *token)     { Add(_strings, token, false); }
    void Numbers(Token *token)     { Add(_numbers, token); }
    void Word(Token *token)        { Add(_words, token); }
    void Delimiter(Token *token)   { Add(_delimiters, token); }

	// Log function call with all nested expressions
	void LogFuncCall(Token *name, Token *end, std::string &cur_file);
	// Get meta type for the specified identificator
	const char* GetMetaIdent(Token *name);

    // Collect statistics for the specified item
    void Add(std::map<std::string, StatsItem> &map, Token *token, bool case_insense = true);
    void Add(std::map<std::string, StatsItem> &map, Token *start, Token *end, bool case_insense = true);
    void Add(std::map<std::string, StatsItem> &map, std::string value, const char* target, 
        bool case_insense = true, bool conv_not_required = false, bool warn = false, const char *notes = NULL);
    
    void Add(std::map<std::string, int> &map, std::string value, bool case_insense = true);
    void Add(std::map<std::string, int> &map, Token *token, bool case_insense = true);
    void Add(std::map<std::string, int> &map, TokenStr *token, bool case_insense = true);

    void Add(std::map<std::string, int> &map, Token *start, Token *end, bool case_insense = true);
};

#endif // sqlexec_stats_h
