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

import javafx.beans.value.ChangeListener;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ToolBar;
import javafx.scene.control.Tooltip;
import javafx.scene.control.Separator;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.text.Text;

import java.net.URL;
import java.util.List;

/**
 * Horizontal bar containing icons used to select the application's most frequently used tools.
 * <p>
 * Contains the following items:
 * <li>Open New Tab button
 * <li>Open File button
 * <li>Save File button
 * <li>Run Conversion button
 * <li>Source modes choice box
 * <li>Target modes choice box
 */
class MainToolBar extends ToolBar {
    private final Button newTabButton = new Button();
    private final Button openFileButton = new Button();
    private final Button saveFileButton = new Button();
    private final Button runButton = new Button();
    private final ChoiceBox<String> sourceModesBox = new ChoiceBox<>();
    private final ChoiceBox<String> targetModesBox = new ChoiceBox<>();

    /**
     * Constructs a new MainToolBar.
     *
     * @throws IllegalStateException if any of the toolbar icons
     * were not found in application resources
     */
    public MainToolBar() {
        ignoreKeyEvents();
        setUpNewTabButton();
        setUpOpenFileButton();
        setUpSaveFileButton();
        setUpRunButton();
        setUpModeBoxes();
        setUpLayout();
    }

    private void ignoreKeyEvents() {
        addEventFilter(KeyEvent.ANY, keyEvent -> {
            if (keyEvent.getCode() == KeyCode.TAB
                    || keyEvent.getCode() == KeyCode.RIGHT
                    || keyEvent.getCode() == KeyCode.LEFT
                    || keyEvent.getCode() == KeyCode.UP
                    || keyEvent.getCode() == KeyCode.DOWN) {
                keyEvent.consume();
            }
        });
    }

    private void setUpNewTabButton() {
        String newTabIconUrl = loadNewTabIcon().toExternalForm();
        ImageView newTabImg = new ImageView(new Image(newTabIconUrl));
        newTabImg.setFitHeight(17);
        newTabImg.setFitWidth(17);
        newTabButton.setGraphic(newTabImg);
        newTabButton.setTooltip(new Tooltip("New tab"));
    }

    private URL loadNewTabIcon() {
        URL newTabIconUrl = getClass().getResource("/icons/open-tab.png");
        if (newTabIconUrl == null) {
            String errorMsg = "File not found in application resources: icons/open-tab.png";
            throw new IllegalStateException(errorMsg);
        }

        return newTabIconUrl;
    }

    private void setUpOpenFileButton() {
        String openFileIconUrl = loadOpenFileIcon().toExternalForm();
        ImageView openFileImg = new ImageView(new Image(openFileIconUrl));
        openFileImg.setFitHeight(17);
        openFileImg.setFitWidth(17);
        openFileButton.setGraphic(openFileImg);
        openFileButton.setTooltip(new Tooltip("Open file"));
    }

    private URL loadOpenFileIcon() {
        URL openFileIconUrl = getClass().getResource("/icons/open-file.png");
        if (openFileIconUrl == null) {
            String errorMsg = "File not found in application resources: icons/open-file.png";
            throw new IllegalStateException(errorMsg);
        }

        return openFileIconUrl;
    }

    private void setUpSaveFileButton() {
        String saveFileIconUrl = loadSaveFileIcon().toExternalForm();
        ImageView saveFileImg = new ImageView(new Image(saveFileIconUrl));
        saveFileImg.setFitHeight(17);
        saveFileImg.setFitWidth(17);
        saveFileButton.setGraphic(saveFileImg);
        saveFileButton.setTooltip(new Tooltip("Save file"));
    }

    private URL loadSaveFileIcon() {
        URL saveFileIconUrl = getClass().getResource("/icons/save-file.png");
        if (saveFileIconUrl == null) {
            String errorMsg = "File not found in application resources: icons/save-file.png";
            throw new IllegalStateException(errorMsg);
        }

        return saveFileIconUrl;
    }

    private void setUpRunButton() {
        String runButtonIconUrl = loadRunButtonIcon().toExternalForm();
        ImageView runImg = new ImageView(new Image(runButtonIconUrl));
        runImg.setFitHeight(17);
        runImg.setFitWidth(17);
        runButton.setGraphic(runImg);
        runButton.setTooltip(new Tooltip("Run conversion"));
    }

    private URL loadRunButtonIcon() {
        URL runImgUrl = getClass().getResource("/icons/run.png");
        if (runImgUrl == null) {
            String errorMsg = "File not found in application resources: icons/run.png";
            throw new IllegalStateException(errorMsg);
        }

        return runImgUrl;
    }

    private void setUpModeBoxes() {
        sourceModesBox.setTooltip(new Tooltip("Source conversion mode"));
        targetModesBox.setTooltip(new Tooltip("Target conversion mode"));
    }

