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

import com.sqlines.studio.view.AbstractWindow;
import com.sqlines.studio.view.mainwindow.editor.CodeEditor;
import com.sqlines.studio.view.mainwindow.event.RecentFileEvent;
import com.sqlines.studio.view.mainwindow.event.TabCloseEvent;
import com.sqlines.studio.view.mainwindow.listener.FocusChangeListener;
import com.sqlines.studio.view.mainwindow.listener.ModeChangeListener;
import com.sqlines.studio.view.mainwindow.listener.TabTitleChangeListener;
import com.sqlines.studio.view.mainwindow.listener.TextChangeListener;
import com.sqlines.studio.view.ErrorWindow;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.input.DragEvent;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;
import javafx.stage.WindowEvent;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.LinkedList;
import java.util.Optional;

/**
 * The concrete main window.
 */
public class MainWindow extends AbstractWindow implements MainWindowView, MainWindowSettingsView {
    private final BorderPane layout = new BorderPane();
    private final MainMenuBar menuBar = new MainMenuBar();
    private final MainToolBar toolBar = new MainToolBar();
    private final TabPane tabBar = new TabPane();
    private final StatusBar statusBar = new StatusBar();

    private final List<TabTitleChangeListener> tabTitleListeners = new ArrayList<>(5);
    private final List<ModeChangeListener> sourceModeListeners = new ArrayList<>(5);
    private final List<ModeChangeListener> targetModeListeners = new ArrayList<>(5);
    private final List<TextChangeListener> sourceTextListeners = new ArrayList<>(5);
    private final List<TextChangeListener> targetTextListeners = new ArrayList<>(5);
    private final List<FocusChangeListener> focusListeners = new ArrayList<>(5);

    private EventHandler<TabCloseEvent> tabCloseEventHandler;
    private EventHandler<DragEvent> dragEventHandler;
    private EventHandler<DragEvent> dropEventHandler;

    private FieldInFocus inFocus = FieldInFocus.SOURCE;
    private TargetFieldPolicy targetFieldPolicy = TargetFieldPolicy.AS_NEEDED;
    private WrappingPolicy wrappingPolicy = WrappingPolicy.NO_WRAP;
    private HighlighterPolicy highlighterPolicy = HighlighterPolicy.HIGHLIGHT;
    private LineNumbersPolicy lineNumbersPolicy = LineNumbersPolicy.SHOW;

    public MainWindow() {
        setUpMenuBar();
        setUpTabBar();
        setUpScene();
        setUpWindow();

        setUpMenuBarEventHandlers();
        setUpTabBarEventHandlers();
        setUpToolBarEventHandlers();
        ignoreTabBarKeyEvents();
    }

    private void setUpMenuBar() {
        menuBar.setCloseTabState(false);
        menuBar.setNextTabState(false);
        menuBar.setPrevTabState(false);
        menuBar.setOpenRecentState(false);
        menuBar.setUndoState(false);
        menuBar.setRedoState(false);
        menuBar.setStatusBarSelected(true);
        menuBar.setTargetFieldSelected(false);
        menuBar.setWrappingSelected(false);
        menuBar.setHighlighterSelected(true);
        menuBar.setLineNumbersSelected(true);
    }

    private void setUpTabBar() {
        tabBar.setTabClosingPolicy(TabPane.TabClosingPolicy.ALL_TABS);
    }

    private void setUpScene() {
        layout.setTop(new VBox(menuBar, toolBar));
        layout.setCenter(tabBar);
        layout.setBottom(statusBar);
        setRoot(layout);
    }

    private void setUpWindow() {
        setTitle("SQLines Studio");
        setMinWidth(660);
        setMinHeight(300);
        setWidth(770);
        setHeight(650);
    }

