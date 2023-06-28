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

import com.sqlines.studio.model.coreprocess.Arguments;
import com.sqlines.studio.model.coreprocess.CoreProcessRunner;
import com.sqlines.studio.model.license.listener.LicenseChangeListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.time.Instant;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import java.util.TimeZone;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Works with license file.
 *
 * @see LicenseChangeListener
 */
public class License implements Runnable {
    private static final Logger logger = LogManager.getLogger(License.class);

    private final CoreProcessRunner coreProcess;
    private final List<LicenseChangeListener> licenseListeners = new ArrayList<>(5);
    private long lastModified;

    /**
     * Constructs a new License with the specified sqlines command line program.
     *
     * @param coreProcess sqlines command line program
     */
    public License(CoreProcessRunner coreProcess) {
        this.coreProcess = coreProcess;
        try {
            File file = getLicenseFile();
            lastModified = file.lastModified();
        } catch (Exception e) {
            logger.warn("License file not found: " + e.getMessage());
        }
    }

    private File getLicenseFile() {
        String path = System.getProperty("model.app-dir", "null") + "/license.txt";
        File licenseFile = new File(path);
        if (!licenseFile.exists()) {
            throw new IllegalStateException("File not found: " + path);
        }

        return licenseFile;
    }

    @Override
    @SuppressWarnings("BusyWait")
    public void run() {
        while (true) {
            try {
                Thread.sleep(10000);
                monitorLicenseFile();
            } catch (InterruptedException e) {
                break;
            } catch (Exception e) {
                logger.error("License check - " + e.getMessage());
            }
        }
    }

    private synchronized void monitorLicenseFile() {
        File file = getLicenseFile();
        if (file.lastModified() != lastModified) {
            String info = "Changing license due to license file update: license file updated at " +
                    LocalDateTime.ofInstant(Instant.ofEpochMilli(file.lastModified()),
                            TimeZone.getDefault().toZoneId());
            logger.info(info);
            changeLicense(file.lastModified());
        }
    }

    private void changeLicense(long lastModified) {
        boolean isActive = isActive();
        licenseListeners.forEach(license -> license.changed(isActive));
        this.lastModified = lastModified;
        logger.info("License changed. Status: " +  (isActive ? "Active" : "Not active"));
    }

    /**
     * @return true if license is active, false otherwise
     */
    public synchronized boolean isActive() {
        try {
            logger.info("Checking license...");
            String logFilePath = createLogFile();
            Arguments arguments = Arguments.builder()
                    .withLogFilePath(logFilePath)
                    .isLicenseCheck(true)
                    .build();
            String output = coreProcess.runAndWait(arguments);
            deleteLogFile(logFilePath);
            return !output.contains("FOR EVALUATION USE ONLY");
        } catch (Exception e) {
            logger.error(e.getMessage());
            return false;
        } finally {
            logger.info("License checked");
        }
    }

    private String createLogFile() {
        String path = "";
        try {
            File file = File.createTempFile("sqlines-log", ".tmp");
            logger.info("Log file created: " + file.getAbsolutePath());
            path = file.getAbsolutePath();
        } catch (Exception e) {
            logger.error("Cannot create log file: " + e.getMessage());
        }

        return path;
    }

    private void deleteLogFile(String path) {
        try {
            File file = new File(path);
            boolean success = file.delete();
            if (success) {
                logger.info("Log file deleted: " + path);
            } else {
                logger.error("Cannot delete log file: " + path);
            }
        } catch (Exception e) {
            logger.error("Cannot delete log file: " + e.getMessage());
        }
    }

    /**
     * Updates license file and checks license status.
     * <p>
     * Notifies all {@link LicenseChangeListener} listeners of the license status change.
     *
     * @param regName registration name to set
     * @param regNumber registration number to set
     *
     * @throws IllegalArgumentException if registration data is invalid
     * @throws IllegalStateException if license file was not found
     * @throws IllegalStateException if sqlines command-line program was not found
     * @throws IOException if any IO error occurred
     * @throws SecurityException if a security manager exists and its checkWrite method
     * denies write access to the file
     */
    public synchronized void changeLicense(String regName, String regNumber) throws IOException {
        writeLicenseInfo(regName, regNumber);
        if (isActive()) {
            licenseListeners.forEach(license -> license.changed(true));
        } else {
            licenseListeners.forEach(license -> license.changed(false));
            throw new IllegalArgumentException("Invalid registration data");
        }
    }

    private void writeLicenseInfo(String regName, String regNumber) throws IOException {
        try (FileOutputStream stream = new FileOutputStream(getLicenseFile())) {
            String info = "SQLines license file:\n" +
                    "\nRegistration Name: " + regName +
                    "\nRegistration Number: " + regNumber;
            stream.write(info.getBytes(StandardCharsets.UTF_8));
        }
    }

    /**
     * Adds a listener which will be notified when the license status changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public synchronized void addLicenseListener(LicenseChangeListener listener) {
        licenseListeners.add(listener);
    }
}
