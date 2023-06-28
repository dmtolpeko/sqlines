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
#include <list>
#include "token.h"

// Conversion status
#define STATS_CONV_UNDEFINED		1
#define STATS_CONV_NO_NEED			2
#define STATS_CONV_OK				3
#define STATS_CONV_WARN				4
#define STATS_CONV_ERROR			5

// Complexity levels
#define STATS_CONV_VERY_LOW         1
#define STATS_CONV_LOW              2
#define STATS_CONV_MEDIUM           3
#define STATS_CONV_HIGH             4
#define STATS_CONV_VERY_HIGH        5

// Statistics
#define DTYPE_STATS(value)           { if(_stats != NULL) _stats->DataTypes(value, &ssi, GetLastToken()); }
#define DTYPE_STATS_V(value, start)  { if(_stats != NULL) _stats->DataTypes(value, &ssi, start, GetLastToken()); }
#define UDTYPE_STATS_V(value, start) { if(_stats != NULL) _stats->UdtDataTypes(value, &ssi, start, GetLastToken()); }
#define FUNC_STATS(name)             { if(_stats != NULL) _stats->Functions(name, &ssi, GetLastToken()); }
#define FUNC_STATS_V(value, start)   { if(_stats != NULL) _stats->Functions(value, &ssi, start, GetLastToken()); }
#define UDF_FUNC_STATS(name)         { if(_stats != NULL) _stats->UdfFunctions(name, &ssi, GetLastToken()); }
#define PROC_STATS(name)             { if(_stats != NULL) _stats->Procedures(name, &ssi, GetLastToken()); }
#define PROC_STATS_V(value, start)   { if(_stats != NULL) _stats->Procedures(value, &ssi, start, GetLastToken()); }

#define STMS_STATS(start)            { if(_stats != NULL) _stats->Statements(start, &ssi, GetLastToken()); }
#define STMS_STATS_V(value, start)   { if(_stats != NULL) _stats->Statements(value, &ssi, start, GetLastToken()); }
#define CREATE_TAB_STMS_STATS(value) { if(_stats != NULL) _stats->CreateTabStatements(value); }
#define ALTER_TAB_STMS_STATS(value)  { if(_stats != NULL) _stats->AlterTabStatements(value); }

#define SELECT_DTL_STATS_V(value, start) { if(_stats != NULL) _stats->SelectStatementsDetail(value, &sdi, start, GetLastToken()); }

#define CREATE_PROC_STMS_STATS(start)           { if(_stats != NULL) _stats->CreateProcDetail(start, &sdi, GetLastToken()); }
#define CREATE_PROC_STMS_STATS_V(value, start)  { if(_stats != NULL) _stats->CreateProcDetail(value, &sdi, start, GetLastToken()); }

#define PL_STMS_STATS(start)           { if(_stats != NULL) _stats->ProceduralStatements(start, &ssi, GetLastToken()); }
#define PL_STMS_STATS_V(value, start)  { if(_stats != NULL) _stats->ProceduralStatements(value, &ssi, start, GetLastToken()); }
#define PL_STMS_EXCEPTION_STATS(value) { if(_stats != NULL) _stats->ExceptionsDetail(value, &sdi, GetLastToken()); }

#define DTYPE_DTL_STATS_0(start)  { if(_stats != NULL) _stats->DataTypesDetail(start, &si); }
#define DTYPE_DTL_STATS_L(start)  { if(_stats != NULL) _stats->DataTypesDetail(start, &si, GetLastToken()); }
#define DTYPE_DTL_STATS_ST(value, target)  { if(_stats != NULL) _stats->DataTypesDetail(value, &si, target); }

#define UDTYPE_DTL_STATS_L(start) { if(_stats != NULL) _stats->UdtDataTypesDetail(start, GetLastToken(), &sdi); }

#define FUNC_DTL_STATS_V(value, start) { if(_stats != NULL) _stats->FunctionsDetail(value, &sdi, start, GetLastToken()); }
#define PROC_DTL_STATS(value, start) { if(_stats != NULL) _stats->ProceduresDetail(value, &sdi, start, GetLastToken()); }