    private void setUpMenuBarEventHandlers() {
        menuBar.setOnCloseTabAction(this::handleTabCloseEvent);
        menuBar.setOnAboutAction(event -> showAbout());
        menuBar.setOnNextTabAction(event -> nextTab());
        menuBar.setOnPrevTabAction(event -> prevTab());
        menuBar.setOnUndoAction(event -> undo());
        menuBar.setOnRedoAction(event -> redo());
        menuBar.setOnSelectAllAction(event -> selectAll());
        menuBar.setOnCutAction(event -> cut());
        menuBar.setOnCopyAction(event -> copy());
        menuBar.setOnPasteAction(event -> paste());
        menuBar.setOnZoomInAction(event -> zoomIn());
        menuBar.setOnZoomOutAction(event -> zoomOut());
    }

    private void handleTabCloseEvent(ActionEvent event) {
        int tabIndex = tabBar.getSelectionModel().getSelectedIndex();
        TabCloseEvent closeEvent = new TabCloseEvent(tabIndex);
        tabBar.fireEvent(closeEvent);

        if (tabCloseEventHandler != null) {
            tabCloseEventHandler.handle(closeEvent);
            event.consume();
        }
    }

    private void showAbout() {
        AboutWindow aboutWindow = new AboutWindow();
        setStylesheets(aboutWindow);
        aboutWindow.show();
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

    private void nextTab() {
        int currIndex = tabBar.getSelectionModel().getSelectedIndex();
        if (currIndex != tabBar.getTabs().size() - 1) {
            tabBar.getSelectionModel().select(currIndex + 1);
        }
    }

    private void prevTab() {
        int currIndex = tabBar.getSelectionModel().getSelectedIndex();
        if (currIndex != 0) {
            tabBar.getSelectionModel().select(currIndex - 1);
        }
    }

    private void undo() {
        CentralNode centralNode = getSelectedCentralNode();
        centralNode.undo();
    }

    private CentralNode getSelectedCentralNode() {
        Tab currTab = tabBar.getSelectionModel().getSelectedItem();
        return (CentralNode) currTab.getContent();
    }

    private void redo() {
        CentralNode centralNode = getSelectedCentralNode();
        centralNode.redo();
    }

    private void selectAll() {
        CentralNode centralNode = getSelectedCentralNode();
        centralNode.selectAll();
    }

    private void cut() {
        CentralNode centralNode = getSelectedCentralNode();
        centralNode.cut();
    }

    private void copy() {
        CentralNode centralNode = getSelectedCentralNode();
        centralNode.copy();
    }

    private void paste() {
        CentralNode centralNode = getSelectedCentralNode();
        centralNode.paste();
    }

    private void zoomIn() {
        for (Tab tab : tabBar.getTabs()) {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.zoomIn();
        }
    }

    private void zoomOut() {
        for (Tab tab : tabBar.getTabs()) {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.zoomOut();
        }
    }

    private void setUpTabBarEventHandlers() {
        tabBar.setOnDragOver(this::handleDragEvent);
        tabBar.setOnDragDropped(this::handleDropEvent);
        tabBar.focusedProperty().addListener(this::focusChanged);
        tabBar.getSelectionModel().selectedIndexProperty().addListener(this::tabSelectionChanged);
    }

    private void handleDragEvent(DragEvent event) {
        if (dragEventHandler != null) {
            dragEventHandler.handle(event);
        }
    }

    private void handleDropEvent(DragEvent event) {
        if (dropEventHandler != null) {
            dropEventHandler.handle(event);
        }
    }

    private void focusChanged(ObservableValue<? extends Boolean> observable,
                              Boolean wasFocused, Boolean isFocusedNow) {
        if (tabBar.getSelectionModel().getSelectedIndex() == -1) {
            return;
        }

        CentralNode centralNode = getSelectedCentralNode();
        restoreFocus(centralNode);
        setShortcutsAvailable(centralNode);
    }

    private void setShortcutsAvailable(CentralNode centralNode) {
        menuBar.setUndoState(centralNode.isUndoAvailable());
        menuBar.setRedoState(centralNode.isRedoAvailable());
    }

    private void restoreFocus(CentralNode centralNode) {
        if (inFocus == FieldInFocus.SOURCE) {
            centralNode.focusOn(CentralNode.inFocus.SOURCE);
        } else if (inFocus == FieldInFocus.TARGET) {
            centralNode.focusOn(CentralNode.inFocus.TARGET);
        }
    }

    private void tabSelectionChanged(ObservableValue<? extends Number> observable,
                                     Number oldIndex, Number newIndex) {
        if (oldIndex.intValue() == -1) {
            return;
        }

        int tabIndex = newIndex.intValue();
        menuBar.setPrevTabState(tabIndex > 0);
        menuBar.setNextTabState(tabIndex != tabBar.getTabs().size() - 1);

        CentralNode centralNode = getSelectedCentralNode();
        centralNode.focusOn(CentralNode.inFocus.SOURCE);
    }

    private void setUpToolBarEventHandlers() {
        toolBar.addSourceModeListener(this::sourceModeChanged);
        toolBar.addTargetModeListener(this::targetModeChanged);
        toolBar.addFocusListener(this::focusChanged);
    }

    private void sourceModeChanged(ObservableValue<? extends String> observable,
                                   String oldMode, String newMode) {
        int currIndex = tabBar.getSelectionModel().getSelectedIndex();
        sourceModeListeners.forEach(listener -> listener.changed(newMode, currIndex));
    }

    private void targetModeChanged(ObservableValue<? extends String> observable,
                                   String oldMode, String newMode) {
        int currIndex = tabBar.getSelectionModel().getSelectedIndex();
        targetModeListeners.forEach(listener -> listener.changed(newMode, currIndex));
    }

    private void ignoreTabBarKeyEvents() {
        tabBar.addEventFilter(KeyEvent.ANY, keyEvent -> {
            if ((keyEvent.getCode() == KeyCode.TAB
                    || keyEvent.getCode() == KeyCode.RIGHT
                    || keyEvent.getCode() == KeyCode.LEFT
                    || keyEvent.getCode() == KeyCode.UP
                    || keyEvent.getCode() == KeyCode.DOWN)
                    && tabBar.isFocused()) {
                keyEvent.consume();
            }
        });
    }

    /**
     * Sets conversion modes displayed in the toolbar.
     *
     * @param sourceModes list of source modes to set
     * @param targetModes list of target modes to set
     *
     * @throws IllegalStateException if either source modes list or target modes list is empty
     */
    public void setConversionModes(List<String> sourceModes,
                                   List<String> targetModes) {
        toolBar.setSourceModes(sourceModes);
        toolBar.setTargetModes(targetModes);
    }

    @Override
    public FieldInFocus inFocus(int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size());
        return inFocus;
    }

