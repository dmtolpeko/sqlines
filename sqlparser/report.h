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
#define SQLEXEC_REPORT_TEMPLATE     "sqlines_report.tpl"

class Report
{
    const char *_tpl_path;
    const char *_report_path;

    const char *_summary;

public:
    Report();

    // Create a report file by template
    void CreateReport(Stats *stats, const char *summary);

private:
    // Fill the specified report section
    void GetReportSection(Stats *stats, std::string &macro, std::string &data); 
    void GetReportSectionRows(std::map<std::string, int> &values, std::string &data, int *distinct, int *occurrences);
    void GetReportSectionRows(std::map<std::string, StatsItem> &values, std::string &data, int *distinct, int *occurrences);
};

#endif // sqlexec_report_h
