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
 * A TabsChangeListener is notified whenever the list of tabs changes.
 */
@FunctionalInterface
public interface TabsChangeListener {

    /**
     * Called after a change has been made to the list of tabs.
     *
     * @param change an object representing the change that was done
     */
    void onChange(Change change);

    /**
     * Represents a report of changes done to the list of tabs.
     * The change type in specified by the {@link Change.ChangeType}.
     */
    final class Change {

        /**
         * Enumeration indication the type of change that occurred in the list of tabs.
         */
        public enum ChangeType { TAB_ADDED, TAB_REMOVED }

        private final ChangeType changeType;
        private final int tabIndex;

        /**
         * Creates a new {@link Change} with the specified change type
         * and the tab index where the change occurred.
         *
         * @param changeType a type of change that occurred in the list of tabs
         * @param tabIndex a tab index where the change occurred
         *
         * @apiNote Suitable for the TAB_ADDED and the TAB_REMOVED change types.
         */
        public Change(ChangeType changeType, int tabIndex) {
            this.changeType = changeType;
            this.tabIndex = tabIndex;
        }

        /**
         * @return the type of change that occurred in the list of tabs
         */
        public ChangeType getChangeType() {
            return changeType;
        }

        /**
         * @return the tab index where the change occurred
         */
        public int getTabIndex() {
            return tabIndex;
        }
    }
}
