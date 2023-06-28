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

#include <algorithm>
#include <QDesktopServices>
#include <QUrl>

#include "mainwindowpresenter.hpp"
#include "coreprocess.hpp"

using namespace presenter;

MainWindowPresenter::MainWindowPresenter(const model::Settings& settings,
                                         model::Converter& converter,
                                         view::IMainWindow& mainWindow) noexcept
 : settings(settings), converter(converter), mainWindow(mainWindow)
{
    openNewTab();
    this->converter.updateTabTitle(0, this->mainWindow.currentTabTitle());
    this->converter.updateSourceMode(0, this->mainWindow.currentSourceMode());
    this->converter.updateTargetMode(0, this->mainWindow.currentTargetMode());

    auto view = dynamic_cast<QObject*>(&this->mainWindow);

    connect(view, SIGNAL(tabTitleChanged(int,QString)),
            this, SLOT(tabTitleChanged(int,QString)));

    connect(view, SIGNAL(sourceDataChanged(int,QString)),
            this, SLOT(sourceDataChanged(int,QString)));

    connect(view, SIGNAL(targetDataChanged(int,QString)),
            this, SLOT(targetDataChanged(int,QString)));

    connect(view, SIGNAL(sourceModeChanged(int,QString)),
            this, SLOT(sourceModeChanged(int,QString)));

    connect(view, SIGNAL(targetModeChanged(int,QString)),
            this, SLOT(targetModeChanged(int,QString)));

    connect(view, SIGNAL(fileOpened(int,QString,QString)),
            this, SLOT(fileOpened(int,QString,QString)));

    connect(view, SIGNAL(focusChanged(int,view::IMainWindow::FieldInFocus)),
            this, SLOT(focusChanged(int,view::IMainWindow::FieldInFocus)));

    connect(view, SIGNAL(convertPressed(int)),
            this, SLOT(convert(int)));

    connect(view, SIGNAL(openNewTabPressed()),
            this, SLOT(openNewTab()));

    connect(view, SIGNAL(closeTabPressed(int)),
            this, SLOT(closeTab(int)));

    connect(view, SIGNAL(tabChanged(int)),
            this, SLOT(tabChanged(int)));

    connect(view, SIGNAL(tabMoved(int,int)),
            this, SLOT(tabMoved(int,int)));

    connect(view, SIGNAL(openFilePressed(int)),
            this, SLOT(openFile(int)));

    connect(view, SIGNAL(saveFilePressed(int)),
            this, SLOT(saveFile(int)));

    connect(view, SIGNAL(saveFileAsPressed(int)),
            this, SLOT(saveFileAs(int)));

    connect(view, SIGNAL(openAboutPressed()),
            this, SIGNAL(openAboutPressed()));

    connect(view, SIGNAL(openConvertUtilityPressed()),
            this, SIGNAL(openConvertUtilityPressed()));

    connect(view, SIGNAL(openSettingsPressed()),
            this, SIGNAL(openSettingsPressed()));

    connect(view, SIGNAL(openOnlineHelpPressed()),
            this, SLOT(openOnlineHelp()));

    connect(view, SIGNAL(openContactUsPressed()),
            this, SLOT(openContactUs()));

    connect(view, SIGNAL(openSitePressed()),
            this, SLOT(openSite()));

    connect(view, SIGNAL(openSourceCodePressed()),
            this, SLOT(openSourceCode()));
}

void MainWindowPresenter::conversionStarted(int tabIndex) noexcept
{
    this->mainWindow.showConversionStart(tabIndex);
}

void MainWindowPresenter::conversionEnded(int tabIndex) noexcept
{
    this->mainWindow.showConversionEnd(tabIndex);
}

