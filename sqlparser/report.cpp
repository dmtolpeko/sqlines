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

#include <stdio.h>
#include <string.h>
#include "report.h"
#include "sqlparser.h"
#include "str.h"
#include "doc.h"
#include "file.h"

// Constructor
Report::Report()
{
    _tpl_path = SQLEXEC_REPORT_TEMPLATE;
    _report_path = SQLEXEC_REPORT_FILE;
	_report_snippets_path = SQLEXEC_REPORT_SNIPPETS;

    _summary = "";
	
	_source = 0;
	_target = 0;
	_source_url = "";
	_target_url = "";
}

// Create a report file by template
void Report::CreateReport(Stats *stats, int source, int target, const char *summary)
{
    if(stats == NULL)
        return;

	_source = source;
	_target = target;
    _summary = summary;

	_source_name = GetDbName(_source);
	_target_name = GetDbName(_target);

	_source_url = GetUrlName(_source);
	_target_url = GetUrlName(_target);

	// Report template file size
	int tpl_size = File::GetFileSize(_tpl_path);

	if(tpl_size == -1)
	{
        printf("\n\nError: Cannot open report template file %s", _tpl_path);
        return;
	}
 
	char *tpl_input = new char[(size_t)tpl_size + 1];

	// Get content of the file (without terminating 'x0')
	if(File::GetContent(_tpl_path, tpl_input, (size_t)tpl_size) == -1)
	{
		delete tpl_input;
		return;
	}

	tpl_input[tpl_size] = '\x0';

	const char *cur = tpl_input;

    FILE *file = fopen(_report_path, "w+");
	FILE *snippets_file = fopen(_report_snippets_path, "wb+");
    
    if(file == NULL)
    {
        printf("\n\nError: Cannot create report file %s", _report_path);
        return;
    }

	if(snippets_file == NULL)
    {
        printf("\n\nError: Cannot create report snippets file %s", _report_snippets_path);
        return;
    }

    while(*cur != '\x0')
    {
        // Check for <?macro?> replacement macro
        if(cur[0] == '<' && cur[1] == '?')
        {
			std::string macro;
            std::string data;
			std::string snippets;

			cur += 2;

            while(*cur != '\x0' && *cur != '?')
            {
				macro += *cur;
				cur++;
            }

			// Check for <?ifexists:macro> condition
			if(_strnicmp(macro.c_str(), "ifexists:", 9) == 0)
				cur = ProcessReportIfExists(stats, cur, macro.c_str() + 9);
			else
			{
				GetReportSection(stats, macro, data, snippets); 

				fprintf(file, "%s", data.c_str());
				fprintf(snippets_file, "%s", snippets.c_str());
			}

			// Consume closing ?>
			if(*cur == '?' && cur[1] == '>')
				cur += 2;
       }
       // Forward character to the report file as is
       else
       {
			fputc(*cur, file);
			fputc(*cur, snippets_file);

			cur++;
		}
    }

	delete tpl_input;

    fclose(file);
	fclose(snippets_file);
}

