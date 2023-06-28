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

package com.sqlines.studio.view.settings.event;

import javafx.event.Event;
import javafx.event.EventType;

/**
 * An event representing a click on a change license button.
 */
public class ChangeLicenseEvent extends Event {
    private static final EventType<ChangeLicenseEvent> CLICKED = new EventType<>(ANY, "CLICKED");
    private final String regName;
    private final String regNumber;

    /**
     * Constructs a new ChangeLicenseEvent with the specified
     * registration name and registration number.
     * <p>
     * The source and target of the event is set to {@link Event#NULL_SOURCE_TARGET}.
     *
     * @param regName registration name entered
     * @param regNumber registration number entered
     */
    public ChangeLicenseEvent(String regName, String regNumber) {
        super(CLICKED);
        this.regName = regName;
        this.regNumber = regNumber;
    }

    /**
     * @return user-entered registration name
     */
    public String getRegName() {
        return regName;
    }

    /**
     * @return user-entered registration number
     */
    public String getRegNumber() {
        return regNumber;
    }
}
