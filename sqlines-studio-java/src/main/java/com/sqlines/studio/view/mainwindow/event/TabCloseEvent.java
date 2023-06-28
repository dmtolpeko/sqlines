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

package com.sqlines.studio.view.mainwindow.event;

import javafx.event.Event;
import javafx.event.EventType;

/**
 * An event representing tab close request.
 */
public class TabCloseEvent extends Event {
    private static final EventType<TabCloseEvent> CLOSE = new EventType<>(ANY, "CLOSE");
    private final int tabIndex;

    /**
     * Constructs a new TabCloseEvent with the index of the tab requesting closure.
     * <p>
     * The source and target of the event is set to {@link Event#NULL_SOURCE_TARGET}.
     *
     * @param tabIndex index of the tab requesting closure
     */
    public TabCloseEvent(int tabIndex) {
        super(CLOSE);
        this.tabIndex = tabIndex;
    }

    /**
     * @return the index of the tab requesting closure
     */
    public int getTabIndex () {
        return tabIndex;
    }
}
