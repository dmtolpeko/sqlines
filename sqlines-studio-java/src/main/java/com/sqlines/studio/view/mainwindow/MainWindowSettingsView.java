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

package com.sqlines.studio.view.mainwindow;

import com.sqlines.studio.view.BaseView;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;

/**
 * Interface through with the presenter / controller can interact with the main window UI.
 * Provides methods to configure the main window.
 */
public interface MainWindowSettingsView extends BaseView {

    /**
     * An enumeration denoting the policy to be used by a MainWindow
     * in deciding whether to show a status bar.
     */
    enum StatusBarPolicy { SHOW, DO_NOT_SHOW }

    /**
     * An enumeration denoting the policy to be used by a MainWindow
     * in deciding whether to show a target text input field.
     */
    enum TargetFieldPolicy { ALWAYS, AS_NEEDED }

    /**
     * An enumeration denoting the policy to be used by a text input field
     * in deciding whether to wrap lines.
     */
    enum WrappingPolicy { WRAP_LINES, NO_WRAP }

    /**
     * An enumeration denoting the policy to be used by a text input field
     * in deciding whether to highlight text.
     */
    enum HighlighterPolicy { HIGHLIGHT, DO_NOT_HIGHLIGHT }

    /**
     * An enumeration denoting the policy to be used by a text input field
     * in deciding whether to show line number area.
     */
    enum LineNumbersPolicy { SHOW, DO_NOT_SHOW }

    /**
     * Specifies whether main window should be a full-screen, undecorated window.
     *
     * @param isMaximized makes the window maximized if true, minimized otherwise
     */
    void setFullScreen(boolean isMaximized);

    /**
     * Sets the width of the main window.
     *
     * @param width width to set
     */
    void setWidth(double width);

    /**
     * Sets the height of the main window.
     *
     * @param height height to set
     */
    void setHeight(double height);

    /**
     * Sets the horizontal location of the main window on the screen.
     *
     * @param x x position to set
     */
    void setX(double x);

    /**
     * Sets the vertical location of the main window on the screen.
     *
     * @param y y position to set
     */
    void setY(double y);

    /**
     * Sets the {@link StatusBarPolicy}.
     * <p>
     * The default value is SHOW.
     *
     * @param policy status bar policy to set
     */
    void setStatusBarPolicy(StatusBarPolicy policy);

    /**
     * Sets the {@link TargetFieldPolicy}.
     * <p>
     * The default value is AS_NEEDED.
     *
     * @param policy target field policy to set
     */
    void setTargetFieldPolicy(TargetFieldPolicy policy);

    /**
     * Sets the {@link WrappingPolicy} of the text input fields.
     * <p>
     * The default value is NO_WRAP.
     *
     * @param policy wrapping policy to set
     */
    void setWrappingPolicy(WrappingPolicy policy);

    /**
     * Sets the {@link HighlighterPolicy} of the text input fields.
     * <p>
     * The default value is HIGHLIGHT.
     *
     * @param policy highlighter policy to set
     */
    void setHighlighterPolicy(HighlighterPolicy policy);

    /**
     * Sets the {@link LineNumbersPolicy} of the text input fields.
     * <p>
     * The default value is {@link LineNumbersPolicy#SHOW}.
     *
     * @param policy line numbers policy to set
     */
    void setLineNumbersPolicy(LineNumbersPolicy policy);

    /**
     * Sets the action which is invoked when
     * the Preferences menu item in the menu bar is clicked.
     *
     * @param action the action to register
     */
    void setOnPreferencesAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when
     * the Show Status Bar check menu item in the menu bar is clicked.
     *
     * @param action the action to register
     */
    void setOnStatusBarAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Always Show Target Field check menu item
     * in the menu bar is clicked.
     *
     * @param action the action to register
     */
    void setOnTargetFieldAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Wrap Lines To Editor Width check menu item
     * in the Editor menu in the menu bar is clicked.
     *
     * @param action the action to register
     */
    void setOnWrappingAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Highlighter check menu item
     * in the Editor menu in the menu bar is clicked.
     *
     * @param action the action to register
     */
    void setOnHighlighterAction(EventHandler<ActionEvent> action);

    /**
     * Sets the action which is invoked when the Line Numbers check menu item
     * in the Editor menu in the menu bar is clicked.
     *
     * @param action the action to register
     */
    void setOnLineNumbersAction(EventHandler<ActionEvent> action);
}
