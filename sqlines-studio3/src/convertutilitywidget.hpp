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

#ifndef CONVERT_UTILITY_WIDGET_HPP
#define CONVERT_UTILITY_WIDGET_HPP

#include <QCloseEvent>
#include <QString>
#include <QStringList>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QLabel>

#include "iconvertutilitywidget.hpp"

namespace ui {
    class ConvertUtilityWidget;
}

class ui::ConvertUtilityWidget : public QWidget, public view::IConvertUtilityWidget {
    Q_OBJECT

public:
    ConvertUtilityWidget(const QStringList& sourceModes,
                         const QStringList& targetModes,
                         QWidget* parent = nullptr) noexcept;
    ConvertUtilityWidget(const ui::ConvertUtilityWidget& other) = delete;
    ~ConvertUtilityWidget() override = default;

    void showFilePath(const QString& path) noexcept override;
    void showLog(const QString& log) noexcept override;

    QString currentSourceMode() const noexcept override;
    QString currentTargetMode() const noexcept override;

    QString choseFileToOpen(const QString& currDir) noexcept override;

public: signals:
    void convertPressed() override;
    void selectFilePressed() override;

    void sourceModeSelected(const QString& sourceMode) override;
    void targetModeSelected(const QString& targetMode) override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void paint() noexcept;

private:
    QComboBox sourceComboBox;
    QComboBox targetComboBox;
    QLabel filePathLabel;
    QPlainTextEdit logLabel;
};

#endif // CONVERT_UTILITY_WIDGET_HPP
