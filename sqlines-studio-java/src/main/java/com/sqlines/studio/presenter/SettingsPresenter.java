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

package com.sqlines.studio.presenter;

import com.sqlines.studio.model.license.License;
import com.sqlines.studio.model.PropertiesLoader;
import com.sqlines.studio.view.AbstractWindow;
import com.sqlines.studio.view.mainwindow.MainWindowSettingsView;
import com.sqlines.studio.view.settings.event.ChangeLicenseEvent;
import com.sqlines.studio.view.settings.SettingsWindowView;

import javafx.application.Platform;
import javafx.beans.value.ObservableValue;

import java.util.List;
import java.util.Optional;
import java.util.Properties;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Responds to user actions in the settings window.
 * Retrieves data from the model, and displays it in the settings window.
 */
public class SettingsPresenter {
    private static final Logger logger = LogManager.getLogger(SettingsPresenter.class);
    private static final Properties properties = System.getProperties();

    private final License license;
    private final SettingsWindowView settingsWindow;
    private final MainWindowSettingsView mainWindow;
    private final List<? extends AbstractWindow> windows;

    /**
     * Constructs a new  SettingsPresenter.
     *
     * @param license license
     * @param settingsWindowView settings window
     * @param mainWindowView main window
     * @param windows all windows
     */
    public SettingsPresenter(License license,
                             SettingsWindowView settingsWindowView,
                             MainWindowSettingsView mainWindowView,
                             List<? extends AbstractWindow> windows) {
        this.license = license;
        this.settingsWindow = settingsWindowView;
        this.mainWindow = mainWindowView;
        this.windows = windows;
        init();
    }

    private void init() {
        setDefaults();
        initHandlers();
        loadProperties();
        checkLicense();
    }

    private void setDefaults() {
        String defaultDir = properties.getProperty("user.home") + "/sqlines";
        settingsWindow.setWorkingDirectories(List.of(defaultDir));
        settingsWindow.setThemes(List.of("Light", "Dark"));
    }

    private void initHandlers() {
        initLicenseHandlers();
        initMainWindowHandlers();
        initSettingsWindowHandlers();
    }

    private void initLicenseHandlers() {
        license.addLicenseListener(this::licenseChanged);
    }

    private void licenseChanged(boolean isActive) {
        Platform.runLater(() -> {
            if (isActive) {
                mainWindow.setWindowTitle("SQLines Studio");
                settingsWindow.setLicenseInfo("License: Active");
            } else {
                mainWindow.setWindowTitle("SQLINES STUDIO - FOR EVALUATION USE ONLY");
                settingsWindow.setLicenseInfo("License: For evaluation use only");
            }
        });
    }

    private void initMainWindowHandlers() {
        mainWindow.setOnPreferencesAction(event -> showSettingsWindow());
        mainWindow.setOnCloseAction(event -> windows.forEach(AbstractWindow::close));
        mainWindow.setOnStatusBarAction(event -> changeStatusBarPolicyPressed());
        mainWindow.setOnTargetFieldAction(event -> changeTargetFieldPolicyPressed());
        mainWindow.setOnWrappingAction(event -> changeWrappingPolicyPressed());
        mainWindow.setOnHighlighterAction(event -> changeHighlighterPolicePressed());
        mainWindow.setOnLineNumbersAction(event -> changeLineNumbersPolicyPressed());
    }

    private void showSettingsWindow() {
        if (!settingsWindow.isShowing()) {
            settingsWindow.show();
        } else {
            settingsWindow.toFront();
        }
    }

