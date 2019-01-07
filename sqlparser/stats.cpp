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
#include <algorithm>
#include "stats.h"

// Constructor/destructor
Stats::Stats() {}

Stats::~Stats() {}

// Log function call with all nested expressions
void Stats::LogFuncCall(Token *name, Token *end, std::string &cur_file)
{
	if(name == NULL || end == NULL)
		return;

	static bool first_call = true;

	const char *openMode = first_call ? "w" : "a";

	FILE *file = fopen(SQLEXEC_STAT_FILE, openMode);

	if(file != NULL)
	{
		// Line and source name
		fprintf(file, "%s,%lld,%.*s,", cur_file.c_str(), name->line, static_cast<int>(name->len), name->str);

		std::string src, src_meta;
		std::string tgt, tgt_meta;

		Token *cur = name;

		// Build source and target strings
		while(true)
		{
			const char *meta = GetMetaIdent(cur);

			// Source expression
			if(!(cur->flags & TOKEN_INSERTED))
			{
				if(cur->str != NULL)
				{
					src.append(cur->str, cur->len);

					if(meta != NULL)
						src_meta.append(meta);
					else
						src_meta.append(cur->str, cur->len);
				}
				else
				if(cur->chr == '\r' || cur->chr == '\n' || cur->chr == '\t')
				{
					src += ' ';
					src_meta += ' ';
				}
				else
				{
					src += cur->chr;
					src_meta += cur->chr;
				}
			}

			// Target expression
			if(!(cur->flags & TOKEN_REMOVED))
			{
				if(cur->t_str != NULL)
				{
					tgt.append(cur->t_str, cur->t_len);

					if(meta != NULL)
						tgt_meta.append(meta);
					else
						tgt_meta.append(cur->t_str, cur->t_len);
				}
				else
				if(cur->str != NULL)
				{
					tgt.append(cur->str, cur->len);

					if(meta != NULL)
						tgt_meta.append(meta);
					else
						tgt_meta.append(cur->str, cur->len);
				}
				else
				if(cur->chr == '\r' || cur->chr == '\n' || cur->chr == '\t')
				{
					tgt += ' ';
					tgt_meta += ' ';
				}
				else
				{
					tgt += cur->chr;
					tgt_meta += cur->chr;
				}
			}

			if(cur == end)
				break;

			cur = cur->next;
		}
		
		fprintf(file, "<sqlines>%s</sqlines>,<sqlines>%s</sqlines>,<sqlines>%s</sqlines>,<sqlines>%s</sqlines>", src.c_str(), tgt.c_str(), 
			src_meta.c_str(), tgt_meta.c_str());

		fprintf(file, "\n");
		fclose(file);
	}

	first_call = false;
}

// Get meta type for the specified identificator
const char* Stats::GetMetaIdent(Token *name)
{
	if(name == NULL || name->type != TOKEN_IDENT)
		return NULL;

	// Check subtypes first as they are more specific
	if(name->data_subtype == TOKEN_DT2_DATETIME)
		return "expr_datetime";
	else
	if(name->data_subtype == TOKEN_DT2_DATETIME_SEC)
		return "expr_datetime_sec";
	else
	if(name->data_subtype == TOKEN_DT2_DATE)
		return "expr_date";
	else
	if(name->data_type == TOKEN_DT_NUMBER)
		return "expr_number";
	else
	if(name->data_type == TOKEN_DT_DATETIME)
		return "expr_datetime";
	
	return "expr_null";
}

// Collect statistics for the specified item
void Stats::Add(std::map<std::string, StatsItem> &map, Token *token, bool case_insense)
{
    if(token == NULL || token->str == NULL || token->len == 0)
        return;

    std::string source(token->str, token->len);

    bool conv_not_required = (token->flags & TOKEN_CONV_NOT_REQUIRED) ? true : false;
    bool warn = (token->flags & TOKEN_WARNING) ? true : false;

    Add(map, source, token->t_str, case_insense, conv_not_required, warn, token->notes_str);
}

void Stats::Add(std::map<std::string, StatsItem> &map, Token* start, Token* end, bool case_insense)
{
    if(start == NULL || start->str == NULL || start->len == 0 || end == NULL)
        return;

    std::string source(start->str, start->len + start->remain_size - end->remain_size);

    bool conv_not_required = (start->flags & TOKEN_CONV_NOT_REQUIRED) ? true : false;
    bool warn = (start->flags & TOKEN_WARNING) ? true : false;

    Add(map, source, start->t_str, case_insense, conv_not_required, warn, start->notes_str);
}

void Stats::Add(std::map<std::string, StatsItem> &map, std::string value, const char* target, 
                    bool case_insense, bool conv_not_required, bool warn, const char *notes)
{
    if(case_insense)
        std::transform(value.begin(), value.end(), value.begin(), ::toupper);

    // Check if this value already used
	std::map<std::string, StatsItem>::iterator i = map.find(value);

    // Increment the counter or insert new key-value pair
	if(i != map.end())
    	(*i).second.occurrences++;
    else 
	    map[value] = StatsItem(1, target, conv_not_required, warn, notes);    
}

// Collect statistics for the specified item
void Stats::Add(std::map<std::string, int> &map, Token *token, bool case_insense)
{
    if(token == NULL || token->str == NULL || token->len == 0)
        return;

    Add(map, std::string(token->str, token->len), case_insense);
}

void Stats::Add(std::map<std::string, int> &map, TokenStr *token, bool case_insense)
{
    if(token == NULL)
        return;

    Add(map, token->str, case_insense);
}

void Stats::Add(std::map<std::string, int> &map, Token *start, Token *end, bool case_insense )
{
    if(start == NULL || start->str == NULL || start->len == 0)
        return;

    Add(map, std::string(start->str, start->len + start->remain_size - end->remain_size), case_insense);
}

void Stats::Add(std::map<std::string, int> &map, std::string value, bool case_insense)
{
    if(case_insense)
        std::transform(value.begin(), value.end(), value.begin(), ::toupper);

    // Check if this value already used
	std::map<std::string, int>::iterator i = map.find(value);

    // Increment the counter or insert new key-value pair
	if(i != map.end())
    	(*i).second++;
    else 
	    map[value] = 1;    
}