// Check if there is data in section by name
bool Report::IsDataInSection(Stats *stats, const char *macro)
{
	if((_stricmp(macro, "datatypes_table") == 0 && stats->_data_types.size() > 0) ||
		(_stricmp(macro, "udt_datatypes_table") == 0 && stats->_udt_data_types.size() > 0) ||
		(_stricmp(macro, "udt_datatype_dtl_table") == 0 && stats->_udt_data_types_dtl.size() > 0) ||
		(_stricmp(macro, "datatype_dtl_table") == 0 && stats->_data_types_dtl.size() > 0) ||
		(_stricmp(macro, "builtin_func_table") == 0 && stats->_builtin_func.size() > 0) ||
		(_stricmp(macro, "builtin_func_dtl_table") == 0 && stats->_builtin_func_dtl.size() > 0) ||
		(_stricmp(macro, "udf_func_table") == 0 && stats->_udf_func.size() > 0) ||
		(_stricmp(macro, "seq_table") == 0 && stats->_sequences.size() > 0) ||
		(_stricmp(macro, "seq_dtl_table") == 0 && stats->_sequences_dtl.size() > 0) ||
		(_stricmp(macro, "seq_opt_dtl_table") == 0 && stats->_sequences_opt_dtl.size() > 0) ||
		(_stricmp(macro, "seq_ref_table") == 0 && stats->_sequences_ref.size() > 0) ||
		(_stricmp(macro, "seq_ref_dtl_table") == 0 && stats->_sequences_ref_dtl.size() > 0) ||
		(_stricmp(macro, "system_proc_table") == 0 && stats->_system_proc.size() > 0) ||
		(_stricmp(macro, "system_proc_dtl_table") == 0 && stats->_system_proc_dtl.size() > 0) ||
		(_stricmp(macro, "statements_table") == 0 && stats->_statements.size() > 0) ||
		(_stricmp(macro, "crtab_stmt_table") == 0 && stats->_crtab_statements.size() > 0) ||
		(_stricmp(macro, "alttab_stmt_table") == 0 && stats->_alttab_statements.size() > 0) ||
		(_stricmp(macro, "select_stmt_table") == 0 && stats->_select_statements_dtl.size() > 0) ||
		(_stricmp(macro, "crproc_stmt_table") == 0 && stats->_crproc_statements_dtl.size() > 0) ||
		(_stricmp(macro, "pl_statements") == 0 && stats->_pl_statements.size() > 0) ||
		(_stricmp(macro, "pl_statements_exceptions") == 0 && stats->_pl_statements_exceptions.size() > 0) ||
		(_stricmp(macro, "packages") == 0 && stats->_packages.size() > 0) ||
		(_stricmp(macro, "pkg_statements_items") == 0 && stats->_pkg_statements_items.size() > 0) ||
		(_stricmp(macro, "quoted_idents_table") == 0 && stats->_quoted_idents.size() > 0) ||
		(_stricmp(macro, "nonascii_idents") == 0 && stats->_non_7bit_ascii_idents.size() > 0) ||
		(_stricmp(macro, "strings_table") == 0 && stats->_strings.size() > 0) ||
		(_stricmp(macro, "numbers_table") == 0 && stats->_numbers.size() > 0) ||
		(_stricmp(macro, "words_table") == 0 && stats->_words.size() > 0) ||
		(_stricmp(macro, "delimiters_table") == 0 && stats->_delimiters.size() > 0))
		return true;

	return false;
}

// Handle <?ifexists:macro> condition
const char* Report::ProcessReportIfExists(Stats *stats, const char *cur_tpl, const char *macro)
{
	const char *cur = cur_tpl;

	// Skip the entire section if there is no data for it
	if(!IsDataInSection(stats, macro))
	{
		while(*cur != '\x0')
		{
			// Check for <?/...> end of ifexists
			if(cur[0] == '<' && cur[1] == '?' && cur[2] == '/')
			{
				std::string end_macro;

				cur += 3;

				while(*cur != '\x0' && *cur != '?')
				{
					end_macro += *cur;
					cur++;
				}

				if(_strnicmp(end_macro.c_str(), "ifexists:", 9) == 0 && strcmp(macro, end_macro.c_str() + 9) == 0)
				{
					// Consume closing >
					if(*cur == '>')
						cur++;

					break;
				}
			}
			else
				cur++;
       }
	}

	return cur;
}