#define SEQ_STATS_V(value, start)        { if(_stats != NULL) _stats->Sequences(value, &ssi, start, GetLastToken()); }
#define SEQ_DTL_STATS_V(value, start)    { if(_stats != NULL) _stats->SequencesDetail(value, &sdi, start, GetLastToken()); }
#define SEQ_OPT_DTL_STATS(start, end)    { if(_stats != NULL) _stats->SequencesOptionsDetail(start, end, &sdi); }
#define SEQ_REF_STATS(value, start)      { if(_stats != NULL) _stats->SequencesReference(value, &sdi, start, GetLastToken()); }
#define SEQ_REF_DTL_STATS(start)         { if(_stats != NULL) _stats->SequencesReferenceDetail(start, GetLastToken(), &sdi); }

#define PKG_STATS_V(value, start)        { if(_stats != NULL) _stats->Packages(value, &ssi, start, GetLastToken()); }
#define PKG_DTL_STATS(value)             { if(_stats != NULL) _stats->PackagesDetail(value, &sdi, GetLastToken()); }

#define CONV_STATS                       int conv_stats = STATS_CONV_UNDEFINED; const char *conv_desc = NULL; const char *conv_conv = NULL; const char *conv_url = NULL; 
#define CONV_DESC(desc)                  { conv_desc = desc; }
#define CONV(cond, status, conv, url)    { if(cond) { conv_stats = status; conv_conv = conv; conv_url = url; } }
#define CONV_NO_NEED(cond)               CONV(cond, STATS_CONV_NO_NEED, NULL, NULL)
#define IS_CONV_WARN                     (conv_stats == STATS_CONV_WARN)

#define STATS_DECL                                    StatsSummaryItem ssi;
#define STATS_SET_DESC(p_desc)                        ssi.desc = p_desc; 
#define STATS_SET_CONV(cond, status)                  { if(cond) { ssi.Inc(status); } }
#define STATS_SET_CONV_NO_NEED(cond)                  STATS_SET_CONV(cond, STATS_CONV_NO_NEED)
#define STATS_SET_CONV_OK(cond)                       STATS_SET_CONV(cond, STATS_CONV_OK)
#define STATS_SET_COMPLEXITY(cond, value)             { if(cond) { ssi.UpdateSingleOccurrenceComplexity(value); } }
#define STATS_UPDATE_STATUS                           ssi.UpdateSingleOccurrenceStatus(&sdi); ssi.UpdateSingleOccurrenceComplexity(&sdi);

#define STATS_DTL_DECL                                StatsDetailItem sdi;
#define STATS_DTL_DESC(p_desc)                        sdi.desc = p_desc; 
#define STATS_DTL_CONV(cond, status, cmplx, nt, lnk)  sdi.conv_status = status; sdi.complexity = cmplx; sdi.note = nt; sdi.link = lnk;
#define STATS_DTL_CONV_NO_NEED(cond)                  STATS_DTL_CONV(cond, STATS_CONV_NO_NEED, 0, "", "")
#define STATS_DTL_CONV_OK(cond, cmplx, note, lnk)     STATS_DTL_CONV(cond, STATS_CONV_OK, cmplx, note, lnk)
#define STATS_DTL_CONV_ERROR(cond, cmplx, note, lnk)  STATS_DTL_CONV(cond, STATS_CONV_ERROR, cmplx, note, lnk)

#define STATS_ITM_DECL                                StatsItem si;
#define STATS_ITM_CONV(status)                        si.conv_status = status; 
#define STATS_ITM_CONV_NO_NEED                        STATS_ITM_CONV(STATS_CONV_NO_NEED)
#define STATS_ITM_CONV_OK                             STATS_ITM_CONV(STATS_CONV_OK)
#define STATS_ITM_CONV_ERROR                          STATS_ITM_CONV(STATS_CONV_ERROR)

#define SQLEXEC_STAT_FILE                             "sqlines_func_calls.txt"

struct StatsDetailItem;
struct StatsSnippetItem;

