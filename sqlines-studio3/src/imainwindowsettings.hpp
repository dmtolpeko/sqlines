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


#ifndef IMAIN_WINDOW_SETTINGS_HPP
#define IMAIN_WINDOW_SETTINGS_HPP

#include <QString>

namespace view {
    class IMainWindowSettings;
}

class view::IMainWindowSettings {
public:
    virtual ~IMainWindowSettings() = default;

    virtual int height() const noexcept = 0;
    virtual int width() const noexcept = 0;
    virtual int posX() const noexcept = 0;
    virtual int posY() const noexcept = 0;

    virtual void resize(int width, int height) noexcept = 0;
    virtual void move(int x, int y) noexcept = 0;

    virtual void changeTitle(const QString& title) noexcept = 0;
    virtual void changeNumberArea(bool isOn) noexcept = 0;
    virtual void changeHighlighting(bool isOn) noexcept = 0;
    virtual void changeWrapping(bool isOn) noexcept = 0;
};

#endif // IMAIN_WINDOW_SETTINGS_HPP
