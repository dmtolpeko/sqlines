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

#ifndef ISETTINGS_WIDGET_HPP
#define ISETTINGS_WIDGET_HPP

#include <QString>
#include <QStringList>

namespace view {
    class ISettingsWidget;
}

class view::ISettingsWidget {
public:
    virtual ~ISettingsWidget() = default;

    // General settings
    virtual void showDefaults() noexcept = 0;
    virtual void showSavingSessionOption(bool isOn) noexcept = 0;
    virtual void showWorkingDirs(const QStringList& dirs) noexcept = 0;
    virtual void showCurrentDir(const QString& dir) noexcept = 0;

    virtual QString choseWorkingDir(const QString& currDir) noexcept = 0;

    // Editor settings
    virtual void showNumberAreaOption(bool isOn) noexcept = 0;
    virtual void showHighlightingOption(bool isOn) noexcept = 0;
    virtual void showWrappingOption(bool isOn) noexcept = 0;

    // License settings
    virtual void showInvalidLicense() noexcept = 0;
    virtual void showNoLicenseFile() noexcept = 0;

    virtual void showLicenseInfo(const QString& licenseInfo) noexcept = 0;

public: // Signals
    // General settings
    virtual void setDefaultsPressed() = 0;
    virtual void changeSessionSavingPressed(bool isOn) = 0;
    virtual void changeWorkingDirPressed(const QString& workingDir) = 0;

    // Editor settings
    virtual void changeNumberAreaPressed(bool isOn) = 0;
    virtual void changeHighlighterPressed(bool isOn) = 0;
    virtual void changeWrappingPressed(bool isOn) = 0;

    // License settings
    virtual void commitLicenseChangePressed(const QString& regName,
                                            const QString& regNumber) = 0;
};

#endif // ISETTINGS_WIDGET_HPP
