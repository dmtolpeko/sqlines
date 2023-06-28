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

import java.util.Objects;
import java.util.stream.Stream;

/**
 * SQLines command-line program arguments.
 */
public class Arguments {
    private String sourceMode = "";
    private String targetMode = "";
    private String sourceFilePath = "";
    private String targetFilePath = "";
    private String logFilePath = "";
    private String isLicenseCheck = "";

    /**
     * @return Arguments object builder
     */
    public static Builder builder() {
        return new Arguments().new Builder();
    }

    private Arguments() {
    }

    public String[] toArray() {
        return Stream.of(sourceMode, targetMode, sourceFilePath,
                         targetFilePath, logFilePath, isLicenseCheck)
                .filter(arg -> !arg.isEmpty())
                .toArray(String[]::new);
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }

        if (other == null || getClass() != other.getClass()) {
            return false;
        }

        Arguments arguments = (Arguments) other;
        return Objects.equals(sourceMode, arguments.sourceMode)
                && Objects.equals(targetMode, arguments.targetMode)
                && Objects.equals(sourceFilePath, arguments.sourceFilePath)
                && Objects.equals(targetFilePath, arguments.targetFilePath)
                && Objects.equals(logFilePath, arguments.logFilePath);
    }

    @Override
    public int hashCode() {
        return Objects.hash(sourceMode, targetMode, sourceFilePath, targetFilePath, logFilePath);
    }

    @Override
    public String toString() {
        return getClass().getName() + "{" +
                "sourceMode='" + sourceMode + '\'' +
                ", targetMode='" + targetMode + '\'' +
                ", sourceFilePath='" + sourceFilePath + '\'' +
                ", targetFilePath='" + targetFilePath + '\'' +
                ", logFilePath='" + logFilePath + '\'' +
                '}';
    }

    /**
     * Arguments object builder.
     */
    public class Builder {

        private Builder() {
        }

        public Arguments build() {
            return Arguments.this;
        }

        public Builder withSourceMode(String sourceMode) {
            Arguments.this.sourceMode = "-s = " + sourceMode;
            return this;
        }

        public Builder withTargetMode(String targetMode) {
            Arguments.this.targetMode = "-t = " + targetMode;
            return this;
        }

        public Builder withSourceFilePath(String sourcePath) {
            Arguments.this.sourceFilePath = "-in = " + sourcePath;
            return this;
        }

        public Builder withTargetFilePath(String targetPath) {
            Arguments.this.targetFilePath = "-out = " + targetPath;
            return this;
        }

        public Builder withLogFilePath(String logPath) {
            Arguments.this.logFilePath = "-log = " + logPath;
            return this;
        }

        public Builder isLicenseCheck(boolean isLicenseCheck) {
            if (isLicenseCheck) {
                Arguments.this.isLicenseCheck = "-?";
            }

            return this;
        }
    }
}
