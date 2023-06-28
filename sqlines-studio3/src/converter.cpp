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

#include <thread>
#include <chrono>
#include <algorithm>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "converter.hpp"
#include "coreprocess.hpp"

using namespace model;

Converter::filesystemError::filesystemError(filesystemError::FileType fileType,
                                            const QString& filePath,
                                            const QString& errorString) noexcept
 : std::runtime_error(errorString.toStdString()),
   type(fileType),
   path(filePath) {}

Converter::filesystemError::FileType Converter::filesystemError::fileType() const noexcept
{
    return this->type;
}

QString Converter::filesystemError::filePath() const noexcept
{
    return this->path;
}

Converter::fileCreationError::fileCreationError(filesystemError::FileType fileType,
                                                const QString& filePath,
                                                const QString& errorString) noexcept
 :  filesystemError(fileType, filePath, errorString) {}

Converter::fileOpenError::fileOpenError(filesystemError::FileType fileType,
                                        const QString& filePath,
                                        const QString& errorString) noexcept
 :   filesystemError(fileType, filePath, errorString) {}

Converter::fileDoesNotExist::fileDoesNotExist(filesystemError::FileType fileType,
                                              const QString& filePath) noexcept
 :   filesystemError(fileType, filePath, "The file does not exist") {}

Converter::fileIsTooLarge::fileIsTooLarge(filesystemError::FileType fileType,
                                          const QString& filePath) noexcept
 :  filesystemError(fileType, filePath, "The file is too large") {}

Converter::Converter(const model::Settings& settings,
                     const std::map<QString, QString>& cmdModes,
                     const std::atomic<bool>& programIsRunning) noexcept
 :  settings(settings), cmdModes(cmdModes)
 {
    std::thread thread([&] { monitorFileChanges(programIsRunning); });
    thread.detach();

    // UI changes can only be made from the main thread
    auto conversionStarted = [this](int tabIndex) {
        for (auto observer : this->observers) {
            observer.get().conversionStarted(tabIndex);
        }
    };
    connect(this, &Converter::conversionStartedInDetachedThread,
            this, conversionStarted);

    auto conversionEnded = [this](int tabIndex) {
        for (auto observer : this->observers) {
            observer.get().conversionEnded(tabIndex);
        }
    };
    connect(this, &Converter::conversionEndedInDetachedThread,
            this, conversionEnded);

    auto conversionCrashed = [this](int tabIndex, const std::exception_ptr& exception) {
        for (auto observer : this->observers) {
            observer.get().conversionCrashed(tabIndex, exception);
        }
    };
    connect(this, &Converter::conversionCrashedInDetachedThread,
            this, conversionCrashed);

    auto sourceDataChanged = [this](int tabIndex, const QString& data) {
        for (auto observer : this->observers) {
            observer.get().sourceDataUpdated(tabIndex, data);
        }
    };
    connect(this, &Converter::sourceDataChangedInDetachedThread,
            this, sourceDataChanged);

    auto targetDataChanged = [this](int tabIndex, const QString& data) {
        for (auto observer : this->observers) {
            observer.get().targetDataUpdated(tabIndex, data);
        }
    };
    connect(this, &Converter::targetDataChangedInDetachedThread,
            this, targetDataChanged);

    auto sourceFilePathChanged = [this](int tabIndex, const QString& path) {
        for (auto observer : this->observers) {
            observer.get().sourceFilePathUpdated(tabIndex, path);
        }
    };
    connect(this, &Converter::sourceFilePathChangedInDetachedThread,
            this, sourceFilePathChanged);

    auto targetFilePathChanged = [this](int tabIndex, const QString& path) {
        for (auto observer : this->observers) {
            observer.get().targetFilePathUpdated(tabIndex, path);
        }
    };
    connect(this, &Converter::targetFilePathChangedInDetachedThread,
            this, targetFilePathChanged);
}

Converter::~Converter()
{
    this->mutex.lock();
    for (auto& mutex : this->tabLocks) {
        mutex->lock();
    }
}

void Converter::addObserver(model::IConverterObserver& observer) noexcept
{
    this->observers.emplace_back(observer);
}

