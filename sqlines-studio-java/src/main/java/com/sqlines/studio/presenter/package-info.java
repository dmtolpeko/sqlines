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
 * The presenter acts upon the model and the view. It retrieves data from the model,
 * and formats it for display in the view.
 *
 * <p>
 * Classes:
 * <p>
 * {@link com.sqlines.studio.presenter.MainWindowPresenter} -
 * responds to user actions in the main window.
 * Retrieves data from the model, and displays it in the main window.
 * <p>
 *
 * {@link com.sqlines.studio.presenter.SettingsPresenter} -
 * responds to user actions in the settings window.
 * Retrieves data from the model, and displays it in the settings window.
 */
package com.sqlines.studio.presenter;
