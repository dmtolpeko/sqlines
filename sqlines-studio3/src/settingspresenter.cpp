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
#include <QStandardPaths>
#include <QFileInfo>
#include <QStringList>

#include "settingspresenter.hpp"

using namespace presenter;

SettingsPresenter::SettingsPresenter(model::Settings& settings,
                                     model::SettingsLoader& settingsLoader,
                                     model::License& license,
                                     view::ISettingsWidget& settingsWidget,
                                     view::IMainWindowSettings& mainWindow) noexcept
 :  settings(settings),
    settingsLoader(settingsLoader),
    license(license),
    settingsWidget(settingsWidget),
    mainWindow(mainWindow)
{
    if (!this->license.exists() || !this->license.isActive()) {
        this->mainWindow.changeTitle("SQLines Studio – FOR EVALUATION USE ONLY");
        this->settingsWidget.showLicenseInfo("License status: Inactive\n"
                                             "THE PRODUCT IS FOR EVALUATION USE ONLY.");
    } else {
        QString licenseInfo = "License status: Active\n" + this->license.info();
        this->settingsWidget.showLicenseInfo(licenseInfo);
    }

    restoreMainWindowSettings();
    restoreGeneralSettings();
    restoreEditorSettings();

    auto view = dynamic_cast<QObject*>(&this->settingsWidget);

    connect(view, SIGNAL(setDefaultsPressed()),
            this, SLOT(setDefaults()));

    connect(view, SIGNAL(changeSessionSavingPressed(bool)),
            this, SLOT(changeSessionSaving(bool)));

    connect(view, SIGNAL(changeWorkingDirPressed(QString)),
            this, SLOT(changeWorkingDir(QString)));

    connect(view, SIGNAL(changeNumberAreaPressed(bool)),
            this, SLOT(changeNumberArea(bool)));

    connect(view, SIGNAL(changeHighlighterPressed(bool)),
            this, SLOT(changeHighlighter(bool)));

    connect(view, SIGNAL(changeWrappingPressed(bool)),
            this, SLOT(changeWrapping(bool)));

    connect(view, SIGNAL(commitLicenseChangePressed(QString,QString)),
            this, SLOT(changeLicense(QString,QString)));
}

SettingsPresenter::~SettingsPresenter()
{
    saveSettings();
}

void SettingsPresenter::licenseStatusChanged(bool isActive,
                                             const QString& info) noexcept
{
    QString licenseInfo = "License status: ";
    licenseInfo += (isActive) ? "Active" : "Inactive";
    licenseInfo += "\n" + info;

    this->settingsWidget.showLicenseInfo(licenseInfo);

    if (isActive) {
        this->mainWindow.changeTitle("SQLines Studio");
    } else {
        this->mainWindow.changeTitle("SQLines Studio – FOR EVALUATION USE ONLY");
    }
}

void SettingsPresenter::saveSettings() noexcept
{
    // Save the main window geometry
    QString width = QString::number(this->mainWindow.width());
    QString height = QString::number(this->mainWindow.height());
    QString posX =  QString::number(this->mainWindow.posX());
    QString posY = QString::number(this->mainWindow.posY());

    this->settings["ui.main-window.size.width"] = std::move(width);
    this->settings["ui.main-window.size.height"] = std::move(height);
    this->settings["ui.main-window.pos.x"] = std::move(posX);
    this->settings["ui.main-window.pos.y"] = std::move(posY);
}

void SettingsPresenter::restoreMainWindowSettings() noexcept
{
    // Load the main window geometry
    const QString& width = settings["ui.main-window.size.width"];
    const QString& height = settings["ui.main-window.size.height"];
    const QString& posX = settings["ui.main-window.pos.x"];
    const QString& posY = settings["ui.main-window.pos.y"];

    if (!width.isEmpty() && !height.isEmpty()) {
        this->mainWindow.resize(width.toInt(), height.toInt());
    }
    if (!posX.isEmpty() && !posY.isEmpty()) {
        this->mainWindow.move(posX.toInt(), posY.toInt());
    }
}

void SettingsPresenter::restoreGeneralSettings() noexcept
{
    // Load the save session setting
    bool saveLastSession = this->settings["general.save-last-session"].toInt();
    if (saveLastSession) {
        this->settingsWidget.showSavingSessionOption(true);
    } else {
        this->settingsWidget.showSavingSessionOption(false);
    }

    // Load user-added directories
    auto dirsNumber = this->settings["general.dirs.dirs-number"].toInt();
    QStringList currentDirs;
    for (auto i = 0; i < dirsNumber; i++) {
        currentDirs.append(this->settings["general.dirs." + QString::number(i)]);
    }
    if (!currentDirs.isEmpty()) {
        this->settingsWidget.showWorkingDirs(currentDirs);
    }

    // Load current directory
    const QString& currDir = this->settings["general.dirs.curr-dir"];
    if (currDir == QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) {
        this->settingsWidget.showCurrentDir("Documents");
    } else if (currDir == QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)) {
        this->settingsWidget.showCurrentDir("Desktop");
    } else if (currDir == QStandardPaths::writableLocation(QStandardPaths::HomeLocation)) {
        this->settingsWidget.showCurrentDir("Home");
    } else {
        this->settingsWidget.showCurrentDir(currDir);
    }
}

