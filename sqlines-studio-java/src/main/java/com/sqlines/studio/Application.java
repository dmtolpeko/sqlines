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

package com.sqlines.studio;

import com.sqlines.studio.model.converter.CmdModes;
import com.sqlines.studio.model.converter.Converter;
import com.sqlines.studio.model.converter.ConverterImpl;
import com.sqlines.studio.model.coreprocess.CoreProcessRunner;
import com.sqlines.studio.model.coreprocess.CoreProcessRunnerImp;
import com.sqlines.studio.model.license.License;
import com.sqlines.studio.model.PropertiesLoader;
import com.sqlines.studio.model.ResourceLoader;
import com.sqlines.studio.model.filehandler.FileHandler;
import com.sqlines.studio.model.tabsdata.ObservableTabsData;
import com.sqlines.studio.presenter.MainWindowPresenter;
import com.sqlines.studio.presenter.SettingsPresenter;
import com.sqlines.studio.view.mainwindow.MainWindow;
import com.sqlines.studio.view.settings.SettingsWindow;

import javafx.stage.Stage;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.List;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class Application extends javafx.application.Application {
    private static final Logger logger = LogManager.getLogger(Application.class);

    private ObservableTabsData tabsData;
    private FileHandler fileHandler;
    private MainWindow mainWindow;
    private Thread fileCheckingThread;
    private Thread licenseCheckingThread;
    private Thread checkpointThread;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void init() {
        loadProperties();
        loadLastState();
    }

    private void loadProperties() {
        try {
            PropertiesLoader.loadProperties();
            logger.info("Properties loaded");
        } catch (Exception e) {
            logger.warn("Properties loading failed: " + e.getMessage());
            PropertiesLoader.setDefaults();
        }
    }

    private void loadLastState() {
        String saveSession = System.getProperty("model.save-session");
        if (saveSession.equals("enabled")) {
            deserializeModel();
            logger.info("Last state loaded");
        } else {
            tabsData = new ObservableTabsData();
            fileHandler = new FileHandler();
            fileHandler.setTabsData(tabsData);
        }
    }

    private void deserializeModel() {
        try (ObjectInputStream tabsDataStream = getTabsDataInStream();
             ObjectInputStream fileHandlerStream = getFileHandlerInStream()) {
            tabsData = (ObservableTabsData) tabsDataStream.readObject();
            fileHandler = (FileHandler) fileHandlerStream.readObject();
        } catch (Exception e) {
            logger.warn("Deserialization error: " + e.getMessage());
            tabsData = new ObservableTabsData();
            fileHandler = new FileHandler();
        } finally {
            fileHandler.setTabsData(tabsData);
        }
    }

    private ObjectInputStream getTabsDataInStream() throws IOException  {
        String tabsDataPath = System.getProperty("java.io.tmpdir") + "sqlines-tabsdata.serial";
        return new ObjectInputStream(new FileInputStream(tabsDataPath));
    }

    private ObjectInputStream getFileHandlerInStream() throws IOException {
        String fileHandlerPath = System.getProperty("java.io.tmpdir") + "sqlines-filehandler.serial";
        return new ObjectInputStream(new FileInputStream(fileHandlerPath));
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        CoreProcessRunner coreProcess = new CoreProcessRunnerImp();
        Converter converter = new ConverterImpl(new CmdModes(ResourceLoader.loadCmdModes()), coreProcess);
        License license = new License(coreProcess);

        mainWindow = new MainWindow();
        mainWindow.setConversionModes(ResourceLoader.loadSourceModes(), ResourceLoader.loadTargetModes());
        mainWindow.setLightStylesheets(ResourceLoader.loadMainLightStyles());
        mainWindow.setDarkStylesheets(ResourceLoader.loadMainDarkStyles());

        SettingsWindow settingsWindow = new SettingsWindow();
        settingsWindow.setLightStylesheets(ResourceLoader.loadSettingLightStyles());
        settingsWindow.setDarkStylesheets(ResourceLoader.loadSettingDarkStyles());

        SettingsPresenter settingsPresenter = new SettingsPresenter(
                license, settingsWindow, mainWindow, List.of(mainWindow, settingsWindow)
        );
        MainWindowPresenter mainPresenter = new MainWindowPresenter(
                tabsData, fileHandler, converter, mainWindow
        );

        fileCheckingThread = new Thread(fileHandler, "File Checking Thread");
        fileCheckingThread.setDaemon(true);
        fileCheckingThread.start();

        licenseCheckingThread = new Thread(license, "License Checking Thread");
        licenseCheckingThread.setDaemon(true);
        licenseCheckingThread.start();

        checkpointThread = new Thread(this::runCheckpointLoop, "Checkpoint Thread");
        checkpointThread.setDaemon(true);
        checkpointThread.start();
    }

    @SuppressWarnings("BusyWait")
    private void runCheckpointLoop() {
        while (true) {
            try {
                Thread.sleep(40000);
                serializeModel();
                logger.info("Checkpoint made");
            } catch (InterruptedException e) {
                break;
            } catch (Exception e) {
                logger.error("Checkpoint loop: " + e.getMessage());
            }
        }
    }

    private void serializeModel() {
        try (ObjectOutputStream tabsDataStream = getTabsDataOutStream();
             ObjectOutputStream fileHandlerStream = getFileHandlerOutStream()) {
            tabsDataStream.writeObject(tabsData);
            fileHandlerStream.writeObject(fileHandler);
        } catch (Exception e) {
            logger.warn("Serialization error: " + e.getMessage());
        }
    }

    private ObjectOutputStream getTabsDataOutStream() throws IOException {
        String tabsDataPath = System.getProperty("java.io.tmpdir") + "sqlines-tabsdata.serial";
        return new ObjectOutputStream(new FileOutputStream(tabsDataPath));
    }

    private ObjectOutputStream getFileHandlerOutStream() throws IOException {
        String fileHandlerPath = System.getProperty("java.io.tmpdir") + "sqlines-filehandler.serial";
        return new ObjectOutputStream(new FileOutputStream(fileHandlerPath));
    }

    @Override
    public void stop() {
        try {
            fileCheckingThread.interrupt();
            licenseCheckingThread.interrupt();
            checkpointThread.interrupt();

            saveFiles();
            saveProperties();
            saveLastState();
        } catch (Exception e) {
            logger.error(e.getMessage());
        }
    }

    private void saveFiles() throws IOException {
        int tabsNumber = tabsData.countTabs();
        for (int i = 0; i < tabsNumber; i++) {
            if (!tabsData.getSourceFilePath(i).isEmpty()) {
                fileHandler.saveSourceFile(i);
            }

            if (!tabsData.getTargetFilePath(i).isEmpty()) {
                fileHandler.saveTargetFile(i);
            }
        }
    }

    private void saveProperties() {
        try {
            logger.info("Saving properties");
            saveUISettings();
            PropertiesLoader.saveProperties();
            logger.info("Properties saved");
        } catch (Exception e) {
            logger.warn("Properties saving failed: " + e.getMessage());
        }
    }

    private void saveUISettings() {
        System.setProperty("view.height", String.valueOf(mainWindow.getHeight()));
        System.setProperty("view.width", String.valueOf(mainWindow.getWidth()));
        System.setProperty("view.pos.x", String.valueOf(mainWindow.getX()));
        System.setProperty("view.pos.y", String.valueOf(mainWindow.getY()));
        System.setProperty("view.isMaximized", String.valueOf(mainWindow.isMaximized()));
    }

    private void saveLastState() {
        logger.info("Saving last state");
        serializeModel();
        logger.info("Last state saved");
    }
}
