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

#ifndef ICONVERTER_OBSERVER_HPP
#define ICONVERTER_OBSERVER_HPP

#include <exception>
#include <QString>

namespace model {
    class IConverterObserver;
}

class model::IConverterObserver {
public:
    virtual ~IConverterObserver() = default;

    virtual void conversionStarted(int tabIndex) noexcept = 0;
    virtual void conversionEnded(int tabIndex) noexcept = 0;
    virtual void conversionCrashed(int tabIndex,
                                   const std::exception_ptr& exception) noexcept = 0;

    virtual void remodeled() noexcept = 0;
    virtual void tabAdded() noexcept = 0;

    virtual void tabTitleUpdated(int tabIndex,
                                 const QString& tabTitle) noexcept = 0;
    virtual void sourceModeUpdated(int tabIndex,
                                   const QString& sourceMode) noexcept = 0;
    virtual void targetModeUpdated(int tabIndex,
                                   const QString& targetMode) noexcept = 0;
    virtual void sourceDataUpdated(int tabIndex,
                                   const QString& sourceData) noexcept = 0;
    virtual void targetDataUpdated(int tabIndex,
                                   const QString& targetData) noexcept = 0;
    virtual void sourceFilePathUpdated(int tabIndex,
                                       const QString& filePath) noexcept = 0;
    virtual void targetFilePathUpdated(int tabIndex,
                                       const QString& filePath) noexcept = 0;
};

#endif // ICONVERTER_OBSERVER_HPP
