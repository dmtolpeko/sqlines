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

package com.sqlines.studio.view.settings;

import com.sqlines.studio.view.BaseView;
import com.sqlines.studio.view.AbstractWindow;
import com.sqlines.studio.view.settings.event.ChangeLicenseEvent;

import javafx.beans.value.ChangeListener;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;

import java.util.List;
import java.util.Optional;

/**
 * Interface through with the presenter / controller can interact with the settings window UI.
 * Provides methods for work with the settings window.
 *
 * @see ChangeLicenseEvent
 */
public interface SettingsWindowView extends BaseView {

    /**
     * Shows a window prompting the user to choose new working directory.
     *
     * @return a selected directory path or {@link Optional#empty()}
     * if the user did not select a directory
     */
    Optional<String> choseDirectoryToAdd();

    /**
     * Adds the specified working directory to the directories choice box.
     *
     * @param dir directory to add
     *
     * @throws IllegalArgumentException if dir is empty
     */
    void addDirectory(String dir);

    /**
     * Sets working directories displayed in the working directories choice box.
     * <p>
     * Selects the first directory from the list as current.
     *
     * @param dirs list of directories to set
     *
     * @throws IllegalArgumentException if the list of directories is empty
     */
    void setWorkingDirectories(List<String> dirs);

    /**
     * Sets themes displayed in the themes choice box.
     * <p>
     * Selects the first theme from the list as current.
     *
     * @param themes list of themes to set
     *
     * @throws IllegalArgumentException if the list of themes is empty
     */
    void setThemes(List<String> themes);

    /**
     * Selects the specified working directory as current in the working directories choice box.
     *
     * @param dir directory to select
     *
     * @throws IllegalArgumentException if such a directory does not exist
     */
    void selectDirectory(String dir);

    /**
     * Selects the specified theme as current in the themes choice box.
     *
     * @param theme theme to select
     */
    void selectTheme(AbstractWindow.Theme theme);

    /**
     * Sets the text that will be displayed in the license tab.
     *
     * @param info license info to show
     */
    void setLicenseInfo(String info);

    /**
     * Defines the selection state of the Save Last Session button.
     *
     * @param isSelected makes the menu item selected if true, unselected otherwise.
     */
    void setSaveSessionSelected(boolean isSelected);

    /**
     * Defines the selection state of the Status Bar button.
     *
     * @param isSelected makes the menu item selected if true, unselected otherwise
     */
    void setStatusBarSelected(boolean isSelected);

    /**
     * Defines the selection state of the Always Show Target Field button.
     *
     * @param isSelected makes the menu item selected if true, unselected otherwise
     */
    void setTargetFieldSelected(boolean isSelected);

    /**
     * Defines the selection state of the Wrap Lines To Editor Width button.
     *
     * @param isSelected makes the menu item selected if true, unselected otherwise
     */
    void setWrappingSelected(boolean isSelected);

    /**
     * Defines the selection state of the Highlighter button.
     *
     * @param isSelected makes the menu item selected if true, unselected otherwise
     */
    void setHighlighterSelected(boolean isSelected);

    /**
     * Defines the selection state of the Line Numbers button.
     *
     * @param isSelected makes the menu item selected if true, unselected otherwise
     */
    void setLineNumbersSelected(boolean isSelected);

    /**
     * Adds a listener which will be notified when the currently selected theme
     * in the themes choice box changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    void addThemeChangeListener(ChangeListener<String> listener);

    /**
     * Adds a listener which will be notified when the currently selected working directory
     * in the working directories choice box changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    void addDirChangeListener(ChangeListener<String> listener);

    /**
     * Sets the action which is invoked when the Add New Directory button is clicked.
     *
     * @param action the action to register
     */
    void setOnAddDirAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Save Last Session button is clicked.
     *
     * @param action the action to register
     */
    void setOnSaveSessionAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Set Defaults button is clicked.
     *
     * @param action the action to register
     */
    void setOnSetDefaultsAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Status Bar button is clicked.
     *
     * @param action the action to register
     */
    void setOnStatusBarAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Always Show Target Field button is clicked.
     *
     * @param action the action to register
     */
    void setOnTargetFieldAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Wrapping button is clicked.
     *
     * @param action the action to register
     */
    void setOnWrappingAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Highlighter button is clicked.
     *
     * @param action the action to register
     */
    void setOnHighlighterAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Line Numbers button is clicked.
     *
     * @param action the action to register
     */
    void setOnLineNumbersAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Change License button is clicked.
     *
     * @param action the action to register
     */
    void setOnChangeLicenseAction(EventHandler<ChangeLicenseEvent> action);
}
