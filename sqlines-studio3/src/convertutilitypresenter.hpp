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

#ifndef CONVERT_UTILITY_PRESENTER_HPP
#define CONVERT_UTILITY_PRESENTER_HPP

#include <map>
#include <exception>
#include <QString>
#include <QStringList>
#include <QObject>

#include "settings.hpp"
#include "iconvertutilitywidget.hpp"

namespace presenter {
    class ConvertUtilityPresenter;
}

class presenter::ConvertUtilityPresenter : public QObject {
    Q_OBJECT

public:
    ConvertUtilityPresenter(const model::Settings& settings,
                            const std::map<QString, QString>& cmdModes,
                            view::IConvertUtilityWidget& view) noexcept;
    ~ConvertUtilityPresenter() override = default;

private slots: // UI actions
    void convert() noexcept;
    void selectFile() noexcept;

    void sourceModeSelected(const QString& sourceMode) noexcept;
    void targetModeSelected(const QString& targetMode) noexcept;

private:
    QStringList prepareArguments() const;
    QString createTargetFile() const;
    QString createLogFile() const;

private:
    class fileCreationError : public std::runtime_error {
    public:
        enum class FileType { Target, Log };

    public:
        fileCreationError(FileType fileType,
                          const QString& filePath,
                          const QString& errorString) noexcept;

        FileType fileType() const noexcept;
        const QString& filePath() const noexcept;

    private:
        FileType type;
        QString path;
    };

    class noConfigFile : public std::exception {};
    class noArguments : public std::exception {};

private:
    QString sourceMode;
    QString targetMode;
    QString sourceFilePath;

    const model::Settings& settings;
    const std::map<QString, QString>& cmdModes;

    view::IConvertUtilityWidget& view;
};

#endif // CONVERT_UTILITY_PRESENTER_HPP
