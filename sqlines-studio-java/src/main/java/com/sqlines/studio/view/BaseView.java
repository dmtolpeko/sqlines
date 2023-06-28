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

package com.sqlines.studio.view;

import javafx.event.EventHandler;
import javafx.stage.WindowEvent;

/**
 * Interface through with the presenter / controller can interact with UI.
 * Provides methods for basic work with UI.
 */
public interface BaseView {

    /**
     * Shows window.
     */
    void show();

    /**
     * Whether this window is showing (that is, open on the user's system).
     * The window might be "showing", yet the user might not be able to see it due to the window being
     * rendered behind another window or due to the Window being positioned off the monitor.
     *
     * @return true if window is showed, false otherwise
     */
    boolean isShowing();

    /**
     * Brings the window to the foreground.
     * If the window is already in the foreground there is no visible difference.
     */
    void toFront();

    /**
     * Defines the title of window.
     *
     * @param title title to set
     *
     * @throws UnsupportedOperationException if the requested operation is not supported
     */
    default void setWindowTitle(String title) {
        throw new UnsupportedOperationException();
    }

    /**
     * Shows an error message.
     *
     * @param cause title of the error message window
     * @param errorMsg error information
     *
     * @throws IllegalStateException if the constructor of the error window
     * did not find the required resource
     * @throws UnsupportedOperationException if the requested operation is not supported
     */
    default void showError(String cause, String errorMsg) {
        throw new UnsupportedOperationException();
    }

    /**
     * Sets the action which is invoked when the window requests closure.
     *
     * @param action the action to register
     *
     * @throws UnsupportedOperationException if the requested operation is not supported
     */
    default void setOnCloseAction(EventHandler<WindowEvent> action) {
        throw new UnsupportedOperationException();
    }
}
