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
 * An event representing a click on a recent file in the Open Recent menu in the menu bar.
 */
public class RecentFileEvent extends Event {
    private static final EventType<RecentFileEvent> CLICKED = new EventType<>(ANY, "CLICKED");
    private final String filePath;

    /**
     * Constructs a new RecentFileEvent with the specified file path.
     * <p>
     * The source and target of the event is set to {@link Event#NULL_SOURCE_TARGET}.
     *
     * @param filePath path to the recent file
     */
    public RecentFileEvent(String filePath) {
        super(CLICKED);
        this.filePath = filePath;
    }

    /**
     * @return path to the recent file
     */
    public String getFilePath() {
        return filePath;
    }
}