void Converter::makeSnapshot(model::Snapshot& snapshot) noexcept
{
    std::lock_guard<std::mutex> lock(this->mutex);

    auto tabsNumber = this->tabsData.size();
    snapshot["ui.tabs.tabs-number"] = QString::number(tabsNumber);

    // Save tab data
    for (auto i = 0; i < tabsNumber; i++) {
        this->tabLocks[i]->lock();

        QString index = QString::number(i);
        snapshot["ui.tabs." + index + ".tab-name"] = this->tabsData[i].tabTitle;
        snapshot["ui.tabs." + index + ".source-mode"] = this->tabsData[i].sourceMode;
        snapshot["ui.tabs." + index + ".target-mode"] = this->tabsData[i].targetMode;

        const QString& sourceFilePath = this->tabsData[i].sourceFilePath;
        if (!sourceFilePath.isEmpty()) {
            snapshot["ui.tabs." + index + ".source-filePath"] = sourceFilePath;
        } else if (!this->tabsData[i].sourceData.isEmpty()) {
            saveSourceDataInCache(i);
        }

        const QString& targetFilePath = this->tabsData[i].targetFilePath;
        if (!targetFilePath.isEmpty()) {
            snapshot["ui.tabs." + index + ".target-filePath"] = targetFilePath;
        } else if (!this->tabsData[i].targetData.isEmpty()) {
            saveTargetDataInCache(i);
        }

        this->tabLocks[i]->unlock();
    }
}

void Converter::restoreFromSnapshot(const model::Snapshot& snapshot) noexcept
{
    std::lock_guard<std::mutex> lock(this->mutex);

    auto tabsNumber = snapshot["ui.tabs.tabs-number"].toInt();
    if (tabsNumber == 0) {
        return;
    }
    reset(); // Delete all tabs

    // Restore tab data
    for (auto i = 0; i < tabsNumber; i++) {
        QString index = QString::number(i);

        this->tabsData.emplace_back();
        this->sourceFiles.emplace_back();
        this->targetFiles.emplace_back();
        this->tabLocks.emplace_back(new std::mutex);

        this->tabLocks[i]->lock();

        this->tabsData[i].tabTitle = snapshot["ui.tabs." + index + ".tab-name"];
        this->tabsData[i].sourceMode = snapshot["ui.tabs." + index + ".source-mode"];
        this->tabsData[i].targetMode = snapshot["ui.tabs." + index + ".target-mode"];

        QString sourceFilePath = snapshot["ui.tabs." + index + ".source-filePath"];
        if (QFileInfo::exists(sourceFilePath)) {
            restoreSourceData(i, sourceFilePath);
        } else {
            restoreSourceDataFromCache(i);
        }

        QString targetFilePath = snapshot["ui.tabs." + index + ".target-filePath"];
        if (!targetFilePath.isEmpty() && QFileInfo::exists(targetFilePath)) {
            restoreTargetData(i, targetFilePath);
        } else {
            restoreTargetDataFromCache(i);
        }

        this->tabLocks[i]->unlock();

        for (auto& observer : this->observers) {
            const TabData& data = this->tabsData[i];

            observer.get().tabAdded();
            observer.get().tabTitleUpdated(i, data.tabTitle);
            observer.get().sourceModeUpdated(i, data.sourceMode);
            observer.get().targetModeUpdated(i, data.targetMode);
            observer.get().sourceDataUpdated(i, data.sourceData);
            observer.get().targetDataUpdated(i, data.targetData);
            observer.get().targetFilePathUpdated(i, data.targetFilePath);
            observer.get().sourceFilePathUpdated(i, data.sourceFilePath);
        }
    }
}

void Converter::updateTabTitle(int tabIndex, const QString& tabTitle)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    this->tabsData[tabIndex].tabTitle = tabTitle;
}

void Converter::updateSourceMode(int tabIndex, const QString& sourceMode)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    this->tabsData[tabIndex].sourceMode = sourceMode;
}

void Converter::updateTargetMode(int tabIndex, const QString& targetMode)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    this->tabsData[tabIndex].targetMode = targetMode;
}

void Converter::updateSourceData(int tabIndex, const QString& sourceData)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    this->tabsData[tabIndex].sourceData = sourceData;
}

void Converter::updateTargetData(int tabIndex, const QString& targetData)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    this->tabsData[tabIndex].targetData = targetData;
}

