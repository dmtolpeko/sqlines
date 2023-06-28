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

import com.sqlines.studio.model.converter.ConversionResult;
import com.sqlines.studio.model.converter.Converter;
import com.sqlines.studio.model.filehandler.FileHandler;
import com.sqlines.studio.model.filehandler.listener.RecentFilesChangeListener;
import com.sqlines.studio.model.tabsdata.ObservableTabsData;
import com.sqlines.studio.model.tabsdata.listener.TabsChangeListener;
import com.sqlines.studio.model.tabsdata.listener.TabIndexChangeListener;
import com.sqlines.studio.model.tabsdata.listener.TabTitleChangeListener;
import com.sqlines.studio.model.tabsdata.listener.TextChangeListener;
import com.sqlines.studio.model.tabsdata.listener.ModeChangeListener;
import com.sqlines.studio.view.mainwindow.MainWindowView;
import com.sqlines.studio.view.mainwindow.event.RecentFileEvent;
import com.sqlines.studio.view.mainwindow.event.TabCloseEvent;

import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.scene.input.DragEvent;
import javafx.scene.input.Dragboard;
import javafx.scene.input.TransferMode;

import java.awt.Desktop;
import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Responds to user actions in the main window.
 * Retrieves data from the model, and displays it in the main window.
 */
public class MainWindowPresenter {
    private static final Logger logger = LogManager.getLogger(MainWindowPresenter.class);

    private final ObservableTabsData tabsData;
    private final FileHandler fileHandler;
    private final Converter converter;
    private final MainWindowView view;

    private final TabsChangeListener modelTabsListener = this::modelTabsChanged;
    private final TabIndexChangeListener modelIndexListener = this::modelTabIndexChanged;
    private final TabTitleChangeListener modelTabTitleListener = this::modelTabTileChanged;
    private final ModeChangeListener modelSourceModeListener = (newMode, tabIndex) ->
            modelSourceModeChanged(newMode);
    private final ModeChangeListener modelTargetModeListener = (newMode, tabIndex) ->
            modelTargetModeChanged(newMode);
    private final TextChangeListener modelSourceTextListener = this::modelSourceTextChanged;
    private final TextChangeListener modelTargetTextListener = this::modelTargetTextChanged;

    private final ChangeListener<Number> viewTabIndexListener = (o, oldIndex, newIndex) ->
            viewTabIndexChanged(newIndex.intValue());
    private final com.sqlines.studio.view.mainwindow.listener.TabTitleChangeListener viewTabTitleListener =
            this::viewTabTitleChanged;
    private final com.sqlines.studio.view.mainwindow.listener.ModeChangeListener viewSourceModeListener =
            this::viewSourceModeChanged;
    private final com.sqlines.studio.view.mainwindow.listener.ModeChangeListener viewTargetModeListener =
            this::viewTargetModeChanged;
    private final com.sqlines.studio.view.mainwindow.listener.TextChangeListener viewSourceTextListener =
            this::viewSourceTextChanged;
    private final com.sqlines.studio.view.mainwindow.listener.TextChangeListener viewTargetTextListener =
            this::viewTargetTextChanged;

    public MainWindowPresenter(ObservableTabsData tabsData,
                               FileHandler fileHandler,
                               Converter converter,
                               MainWindowView view) {
        this.tabsData = tabsData;
        this.fileHandler = fileHandler;
        this.converter = converter;
        this.view = view;

        initHandlers();
        initView();
        view.show();
    }

    private void modelTabsChanged(TabsChangeListener.Change change) {
        if (change.getChangeType() == TabsChangeListener.Change.ChangeType.TAB_ADDED) {
            view.openTab(change.getTabIndex());
            tabsData.setCurrTabIndex(change.getTabIndex());
        } else if (change.getChangeType() == TabsChangeListener.Change.ChangeType.TAB_REMOVED) {
            view.closeTab(change.getTabIndex());
        }
    }

    private void modelTabIndexChanged(int newIndex) {
        Platform.runLater(() -> {
            setCurrIndexInView(newIndex);
            setSourceModeInView(tabsData.getSourceMode(newIndex));
            setTargetModeInView(tabsData.getTargetMode(newIndex));
            showFilePathInView(newIndex);
        });
    }

    private void setCurrIndexInView(int index) {
        view.removeTabSelectionListener(viewTabIndexListener);
        view.setCurrTabIndex(index);
        view.addTabSelectionListener(viewTabIndexListener);
    }

