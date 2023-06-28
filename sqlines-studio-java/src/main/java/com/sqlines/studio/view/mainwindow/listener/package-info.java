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
 * Contains interfaces that receive notifications of data changes
 * in the main application window.
 * <p>
 *
 * {@link com.sqlines.studio.view.mainwindow.listener.ModeChangeListener} -
 * is notified whenever the conversion mode in any tab changes.
 * <p>
 *
 * {@link com.sqlines.studio.view.mainwindow.listener.TextChangeListener} -
 * is notified whenever the text in the text input field in any tab changes.
 *
 * {@link com.sqlines.studio.view.mainwindow.listener.TabTitleChangeListener} -
 * is notified whenever the tab title in any tab changes.
 *
 * {@link com.sqlines.studio.view.mainwindow.listener.FocusChangeListener} -
 * is notified whenever the currently focused text input field in any tab changes.
 */
package com.sqlines.studio.view.mainwindow.listener;
