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

#ifndef INPUT_FIELD_WIDGET
#define INPUT_FIELD_WIDGET

#include <QResizeEvent>
#include <QPaintEvent>
#include <QMimeData>
#include <QString>
#include <QSize>
#include <QRect>
#include <QPlainTextEdit>

#include "highlighter.hpp"

namespace uiImpl {
    class LineNumberArea;
}

namespace ui {
    class InputFieldWidget;
    class SourceFieldWidget;
    class TargetFieldWidget;
}

class uiImpl::LineNumberArea : public QWidget {
public:
    explicit LineNumberArea(ui::InputFieldWidget& editor) noexcept;
    LineNumberArea(const uiImpl::LineNumberArea& other) = delete;
    ~LineNumberArea() override = default;

    int width() const noexcept;
    
protected:
    QSize sizeHint() const override;
    void paintEvent(QPaintEvent* event) override;
    
private:
    ui::InputFieldWidget& editor;
};


class ui::InputFieldWidget : public QPlainTextEdit {
public:
    explicit InputFieldWidget(bool lineNumberEnabled,
                              bool highlighterEnabled,
                              bool wrappingEnabled,
                              QWidget* parent = nullptr) noexcept;
    InputFieldWidget(const ui::InputFieldWidget& other) = delete;
    ~InputFieldWidget() noexcept override = default;
    
    // Make the line number area
    void lineNumberAreaPaintEvent(QPaintEvent* event) noexcept;
    void makeLineNumberArea() noexcept;

    void showLineNumberArea() noexcept;
    void hideLineNumberArea() noexcept;

    void makeHighlighter() noexcept;
    void removeHighlighter() noexcept;
    
protected:
    // Line number area setting
    void resizeEvent(QResizeEvent* event) override;
    
private:
    // Line number area setting
    void updateLineNumberArea(const QRect& rect, int dy) noexcept;
    void updateLineNumberAreaWidth(int newBlockCount) noexcept;
    void highlightCurrentLine() noexcept;

protected:
    uiImpl::LineNumberArea* lineNumberArea;
    uiImpl::Highlighter* highlighter;
};


class ui::SourceFieldWidget : public ui::InputFieldWidget {
    Q_OBJECT
    
public:
    explicit SourceFieldWidget(bool lineNumberEnabled,
                               bool highlighterEnabled,
                               bool wrappingEnabled,
                               QWidget* parent = nullptr) noexcept;
    SourceFieldWidget(const ui::SourceFieldWidget& other) = delete;
    ~SourceFieldWidget() override = default;
    
public: signals:
    void fileOpened(const QString& filePath);
    
protected:
    // Drag & drop setting
    bool canInsertFromMimeData(const QMimeData* source) const override;
    void insertFromMimeData(const QMimeData* source) override;
};


class ui::TargetFieldWidget : public ui::InputFieldWidget {
public:
    explicit TargetFieldWidget(bool lineNumberEnabled,
                               bool highlighterEnabled,
                               bool wrappingEnabled,
                               QWidget* parent = nullptr) noexcept;
    TargetFieldWidget(const ui::TargetFieldWidget& other) = delete;
    ~TargetFieldWidget() override = default;
};

#endif // INPUT_FIELD_WIDGET