void Converter::updateSourceFilePath(int tabIndex, const QString& filePath)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);

    TabData& tabData = this->tabsData[tabIndex];
    tabData.sourceFilePath = filePath;

    if (QFileInfo::exists(tabData.sourceFilePath)) {
        this->sourceFiles[tabIndex].first = tabData.sourceFilePath;
        QDateTime lastModified = QFileInfo(tabData.sourceFilePath).lastModified();
        this->sourceFiles[tabIndex].second = std::move(lastModified);
    }
}

int Converter::tabsNumber() const noexcept
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->tabsData.size();
}

const QString& Converter::sourceMode(int tabIndex) const
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    return this->tabsData[tabIndex].sourceMode;
}

const QString& Converter::targetMode(int tabIndex) const
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    return this->tabsData[tabIndex].targetMode;
}

const QString& Converter::sourceFilePath(int tabIndex) const
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    return this->tabsData[tabIndex].sourceFilePath;
}

const QString& Converter::targetFilePath(int tabIndex) const
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);
    return this->tabsData[tabIndex].targetFilePath;
}

void Converter::addNewTab() noexcept
{
    std::lock_guard<std::mutex> lock(this->mutex);

    this->tabsData.emplace_back();
    this->sourceFiles.emplace_back();
    this->targetFiles.emplace_back();
    this->tabLocks.emplace_back(new std::mutex);
}

void Converter::deleteTab(int tabIndex)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(this->mutex);
    std::lock_guard<std::mutex> lock2(*this->tabLocks[tabIndex]);

    this->tabsData.erase(std::next(this->tabsData.cbegin(), tabIndex));
    this->sourceFiles.erase(std::next(this->sourceFiles.cbegin(), tabIndex));
    this->targetFiles.erase(std::next(this->targetFiles.cbegin(), tabIndex));
    this->tabLocks.erase(std::next(this->tabLocks.cbegin(), tabIndex));
}

void Converter::moveTab(int from, int to)
{
    if (from > this->tabsData.size() - 1 || to > this->tabsData.size() - 1 ||
    this->tabsData.empty()) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(this->mutex);

    auto tabFrom = std::next(this->tabsData.begin(), from);
    auto tabTo = std::next(this->tabsData.begin(), to);
    std::iter_swap(tabFrom, tabTo);

    auto sourceFileFrom = std::next(this->sourceFiles.begin(), from);
    auto sourceFileTo = std::next(this->sourceFiles.begin(), to);
    std::iter_swap(sourceFileFrom, sourceFileTo);

    auto targetFileFrom = std::next(this->targetFiles.begin(), from);
    auto targetFileTo = std::next(this->targetFiles.begin(), to);
    std::iter_swap(targetFileFrom, targetFileTo);

    auto mutexFrom = std::next(this->tabLocks.begin(), from);
    auto mutexTo = std::next(this->tabLocks.begin(), to);
    std::iter_swap(mutexFrom, mutexTo);
}

void Converter::convert(int tabIndex)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }

    auto runConversion = [this](int tabIndex) {
        std::lock_guard<std::mutex> lock(this->mutex);
        std::lock_guard<std::mutex> lock2(*this->tabLocks[tabIndex]);

        try {
            if (tabsData[tabIndex].sourceFilePath.isEmpty() &&
            tabsData[tabIndex].sourceData.isEmpty()) {
                throw noConversionData();
            }

            emit conversionStartedInDetachedThread(tabIndex);

            QStringList arguments = prepareArguments(tabIndex);
            model::CoreProcess process(this->settings["general.dirs.process-filePath"]);
            process.run(arguments);

            QString& targetFilePath = arguments[3];
            targetFilePath.remove("-out = ");

            QFile file(targetFilePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                throw fileOpenError(filesystemError::FileType::Target,
                                    targetFilePath,
                                    file.errorString());
            }

            QTextStream stream(&file);
            TabData& data = this->tabsData[tabIndex];
            data.targetData = stream.readAll();
            data.targetFilePath = targetFilePath;

            this->targetFiles[tabIndex].first = targetFilePath;
            this->targetFiles[tabIndex].second = QFileInfo(file).lastModified();

            emit conversionEndedInDetachedThread(tabIndex);
            emit targetDataChangedInDetachedThread(tabIndex, data.targetData);
            emit targetFilePathChangedInDetachedThread(tabIndex, data.targetFilePath);


        } catch (...) {
            std::exception_ptr exception = std::current_exception();
            emit conversionCrashedInDetachedThread(tabIndex, exception);
        }
    };

    std::thread conversionThread(runConversion, tabIndex);
    conversionThread.detach();
}

