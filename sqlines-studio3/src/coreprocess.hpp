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

#ifndef CORE_PROCESS_HPP
#define CORE_PROCESS_HPP

#include <exception>
#include <QProcess>
#include <QString>
#include <QStringList>

namespace model {
    class CoreProcess;
}

class model::CoreProcess {
public:
    class noProcess : public std::exception {};

    class processError : public std::runtime_error {
    public:
        processError(const QString& errorString,
                     const QString& log,
                     int exitCode) noexcept;

        QString errorType() const noexcept;
        const QString& log() const noexcept;
        int exitCode() const noexcept;
        
    private:
        QString processLog;
        int processExitCode;
    };
    
public:
    explicit CoreProcess(const QString& processPath) noexcept;

    void run(const QStringList& arguments);
    const QString& log() const noexcept;

private:
    void errorOccurred();

private:
    QProcess process;
    QString processPath;
    QString processLog;
    int processExitCode;
};

#endif // CORE_PROCESS_HPP
