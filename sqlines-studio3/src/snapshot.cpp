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

#include "snapshot.hpp"

using namespace model;

QString Snapshot::operator[](const QString& key) const noexcept
{
    const auto& item = this->state.find(key);
    if (item != this->state.cend()) {
        return (*item).second;
    } else {
        return {};
    }
}

QString& Snapshot::operator[](const QString& key) noexcept
{
    return this->state[key];
}

void Snapshot::clear() noexcept
{
    this->state.clear();
}