void Converter::openFile(int tabIndex, const QString& filePath)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);

    QFile file(filePath);
    if (file.size() > 2'000'000) { // 2 MB
        throw fileIsTooLarge(filesystemError::FileType::Source, filePath);
    }
    if (!file.exists()) {
        throw fileDoesNotExist(filesystemError::FileType::Source, filePath);
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw fileOpenError(filesystemError::FileType::Source,
                            filePath,
                            file.errorString());
    }

    QTextStream stream(&file);
    TabData& data = this->tabsData[tabIndex];
    data.sourceData = stream.readAll();
    data.sourceFilePath = filePath;
    data.tabTitle = QFileInfo(file).fileName();

    this->sourceFiles[tabIndex].first = filePath;
    this->sourceFiles[tabIndex].second = QFileInfo(file).lastModified();

    for (auto& observer : this->observers) {
        observer.get().sourceDataUpdated(tabIndex, data.sourceData);
        observer.get().sourceFilePathUpdated(tabIndex, data.sourceFilePath);
        observer.get().tabTitleUpdated(tabIndex, data.tabTitle);
    }
}

void Converter::saveSourceFile(int tabIndex)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);

    const QString& filePath = this->sourceFiles[tabIndex].first;
    QFile file(filePath);
    if (!file.exists()) {
        throw fileDoesNotExist(filesystemError::FileType::Source, filePath);
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw fileOpenError(filesystemError::FileType::Source,
                            filePath,
                            file.errorString());
    }

    QTextStream stream(&file);
    stream << this->tabsData[tabIndex].sourceData;

    this->sourceFiles[tabIndex].second = QFileInfo(file).lastModified();
}

void Converter::saveSourceFileAs(int tabIndex, const QString& filePath)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { ;
        throw fileOpenError(filesystemError::FileType::Source,
                            filePath,
                            file.errorString());
    }

    TabData& data = this->tabsData[tabIndex];
    QTextStream stream(&file);
    stream << data.sourceData;
    data.sourceFilePath = filePath;

    this->sourceFiles[tabIndex].first = filePath;
    this->sourceFiles[tabIndex].second = QFileInfo(filePath).lastModified();

    for (auto& observer : this->observers) {
        observer.get().sourceFilePathUpdated(tabIndex, data.sourceFilePath);
    }
}

void Converter::saveTargetFile(int tabIndex)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);

    const QString& filePath = this->targetFiles[tabIndex].first;
    QFile file(filePath);
    if (!file.exists()) {
        throw fileDoesNotExist(filesystemError::FileType::Target, filePath);
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw fileOpenError(filesystemError::FileType::Target,
                            filePath,
                            file.errorString());
    }

    QTextStream stream(&file);
    stream << this->tabsData[tabIndex].targetData;

    this->targetFiles[tabIndex].second = QFileInfo(file).lastModified();
}

void Converter::saveTargetFileAs(int tabIndex, const QString& filePath)
{
    if (this->tabsData.empty() || tabIndex > this->tabsData.size() - 1) {
        throw noTab();
    }
    std::lock_guard<std::mutex> lock(*this->tabLocks[tabIndex]);

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { ;
        throw fileOpenError(filesystemError::FileType::Target,
                            filePath,
                            file.errorString());
    }

    TabData& data = this->tabsData[tabIndex];
    QTextStream stream(&file);
    stream << data.targetData;
    this->tabsData[tabIndex].targetFilePath = filePath;

    this->targetFiles[tabIndex].first = filePath;
    this->targetFiles[tabIndex].second = QFileInfo(filePath).lastModified();

    for (auto& observer : this->observers) {
        observer.get().targetFilePathUpdated(tabIndex, data.targetFilePath);
    }
}

void Converter::reset() noexcept
{
    this->tabsData.clear();
    this->sourceFiles.clear();
    this->targetFiles.clear();
    this->tabLocks.clear();

    for (auto& observer : this->observers) {
        observer.get().remodeled();
    }
}

