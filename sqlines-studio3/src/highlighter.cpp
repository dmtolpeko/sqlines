/**
 * Copyright (c) 2021 SQLines
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

#include <utility>
#include <QRegularExpressionMatch>
#include <QColor>

#include "highlighter.hpp"

using namespace uiImpl;

Highlighter::Highlighter(QTextDocument& parent) noexcept
 :  QSyntaxHighlighter(&parent),
    keywords(setKeywords()),
    functions(setFunctions())
{
    setKeywordHighlighting();
    setDigitHighlighting();
    setQuotationHighlighting();
    setCommentHighlighting();
}

void Highlighter::highlightBlock(const QString &text)
{
    // Keyword, function, number and expression highlighting
    for (const auto& rule : this->highlightingRules) {
        auto i = rule.first.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(),
                      match.capturedLength(),
                      rule.second);
        }
    }
    
    // Comment highlighting
    setCurrentBlockState(0);
    auto startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(this->commentStartExpression);
    }
    
    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch;
        auto endIndex = text.indexOf(this->commentEndExpression,
                                    startIndex,
                                    &endMatch);
        auto length = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            length = text.length() - startIndex;
        } else {
            length = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, length, this->multiLineCommentFormat);
        startIndex = text.indexOf(this->commentStartExpression,
                                  startIndex + length);
    }
}

void Highlighter::setKeywordHighlighting() noexcept
{
    QTextCharFormat format;
    format.setForeground(QColor(Qt::blue).lighter(140));
    format.setFontWeight(QFont::Bold);
    
    for (const auto& keyword : this->keywords) {
        QRegularExpression pattern(QString("\\b%1\\b").arg(keyword),
                                   QRegularExpression::CaseInsensitiveOption);
        
        this->highlightingRules.emplace_back(std::move(pattern), format);
    }
        
    for (const auto& function : this->functions) {
        QRegularExpression pattern(QString("\\b%1\\b").arg(function),
                                   QRegularExpression::CaseInsensitiveOption);
            
        this->highlightingRules.emplace_back(std::move(pattern),
                                             std::move(format));
    }
}

void Highlighter::setDigitHighlighting() noexcept
{
    QRegularExpression pattern("\\b[0-9]+\\b");
    QTextCharFormat format;
    format.setForeground(Qt::darkMagenta);
    format.setFontWeight(QFont::Bold);
    
    this->highlightingRules.emplace_back(std::move(pattern),
                                         std::move(format));
}

void Highlighter::setQuotationHighlighting() noexcept
{
    QRegularExpression quotationPattern("\".*\"");
    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(QColor("orange").darker(120));
    quotationFormat.setFontWeight(QFont::Bold);
    
    QRegularExpression singleQuotationPattern("'.*'");
    QTextCharFormat singleQuotationFormat;
    singleQuotationFormat.setForeground(QColor("orange").darker(120));
    singleQuotationFormat.setFontWeight(QFont::Bold);
    
    this->highlightingRules.emplace_back(std::move(quotationPattern),
                                         std::move(quotationFormat));
    
    this->highlightingRules.emplace_back(std::move(singleQuotationPattern),
                                         std::move(singleQuotationFormat));

}

void Highlighter::setCommentHighlighting() noexcept
{
    QRegularExpression singleLineCommentPattern("//[^\n]*");
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    
    QRegularExpression singleLineCommentPattern2("--[^\n]*");
    QTextCharFormat singleLineCommentFormat2;
    singleLineCommentFormat2.setForeground(Qt::darkGreen);
    
    this->highlightingRules.emplace_back(std::move(singleLineCommentPattern),
                                         std::move(singleLineCommentFormat));
    
    this->highlightingRules.emplace_back(std::move(singleLineCommentPattern2),
                                         std::move(singleLineCommentFormat2));
    
    this->multiLineCommentFormat.setForeground(Qt::darkGreen);
    this->commentStartExpression = QRegularExpression("/\\*");
    this->commentEndExpression = QRegularExpression("\\*/");
}


