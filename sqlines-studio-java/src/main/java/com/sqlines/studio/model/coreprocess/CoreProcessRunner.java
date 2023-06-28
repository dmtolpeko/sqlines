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

import java.io.IOException;

/**
 * SQLines command-line program.
 */
public interface CoreProcessRunner {

    /**
     * Starts a new process with the specified command-line arguments.
     * Causes the current thread to wait, if necessary, until the process has terminated.
     * <p>
     * The program path is taken from the {@link java.util.Properties}.
     * Key - model.process-dir.
     *
     * @param arguments command-line arguments to set
     *
     * @return output log
     *
     * @throws IllegalStateException if the sqlines program was not found
     * @throws IOException if an I/O error occurs
     * @throws SecurityException if a security manager exists and its checkExec
     * method doesn't allow creation of the subprocess, or the standard input to the
     * subprocess was redirected from a file and the security manager's checkRead method
     * denies read access to the file, or the standard output or standard error of the
     * subprocess was redirected to a file and the security manager's checkWrite method
     * denies write access to the file
     */
    String runAndWait(Arguments arguments) throws IOException;
}
