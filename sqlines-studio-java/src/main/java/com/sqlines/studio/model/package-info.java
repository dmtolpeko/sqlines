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
 * The model is an interface defining the data to be displayed or otherwise acted upon in the user interface.
 *
 * <p>
 * Classes:
 * <p>
 * {@link com.sqlines.studio.model.converter.ConverterImpl} - allows you to start conversion.
 * <p>
 *
 * {@link com.sqlines.studio.model.coreprocess.CoreProcessRunnerImp} - SQLines command-line program.
 * <p>
 *
 * {@link com.sqlines.studio.model.PropertiesLoader} -
 * loads the application properties into the standard {@link java.util.Properties} class.
 * <p>
 *
 * {@link com.sqlines.studio.model.ResourceLoader} - loads application resources.
 *
 * <p>
 * Packages:
 * <p>
 * {@link com.sqlines.studio.model.tabsdata} -
 * contains elements tha allow you to manage data of the opened tabs.
 * <p>
 *
 * {@link com.sqlines.studio.model.filehandler} -
 * contains elements tha allow you to work with files.
 *
 * {@link com.sqlines.studio.model.license} -
 * contains elements tha allow you to work with license.
 */
package com.sqlines.studio.model;
