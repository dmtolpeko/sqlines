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

#ifndef SETTINGS_WIDGET_HPP
#define SETTINGS_WIDGET_HPP

#include <QCloseEvent>
#include <QResizeEvent>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QDialog>

#include "isettingswidget.hpp"

namespace ui {
    class SettingsWidget;
}

class ui::SettingsWidget : public QDialog, public view::ISettingsWidget {
    Q_OBJECT
    
public:
    explicit SettingsWidget(QWidget* parent = nullptr) noexcept;
    SettingsWidget(const ui::SettingsWidget& other) = delete;
    ~SettingsWidget() override = default;

    // General settings
    void showDefaults() noexcept override;
    void showSavingSessionOption(bool isOn) noexcept override;
    void showWorkingDirs(const QStringList& dirs) noexcept override;
    void showCurrentDir(const QString &dir) noexcept override;

    QString choseWorkingDir(const QString& currDir) noexcept override;

    // Editor settings
    void showNumberAreaOption(bool isOn) noexcept override;
    void showHighlightingOption(bool isOn) noexcept override;
    void showWrappingOption(bool isOn) noexcept override;

    // License settings
    void showInvalidLicense() noexcept override;
    void showNoLicenseFile() noexcept override;

    void showLicenseInfo(const QString &info) noexcept override;

public: signals:
    // General settings
    void setDefaultsPressed() override;
    void changeSessionSavingPressed(bool isOn) override;
    void changeWorkingDirPressed(const QString& workingDir) override;

    // Editor settings
    void changeNumberAreaPressed(bool isOn) override;
    void changeHighlighterPressed(bool isOn) override;
    void changeWrappingPressed(bool isOn) override;

    // License settings
    void commitLicenseChangePressed(const QString& regName,
                                    const QString& regNumber) override;

protected:
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    void paint() noexcept;
    QWidget* makeGeneralSettings() noexcept;
    QWidget* makeEditorSettings() noexcept;
    QWidget* makeLicenseSettings() noexcept;
    
private:
    QTabWidget tabWidget;

    // General settings
    QComboBox dirChangeBox;
    QCheckBox saveSessionBox;
    
    // Editor settings
    QCheckBox numberAreaChangeBox;
    QCheckBox highlighterChangeBox;
    QCheckBox wrappingChangeBox;

    // License settings
    QLabel licenseStatusLabel;
    QLineEdit regNameLine;
    QLineEdit regNumberLine;
};

#endif // SETTINGS_WIDGET_HPP
