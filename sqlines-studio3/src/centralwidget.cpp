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

#include <QHBoxLayout>
#include <QPlainTextEdit>

#include "centralwidget.hpp"

using namespace ui;

CentralWidget::CentralWidget(bool lineNumberEnabled,
                             bool highlighterEnabled,
                             bool wrappingEnabled,
                             QWidget* parent) noexcept
 :  QWidget(parent),
    sourceField(lineNumberEnabled, highlighterEnabled, wrappingEnabled),
    targetField(lineNumberEnabled, highlighterEnabled, wrappingEnabled)
{
    paint();
    
    this->sourceField.installEventFilter(this);
    this->targetField.installEventFilter(this);
    
    connect(&this->sourceField, &SourceFieldWidget::fileOpened,
            this, &ui::CentralWidget::fileOpened);

    connect(&this->sourceField, &SourceFieldWidget::textChanged,
            this, [this] { emit sourceDataChanged(*this->sourceField.document()); });

    connect(&this->targetField, &TargetFieldWidget::textChanged,
            this, [this] { emit targetDataChanged(*this->targetField.document()); });
}

const QTextDocument& CentralWidget::sourceData() const noexcept
{
    return *this->sourceField.document();
}

QTextDocument& CentralWidget::sourceData() noexcept
{
    return *this->sourceField.document();
}

const QTextDocument& CentralWidget::targetData() const noexcept
{
    return *this->targetField.document();
}

QTextDocument& CentralWidget::targetData() noexcept
{
    return *this->targetField.document();
}

void CentralWidget::setSourceData(const QString& data) noexcept
{
    this->sourceField.document()->setPlainText(data);
}

void CentralWidget::setTargetData(const QString& data) noexcept
{
    this->targetField.document()->setPlainText(data);
}

CentralWidget::FieldInFocus CentralWidget::inFocus() const noexcept
{
    if (this->sourceField.hasFocus()) {
        return FieldInFocus::SourceField;
    } else {
        return FieldInFocus::TargetField;
    }
}

InputFieldWidget& CentralWidget::inFocus() noexcept
{
    if (this->sourceField.hasFocus()) {
        return this->sourceField;
    } else {
        return this->targetField;
    }
}

void CentralWidget::focusOn(FieldInFocus focus) noexcept
{
    if (focus == FieldInFocus::SourceField) {
        this->sourceField.setFocus();
    } else if (focus == FieldInFocus::TargetField) {
        this->targetField.setFocus();
    }
}

void CentralWidget::makeActive() noexcept
{
    this->sourceField.setDisabled(false);
    this->targetField.setDisabled(false);
}

void CentralWidget::makeInactive() noexcept
{
    this->sourceField.setDisabled(true);
    this->targetField.setDisabled(true);
}

void CentralWidget::zoomIn() noexcept
{
    this->sourceField.zoomIn();
    this->targetField.zoomIn();
}

void CentralWidget::zoomOut() noexcept
{
    this->sourceField.zoomOut();
    this->targetField.zoomOut();
}

void CentralWidget::changeNumberArea(bool isOn) noexcept
{
    if (isOn) {
        this->sourceField.showLineNumberArea();
        this->targetField.showLineNumberArea();
    } else {
        this->sourceField.hideLineNumberArea();
        this->targetField.hideLineNumberArea();
    }
}

void CentralWidget::changeHighlighting(bool isOn) noexcept
{
    if (isOn) {
        this->sourceField.makeHighlighter();
        this->targetField.makeHighlighter();
    } else {
        this->sourceField.removeHighlighter();
        this->targetField.removeHighlighter();
    }
}

void CentralWidget::changeWrapping(bool isOn) noexcept
{
    if (isOn) {
        this->sourceField.setLineWrapMode(QPlainTextEdit::WidgetWidth);
        this->targetField.setLineWrapMode(QPlainTextEdit::WidgetWidth);
    } else {
        this->sourceField.setLineWrapMode(QPlainTextEdit::NoWrap);
        this->targetField.setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

bool CentralWidget::eventFilter(QObject* object, QEvent* event) noexcept
{
    if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
        if (object == &this->sourceField) {
            emit focusChanged(FieldInFocus::SourceField);
        } else if (object == &this->targetField) {
            emit focusChanged(FieldInFocus::TargetField);
        }
    }
    return false;
}

void CentralWidget::paint() noexcept
{
    auto layout = new QHBoxLayout(this);

    layout->addWidget(&this->sourceField);
    layout->addWidget(&this->targetField);
}