void SettingsPresenter::restoreEditorSettings() noexcept
{
    // Load the number area display setting
    const QString& numberAreaEnabled = this->settings["ui.input-field.number-area"];
    if (numberAreaEnabled.toInt()) {
        this->settingsWidget.showNumberAreaOption(true);
    } else {
        this->settingsWidget.showNumberAreaOption(false);
    }

    // Load the highlighter setting
    const QString& highlighterEnabled = this->settings["ui.input-field.highlighter"];
    if (highlighterEnabled.toInt()) {
        this->settingsWidget.showHighlightingOption(true);
    } else {
        this->settingsWidget.showHighlightingOption(false);
    }

    // Load the wrapping setting
    const QString& wrappingEnabled = this->settings["ui.input-field.wrapping"];
    if (wrappingEnabled.toInt()) {
        this->settingsWidget.showWrappingOption(true);
    } else {
        this->settingsWidget.showWrappingOption(false);
    }
}

void SettingsPresenter::changeSessionSaving(bool isOn) noexcept
{
    this->settings["general.save-last-session"] = QString::number(isOn);
    this->settingsLoader.saveAppSettings();
}

void SettingsPresenter::setDefaults() noexcept
{
    // Set default general settings
    this->settings["general.save-last-session"] = QString::number(true);

    auto customDirsNumber = this->settings["general.dirs.dirs-number"].toInt();
    for (auto i = 0; i < customDirsNumber; i++) {
        this->settings.erase("general.dirs." + QString::number(i));
    }
    this->settings["general.dirs.dirs-number"] = QString::number(0);

    QString currDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    this->settings["general.dirs.curr-dir"] = std::move(currDir);

    // Set default editor settings
    this->settings["ui-input-field.number-area"] = QString::number(true);
    this->settings["ui.input-field.highlighter"] = QString::number(true);
    this->settings["ui.input-field.wrapping"] = QString::number(true);

    this->mainWindow.changeNumberArea(true);
    this->mainWindow.changeHighlighting(true);
    this->mainWindow.changeWrapping(true);

    // Save settings
    this->settingsLoader.clearSettings();
    this->settingsLoader.saveAppSettings();

    // Show default settings
    this->settingsWidget.showDefaults();
}

void SettingsPresenter::changeWorkingDir(const QString& newWorkingDir) noexcept
{
    QString& currDir = this->settings["general.dirs.curr-dir"];

    // Default directory selected
    if (newWorkingDir == "Documents") {
        currDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        this->settingsWidget.showCurrentDir("Documents");
    } else if (newWorkingDir == "Desktop") {
        currDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        this->settingsWidget.showCurrentDir("Desktop");
    } else if (newWorkingDir == "Home") {
        currDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        this->settingsWidget.showCurrentDir("Home");

    // New directory selected
    } else if (newWorkingDir == "Select directory...") {
        QString path = this->settingsWidget.choseWorkingDir(currDir);
        if (path.isEmpty()) {
            this->settingsWidget.showCurrentDir("Documents");
            return;
        }

        currDir = path;
        auto dirsNumber = this->settings["general.dirs.dirs-number"].toInt();
        this->settings["general.dirs." + QString::number(dirsNumber)] = path;
        this->settings["general.dirs.dirs-number"] = QString::number(dirsNumber + 1);

        QStringList currentDirs;
        for (auto i = 0; i < dirsNumber + 1; i++) {
            currentDirs.append(this->settings["general.dirs." + QString::number(i)]);
        }

        this->settingsWidget.showWorkingDirs(currentDirs);
        this->settingsWidget.showCurrentDir(currDir);

    // Existing directory selected
    } else {
        currDir = newWorkingDir;
        this->settingsWidget.showCurrentDir(currDir);
    }

    this->settingsLoader.saveAppSettings();
}

void SettingsPresenter::changeNumberArea(bool isOn) noexcept
{
    this->settings["ui.input-field.number-area"] = QString::number(isOn);
    this->settingsLoader.saveAppSettings();

    this->mainWindow.changeNumberArea(isOn);
}

void SettingsPresenter::changeHighlighter(bool isOn) noexcept
{
    this->settings["ui.input-field.highlighter"] = QString::number(isOn);
    this->settingsLoader.saveAppSettings();

    this->mainWindow.changeHighlighting(isOn);
}

void SettingsPresenter::changeWrapping(bool isOn) noexcept
{
    this->settings["ui.input-field.wrapping"] = QString::number(isOn);
    this->settingsLoader.saveAppSettings();

    this->mainWindow.changeWrapping(isOn);
}

void SettingsPresenter::changeLicense(const QString& regName,
                                      const QString& regNumber) noexcept
{
    if (regName.isEmpty() || regNumber.isEmpty()) {
        return;
    }

    try {
        this->license.changeLicense(regName, regNumber);

        bool licenseIsActive = this->license.isActive();
        QString licenseInfo = "License status: ";
        licenseInfo += (licenseIsActive) ? "Active" : "Inactive";
        licenseInfo += "\n" + this->license.info();

        this->settingsWidget.showLicenseInfo(licenseInfo);

        if (licenseIsActive) {
            this->mainWindow.changeTitle("SQLines Studio");
        } else {
            this->settingsWidget.showInvalidLicense();
            this->mainWindow.changeTitle("SQLines Studio – FOR EVALUATION USE ONLY");
        }

    } catch (model::License::noLicenseFile&) {
        this->settingsWidget.showNoLicenseFile();
    }
}
