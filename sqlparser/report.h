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

#ifndef sqlexec_report_h
#define sqlexec_report_h

#include <string>
#include <map>
#include "stats.h"

#define SQLEXEC_REPORT_FILE         "sqlines_report.html"
#define SQLEXEC_REPORT_SNIPPETS     "sqlines_report_snippets.html"
#define SQLEXEC_REPORT_TEMPLATE     "sqlines_report.tpl"

#define SUFFIX(int_value)				((int_value == 1) ? "" : "s")
#define SUFFIX2(int_value, str1, str2)	((int_value == 1) ? str1 : str2)

#define SQLINES_REPORT_OK_MARK      "<span style=\"color:green\"><strong>&#x2713;</strong></span>"
#define SQLINES_REPORT_WARN_MARK    "<span style=\"color:red\"><strong>!</strong></span>"
#define SQLINES_REPORT_ERROR_MARK   "<span style=\"color:red\"><strong>&#x2715;</strong></span>"

class Report
{
    const char *_tpl_path;
    const char *_report_path;
	const char *_report_snippets_path;

    const char *_summary;

	// Source and target databases
	int _source;
	int _target;

	std::string _source_name;
	std::string _target_name;

	// Source and target names used in site URLs
	const char *_source_url;
	const char *_target_url;

public:
    Report();

    // Create a report file by template
    void CreateReport(Stats *stats, int source, int target, const char *summary);

private:
    // Fill the specified report section
    void GetReportSection(Stats *stats, std::string &macro, std::string &data, std::string &snippets); 
    void GetReportSectionRows(std::map<std::string, int> &values, std::string &data, int *distinct, int *occurrences);
    void GetReportSectionRows(std::map<std::string, StatsItem> &values, std::string &data, int *distinct, int *occurrences);
	void GetReportSectionRows(std::string section, std::map<std::string, StatsSummaryItem> &values, std::string &data, std::string &snippets, int *distinct, int *occurrences, bool desc = true, bool note = true, bool complexity = true);
	void GetReportSectionRows(std::string section, std::map<std::string, StatsDetailItem> &values, std::string &data, std::string &snippets, int *distinct, int *occurrences, bool desc = true, bool note = true, bool complexity = true);

	// Check if there is data in section by name
	bool IsDataInSection(Stats *stats, const char *macro);
	// Handle <?ifexists:macro> condition
	const char* ProcessReportIfExists(Stats *stats, const char *cur_tpl, const char *macro);

	// Get the database name to use in reports
	const char *GetDbName(int type);

	// Get the database name used in URL by type
	const char *GetUrlName(int type);

	// Get complexity text
	const char *GetComplexityText(int complexity);
};

#endif // sqlexec_report_h