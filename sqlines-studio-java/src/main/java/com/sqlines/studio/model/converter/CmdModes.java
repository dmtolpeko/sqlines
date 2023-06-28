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

package com.sqlines.studio.model.converter;

import java.util.Map;
import java.util.Objects;

/**
 * Command-line conversion modes.
 */
public class CmdModes {
    private final Map<String, String> cmdModes;

    public CmdModes(Map<String, String> cmdModes) {
        this.cmdModes = cmdModes;
    }

    public String get(String rawMode) {
        return cmdModes.get(rawMode);
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }

        if (other == null || getClass() != other.getClass()) {
            return false;
        }

        CmdModes modes = (CmdModes) other;
        return Objects.equals(cmdModes, modes.cmdModes);
    }

    @Override
    public int hashCode() {
        return Objects.hash(cmdModes);
    }

    @Override
    public String toString() {
        return getClass().getName() + "{" +
                "cmdModes=" + cmdModes +
                '}';
    }
}

