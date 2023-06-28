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

/**
 * Model-view-presenter architecture.
 * <p>
 * The view is a passive interface that displays data and routes user events
 * to the presenter to act upon that data.
 *
 * <p>
 * Interfaces:
 * <p>
 * {@link com.sqlines.studio.view.BaseView} -
 * interface through with the presenter / controller can interact with UI.
 * Provides methods for basic work with UI.
 *
 * <p>
 * Classes:
 * <p>
 * {@link com.sqlines.studio.view.AbstractWindow} -
 * the base class of all UI windows. Allows you to set light and
 * dark interface stylesheets and switch between them.
 * <p>
 *
 * {@link com.sqlines.studio.view.ErrorWindow} -
 * provides a message for informing the user about an occurred error.
 *
 * <p>
 * Packages:
 * <p>
 * {@link com.sqlines.studio.view.mainwindow} -
 * implementation of the main application window.
 * <p>
 *
 * {@link com.sqlines.studio.view.settings} -
 * implementation of the settings window.
 */
package com.sqlines.studio.view;