// Summary statistics item
struct StatsSummaryItem
{
	// Item description
	std::string desc;

	// Total number of occurrences
    int occurrences;
	// Occurrences when it is undefined whether conversion is required or not
	int conv_undef;
	// Occurrences when conversion is not required
	int conv_no_need;
	// Occurrences when conversion is successful
	int conv_ok;
	// Occurrences when conversion is with warning (should be compiled but some functionality is missed)
	int conv_warn;
	// Occurrences when conversion is with error (code will not compile)
	int conv_error;

	// Complexity level
	int complexity_very_low;
	int complexity_low;
	int complexity_medium;
	int complexity_high;
	int complexity_very_high;

	// Code snippets
	std::list<StatsSnippetItem> snippets;

	StatsSummaryItem() { Init(); }
	StatsSummaryItem(int conv_status) { Init(); Inc(conv_status); }

	void Init() { occurrences = 0; conv_undef = 0; conv_no_need = 0; conv_ok = 0; conv_warn = 0; conv_error = 0; 
	  complexity_very_low = 0; complexity_low = 0; complexity_medium = 0; complexity_high = 0;
	  complexity_very_high = 0; }

	// Increment stats summary item
	void Inc(int conv_status);
	void Inc(StatsSummaryItem *item);

	// Update summary status for single occurrence from detail
	void UpdateSingleOccurrenceStatus(StatsDetailItem *sid);
	// Update summary complexity for single occurrence from detail
	void UpdateSingleOccurrenceComplexity(StatsDetailItem *sid);
	void UpdateSingleOccurrenceComplexity(int complexity);

	// Get most critical status
	int GetMostCritical();
};

struct StatsDetailItem
{
	// Total number of occurrences
    int count;
	// Conversion status
	int conv_status;
	// Item description
	std::string desc;
	// Conversion notes
	std::string note;
	// Link to documentation
	std::string link;

	// Complexity level
	int complexity;

	// Code snippets
	std::list<StatsSnippetItem> snippets;

	StatsDetailItem() { count = 0; conv_status = 0; complexity = 0; }
};

// Code snippet
struct StatsSnippetItem
{
	// Source file name
	std::string filename;
	// Line in source code
	int line;
	// Source code snippet
	std::string snippet;

	StatsSnippetItem(std::string &f, Token *start, Token *end);
};

struct StatsItem
{
    // Number of occurrences
    int occurrences;

    // Conversion result (empty if no changes)
    std::string t_value;

    // Conversion status
	int conv_status;

    // Conversion notes
    std::string notes;

    StatsItem() { occurrences = 0; conv_status = 0; }
    StatsItem(int o, const char *tv, int cv, const char *nt) 
    { 
        occurrences = o;
		conv_status = cv;
        
        if (tv != NULL) 
            t_value.assign(tv);

        if (nt != NULL) 
            notes.assign(nt);
    }
};

class Stats
{
public:
    std::map<std::string, StatsSummaryItem> _data_types;
    std::map<std::string, StatsSummaryItem> _udt_data_types;
    std::map<std::string, StatsItem> _data_types_dtl;
    std::map<std::string, StatsDetailItem> _udt_data_types_dtl;
    std::map<std::string, StatsSummaryItem> _builtin_func;
    std::map<std::string, StatsDetailItem> _builtin_func_dtl;
    std::map<std::string, StatsSummaryItem> _udf_func;

	std::map<std::string, StatsSummaryItem> _sequences;
	std::map<std::string, StatsDetailItem> _sequences_dtl;
    std::map<std::string, StatsDetailItem> _sequences_opt_dtl;
	std::map<std::string, StatsDetailItem> _sequences_ref;
    std::map<std::string, StatsDetailItem> _sequences_ref_dtl;

	std::map<std::string, StatsSummaryItem> _system_proc;
    std::map<std::string, StatsDetailItem> _system_proc_dtl;
    std::map<std::string, StatsSummaryItem> _statements;
    std::map<std::string, int> _crtab_statements;
    std::map<std::string, int> _alttab_statements;
	std::map<std::string, StatsDetailItem> _select_statements_dtl;
	std::map<std::string, StatsDetailItem> _crproc_statements_dtl;

