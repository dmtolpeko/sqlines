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

#ifndef MAIN_WINDOW_PRESENTER_HPP
#define MAIN_WINDOW_PRESENTER_HPP

#include <exception>
#include <QString>
#include <QObject>

#include "converter.hpp"
#include "settings.hpp"
#include "iconverterobserver.hpp"
#include "imainwindow.hpp"

namespace presenter {
    class MainWindowPresenter;
}

class presenter::MainWindowPresenter : public QObject, public model::IConverterObserver {
    Q_OBJECT

public:
    MainWindowPresenter(const model::Settings& settings,
                        model::Converter& converter,
                        view::IMainWindow& mainWindow) noexcept;
    ~MainWindowPresenter() override = default;

    // Observer methods
    void conversionStarted(int tabIndex) noexcept override;
    void conversionEnded(int tabIndex) noexcept override;
    void conversionCrashed(int tabIndex,
                           const std::exception_ptr& exception) noexcept override;

    void remodeled() noexcept override;
    void tabAdded() noexcept override;

    void tabTitleUpdated(int tabIndex,
                         const QString& tabTitle) noexcept override;
    void sourceModeUpdated(int tabIndex,
                           const QString& sourceMode) noexcept override;
    void targetModeUpdated(int tabIndex,
                           const QString& targetMode) noexcept override;
    void sourceDataUpdated(int tabIndex,
                           const QString& sourceData) noexcept override;
    void targetDataUpdated(int tabIndex,
                           const QString& targetData) noexcept override;
    void sourceFilePathUpdated(int tabIndex,
                               const QString& filePath) noexcept override;
    void targetFilePathUpdated(int tabIndex,
                               const QString& filePath) noexcept override;

public: signals:
    void openAboutPressed();
    void openSettingsPressed();
    void openConvertUtilityPressed();

private slots: // UI Actions
    void tabTitleChanged(int tabIndex, const QString& tabTitle) noexcept;
    void sourceModeChanged(int tabIndex, const QString& currentMode) noexcept;
    void targetModeChanged(int tabIndex, const QString& currentMode) noexcept;
    void sourceDataChanged(int tabIndex, const QString& data) noexcept;
    void targetDataChanged(int tabIndex, const QString& data) noexcept;

    void fileOpened(int tabIndex,
                    const QString& sourceData,
                    const QString& filePath) noexcept;

    void focusChanged(int tabIndex,
                      view::IMainWindow::FieldInFocus inFocus) noexcept;

    void convert(int tabIndex) noexcept;

    void openNewTab() noexcept;
    void closeTab(int tabIndex) noexcept;
    void tabChanged(int currTabIndex) noexcept;
    void tabMoved(int from, int to) noexcept;

    void openFile(int tabIndex) noexcept;
    void saveFile(int tabIndex) noexcept;
    void saveFileAs(int tabIndex) noexcept;

    void openOnlineHelp() const noexcept;
    void openContactUs() const noexcept;
    void openSite() const noexcept;
    void openSourceCode() const noexcept;

private:
    const model::Settings& settings;
    model::Converter& converter;

    view::IMainWindow& mainWindow;
};

#endif // MAIN_WINDOW_PRESENTER_HPP
