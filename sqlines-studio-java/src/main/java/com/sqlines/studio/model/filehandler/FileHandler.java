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

package com.sqlines.studio.model.filehandler;

import com.sqlines.studio.model.tabsdata.ObservableTabsData;
import com.sqlines.studio.model.tabsdata.listener.TabsChangeListener;
import com.sqlines.studio.model.filehandler.listener.RecentFilesChangeListener;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.function.Consumer;
import java.util.stream.Stream;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Allows you to open files. Contains the list of recent files.
 * <p>
 * Updates the tab data if the data in the opened files have been changed.
 *
 * @apiNote Use {@link Runnable#run()} to start file verification.
 *
 * @see RecentFilesChangeListener
 */
public class FileHandler implements Runnable, Serializable {
    private static final Logger logger = LogManager.getLogger(FileHandler.class);
    private static final long serialVersionUID = 646756239;

    private ObservableTabsData tabsData;
    private List<Long> sourceFilesLastModified = new ArrayList<>();
    private List<Long> targetFilesLastModified = new ArrayList<>();
    private List<File> recentFiles = new ArrayList<>();

    private List<RecentFilesChangeListener> recentFilesListeners = new ArrayList<>(5);

    /**
     * Starts file verification.
     * Updates the tab data if the data in the files have been changed.
     */
    @Override
    @SuppressWarnings("BusyWait")
    public void run() {
        while (true) {
            try {
                monitorFileChanged();
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                break;
            } catch (Exception e) {
                logger.error("File verification: " + e.getMessage());
                break;
            }
        }
    }

    private synchronized void monitorFileChanged() {
        monitorSourceFiles();
        monitorTargetFiles();
    }

    private void monitorSourceFiles() {
        for (int i = 0; i < sourceFilesLastModified.size(); i++) {
            String filePath = tabsData.getSourceFilePath(i);
            long lastModified = sourceFilesLastModified.get(i);
            monitorSourceFile(i, filePath, lastModified);
        }
    }

    private void monitorSourceFile(int index, String path, long lastModified) {
        Consumer<Runnable> update = action -> {
            logger.info("Updating source file: " + path);
            action.run();
            logger.info("Source file updated: " + path);
        };

        if (fileWasDeleted(path)) {
            update.accept(() -> resetSourceFile(index));
        } else if (fileWasUpdated(path, lastModified)) {
            update.accept(() -> updateSourceTabData(path, index));
        }
    }

    private boolean fileWasDeleted(String filePath) {
        File file = new File(filePath);
        return !filePath.isEmpty() && !file.exists();
    }

    private void resetSourceFile(int index) {
        tabsData.setSourceFilePath("", index);
        sourceFilesLastModified.set(index, 0L);
    }

    private boolean fileWasUpdated(String filePath, long lastModified) {
        File file = new File(filePath);
        if (!file.exists()) {
            return false;
        }

        return file.lastModified() != lastModified;
    }

    private void updateSourceTabData(String filePath, int tabIndex) {
        try {
            String data = readFromFile(filePath);
            tabsData.setSourceText(data, tabIndex);

            File file = new File(filePath);
            sourceFilesLastModified.set(tabIndex, file.lastModified());
        } catch (Exception e) {
            logger.error("Updating source file: " + e.getMessage());
        }
    }

    private String readFromFile(String path) throws IOException {
        try (FileInputStream stream = new FileInputStream(path)) {
            return new String(stream.readAllBytes(), StandardCharsets.UTF_8);
        }
    }

    private void monitorTargetFiles() {
        for (int i = 0; i < targetFilesLastModified.size(); i++) {
            String filePath = tabsData.getTargetFilePath(i);
            long lastModified = targetFilesLastModified.get(i);
            monitorTargetFile(i, filePath, lastModified);
        }
    }

    private void monitorTargetFile(int index, String path, long lastModified) {
        Consumer<Runnable> update = action -> {
            logger.info("Updating target file: " + path);
            action.run();
            logger.info("Target file updated: " + path);
        };

        if (fileWasDeleted(path)) {
            update.accept(() -> resetTargetFile(index));
        } else if (fileWasUpdated(path, lastModified)) {
            update.accept(() -> updateTargetTabData(path, index));
        }
    }

    private void resetTargetFile(int index) {
        tabsData.setTargetFilePath("", index);
        targetFilesLastModified.set(index, 0L);
    }

    private void updateTargetTabData(String filePath, int tabIndex) {
        try {
            String data = readFromFile(filePath);
            tabsData.setTargetText(data, tabIndex);

            File file = new File(filePath);
            targetFilesLastModified.set(tabIndex, file.lastModified());
        } catch (Exception e) {
            logger.error("Updating target file: " + e.getMessage());
        }
    }

    /**
     * Sets the underlying source of the tab data.
     *
     * @param tabsData source of tab data to set
     */
    public synchronized void setTabsData(ObservableTabsData tabsData) {
        this.tabsData = tabsData;
        tabsData.addTabsListener(this::tabsDataChanged);
    }

    private synchronized void tabsDataChanged(TabsChangeListener.Change change) {
        if (change.getChangeType() == TabsChangeListener.Change.ChangeType.TAB_ADDED) {
            handleTabAddEvent(change);
        } else if (change.getChangeType() == TabsChangeListener.Change.ChangeType.TAB_REMOVED) {
            handleTabCloseEvent(change);
        }
    }

    private void handleTabAddEvent(TabsChangeListener.Change change) {
        int tabIndex = change.getTabIndex();
        addSourceFile(tabsData.getSourceFilePath(tabIndex), tabIndex);
        addTargetFile(tabsData.getTargetFilePath(tabIndex), tabIndex);
    }

    private void handleTabCloseEvent(TabsChangeListener.Change change) {
        int tabIndex = change.getTabIndex();
        sourceFilesLastModified.remove(tabIndex);
        targetFilesLastModified.remove(tabIndex);
    }

    private void addSourceFile(String filePath, int index) {
        File file = new File(filePath);
        sourceFilesLastModified.add(index, file.lastModified());
    }

    private void addTargetFile(String filePath, int index) {
        File file = new File(filePath);
        targetFilesLastModified.add(index, file.lastModified());
    }

    /**
     * Opens files and updates tabs data.
     * Adds all the files to the recent files list. If a file is already
     * in the recent files list, moves it to the top of the list.
     * <p>
     * Notifies all {@link RecentFilesChangeListener} listeners of the occurred change.
     *
     * @param files files to open
     *
     * @throws FileNotFoundException if the file does not exist, is a directory rather than
     * a regular file, or for some other reason cannot be opened for reading.
     * @throws IOException if any IO error occurred
     * @throws SecurityException if a security manager exists and its checkRead
     * method denies read access to the file.
     */
    public synchronized void openSourceFiles(List<File> files) throws IOException {
        files.removeIf(File::isDirectory);
        for (File file : files) {
            openFile(file);
        }
    }

    private void openFile(File file) throws IOException {
        int currIndex = tabsData.getCurrTabIndex();
        if (!tabsData.getSourceText(currIndex).isEmpty()) {
            tabsData.openTab(currIndex + 1);
            tabsData.setCurrTabIndex(currIndex + 1);
            currIndex = tabsData.getCurrTabIndex();
        }

        String text = readFromFile(file.getAbsolutePath());
        tabsData.setSourceText(text, currIndex);
        tabsData.setSourceFilePath(file.getAbsolutePath(), currIndex);
        tabsData.setTabTitle(file.getName(), currIndex);
        sourceFilesLastModified.set(currIndex, file.lastModified());

        saveLastOpenedDir(file.getParentFile().getAbsolutePath());
        changeRecentFiles(file);
    }

    private void saveLastOpenedDir(String dir) {
        System.getProperties().setProperty("model.last-dir", dir);
    }

    private void changeRecentFiles(File file) {
        if (!recentFiles.contains(file)) {
            recentFiles.add(file);
            notifyRecentFileAddListeners(file.getAbsolutePath(), recentFiles.indexOf(file));
        } else {
            int from = recentFiles.indexOf(file);
            recentFiles.remove(file);
            recentFiles.add(0, file);
            notifyRecentFileMoveListeners(file.getAbsolutePath(), from, 0);
        }
    }

    private void notifyRecentFileAddListeners(String filePath, int index) {
        RecentFilesChangeListener.Change change = new RecentFilesChangeListener.Change(
                RecentFilesChangeListener.Change.ChangeType.FILE_ADDED, filePath, index
        );
        recentFilesListeners.forEach(listener -> listener.onChange(change));
    }

    private void notifyRecentFileMoveListeners(String filePath, int movedFrom, int movedTo) {
        RecentFilesChangeListener.Change change = new RecentFilesChangeListener.Change(
                RecentFilesChangeListener.Change.ChangeType.FILE_MOVED, filePath, movedFrom, movedTo
        );
        recentFilesListeners.forEach(listener -> listener.onChange(change));
    }

    /**
     * Saves source data from the specified tab from {@link ObservableTabsData} to
     * the source file from the specified tab from {@link ObservableTabsData}.
     *
     * @param tabIndex the index of the tab with source data and source file path
     *
     * @throws IndexOutOfBoundsException – if the index is out of range
     * (tabIndex < 0 || tabIndex >= {@link ObservableTabsData#countTabs()})
     * @throws IllegalStateException if there is no source file opened
     * @throws IOException if any I/O error occurred
     * @throws SecurityException if a security manager exists and its checkWrite method
     * denies write access to the file
     */
    public synchronized void saveSourceFile(int tabIndex) throws IOException {
        File file = new File(tabsData.getSourceFilePath(tabIndex));
        checkFilePresence(file);
        writeToFile(file, tabsData.getSourceText(tabIndex));
        tabsData.setTabTitle(file.getName(), tabIndex);
        sourceFilesLastModified.set(tabIndex, file.lastModified());
    }

    private void checkFilePresence(File file) {
        if (!file.exists()) {
            throw new IllegalStateException("File does not exist: " + file.getAbsolutePath());
        }
    }

    private void writeToFile(File file, String data) throws IOException {
        try (FileOutputStream stream = new FileOutputStream(file.getAbsoluteFile())) {
            stream.write(data.getBytes());
        }
    }

    /**
     * Saves target data from the specified tab from {@link ObservableTabsData} to
     * the target file from the specified tab from {@link ObservableTabsData}.
     *
     * @param tabIndex the index of the tab with target data and target file path
     *
     * @throws IndexOutOfBoundsException – if the index is out of range
     * (tabIndex < 0 || tabIndex >= {@link ObservableTabsData#countTabs()})
     * @throws IllegalStateException if there is no target file opened
     * @throws FileNotFoundException if the file exists but is a directory rather than a regular file,
     * does not exist but cannot be created, or cannot be opened for any other reason
     * @throws IOException if any I/O error occurred
     * @throws SecurityException if a security manager exists and its checkWrite method
     * denies write access to the file
     */
    public synchronized void saveTargetFile(int tabIndex) throws IOException {
        File file = new File(tabsData.getTargetFilePath(tabIndex));
        checkFilePresence(file);
        writeToFile(file, tabsData.getTargetText(tabIndex));
        targetFilesLastModified.set(tabIndex, file.lastModified());
    }

    /**
     * Creates a new file with the specified file path. Writes source data from the specified tab
     * from {@link ObservableTabsData} to this file.
     *
     * @param tabIndex the index of the tab with source data
     * @param path file path

     * @throws IndexOutOfBoundsException – if the index is out of range
     * (tabIndex < 0 || tabIndex >= {@link ObservableTabsData#countTabs()})
     * @throws FileNotFoundException if the file exists but is a directory rather than a regular file,
     * does not exist but cannot be created, or cannot be opened for any other reason
     * @throws IOException if some I/O error occurred
     * @throws SecurityException if a security manager exists and its checkWrite method
     * denies write access to the file
     */
    public synchronized void saveSourceFileAs(int tabIndex, String path) throws IOException  {
        File file = new File(path);
        writeToFile(file, tabsData.getSourceText(tabIndex));
        tabsData.setSourceFilePath(path, tabIndex);
        tabsData.setTabTitle(file.getName(), tabIndex);
        sourceFilesLastModified.set(tabIndex, file.lastModified());
    }

    /**
     * Creates a new file with the specified file path. Writes target data from the specified tab
     * from {@link ObservableTabsData} to this file.
     *
     * @param tabIndex the index of the tab with target data
     * @param path file path

     * @throws IndexOutOfBoundsException – if the index is out of range
     * (tabIndex < 0 || tabIndex >= {@link ObservableTabsData#countTabs()})
     * @throws FileNotFoundException if the file exists but is a directory rather than a regular file,
     * does not exist but cannot be created, or cannot be opened for any other reason
     * @throws IOException if some I/O error occurred
     * @throws SecurityException if a security manager exists and its checkWrite method
     * denies write access to the file
     */
    public synchronized void saveTargetFileAs(int tabIndex, String path) throws IOException  {
        File file = new File(path);
        writeToFile(file, tabsData.getTargetText(tabIndex));
        tabsData.setTargetFilePath(path, tabIndex);
        targetFilesLastModified.set(tabIndex, file.lastModified());
    }

    /**
     * @param index the index of the recent file to get
     *
     * @return the specified recent file
     */
    public String getRecentFile(int index) {
        return recentFiles.get(index).getAbsolutePath();
    }

    /**
     * Removes all recent files from the list of recent files.
     */
    public void clearRecentFiles() {
        Stream<String> paths = recentFiles.stream().map(File::getAbsolutePath);
        recentFiles.clear();
        paths.forEach(this::notifyRecentFileListenersOfDelete);
    }

    private void notifyRecentFileListenersOfDelete(String path) {
        RecentFilesChangeListener.Change change = new RecentFilesChangeListener.Change(
                RecentFilesChangeListener.Change.ChangeType.FILE_REMOVED, path, 0
        );
        recentFilesListeners.forEach(listener -> listener.onChange(change));
    }

    /**
     * @return the number of recent files in the list of recent files
     */
    public int countRecentFiles() {
        return recentFiles.size();
    }

    /**
     * Adds a listener which will be notified when the list of recent files changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addRecentFileListener(RecentFilesChangeListener listener) {
        recentFilesListeners.add(listener);
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }

        if (other == null || getClass() != other.getClass()) {
            return false;
        }

        FileHandler that = (FileHandler) other;
        return Objects.equals(sourceFilesLastModified, that.sourceFilesLastModified)
                && Objects.equals(targetFilesLastModified, that.targetFilesLastModified)
                && Objects.equals(recentFiles, that.recentFiles);
    }

    @Override
    public int hashCode() {
        return Objects.hash(sourceFilesLastModified, targetFilesLastModified, recentFiles);
    }

    private synchronized void readObject(ObjectInputStream stream)
            throws ClassNotFoundException, IOException {
        initFields();
        int sourceFilesNumber = stream.readInt();
        for (int i = 0 ; i < sourceFilesNumber; i++) {
            Long data = (Long) stream.readObject();
            sourceFilesLastModified.add(data);
        }

        int targetFilesNumber = stream.readInt();
        for (int i = 0 ; i < targetFilesNumber; i++) {
            Long data = (Long) stream.readObject();
            targetFilesLastModified.add(data);
        }

        int recentFilesNumber = stream.readInt();
        for (int i = 0 ; i < recentFilesNumber; i++) {
            File data = (File) stream.readObject();
            recentFiles.add(data);
        }
    }

    private void initFields() {
        sourceFilesLastModified = new ArrayList<>();
        targetFilesLastModified = new ArrayList<>();
        recentFiles = new ArrayList<>();
        recentFilesListeners = new ArrayList<>();
    }

    private synchronized void writeObject(ObjectOutputStream stream) throws IOException {
        stream.writeInt(sourceFilesLastModified.size());
        for (Long data : sourceFilesLastModified) {
            stream.writeObject(data);
        }

        stream.writeInt(targetFilesLastModified.size());
        for (Long data : targetFilesLastModified) {
            stream.writeObject(data);
        }

        stream.writeInt(recentFiles.size());
        for (File data : recentFiles) {
            stream.writeObject(data);
        }
    }
}
