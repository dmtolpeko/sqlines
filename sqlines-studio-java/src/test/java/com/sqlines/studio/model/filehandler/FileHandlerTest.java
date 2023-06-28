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

import com.sqlines.studio.model.filehandler.listener.RecentFilesChangeListener;
import com.sqlines.studio.model.tabsdata.ObservableTabsData;

import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import static org.hamcrest.CoreMatchers.equalTo;
import static org.hamcrest.CoreMatchers.not;
import static org.junit.Assert.*;

public class FileHandlerTest {
    private ObservableTabsData tabsData;
    private FileHandler fileHandler;

    @Before
    public void setUp() {
        tabsData = new ObservableTabsData();
        fileHandler = new FileHandler();
        fileHandler.setTabsData(tabsData);

        tabsData.openTab(0);
        tabsData.setCurrTabIndex(0);
    }

    @Test
    public void shouldModifyTabsDataWhenOpeningFiles() throws IOException {
        File file1 = new File(getClass().getResource("/srcFile1.sql").getPath());
        File file2 = new File(getClass().getResource("/srcFile2.sql").getPath());
        List<File> mutableList = new ArrayList<>(List.of(file1, file2));

        fileHandler.openSourceFiles(mutableList);

        String firstData = "CREATE TABLE t1 (jdoc JSON);";
        assertThat(tabsData.getSourceText(0), equalTo(firstData));

        String secondData = "CREATE OR REPLACE PROCEDURE SP_VIEW_TRADE_ALLOCATION";
        assertThat(tabsData.getSourceText(1), equalTo(secondData));
    }

    @Test
    public void shouldUpdateFileWhenSavingFile() throws IOException {
        File file = new File(getClass().getResource("/srcFile1.sql").getPath());
        fileHandler.openSourceFiles(new ArrayList<>(List.of(file)));
        long lastModified = file.lastModified();

        fileHandler.saveSourceFile(0);

        assertThat(lastModified, not(equalTo(file.lastModified())));
    }

    @Test
    public void shouldNotifyWhenOpeningFiles() throws IOException {
        AtomicBoolean notified = new AtomicBoolean(false);
        fileHandler.addRecentFileListener(change -> {
            if (change.getChangeType() == RecentFilesChangeListener.Change.ChangeType.FILE_ADDED) {
                notified.set(true);
            }
        });

        File file = new File(getClass().getResource("/srcFile1.sql").getPath());
        fileHandler.openSourceFiles(new ArrayList<>(List.of(file)));

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldWriteToFileWhenSerialized() throws IOException {
        File file1 = new File(getClass().getResource("/srcFile1.sql").getPath());
        File file2 = new File(getClass().getResource("/srcFile2.sql").getPath());
        fileHandler.openSourceFiles(new ArrayList<>(List.of(file1, file2)));

        URL serialFile = getClass().getResource("/filehandler.serial");
        try (ObjectOutputStream outStream = new ObjectOutputStream(new FileOutputStream(serialFile.getPath()));
             ObjectInputStream inStream = new ObjectInputStream(new FileInputStream(serialFile.getPath()))) {
            outStream.writeObject(fileHandler);
            FileHandler data = (FileHandler) inStream.readObject();

            assertThat(fileHandler.equals(data), equalTo(true));
        } catch (FileNotFoundException | SecurityException ignored) {
        } catch (ClassNotFoundException e) {
            fail();e.getMessage();
        }
    }
}