    private void changeStatusBarPolicyPressed() {
        try {
            String currentPolicy = properties.getProperty("view.status-bar", "show");
            if (currentPolicy.equals("show")) {
                hideStatusBar();
                logger.info("Status bar policy changed. New policy: Do not show");
            } else if (currentPolicy.equals("do-not-show")) {
                showStatusBar();
                logger.info("Status bar policy changed. New policy: Show");
            }
            PropertiesLoader.saveProperties();
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void hideStatusBar() {
        properties.setProperty("view.status-bar", "do-not-show");
        mainWindow.setStatusBarPolicy(MainWindowSettingsView.StatusBarPolicy.DO_NOT_SHOW);
        settingsWindow.setStatusBarSelected(false);
    }

    private void showStatusBar() {
        properties.setProperty("view.status-bar", "show");
        mainWindow.setStatusBarPolicy(MainWindowSettingsView.StatusBarPolicy.SHOW);
        settingsWindow.setStatusBarSelected(true);
    }

    private void handleSettingsSaveException(Exception e) {
        String errorMsg = "An error occurred while " +
                "saving the settings.\n" + e.getMessage();
        logger.error(errorMsg);
        settingsWindow.showError("Error", errorMsg);
    }

    private void changeTargetFieldPolicyPressed() {
        try {
            String currentPolicy = properties.getProperty("view.target-field", "as-needed");
            if (currentPolicy.equals("always")) {
                hideTargetField();
                logger.info("Target field policy changed. New policy: As needed");
            } else if (currentPolicy.equals("as-needed")) {
                showTargetField();
                logger.info("Target field policy changed. New policy: Always");
            }
            PropertiesLoader.saveProperties();
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void hideTargetField() {
        properties.setProperty("view.target-field", "as-needed");
        mainWindow.setTargetFieldPolicy(MainWindowSettingsView.TargetFieldPolicy.AS_NEEDED);
        settingsWindow.setTargetFieldSelected(false);
    }

    private void showTargetField() {
        properties.setProperty("view.target-field", "always");
        mainWindow.setTargetFieldPolicy(MainWindowSettingsView.TargetFieldPolicy.ALWAYS);
        settingsWindow.setTargetFieldSelected(true);
    }

    private void changeWrappingPolicyPressed() {
        try {
            String currentPolicy = properties.getProperty("view.wrapping", "enabled");
            if (currentPolicy.equals("enabled")) {
                disableWrapping();
                logger.info("Wrapping policy changed. New policy: No wrap");
            } else if (currentPolicy.equals("disabled")) {
                enableWrapping();
                logger.info("Wrapping policy changed. New policy: Wrap lines");
            }
            PropertiesLoader.saveProperties();
        } catch (Exception e) {
           handleSettingsSaveException(e);
        }
    }

    private void disableWrapping() {
        properties.setProperty("view.wrapping", "disabled");
        mainWindow.setWrappingPolicy(MainWindowSettingsView.WrappingPolicy.NO_WRAP);
        settingsWindow.setWrappingSelected(false);
    }

    private void enableWrapping() {
        properties.setProperty("view.wrapping", "enabled");
        mainWindow.setWrappingPolicy(MainWindowSettingsView.WrappingPolicy.WRAP_LINES);
        settingsWindow.setWrappingSelected(true);
    }

    public void changeHighlighterPolicePressed() {
        try {
            String currentPolicy = properties.getProperty("view.highlighter", "enabled");
            if (currentPolicy.equals("enabled")) {
                disableHighlighter();
                logger.info("Highlighter policy changed. New policy: Do not highlight");
            } else if (currentPolicy.equals("disabled")) {
                enableHighlighter();
                logger.info("Highlighter policy changed. New policy: Highlight");
            }
            PropertiesLoader.saveProperties();
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void disableHighlighter() {
        properties.setProperty("view.highlighter", "disabled");
        mainWindow.setHighlighterPolicy(MainWindowSettingsView.HighlighterPolicy.DO_NOT_HIGHLIGHT);
        settingsWindow.setHighlighterSelected(false);
    }

    private void enableHighlighter() {
        properties.setProperty("view.highlighter", "enabled");
        mainWindow.setHighlighterPolicy(MainWindowSettingsView.HighlighterPolicy.HIGHLIGHT);
        settingsWindow.setHighlighterSelected(true);
    }

    private void changeLineNumbersPolicyPressed() {
        try {
            String currentPolicy = properties.getProperty("view.line-numbers", "enabled");
            if (currentPolicy.equals("enabled")) {
                hideLineNumbers();
                logger.info("Line numbers area policy changed. New policy: Do not show");
            } else if (currentPolicy.equals("disabled")) {
                showLineNumbers();
                logger.info("Line numbers area policy changed. New policy: Show");
            }
            PropertiesLoader.saveProperties();
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void hideLineNumbers() {
        properties.setProperty("view.line-numbers", "disabled");
        mainWindow.setLineNumbersPolicy(MainWindowSettingsView.LineNumbersPolicy.DO_NOT_SHOW);
        settingsWindow.setLineNumbersSelected(false);
    }

    private void showLineNumbers() {
        properties.setProperty("view.line-numbers", "enabled");
        mainWindow.setLineNumbersPolicy(MainWindowSettingsView.LineNumbersPolicy.SHOW);
        settingsWindow.setLineNumbersSelected(true);
    }

    private void initSettingsWindowHandlers() {
        settingsWindow.addThemeChangeListener((o, old, newValue) -> themeChanged(newValue));
        settingsWindow.addDirChangeListener(this::workingDirChanged);
        settingsWindow.setOnAddDirAction(event -> addDirPressed());
        settingsWindow.setOnSaveSessionAction(event -> saveLastSessionPressed());
        settingsWindow.setOnSetDefaultsAction(event -> setDefaultsPressed());
        settingsWindow.setOnChangeLicenseAction(this::changeLicensePressed);
        settingsWindow.setOnStatusBarAction(event -> changeStatusBarPolicyPressed());
        settingsWindow.setOnTargetFieldAction(event -> changeTargetFieldPolicyPressed());
        settingsWindow.setOnWrappingAction(event -> changeWrappingPolicyPressed());
        settingsWindow.setOnHighlighterAction(event -> changeHighlighterPolicePressed());
        settingsWindow.setOnLineNumbersAction(event -> changeLineNumbersPolicyPressed());
    }

    private void themeChanged(String newTheme) {
        try {
            if (newTheme.equals("Light")) {
                setLightTheme();
                logger.info("Theme changed. New theme - light");
            } else if (newTheme.equals("Dark")) {
                setDarkTheme();
                logger.info("Theme changed. New theme - dark");
            }
            PropertiesLoader.saveProperties();
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void setLightTheme() {
        properties.setProperty("view.theme", "light");
        settingsWindow.selectTheme(AbstractWindow.Theme.LIGHT);
        windows.forEach(window -> window.setTheme(AbstractWindow.Theme.LIGHT));
    }

    private void setDarkTheme() {
        properties.setProperty("view.theme", "dark");
        settingsWindow.selectTheme(AbstractWindow.Theme.LIGHT);
        windows.forEach(window -> window.setTheme(AbstractWindow.Theme.DARK));
    }

    private void workingDirChanged(ObservableValue<? extends String> observable,
                                   String oldDir, String newDir) {
        try {
            changeDir(newDir);
            logger.info("Current working directory changed: " + newDir);
            PropertiesLoader.saveProperties();
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void changeDir(String newDir) {
        properties.setProperty("model.curr-dir", newDir);
        settingsWindow.selectDirectory(newDir);
    }

    private void addDirPressed() {
        try {
            Optional<String> dir = settingsWindow.choseDirectoryToAdd();
            if (dir.isPresent()) {
                String dirPath = dir.get();
                addDir(dirPath);
                changeDir(dirPath);
                logger.info("New directory added: " + dirPath);
                PropertiesLoader.saveProperties();
            }
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void addDir(String newDir) {
        int dirsNumber = Integer.parseInt(properties.getProperty("model.dirs-number", "0"));
        properties.setProperty("model.dir-" + dirsNumber, newDir);
        properties.setProperty("model.dirs-number", String.valueOf((dirsNumber + 1)));

        settingsWindow.addDirectory(newDir);
    }

    private void saveLastSessionPressed() {
        try {
            String policy = properties.getProperty("model.save-session");
            if (policy.equals("enabled")) {
                disableSessionSaving();
                logger.info("Session saving policy changed. New policy: Disabled");
            } else if (policy.equals("disabled")) {
                enableSessionSaving();
                logger.info("Session saving policy changed. New policy: Enabled");
            }
            PropertiesLoader.saveProperties();
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void disableSessionSaving() {
        properties.setProperty("model.save-session", "disabled");
        settingsWindow.setSaveSessionSelected(false);
    }

    private void enableSessionSaving() {
        properties.setProperty("model.save-session", "enabled");
        settingsWindow.setSaveSessionSelected(true);
    }

    private void setDefaultsPressed() {
        try {
            PropertiesLoader.setDefaults();
            loadProperties();
            PropertiesLoader.saveProperties();
            logger.info("Default settings set");
        } catch (Exception e) {
            handleSettingsSaveException(e);
        }
    }

    private void loadProperties() {
        try {
            loadWindowProperties();
            loadSessionSaving();
            loadDirectories();
            loadTheme();
            loadStatusBarSetting();
            loadTargetFieldSetting();
            loadWrappingSetting();
            loadHighlighterSetting();
            loadLineNumbersSetting();
        } catch (Exception e) {
            logger.error("Loading properties: " + e.getMessage());
            PropertiesLoader.setDefaults();
            loadProperties();
        }
    }

    private void loadWindowProperties() {
        String heightProperty = properties.getProperty("view.height", "650.0");
        double height = Double.parseDouble(heightProperty);
        mainWindow.setHeight(height);

        String widthProperty = properties.getProperty("view.width", "770.0");
        double width = Double.parseDouble(widthProperty);
        mainWindow.setWidth(width);

        String posXProperty = properties.getProperty("view.pos.x", "0.0");
        double posX = Double.parseDouble(posXProperty);
        mainWindow.setX(posX);

        String posYProperty = properties.getProperty("view.pos.y", "0.0");
        double posY = Double.parseDouble(posYProperty);
        mainWindow.setY(posY);

        String isMaximized = properties.getProperty("view.is-maximized", "false");
        if (isMaximized.equals("true")) {
            mainWindow.setFullScreen(true);
        } else if (isMaximized.equals("false")) {
            mainWindow.setFullScreen(false);
        }
    }

    private void loadSessionSaving() {
        String saveSession = properties.getProperty("model.save-session", "enabled");
        if (saveSession.equals("enabled")) {
            enableSessionSaving();
        } else if (saveSession.equals("disabled")) {
            disableSessionSaving();
        }
    }

    private void loadDirectories() {
        loadAllDirs();
        loadCurrDir();
    }

    private void loadAllDirs() {
        int dirsNumber = Integer.parseInt(properties.getProperty("model.dirs-number", "0"));
        for (int i = 0; i < dirsNumber; i++) {
            String dir = properties.getProperty("model.dir-" + i);
            settingsWindow.addDirectory(dir);
        }
    }

    private void loadCurrDir() {
        String defaultDir = properties.getProperty("user.home") + "/sqlines";
        String currDir = properties.getProperty("model.curr-dir", defaultDir);
        settingsWindow.selectDirectory(currDir);
    }

    private void loadTheme() {
        String theme = properties.getProperty("view.theme", "light");
        if (theme.equals("light")) {
            setLightTheme();
        } else if (theme.equals("dark")) {
            setDarkTheme();
        }
    }

    private void loadStatusBarSetting() {
        String showStatusBar = properties.getProperty("view.status-bar", "show");
        if (showStatusBar.equals("show")) {
            showStatusBar();
        } else if (showStatusBar.equals("do-not-show")) {
            hideStatusBar();
        }
    }

    private void loadTargetFieldSetting() {
        String showTargetField = properties.getProperty("view.target-field", "as-needed");
        if (showTargetField.equals("always")) {
            showTargetField();
        } else if (showTargetField.equals("as-needed")) {
            hideTargetField();
        }
    }

    private void loadWrappingSetting() {
        String enableWrapping = properties.getProperty("view.wrapping", "enabled");
        if (enableWrapping.equals("enabled")) {
            enableWrapping();
        } else if (enableWrapping.equals("disabled")) {
            disableWrapping();
        }
    }

    private void loadHighlighterSetting() {
        String enableHighlighter = properties.getProperty("view.highlighter", "enabled");
        if (enableHighlighter.equals("enabled")) {
            enableHighlighter();
        } else if (enableHighlighter.equals("disabled")) {
            disableHighlighter();
        }
    }

    private void loadLineNumbersSetting() {
        String lineNumbersProperty = properties.getProperty("view.line-numbers", "enabled");
        if (lineNumbersProperty.equals("enabled")) {
            showLineNumbers();
        } else if (lineNumbersProperty.equals("disabled")) {
            hideLineNumbers();
        }
    }

    private void changeLicensePressed(ChangeLicenseEvent event) {
        try {
            license.changeLicense(event.getRegName(), event.getRegNumber());
            logger.info("License status changed");
        } catch (Exception e) {
            String errorMsg = "Change license status: " + e.getMessage();
            logger.info(errorMsg);
            settingsWindow.showError("Error", errorMsg);
        }
    }

    private void checkLicense() {
        try {
            if (license.isActive()) {
                showActiveLicense();
            } else {
                showNotActiveLicense();
            }
        } catch (Exception e) {
            showNotActiveLicense();
            logger.error("License check: " + e.getMessage());
        }
    }

    private void showActiveLicense() {
        mainWindow.setWindowTitle("SQLines Studio");
        settingsWindow.setLicenseInfo("License: Active");
    }

    private void showNotActiveLicense() {
        mainWindow.setWindowTitle("SQLINES STUDIO - FOR EVALUATION USE ONLY");
        settingsWindow.setLicenseInfo("License: For evaluation use only");
    }
}
