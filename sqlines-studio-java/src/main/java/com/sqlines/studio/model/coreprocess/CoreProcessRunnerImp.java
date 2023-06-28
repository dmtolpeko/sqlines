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

package com.sqlines.studio.model.coreprocess;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.stream.Stream;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class CoreProcessRunnerImp implements CoreProcessRunner {
    private static final Logger logger = LogManager.getLogger(CoreProcessRunnerImp.class);

    @Override
    public String runAndWait(Arguments arguments) throws IOException {
        try {
            String[] args = prepareArgs(arguments);
            Process process = new ProcessBuilder(args).start();
            logger.info("Running SQLines command-line program with args: " + Arrays.toString(args));
            process.waitFor();
            logger.info("SQLines command-line program finished successfully");
            return new  String(process.getInputStream().readAllBytes());
        } catch (InterruptedException e) {
            logger.error("SQLines command-line program crashed: " + e.getMessage());
            return e.getMessage();
        }
    }

    private String[] prepareArgs(Arguments arguments) {
        Stream<String> processPath = Stream.of(getProcessPath());
        Stream<String> args = Arrays.stream(arguments.toArray());
        return Stream.concat(processPath, args)
                .toArray(String[]::new);
    }

    private String getProcessPath() {
        String processPath = System.getProperty("model.app-dir");
        processPath += osIsWin() ? "/sqlines.exe" : "/sqlines";
        checkFileExistence(processPath);
        return processPath;
    }

    private boolean osIsWin() {
        String os = System.getProperty("os.name").toLowerCase();
        return os.startsWith("windows");
    }

    private void checkFileExistence(String path) {
        File file = new File(path);
        if (!file.exists()) {
            String errorMsg = "SQLines command-line program was not found:\n" + path;
            throw new IllegalStateException(errorMsg);
        }
    }
}