void Converter::saveSourceDataInCache(int tabIndex) const noexcept
{
    QString fileName = this->settings["general.dirs.cache-dir"];
    fileName += "/source" + QString::number(tabIndex) + ".txt";

    QFile file(QDir::toNativeSeparators(fileName));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << this->tabsData[tabIndex].sourceData;
    }
}

void Converter::saveTargetDataInCache(int tabIndex) const noexcept
{
    QString fileName = this->settings["general.dirs.cache-dir"];
    fileName += "/target" + QString::number(tabIndex) + ".txt";

    QFile file(QDir::toNativeSeparators(fileName));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << this->tabsData[tabIndex].targetData;
    }
}

void Converter::restoreSourceDataFromCache(int tabIndex) noexcept
{
    QString fileName = this->settings["general.dirs.cache-dir"];
    fileName += "/source" + QString::number(tabIndex) + ".txt";
    fileName = QDir::toNativeSeparators(fileName);

    QFile file(QDir::toNativeSeparators(fileName));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        this->tabsData[tabIndex].sourceData = stream.readAll();
    }
}

void Converter::restoreTargetDataFromCache(int tabIndex) noexcept
{
    QString fileName = this->settings["general.dirs.cache-dir"];
    fileName += "/target" + QString::number(tabIndex) + ".txt";
    fileName = QDir::toNativeSeparators(fileName);

    QFile file(QDir::toNativeSeparators(fileName));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        this->tabsData[tabIndex].targetData = stream.readAll();
    }
}

void Converter::restoreSourceData(int tabIndex, const QString& filePath) noexcept
{
    if (!QFileInfo::exists(filePath)) {
        return;
    }

    this->tabsData[tabIndex].sourceFilePath = filePath;
    this->sourceFiles[tabIndex].first = filePath;
    this->sourceFiles[tabIndex].second = QFileInfo(filePath).lastModified();

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        this->tabsData[tabIndex].sourceData = stream.readAll();
    }
}

void Converter::restoreTargetData(int tabIndex, const QString& filePath) noexcept
{
    if (!QFileInfo::exists(filePath)) {
        return;
    }

    this->tabsData[tabIndex].targetFilePath = filePath;
    this->targetFiles[tabIndex].first = filePath;
    this->targetFiles[tabIndex].second = QFileInfo(filePath).lastModified();

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        this->tabsData[tabIndex].targetData = stream.readAll();
    }
}

QStringList Converter::prepareArguments(int tabIndex) const
{
    QStringList arguments;

    QString sourceMode = this->tabsData[tabIndex].sourceMode;
    auto sourceCmdMode = this->cmdModes.find(sourceMode);
    if (sourceCmdMode == this->cmdModes.cend()) {
        throw noConfigFile();
    }
    arguments.append("-s = " + sourceCmdMode->second);

    QString targetMode = this->tabsData[tabIndex].targetMode;
    auto targetCmdMode = this->cmdModes.find(targetMode);
    if (targetCmdMode == this->cmdModes.cend()) {
        throw noConfigFile();
    }
    arguments.append("-t = " + targetCmdMode->second);

    QString sourceFilePath = this->tabsData[tabIndex].sourceFilePath;
    if (!sourceFilePath.isEmpty()) {
        if (this->tabsData[tabIndex].sourceData.isEmpty()) {
            throw noConversionData();
        }
        arguments.append("-in = " + sourceFilePath);
    } else {
        arguments.append("-in = " + createSourceFile(tabIndex));
    }

    arguments.append("-out = " + createTargetFile(tabIndex));
    arguments.append("-log = " + createLogFile());

    return arguments;
}

QString Converter::createSourceFile(int tabIndex) const
{
    if (this->tabsData[tabIndex].sourceData.isEmpty()) {
        throw noConversionData();
    }

    QString fileName = this->tabsData[tabIndex].tabTitle;
    QString cacheDir = this->settings["general.dirs.cache-dir"];
    QString filePath = cacheDir + "/" + fileName + ".txt";
    filePath = QDir::toNativeSeparators(filePath);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw fileCreationError(filesystemError::FileType::Source,
                                filePath,
                                file.errorString());
    }

    QTextStream stream(&file);
    stream << this->tabsData[tabIndex].sourceData;

    return filePath;
}

