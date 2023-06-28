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

package com.sqlines.studio.model.converter;

import java.io.IOException;

/**
 * Runs the conversion.
 */
public interface Converter {

    /**
     * Runs the conversion using the specified conversion modes and source file path.
     *
     * @param sourceMode source conversion mode to set
     * @param targetMode target conversion mode to set
     * @param sourceFilePath source file path to set
     * @param targetFileName target file name to set
     *
     * @throws IOException if any IO error occurred
     * @throws IllegalStateException if there is no conversion data
     * @throws IllegalStateException if the sqlines program was not found
     * @throws SecurityException if a security manager exists and its checkExec method
     * doesn't allow creation of the subprocess, or the standard input to the subprocess
     * was redirected from a file and the security manager's checkRead method denies read access
     * to the file, or the standard output or standard error of the subprocess was redirected to
     * a file and the security manager's checkWrite method denies write access to the file
     */
    ConversionResult run(String sourceMode, String targetMode,
                         String sourceFilePath, String targetFileName) throws Exception;

    /**
     * Runs the conversion using the specified conversion modes and source data.
     *
     * @param sourceMode source conversion mode to set
     * @param targetMode target conversion mode to set
     * @param sourceData source data to set
     * @param targetFileName target file name to set
     *
     * @throws IOException if any IO error occurred
     * @throws IllegalStateException if there is no conversion data
     * @throws IllegalStateException if the sqlines program was not found
     * @throws SecurityException if a security manager exists and its checkExec method
     * doesn't allow creation of the subprocess, or the standard input to the subprocess
     * was redirected from a file and the security manager's checkRead method denies read access
     * to the file, or the standard output or standard error of the subprocess was redirected to
     * a file and the security manager's checkWrite method denies write access to the file
     */
    ConversionResult run(String sourceMode, String targetMode,
                         byte[] sourceData, String targetFileName) throws Exception;
}