    private void setSourceModeInView(String mode) {
        view.removeSourceModeListener(viewSourceModeListener);
        view.setSourceMode(mode);
        view.addSourceModeListener(viewSourceModeListener);
    }

    private void setTargetModeInView(String mode) {
        view.removeTargetModeListener(viewTargetModeListener);
        view.setTargetMode(mode);
        view.addTargetModeListener(viewTargetModeListener);
    }

    private void showFilePathInView(int tabIndex) {
        MainWindowView.FieldInFocus inFocus = view.inFocus(tabIndex);
        if (inFocus == MainWindowView.FieldInFocus.SOURCE) {
            view.showFilePath(tabsData.getSourceFilePath(tabIndex));
        } else if (inFocus == MainWindowView.FieldInFocus.TARGET) {
            view.showFilePath(tabsData.getTargetFilePath(tabIndex));
        }
    }

    private void modelTabTileChanged(String newTitle, int tabIndex) {
        Platform.runLater(() -> {
            view.removeTabTitleListener(viewTabTitleListener);
            view.setTabTitle(newTitle, tabIndex);
            view.addTabTitleListener(viewTabTitleListener);
        });
    }

    private void modelSourceModeChanged(String newMode) {
        if (!newMode.equals("")) {
            Platform.runLater(() -> setSourceModeInView(newMode));
        }
    }

    private void modelTargetModeChanged(String newMode) {
        if (!newMode.equals("")) {
            Platform.runLater(() -> setTargetModeInView(newMode));
        }
    }

    private void modelSourceTextChanged(String newText, int tabIndex) {
        Platform.runLater(() -> {
            view.removeSourceTextListener(viewSourceTextListener);
            view.setSourceText(newText, tabIndex);
            view.addSourceTextListener(viewSourceTextListener);
        });
    }

    private void modelTargetTextChanged(String newText, int tabIndex) {
        Platform.runLater(() -> {
            view.removeTargetTextListener(viewTargetTextListener);
            view.setTargetText(newText, tabIndex);
            view.addTargetTextListener(viewTargetTextListener);
        });
    }

    private void viewTabIndexChanged(int newIndex) {
        setCurrIndexInModel(newIndex);
        showFilePathInView(newIndex);

        String sourceMode = tabsData.getSourceMode(newIndex);
        if (sourceMode != null) {
            modelSourceModeChanged(sourceMode);
        }

        String targetMode = tabsData.getTargetMode(newIndex);
        if (targetMode != null) {
            modelTargetModeChanged(targetMode);
        }
    }

    private void setCurrIndexInModel(int index) {
        tabsData.removeTabIndexListener(modelIndexListener);
        tabsData.setCurrTabIndex(index);
        tabsData.addTabIndexListener(modelIndexListener);
    }

    private void viewTabTitleChanged(String newTitle, int tabIndex) {
        tabsData.removeTabTitleListener(modelTabTitleListener);
        tabsData.setTabTitle(newTitle, tabIndex);
        tabsData.addTabTitleListener(modelTabTitleListener);
    }

    private void viewSourceModeChanged(String newMode, int tabIndex) {
        tabsData.removeSourceModeListener(modelSourceModeListener);
        tabsData.setSourceMode(newMode, tabIndex);
        tabsData.addSourceModeListener(modelSourceModeListener);
    }

    private void viewTargetModeChanged(String newMode, int tabIndex) {
        tabsData.removeTargetModeListener(modelTargetModeListener);
        tabsData.setTargetMode(newMode, tabIndex);
        tabsData.addTargetModeListener(modelTargetModeListener);
    }

    private void viewSourceTextChanged(String newText, int tabIndex) {
        tabsData.removeSourceTextListener(modelSourceTextListener);
        tabsData.setSourceText(newText, tabIndex);
        tabsData.addSourceTextListener(modelSourceTextListener);
    }

    private void viewTargetTextChanged(String newText, int tabIndex) {
        tabsData.removeTargetTextListener(modelTargetTextListener);
        tabsData.setTargetText(newText, tabIndex);
        tabsData.addTargetTextListener(modelTargetTextListener);
    }

    private void initHandlers() {
        initFileHandler();
        initTabsDataHandlers();
        initViewHandlers();
    }

    private void initFileHandler() {
        fileHandler.addRecentFileListener(this::modelRecentFilesChanged);
    }

