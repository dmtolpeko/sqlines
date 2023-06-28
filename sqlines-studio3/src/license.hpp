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

#ifndef LICENSE_HPP
#define LICENSE_HPP

#include <atomic>
#include <mutex>
#include <vector>
#include <exception>
#include <utility>
#include <QDateTime>
#include <QString>
#include <QObject>

#include "settings.hpp"
#include "ilicenseobserver.hpp"

namespace model {
    class License;
}

class model::License : public QObject {
    Q_OBJECT

public:
    class noLicenseFile : std::exception {};

public:
    License(const model::Settings& settings,
            const std::atomic<bool>& programIsRunning) noexcept;
    ~License() override;

    void addObserver(ILicenseObserver& observer) noexcept;

    QString info() const;
    bool isActive() const;
    bool exists() const noexcept;

    void changeLicense(const QString& regName, const QString& regNumber);

private:
    // Lock-free operations
    QString licenseInfo() const;
    bool licenseIsActive() const;

    // Log file for the core process
    QString createLogFile() const noexcept;

    // Tracking manual changes in the license file
    void monitorLicenseFile(const std::atomic<bool>& programIsRunning) noexcept;
    void updateLicenseFile() noexcept;

private: signals:
    // UI changes can only be made from the main thread
    void statusChangedInDetachedThread(bool isActive, const QString& info);

private:
    std::vector<std::reference_wrapper<ILicenseObserver>> observers;

    QDateTime licenseFileLastModified;
    const model::Settings& settings;

    mutable std::mutex mutex;
};

#endif // LICENSE_HPP
