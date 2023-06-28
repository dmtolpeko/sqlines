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

#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <mutex>
#include <atomic>
#include <utility>
#include <map>
#include <vector>
#include <exception>
#include <memory>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QObject>

#include "settings.hpp"
#include "snapshot.hpp"
#include "iconverterobserver.hpp"

namespace model {
    class Converter;
}

class model::Converter : public QObject {
    Q_OBJECT

public:
    class filesystemError : public std::runtime_error {
    public:
        enum class FileType { Source, Target, Log };

    public:
        filesystemError(filesystemError::FileType fileType,
                        const QString& filePath,
                        const QString& errorString) noexcept;

        filesystemError::FileType fileType() const noexcept;
        QString filePath() const noexcept;

    private:
        FileType type;
        QString path;
    };

    class fileCreationError : public filesystemError {
    public:
        fileCreationError(filesystemError::FileType fileType,
                          const QString& filePath,
                          const QString& errorString) noexcept;
    };

    class fileOpenError : public filesystemError {
    public:
        fileOpenError(filesystemError::FileType fileType,
                      const QString& filePath,
                      const QString& errorString) noexcept;
    };

    class fileDoesNotExist : public filesystemError {
    public:
        fileDoesNotExist(filesystemError::FileType fileType,
                         const QString& filePath) noexcept;
    };

    class fileIsTooLarge : public filesystemError {
    public:
        fileIsTooLarge(filesystemError::FileType fileType,
                       const QString& filePath) noexcept;
    };

    class noConfigFile : public std::exception {};
    class noTab : public std::exception {};
    class noConversionData : public std::exception {};

public:
    Converter(const model::Settings& settings,
              const std::map<QString, QString>& path,
              const std::atomic<bool>& programIsRunning) noexcept;
    ~Converter() override;

    void addObserver(model::IConverterObserver& observer) noexcept;

    void makeSnapshot(model::Snapshot& snapshot) noexcept;
    void restoreFromSnapshot(const model::Snapshot& snapshot) noexcept;

    void updateTabTitle(int tabIndex, const QString& tabTitle);
    void updateSourceMode(int tabIndex, const QString& sourceMode);
    void updateTargetMode(int tabIndex, const QString& targetMode);
    void updateSourceData(int tabIndex, const QString& sourceData);
    void updateTargetData(int tabIndex, const QString& targetData);
    void updateSourceFilePath(int tabIndex, const QString& filePath);

    int tabsNumber() const noexcept;
    const QString& sourceMode(int tabIndex) const;
    const QString& targetMode(int tabIndex) const;
    const QString& sourceFilePath(int tabIndex) const;
    const QString& targetFilePath(int tabIndex) const;

    void addNewTab() noexcept;
    void deleteTab(int tabIndex);
    void moveTab(int from, int to);

    void convert(int tabIndex);

    void openFile(int tabIndex, const QString& filePath);
    void saveSourceFile(int tabIndex);
    void saveSourceFileAs(int tabIndex, const QString& filePath);
    void saveTargetFile(int tabIndex);
    void saveTargetFileAs(int tabIndex, const QString& filePath);

private:
    void reset() noexcept;

    void saveSourceDataInCache(int tabIndex) const noexcept;
    void saveTargetDataInCache(int tabIndex) const noexcept;

    void restoreSourceDataFromCache(int tabIndex) noexcept;
    void restoreTargetDataFromCache(int tabIndex) noexcept;

    void restoreSourceData(int tabIndex, const QString& filePath) noexcept;
    void restoreTargetData(int tabIndex, const QString& filePath) noexcept;

    QStringList prepareArguments(int tabIndex) const;
    QString createSourceFile(int tabIndex) const;
    QString createTargetFile(int tabIndex) const;
    QString createLogFile() const;

    // Tracking changes in opened files
    void monitorFileChanges(const std::atomic<bool>& programIsRunning) noexcept;
    void updateSourceFileData(int tabIndex) noexcept;
    void updateTargetFileData(int tabIndex) noexcept;

private: signals:
    // UI changes can only be made from the main thread
    void conversionStartedInDetachedThread(int tabIndex);
    void conversionEndedInDetachedThread(int tabIndex);
    void conversionCrashedInDetachedThread(int tabIndex,
                                           const std::exception_ptr& exception);
    void sourceDataChangedInDetachedThread(int tabIndex,
                                           const QString& sourceData);
    void targetDataChangedInDetachedThread(int tabIndex,
                                           const QString& targetData);
    void sourceFilePathChangedInDetachedThread(int tabIndex,
                                               const QString& filePath);
    void targetFilePathChangedInDetachedThread(int tabIndex,
                                               const QString& filePath);

private:
    struct TabData {
        QString tabTitle;
        QString sourceMode;
        QString targetMode;
        QString sourceData;
        QString targetData;
        QString sourceFilePath;
        QString targetFilePath;
    };

private:
    std::vector<std::reference_wrapper<IConverterObserver>> observers;

    std::vector<TabData> tabsData;
    std::vector<std::pair<QString, QDateTime>> sourceFiles;
    std::vector<std::pair<QString, QDateTime>> targetFiles;

    const model::Settings& settings;
    const std::map<QString, QString>& cmdModes;

    mutable std::vector<std::unique_ptr<std::mutex>> tabLocks;
    mutable std::mutex mutex;
};

#endif // CONVERTER_HPP