    private void modelRecentFilesChanged(RecentFilesChangeListener.Change change) {
        if (change.getChangeType() == RecentFilesChangeListener.Change.ChangeType.FILE_ADDED) {
            Platform.runLater(() -> view.addRecentFile(change.getFilePath()));
        } else if (change.getChangeType() == RecentFilesChangeListener.Change.ChangeType.FILE_MOVED) {
            String filePath = change.getFilePath();
            int movedTo = change.getMovedTo();
            Platform.runLater(() -> view.moveRecentFile(filePath, movedTo));
        }
    }

    private void initTabsDataHandlers() {
        tabsData.addTabsListener(modelTabsListener);
        tabsData.addTabIndexListener(modelIndexListener);
        tabsData.addTabTitleListener(modelTabTitleListener);
        tabsData.addSourceModeListener(modelSourceModeListener);
        tabsData.addTargetModeListener(modelTargetModeListener);
        tabsData.addSourceTextListener(modelSourceTextListener);
        tabsData.addTargetTextListener(modelTargetTextListener);
        tabsData.addSourceFilePathListener(this::modelSourcePathChanged);
        tabsData.addTargetFilePathListener(this::modelTargetPathChanged);
    }

    private void modelSourcePathChanged(String newPath, int tabIndex) {
        if (tabIndex == tabsData.getCurrTabIndex()) {
            MainWindowView.FieldInFocus inFocus = view.inFocus(tabIndex);
            if (inFocus == MainWindowView.FieldInFocus.SOURCE
                    || inFocus == MainWindowView.FieldInFocus.NONE) {
                Platform.runLater(() -> view.showFilePath(newPath));
            }
        }
    }

    private void modelTargetPathChanged(String newPath, int tabIndex) {
        if (tabIndex == tabsData.getCurrTabIndex()) {
            MainWindowView.FieldInFocus inFocus = view.inFocus(tabIndex);
            if (inFocus == MainWindowView.FieldInFocus.TARGET
                    || inFocus == MainWindowView.FieldInFocus.NONE) {
                Platform.runLater(() -> view.showFilePath(newPath));
            }
        }
    }

    private void initViewHandlers() {
        view.addTabSelectionListener(viewTabIndexListener);
        view.addTabTitleListener(viewTabTitleListener);
        view.addSourceModeListener(viewSourceModeListener);
        view.addTargetModeListener(viewTargetModeListener);
        view.addSourceTextListener(viewSourceTextListener);
        view.addTargetTextListener(viewTargetTextListener);
        view.addFocusListener(this::viewFocusChanged);
        view.setOnDragAction(this::receiveDrag);
        view.setOnDropAction(this::receiveDrop);
        view.setOnNewTabAction(event -> openTabPressed());
        view.setOnCloseTabAction(this::closeTabPressed);
        view.setOnOpenFileAction(event -> openFilePressed());
        view.setOnRecentFileAction(this::openRecentFilePressed);
        view.setOnClearRecentAction(event -> clearRecentFilesPressed());
        view.setOnSaveFileAction(event -> saveFilePressed());
        view.setOnSaveAsAction(event -> saveFileAsPressed());
        view.setOnRunAction(event -> runConversionPressed());
        view.setOnOnlineHelpAction(event -> openOnlineHelpPressed());
        view.setOnOpenSiteAction(event -> openSitePressed());
    }

    private void viewFocusChanged(MainWindowView.FieldInFocus inFocus, int tabIndex) {
        if (inFocus == MainWindowView.FieldInFocus.SOURCE) {
            view.showFilePath(tabsData.getSourceFilePath(tabIndex));
        } else if (inFocus == MainWindowView.FieldInFocus.TARGET) {
            view.showFilePath(tabsData.getTargetFilePath(tabIndex));
        }
    }

    private void receiveDrag(DragEvent dragEvent) {
        logger.info("Drag received");
        Dragboard dragboard = dragEvent.getDragboard();
        if (!dragboard.hasFiles() || dragboard.getFiles().stream().allMatch(File::isDirectory)) {
            dragEvent.consume();
            logger.info("Drag consumed");
        } else {
            dragEvent.acceptTransferModes(TransferMode.COPY);
            logger.info("Drag accepted");
        }
    }

    private void receiveDrop(DragEvent dragEvent) {
       openFiles(dragEvent.getDragboard().getFiles());
    }

    private void openFiles(List<File> files) {
        try {
            logger.info("Opening " + files.size() + " files");
            fileHandler.openSourceFiles(files);
            logger.info(files.size() + " files opened: " + files);
        } catch (Exception e) {
            view.showError("File opening error: ", e.getMessage());
        }
    }

