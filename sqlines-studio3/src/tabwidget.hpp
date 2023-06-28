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

#ifndef TAB_WIDGET_HPP
#define TAB_WIDGET_HPP

#include <exception>
#include <QTabWidget>
#include <QString>

#include "centralwidget.hpp"

namespace ui {
    class TabWidget;
}

class ui::TabWidget : public QTabWidget {
public:
    class noTab : public std::exception {};

public:
    explicit TabWidget(QWidget* parent = nullptr) noexcept;
    TabWidget(const ui::TabWidget& other) = delete;
    ~TabWidget() override = default;
    
    void addTab(ui::CentralWidget& widget) noexcept;

    ui::CentralWidget& getWidget(int tabIndex);
    const ui::CentralWidget& currentWidget() const;
    ui::CentralWidget& currentWidget();

    QString tabTitle(int tabIndex) const noexcept;
    void setTabTitle(int tabIndex, const QString& tabTitle) noexcept;

public slots:
    void nextTab() noexcept;
    void prevTab() noexcept;
};

#endif // TAB_WIDGET_HPP