    std::map<std::string, StatsSummaryItem> _pl_statements;
	std::map<std::string, StatsDetailItem> _pl_statements_exceptions;

	std::map<std::string, StatsSummaryItem> _packages;
	std::map<std::string, StatsDetailItem> _pkg_statements_items;
    
	std::map<std::string, int> _quoted_idents;
	std::map<std::string, int> _non_7bit_ascii_idents;
    std::map<std::string, int> _strings;
    std::map<std::string, int> _numbers;
    std::map<std::string, int> _words;
    std::map<std::string, int> _delimiters;

	// Relative path to the current file
	std::string _source_current_file;

    // Constructor/destructor
    Stats();
    ~Stats();

    // Collect statistics 
    void DataTypes(const char* value, StatsSummaryItem *ssi, Token *start, Token *end)  { Add(_data_types, value, ssi, start, end); }
    void DataTypes(Token *token, StatsSummaryItem *ssi, Token *end) { Add(_data_types, token, ssi, end); }
    void DataTypesDetail(Token *start, StatsItem *si)  { Add(_data_types_dtl, start, si); }
    void DataTypesDetail(Token *start, StatsItem *si, Token *end)  { Add(_data_types_dtl, start, si, end); }
	void DataTypesDetail(std::string value, StatsItem *si, const char* target)  { Add(_data_types_dtl, value, target, si); }

    void UdtDataTypes(const char* value, StatsSummaryItem *ssi, Token *start, Token *end)  { Add(_udt_data_types, value, ssi, start, end); }
    void UdtDataTypesDetail(Token *start, Token *end, StatsDetailItem *sdi)  { Add(_udt_data_types_dtl, start, end, sdi, start, end); }
    
    void Statements(const char* value, StatsSummaryItem *ssi, Token *start, Token *end) { Add(_statements, value, ssi, start, end); }
    void Statements(Token *token, StatsSummaryItem *ssi, Token *end) { Add(_statements, token, ssi, end); }
    void CreateTabStatements(const char *value)  { Add(_crtab_statements, value, false); }
    void AlterTabStatements(const char *value)   { Add(_alttab_statements, value, false); }

	void SelectStatementsDetail(const char *value, StatsDetailItem *sdi, Token *start, Token *end) { Add(_select_statements_dtl, value, sdi, start, end); }

	void CreateProcDetail(Token *token, StatsDetailItem *sdi, Token *end) { Add(_crproc_statements_dtl, token, sdi, end); }
	void CreateProcDetail(const char *value, StatsDetailItem *sdi, Token *start, Token *end) { Add(_crproc_statements_dtl, value, sdi, start, end); }
    
	void ProceduralStatements(const char *value, StatsSummaryItem *ssi, Token *start, Token *end) { Add(_pl_statements, value, ssi, start, end); }
    void ProceduralStatements(Token *token, StatsSummaryItem *ssi, Token *end) { Add(_pl_statements, token, ssi, end); }

	void ExceptionsDetail(Token *token, StatsDetailItem *sdi, Token *end) { Add(_pl_statements_exceptions, token, sdi, end); }

	void Functions(const char* value, StatsSummaryItem *ssi, Token *start, Token *end) { Add(_builtin_func, value, ssi, start, end); }
    void Functions(Token *token, StatsSummaryItem *ssi, Token *end) { Add(_builtin_func, token, ssi, end); }
    void FunctionsDetail(const char *value, StatsDetailItem *sdi, Token *start, Token *end) { Add(_builtin_func_dtl, value, sdi, start, end); }
    void UdfFunctions(Token *token, StatsSummaryItem *ssi, Token *end) { Add(_udf_func, token, ssi, end); }

