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
#include <QFileInfo>
#include <QDir>

#include "settingsloader.hpp"

using namespace model;

SettingsLoader::SettingsLoader(const QString& orgName,
                               const QString& appName,
                               model::Settings& settings,
                               model::Snapshot& snapshot) noexcept
 : settingsFile(orgName, appName),
   settings(settings),
   state(snapshot)
{
    QString settingsFilePath = QDir(this->settingsFile.fileName()).absolutePath();
    if (!QFileInfo::exists(settingsFilePath)) {
        return;
    }
    
    loadGeneralSettings();
    loadUiSettings();
    loadUiState();
}

SettingsLoader::~SettingsLoader() noexcept
{
    this->settingsFile.clear();
    
    saveGeneralSettings();
    saveUiSettings();
    saveUiState();
}

void SettingsLoader::saveAppSettings() noexcept
{
    saveGeneralSettings();
    saveUiSettings();
}

void SettingsLoader::saveAppState() noexcept
{
    saveUiState();
}

void SettingsLoader::clearSettings() noexcept
{
    this->settingsFile.clear();
}

void SettingsLoader::loadGeneralSettings() noexcept
{
    this->settingsFile.beginGroup("general-settings");
    
    // Load the save session setting
    if (this->settingsFile.contains("save-last-session")) {
        QString saveSession = this->settingsFile.value("save-last-session").toString();
        this->settings["general.save-last-session"] = std::move(saveSession);
    }
    
    loadDirectories("general.dirs.");
    
    this->settingsFile.endGroup();
}

void SettingsLoader::saveGeneralSettings() noexcept
{
    this->settingsFile.beginGroup("general-settings");
    
    // Save the save session setting
    QString saveSession = this->settings["general.save-last-session"];
    this->settingsFile.setValue("save-last-session", saveSession);
    
    saveDirectories("general.dirs.");
    
    this->settingsFile.endGroup();
}

void SettingsLoader::loadUiSettings() noexcept
{
    this->settingsFile.beginGroup("ui-settings");
    
    loadMainWindowSettings("ui.main-window.");
    loadInputFieldSettings("ui.input-field.");
    
    this->settingsFile.endGroup();
}

void SettingsLoader::saveUiSettings() noexcept
{
    this->settingsFile.beginGroup("ui-settings");
    
    saveMainWindowSettings("ui.main-window.");
    saveInputFieldSettings("ui.input-field.");
    
    this->settingsFile.endGroup();
}

void SettingsLoader::loadUiState() noexcept
{
    this->settingsFile.beginGroup("ui-state");
    loadTabsState("ui.tabs.");
    this->settingsFile.endGroup();
}

void SettingsLoader::saveUiState() noexcept
{
    this->settingsFile.beginGroup("ui-state");
    saveTabsState("ui.tabs.");
    this->settingsFile.endGroup();
}

void SettingsLoader::loadDirectories(const QString& prefix) noexcept
{
    this->settingsFile.beginGroup("dirs");
    
    // Load the current working directory
    if (this->settingsFile.contains("curr-dir")) {
        QString currDir = this->settingsFile.value("curr-dir").toString();
        this->settings[prefix + "curr-dir"] = std::move(currDir);
    }
    
    // Save the number of user-added directories
    QString dirsNumber = this->settingsFile.value("dirs-number").toString();
    this->settings[prefix + "dirs-number"] = std::move(dirsNumber);
    
    // Load all user-added working directories
    auto dirsCount = this->settingsFile.beginReadArray("dirs");
    for (auto i = 0; i < dirsCount; i++) {
        this->settingsFile.setArrayIndex(i);
        
        QString dir = this->settingsFile.value("dir").toString();
        this->settings[prefix + QString::number(i)] = std::move(dir);
    }
    this->settingsFile.endArray();
    
    this->settingsFile.endGroup();
}

void SettingsLoader::saveDirectories(const QString& prefix) noexcept
{
    this->settingsFile.beginGroup("dirs");
    
    // Save the current working directory
    const QString& currDir = this->settings[prefix + "curr-dir"];
    this->settingsFile.setValue("curr-dir", currDir);
    
    // Save the number of user-added directories
    const QString& dirsNumber = this->settings[prefix + "dirs-number"];
    this->settingsFile.setValue("dirs-number", dirsNumber);
    
    // Save all user-added directories
    this->settingsFile.beginWriteArray("dirs");
    for (auto i = 0; i < dirsNumber.toInt(); i++) {
        this->settingsFile.setArrayIndex(i);
        
        const QString& dir = settings[prefix + QString::number(i)];
        this->settingsFile.setValue("dir", dir);
    }
    this->settingsFile.endArray();
    
    this->settingsFile.endGroup();
}

void SettingsLoader::loadMainWindowSettings(const QString& prefix) noexcept
{
    this->settingsFile.beginGroup("main-window");
    this->settingsFile.beginGroup("settings");
    
    // Load the main window geometry
    if (this->settingsFile.contains("size.height")) {
        QString height = this->settingsFile.value("size.height").toString();
        this->settings[prefix + "size.height"] = std::move(height);
    }
    if (this->settingsFile.contains("size.width")) {
        QString width = this->settingsFile.value("size.width").toString();
        this->settings[prefix + "size.width"] = std::move(width);
    }
    if (this->settingsFile.contains("pos.x")) {
        QString posX = this->settingsFile.value("pos.x").toString();
        this->settings[prefix + "pos.x"] = std::move(posX);
    }
    if (this->settingsFile.contains("pos.y")) {
        QString posY = this->settingsFile.value("pos.y").toString();
        this->settings[prefix + "pos.y"] = std::move(posY);
    }
   
    this->settingsFile.endGroup();
    this->settingsFile.endGroup();
}