    private void setUpLayout() {
        getItems().add(newTabButton);
        getItems().add(new Separator());
        getItems().addAll(openFileButton, saveFileButton);
        getItems().add(new Separator());
        getItems().add(runButton);
        getItems().add(new Separator());
        getItems().addAll(new Text(" Source:  "), sourceModesBox,
                new Text(" Target:  "), targetModesBox);
    }

    /**
     * @return currently selected source conversion mode
     *
     * @throws IllegalStateException if none of the source modes is currently selected
     */
    public String getSourceMode() {
        String currMode = sourceModesBox.getSelectionModel().getSelectedItem();
        if (currMode == null) {
            throw new IllegalStateException("No source mode selected");
        }

        return currMode;
    }

    /**
     * @return currently selected target conversion mode
     *
     * @throws IllegalStateException if none of the target modes is currently selected
     */
    public String getTargetMode() {
        String currMode = targetModesBox.getSelectionModel().getSelectedItem();
        if (currMode == null) {
            throw new IllegalStateException("No target mode selected");
        }

        return currMode;
    }

    /**
     * Appends all the elements from the list to the source modes check box.
     * <p>
     * Selects the first mode from the list as current.
     *
     * @param modes source modes to set
     *
     * @throws IllegalArgumentException if the list of source modes is empty
     */
    public void setSourceModes(List<String> modes) {
        if (modes.isEmpty()) {
            throw new IllegalArgumentException("List of source modes is empty");
        }

        sourceModesBox.getItems().addAll(modes);
        sourceModesBox.setValue(sourceModesBox.getItems().get(0));
    }

    /**
     * Appends all the elements from the list to the target modes check box.
     * <p>
     * Selects the first mode from the list as current.
     *
     * @param modes target modes to set
     *
     * @throws IllegalArgumentException if the list of target modes is empty
     */
    public void setTargetModes(List<String> modes) {
        if (modes.isEmpty()) {
            throw new IllegalArgumentException("List of target modes is empty");
        }

        targetModesBox.getItems().addAll(modes);
        targetModesBox.setValue(targetModesBox.getItems().get(0));
    }

    /**
     * Selects the specified source conversion mode as current.
     *
     * @param mode a source mode to select
     *
     * @throws IllegalArgumentException if such a mode does not exist
     */
    public void selectSourceMode(String mode) {
        if (!sourceModesBox.getItems().contains(mode)) {
            throw new IllegalArgumentException("Such a mode does not exist: " + mode);
        }

        sourceModesBox.getSelectionModel().select(mode);
    }

    /**
     * Selects the specified target conversion mode as current.
     *
     * @param mode a target mode to select
     *
     * @throws IllegalArgumentException if such a mode does not exist
     */
    public void selectTargetMode(String mode) {
        if (!targetModesBox.getItems().contains(mode)) {
            throw new IllegalArgumentException("Such a mode does not exist: " + mode);
        }

        targetModesBox.getSelectionModel().select(mode);
    }

    /**
     * Adds a listener which will be notified when the source conversion mode changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addSourceModeListener(ChangeListener<String> listener) {
       sourceModesBox.getSelectionModel().selectedItemProperty().addListener(listener);
    }

    /**
     * Adds a listener which will be notified when the target conversion mode changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addTargetModeListener(ChangeListener<String> listener) {
        targetModesBox.getSelectionModel().selectedItemProperty().addListener(listener);
    }

    /**
     * Adds a listener which will be notified when the focus changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addFocusListener(ChangeListener<Boolean> listener) {
        newTabButton.focusedProperty().addListener(listener);
        openFileButton.focusedProperty().addListener(listener);
        saveFileButton.focusedProperty().addListener(listener);
        runButton.focusedProperty().addListener(listener);
        sourceModesBox.focusedProperty().addListener(listener);
        targetModesBox.focusedProperty().addListener(listener);
    }

    /**
     * Sets the action which is invoked when the New Tab button is clicked.
     *
     * @param action the action to register
     */
    public void setOnNewTabAction(EventHandler<ActionEvent> action) {
        newTabButton.setOnAction(action);
    }

    /**
     * Sets the action which is invoked when the Open File button is clicked.
     *
     * @param action the action to register
     */
    public void setOnOpenFileAction(EventHandler<ActionEvent> action) {
        openFileButton.setOnAction(action);
    }

    /**
     * Sets the action which is invoked when the Save File button is clicked.
     *
     * @param action the action to register
     */
    public void setOnSaveFileAction(EventHandler<ActionEvent> action) {
        saveFileButton.setOnAction(action);
    }

    /**
     * Sets the action which is invoked when the Run button is clicked.
     *
     * @param action the action to register
     */
    public void setOnRunAction(EventHandler<ActionEvent> action) {
        runButton.setOnAction(action);
    }
}
