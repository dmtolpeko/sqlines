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
#include "QApplication"
#include <QStandardPaths>
#include <QDir>

#include "settings.hpp"

using namespace model;

Settings::Settings() noexcept
{
    setDefaultSettings();
}

QString Settings::operator[](const QString& key) const noexcept
{
    const auto& item = this->settings.find(key);
    if (item != this->settings.end()) {
        return (*item).second;
    } else {
        return {};
    }
}

QString& Settings::operator[](const QString& key) noexcept
{
    return this->settings[key];
}

void Settings::erase(const QString& key) noexcept
{
    this->settings.erase(key);
}

void Settings::setDefaultSettings() noexcept
{
    this->settings["general.save-last-session"] = QString::number(true);

    QString workingDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    this->settings["general.dirs.curr-dir"] = std::move(workingDir);

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    this->settings["general.dirs.cache-dir"] = std::move(cacheDir);

    QString processPath = QApplication::applicationDirPath();
#if defined(Q_OS_WIN)
    processPath += "\sqlines.exe";
#elif defined(Q_OS_UNIX)
    processPath += "/sqlines";
#endif
    this->settings["general.dirs.process-filePath"] = std::move(processPath);

    QString licensePath = QApplication::applicationDirPath() + "/license.txt";
    this->settings["general.dirs.license-filePath"] = QDir::toNativeSeparators(licensePath);

    this->settings["ui.input-field.number-area"] = QString::number(true);
    this->settings["ui.input-field.highlighter"] = QString::number(true);
    this->settings["ui.input-field.wrapping"] = QString::number(true);
}
