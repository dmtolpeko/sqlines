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

#ifndef SETTINGS_PRESENTER_HPP
#define SETTINGS_PRESENTER_HPP

#include <QObject>
#include <QString>

#include "settings.hpp"
#include "settingsloader.hpp"
#include "license.hpp"
#include "ilicenseobserver.hpp"
#include "isettingswidget.hpp"
#include "imainwindowsettings.hpp"

namespace presenter {
    class SettingsPresenter;
}

class presenter::SettingsPresenter : public QObject, public model::ILicenseObserver {
    Q_OBJECT

public:
    SettingsPresenter(model::Settings& settings,
                      model::SettingsLoader& settingsLoader,
                      model::License& license,
                      view::ISettingsWidget& settingsWidget,
                      view::IMainWindowSettings& mainWindow) noexcept;
    ~SettingsPresenter() override;

    // Observer methods
    void licenseStatusChanged(bool isActive,
                              const QString& info) noexcept override;

private:
    void saveSettings() noexcept;
    void restoreMainWindowSettings() noexcept;
    void restoreGeneralSettings() noexcept;
    void restoreEditorSettings() noexcept;

private slots: // UI actions
    // General settings
    void setDefaults() noexcept;
    void changeSessionSaving(bool isOn) noexcept;
    void changeWorkingDir(const QString& newWorkingDir) noexcept;

    // Editor settings
    void changeNumberArea(bool isOn) noexcept;
    void changeHighlighter(bool isOn) noexcept;
    void changeWrapping(bool isOn) noexcept;

    // License settings
    void changeLicense(const QString& regName, const QString& regNumber) noexcept;

private:
    model::Settings& settings;
    model::SettingsLoader& settingsLoader;
    model::License& license;

    view::ISettingsWidget& settingsWidget;
    view::IMainWindowSettings& mainWindow;
};

#endif // SETTINGS_PRESENTER_HPP
