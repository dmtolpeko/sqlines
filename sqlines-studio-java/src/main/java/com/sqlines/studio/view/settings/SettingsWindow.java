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

import com.sqlines.studio.view.ErrorWindow;
import com.sqlines.studio.view.AbstractWindow;
import com.sqlines.studio.view.settings.event.ChangeLicenseEvent;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.RadioButton;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.TextField;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;
import javafx.stage.DirectoryChooser;
import javafx.stage.StageStyle;

import java.io.File;
import java.util.List;
import java.util.Optional;

/**
 * The concrete setting window.
 */
public class SettingsWindow extends AbstractWindow implements SettingsWindowView {
    private EventHandler<ChangeLicenseEvent> licenseEventHandler;

    // General settings
    private final ChoiceBox<String> dirsBox = new ChoiceBox<>();
    private final Button addDirButton = new Button();
    private final Button setDefaultsButton = new Button();
    private final RadioButton saveSessionButton = new RadioButton();

    // Editor settings
    private final ChoiceBox<String> themesBox = new ChoiceBox<>();
    private final RadioButton statusBarButton = new RadioButton();
    private final RadioButton targetFieldButton = new RadioButton();
    private final RadioButton wrappingButton = new RadioButton();
    private final RadioButton highlighterButton = new RadioButton();
    private final RadioButton lineNumbersButton = new RadioButton();

    // License settings
    private final Text licenseInfo = new Text();
    private final TextField regNameField = new TextField();
    private final TextField regNumberField = new TextField();
    private final Button changeButton = new Button();

    public SettingsWindow() {
        setUpScene();
        setUpWindow();
    }

    private void setUpScene() {
        TabPane tabPane = new TabPane();
        setRoot(tabPane);
        setUpTabPane(tabPane);
    }

    private void setUpTabPane(TabPane tabPane) {
        tabPane.setTabClosingPolicy(TabPane.TabClosingPolicy.UNAVAILABLE);
        tabPane.getSelectionModel().selectedIndexProperty().addListener(this::tabIndexChanged);

        tabPane.getTabs().add(new Tab("General", makeGeneralTab()));
        tabPane.getTabs().add(new Tab("Appearance", makeAppearanceTab()));
        tabPane.getTabs().add(new Tab("License", makeLicenseTab()));
    }

    private void tabIndexChanged(ObservableValue<? extends Number> observable,
                                 Number oldIndex, Number newIndex) {
        int tabIndex = newIndex.intValue();
        if (tabIndex == 0) { // General settings tab
            setGeneralTabSize();
        } else if (tabIndex == 1) { // Editor settings tab
            setEditorTabSize();
        } else if (tabIndex == 2) { // License settings tab
            setLicenseTabSize();
        }
    }

    private void setGeneralTabSize() {
        setWidth(320);
        setHeight(210);
    }

    private void setEditorTabSize() {
        setWidth(320);
        setHeight(240);
    }

    private void setLicenseTabSize() {
        setWidth(320);
        setHeight(220);
    }

    private VBox makeGeneralTab() {
        setUpGeneralTabItems();
        return createGeneralTabLayout();
    }

    private void setUpGeneralTabItems() {
        addDirButton.setText("Add new");
        saveSessionButton.setText("Save last session");
        setDefaultsButton.setText("Set defaults");
    }

    private VBox createGeneralTabLayout() {
        GridPane topLayout = new GridPane();
        topLayout.setHgap(10);
        topLayout.setVgap(10);
        topLayout.add(new Text("Working directory:"), 0, 0);
        topLayout.add(dirsBox, 1, 0);
        topLayout.add(addDirButton, 1, 1);

        VBox mainLayout = new VBox(topLayout, saveSessionButton, setDefaultsButton);
        mainLayout.setPadding(new Insets(10, 15, 10, 15));
        mainLayout.setSpacing(10);

        return mainLayout;
    }

    private VBox makeAppearanceTab() {
        setUpAppearanceTabItems();
        return createAppearanceTabLayout();
    }

    private void setUpAppearanceTabItems() {
        statusBarButton.setText("Status bar");
        targetFieldButton.setText("Always show target field");
        wrappingButton.setText("Wrap lines to editor width");
        highlighterButton.setText("Highlighter");
        lineNumbersButton.setText("Line numbers");
    }

    private VBox createAppearanceTabLayout() {
        GridPane topLayout = new GridPane();
        topLayout.setVgap(10);
        topLayout.setHgap(10);
        topLayout.add(new Text("Theme:"), 0, 0);
        topLayout.add(themesBox, 1, 0);

        VBox mainLayout = new VBox(topLayout, statusBarButton, targetFieldButton,
                wrappingButton, highlighterButton, lineNumbersButton);
        mainLayout.setPadding(new Insets(10, 15, 10, 15));
        mainLayout.setSpacing(10);

        return mainLayout;
    }

    private VBox makeLicenseTab() {
        setUpLicenseTabItems();
        return createLicenseTabLayout();
    }

    private void setUpLicenseTabItems() {
        regNameField.setPromptText("Enter registration name");
        regNumberField.setPromptText("Enter registration number");
        changeButton.setText("Commit change");

        changeButton.setOnAction(event -> handleLicenceChangeEvent());
    }