void MainWindowPresenter::conversionCrashed(int tabIndex,
                                            const std::exception_ptr& exception) noexcept
{
    try {
        std::rethrow_exception(exception);

    } catch (const model::Converter::fileCreationError& error) {
        QString errorMsg = "Unable to create ";
        if (error.fileType() == model::Converter::fileCreationError::FileType::Source) {
            errorMsg += "source file.";
        } else if (error.fileType() == model::Converter::fileCreationError::FileType::Target) {
            errorMsg += "target file.";
        } else if (error.fileType() == model::Converter::fileCreationError::FileType::Log) {
            errorMsg += "log file.";
        }
        errorMsg += "\nPath: " + error.filePath();
        errorMsg += "\n\n" + QString(error.what()) + ".";

        this->mainWindow.showConversionError(tabIndex, errorMsg);

    } catch (const model::Converter::fileOpenError& error) {
        QString errorMsg = "Unable to open ";
        if (error.fileType() == model::Converter::fileOpenError::FileType::Source) {
            errorMsg += "source file.";
        } else if (error.fileType() == model::Converter::fileOpenError::FileType::Target) {
            errorMsg += "target file.";
        } else if (error.fileType() == model::Converter::fileOpenError::FileType::Log) {
            errorMsg += "log file.";
        }
        errorMsg += "\nPath: " + error.filePath();
        errorMsg += "\n\n" + QString(error.what()) + ".";

        this->mainWindow.showConversionError(tabIndex, errorMsg);

    } catch (const model::CoreProcess::processError& error) {
        QString errorMsg = "Error type: " + error.errorType();
        errorMsg += "\nExit code: " + QString::number(error.exitCode());
        errorMsg += "\nLog: " + error.log() + ".";

        this->mainWindow.showConversionError(tabIndex, errorMsg);

    } catch (const model::CoreProcess::noProcess&) {
        QString errorMsg = "No SQLines process.\nReinstall the application.";
        this->mainWindow.showConversionError(tabIndex, errorMsg);

    } catch (const model::Converter::noConfigFile&) {
        QString errorMsg = "No config file.\nReinstall the application.";
        this->mainWindow.showConversionError(tabIndex, errorMsg);

    } catch (const model::Converter::noConversionData&) {
        QString errorMsg = "No conversion data.\n"
                           "Open the file or enter the text manually.";
        this->mainWindow.showConversionError(tabIndex, errorMsg);

    } catch (...) {
        this->mainWindow.showConversionError(tabIndex, "Unknown error.");
    }
}

void MainWindowPresenter::remodeled() noexcept
{
    this->mainWindow.reset();
}

void MainWindowPresenter::tabAdded() noexcept
{
    bool lineNumberEnabled = this->settings["ui.input-field.number-area"].toInt();
    bool highlighterEnabled = this->settings["ui.input-field.highlighter"].toInt();
    bool wrappingEnabled = this->settings["ui.input-field.wrapping"].toInt();

    this->mainWindow.openNewTab(lineNumberEnabled,
                                highlighterEnabled,
                                wrappingEnabled);
}

void MainWindowPresenter::tabTitleUpdated(int tabIndex,
                                          const QString& tabTitle) noexcept
{
    this->mainWindow.showTabTitle(tabIndex, tabTitle);
}

void MainWindowPresenter::sourceModeUpdated(int, const QString& sourceMode) noexcept
{
    this->mainWindow.showSourceMode(sourceMode);
}

void MainWindowPresenter::targetModeUpdated(int, const QString& targetMode) noexcept
{
    this->mainWindow.showTargetMode(targetMode);
}

void MainWindowPresenter::sourceDataUpdated(int tabIndex,
                                            const QString& sourceData) noexcept
{
    this->mainWindow.showSourceData(tabIndex, sourceData);
}

void MainWindowPresenter::targetDataUpdated(int tabIndex,
                                            const QString& targetData) noexcept
{
    this->mainWindow.showTargetData(tabIndex, targetData);
}

void MainWindowPresenter::sourceFilePathUpdated(int tabIndex,
                                                const QString& filePath) noexcept
{
    if (this->mainWindow.currentTabIndex() == tabIndex) {
        this->mainWindow.showFilePath(filePath);
    }
}

void MainWindowPresenter::targetFilePathUpdated(int tabIndex,
                                                const QString& filePath) noexcept
{
    if (this->mainWindow.currentTabIndex() == tabIndex) {
        this->mainWindow.showFilePath(filePath);
    }
}

