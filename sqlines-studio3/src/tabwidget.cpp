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

#include <algorithm>
#include <vector>
#include <QFileInfo>
#include <QTabBar>

#include "tabwidget.hpp"

ui::TabWidget::TabWidget(QWidget* parent) noexcept
 :  QTabWidget(parent)
{
    setMovable(true);
    setTabsClosable(true);
}

void ui::TabWidget::addTab(ui::CentralWidget& widget) noexcept
{
    auto setTabTitle = [this]()->QString {
        std::vector<int> tabNumbers;
        for (auto i = 0; i < count(); i++) {
            QString title = tabText(i);
            title.erase(title.cbegin(), title.cbegin() + 4); // Delete "Tab "
            bool ok;
            int number = title.toInt(&ok);
            if (ok) { // The tab had a standard title "Tab i"
                tabNumbers.push_back(number);
            }
        }
        std::sort(tabNumbers.begin(), tabNumbers.end());
        
        if (tabNumbers.empty()) {
            return "Tab 1";
        }
        auto i = 0;
        for (; i < tabNumbers.size(); i++) {
            // The tab numbers must be ordered
            if (tabNumbers[i] != i + 1) {
                break;
            }
        }
        return "Tab " + QString::number(i + 1);
    };

    insertTab(count(), &widget, setTabTitle());
    setCurrentIndex(count() - 1);

    widget.focusOn(CentralWidget::FieldInFocus::SourceField);
}

ui::CentralWidget& ui::TabWidget::getWidget(int tabIndex)
{
    if (tabIndex > count() - 1) {
        throw noTab();
    }
    return *dynamic_cast<CentralWidget*>(widget(tabIndex));
}

const ui::CentralWidget& ui::TabWidget::currentWidget() const
{
    if (count() == 0) {
        throw noTab();
    }
    return *dynamic_cast<CentralWidget*>(widget(currentIndex()));
}

ui::CentralWidget& ui::TabWidget::currentWidget()
{
    if (count() == 0) {
        throw noTab();
    }
    return *dynamic_cast<CentralWidget*>(widget(currentIndex()));
}

QString ui::TabWidget::tabTitle(int tabIndex) const noexcept
{
    return tabText(tabIndex);
}

void ui::TabWidget::setTabTitle(int tabIndex, const QString& tabTitle) noexcept
{
    setTabText(tabIndex, tabTitle);
}

void ui::TabWidget::nextTab() noexcept
{
    if (currentIndex() != count()) {
        setCurrentIndex(currentIndex() + 1);
    }
}

void ui::TabWidget::prevTab() noexcept
{
    if (currentIndex() - 1 != -1) {
        setCurrentIndex(currentIndex() - 1);
    }
}
