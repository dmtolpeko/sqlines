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

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QString>
#include <QStringList>
#include <QMainWindow>
#include <QComboBox>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>

#include "imainwindow.hpp"
#include "imainwindowsettings.hpp"
#include "tabwidget.hpp"

namespace ui {
    class MainWindow;
}

class ui::MainWindow : public QMainWindow, public view::IMainWindow, public view::IMainWindowSettings {
    Q_OBJECT
    
public:
    explicit MainWindow(const QStringList& sourceModes,
                        const QStringList& targetModes,
                        QWidget* parent = nullptr) noexcept;
    MainWindow(const ui::MainWindow& other) = delete;
    ~MainWindow() override = default;

public:
    void showConversionStart(int tabIndex) noexcept override;
    void showConversionEnd(int tabIndex) noexcept override;
    void showConversionError(int tabIndex,
                             const QString& errorString) noexcept override;
    void showFileOpeningError(const QString& errorString) noexcept override;

    void showTabTitle(int tabIndex, const QString &data) noexcept override;
    void showSourceMode(const QString &mode) noexcept override;
    void showTargetMode(const QString &mode) noexcept override;
    void showSourceData(int tabIndex, const QString &data) noexcept override;
    void showTargetData(int tabIndex, const QString &data) noexcept override;
    void showFilePath(const QString &path) noexcept override;

    void openNewTab(bool lineNumberEnabled,
                    bool highlighterEnabled,
                    bool wrappingEnabled) noexcept override;
    void closeTab(int tabIndex) noexcept override;

    QString chooseFileToOpen(const QString& currentDir) noexcept override;
    QString choseFileToCreate(const QString& currentDir) noexcept override;

    int currentTabIndex() const noexcept override;
    QString currentTabTitle() const noexcept override;
    QString currentSourceMode() const noexcept override;
    QString currentTargetMode() const noexcept override;
    view::IMainWindow::FieldInFocus inFocus() const noexcept override;

    void reset() noexcept override;

    int height() const noexcept override;
    int width() const noexcept override;
    int posX() const noexcept override;
    int posY() const noexcept override;

    void resize(int width, int height) noexcept override;
    void move(int x, int y) noexcept override;

    void changeTitle(const QString &title) noexcept override;
    void changeNumberArea(bool isOn) noexcept override;
    void changeHighlighting(bool isOn) noexcept override;
    void changeWrapping(bool isOn) noexcept override;

public: signals:
    void tabTitleChanged(int tabIndex, const QString& tabTitle) override;
    void sourceDataChanged(int tabindex, const QString& data) override;
    void targetDataChanged(int tabindex, const QString& data) override;
    void sourceModeChanged(int tabIndex, const QString& currentMode) override;
    void targetModeChanged(int tabIndex, const QString& currentMode) override;

    void fileOpened(int TabIndex,
                    const QString& sourceData,
                    const QString& filePath) override;
    void focusChanged(int tabIndex,
                      view::IMainWindow::FieldInFocus inFocus) override;

    void convertPressed(int tabIndex) override;

    void openNewTabPressed() override;
    void closeTabPressed(int tabIndex) override;
    void tabChanged(int currTabIndex) override;
    void tabMoved(int to, int from) override;

    void openFilePressed(int tabIndex) override;
    void saveFilePressed(int tabIndex) override;
    void saveFileAsPressed(int tabIndex) override;

    void openAboutPressed() override;
    void openSettingsPressed() override;
    void openConvertUtilityPressed() override;
    void openOnlineHelpPressed() override;
    void openContactUsPressed() override;
    void openSitePressed() override;
    void openSourceCodePressed() override;

private:
    // Make main window
    void paint() noexcept;
    
    // Make tool bar
    void makeToolBar() noexcept;
    void makeTabsSection(QToolBar* parent) noexcept;
    void makeFileSection(QToolBar* parent) noexcept;
    void makeToolsSection(QToolBar* parent) noexcept;
    
    // Make menu bar
    void makeMenuBar() noexcept;
    QMenu* makeMainMenuTab(QMenuBar* parent) noexcept;
    QMenu* makeFileMenuTab(QMenuBar* parent) noexcept;
    QMenu* makeEditMenuTab(QMenuBar* parent) noexcept;
    QMenu* makeViewMenuTab(QMenuBar* parent) noexcept;
    QMenu* makeToolsMenuTab(QMenuBar* parent) noexcept;
    QMenu* makeHelpMenuTab(QMenuBar* parent) noexcept;
    
private:
    ui::TabWidget tabWidget;
    QComboBox sourceComboBox;
    QComboBox targetComboBox;
};

#endif // MAIN_WINDOW_HPP