QStringList Highlighter::setKeywords() const noexcept
{
    QStringList keywords;
    
    keywords
        << "ADD" << "ADD CONSTRAINT" << "ALL" << "ALTER" << "ALTER COLUMN"
        << "ALTER TABLE" << "AND"  << "ANY"  << "AS"  << "ASC"
        << "AUTOINCREMENT"
    
        << "BACKUP DATABASE" << "BEGIN" << "BETWEEN" << "BIGINT" << "BINARY"
        << "BY"

        << "CASCADE" << "CASE" << "CHAR" << "CHECK" << "COLUMN" << "COMMENT"
        << "COMMIT" << "CLOSE" << "CONNECT" << "CONSTRAINT" << "CREATE"
        << "CREATE DATABASE" << "CREATE INDEX" << "CREATE OR REPLACE VIEW"
        << "CREATE TABLE" << "CREATE PROCEDURE" << "CREATE UNIQUE INDEX"
        << "CREATE VIEW" << "CURSOR"
    
        << "DATABASE"  << "DATE" << "DATETIME" << "DAY"  << "DEALLOCATE "
        << "DECLARE" << "DELETE" << "DEFAULT" << "DELETE" << "DESC"
        << "DISTINCT" << "DOUBLE" << "DROP"  << "DROP COLUMN"
        << "DROP CONSTRAINT" << "DROP DATABASE"  << "DROP DEFAULT"
        << "DROP INDEX" << "DROP TABLE" "DROP VIEW"
    
        << "ELSE" << "END" << "EXCEPTION" << "EXEC"  << "EXISTS" << "EXIT"
    
        << "FETCH" << "FOR" << "FOREACH"  << "FOREIGN" << "FOREIGN KEY"
        << "FROM" << "FULL OUTER JOIN" << "FUNCTION"
    
        << "GROUP BY" << "GO"
    
        << "HAVING" << "HANDLER"
    
        << "IF"  << "IN" << "INDEX" << "INNER JOIN" << "INSERT" << "INSERT INTO"
        << "INSERT INTO SELECT" << "IS" << "IS NULL" << "IS NOT NULL" << "INT"
        << "INTO"
      
        << "JOIN"
    
        << "KEY"
    
        << "LEFT JOIN"  << "LIKE"  <<"LIMIT" << "LONG" << "LOOP"
    
        << "MONEY"
    
        << "NOT" << "NOT NULL" << "NULL" << "NUMBER" << "NUMERIC"
        << "NVARCHAR" << "NVARCHAR2"
        
        << "ON" << "OPEN" << "OR" << "ORDER BY" << "OUT" << "OUTER JOIN"
        << "OVER"
    
        << "PRIMARY" << "PRIMARY KEY" << "PROCEDURE"
    
        << "REFERENCES" << "REPLACE" << "RESTRICT" << "RETURN" << "RETURNS"
        << "RETURNING" << "ROWNUM"
    
        << "SELECT" << "SELECT" << "DISTINCT" << "SELECT INTO" << "SELECT TOP"
        << "SET" << "SHORT" << "SIGNED" << "SMALLINT" << "SMALLMONEY"
    
        << "TABLE" << "THEN" << "TIME" << "TIMESTAMP" << "TOP"
        << "TRUNCATE TABLE"
    
        << "UNION" << "UNION ALL" << "UNIQUE" << "UNIQUEIDENTIFIER"
        << "UNSIGNED"<< "UPDATE"
        
        << "VALUES"  << "varchar" << "varchar2" << "VIEW" << "volatile"
       
        << "WHEN" << "WHERE" << "WHILE" << "WORK"
    
        << "XML";

    return keywords;
}

QStringList Highlighter::setFunctions() const noexcept
{
    QStringList functions;
    
    functions
        << "ABS" << "ACOS" << "ADDDATE" << "ADDTIME" << "ASCII" << "ASIN"
        << "ATAN" << "ATAN2" << "ATN2" << "AVG"

        << "BIN"  << "BINARY"
    
        << "CASE" << "CAST" << "CEIL" << "CEILING" << "CHAR"
        << "CHARACTER_LENGTH" << "CHARINDEX" << "CHAR_LENGTH" << "COALESCE"
        << "CONCAT" << "CONCAT_WS" << "CONNECTION_ID" << "CONV" << "CONVERT"
        << "COS" << "COT" << "COUNT" << "CURRENT_DATE" << "CURRENT_TIME"
        << "CURRENT_TIMESTAMP" << "CURRENT_USER"  << "CURDATE" << "CURTIME"
    
        << "DATALENGTH" << "DATE" << "DATEADD" << "DATEDIFF" << "DATEFROMPARTS"
        << "DATENAME" << "DATEPART" << "DATE_ADD" << "DATE_FORMAT" << "DATE_SUB"
        << "DAY" << "DAYNAME" << "DAYOFMONTH" << "DAYOFWEEK" << "DAYOFYEAR"
        << "DECODE" << "DEGREES" << "DIFFERENCE" << "DIV"
    
        << "EXP" << "EXTRACT"
    
        << "FIELD" << "FIND_IN_SET" << "FLOOR" << "FORMAT" << "FROM_DAYS"
    
        << "GETUTCDATE" << "GREATEST"
    
        << "HOUR"
    
        << "IIF" << "IF" << "IFNULL" << "ISDATE" << "ISNULL" << "ISNUMERIC"
        << "INSERT" << "INSTR"
    
        << "LAST_DAY" << "LAST_INSERT_ID" << "LEFT" << "LEN "<< "LENGTH"
        << "LEAST" << "LN" << "LOCALTIME" << "LOCALTIMESTAMP" << "LOCATE"
        << "LOWER" << "LPAD" << "LTRIM"  << "Log" << "LOG2" << "LOG10"
    
        << "MAKEDATE" << "MAKETIME" << "MAX" << "MICROSECOND" << "MID"
        << "MIN" << "MINUTE" << "MOD" << "MONTH" << "MONTHNAME"
    
        << "NCHAR" << "NOW" << "NULLIF"
    
        << "PATINDEX" << "PI" << "POSITION" << "POW" << "POWER"
    
        << "QUARTER" << "QUOTENAME"
    
        << "RADIANS" << "RAND" << "RIGHT" << "ROUND" << "REPEAT" << "REPLACE"
        << "REPLICATE" << "REVERSE" << "RPAD" << "RTRIM"
    
        << "SECOND" << "SEC_TO_TIME" << "SESSIONPROPERTY" << "SESSION_USER"
        << "SIGN" << "SIN" << "SOUNDEX" << "SPACE" << "STR" << "STRCMP"
        << "STUFF" << "STR_TO_DATE" << "SQUARE" << "SQRT" << "SUBDATE" << "SUM"
        << "SUBSTR" << "SUBSTRING" << "SUBSTRING_INDEX" << "SUBTIME"
        << "SYSDATE" << "SYSDATETIME" << "SYSTEM_USER"
    
        << "TAN" << "TIME" << "TIMEDIFF" << "TIMESTAMP" << "TIME_FORMAT"
        << "TIME_TO_SEC" << "TO_DAYS" << "TRANSLATE" << "TRIM" << "TRUNCATE"
    
        << "UCASE" << "UNICODE" << "UPPER" << "USER" << "USER_NAME"
    
        << "VERSION"
    
        << "WEEK" << "WEEKDAY" << "WEEKOFYEAR"
    
        << "YEAR" << "YEARWEEK";
    
    return functions;
}