void MainWindowPresenter::tabTitleChanged(int tabIndex,
                                          const QString& tabTitle) noexcept
{
    this->converter.updateTabTitle(tabIndex, tabTitle);
}

void MainWindowPresenter::sourceModeChanged(int tabIndex,
                                            const QString& currentMode) noexcept
{
    this->converter.updateSourceMode(tabIndex, currentMode);
}

void MainWindowPresenter::targetModeChanged(int tabIndex,
                                            const QString& currentMode) noexcept
{
    this->converter.updateTargetMode(tabIndex, currentMode);
}

void MainWindowPresenter::sourceDataChanged(int tabIndex,
                                            const QString& data) noexcept
{
    this->converter.updateSourceData(tabIndex, data);
}

void MainWindowPresenter::targetDataChanged(int tabIndex,
                                            const QString& data) noexcept
{
    this->converter.updateTargetData(tabIndex, data);
}

void MainWindowPresenter::fileOpened(int tabIndex,
                                     const QString& sourceData,
                                     const QString& filePath) noexcept
{
    this->converter.updateSourceData(tabIndex, sourceData);
    this->converter.updateSourceFilePath(tabIndex, filePath);

    this->mainWindow.showFilePath(filePath);
}

void MainWindowPresenter::focusChanged(int tabIndex,
                                       view::IMainWindow::FieldInFocus inFocus) noexcept
{
    // Show the source file path of the current input field
    if (inFocus == view::IMainWindow::FieldInFocus::SourceField) {
        const QString& sourcePath = this->converter.sourceFilePath(tabIndex);
        this->mainWindow.showFilePath(sourcePath);

    } else if (inFocus == view::IMainWindow::FieldInFocus::TargetField) {
        const QString& targetPath = this->converter.targetFilePath(tabIndex);
        this->mainWindow.showFilePath(targetPath);
    }
}

void MainWindowPresenter::convert(int tabIndex) noexcept
{
    const QString& sourceFilePath = this->converter.sourceFilePath(tabIndex);
    if (!sourceFilePath.isEmpty()) {
        saveFile(tabIndex);
    }

    this->converter.convert(tabIndex);
}

void MainWindowPresenter::openNewTab() noexcept
{
    bool lineNumberEnabled = this->settings["ui.input-field.number-area"].toInt();
    bool highlighterEnabled = this->settings["ui.input-field.highlighter"].toInt();
    bool wrappingEnabled = this->settings["ui.input-field.wrapping"].toInt();

    this->converter.addNewTab();
    this->mainWindow.openNewTab(lineNumberEnabled,
                                highlighterEnabled,
                                wrappingEnabled);

    this->converter.updateTabTitle(this->mainWindow.currentTabIndex(),
                                   this->mainWindow.currentTabTitle());
    this->converter.updateSourceMode(this->mainWindow.currentTabIndex(),
                                     this->mainWindow.currentSourceMode());
    this->converter.updateTargetMode(this->mainWindow.currentTabIndex(),
                                     this->mainWindow.currentTargetMode());
}

void MainWindowPresenter::closeTab(int tabIndex) noexcept
{
    if (this->converter.tabsNumber() == 1) {
        return;
    }
    this->converter.deleteTab(tabIndex);
    this->mainWindow.closeTab(tabIndex);
}

void MainWindowPresenter::tabChanged(int currTabIndex) noexcept
{
    // Show selected modes in current tab
    const QString& sourceMode = this->converter.sourceMode(currTabIndex);
    const QString& targetMode = this->converter.targetMode(currTabIndex);
    this->mainWindow.showSourceMode(sourceMode);
    this->mainWindow.showTargetMode(targetMode);

    // Show the source file path of the current input field
    view::IMainWindow::FieldInFocus inFocus = this->mainWindow.inFocus();
    if (inFocus == view::IMainWindow::FieldInFocus::SourceField) {
        const QString& sourcePath = this->converter.sourceFilePath(currTabIndex);
        this->mainWindow.showFilePath(sourcePath);

    } else if (inFocus == view::IMainWindow::FieldInFocus::TargetField) {
        const QString& targetPath = this->converter.targetFilePath(currTabIndex);
        this->mainWindow.showFilePath(targetPath);
    }
}

