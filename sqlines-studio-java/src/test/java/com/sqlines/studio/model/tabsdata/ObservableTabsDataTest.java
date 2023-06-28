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

package com.sqlines.studio.model.tabsdata;

import com.sqlines.studio.model.tabsdata.listener.TabsChangeListener;

import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.net.URL;
import java.util.concurrent.atomic.AtomicReference;

import static org.junit.Assert.*;
import static org.hamcrest.CoreMatchers.*;

public class ObservableTabsDataTest {
    private ObservableTabsData tabsData;

    @Before
    public void setUp() {
        tabsData = new ObservableTabsData();
    }

    @Test
    public void shouldCount3TabsWhenAdding3Tabs() {
        tabsData.openTab(0);
        tabsData.openTab(1);
        tabsData.openTab(2);

        assertThat(tabsData.countTabs(), equalTo(3));
    }

    @Test
    public void shouldCount3TabsWhenDeleting3Tabs() {
        tabsData.openTab(0);
        tabsData.openTab(1);
        tabsData.openTab(2);

        tabsData.removeTab(0);
        tabsData.removeTab(0);
        tabsData.removeTab(0);

        assertThat(tabsData.countTabs(), equalTo(0));
    }

    @Test
    public void shouldNotifyWhenAddingTabs() {
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addTabsListener((change -> {
            if (change.getChangeType() == TabsChangeListener.Change.ChangeType.TAB_ADDED) {
                notified.set(true);
            }
        }));
        tabsData.openTab(0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenDeletingTabs() {
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addTabsListener((change -> {
            if (change.getChangeType() == TabsChangeListener.Change.ChangeType.TAB_REMOVED) {
                notified.set(true);
            }
        }));

        tabsData.openTab(0);
        tabsData.removeTab(0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenSettingCurrTabIndex() {
        tabsData.openTab(0);
        tabsData.openTab(1);
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addTabIndexListener(index -> {
            if (index == 0) {
                notified.set(true);
            }
        });
        tabsData.setCurrTabIndex(0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenSettingTabTitle() {
        tabsData.openTab(0);
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addTabTitleListener((title, index) -> {
            if (title.equals("TITLE") && index == 0) {
                notified.set(true);
            }
        });
        tabsData.setTabTitle("TITLE", 0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenSettingSourceText() {
        tabsData.openTab(0);
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addSourceTextListener((text, index) -> {
            if (text.equals("TEXT") && index == 0) {
                notified.set(true);
            }
        });
        tabsData.setSourceText("TEXT", 0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenSettingTargetText() {
        tabsData.openTab(0);
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addTargetTextListener((text, index) -> {
            if (text.equals("TEXT") && index == 0) {
                notified.set(true);
            }
        });
        tabsData.setTargetText("TEXT", 0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenSettingSourceMode() {
        tabsData.openTab(0);
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addSourceModeListener((mode, index) -> {
            if (mode.equals("MODE") && index == 0) {
                notified.set(true);
            }
        });
        tabsData.setSourceMode("MODE", 0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenSettingTargetMode() {
        tabsData.openTab(0);
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addTargetModeListener((mode, index) -> {
            if (mode.equals("MODE") && index == 0) {
                notified.set(true);
            }
        });
        tabsData.setTargetMode("MODE", 0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenSettingSourceFilePath() {
        tabsData.openTab(0);
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addSourceFilePathListener((path, index) -> {
            if (path.equals("PATH") && index == 0) {
                notified.set(true);
            }
        });
        tabsData.setSourceFilePath("PATH", 0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldNotifyWhenSettingTargetFilePath() {
        tabsData.openTab(0);
        AtomicReference<Boolean> notified = new AtomicReference<>(false);
        tabsData.addTargetFilePathListener((path, index) -> {
            if (path.equals("PATH") && index == 0) {
                notified.set(true);
            }
        });
        tabsData.setTargetFilePath("PATH", 0);

        assertThat(notified.get(), equalTo(true));
    }

    @Test
    public void shouldWriteToFileWhenSerialized() throws IOException {
        tabsData.openTab(0);
        tabsData.setCurrTabIndex(0);
        tabsData.setTabTitle("TITLE", 0);
        tabsData.setSourceMode("SMODE", 0);
        tabsData.setTargetMode("TMODE", 0);
        tabsData.setSourceText("STEXT", 0);
        tabsData.setTargetText("TTEXT", 0);
        tabsData.setSourceFilePath("SPATH", 0);
        tabsData.setTargetFilePath("TPATH", 0);

        URL serialFile = getClass().getResource("/tabsdata.serial");
        try (ObjectOutputStream outStream = new ObjectOutputStream(new FileOutputStream(serialFile.getPath()));
             ObjectInputStream inStream = new ObjectInputStream(new FileInputStream(serialFile.getPath()))) {
            outStream.writeObject(tabsData);
            ObservableTabsData data = (ObservableTabsData) inStream.readObject();

            assertThat(tabsData.equals(data), equalTo(true));
        } catch (FileNotFoundException | SecurityException ignored) {
        } catch (ClassNotFoundException e) {
            fail(e.getMessage());
        }
    }
}
