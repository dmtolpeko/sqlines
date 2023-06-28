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

#ifndef IMAIN_WINDOW_HPP
#define IMAIN_WINDOW_HPP

#include <QString>

namespace view {
    class IMainWindow;
}

class view::IMainWindow {
public:
    enum class FieldInFocus { SourceField, TargetField };

public:
    virtual ~IMainWindow() = default;

    virtual void showConversionStart(int tabIndex) noexcept = 0;
    virtual void showConversionEnd(int tabIndex) noexcept = 0;
    virtual void showConversionError(int tabIndex,
                                     const QString& errorString) noexcept = 0;
    virtual void showFileOpeningError(const QString& errorString) noexcept = 0;

    virtual void showTabTitle(int tabIndex, const QString& data) noexcept = 0;
    virtual void showSourceMode(const QString& mode) noexcept = 0;
    virtual void showTargetMode(const QString& mode) noexcept = 0;
    virtual void showSourceData(int tabIndex, const QString& data) noexcept = 0;
    virtual void showTargetData(int tabIndex, const QString& data) noexcept = 0;
    virtual void showFilePath(const QString& path) noexcept = 0;

    virtual void openNewTab(bool lineNumberEnabled,
                            bool highlighterEnabled,
                            bool wrappingEnabled) noexcept = 0;
    virtual void closeTab(int tabIndex) noexcept = 0;

    virtual QString chooseFileToOpen(const QString& currentDir) noexcept = 0;
    virtual QString choseFileToCreate(const QString& currentDir) noexcept = 0;

    virtual void reset() noexcept = 0;

    virtual int currentTabIndex() const noexcept = 0;
    virtual QString currentTabTitle() const noexcept = 0;
    virtual QString currentSourceMode() const noexcept = 0;
    virtual QString currentTargetMode() const noexcept = 0;
    virtual view::IMainWindow::FieldInFocus inFocus() const noexcept = 0;

public: // Signals
    virtual void tabTitleChanged(int tabIndex, const QString& tabTitle) = 0;
    virtual void sourceModeChanged(int tabIndex, const QString& currentMode) = 0;
    virtual void targetModeChanged(int tabIndex, const QString& currentMode) = 0;
    virtual void sourceDataChanged(int tabindex, const QString& data) = 0;
    virtual void targetDataChanged(int tabindex, const QString& data) = 0;

    virtual void fileOpened(int tabIndex,
                            const QString& data,
                            const QString& filePath) = 0;

    virtual void focusChanged(int tabIndex,
                              view::IMainWindow::FieldInFocus inFocus) = 0;

    virtual void convertPressed(int tabIndex) = 0;

    virtual void openNewTabPressed() = 0;
    virtual void closeTabPressed(int tabIndex) = 0;
    virtual void tabChanged(int currTabIndex) = 0;
    virtual void tabMoved(int to, int from) = 0;

    virtual void openFilePressed(int tabIndex) = 0;
    virtual void saveFilePressed(int tabIndex) = 0;
    virtual void saveFileAsPressed(int tabIndex) = 0;

    virtual void openAboutPressed() = 0;
    virtual void openSettingsPressed() = 0;
    virtual void openConvertUtilityPressed() = 0;
    virtual void openOnlineHelpPressed() = 0;
    virtual void openContactUsPressed() = 0;
    virtual void openSitePressed() = 0;
    virtual void openSourceCodePressed() = 0;
};

#endif // IMAIN_WINDOW_HPP
