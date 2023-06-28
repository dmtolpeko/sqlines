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

package com.sqlines.studio.model.filehandler.listener;

/**
 * A RecentFilesChangeListener is notified whenever the list of recent files changes.
 */
@FunctionalInterface
public interface RecentFilesChangeListener {

    /**
     * Called after a change has been made to the list of recent files.
     *
     * @param change an object representing the change that was done
     */
    void onChange(Change change);

    /**
     * Represents a report of changes done to the list of recent files.
     * The change type in specified by the {@link Change.ChangeType}.
     */
    final class Change {

        /**
         * Enumeration indication the type of change that occurred in the list of recent files.
         */
        public enum ChangeType { FILE_ADDED, FILE_REMOVED, FILE_MOVED }

        private final ChangeType changeType;
        private final String filePath;
        private final int fileIndex;
        private int movedTo;

        /**
         * Constructs a new Change with the specified change type,
         * recent file path and the file index where the change occurred.
         *
         * @param changeType a type of change that occurred in the list of recent files
         * @param path path to the changed recent file
         * @param fileIndex a file index where the change occurred
         *
         * @throws IllegalStateException if change type is neither FILE_ADDED nor FILE_REMOVED
         *
         * @apiNote Suitable for the FILE_ADDED and the FILE_REMOVED change types.
         */
        public Change(ChangeType changeType, String path, int fileIndex) {
            if (changeType != ChangeType.FILE_ADDED && changeType != ChangeType.FILE_REMOVED) {
                String errorMsg = "Invalid change type: FILE_ADDED or FILE_REMOVED" +
                        " expected, " + changeType + " provided";
                throw new IllegalStateException(errorMsg);
            }

            this.changeType = changeType;
            this.filePath = path;
            this.fileIndex = fileIndex;
        }

        /**
         * Constructs a new Change with the specified change type, recent file path,
         * initial file index and new file index.
         *
         * @param changeType a type of change that occurred in the list of recent files
         * @param path path to the changed recent file
         * @param movedFrom initial tab index
         * @param movedTo new tab index
         *
         * @throws IllegalStateException If change type is not FILE_MOVED
         *
         * @apiNote Suitable for the FILE_MOVED change type.
         */
        public Change(ChangeType changeType, String path, int movedFrom, int movedTo) {
            if (changeType != ChangeType.FILE_MOVED) {
                String errorMsg = "Invalid change type: FILE_MOVED expected, " +
                        changeType + " provided";
                throw new IllegalStateException(errorMsg);
            }

            this.changeType = changeType;
            this.filePath = path;
            this.movedTo = movedTo;
            fileIndex = movedFrom;
        }

        /**
         * @return the type of change that occurred in the list of recent files
         */
        public ChangeType getChangeType() {
            return changeType;
        }

        /**
         * @return recent file path
         */
        public String getFilePath() {
            return filePath;
        }

        /**
         * @return the file index where the change occurred
         *
         * @throws IllegalStateException if change type is neither FILE_ADDED nor FILE_REMOVED
         */
        public int getFileIndex() {
            if (changeType == ChangeType.FILE_MOVED) {
                String errorMsg = "Invalid change type:FILE_ADDED or FILE_REMOVED" +
                        " expected, " + changeType + " provided";
                throw new IllegalStateException(errorMsg);
            }

            return fileIndex;
        }

        /**
         * @return initial file index
         *
         * @throws IllegalStateException if change type is not FILE_MOVED
         */
        public int getMovedFrom() {
            if (changeType != ChangeType.FILE_MOVED) {
                String errorMsg = "Invalid change type: FILE_MOVED expected, "
                        + changeType + " provided";
                throw new IllegalStateException(errorMsg);
            }

            return fileIndex;
        }

        /**
         * @return new file index
         *
         * @throws IllegalStateException If change type is not FILE_MOVED
         */
        public int getMovedTo() {
            if (changeType != ChangeType.FILE_MOVED) {
                String errorMsg = "Invalid change type: FILE_MOVED expected, "
                        + changeType + " provided";
                throw new IllegalStateException(errorMsg);
            }

            return movedTo;
        }
    }
}
