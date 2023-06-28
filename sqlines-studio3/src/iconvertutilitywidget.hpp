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

#ifndef ICONVERT_UTILITY_WIDGET_HPP
#define ICONVERT_UTILITY_WIDGET_HPP

#include <QString>

namespace view {
    class IConvertUtilityWidget;
}

class view::IConvertUtilityWidget {
public:
    virtual ~IConvertUtilityWidget() = default;

    virtual void showFilePath(const QString& path) noexcept = 0;
    virtual void showLog(const QString& log) noexcept = 0;

    virtual QString currentSourceMode() const noexcept = 0;
    virtual QString currentTargetMode() const noexcept = 0;

    virtual QString choseFileToOpen(const QString& currDir) noexcept = 0;

public: // Signals
    virtual void convertPressed() = 0;
    virtual void selectFilePressed() = 0;

    virtual void sourceModeSelected(const QString& sourceMode) = 0;
    virtual void targetModeSelected(const QString& targetMode) = 0;
};

#endif // ICONVERT_UTILITY_WIDGET_HPP
