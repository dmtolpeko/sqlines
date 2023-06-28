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

#ifndef HIGHLIGHTER_HPP
#define HIGHLIGHTER_HPP

#include <vector>
#include <QString>
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QStringList>

namespace uiImpl {
    class Highlighter;
}

class uiImpl::Highlighter : public QSyntaxHighlighter {
    using HighlightingRules = std::vector<std::pair<QRegularExpression, QTextCharFormat>>;
    
public:
    explicit Highlighter(QTextDocument& editor) noexcept;
    Highlighter(const uiImpl::Highlighter& other) = delete;
    ~Highlighter() override = default;

protected:
    void highlightBlock(const QString& text) override;
    
private:
    // Set highlighting patterns
    void setKeywordHighlighting() noexcept;
    void setDigitHighlighting() noexcept;
    void setQuotationHighlighting() noexcept;
    void setCommentHighlighting() noexcept;

    // Set highlighting keywords
    QStringList setKeywords() const noexcept;
    QStringList setFunctions() const noexcept;
    
private:
    HighlightingRules highlightingRules;
    QStringList keywords;
    QStringList functions;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QTextCharFormat multiLineCommentFormat;
};

#endif // HIGHLIGHTER_HPP