    private void openTabPressed() {
        int nextIndex = tabsData.getCurrTabIndex() + 1;
        tabsData.removeTabsListener(modelTabsListener);
        tabsData.openTab(nextIndex);
        tabsData.addTabsListener(modelTabsListener);

        view.openTab(nextIndex);
        tabsData.setCurrTabIndex(nextIndex);

        logger.info("Tab opened. Index: " + nextIndex);
    }

    private void closeTabPressed(TabCloseEvent closeRequestEvent) {
        if (tabsData.countTabs() == 1) {
            return;
        }

        int tabIndex = closeRequestEvent.getTabIndex();
        tabsData.removeTabsListener(modelTabsListener);
        tabsData.removeTab(tabIndex);
        tabsData.addTabsListener(modelTabsListener);

        view.closeTab(tabIndex);
        view.setCurrTabIndex((tabIndex != 0) ? tabIndex - 1 : tabIndex);
        logger.info("Tab " + tabIndex + " closed");
    }

    private void openFilePressed() {
        Optional<File> initialDir = getLastDir();
        Optional<List<File>> selectedFiles;
        if (initialDir.isPresent()) {
            selectedFiles = view.choseFilesToOpen(initialDir.get());
        } else {
            selectedFiles = view.choseFilesToOpen();
        }

        if (selectedFiles.isEmpty()) {
            logger.info("No files to open");
            return;
        }

        openFiles(new ArrayList<>(selectedFiles.get()));
    }

    private Optional<File> getLastDir() {
        String lastDir = System.getProperties().getProperty("model.last-dir", null);
        return Optional.ofNullable(lastDir)
                .flatMap(path -> (path.equals("null") ? Optional.empty() : Optional.of(path)))
                .flatMap(path -> Optional.of(new File(path)));
    }

    private void openRecentFilePressed(RecentFileEvent recentFileEvent) {
        List<File> mutableList = new ArrayList<>();
        File file = new File(recentFileEvent.getFilePath());
        mutableList.add(file);
        openFiles(mutableList);
    }

    private void clearRecentFilesPressed() {
        fileHandler.clearRecentFiles();
        view.clearRecentFiles();
        logger.info("Recent files cleared");
    }

    private void saveFilePressed() {
        int currIndex = tabsData.getCurrTabIndex();
        MainWindowView.FieldInFocus inFocus = view.inFocus(currIndex);
        try {
            if (inFocus == MainWindowView.FieldInFocus.SOURCE) {
                saveSourceFile(currIndex);
            } else if (inFocus == MainWindowView.FieldInFocus.TARGET) {
                saveTargetFile(currIndex);
            }
        } catch (Exception e) {
            logger.error("Saving file: " + e.getMessage());
            view.showError("Filesystem error", e.getMessage());
        }
    }

    private void saveSourceFile(int tabIndex) throws IOException {
        if (tabsData.getSourceFilePath(tabIndex).isEmpty()) {
            saveFileAsPressed();
            return;
        }

        logger.info("Saving source file in tab " + tabIndex);
        fileHandler.saveSourceFile(tabIndex);
        logger.info("Source file saved in tab " + tabIndex);
    }

    private void saveTargetFile(int tabIndex) throws IOException {
        if (tabsData.getTargetFilePath(tabIndex).isEmpty()) {
            saveFileAsPressed();
            return;
        }

        logger.info("Saving target file in tab " + tabIndex);
        fileHandler.saveTargetFile(tabIndex);
        logger.info("Target file saved in tab " + tabIndex);
    }

    private void saveFileAsPressed() {
        int currIndex = tabsData.getCurrTabIndex();
        MainWindowView.FieldInFocus inFocus = view.inFocus(currIndex);
        Optional<String> optionalFilePath = view.choseFileSavingLocation();
        String filePath;
        if (optionalFilePath.isPresent()) {
            filePath = optionalFilePath.get();
        } else {
            return;
        }

        try {
            if (inFocus == MainWindowView.FieldInFocus.SOURCE) {
                saveSourceFileAs(currIndex, filePath);
            } else if (inFocus == MainWindowView.FieldInFocus.TARGET) {
                saveTargetFileAs(currIndex, filePath);
            }
        } catch (Exception e) {
            logger.error("Saving file: " + e.getMessage());
            view.showError("Filesystem error", e.getMessage());
        }
    }

    private void saveSourceFileAs(int tabIndex, String path) throws IOException {
        logger.info("Saving source file: " + path);
        fileHandler.saveSourceFileAs(tabIndex, path);
        logger.info("Source file saved: " + path);
    }

