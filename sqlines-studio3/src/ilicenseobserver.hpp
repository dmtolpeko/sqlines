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

#ifndef ILICENSE_OBSERVER_HPP
#define ILICENSE_OBSERVER_HPP

#include <QString>

namespace model {
    class ILicenseObserver;
}

class model::ILicenseObserver {
public:
    virtual ~ILicenseObserver() = default;

    virtual void licenseStatusChanged(bool isActive,
                                      const QString& info) noexcept = 0;
};

#endif // ILICENSE_OBSERVER_HPP
