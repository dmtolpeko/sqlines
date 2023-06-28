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

#include <QFileDialog>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

#include "convertutilitywidget.hpp"

using namespace ui;

ConvertUtilityWidget::ConvertUtilityWidget(const QStringList& sourceModes,
                                           const QStringList& targetModes,
                                           QWidget* parent) noexcept
 :  QWidget(parent)
{
    setObjectName("ConvertUtility");

    this->sourceComboBox.addItems(sourceModes);
    this->targetComboBox.addItems(targetModes);

    this->filePathLabel.setWordWrap(true);
    this->filePathLabel.setDisabled(true);

    this->logLabel.setReadOnly(true);
    this->logLabel.setDisabled(true);
    this->logLabel.setObjectName("LogLabel");

    auto makeActive = [this] {
        if (this->logLabel.toPlainText() == "Log") {
            this->logLabel.setDisabled(true);
        } else {
            this->logLabel.setDisabled(false);
        }

        if (this->filePathLabel.text() == "Source:") {
            this->filePathLabel.setDisabled(true);
        } else {
            this->filePathLabel.setDisabled(false);
        }
    };

    connect(&this->logLabel, &QPlainTextEdit::textChanged,
            this, makeActive);

    paint();
}

void ConvertUtilityWidget::showFilePath(const QString& path) noexcept
{
    this->filePathLabel.setText("Source:" + path);
}

void ConvertUtilityWidget::showLog(const QString& log) noexcept
{
    this->logLabel.setPlainText(log);
}

QString ConvertUtilityWidget::currentSourceMode() const noexcept
{
    return this->sourceComboBox.currentText();;
}

QString ConvertUtilityWidget::currentTargetMode() const noexcept
{
    return this->targetComboBox.currentText();;
}

QString ConvertUtilityWidget::choseFileToOpen(const QString& currDir) noexcept
{
    return QFileDialog::getOpenFileName(this, "", currDir);
}

void ConvertUtilityWidget::closeEvent(QCloseEvent*)
{
    this->filePathLabel.setText("Source:");
    this->logLabel.setPlainText("Log");
}

void ConvertUtilityWidget::paint() noexcept
{
    setWindowTitle("Conversion utility");
    setFixedSize(350, 500);

    auto layout = new QGridLayout(this);

    auto sourceComboBoxName = new QLabel("Source: ");
    connect(&this->sourceComboBox, &QComboBox::currentTextChanged,
            this, &ConvertUtilityWidget::sourceModeSelected);

    auto targetComboBoxName = new QLabel("Target: ");
    connect(&this->targetComboBox, &QComboBox::currentTextChanged,
            this, &ConvertUtilityWidget::targetModeSelected);

    this->filePathLabel.setText("Source:");
    this->logLabel.setPlainText("Log");

    auto selectFileButton = new QPushButton("Select source file");
    connect(selectFileButton, &QPushButton::clicked,
            this, &ConvertUtilityWidget::selectFilePressed);

    auto convertButton = new QPushButton("Convert");
    connect(convertButton, &QPushButton::clicked,
            this, &ConvertUtilityWidget::convertPressed);

    layout->addWidget(sourceComboBoxName, 0, 0);
    layout->addWidget(&this->sourceComboBox, 0, 1);
    layout->addWidget(targetComboBoxName, 1, 0);
    layout->addWidget(&this->targetComboBox, 1, 1);
    layout->addWidget(&this->logLabel, 2, 0, 1, 0);
    layout->addWidget(&this->filePathLabel, 3, 0, 1, 0);
    layout->addWidget(selectFileButton, 4, 0, 1, 0);
    layout->addWidget(convertButton, 5, 0, 1, 0);
}