    private void checkRange(int tabIndex, int from, int to) {
        if (tabIndex < from || tabIndex >= to) {
            int endInd = (tabBar.getTabs().size() == 0) ? 0 : tabBar.getTabs().size() - 1;
            String errorMsg = "Invalid index: " + "(0:" + endInd + ") expected, " +
                    tabIndex + " provided";
            throw new IndexOutOfBoundsException(errorMsg);
        }
    }

    @Override
    public void setSourceMode(String mode) {
        toolBar.selectSourceMode(mode);
    }

    @Override
    public void setTargetMode(String mode) {
        toolBar.selectTargetMode(mode);
    }

    @Override
    public void setSourceText(String text, int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size());

        Tab tab = tabBar.getTabs().get(tabIndex);
        CentralNode centralNode = (CentralNode) tab.getContent();
        centralNode.setSourceText(text);
    }

    @Override
    public void setTargetText(String text, int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size());

        Tab tab = tabBar.getTabs().get(tabIndex);
        CentralNode centralNode = (CentralNode) tab.getContent();
        centralNode.setTargetText(text);
    }

    @Override
    public void setWindowTitle(String title) {
        setTitle(title);
    }

    @Override
    public void setTabTitle(String title, int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size());

        Tab tab = tabBar.getTabs().get(tabIndex);
        tab.setText(title);
        tabTitleListeners.forEach(listener -> listener.changed(title, tabIndex));
    }

    @Override
    public void setCurrTabIndex(int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size());
        tabBar.getSelectionModel().select(tabIndex);
    }

    @Override
    public void setStatusBarPolicy(StatusBarPolicy policy) {
        if (policy == StatusBarPolicy.SHOW) {
            addStatusBar();
        } else if (policy == StatusBarPolicy.DO_NOT_SHOW) {
            removeStatusBar();
        }
    }

    private void addStatusBar() {
        if (layout.getChildren().contains(statusBar)) {
            return;
        }

        layout.setBottom(statusBar);
        menuBar.setStatusBarSelected(true);
    }

    private void removeStatusBar() {
        layout.getChildren().remove(statusBar);
        menuBar.setStatusBarSelected(false);
    }

    @Override
    public void setTargetFieldPolicy(TargetFieldPolicy policy) {
        targetFieldPolicy = policy;
        if (policy == TargetFieldPolicy.ALWAYS) {
            showTargetFieldInEveryTab();
        } else if (policy == TargetFieldPolicy.AS_NEEDED) {
            hideTargetFieldInEveryTab();
        }
    }

    private void showTargetFieldInEveryTab() {
        tabBar.getTabs().forEach(tab -> {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.setTargetFieldPolicy(CentralNode.TargetFieldPolicy.ALWAYS);
        });

        menuBar.setTargetFieldSelected(true);
    }

    private void hideTargetFieldInEveryTab() {
        tabBar.getTabs().forEach(tab -> {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.setTargetFieldPolicy(CentralNode.TargetFieldPolicy.AS_NEEDED);
        });

        menuBar.setTargetFieldSelected(false);
    }

    @Override
    public void setWrappingPolicy(WrappingPolicy policy) {
        wrappingPolicy = policy;
        if (policy == WrappingPolicy.WRAP_LINES) {
            wrapLinesInEveryTab();
        } else if (policy == WrappingPolicy.NO_WRAP) {
            doNotWrapLinesInEveryTab();
        }
    }

    private void wrapLinesInEveryTab() {
        tabBar.getTabs().forEach(tab -> {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.setWrappingPolicy(CodeEditor.WrappingPolicy.WRAP_LINES);
        });

        menuBar.setWrappingSelected(true);
    }

    private void doNotWrapLinesInEveryTab() {
        tabBar.getTabs().forEach(tab -> {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.setWrappingPolicy(CodeEditor.WrappingPolicy.NO_WRAP);
        });

        menuBar.setWrappingSelected(false);
    }

    @Override
    public void setHighlighterPolicy(HighlighterPolicy policy) {
        highlighterPolicy = policy;
        if (policy == HighlighterPolicy.HIGHLIGHT) {
            highlightInEveryTab();
        } else if (policy == HighlighterPolicy.DO_NOT_HIGHLIGHT) {
            doNotHighlightInEveryTab();
        }
    }

    private void highlightInEveryTab() {
        tabBar.getTabs().forEach(tab -> {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.setHighlighterPolicy(CodeEditor.HighlighterPolicy.HIGHLIGHT);
        });

        menuBar.setHighlighterSelected(true);
    }

    private void doNotHighlightInEveryTab() {
        tabBar.getTabs().forEach(tab -> {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.setHighlighterPolicy(CodeEditor.HighlighterPolicy.DO_NOT_HIGHLIGHT);
        });

        menuBar.setHighlighterSelected(false);
    }

    @Override
    public void setLineNumbersPolicy(LineNumbersPolicy policy) {
        lineNumbersPolicy = policy;
        if (policy == LineNumbersPolicy.SHOW) {
            showLineNumbersInEveryTab();
        } else if (policy == LineNumbersPolicy.DO_NOT_SHOW) {
            hideLineNumbersInEveryTab();
        }
    }

    private void showLineNumbersInEveryTab() {
        tabBar.getTabs().forEach(tab -> {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.setLineNumbersPolicy(CodeEditor.LineNumbersPolicy.SHOW);
        });

        statusBar.showLineColumnNumberArea(true);
        menuBar.setLineNumbersSelected(true);
    }

    private void hideLineNumbersInEveryTab() {
        tabBar.getTabs().forEach(tab -> {
            CentralNode centralNode = (CentralNode) tab.getContent();
            centralNode.setLineNumbersPolicy(CodeEditor.LineNumbersPolicy.DO_NOT_SHOW);
        });

        statusBar.showLineColumnNumberArea(false);
        menuBar.setLineNumbersSelected(false);
    }

    @Override
    public void openTab(int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size() + 1);
        createNewTab(tabIndex);
        notifyModeListeners(tabIndex);
        setClosablePolicyInEveryTab();
    }

    private void createNewTab(int tabIndex) {
        Tab newTab = new Tab();
        createCentralNode(newTab);
        setTabTitle(newTab, tabIndex);
        newTab.setOnCloseRequest(event -> handleTabCloseEvent(newTab, event));

        tabBar.getTabs().add(tabIndex, newTab);
        tabBar.getSelectionModel().select(tabIndex);
    }

    private void setTabTitle(Tab tab, int tabIndex) {
        List<Integer> tabNumbers = getTabNumbers();
        String title = getNextTabTile(tabNumbers);
        tab.setText(title);
        tabTitleListeners.forEach(listener -> listener.changed(title, tabIndex));
    }

    private List<Integer> getTabNumbers() {
        List<Integer> tabNumbers = new LinkedList<>();
        tabBar.getTabs().forEach(item -> {
            StringBuilder tabTitle = new StringBuilder(item.getText());
            tabTitle.delete(0, 4); // Delete "Tab "
            try {
                int tabNumber = Integer.parseInt(tabTitle.toString());
                tabNumbers.add(tabNumber);  // The tab has a standard title "Tab i"
            } catch (NumberFormatException ignored) {
                // The tab had a custom title. Skip
            }
        });

        return tabNumbers;
    }

    private String getNextTabTile(List<Integer> tabNumbers) {
        Collections.sort(tabNumbers);
        int i = 0;
        for (; i < tabNumbers.size(); i++) {
            if (tabNumbers.get(i) != i + 1) {
                break;
            }
        }

        return  "Tab " + (i + 1);
    }

    private void createCentralNode(Tab tab) {
        CentralNode centralNode = new CentralNode();
        setUpCentralNode(tab, centralNode);
        tab.setContent(centralNode);
    }

    private void setUpCentralNode(Tab tab, CentralNode centralNode) {
        setTargetFieldPolicy(centralNode);
        setWrappingPolicy(centralNode);
        setHighlightingPolicy(centralNode);
        setLineNumbersPolicy(centralNode);
        setUpEventListeners(centralNode, tab);
    }

    private void setTargetFieldPolicy(CentralNode centralNode) {
        if (targetFieldPolicy == TargetFieldPolicy.ALWAYS) {
            centralNode.setTargetFieldPolicy(CentralNode.TargetFieldPolicy.ALWAYS);
        } else if (targetFieldPolicy == TargetFieldPolicy.AS_NEEDED) {
            centralNode.setTargetFieldPolicy(CentralNode.TargetFieldPolicy.AS_NEEDED);
        }
    }

    private void setWrappingPolicy(CentralNode centralNode) {
        if (wrappingPolicy == WrappingPolicy.WRAP_LINES) {
            centralNode.setWrappingPolicy(CodeEditor.WrappingPolicy.WRAP_LINES);
        } else if (wrappingPolicy == WrappingPolicy.NO_WRAP) {
            centralNode.setWrappingPolicy(CodeEditor.WrappingPolicy.NO_WRAP);
        }
    }

    private void setHighlightingPolicy(CentralNode centralNode) {
        if (highlighterPolicy == HighlighterPolicy.HIGHLIGHT) {
            centralNode.setHighlighterPolicy(CodeEditor.HighlighterPolicy.HIGHLIGHT);
        } else if (highlighterPolicy == HighlighterPolicy.DO_NOT_HIGHLIGHT) {
            centralNode.setHighlighterPolicy(CodeEditor.HighlighterPolicy.DO_NOT_HIGHLIGHT);
        }
    }

    private void setLineNumbersPolicy(CentralNode centralNode) {
        if (lineNumbersPolicy == LineNumbersPolicy.SHOW) {
            centralNode.setLineNumbersPolicy(CodeEditor.LineNumbersPolicy.SHOW);
        } else if (lineNumbersPolicy == LineNumbersPolicy.DO_NOT_SHOW) {
            centralNode.setLineNumbersPolicy(CodeEditor.LineNumbersPolicy.DO_NOT_SHOW);
        }
    }

    private void setUpEventListeners(CentralNode centralNode, Tab tab) {
        centralNode.addSourceTextListener((o, oldText, newText) ->
                handleSourceTextChangeEvent(centralNode, tab, newText));
        centralNode.addTargetTextListener((o, oldText, newText) ->
                handleTargetTextChangeEvent(centralNode, tab, newText));
        centralNode.addSourceLineIndexListener((o, oldNum, newNum) -> lineNumberChanged(newNum));
        centralNode.addSourceColumnIndexListener((o, oldNum, newNum) -> columnNumberChanged(newNum));
        centralNode.addTargetLineIndexListener((o, oldNum, newNum) -> lineNumberChanged(newNum));
        centralNode.addTargetColumnIndexListener((o, oldNum, newNum) -> columnNumberChanged(newNum));
        centralNode.addSourceFocusListener((o, wasFocused, isFocused) -> {
            if (isFocused) {
                focusInTabChanged(centralNode, FieldInFocus.SOURCE);
            }
        });
        centralNode.addTargetFocusListener((o, wasFocused, isFocused) -> {
            if (isFocused) {
                focusInTabChanged(centralNode, FieldInFocus.TARGET);
            }
        });
    }

    private void handleSourceTextChangeEvent(CentralNode centralNode, Tab tab, String newText) {
        setShortcutsAvailable(centralNode);
        int tabIndex = tabBar.getTabs().indexOf(tab);
        sourceTextListeners.forEach(listener -> listener.changed(newText, tabIndex));
    }

    private void handleTargetTextChangeEvent(CentralNode centralNode, Tab tab, String newText) {
        setShortcutsAvailable(centralNode);
        int tabIndex = tabBar.getTabs().indexOf(tab);
        targetTextListeners.forEach(listener -> listener.changed(newText, tabIndex));
    }

    private void lineNumberChanged(int newNumber) {
        int lineNumber = newNumber + 1;
        statusBar.setLineNumber(lineNumber);
    }

    private void columnNumberChanged(int newNumber) {
        int columnNumber = newNumber + 1;
        statusBar.setColumnNumber(columnNumber);
    }

    private void focusInTabChanged(CentralNode centralNode, FieldInFocus fieldInFocus) {
        inFocus = fieldInFocus;

        statusBar.setLineNumber(getLineIndexInFocusedField() + 1);
        statusBar.setColumnNumber(getColumnIndexInFocusedField() + 1);

        setShortcutsAvailable(centralNode);
        notifyFocusListeners(inFocus, tabBar.getSelectionModel().getSelectedIndex());
    }

    private int getLineIndexInFocusedField() {
        CentralNode centralNode = getSelectedCentralNode();
        if (inFocus == FieldInFocus.SOURCE) {
            return centralNode.getSourceLineIndex();
        } else {
            return centralNode.getTargetLineIndex();
        }
    }

    private int getColumnIndexInFocusedField() {
        CentralNode centralNode = getSelectedCentralNode();
        if (inFocus == FieldInFocus.SOURCE) {
            return centralNode.getSourceColumnIndex();
        } else {
            return centralNode.getTargetColumnIndex();
        }
    }

    private void notifyFocusListeners(FieldInFocus inFocus, int tabIndex) {
        focusListeners.forEach(listener -> listener.changed(inFocus, tabIndex));
    }

    private void handleTabCloseEvent(Tab tab, Event event) {
        int index = tabBar.getTabs().indexOf(tab);
        TabCloseEvent closeEvent = new TabCloseEvent(index);
        tabBar.fireEvent(closeEvent);

        if (tabCloseEventHandler != null) {
            tabCloseEventHandler.handle(closeEvent);
            event.consume();
        }
    }

    private void setClosablePolicyInEveryTab() {
        int tabsNumber = tabBar.getTabs().size();
        menuBar.setCloseTabState(tabsNumber != 1);
        tabBar.getTabs().forEach(tab -> tab.setClosable(tabsNumber != 1));
    }

    private void notifyModeListeners(int tabIndex) {
        sourceModeListeners.forEach(listener -> listener.changed(toolBar.getSourceMode(), tabIndex));
        targetModeListeners.forEach(listener -> listener.changed(toolBar.getTargetMode(), tabIndex));
    }

    @Override
    public void closeTab(int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size());
        tabBar.getTabs().remove(tabIndex);
        setClosablePolicyInEveryTab();

        if (tabIndex != 0) {
            CentralNode centralNode = getSelectedCentralNode();
            centralNode.focusOn(CentralNode.inFocus.SOURCE);
        }
    }

    @Override
    public void closeAllTabs() {
        tabBar.getTabs().clear();
    }

    @Override
    public void showConversionStart(int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size());

        CentralNode centralNode = getSelectedCentralNode();
        centralNode.setDisable(true);
    }

    @Override
    public void showConversionEnd(int tabIndex) {
        checkRange(tabIndex, 0, tabBar.getTabs().size());

        CentralNode centralNode = getSelectedCentralNode();
        centralNode.setDisable(false);
        centralNode.focusOn(CentralNode.inFocus.TARGET);
    }

    @Override
    public void showError(String cause, String errorMsg) {
        ErrorWindow errorWindow = new ErrorWindow(cause, errorMsg);
        setStylesheets(errorWindow);
        errorWindow.show();
    }

    @Override
    public void showFilePath(String filePath) {
        statusBar.setFilePath(filePath);
    }

    @Override
    public Optional<List<File>> choseFilesToOpen() {
        FileChooser chooser = new FileChooser();
        List<File> files = chooser.showOpenMultipleDialog(this);
        return Optional.ofNullable(files);
    }

    @Override
    public Optional<List<File>> choseFilesToOpen(File initialDir) {
        FileChooser chooser = new FileChooser();
        chooser.setInitialDirectory(initialDir);
        List<File> files = chooser.showOpenMultipleDialog(this);
        return Optional.ofNullable(files);
    }

    @Override
    public Optional<String> choseFileSavingLocation() {
        FileChooser chooser = new FileChooser();
        File dir = chooser.showSaveDialog(this);
        return Optional.ofNullable(dir)
                .map(File::getAbsolutePath);
    }

    @Override
    public void addRecentFile(String filePath) {
        menuBar.addRecentFile(filePath);
        menuBar.setOpenRecentState(true);
    }

    @Override
    public void clearRecentFiles() {
        menuBar.clearRecentFiles();
        menuBar.setOpenRecentState(false);
    }

    @Override
    public void moveRecentFile(String filePath, int moveTo) {
        menuBar.moveRecentFile(filePath, moveTo);
    }

    @Override
    public void addTabSelectionListener(ChangeListener<Number> listener) {
        tabBar.getSelectionModel().selectedIndexProperty().addListener(listener);
    }

    @Override
    public void removeTabSelectionListener(ChangeListener<Number> listener) {
        tabBar.getSelectionModel().selectedIndexProperty().removeListener(listener);
    }

    @Override
    public void addTabTitleListener(TabTitleChangeListener listener) {
        tabTitleListeners.add(listener);
    }

    @Override
    public void removeTabTitleListener(TabTitleChangeListener listener) {
        tabTitleListeners.remove(listener);
    }

    @Override
    public void addSourceTextListener(TextChangeListener listener) {
        sourceTextListeners.add(listener);
    }

    @Override
    public void removeSourceTextListener(TextChangeListener listener) {
        sourceTextListeners.remove(listener);
    }

    @Override
    public void addTargetTextListener(TextChangeListener listener) {
        targetTextListeners.add(listener);
    }

    @Override
    public void removeTargetTextListener(TextChangeListener listener) {
        targetTextListeners.remove(listener);
    }

    @Override
    public void addSourceModeListener(ModeChangeListener listener) {
        sourceModeListeners.add(listener);
    }

    @Override
    public void removeSourceModeListener(ModeChangeListener listener) {
        sourceModeListeners.remove(listener);
    }

    @Override
    public void addTargetModeListener(ModeChangeListener listener) {
        targetModeListeners.add(listener);
    }

    @Override
    public void removeTargetModeListener(ModeChangeListener listener) {
        targetModeListeners.remove(listener);
    }

    @Override
    public void addFocusListener(FocusChangeListener listener) {
        focusListeners.add(listener);
    }

    @Override
    public void setOnCloseAction(EventHandler<WindowEvent> action) {
        setOnCloseRequest(action);
    }

    @Override
    public void setOnDragAction(EventHandler<DragEvent> action) {
        dragEventHandler = action;
    }

    @Override
    public void setOnDropAction(EventHandler<DragEvent> action) {
        dropEventHandler = action;
    }

    @Override
    public void setOnPreferencesAction(EventHandler<ActionEvent> action) {
        menuBar.setOnPreferencesAction(action);
    }

    @Override
    public void setOnNewTabAction(EventHandler<ActionEvent> action) {
        menuBar.setOnNewTabAction(action);
        toolBar.setOnNewTabAction(action);
    }

    @Override
    public void setOnCloseTabAction(EventHandler<TabCloseEvent> action) {
        tabCloseEventHandler = action;
    }

    @Override
    public void setOnOpenFileAction(EventHandler<ActionEvent> action) {
        menuBar.setOnOpenFileAction(action);
        toolBar.setOnOpenFileAction(action);
    }

    @Override
    public void setOnRecentFileAction(EventHandler<RecentFileEvent> action) {
        menuBar.setOnOpenRecentAction(action);
    }

    @Override
    public void setOnClearRecentAction(EventHandler<ActionEvent> action) {
        menuBar.setOnClearRecentAction(action);
    }

    @Override
    public void setOnSaveFileAction(EventHandler<ActionEvent> action) {
        menuBar.setOnSaveFileAction(action);
        toolBar.setOnSaveFileAction(action);
    }

    @Override
    public void setOnSaveAsAction(EventHandler<ActionEvent> action) {
        menuBar.setOnSaveAsAction(action);
    }

    @Override
    public void setOnRunAction(EventHandler<ActionEvent> action) {
        menuBar.setOnRunAction(action);
        toolBar.setOnRunAction(action);
    }

    @Override
    public void setOnOnlineHelpAction(EventHandler<ActionEvent> action) {
        menuBar.setOnOnlineHelpAction(action);
    }

    @Override
    public void setOnOpenSiteAction(EventHandler<ActionEvent> action) {
        menuBar.setOnOpenSiteAction(action);
    }

    @Override
    public void setOnStatusBarAction(EventHandler<ActionEvent> action) {
        menuBar.setOnStatusBarAction(action);
    }

    @Override
    public void setOnTargetFieldAction(EventHandler<ActionEvent> action) {
        menuBar.setOnTargetFieldAction(action);
    }

    @Override
    public void setOnWrappingAction(EventHandler<ActionEvent> action) {
        menuBar.setOnWrappingAction(action);
    }

    @Override
    public void setOnHighlighterAction(EventHandler<ActionEvent> action) {
        menuBar.setOnHighlighterAction(action);
    }

    @Override
    public void setOnLineNumbersAction(EventHandler<ActionEvent> action) {
        menuBar.setOnLineNumbersAction(action);
    }
}
