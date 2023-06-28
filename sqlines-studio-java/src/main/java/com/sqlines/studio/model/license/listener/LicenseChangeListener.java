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

package com.sqlines.studio.model.license.listener;

/**
 * A LicenseChangeListener is notified when license status changes.
 */
@FunctionalInterface
public interface LicenseChangeListener {

    /**
     * Called when license status changes.
     *
     * @param isActive true if license is active, false otherwise
     */
    void changed(boolean isActive);
}
