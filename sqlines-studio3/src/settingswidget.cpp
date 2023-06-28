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

#include <QSignalBlocker>
#include <QFileDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "settingswidget.hpp"

using namespace ui;

ui::SettingsWidget::SettingsWidget(QWidget* parent) noexcept
 :  QDialog(parent), tabWidget(this)
{
    paint();
}

void SettingsWidget::showDefaults() noexcept
{
    // Prevent emission of the currentChanged signals
    QSignalBlocker block(this);

    // Show default general settings
    this->saveSessionBox.setCheckState(Qt::Checked);

    this->dirChangeBox.clear();
    this->dirChangeBox.addItem("Documents");
    this->dirChangeBox.addItem("Desktop");
    this->dirChangeBox.addItem("Home");
    this->dirChangeBox.insertSeparator(3);
    this->dirChangeBox.addItem("Select directory...");

    // Show default editor settings
    this->numberAreaChangeBox.setCheckState(Qt::Checked);
    this->highlighterChangeBox.setCheckState(Qt::Checked);
    this->wrappingChangeBox.setCheckState(Qt::Checked);
}

void SettingsWidget::showSavingSessionOption(bool isOn) noexcept
{
    // Prevent emission of the stateChanged signals
    QSignalBlocker block(this);

    if (isOn) {
        this->saveSessionBox.setCheckState(Qt::Checked);
    } else {
        this->saveSessionBox.setCheckState(Qt::Unchecked);
    }
}

void SettingsWidget::showWorkingDirs(const QStringList& dirs) noexcept
{
    // Prevent emission of the currentChanged signals
    QSignalBlocker block(this);

    this->dirChangeBox.clear();
    this->dirChangeBox.addItem("Documents");
    this->dirChangeBox.addItem("Desktop");
    this->dirChangeBox.addItem("Home");
    this->dirChangeBox.insertSeparator(3);

    for (const auto& dir : dirs) {
        this->dirChangeBox.addItem(dir);
    }

    if (!dirs.isEmpty()) {
        this->dirChangeBox.insertSeparator(4 + dirs.size());
    }
    this->dirChangeBox.addItem("Select directory...");
}

void SettingsWidget::showCurrentDir(const QString& dir) noexcept
{
    // Prevent emission of the currentChanged signals
    QSignalBlocker block(this);

    this->dirChangeBox.setCurrentText(dir);
}

QString SettingsWidget::choseWorkingDir(const QString& currDir) noexcept
{
    return QFileDialog::getExistingDirectory(this,
                                             "",
                                             currDir,
                                             QFileDialog::ShowDirsOnly |
                                             QFileDialog::DontResolveSymlinks);
}

void SettingsWidget::showNumberAreaOption(bool isOn) noexcept
{
    // Prevent emission of the stateChanged signals
    QSignalBlocker block(this);

    if (isOn) {
        this->numberAreaChangeBox.setCheckState(Qt::Checked);
    } else {
        this->numberAreaChangeBox.setCheckState(Qt::Unchecked);
    }
}

void SettingsWidget::showHighlightingOption(bool isOn) noexcept
{
    // Prevent emission of the stateChanged signals
    QSignalBlocker block(this);

    if (isOn) {
        this->highlighterChangeBox.setCheckState(Qt::Checked);
    } else {
        this->highlighterChangeBox.setCheckState(Qt::Unchecked);
    }
}

void SettingsWidget::showWrappingOption(bool isOn) noexcept
{
    // Prevent emission of the stateChanged signals
    QSignalBlocker block(this);

    if (isOn) {
        this->wrappingChangeBox.setCheckState(Qt::Checked);
    } else {
        this->wrappingChangeBox.setCheckState(Qt::Unchecked);
    }
}

void SettingsWidget::showInvalidLicense() noexcept
{
    QMessageBox::critical(this, "License error", "Invalid license.\n"
                                                 "Please, try again.");
}

void SettingsWidget::showNoLicenseFile() noexcept
{
    QMessageBox::critical(this, "License error", "No license file.\n"
                                                 "Reinstall the application.");
}

void SettingsWidget::showLicenseInfo(const QString& info) noexcept
{
    this->licenseStatusLabel.setText(info);
}

void SettingsWidget::closeEvent(QCloseEvent*)
{
    this->regNameLine.clear();
    this->regNumberLine.clear();
}

void SettingsWidget::resizeEvent(QResizeEvent*)
{
   this->tabWidget.resize(size());
}