void SettingsLoader::saveMainWindowSettings(const QString& prefix) noexcept
{
    this->settingsFile.beginGroup("main-window");
    this->settingsFile.beginGroup("settings");
    
    // Save the main window geometry
    const QString& height = this->settings[prefix + "size.height"];
    this->settingsFile.setValue("size.height", height);
    
    const QString& width = this->settings[prefix + "size.width"];
    this->settingsFile.setValue("size.width", width);
    
    const QString& posX = this->settings[prefix + "pos.x"];
    this->settingsFile.setValue("pos.x", posX);
    
    const QString& posY = this->settings[prefix + "pos.y"];
    this->settingsFile.setValue("pos.y", posY);
    
    this->settingsFile.endGroup();
    this->settingsFile.endGroup();
}

void SettingsLoader::loadInputFieldSettings(const QString& prefix) noexcept
{
    this->settingsFile.beginGroup("input-field");
    this->settingsFile.beginGroup("settings");
    
    if (this->settingsFile.contains("number-area")) {
        QString lineNumber = this->settingsFile.value("number-area").toString();
        this->settings[prefix + "number-area"] = std::move(lineNumber);
    }
    if (this->settingsFile.contains("highlighter")) {
        QString highlighter = this->settingsFile.value("highlighter").toString();
        this->settings[prefix + "highlighter"] = std::move(highlighter);
    }
    if (this->settingsFile.contains("wrapping")) {
        QString wrapping = this->settingsFile.value("wrapping").toString();
        this->settings[prefix + "wrapping"] = std::move(wrapping);
    }
    
    this->settingsFile.endGroup();
    this->settingsFile.endGroup();
}

void SettingsLoader::saveInputFieldSettings(const QString& prefix) noexcept
{
    this->settingsFile.beginGroup("input-field");
    this->settingsFile.beginGroup("settings");
    
    const QString& lineNumber = this->settings[prefix + "number-area"];
    this->settingsFile.setValue("number-area", lineNumber);
    
    const QString& highlighter = this->settings[prefix + "highlighter"];
    this->settingsFile.setValue("highlighter", highlighter);
    
    const QString& wrapping = this->settings[prefix + "wrapping"];
    this->settingsFile.setValue("wrapping", wrapping);
    
    this->settingsFile.endGroup();
    this->settingsFile.endGroup();
}

void SettingsLoader::loadTabsState(const QString& prefix) noexcept
{
    this->settingsFile.beginGroup("tabs");

    // Load the number of tabs
    if (this->settingsFile.contains("tabs.number")) {
        QString tabsNumber = this->settingsFile.value("tabs.number").toString();
        this->state[prefix + "tabs-number"] = std::move(tabsNumber);
    }
    
    // Load tab data
    auto tabsNumber = this->settingsFile.beginReadArray("tabs-data");
    for (auto i = 0; i < tabsNumber; i++) {
        this->settingsFile.setArrayIndex(i);
        QString index = QString::number(i);

        if (this->settingsFile.contains("tab-name")) {
            QString tabName = this->settingsFile.value("tab-name").toString();
            this->state[prefix + index + ".tab-name"] = std::move(tabName);
        }

        if (this->settingsFile.contains("source-mode")) {
            QString sourceMode = this->settingsFile.value("source-mode").toString();
            this->state[prefix + index + ".source-mode"] = std::move(sourceMode);
        }

        if (this->settingsFile.contains("target-mode")) {
            QString targetMode = this->settingsFile.value("target-mode").toString();
            this->state[prefix + index + + ".target-mode"] = std::move(targetMode);
        }

        if (this->settingsFile.contains("source-filePath")) {
            QString sourcePath = this->settingsFile.value("source-filePath").toString();
            this->state[prefix + index + ".source-filePath"] = std::move(sourcePath);
        }

        if (this->settingsFile.contains("target-filePath")) {
            QString targetPath = this->settingsFile.value("target-filePath").toString();
            this->state[prefix + index + ".target-filePath"] = std::move(targetPath);
        }
    }
    this->settingsFile.endArray();

    this->settingsFile.endGroup();
}

void SettingsLoader::saveTabsState(const QString& prefix) noexcept
{
    this->settingsFile.beginGroup("tabs");
    
    // Save the number of tabs
    const QString& tabsNumber = this->state[prefix + "tabs-number"];
    this->settingsFile.setValue("tabs.number", tabsNumber);
    
    // Save tab data
    this->settingsFile.beginWriteArray("tabs-data");
    for (auto i = 0; i < tabsNumber.toInt(); i++) {
        this->settingsFile.setArrayIndex(i);
        QString index = QString::number(i);
        
        const QString& tabName = this->state[prefix + index + ".tab-name"];
        this->settingsFile.setValue("tab-name", tabName);
        
        const QString& sourceMode = this->state[prefix + index + ".source-mode"];
        this->settingsFile.setValue("source-mode", sourceMode);
        
        const QString& targetMode = this->state[prefix + index + ".target-mode"];
        this->settingsFile.setValue("target-mode", targetMode);
        
        const QString& sourcePath = this->state[prefix + index + ".source-filePath"];
        this->settingsFile.setValue("source-filePath", sourcePath);

        const QString& targetPath = this->state[prefix + index + ".target-filePath"];
        this->settingsFile.setValue("target-filePath", targetPath);
    }
    this->settingsFile.endArray();
    
    this->settingsFile.endGroup();
}