void MainWindowPresenter::tabMoved(int from, int to) noexcept
{
    this->converter.moveTab(from, to);
}

void MainWindowPresenter::openFile(int tabIndex) noexcept
{
    QString currDir = this->settings["general.dirs.curr-dir"];
    QString path = this->mainWindow.chooseFileToOpen(currDir);
    if (path.isEmpty()) {
        return;
    }

    try {
        this->converter.openFile(tabIndex, path);

    } catch (model::Converter::filesystemError& error) {
        QString errorMsg = "Unable to open source file.";
        errorMsg += "\nPath: " + error.filePath();
        errorMsg += "\n\n" + QString(error.what()) + ".";
        errorMsg += "\nUse the Conversion utility"
                    "(Tools->Conversion utility) for large files.";

        this->mainWindow.showFileOpeningError(errorMsg);
    }
}

void MainWindowPresenter::saveFile(int tabIndex) noexcept
{
    try {
        view::IMainWindow::FieldInFocus workingField = this->mainWindow.inFocus();

        if (workingField == view::IMainWindow::FieldInFocus::SourceField) {
            const QString& filePath = this->converter.sourceFilePath(tabIndex);

            if (!filePath.isEmpty()) {
                this->converter.saveSourceFile(tabIndex);
            } else {
                saveFileAs(tabIndex);
            }

        } else if (workingField == view::IMainWindow::FieldInFocus::TargetField) {
            const QString& filePath = this->converter.targetFilePath(tabIndex);

            if (!filePath.isEmpty()) {
                this->converter.saveTargetFile(tabIndex);
            } else {
                saveFileAs(tabIndex);
            }
        }

    } catch (const model::Converter::filesystemError& error) {
        QString errorMsg = "Unable to open ";
        if (error.fileType() == model::Converter::fileOpenError::FileType::Source) {
            errorMsg += "source file.";
        } else if (error.fileType() == model::Converter::fileOpenError::FileType::Target) {
            errorMsg += "target file.";
        }
        errorMsg += "\nPath: " + error.filePath();
        errorMsg += "\n\n" + QString(error.what()) +  ".";

        this->mainWindow.showFileOpeningError(errorMsg);
    }
}

void MainWindowPresenter::saveFileAs(int tabIndex) noexcept
{
    QString currDir = this->settings["general.dirs.curr-dir"];
    QString path = this->mainWindow.choseFileToCreate(currDir);
    if (path.isEmpty()) {
        return;
    }

    try {
        view::IMainWindow::FieldInFocus workingField = this->mainWindow.inFocus();

        if (workingField == view::IMainWindow::FieldInFocus::SourceField) {
            this->converter.saveSourceFileAs(tabIndex, path);

        } else if (workingField == view::IMainWindow::FieldInFocus::TargetField) {
            this->converter.saveTargetFileAs(tabIndex, path);
        }

    } catch (const model::Converter::fileOpenError& error) {
        QString errorMsg = "Unable to open ";
        if (error.fileType() == model::Converter::fileOpenError::FileType::Source) {
            errorMsg += "source file.";
        } else if (error.fileType() == model::Converter::fileOpenError::FileType::Target) {
            errorMsg += "target file.";
        }
        errorMsg += "\nPath: " + error.filePath();
        errorMsg += "\n\n" + QString(error.what()) + ".";

        this->mainWindow.showFileOpeningError(errorMsg);
    }
}

void MainWindowPresenter::openOnlineHelp() const noexcept
{
    QDesktopServices::openUrl(QUrl("https://www.sqlines.com/support"));
}

void MainWindowPresenter::openContactUs() const noexcept
{
    QDesktopServices::openUrl(QUrl("https://www.sqlines.com/contact-us"));
}

void MainWindowPresenter::openSite() const noexcept
{
    QDesktopServices::openUrl(QUrl("https://www.sqlines.com"));
}

void MainWindowPresenter::openSourceCode() const noexcept
{
    QDesktopServices::openUrl(QUrl("https://github.com/dmtolpeko/sqlines/tree/master/sqlines-studio3"));
}