void ui::SettingsWidget::paint() noexcept
{
    setWindowTitle("Preferences");
    setMinimumSize(300, 150);
    setMaximumSize(350, 265);

    this->tabWidget.addTab(makeGeneralSettings(), "General");
    this->tabWidget.addTab(makeEditorSettings(), "Editor");
    this->tabWidget.addTab(makeLicenseSettings(), "License");
    this->tabWidget.resize(380, 180);

    auto resizeWindow = [this](int tabIndex) {
        if (tabIndex == 0) { // General settings tab selected
            resize(380, 180);
        } else if (tabIndex == 1) { // Editor settings tab selected
            resize(300, 150);
        } else if (tabIndex == 2) { // License settings tab selected
            resize(350, 265);
        }
    };

    connect(&this->tabWidget, &QTabWidget::currentChanged,
            this, resizeWindow);
}

QWidget* ui::SettingsWidget::makeGeneralSettings() noexcept
{
    auto generalSettings = new QWidget();
    auto layout = new QGridLayout(generalSettings);

    auto dirChangeBoxName = new QLabel("Working directory:");
    dirChangeBoxName->setObjectName("TabWidgetLabel");

    this->dirChangeBox.addItem("Documents");
    this->dirChangeBox.addItem("Desktop");
    this->dirChangeBox.addItem("Home");
    this->dirChangeBox.insertSeparator(3);
    this->dirChangeBox.addItem("Select directory...");
    connect(&this->dirChangeBox, &QComboBox::currentTextChanged,
            this, &SettingsWidget::changeWorkingDirPressed);

    this->saveSessionBox.setText("Save last session");
    this->saveSessionBox.setObjectName("TabWidgetBox");
    this->saveSessionBox.setCheckState(Qt::Checked);
    connect(&this->saveSessionBox, &QCheckBox::clicked,
            this, [this]
            { emit changeSessionSavingPressed(this->saveSessionBox.isChecked()); });

    auto setDefaultsButton = new QPushButton("Set defaults");
    connect(setDefaultsButton, &QPushButton::pressed,
            this, &SettingsWidget::setDefaultsPressed);
    
    layout->addWidget(dirChangeBoxName, 0, 0);
    layout->addWidget(&this->dirChangeBox, 0, 1);
    layout->addWidget(&this->saveSessionBox, 1, 0);
    layout->addWidget(setDefaultsButton, 2, 0);
    layout->setSpacing(6);
    
    return generalSettings;
}

QWidget* ui::SettingsWidget::makeEditorSettings() noexcept
{
    auto editorSettings = new QWidget();
    auto layout = new QVBoxLayout(editorSettings);
    
    this->numberAreaChangeBox.setText("Line numbers");
    this->numberAreaChangeBox.setObjectName("TabWidgetBox");
    connect(&this->numberAreaChangeBox, &QCheckBox::stateChanged,
            this, [this]
            { changeNumberAreaPressed(this->numberAreaChangeBox.isChecked()); });

    this->highlighterChangeBox.setText("Highlighter(Disabling this option\n "
                                       "greatly improves performance)");
    this->highlighterChangeBox.setObjectName("TabWidgetBox");
    connect(&this->wrappingChangeBox, &QCheckBox::stateChanged,
            this, [this]
            { changeWrappingPressed(this->wrappingChangeBox.isChecked()); });

    this->wrappingChangeBox.setText("Wrap lines to editor width");
    this->wrappingChangeBox.setObjectName("TabWidgetBox");
    connect(&this->highlighterChangeBox, &QCheckBox::stateChanged,
            this, [this]
            { changeHighlighterPressed(this->highlighterChangeBox.isChecked()); });
    
    layout->addWidget(&this->numberAreaChangeBox);
    layout->addWidget(&this->highlighterChangeBox);
    layout->addWidget(&this->wrappingChangeBox);
    
    return editorSettings;
}

QWidget* SettingsWidget::makeLicenseSettings() noexcept
{
    auto licenseSettings = new QWidget();

    auto mainLayout = new QVBoxLayout(licenseSettings);
    auto inputFieldsLayout = new QVBoxLayout;

    this->licenseStatusLabel.setObjectName("TabWidgetLabel");

    auto regNameTitle = new QLabel("Registration name: ");
    regNameTitle->setObjectName("TabWidgetLabel");

    auto regNumberTitle = new QLabel("Registration number: ");
    regNumberTitle->setObjectName("TabWidgetLabel");

    auto commitChangeButton = new QPushButton("Commit");
    connect(commitChangeButton, &QPushButton::pressed,
            this, [this]
            { emit commitLicenseChangePressed(this->regNameLine.text(),
                                              this->regNumberLine.text()); });

    inputFieldsLayout->addWidget(regNameTitle);
    inputFieldsLayout->addWidget(&this->regNameLine);
    inputFieldsLayout->addWidget(regNumberTitle);
    inputFieldsLayout->addWidget(&this->regNumberLine);

    mainLayout->addWidget(&this->licenseStatusLabel);
    mainLayout->addLayout(inputFieldsLayout);
    mainLayout->addWidget(commitChangeButton);

    return licenseSettings;
}