    private void handleLicenceChangeEvent() {
        ChangeLicenseEvent clickedEvent = new ChangeLicenseEvent(
                regNameField.getText(), regNumberField.getText()
        );
        changeButton.fireEvent(clickedEvent);

        if (licenseEventHandler != null) {
            licenseEventHandler.handle(clickedEvent);
        }
    }

    private VBox createLicenseTabLayout() {
        VBox topLayout = new VBox(licenseInfo);
        topLayout.setPadding(new Insets(10, 0, 5, 0));

        VBox mainLayout = new VBox(topLayout, regNameField,
                regNumberField, changeButton);
        mainLayout.setPadding(new Insets(10, 15, 10, 15));
        mainLayout.setSpacing(10);

        return mainLayout;
    }

    private void setUpWindow() {
        initStyle(StageStyle.UTILITY);
        setTitle("Preferences");
        setWidth(320);
        setHeight(210);
        setResizable(false);
    }

    @Override
    public void showError(String cause, String errorMsg) {
        ErrorWindow errorWindow = new ErrorWindow(cause, errorMsg);
        setStylesheets(errorWindow);
        errorWindow.show();
    }

    private void setStylesheets(AbstractWindow window) {
        Theme theme = getTheme();
        if (theme == Theme.LIGHT) {
            window.setLightStylesheets(getLightStylesheets());
        } else if (theme == Theme.DARK) {
            window.setDarkStylesheets(getDarkStylesheets());
        }

        window.setTheme(theme);
    }

    @Override
    public Optional<String> choseDirectoryToAdd() {
        DirectoryChooser chooser = new DirectoryChooser();
        File dir = chooser.showDialog(this);
        return Optional.ofNullable(dir)
                .map(File::getAbsolutePath);
    }

    @Override
    public void addDirectory(String dir) {
        if (dir.isEmpty()) {
            throw new IllegalArgumentException("Working directory is empty");
        }

        dirsBox.getItems().add(dir);
    }

    @Override
    public final void setWorkingDirectories(List<String> dirs) {
        if (dirs.isEmpty()) {
            throw new IllegalArgumentException("List of working directories is empty");
        }

        dirsBox.getItems().clear();
        dirsBox.getItems().addAll(dirs);
        dirsBox.getSelectionModel().select(0);
    }

    @Override
    public final void setThemes(List<String> themes) {
        if (themes.isEmpty()) {
            throw new IllegalArgumentException("List of themes is empty");
        }

        themesBox.getItems().addAll(themes);
        themesBox.getSelectionModel().select(0);
    }

    @Override
    public void selectDirectory(String dir) {
        if (!dirsBox.getItems().contains(dir)) {
            throw new IllegalArgumentException("Such a directory does not exist: " + dir);
        }

        dirsBox.getSelectionModel().select(dir);
    }

    @Override
    public void selectTheme(Theme theme) {
        if (theme == Theme.LIGHT) {
            themesBox.getSelectionModel().select(0);
        } else if (theme == Theme.DARK) {
            themesBox.getSelectionModel().select(1);
        }
    }

    @Override
    public void setLicenseInfo(String info) {
        licenseInfo.setText(info);
    }

    @Override
    public void setSaveSessionSelected(boolean isSelected) {
        saveSessionButton.setSelected(isSelected);
    }

    @Override
    public void setStatusBarSelected(boolean isSelected) {
        statusBarButton.setSelected(isSelected);
    }

    @Override
    public void setTargetFieldSelected(boolean isSelected) {
        targetFieldButton.setSelected(isSelected);
    }

    @Override
    public void setWrappingSelected(boolean isSelected) {
        wrappingButton.setSelected(isSelected);
    }

    @Override
    public void setHighlighterSelected(boolean isSelected) {
        highlighterButton.setSelected(isSelected);
    }

    @Override
    public void setLineNumbersSelected(boolean isSelected) {
        lineNumbersButton.setSelected(isSelected);
    }

    @Override
    public void addThemeChangeListener(ChangeListener<String> listener) {
        themesBox.getSelectionModel().selectedItemProperty().addListener(listener);
    }

    @Override
    public void addDirChangeListener(ChangeListener<String> listener) {
        dirsBox.getSelectionModel().selectedItemProperty().addListener(listener);
    }

    @Override
    public void setOnAddDirAction(EventHandler<ActionEvent> action) {
        addDirButton.setOnAction(action);
    }

    @Override
    public void setOnSaveSessionAction(EventHandler<ActionEvent> action) {
        saveSessionButton.setOnAction(action);
    }

    @Override
    public void setOnSetDefaultsAction(EventHandler<ActionEvent> action) {
        setDefaultsButton.setOnAction(action);
    }

    @Override
    public void setOnStatusBarAction(EventHandler<ActionEvent> action) {
        statusBarButton.setOnAction(action);
    }

    @Override
    public void setOnTargetFieldAction(EventHandler<ActionEvent> action) {
        targetFieldButton.setOnAction(action);
    }

    @Override
    public void setOnWrappingAction(EventHandler<ActionEvent> action) {
        wrappingButton.setOnAction(action);
    }

    @Override
    public void setOnHighlighterAction(EventHandler<ActionEvent> action) {
        highlighterButton.setOnAction(action);
    }

    @Override
    public void setOnLineNumbersAction(EventHandler<ActionEvent> action) {
        lineNumbersButton.setOnAction(action);
    }

    @Override
    public void setOnChangeLicenseAction(EventHandler<ChangeLicenseEvent> action) {
        licenseEventHandler = action;
    }
}