    private void saveTargetFileAs(int tabIndex, String path) throws IOException {
        logger.info("Saving target file: " + path);
        fileHandler.saveTargetFileAs(tabIndex, path);
        logger.info("Target file saved: " + path);
    }

    private void runConversionPressed() {
        int currIndex = tabsData.getCurrTabIndex();
        try {
            logger.info("Running conversion in tab " + currIndex);
            runConversion(currIndex);
            logger.info("Conversion ended in tab " + currIndex);
        } catch (Exception e) {
            showConversionError(currIndex, e.getMessage());
        } finally {
            showConversionEnd(currIndex);
        }
    }

    private void runConversion(int tabIndex) throws Exception {
        if (!tabsData.getSourceFilePath(tabIndex).isEmpty()) {
            logger.info("Saving source file in tab " + tabIndex);
            fileHandler.saveSourceFile(tabIndex);
            logger.info("Source file saved in tab " + tabIndex);
        }

        Platform.runLater(() -> view.showConversionStart(tabIndex));
        String sourceMode = tabsData.getSourceMode(tabIndex);
        String targetMode = tabsData.getTargetMode(tabIndex);
        String targetFileName = tabsData.getTabTitle(tabIndex).trim().toLowerCase();
        String sourceFilePath = tabsData.getSourceFilePath(tabIndex);
        ConversionResult result;
        if (!sourceFilePath.isEmpty()) {
            result = converter.run(sourceMode, targetMode, sourceFilePath, targetFileName);
        } else {
            byte[] sourceData = tabsData.getSourceText(tabIndex).getBytes();
            result = converter.run(sourceMode, targetMode, sourceData, targetFileName);
        }

        tabsData.setTargetText(result.getData(), tabIndex);
        tabsData.setTargetFilePath(result.getTargetFilePath(), tabIndex);
    }

    private void showConversionEnd(int tabIndex) {
        Platform.runLater(() -> view.showConversionEnd(tabIndex));
    }

    private void showConversionError(int tabIndex, String errorMsg) {
        String error = "Conversion error in tab " + (tabIndex + 1) + ".\n" + errorMsg;
        logger.error(error);
        Platform.runLater(() -> view.showError("Conversion error", error));
    }

    private void openOnlineHelpPressed() {
        if (Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.BROWSE)) {
            try {
                Desktop.getDesktop().browse(new URI("https://www.sqlines.com/contact-us"));
                logger.info("Online help opened");
            } catch (Exception e) {
               logger.error("Open online help: " + e.getMessage());
            }
        }
    }

    private void openSitePressed() {
        if (Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.BROWSE)) {
            try {
                Desktop.getDesktop().browse(new URI("https://www.sqlines.com"));
                logger.info("Site opened");
            } catch (Exception e) {
                logger.error("Open site: " + e.getMessage());
            }
        }
    }

    private void initView() {
        try {
            int tabsNumber = tabsData.countTabs();
            if (tabsNumber == 0) {
                openTabPressed();
                return;
            }
            loadTabsData();
            loadRecentFiles();
            logger.info("View initialized");
        } catch (Exception e) {
            handleViewInitializationFailure(e);
        }
    }

    private void loadTabsData() {
        int currIndex = tabsData.getCurrTabIndex();
        for (int i = 0; i < tabsData.countTabs(); i++) {
            String title = tabsData.getTabTitle(i);
            String sourceMode = tabsData.getSourceMode(i);
            String targetMode = tabsData.getTargetMode(i);

            view.openTab(i);
            tabsData.setTabTitle(title, i);
            tabsData.setSourceMode(sourceMode, i);
            tabsData.setTargetMode(targetMode, i);
            modelSourceTextChanged(tabsData.getSourceText(i), i);
            modelTargetTextChanged(tabsData.getTargetText(i), i);
        }
        tabsData.setCurrTabIndex(currIndex);
    }

    private void loadRecentFiles() {
        for (int i = 0; i < fileHandler.countRecentFiles(); i++) {
            view.addRecentFile(fileHandler.getRecentFile(i));
        }
    }

    private void handleViewInitializationFailure(Exception e) {
        logger.error("View initialization failed: " + e.getMessage() + ". Going back to defaults");
        tabsData.removeAllTabs();
        view.closeAllTabs();
        openTabPressed();
        fileHandler.clearRecentFiles();
        view.clearRecentFiles();
    }
}
