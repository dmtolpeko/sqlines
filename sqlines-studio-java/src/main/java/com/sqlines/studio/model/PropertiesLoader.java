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

package com.sqlines.studio.model;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

/**
 * Loads the application properties into the standard {@link Properties} class.
 * <p>
 * Defines the following properties:
 *
 * <pre>
 * | Key                | Values                | Definition                      |
 * |--------------------|-----------------------|---------------------------------|
 * | model.save-session | "enabled", "disabled" | Last session saving property    |
 * | model.curr-dir     | path                  | Current working directory path  |
 * | model.dirs-number  | int                   | The number of added directories |
 * | model.dir-i        | path                  | Added directory. i - dir index  |
 * | model.last-dir     | path                  | Last opened directory           |
 * | model.app-dir      | path                  | Jar directory path              |
 * | view.theme         | "light", "dark"       | Current theme                   |
 * | view.status-bar    | "show", "do-not-show" | Status bar policy               |
 * | view.target-field   | "always", "as-needed" | Target field policy              |
 * | view.wrapping      | "enabled", "disabled" | Wrapping policy                 |
 * | view.highlighter   | "enabled", "disabled" | Highlighter policy              |
 * | view.line-numbers  | "enabled", "disabled" | Line-numbers policy             |
 * | view.height        | double                | Main window height              |
 * | view.width         | double                | Main window width               |
 * | view.pos.x         | double                | Main window position on the x   |
 * | view.pos.y         | double                | Main window position on the y   |
 * | view.is-maximized  | "true", "false"       | Is main window maximized        |
 * </pre>
 *
 * Initially sets default properties. See {@link PropertiesLoader#setDefaults()}.
 */
public class PropertiesLoader {
    private static final Properties properties = System.getProperties();
    private static final Logger logger = LogManager.getLogger(PropertiesLoader.class);

    static {
        setDefaults();
    }

    /**
     * Sets the following properties:
     *
     * <pre>
     * | Key                | Value                                    |
     * |--------------------|------------------------------------------|
     * | model.save-session | "enabled"                                |
     * | model.curr-dir     | user.home + /sqlines                     |
     * | model.dirs-number  | 0                                        |
     * | model.app-dir      | jar path                                 |
     * | model.last-dir     | "null"                                   |
     * | view.theme         | "light"                                  |
     * | view.status-bar    | "show"                                   |
     * | view.target-field   | "always"                                 |
     * | view.wrapping      | "disabled"                               |
     * | view.highlighter   | "enabled"                                |
     * | view.line-numbers  | "enabled"                                |
     * | view.height        | 650.0                                    |
     * | view.width         | 770.0                                    |
     * | view.pos.x         | 0.0                                      |
     * | view.pos.y         | 0.0                                      |
     * | view.is-maximized  | "false"                                  |
     * </pre>
     */
    public static void setDefaults() {
        serDefaultModelProperties();
        serDefaultViewProperties();
        loadAppDir();
    }

    private static void serDefaultModelProperties() {
        int dirsNumber = Integer.parseInt(properties.getProperty("model.dirs-number", "0"));
        for (int i = 0; i < dirsNumber; i++) {
            properties.remove("model.dir-" + i);
        }

        properties.setProperty("model.save-session", "enabled");
        properties.setProperty("model.curr-dir", properties.getProperty("user.home") + "/sqlines");
        properties.setProperty("model.dirs-number", "0");
        properties.setProperty("model.last-dir", "null");
    }

    private static void loadAppDir() {
        try {
            String appPath = PropertiesLoader.class
                    .getProtectionDomain()
                    .getCodeSource()
                    .getLocation()
                    .toURI()
                    .getPath();
            appPath = appPath.substring(0, appPath.lastIndexOf("/"));
            properties.setProperty("model.app-dir", appPath);
        } catch (Exception e) {
            logger.error("Loading application dir: " + e.getMessage());
        }
    }

    private static void serDefaultViewProperties() {
        properties.setProperty("view.theme", "light");
        properties.setProperty("view.status-bar", "show");
        properties.setProperty("view.target-field", "always");
        properties.setProperty("view.wrapping", "disabled");
        properties.setProperty("view.highlighter", "enabled");
        properties.setProperty("view.line-numbers", "enabled");
        properties.setProperty("view.height", "650.0");
        properties.setProperty("view.width", "770.0");
        properties.setProperty("view.pos.x", "0.0");
        properties.setProperty("view.pos.y", "0.0");
        properties.setProperty("view.is-maximized", "false");
    }

    /**
     * Loads properties from the properties file.
     *
     * @throws FileNotFoundException if the file does not exist, is a directory rather
     * than a regular file, or for some other reason cannot be opened for reading.
     * @throws IOException - if any IO error occurred
     * @throws SecurityException if a security manager exists and its
     * checkRead method denies read access to the file
     */
    public static void loadProperties() throws IOException {
        String path = properties.getProperty("java.io.tmpdir") + "sqlines-properties.txt";
        File propertiesFile = new File(path);
        loadFromFile(propertiesFile);
    }

    private static void loadFromFile(File file) throws IOException {
        try (FileInputStream stream = new FileInputStream(file)) {
            properties.load(stream);
            loadAppDir();
        }
    }

    /**
     * Saves application properties to the properties file.
     *
     * @throws FileNotFoundException if the file does not exist, is a directory rather
     * than a regular file, or for some other reason cannot be opened for reading
     * @throws IOException - if any IO error occurred
     * @throws ClassCastException if this {@link Properties} object contains
     * any keys or values that are not Strings
     * @throws SecurityException if a security manager exists and its
     * checkRead method denies read access to the file
     */
    public static void saveProperties() throws IOException {
        String path = properties.getProperty("java.io.tmpdir") + "sqlines-properties.txt";
        File propertiesFile = new File(path);
        if (!propertiesFile.exists()) {
            propertiesFile = createFile(path);
        }

        saveToFile(propertiesFile);
    }

    private static File createFile(String path) throws IOException {
        File file = new File(path);
        boolean success = file.createNewFile();
        if (!success) {
            throw new IOException("Cannot create properties file: " + path);
        }

        return file;
    }

    private static void saveToFile(File file) throws IOException {
        try (FileOutputStream stream = new FileOutputStream(file)) {
            properties.store(stream, "SQLines Studio properties");
        }
    }
}