QString Converter::createTargetFile(int tabIndex) const
{
    auto fileExtension = this->cmdModes.find(this->tabsData[tabIndex].targetMode);
    if (fileExtension == this->cmdModes.cend()) {
        throw noConfigFile();
    }

    QString fileName = this->tabsData[tabIndex].tabTitle;
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
    filePath = QDir::toNativeSeparators(filePath);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw fileCreationError(filesystemError::FileType::Target,
                                filePath,
                                file.errorString());
    }

    return filePath;
}

QString Converter::createLogFile() const
{
    QString filePath = this->settings["general.dirs.cache-dir"];
    filePath += "/sqlineslog.txt";
    filePath = QDir::toNativeSeparators(filePath);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw fileCreationError(filesystemError::FileType::Log,
                                filePath,
                                file.errorString());
    }

    return filePath;
}

// Tracking changes in opened files
void Converter::monitorFileChanges(const std::atomic<bool>& programIsRunning) noexcept
{
    while (programIsRunning) {
       if(!this->mutex.try_lock()) {
           // Avoid accessing a null mutex when destroying an object
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
       }

       // Check source files
        for (auto i = this->sourceFiles.cbegin(); i != this->sourceFiles.cend(); ++i) {
            auto tabIndex = std::distance(this->sourceFiles.cbegin(), i);
            this->tabLocks[tabIndex]->lock();

            auto needsUpdate = [this, &i](int tabIndex) {
                return (!(*i).first.isEmpty() &&
                !QFileInfo::exists((*i).first)) ||
                (QFileInfo::exists((*i).first) &&
                this->tabsData[tabIndex].sourceFilePath.isEmpty()) ||
                QFileInfo((*i).first).lastModified() != (*i).second;
            };

            if (needsUpdate(tabIndex)) {
                updateSourceFileData(tabIndex);
            }
        }

        // Check target files
        for (auto i = this->targetFiles.cbegin(); i != this->targetFiles.cend(); ++i) {
            auto tabIndex = std::distance(this->targetFiles.cbegin(), i);

            auto needsUpdate = [this, &i](int tabIndex) {
                return (!(*i).first.isEmpty() &&
                !QFileInfo::exists((*i).first)) ||
                (QFileInfo::exists((*i).first) &&
                this->tabsData[tabIndex].targetFilePath.isEmpty()) ||
                QFileInfo((*i).first).lastModified() != (*i).second;
            };

            if (needsUpdate(tabIndex)) {
                updateTargetFileData(tabIndex);
            }

            this->tabLocks[tabIndex]->unlock();
        }

        this->mutex.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Converter::updateSourceFileData(int tabIndex) noexcept
{
    const QString& filePath = this->sourceFiles[tabIndex].first;
    TabData& tabData = this->tabsData[tabIndex];

    QFile file(filePath);
    if (!file.exists() && !tabData.sourceFilePath.isEmpty()) {
        tabData.sourceFilePath.clear();
        emit sourceFilePathChangedInDetachedThread(tabIndex, tabData.sourceFilePath);
        return;
    }

    if (file.exists() && tabData.sourceFilePath.isEmpty()) {
        tabData.sourceFilePath = filePath;
        emit sourceFilePathChangedInDetachedThread(tabIndex, tabData.sourceFilePath);
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    tabData.sourceData = stream.readAll();
    this->sourceFiles[tabIndex].second = QFileInfo(file).lastModified();

    emit sourceDataChangedInDetachedThread(tabIndex, tabData.sourceData);
}

void Converter::updateTargetFileData(int tabIndex) noexcept
{
    const QString& filePath = this->targetFiles[tabIndex].first;
    TabData& tabData = this->tabsData[tabIndex];

    QFile file(filePath);
    if (!file.exists() && !tabData.targetFilePath.isEmpty()) {
        tabData.targetFilePath.clear();
        emit targetFilePathChangedInDetachedThread(tabIndex, tabData.targetFilePath);
        return;
    }

    if (file.exists() && tabData.targetFilePath.isEmpty()) {
        tabData.targetFilePath = filePath;
        emit targetFilePathChangedInDetachedThread(tabIndex, tabData.targetFilePath);
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    tabData.targetData = stream.readAll();
    this->targetFiles[tabIndex].second = QFileInfo(file).lastModified();

    emit targetDataChangedInDetachedThread(tabIndex, tabData.targetData);
}
