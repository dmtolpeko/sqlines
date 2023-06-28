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

import javafx.collections.ObservableList;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.scene.Parent;

/**
 * The base class of all UI windows.
 * Allows you to set light and dark interface stylesheets and switch between them.
 *
 * @apiNote Use {@link AbstractWindow#setRoot(Parent)} to set main layout before using AbstractWindow methods.
 */
public abstract class AbstractWindow extends Stage {

    /**
     * UI theme.
     */
    public enum Theme { LIGHT, DARK, NONE }

    private Scene scene;
    private Theme theme = Theme.NONE;
    private String lightStylesheets;
    private String darkStylesheets;

    /**
     * @return current UI theme
     */
    public final Theme getTheme() {
        return theme;
    }

    /**
     * @return path to a file with light stylesheets to get
     *
     * @throws IllegalStateException if light stylesheets have not been set
     */
    public final String getLightStylesheets() {
        if (lightStylesheets == null) {
            throw new IllegalStateException("Light styles not set");
        }

        return lightStylesheets;
    }

    /**
     * @return path to a file with dark stylesheets to get
     *
     * @throws IllegalStateException if dark stylesheets have not been set
     */
    public final String getDarkStylesheets() {
        if (darkStylesheets == null) {
            throw new IllegalStateException("Dark styles not set");
        }

        return darkStylesheets;
    }

    /**
     * Sets the light design of the UI.
     * <p>
     * For additional information about using CSS with the scene graph, see the CSS
     * <a href=https://docs.oracle.com/javafx/2/api/javafx/scene/doc-files/cssref.html">Reference Guide</a>.
     *
     * @param filePath path to a file with stylesheets to set
     */
    public final void setLightStylesheets(String filePath) {
        lightStylesheets = filePath;
    }

    /**
     * Sets the dark design of the UI.
     * <p>
     * For additional information about using CSS with the scene graph, see the CSS
     * <a href=https://docs.oracle.com/javafx/2/api/javafx/scene/doc-files/cssref.html">Reference Guide</a>.
     *
     * @param filePath path to a file with stylesheets to set
     */
    public final void setDarkStylesheets(String filePath) {
        darkStylesheets = filePath;
    }

    /**
     * Sets the current theme of the UI.
     *
     * @param theme theme to set
     *
     * @throws IllegalStateException if stylesheets or root node have not been set
     */
    public final void setTheme(Theme theme) {
        if (scene == null) {
            throw new IllegalStateException("Root node not set");
        }

        if (theme == Theme.LIGHT) {
            setLightTheme();
        } else if (theme == Theme.DARK) {
            setDarkTheme();
        } else if (theme == Theme.NONE) {
            clearTheme();
        }
    }

    private void setLightTheme() {
        ObservableList<String> stylesheets = scene.getStylesheets();
        if (lightStylesheets == null) {
            throw new IllegalStateException("Light styles not set");
        }

        stylesheets.clear();
        stylesheets.add(lightStylesheets);
        this.theme = Theme.LIGHT;
    }

    private void setDarkTheme() {
        ObservableList<String> stylesheets = scene.getStylesheets();
        if (darkStylesheets == null) {
            throw new IllegalStateException("Dark styles not set");
        }

        stylesheets.clear();
        stylesheets.add(darkStylesheets);
        this.theme = Theme.DARK;
    }

    private void clearTheme() {
        ObservableList<String> stylesheets = scene.getStylesheets();
        stylesheets.clear();
    }

    /**
     * Defines the root node of the scene graph.
     *
     * @param node root node to set
     */
    protected final void setRoot(Parent node) {
        scene = new Scene(node);
        scene.setRoot(node);
        setScene(scene);
    }
}
