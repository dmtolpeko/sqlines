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

#ifndef SETTINGS_LOADER_HPP
#define SETTINGS_LOADER_HPP

#include <map>
#include <QSettings>
#include <QString>

#include "settings.hpp"
#include "snapshot.hpp"

namespace model {
    class SettingsLoader;
}

class model::SettingsLoader {
public:
    SettingsLoader(const QString& orgName,
                   const QString& appName,
                   model::Settings& settings,
                   model::Snapshot& snapshot) noexcept;
    ~SettingsLoader() noexcept;

    void saveAppSettings() noexcept;
    void saveAppState() noexcept;

    void clearSettings() noexcept;

private:
    void loadGeneralSettings() noexcept;
    void saveGeneralSettings() noexcept;
    
    void loadUiSettings() noexcept;
    void saveUiSettings() noexcept;
    
    void loadUiState() noexcept;
    void saveUiState() noexcept;
   
private:
    void loadDirectories(const QString& prefix) noexcept;
    void saveDirectories(const QString& prefix) noexcept;
    
    void loadMainWindowSettings(const QString& prefix) noexcept;
    void saveMainWindowSettings(const QString& prefix) noexcept;
    
    void loadInputFieldSettings(const QString& prefix) noexcept;
    void saveInputFieldSettings(const QString& prefix) noexcept;
    
    void loadTabsState(const QString& prefix) noexcept;
    void saveTabsState(const QString& prefix) noexcept;
    
private:
    QSettings settingsFile;
    
    model::Settings& settings;
    model::Snapshot& state;
};

#endif // SETTINGS_LOADER_HPP
