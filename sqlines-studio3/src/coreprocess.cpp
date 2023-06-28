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

#include <QCoreApplication>
#include <QStringList>
#include <QByteArray>
#include <QFileInfo>

#include "coreprocess.hpp"

using namespace model;

CoreProcess::processError::processError(const QString& errorString,
                                        const QString& log,
                                        int exitCode) noexcept
 :  std::runtime_error(errorString.toStdString()),
    processLog(log),
    processExitCode(exitCode) {}

QString CoreProcess::processError::errorType() const noexcept
{
    return { std::runtime_error::what() };
}

const QString& CoreProcess::processError::log() const noexcept
{
    return this->processLog;
}

int CoreProcess::processError::exitCode() const noexcept
{
    return this->processExitCode;
}

CoreProcess::CoreProcess(const QString& processPath) noexcept
 :  processPath(processPath) {}

void CoreProcess::run(const QStringList& arguments)
{
    if (!QFileInfo::exists(this->processPath)) {
        throw noProcess();
    }

    this->process.start(this->processPath, arguments);
    this->process.waitForFinished();

    this->processLog = this->process.readAllStandardOutput();
    this->processExitCode = this->process.exitCode();

    this->process.close();

    if (this->process.exitStatus() == QProcess::CrashExit) {
        errorOccurred();
    }
}

const QString& CoreProcess::log() const noexcept
{
    return this->processLog;
}

void CoreProcess::errorOccurred()
{
    QProcess::ProcessError errorType = this->process.error();
    
    if (errorType == QProcess::FailedToStart) {
        throw processError("SQLines converter failed to start",
                           this->processLog, this->processExitCode);
        
    } else if (errorType == QProcess::Crashed) {
        throw processError("SQLines converter crashed",
                           this->processLog, this->processExitCode);
    }
    
    if (this->processExitCode == -1) {
        throw processError("No source / target file",
                           this->processLog, this->processExitCode);
        
    } else if (this->processExitCode == -2) {
        throw processError("Incorrect contents of the source file",
                           this->processLog, this->processExitCode);
        
    } else if (this->processExitCode == -3) {
        throw processError("Error while writing to the target file",
                           this->processLog, this->processExitCode);
    }
    
    throw processError("Unknown error",
                       this->processLog, this->processExitCode);
}
