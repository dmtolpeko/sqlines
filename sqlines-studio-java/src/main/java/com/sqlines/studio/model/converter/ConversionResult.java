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

import java.util.Objects;

/**
 * Result produced by Converter.
 * Contains converted data and path to the file with this data.
 */
public class ConversionResult {
    private final String data;
    private final String targetFilePath;

    /**
     * Constructs a new ConversionResult with the specified data and target file path.
     *
     * @param data data to set
     * @param targetFilePath targetFilePath to set
     */
    public ConversionResult(String data, String targetFilePath) {
        this.data = data;
        this.targetFilePath = targetFilePath;
    }

    public String getData() {
        return data;
    }

    public String getTargetFilePath() {
        return targetFilePath;
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }

        if (other == null || getClass() != other.getClass()) {
            return false;
        }

        ConversionResult that = (ConversionResult) other;
        return Objects.equals(data, that.data)
                && Objects.equals(targetFilePath, that.targetFilePath);
    }

    @Override
    public int hashCode() {
        return Objects.hash(data, targetFilePath);
    }

    @Override
    public String toString() {
        return getClass().getName() + "{" +
                "data='" + data + '\'' +
                ", targetFilePath='" + targetFilePath + '\'' +
                '}';
    }
}
