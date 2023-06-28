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

#ifndef CENTRAL_WIDGET_HPP
#define CENTRAL_WIDGET_HPP

#include <QEvent>
#include <QString>
#include <QTextDocument>
#include <QWidget>

#include "inputfield.hpp"

namespace ui {
    class CentralWidget;
}

class ui::CentralWidget : public QWidget {
    Q_OBJECT

public:
    enum class FieldInFocus { SourceField, TargetField };

public:
    explicit CentralWidget(bool lineNumberEnabled,
                           bool highlighterEnabled,
                           bool wrappingEnabled,
                           QWidget* parent = nullptr) noexcept;
    CentralWidget(const ui::CentralWidget& other) = delete;
    ~CentralWidget() override = default;

    const QTextDocument& sourceData() const noexcept;
    QTextDocument& sourceData() noexcept;
    const QTextDocument& targetData() const noexcept;
    QTextDocument& targetData() noexcept;

    void setSourceData(const QString& data) noexcept;
    void setTargetData(const QString& data) noexcept;

    FieldInFocus inFocus() const noexcept;
    ui::InputFieldWidget& inFocus() noexcept;
    void focusOn(FieldInFocus focus) noexcept;

    void makeActive() noexcept;
    void makeInactive() noexcept;

    void zoomIn() noexcept;
    void zoomOut() noexcept;

    void changeNumberArea(bool isOn) noexcept;
    void changeHighlighting(bool isOn) noexcept;
    void changeWrapping(bool isOn) noexcept;
    
public: signals:
    void sourceDataChanged(const QTextDocument& data);
    void targetDataChanged(const QTextDocument& data);
    void fileOpened(const QString& filePath);
    void focusChanged(ui::CentralWidget::FieldInFocus inFocus);
    
protected:
    bool eventFilter(QObject* object, QEvent* event) noexcept override;
    
private:
    void paint() noexcept;
    
private:
    ui::SourceFieldWidget sourceField;
    ui::TargetFieldWidget targetField;
};

#endif // CENTRAL_WIDGET_HPP
