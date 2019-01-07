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
#include "../sqlcommon/str.h"

// Constructor
Report::Report()
{
    _tpl_path = SQLEXEC_REPORT_TEMPLATE;
    _report_path = SQLEXEC_REPORT_FILE;
    _summary = "";
}

// Create a report file by template
void Report::CreateReport(Stats *stats, const char *summary)
{
    if(stats == NULL)
        return;

    _summary = summary;

    FILE *tpl_file = fopen(_tpl_path, "r");

    if(tpl_file == NULL)
    {
        printf("\n\nError: Cannot open report template file %s", _tpl_path);
        return;
    }

    FILE *file = fopen(_report_path, "w+");
    
    if(file == NULL)
    {
        printf("\n\nError: Cannot create report file %s", _report_path);
        fclose(tpl_file);
        return;
    }

    int c = 0;

    while((c = fgetc(tpl_file)) != EOF)
    {
        // Check for <?macro?> replacement macro
        if((char)c == '<')
        {
            int c2 = fgetc(tpl_file);

            if((char)c2 == '?')
            {
                std::string macro;
                std::string data;

                while(true)
                {
                    c = fgetc(tpl_file);
                    if(c == EOF || (char)c == '?')
                        break;

                    macro += (char)c;
                }

                GetReportSection(stats, macro, data); 
                fprintf(file, "%s", data.c_str());

                // Consume closing >
                if(c != EOF)
                    fgetc(tpl_file);
            }
            // Forward character to the report file as is
            else
            {
                fputc(c, file);
                if((char)c2 != EOF)
                    fputc(c2, file);
                else
                    break;
            }
        }
        else
            fputc(c, file);
    }

    fclose(tpl_file);
    fclose(file);
}

// Fill the specified report section
void Report::GetReportSection(Stats *stats, std::string &macro, std::string &data)
{
    char num[12];

    int distinct = 0;
    int occurrences = 0; 

    // Table of all data types
    if(_stricmp(macro.c_str(), "datatypes_table") == 0)
    {
        data += "<table><tr><th></th><th>Data Type</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_data_types, data, &distinct, &occurrences);

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
    if(_stricmp(macro.c_str(), "udt_datatypes_table") == 0)
    {
        data += "<table><tr><th></th><th>Type</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_udt_data_types, data, &distinct, &occurrences);

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
    if(_stricmp(macro.c_str(), "udt_datatype_dtl_table") == 0)
    {
        data += "<table><tr><th></th><th>Type</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_udt_data_types_dtl, data, &distinct, &occurrences);

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
    if(_stricmp(macro.c_str(), "datatype_dtl_table") == 0)
    {
        data += "<table><tr><th></th><th>Data Type</th><th>Conversion</th><th>Notes</th><th>Occurrences</th></tr>\n";

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
    if(_stricmp(macro.c_str(), "builtin_func_table") == 0)
    {
        data += "<table><tr><th></th><th>Function</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_builtin_func, data, &distinct, &occurrences);

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
    if(_stricmp(macro.c_str(), "builtin_func_dtl_table") == 0)
    {
        data += "<table><tr><th></th><th>Function</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_builtin_func_dtl, data, &distinct, &occurrences);

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
    if(_stricmp(macro.c_str(), "udf_func_table") == 0)
    {
        data += "<table><tr><th></th><th>Function</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_udf_func, data, &distinct, &occurrences);

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
    // Table of all system procedures
    if(_stricmp(macro.c_str(), "system_proc_table") == 0)
    {
        data += "<table><tr><th></th><th>Procedure</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_system_proc, data, &distinct, &occurrences);

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
    if(_stricmp(macro.c_str(), "system_proc_dtl_table") == 0)
    {
        data += "<table><tr><th></th><th>Procedure</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_system_proc_dtl, data, &distinct, &occurrences);

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
    if(_stricmp(macro.c_str(), "statements_table") == 0)
    {
        data += "<table><tr><th></th><th>Statement</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_statements, data, &distinct, &occurrences);

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
    if(_stricmp(macro.c_str(), "crtab_stmt_table") == 0)
    {
        data += "<table><tr><th></th><th>Details</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_crtab_statements, data, &distinct, &occurrences);

        data += "</table>";
    }
    else
    // Table of ALTER TABLE statements details
    if(_stricmp(macro.c_str(), "alttab_stmt_table") == 0)
    {
        data += "<table><tr><th></th><th>Details</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_alttab_statements, data, &distinct, &occurrences);

        data += "</table>";
    }
    else
    // Table of all procedural statements
    if(_stricmp(macro.c_str(), "pl_statements_table") == 0)
    {
        data += "<table><tr><th></th><th>Statement</th><th>Occurrences</th></tr>";

        GetReportSectionRows(stats->_pl_statements, data, &distinct, &occurrences);

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
    // Table of all double quoted identifiers
    if(_stricmp(macro.c_str(), "quoted_idents_table") == 0)
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
    // Table of all single quoted string literals
    if(_stricmp(macro.c_str(), "strings_table") == 0)
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
    if(_stricmp(macro.c_str(), "numbers_table") == 0)
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
    if(_stricmp(macro.c_str(), "words_table") == 0)
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
    if(_stricmp(macro.c_str(), "delimiters_table") == 0)
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
            if((*i).second.warn)
            {
                data += "<td class=\"td_warn\">";
                data += (*i).second.t_value;
                data += "</td><td class=\"td_warn\">";
                data += (*i).second.notes;
            }
            else
            {
                data += "<td>";
                data += (*i).second.t_value;
                data += "</td><td>Ok";
            }
        }
        // Source was not changed
        else
        {
            if(!(*i).second.conv_not_required)
            {
                data += "<td class=\"td_warn\">";
                data += (*i).first;
                data += "</td><td class=\"td_warn\">Not converted";
            }
            else
            {
                data += "<td>";
                data += (*i).first;
                data += "</td><td>Conversion is not required";
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
