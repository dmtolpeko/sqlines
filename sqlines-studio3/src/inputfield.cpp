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
#include <QList>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QPainter>
#include <QColor>
#include <QTextBlock>
#include <QTextFormat>
#include <QMessageBox>

#include "inputfield.hpp"

using namespace ui;
using namespace uiImpl;

LineNumberArea::LineNumberArea(ui::InputFieldWidget& editor) noexcept
:   QWidget(&editor), editor(editor) {}

int LineNumberArea::width() const noexcept
{
    auto digits = 1;
    auto max = qMax(1, this->editor.blockCount());
    while (max >= 10) {
        max /= 10;
        digits++;
    }
    auto space = 0 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

QSize LineNumberArea::sizeHint() const
{
    return { width(), 0 };
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
    this->editor.lineNumberAreaPaintEvent(event);
}

InputFieldWidget::InputFieldWidget(bool lineNumberEnabled,
                                   bool highlighterEnabled,
                                   bool wrappingEnabled,
                                   QWidget* parent) noexcept
 :  QPlainTextEdit(parent),
    lineNumberArea(nullptr),
    highlighter(nullptr)
{
    makeLineNumberArea();

    if (!lineNumberEnabled) {
        this->lineNumberArea->hide();
    }
    if (highlighterEnabled) {
        makeHighlighter();
    }
    if (!wrappingEnabled) {
        setLineWrapMode(NoWrap);
    }
}

// Make the line number area
void InputFieldWidget::lineNumberAreaPaintEvent(QPaintEvent* event) noexcept
{
    QPainter painter(this->lineNumberArea);
    
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QColor color = QColor(Qt::lightGray).lighter(70);
            painter.setPen(std::move(color));
            painter.drawText(0,
                             top,
                             this->lineNumberArea->width(),
                             fontMetrics().height(),
                             Qt::AlignLeft,
                             QString::number(blockNumber + 1));
        }
        
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        blockNumber++;
    }
}

// Line number area setting
void InputFieldWidget::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect rect = contentsRect();
    if (this->lineNumberArea != nullptr) {
        this->lineNumberArea->setGeometry(QRect(rect.left(),
                                                rect.top(),
                                                this->lineNumberArea->width(),
                                                rect.height()));
    }
}

// Line number area setting
void InputFieldWidget::updateLineNumberArea(const QRect& rect, int dy) noexcept
{
    if (dy) {
        this->lineNumberArea->scroll(0, dy);
    } else {
        this->lineNumberArea->update(0, rect.y(),
                                     this->lineNumberArea->width(), rect.height());
    }
    
    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

// Line number area setting
void InputFieldWidget::updateLineNumberAreaWidth(int) noexcept
{
    setViewportMargins(this->lineNumberArea->width(), 0, 0, 0);
}

// Line number area setting
void InputFieldWidget::highlightCurrentLine() noexcept
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly() && !document()->isEmpty()) {
        QColor lineColor = QColor(Qt::lightGray).lighter(122);
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(std::move(lineColor));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        
        extraSelections.append(selection);
    }
     
    setExtraSelections(extraSelections);
}

void InputFieldWidget::makeLineNumberArea() noexcept
{
    this->lineNumberArea = new uiImpl::LineNumberArea(*this);

    connect(this, &InputFieldWidget::blockCountChanged,
            this, &InputFieldWidget::updateLineNumberAreaWidth);
    
    connect(this, &InputFieldWidget::updateRequest,
            this, &InputFieldWidget::updateLineNumberArea);
    
    connect(this, &InputFieldWidget::cursorPositionChanged,
            this, &InputFieldWidget::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
}

void InputFieldWidget::showLineNumberArea() noexcept
{
    if (this->lineNumberArea != nullptr) {
        this->lineNumberArea->show();
    }
}

void InputFieldWidget::hideLineNumberArea() noexcept
{
    if (this->lineNumberArea != nullptr) {
        this->lineNumberArea->hide();
    }
}

void InputFieldWidget::makeHighlighter() noexcept
{
    if (this->highlighter == nullptr) {
        this->highlighter = new uiImpl::Highlighter(*document());
    }
}

void InputFieldWidget::removeHighlighter() noexcept
{
    if (this->highlighter != nullptr) {
        delete this->highlighter;
        this->highlighter = nullptr;
    }
}

SourceFieldWidget::SourceFieldWidget(bool lineNumberEnabled,
                                     bool highlighterEnabled,
                                     bool wrappingEnabled,
                                     QWidget* parent) noexcept
 :  InputFieldWidget(lineNumberEnabled,
                     highlighterEnabled,
                     wrappingEnabled,
                     parent) {}

// Drag & drop setting
bool SourceFieldWidget::canInsertFromMimeData(const QMimeData* source) const
{
    if (source->hasUrls()) {
        QUrl url = source->urls().first();
        QFileInfo file(url.toLocalFile());
        return !file.isDir() && !file.isExecutable() && !file.isBundle();
    
    } else {
        return source->hasText();
    }
}

// Drag & drop setting
void SourceFieldWidget::insertFromMimeData(const QMimeData* source)
{
    // Copy data from dragged file
    if (source->hasUrls()) {
        QUrl url = source->urls().first();
        QFile file(url.toLocalFile());

        if (file.size() > 2'000'000) /* 2 MB */ {
            QMessageBox::critical(this,
                                  "Filesystem error",
                                  "Unable to open source file.\n"
                                  "Path: " + file.fileName() +
                                  "\n\nThe file is too large."
                                  "\nUse the Conversion utility(Tools->Conversion utility) "
                                  "for large files.");
            return;
        }
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this,
                                  "Filesystem error",
                                  "Unable to open source file.\n\n"
                                  "Path: " + file.fileName() + "\n" +
                                  file.errorString() + ".");
            return;
        }
        
        QTextStream stream(&file);
        clear();
        setPlainText(stream.readAll());
        emit fileOpened(file.fileName());
        
    // Copy dragged text
    } else if (source->hasText()) {
        if (source->text().size() > 2'000'000) /* 2 MB */ {
            QMessageBox::critical(this,
                                  "Filesystem error",
                                  "The text cannot be copied.\n\n"
                                  "The text is too large."
                                  "\nUse the Conversion utility(Tools->Conversion utility) "
                                  "for large files.");
            return;
        }

        insertPlainText(source->text());
    }
}

ui::TargetFieldWidget::TargetFieldWidget(bool lineNumberEnabled,
                                         bool highlighterEnabled,
                                         bool wrappingEnabled,
                                         QWidget* parent) noexcept
 :  ui::InputFieldWidget(lineNumberEnabled,
                         highlighterEnabled,
                         wrappingEnabled,
                         parent)
{
    setAcceptDrops(false);
    hide();
    
    auto makeActive = [this] {
        if (!this->document()->isEmpty()) {
            show();
        }
    };
    
    connect(this, &InputFieldWidget::textChanged,
            this, makeActive);
}
