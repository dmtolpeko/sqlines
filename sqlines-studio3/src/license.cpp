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
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QDir>
#include <QTextStream>

#include "license.hpp"
#include "coreprocess.hpp"

using namespace model;

License::License(const model::Settings& settings,
                 const std::atomic<bool>& programIsRunning) noexcept
 :  settings(settings)
{
    const QString& licensePath = this->settings["general.dirs.license-filePath"];
    this->licenseFileLastModified = QFileInfo(licensePath).lastModified();

    std::thread thread([&] { monitorLicenseFile(programIsRunning); });
    thread.detach();

    auto statusChanged = [this](bool isActive, const QString& info) {
        for (auto& observer : this->observers) {
            observer.get().licenseStatusChanged(isActive, info);
        }
    };
    // UI changes can only be made from the main thread
    connect(this, &License::statusChangedInDetachedThread,
            this, statusChanged);
}

License::~License()
{
    this->mutex.lock();
}

void License::addObserver(ILicenseObserver& observer) noexcept
{
    this->observers.emplace_back(observer);
}

QString License::info() const
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return licenseInfo();
}

bool License::isActive() const
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return licenseIsActive();
}

bool License::exists() const noexcept
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return QFileInfo::exists(this->settings["general.dirs.license-filePath"]);
}

void License::changeLicense(const QString& regName, const QString& regNumber)
{
    if (!QFileInfo::exists(this->settings["general.dirs.license-filePath"])) {
        throw noLicenseFile();
    }
    std::lock_guard<std::mutex> lock(this->mutex);

    QString licensePath = this->settings["general.dirs.license-filePath"];
    QFile file(licensePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString info = "SQLines license file:\n"
                       "\nRegistration Name: " + regName +
                       "\nRegistration Number: " + regNumber;

        QTextStream stream(&file);
        stream << info;

        this->licenseFileLastModified = QFileInfo(licensePath).lastModified();
    }
}

// Lock-free license info
QString License::licenseInfo() const
{
    if (!QFileInfo::exists(this->settings["general.dirs.license-filePath"])) {
        throw noLicenseFile();
    }

    QString processPath = this->settings["general.dirs.process-filePath"];
    model::CoreProcess coreProcess(processPath);
    try {
        QStringList arguments { createLogFile() };
        coreProcess.run(arguments); // Get log from the core process

    } catch (const CoreProcess::noProcess&) {
        return "THE PRODUCT IS FOR EVALUATION USE ONLY.";
    }

    QString log = coreProcess.log();
    if (log.contains("The product is licensed to")) {
        // Get license info from the log
        auto startIndex = log.indexOf("The product is licensed to");
        auto strSize = QString("The product is licensed to").size();
        auto endIndex = startIndex + strSize + 1;

        while (log[endIndex] != '\n') {
            endIndex++;
        }

        // Get "The product is licensed to..." string from the log
        log.erase(log.cbegin() + endIndex, log.cend());
        log.erase(log.cbegin(), log.cbegin() + startIndex);

        return log + '.';

    } else {
        return "THE PRODUCT IS FOR EVALUATION USE ONLY.";
    }
}

// Lock-free license is active
bool License::licenseIsActive() const
{
    if (!QFileInfo::exists(this->settings["general.dirs.license-filePath"])) {
        throw noLicenseFile();
    }

    QString processPath = this->settings["general.dirs.process-filePath"];
    model::CoreProcess coreProcess(processPath);

    try {
        QStringList arguments { createLogFile() };
        coreProcess.run(arguments); // Get log from the core process

    } catch (const CoreProcess::noProcess&) {
        return false;
    }

    const QString& log = coreProcess.log();
    return !log.contains("FOR EVALUATION USE ONLY");
}

// Log file for the core process
QString License::createLogFile() const noexcept
{
    QString filePath = this->settings["general.dirs.cache-dir"];
    filePath += "/license_sqlineslog.txt";

    return QDir::toNativeSeparators(filePath);
}

// Tracking manual changes in the license file
void License::monitorLicenseFile(const std::atomic<bool>& programIsRunning) noexcept
{
    while (programIsRunning) {
        if (!this->mutex.try_lock()) {
            // Avoid accessing a null mutex when destroying an object
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        const QString& licensePath = this->settings["general.dirs.license-filePath"];
        QFile file(licensePath);

        auto needsUpdate = [this, &licensePath] {
            return (!QFileInfo::exists(licensePath) ||
            QFileInfo(licensePath).lastModified() != this->licenseFileLastModified);
        };

        if (needsUpdate()) {
            updateLicenseFile();
        }

        this->mutex.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void License::updateLicenseFile() noexcept
{
    const QString& licensePath = this->settings["general.dirs.license-filePath"];

    if (!QFileInfo::exists(licensePath)) {
        QString info = "The product is FOR EVALUATION USE ONLY.";
        emit statusChangedInDetachedThread(false, info);
    }

    if (QFileInfo(licensePath).lastModified() != this->licenseFileLastModified) {
        try {
            emit statusChangedInDetachedThread(licenseIsActive(), licenseInfo());
            this->licenseFileLastModified = QFileInfo(licensePath).lastModified();

        } catch (const noLicenseFile&) {
            QString info = "The product is FOR EVALUATION USE ONLY.";
            emit statusChangedInDetachedThread(false, info);
        }
    }
}
