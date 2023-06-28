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
#include <QDir>

#include "convertutilitypresenter.hpp"
#include "coreprocess.hpp"

using namespace presenter;

ConvertUtilityPresenter::ConvertUtilityPresenter(const model::Settings& settings,
                                                 const std::map<QString, QString>& cmdModes,
                                                 view::IConvertUtilityWidget& view) noexcept
 :  settings(settings), cmdModes(cmdModes), view(view)
{
    this->sourceMode = this->view.currentSourceMode();
    this->targetMode = this->view.currentTargetMode();

    auto widget = dynamic_cast<QObject*>(&this->view);

    connect(widget, SIGNAL(convertPressed()),
            this, SLOT(convert()));

    connect(widget, SIGNAL(selectFilePressed()),
            this, SLOT(selectFile()));

    connect(widget, SIGNAL(sourceModeSelected(QString)),
            this, SLOT(sourceModeSelected(QString)));

    connect(widget, SIGNAL(targetModeSelected(QString)),
            this, SLOT(targetModeSelected(QString)));
}

void ConvertUtilityPresenter::convert() noexcept
{
    try {
        QStringList arguments = prepareArguments();

        model::CoreProcess process(this->settings["general.dirs.process-filePath"]);
        process.run(arguments);

        QString& targetFilePath = arguments[3];
        targetFilePath.remove("-out = ");

        QString log = process.log() + "\nTarget file: " + targetFilePath;
        this->view.showLog(log);

    } catch (const noConfigFile&) {
        QString errorMsg = "Conversion error.\n\nNo config file.\n"
                           "Reinstall the application.";
        this->view.showLog(errorMsg);

    } catch (const noArguments&) {
        QString errorMsg = "Conversion error.\n\nSelect source file.";
        this->view.showLog(errorMsg);

    } catch (const fileCreationError& error) {
        QString errorMsg = "Conversion error.\n\nUnable to create ";
        if (error.fileType() == fileCreationError::FileType::Target) {
            errorMsg += "target file.";
        } else if (error.fileType() == fileCreationError::FileType::Log) {
            errorMsg += "log file.";
        }
        errorMsg += "\nPath: " + error.filePath();
        errorMsg += "\n\n" + QString(error.what()) + ".";

        this->view.showLog(errorMsg);

    } catch (const model::CoreProcess::noProcess&) {
        QString errorMsg = "Conversion error.\n\nNo SQLines process.\n"
                           "Reinstall the application.";
        this->view.showLog(errorMsg);

    } catch (const model::CoreProcess::processError& error) {
        QString errorMsg = "Conversion error.\n\nError type: ";
        errorMsg += error.errorType() + ".";
        errorMsg += "\nExit code: " + QString::number(error.exitCode()) + ".";
        errorMsg += "\nLog: " + error.log() + ".";

        this->view.showLog(errorMsg);
    }
}

void ConvertUtilityPresenter::selectFile() noexcept
{
    const QString& currDir = this->settings["general.dirs.curr-dir"];
    QString path = this->view.choseFileToOpen(currDir);
    if (path.isEmpty()) {
        return;
    }
    this->sourceFilePath = path;

    this->view.showFilePath(path);
    this->view.showLog("Log");
}

void ConvertUtilityPresenter::sourceModeSelected(const QString& sourceMode) noexcept
{
    this->sourceMode = sourceMode;
}

void ConvertUtilityPresenter::targetModeSelected(const QString& targetMode) noexcept
{
    this->targetMode = targetMode;
}

QStringList ConvertUtilityPresenter::prepareArguments() const
{
    QStringList arguments;

    auto sourceCmdMode = this->cmdModes.find(this->sourceMode);
    if (sourceCmdMode == this->cmdModes.cend()) {
        throw noConfigFile();
    }
    arguments.append("-s = " + sourceCmdMode->second);

    auto targetCmdMode = this->cmdModes.find(this->targetMode);
    if (targetCmdMode == this->cmdModes.cend()) {
        throw noConfigFile();
    }
    arguments.append("-t = " + targetCmdMode->second);

    if (this->sourceFilePath.isEmpty()) {
        throw noArguments();
    }
    QString sourceFileArg = "-in = " + this->sourceFilePath;
    arguments.append(sourceFileArg);

    QString targetFileArg = "-out = " + createTargetFile();
    arguments.append(targetFileArg);

    QString logFileArg = "-log = " + createLogFile();
    arguments.append(logFileArg);

    return arguments;
}

QString ConvertUtilityPresenter::createTargetFile() const
{
    auto fileExtension = this->cmdModes.find(this->targetMode);
    if (fileExtension == this->cmdModes.cend()) {
        throw noConfigFile();
    }

    QString fileName = QFileInfo(this->sourceFilePath).fileName();
    if (fileName.contains('.')) {
        for (auto i = fileName.size() - 1; i >= 0; i--) {
            if (fileName[i] == '.') {
                // Insert _target mode before the file extension
                fileName.insert(i, "_" + fileExtension->second);
                break;
            }
        }

    // Default tab name
    } else if (fileName.contains("Tab")) {
        fileName.remove(" ");
        fileName += "_" + fileExtension->second + ".txt";

    // A file name can have no extension on UNIX
    } else {
        fileName += "_" + fileExtension->second + ".txt";
    }

    QString filePath = this->settings["general.dirs.curr-dir"] + "/" + fileName;

    QFile file(QDir::toNativeSeparators(filePath));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw fileCreationError(fileCreationError::FileType::Target,
                                filePath,
                                file.errorString());
    }

    return QDir::toNativeSeparators(filePath);
}

QString ConvertUtilityPresenter::createLogFile() const
{
    QString filePath = this->settings["general.dirs.cache-dir"];
    filePath += QDir::toNativeSeparators("/utility_sqlineslog.txt");

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw fileCreationError(fileCreationError::FileType::Log,
                                filePath,
                                file.errorString());
    }

    return filePath;
}

ConvertUtilityPresenter::fileCreationError::fileCreationError(FileType fileType,
                                                              const QString& filePath,
                                                              const QString& errorString) noexcept
 :  std::runtime_error(errorString.toStdString()),
    type(fileType),
    path(filePath) {}

ConvertUtilityPresenter::fileCreationError::FileType
ConvertUtilityPresenter::fileCreationError::fileType() const noexcept
{
    return this->type;
}

const QString& ConvertUtilityPresenter::fileCreationError::filePath() const noexcept
{
    return this->path;
}
