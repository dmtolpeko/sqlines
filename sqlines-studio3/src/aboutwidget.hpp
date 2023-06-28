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

#ifndef ABOUT_WIDGET_HPP
#define ABOUT_WIDGET_HPP

#include <QDialog>
#include <QString>

namespace ui {
    class AboutWidget;
}

class ui::AboutWidget : public QDialog {
public:
    explicit AboutWidget(QWidget* parent = nullptr) noexcept;
    AboutWidget(const ui::AboutWidget& other) = delete;
    ~AboutWidget() override = default;

private:
    void paint() noexcept;
};

#endif // ABOUT_WIDGET_HPP
