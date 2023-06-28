/*
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

package com.sqlines.studio.model.tabsdata.listener;

/**
 * A TabTitleChangeListener is notified whenever the title of any tab changes.
 */
public interface TabTitleChangeListener {

    /**
     * Called when the tab title changes.
     *
     * @param newTitle updated tab title
     * @param tabIndex the index of the tab where the title was changed
     */
    void changed(String newTitle, int tabIndex);
}