	void Sequences(const char* value, StatsSummaryItem *ssi, Token *start, Token *end)  { Add(_sequences, value, ssi, start, end); }
	void SequencesDetail(const char* value, StatsDetailItem *sdi, Token *start, Token *end)  
		{ Add(_sequences_dtl, value, sdi, start, end); }
	void SequencesOptionsDetail(Token *start, Token *end, StatsDetailItem *sdi)  
		{ Add(_sequences_opt_dtl, start, end, sdi, start, end); }
	void SequencesReference(const char* value, StatsDetailItem *sdi, Token *start, Token *end)  
		{ Add(_sequences_ref, value, sdi, start, end); }
	void SequencesReferenceDetail(Token *start, Token *end, StatsDetailItem *sdi)  
		{ Add(_sequences_ref_dtl, start, end, sdi, start, end); }

    void Procedures(const char* value, StatsSummaryItem *ssi, Token *start, Token *end) { Add(_system_proc, value, ssi, start, end); }
    void Procedures(Token *token, StatsSummaryItem *ssi, Token *end) { Add(_system_proc, token, ssi, end); }
    void ProceduresDetail(const char *value, StatsDetailItem *sdi, Token *start, Token *end)  { Add(_system_proc_dtl, value, sdi, start, end); }

	void Packages(const char *value, StatsSummaryItem *ssi, Token *start, Token *end) { Add(_packages, value, ssi, start, end); }
	void PackagesDetail(Token *token, StatsDetailItem *sdi, Token *end) { Add(_pkg_statements_items, token, sdi, end); }

    void Quoted(Token *token)             { Add(_quoted_idents, token, false); }
	void Non7BitAsciiIdents(Token *token) { Add(_non_7bit_ascii_idents, token); }
    void Strings(Token *token)            { Add(_strings, token, false); }
    void Numbers(Token *token)            { Add(_numbers, token); }
    void Word(Token *token)               { Add(_words, token); }
    void Delimiter(Token *token)          { Add(_delimiters, token); }

	// Log function call with all nested expressions
	void LogFuncCall(Token *name, Token *end, std::string &cur_file);
	// Get meta type for the specified identificator
	const char* GetMetaIdent(Token *name);

    // Collect statistics for the specified item

    void Add(std::map<std::string, StatsItem> &map, Token *token, StatsItem *si, bool case_insense = true);
    void Add(std::map<std::string, StatsItem> &map, Token *start, StatsItem *si, Token *end, bool case_insense = true);
    void Add(std::map<std::string, StatsItem> &map, std::string value, const char* target, StatsItem *si, 
		bool case_insense = true);
    
	void Add(std::map<std::string, StatsSummaryItem> &map, std::string value, int conv_status, bool case_insense = true);
	void Add(std::map<std::string, StatsSummaryItem> &map, std::string value, StatsSummaryItem *item, Token *start, Token *end, bool case_insense = true);
	void Add(std::map<std::string, StatsSummaryItem> &map, Token *token, int conv_status);
	void Add(std::map<std::string, StatsSummaryItem> &map, Token *token, StatsSummaryItem *item, Token *end, bool case_insense = true);
	
	void Add(std::map<std::string, StatsDetailItem> &map, std::string value, StatsDetailItem *item, Token *start, Token *end, bool case_insense = true);
	void Add(std::map<std::string, StatsDetailItem> &map, Token *token, StatsDetailItem *item, Token *end);
	void Add(std::map<std::string, StatsDetailItem> &map, Token *start_val, Token *end_val, StatsDetailItem *item, Token *start, Token *end);
	void Add(std::map<std::string, StatsDetailItem> &map, const char* value, const char* desc, const char* note, const char* link, int conv_status);
	void Add(std::map<std::string, StatsDetailItem> &map, Token *start, Token *end, const char* desc, const char* note, const char* link, int conv_status);

    void Add(std::map<std::string, int> &map, std::string value, bool case_insense = true);
    void Add(std::map<std::string, int> &map, Token *token, bool case_insense = true);
    void Add(std::map<std::string, int> &map, TokenStr *token, bool case_insense = true);

    void Add(std::map<std::string, int> &map, Token *start, Token *end, bool case_insense = true);

	// Set the current source file
	void SetSourceFile(const char *f) { _source_current_file = f; }
};

#endif // sqlexec_stats_h