// Fill the specified report section
void Report::GetReportSection(Stats *stats, std::string &macro, std::string &data, std::string &snippets)
{
    char num[12];

    int distinct = 0;
    int occurrences = 0; 

    // Table of all data types
	if(_stricmp(macro.c_str(), "datatypes_table") == 0 && stats->_data_types.size() > 0)
    {
		data += "<table><tr><th></th><th>" + _source_name + " Data Type</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_data_types, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct data types, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all user-defined and derived data types
    if(_stricmp(macro.c_str(), "udt_datatypes_table") == 0 && stats->_udt_data_types.size() > 0)
    {
		data += "<table><tr><th></th><th>" + _source_name + " Data Type</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_udt_data_types, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " types, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all user-defined and derived data type details
    if(_stricmp(macro.c_str(), "udt_datatype_dtl_table") == 0 && stats->_udt_data_types_dtl.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Type</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_udt_data_types_dtl, data, snippets, &distinct, &occurrences, false, false, false);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " types, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of data type details
    if(_stricmp(macro.c_str(), "datatype_dtl_table") == 0 && stats->_data_types_dtl.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Data Type</th><th>" + _target_name + " Type</th><th>Notes</th><th>Occurrences</th></tr>\n";

        GetReportSectionRows(stats->_data_types_dtl, data, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct data type specifications, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all built-in functions
    if(_stricmp(macro.c_str(), "builtin_func_table") == 0 && stats->_builtin_func.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Function</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_builtin_func, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct functions, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all built-in functions details
    if(_stricmp(macro.c_str(), "builtin_func_dtl_table") == 0 && stats->_builtin_func_dtl.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Function</th><th>Occurrences</th></tr>";

        GetReportSectionRows(macro, stats->_builtin_func_dtl, data, snippets, &distinct, &occurrences, false, false, false);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " function specifications, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all user-defined functions
    if(_stricmp(macro.c_str(), "udf_func_table") == 0 && stats->_udf_func.size() > 0)
    {
        data += "<table><tr><th></th><th>Function</th><th>Occurrences</th></tr>";

		GetReportSectionRows(macro, stats->_udf_func, data, snippets, &distinct, &occurrences, false, false, false);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct functions, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
	else
    // Sequences
    if(_stricmp(macro.c_str(), "seq_table") == 0 && stats->_sequences.size() > 0)
    {
		data += "<table><tr><th></th><th>" + _source_name + " Statement</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_sequences, data, snippets, &distinct, &occurrences);

		data += "</table>";
        data += "<p><b>Total:</b> ";
		sprintf(num, "%d", distinct);
        data += num;
        data += " distinct statement";
		data += SUFFIX(distinct);
		data += ", ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrence";
		data += SUFFIX(occurrences);
		data += ".</p>";
    }
	else
    // Sequence options
    if(_stricmp(macro.c_str(), "seq_dtl_table") == 0 && stats->_sequences_dtl.size() > 0)
    {
        data += "<table><tr><th></th><th>"+ _source_name + " Option</th><th>Description</th><th>"+ _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_sequences_dtl, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
		sprintf(num, "%d", distinct);
        data += num;
        data += " distinct option";
		data += SUFFIX(distinct);
		data += ", ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrence";
		data += SUFFIX(occurrences);
		data += ".</p>";
    }
	else
    // Sequence option details
    if(_stricmp(macro.c_str(), "seq_opt_dtl_table") == 0 && stats->_sequences_opt_dtl.size() > 0)
    {
        data += "<table><tr><th></th><th>Option</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_sequences_opt_dtl, data, snippets, &distinct, &occurrences, false, false, false);

        data += "</table>";
        data += "<p><b>Total:</b> ";
		sprintf(num, "%d", distinct);
        data += num;
        data += " distinct option value";
		data += SUFFIX(distinct);
		data += ", ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrence";
		data += SUFFIX(occurrences);
		data += ".</p>";
    }
	else
    // Sequence references
    if(_stricmp(macro.c_str(), "seq_ref_table") == 0 && stats->_sequences_ref.size() > 0)
    {
        data += "<table><tr><th></th><th>Reference</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_sequences_ref, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
		sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrence";
		data += SUFFIX(occurrences);
		data += ".</p>";
    }
	else
    // Sequence reference details
    if(_stricmp(macro.c_str(), "seq_ref_dtl_table") == 0 && stats->_sequences_ref_dtl.size() > 0)
    {
        data += "<table><tr><th></th><th>Reference</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_sequences_ref_dtl, data, snippets, &distinct, &occurrences, false, false, false);

        data += "</table>";
        data += "<p><b>Total:</b> ";
		sprintf(num, "%d", distinct);
        data += num;
        data += " distinct reference";
		data += SUFFIX(distinct);
		data += ", ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrence";
		data += SUFFIX(occurrences);
		data += ".</p>";
    }
    else
    // Table of all system procedures
    if(_stricmp(macro.c_str(), "system_proc_table") == 0 && stats->_system_proc.size() > 0)
    {
		data += "<table><tr><th></th><th>" + _source_name + " Procedure</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_system_proc, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct procedures, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all system procedures details
    if(_stricmp(macro.c_str(), "system_proc_dtl_table") == 0 && stats->_system_proc_dtl.size() > 0)
    {
		data += "<table><tr><th></th><th>" + _source_name + " Procedure</th><th>Occurrences</th></tr>";

        GetReportSectionRows(macro, stats->_system_proc_dtl, data, snippets, &distinct, &occurrences, false, false, false);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct procedures, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all statements
    if(_stricmp(macro.c_str(), "statements_table") == 0 && stats->_statements.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Statement</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_statements, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct statements, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of CREATE TABLE statements details
    if(_stricmp(macro.c_str(), "crtab_stmt_table") == 0 && stats->_crtab_statements.size() > 0)
    {
        data += "<table><tr><th></th><th>Details</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_crtab_statements, data, &distinct, &occurrences);

        data += "</table>";
    }
    else
    // Table of ALTER TABLE statements details
    if(_stricmp(macro.c_str(), "alttab_stmt_table") == 0 && stats->_alttab_statements.size() > 0)
    {
        data += "<table><tr><th></th><th>Details</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_alttab_statements, data, &distinct, &occurrences);

        data += "</table>";
    }
	else
	// SELECT statements details
	if(_stricmp(macro.c_str(), "select_stmt_table") == 0 && stats->_select_statements_dtl.size() > 0)
    {
        data += "<table><tr><th></th><th>Issue</th><th>Description</th><th>Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_select_statements_dtl, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct issues, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
	else
    // Create procedure details
    if(_stricmp(macro.c_str(), "crproc_stmt_table") == 0 && stats->_crproc_statements_dtl.size() > 0)
    {
        data += "<table><tr><th></th><th>Issue</th><th>Description</th><th>Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_crproc_statements_dtl, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct issues, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all procedural statements
    if(_stricmp(macro.c_str(), "pl_statements") == 0 && stats->_pl_statements.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Statement</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_pl_statements, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct statements, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
	else
    // Exceptions details
    if(_stricmp(macro.c_str(), "pl_statements_exceptions") == 0 && stats->_pl_statements_exceptions.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Handler</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_pl_statements_exceptions, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct exception conditions, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
	else
    // Table of all built-in packages
    if(_stricmp(macro.c_str(), "packages") == 0 && stats->_packages.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Package</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_packages, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct packages, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
	else
    // Table of all built-in packages details
    if(_stricmp(macro.c_str(), "pkg_statements_items") == 0 && stats->_pkg_statements_items.size() > 0)
    {
        data += "<table><tr><th></th><th>" + _source_name + " Package</th><th>Description</th><th>" + _target_name + " Conversion</th><th>Complexity</th><th>Count</th></tr>";

        GetReportSectionRows(macro, stats->_pkg_statements_items, data, snippets, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct items, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all double quoted identifiers
    if(_stricmp(macro.c_str(), "quoted_idents_table") == 0 && stats->_quoted_idents.size() > 0)
    {
        data += "<table><tr><th></th><th>Identifier</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_quoted_idents, data, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct quoted identifiers, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
	else
    // Table of all non-ASCII 7-bit identifiers
    if(_stricmp(macro.c_str(), "nonascii_idents") == 0 && stats->_non_7bit_ascii_idents.size() > 0)
    {
        data += "<table><tr><th></th><th>Identifier</th><th>Count</th></tr>";

        GetReportSectionRows(stats->_non_7bit_ascii_idents, data, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct identifiers, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all single quoted string literals
    if(_stricmp(macro.c_str(), "strings_table") == 0 && stats->_strings.size() > 0)
    {
        data += "<table><tr><th></th><th>String Literal</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_strings, data, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct string literals, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all numeric literals
    if(_stricmp(macro.c_str(), "numbers_table") == 0 && stats->_numbers.size() > 0)
    {
        data += "<table><tr><th></th><th>Numeric Literal</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_numbers, data, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct numeric literals, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all words
    if(_stricmp(macro.c_str(), "words_table") == 0 && stats->_words.size() > 0)
    {
        data += "<table><tr><th></th><th>Words</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_words, data, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct words, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Table of all delimiters
    if(_stricmp(macro.c_str(), "delimiters_table") == 0 && stats->_delimiters.size() > 0)
    {
        data += "<table><tr><th></th><th>Delimiters</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_delimiters, data, &distinct, &occurrences);

        data += "</table>";
        data += "<p><b>Total:</b> ";
        sprintf(num, "%d", distinct);
        data += num;
        data += " distinct delimiters, ";
        sprintf(num, "%d", occurrences);
        data += num;
        data += " occurrences.</p>";
    }
    else
    // Summary
    if(_stricmp(macro.c_str(), "summary") == 0)
        data += _summary;
}

// Get report section
void Report::GetReportSectionRows(std::string section, std::map<std::string, StatsSummaryItem> &values, std::string &data, 
									std::string &snippets, int *distinct, int *occurrences,
									bool desc, bool note, bool complexity)
{
    char num[12];

    int row = 1;
    int dist = 0;
    int occur = 0;

    for(std::map<std::string, StatsSummaryItem>::iterator i = values.begin(); i != values.end(); i++, row++)
    {
        data += "<tr><td>";
        sprintf(num, "%d", row);
        data += num;
        data += "</td><td><a href=\"";
		data += _report_snippets_path;
		data += "#";
		data += section;
		data += "_";
		data += (*i).first;
		data += "\">";
        data += (*i).first;
        data += "</a></td><td>";

		StatsSummaryItem &item = (*i).second;
		int cnt = 0;

		if(desc)
		{
			data += item.desc;
			data += "</td><td>";
		}

		// Conversion note
		if(note)
		{
			// It is undefined whether conversion is required or not
			if(item.conv_undef > 0)
			{
				data += "<span class=\"td_warn1\">Requires Evaluation: ";
				sprintf(num, "%d", item.conv_undef);
				data += num;
				data += "</span> ";
				data += SQLINES_REPORT_WARN_MARK;
				cnt++;
			}

			// Conversion with warnings
			if(item.conv_warn > 0)
			{
				if(cnt > 0)
					data += " ";

				data += "Warnings: ";
				sprintf(num, "%d", item.conv_warn);
				data += num;
				data += " ";
				data += SQLINES_REPORT_WARN_MARK;
				cnt++;
			}

			// Successful conversion
			if(item.conv_ok > 0)
			{
				if(cnt > 0)
					data += " ";

				data += "SQLines Can Convert: ";
				sprintf(num, "%d", item.conv_ok);
				data += num;
				data += " ";
				data += SQLINES_REPORT_OK_MARK;
				cnt++;
			}

			// Conversion is not required
			if(item.conv_no_need > 0)
			{
				if(cnt > 0)
					data += " ";

				data += "No Conversion Required: ";
				sprintf(num, "%d", item.conv_no_need);
				data += num;
				data += " ";
				data += SQLINES_REPORT_OK_MARK;
				cnt++;
			}
				
			// Manual conversion is required
			if(item.conv_error > 0)
			{
				if(item.conv_undef > 0 || item.conv_no_need > 0 || item.conv_ok > 0)
					data += " ";

				data += "Cannot Migrate As Is: ";
				sprintf(num, "%d", item.conv_error);
				data += num;
				data += " ";
				data += SQLINES_REPORT_ERROR_MARK;
			}

			data += "</td><td>";
		}

		// Complexity note
		if(complexity)
		{
			if(item.complexity_very_low > 0)
			{
				data += "Very low: ";
				sprintf(num, "%d", item.complexity_very_low);
				data += num;
			}

			if(item.complexity_low > 0)
			{
				if(item.complexity_very_low > 0)
					data += ", ";

				data += "Low: ";
				sprintf(num, "%d", item.complexity_low);
				data += num;
			}

			if(item.complexity_medium > 0)
			{
				if(item.complexity_very_low > 0 || item.complexity_low > 0)
					data += ", ";

				data += "Medium: ";
				sprintf(num, "%d", item.complexity_medium);
				data += num;
			}

			if(item.complexity_high > 0)
			{
				if(item.complexity_very_low > 0 || item.complexity_low > 0 || item.complexity_medium > 0)
					data += ", ";

				data += "High: ";
				sprintf(num, "%d", item.complexity_high);
				data += num;
			}
	
			if(item.complexity_very_high > 0)
			{
				if(item.complexity_very_low > 0 || item.complexity_low > 0 || item.complexity_medium > 0 ||
					item.complexity_high > 0)
					data += ", ";

				data += "Very high: ";
				sprintf(num, "%d", item.complexity_very_high);
				data += num;
			}

			data += "</td><td>";
		}

        sprintf(num, "%d", item.occurrences);
        data += num;
        data += "</td></tr>\n";

        dist++;
        occur += item.occurrences;   

		int srow = 1;

		snippets += "<a name=\"";
		snippets += section;
		snippets += "_";
		snippets += (*i).first;
		snippets += "\"></a>";

		snippets += "<p>";
		snippets += (*i).first;

		if(desc && !item.desc.empty())
		{
			snippets += " - ";
			snippets += item.desc;
		}

		snippets += "</p>";

		snippets += "<table><th></th><th>Filename:line</th><th>Source Code Snippet</th>";

		// Get code snippets
		for(std::list<StatsSnippetItem>::iterator s = item.snippets.begin(); s != item.snippets.end(); s++, srow++)
		{
			StatsSnippetItem sitem = *s;

			snippets += "<tr><td>";
			sprintf(num, "%d", srow);
			snippets += num;
			snippets += "</td><td>";

			snippets += sitem.filename;
			snippets += ":";
			sprintf(num, "%d", sitem.line);
			snippets += num;

			snippets += "</td><td><pre>";
			snippets += sitem.snippet;
			snippets += "</pre>";

			snippets += "</td></tr>\n";
		}

		snippets += "</table>";
    }

    if(distinct != NULL)
        *distinct = dist;
    
    if(occurrences != NULL)
        *occurrences = occur;
}

// Get report section
void Report::GetReportSectionRows(std::string section, std::map<std::string, StatsDetailItem> &values, std::string &data, 
									std::string &snippets, int *distinct, int *occurrences, bool desc, bool note,
									bool complexity)
{
    char num[12];

    int row = 1;
    int dist = 0;
    int occur = 0;

    for(std::map<std::string, StatsDetailItem>::iterator i = values.begin(); i != values.end(); i++, row++)
    {
 		data += "<tr><td>";
        sprintf(num, "%d", row);
        data += num;
        data += "</td><td><a href=\"";
		data += _report_snippets_path;
		data += "#";
		data += section;
		data += "_";
		data += (*i).first;
		data += "\">";
        data += (*i).first;
        data += "</a></td>";

		StatsDetailItem &item = (*i).second;

		// Description
		if(desc)
		{
			data += "<td>";
			data += item.desc;
			data += "</td>";
		}

		// Conversion note
		if(note)
		{
			data += "<td>";

			// Check if there is a documentation link associated with the conversion note
			if(!item.link.empty())
			{
				data += "<a href=\"";
				data += SQLINES_URL;
				data += "/";
				data += _source_url;
				data += "-to-";
				data += _target_url;
				data += "/";
				data += item.link;
				data += "\" target=\"_blank\">";
			}

			if(!item.note.empty())
				data += item.note;
			else
			if(item.conv_status == STATS_CONV_NO_NEED)
				data += "No Conversion Required";
			else
			if(item.conv_status == STATS_CONV_ERROR)
				data += "Cannot Migrate As Is";

			if(!item.link.empty())
				data += "</a>";

			data += " ";

			if(item.conv_status == STATS_CONV_NO_NEED || item.conv_status == STATS_CONV_OK)
				data += SQLINES_REPORT_OK_MARK;
			else
			if(item.conv_status == STATS_CONV_WARN)
				data += SQLINES_REPORT_WARN_MARK;
			else
			if(item.conv_status == STATS_CONV_ERROR)
				data += SQLINES_REPORT_ERROR_MARK;

			data += "</td>";
		}

		if(complexity)
		{
			data += "<td>";
			data += GetComplexityText(item.complexity);
			data += "</td>";
		}

		data += "<td>";
        sprintf(num, "%d", item.count);
        data += num;
        data += "</td></tr>";		

        dist++;
        occur += item.count; 

		int srow = 1;

		snippets += "<a name=\"";
		snippets += section;
		snippets += "_";
		snippets += (*i).first;
		snippets += "\"></a>";

		snippets += "<p>";
		snippets += (*i).first;
		snippets += " - ";
		snippets += item.desc;
		snippets += "</p>";

		snippets += "<table><th></th><th>Filename:line</th><th>Source Code Snippet</th>";

		// Get code snippets
		for(std::list<StatsSnippetItem>::iterator s = item.snippets.begin(); s != item.snippets.end(); s++, srow++)
		{
			StatsSnippetItem sitem = *s;

			snippets += "<tr><td>";
			sprintf(num, "%d", srow);
			snippets += num;
			snippets += "</td><td>";

			snippets += sitem.filename;
			snippets += ":";
			sprintf(num, "%d", sitem.line);
			snippets += num;

			snippets += "</td><td><pre>";
			snippets += sitem.snippet;
			snippets += "</pre>";

			snippets += "</td></tr>\n";
		}

		snippets += "</table>";
    }

    if(distinct != NULL)
        *distinct = dist;
    
    if(occurrences != NULL)
        *occurrences = occur;
}

// Get report section
void Report::GetReportSectionRows(std::map<std::string, StatsItem> &values, std::string &data,
                                  int *distinct, int *occurrences)
{
    char num[12];

    int row = 1;
    int dist = 0;
    int occur = 0;

    for(std::map<std::string, StatsItem>::iterator i = values.begin(); i != values.end(); i++, row++)
    {
        data += "<tr><td>";
        sprintf(num, "%d", row);
        data += num;
        data += "</td><td>";
        data += (*i).first;
        data += "</td>";

        // The source was modified
        if(!(*i).second.t_value.empty())
        {
			data += "<td>";
            data += (*i).second.t_value;
			data += "</td><td>";
                
			if((*i).second.conv_status == STATS_CONV_ERROR)
            {
                data += "Cannot Migrate As Is ";
				data += SQLINES_REPORT_ERROR_MARK;
            }
            else
            {
                data += "SQLines Can Convert ";
				data += SQLINES_REPORT_OK_MARK;
            }
        }
        // Source was not changed
        else
        {
			data += "<td>";
            data += (*i).first;
			data += "</td><td>";

			if((*i).second.conv_status == STATS_CONV_NO_NEED)
            {
				data += "No Conversion Required ";
				data += SQLINES_REPORT_OK_MARK;
            }
			else
			if((*i).second.conv_status == STATS_CONV_OK)
            {
				data += "SQLines Can Convert ";
				data += SQLINES_REPORT_OK_MARK;
            }
            else
            {                
                data += "Requires Evaluation ";
				data += SQLINES_REPORT_WARN_MARK;
            }			
        }

        data += "</td><td>";
        sprintf(num, "%d", (*i).second.occurrences);
        data += num;
        data += "</td></tr>\n";

        dist++;
        occur += (*i).second.occurrences;        
    }

    if(distinct != NULL)
        *distinct = dist;
    
    if(occurrences != NULL)
        *occurrences = occur;
}

void Report::GetReportSectionRows(std::map<std::string, int> &values, std::string &data,
                                  int *distinct, int *occurrences)
{
    char num[12];

    int row = 1;
    int dist = 0;
    int occur = 0;

    for(std::map<std::string, int>::iterator i = values.begin(); i != values.end(); i++, row++)
    {
        data += "<tr><td>";
        sprintf(num, "%d", row);
        data += num;
        data += "</td><td>";
        sprintf(num, "%d", (*i).second);
        data += (*i).first;
        data += "</td><td>";
        data += num;
        data += "</td></tr>";

        dist++;
        occur += (*i).second;        
    }

    if(distinct != NULL)
        *distinct = dist;
    
    if(occurrences != NULL)
        *occurrences = occur;
}

// Get the database name to use in reports
const char* Report::GetDbName(int type)
{
	if(type == SQL_SQL_SERVER)
		return "SQL Server";
	else
	if(type == SQL_ORACLE)
		return "Oracle";
	else
	if(type == SQL_DB2)
		return "IBM DB2";		
	else
	if(type == SQL_MYSQL)
		return "MySQL";
	else
	if(type == SQL_POSTGRESQL)
		return "PostgreSQL";
	else
	if(type == SQL_SYBASE)
		return "Sybase";
	else
	if(type == SQL_INFORMIX)
		return "Informix";
	else
	if(type == SQL_GREENPLUM) 
		return "Greenplum";
	else
	if(type == SQL_SYBASE_ASA) 
		return  "Sybase ASA";
	else
	if(type == SQL_TERADATA) 
		return "Teradata";
	else
	if(type == SQL_NETEZZA) 
		return "Netezza";
    else
	if(type == SQL_MARIADB) 
		return "MariaDB";
    else
	if(type == SQL_MARIADB_ORA) 
		return "MariaDB";
    else
	if(type == SQL_HIVE) 
		return "Hive";
    else
	if(type == SQL_REDSHIFT) 
		return "Redshift";
    else
	if(type == SQL_ESGYNDB) 
		return "Esgyndb";
	else
	if(type == SQL_SYBASE_ADS) 
		return "Sybase ADS";

	return "";
}

// Get the database name used in URL by type
const char* Report::GetUrlName(int type)
{
	if(type == SQL_SQL_SERVER)
		return "sql-server";
	else
	if(type == SQL_ORACLE)
		return "oracle";
	else
	if(type == SQL_DB2)
		return "db2";		
	else
	if(type == SQL_MYSQL)
		return "mysql";
	else
	if(type == SQL_POSTGRESQL)
		return "postgresql";
	else
	if(type == SQL_SYBASE)
		return "sybase";
	else
	if(type == SQL_INFORMIX)
		return "informix";
	else
	if(type == SQL_GREENPLUM) 
		return "greenplum";
	else
	if(type == SQL_SYBASE_ASA) 
		return  "sybase-asa";
	else
	if(type == SQL_TERADATA) 
		return "teradata";
	else
	if(type == SQL_NETEZZA) 
		return "netezza";
    else
	if(type == SQL_MARIADB) 
		return "mariadb";
    else
	if(type == SQL_MARIADB_ORA) 
		return "mariadb-compatibility";
    else
	if(type == SQL_HIVE) 
		return "hive";
    else
	if(type == SQL_REDSHIFT) 
		return "redshift";
    else
	if(type == SQL_ESGYNDB) 
		return "esgyndb";
	else
	if(type == SQL_SYBASE_ADS) 
		return "sybase-ads";

	return "";
}

// Get complexity text
const char *Report::GetComplexityText(int complexity)
{
	const char *text = "";

	switch(complexity) 
	{ 
		case STATS_CONV_VERY_LOW:
			text = "Very Low";
			break;
		case STATS_CONV_LOW:
			text = "Low";
			break;
		case STATS_CONV_MEDIUM:
			text = "Medium";
			break;
		case STATS_CONV_HIGH:
			text = "High";
			break;
		case STATS_CONV_VERY_HIGH: 
			text = "Very High";
			break;
	}

	return text;
}