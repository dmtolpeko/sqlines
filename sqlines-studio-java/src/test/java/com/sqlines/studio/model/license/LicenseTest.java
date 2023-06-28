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

package com.sqlines.studio.model.license;

import com.sqlines.studio.model.coreprocess.CoreProcessRunner;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

import static org.mockito.Mockito.*;
import static org.junit.Assert.*;
import static org.hamcrest.CoreMatchers.*;

public class LicenseTest {
    private License license;
    private CoreProcessRunner coreProcess;

    @Before
    public void setUp() {
        coreProcess = mock(CoreProcessRunner.class);

        String path = getClass().getResource("/license.txt").getPath();
        path = path.substring(0, path.lastIndexOf("/"));
        System.setProperty("model.app-dir", path);

        license = new License(coreProcess);
    }

    @Test
    public void isActiveShouldReturnTrueWhenLicenseIsActive() throws IOException {
        when(coreProcess.runAndWait(any())).thenReturn("LICENSED TO");
        assertThat(license.isActive(), equalTo(true));
    }

    @Test
    public void isActiveShouldReturnFalseWhenLicenseIsNotActive() throws IOException {
        when(coreProcess.runAndWait(any())).thenReturn("FOR EVALUATION USE ONLY");
        assertThat(license.isActive(), equalTo(false));
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldNotifyWhenLicenseStatusChanges() throws IOException {
        when(coreProcess.runAndWait(any())).thenReturn("FOR EVALUATION USE ONLY");

        AtomicBoolean notified = new AtomicBoolean(false);
        license.addLicenseListener(isActive -> {
            if (!isActive) {
                notified.set(true);
            }
        });

        license.changeLicense("Alexander", "4424242");
        assertThat(notified.get(), equalTo(true));
    }
}
