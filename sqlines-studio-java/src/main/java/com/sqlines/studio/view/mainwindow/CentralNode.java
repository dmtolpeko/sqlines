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

package com.sqlines.studio.view.mainwindow;

import com.sqlines.studio.view.mainwindow.editor.CodeEditor;

import javafx.beans.value.ChangeListener;
import javafx.geometry.Insets;
import javafx.scene.layout.HBox;

import java.util.Optional;

/**
 * Central control in the main window.
 * <p>
 * Contains 2 text input fields for source data and target data, respectively.
 */
class CentralNode extends HBox {

    /**
     * Represents currently focused text input field.
     */
    public enum inFocus { SOURCE, TARGET }

    /**
     * An enumeration denoting the policy to be used by a CentralNode
     * in deciding whether to show a target text input field.
     */
    public enum TargetFieldPolicy { ALWAYS, AS_NEEDED }

    private final CodeEditor sourceEditor = new CodeEditor();
    private final CodeEditor targetEditor = new CodeEditor();

    public CentralNode() {
        setUp();
        showTargetFieldOnTextChange();
    }

    private void setUp() {
        getChildren().addAll(sourceEditor);
        setSpacing(10);
        setPadding(new Insets(5, 5, 5, 5));
    }

    private void showTargetFieldOnTextChange() {
        targetEditor.addTextListener((observable, oldText, newText) -> {
            if (!newText.isEmpty() && !getChildren().contains(targetEditor)) {
                getChildren().add(targetEditor);
            }
        });
    }

    /**
     * Replaces the entire content of the source text input field with the given text.
     *
     * @param text text to set
     */
    public void setSourceText(String text) {
       sourceEditor.setText(text);
    }

    /**
     * Replaces the entire content of the target text input field with the given text.
     *
     * @param text text to set
     */
    public void setTargetText(String text) {
        targetEditor.setText(text);
    }

    /**
     * @return the index of the current line in the source text input field
     */
    public int getSourceLineIndex() {
        return sourceEditor.getLineIndex();
    }

    /**
     * @return the index of the current line in the target text input field
     */
    public int getTargetLineIndex() {
        return targetEditor.getLineIndex();
    }

    /**
     * @return the index of the current column in the source text input field
     */
    public int getSourceColumnIndex() {
        return sourceEditor.getColumnIndex();
    }

    /**
     * @return the index of the current column in the target text input field
     */
    public int getTargetColumnIndex() {
        return targetEditor.getColumnIndex();
    }

    /**
     * Erases the last change done in the currently focused text input field.
     * Does nothing if none of the input fields are in focus.
     */
    public void undo() {
        Optional<CodeEditor> inFocus = inFocus();
        inFocus.ifPresent(CodeEditor::undo);
    }

    private Optional<CodeEditor> inFocus() {
        if (sourceEditor.hasFocus()) {
            return Optional.of(sourceEditor);
        } else if (targetEditor.hasFocus()) {
            return Optional.of(targetEditor);
        } else {
            return Optional.empty();
        }
    }

    /**
     * Restores any actions that were previously undone using an undo
     * in the currently focused text input field.
     * Does nothing if none of the input fields are in focus.
     */
    public void redo() {
        Optional<CodeEditor> inFocus = inFocus();
        inFocus.ifPresent(CodeEditor::redo);
    }

    /**
     * Selects everything in the area in the currently focused text input field.
     * Does nothing if none of the input fields are in focus.
     */
    public void selectAll() {
        Optional<CodeEditor> inFocus = inFocus();
        inFocus.ifPresent(CodeEditor::selectAll);
    }

    /**
     * Transfers the currently selected text in the currently focused text input field
     * to the clipboard, removing the current selection.
     * Does nothing if none of the input fields are in focus.
     */
    public void cut() {
        Optional<CodeEditor> inFocus = inFocus();
        inFocus.ifPresent(CodeEditor::cut);
    }

    /**
     * Transfers the currently selected text in the currently focused text input field
     * to the clipboard, leaving the current selection.
     * Does nothing if none of the input fields are in focus.
     */
    public void copy() {
        Optional<CodeEditor> inFocus = inFocus();
        inFocus.ifPresent(CodeEditor::copy);
    }

    /**
     * Inserts the content from the clipboard into this currently focused text input field,
     * replacing the current selection.
     * If there is no selection, the content from the clipboard is inserted
     * at the current caret position of the currently focused text input field.
     * Does nothing if none of the input fields are in focus.
     */
    public void paste() {
        Optional<CodeEditor> inFocus = inFocus();
        inFocus.ifPresent(CodeEditor::paste);
    }

    /**
     * @return true if undo is available for use in the currently focused
     * text input field, false otherwise
     */
    public boolean isUndoAvailable() {
        Optional<CodeEditor> inFocus = inFocus();
        return inFocus.map(CodeEditor::isUndoAvailable)
                .orElse(false);
    }

    /**
     * @return true if redo is available for use in the currently focused
     * text input field, false otherwise
     */
    public boolean isRedoAvailable() {
        Optional<CodeEditor> inFocus = inFocus();
        return inFocus.map(CodeEditor::isRedoAvailable)
                .orElse(false);
    }

    /**
     * Increases the font size of the source and target text input fields by 1 px.
     */
    public void zoomIn() {
        sourceEditor.zoomIn();
        targetEditor.zoomIn();
    }

    /**
     * Decreases the font size of the source and target text input fields by 1 px.
     */
    public void zoomOut() {
        sourceEditor.zoomOut();
        targetEditor.zoomOut();
    }

    /**
     * Requests focus for the specified text input field.
     *
     * @param inFocus text input field to focus on
     */
    public void focusOn(inFocus inFocus) {
       if (inFocus == CentralNode.inFocus.SOURCE) {
            sourceEditor.requestFocus();
        } else if (inFocus == CentralNode.inFocus.TARGET) {
            targetEditor.requestFocus();
        }
    }

    /**
     * Sets the {@link TargetFieldPolicy}.
     * <p>
     * The default value is AS_NEEDED.
     *
     * @param policy target field policy to set
     */
    public void setTargetFieldPolicy(TargetFieldPolicy policy) {
        if (policy == TargetFieldPolicy.ALWAYS) {
            if (!getChildren().contains(targetEditor)) {
                getChildren().add(targetEditor);
            }
        } else if (policy == TargetFieldPolicy.AS_NEEDED) {
            if (targetEditor.getText().isEmpty()) {
                getChildren().remove(targetEditor);
            }
        }
    }

    /**
     * Sets the {@link CodeEditor.WrappingPolicy} of the text input fields.
     * <p>
     * The default value is NO_WRAP.
     *
     * @param policy wrapping policy to set
     */
    public void setWrappingPolicy(CodeEditor.WrappingPolicy policy) {
        sourceEditor.setWrappingPolicy(policy);
        targetEditor.setWrappingPolicy(policy);
    }

    /**
     * Sets the {@link CodeEditor.HighlighterPolicy} of the text input fields.
     * <p>
     * The default value is HIGHLIGHT.
     *
     * @param policy highlighter policy to set
     */
    public void setHighlighterPolicy(CodeEditor.HighlighterPolicy policy) {
        sourceEditor.setHighlighterPolicy(policy);
        targetEditor.setHighlighterPolicy(policy);
    }

    /**
     * Sets the {@link CodeEditor.LineNumbersPolicy} of the text input fields.
     * <p>
     * The default value is SHOW.
     *
     * @param policy line numbers policy to set
     */
    public void setLineNumbersPolicy(CodeEditor.LineNumbersPolicy policy) {
        sourceEditor.setLineNumbersPolicy(policy);
        targetEditor.setLineNumbersPolicy(policy);
    }
    
    /**
     * Adds a listener which will be notified when the text
     * in the source text input field changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addSourceTextListener(ChangeListener<String> listener) {
        sourceEditor.addTextListener(listener);
    }

    /**
     * Adds a listener which will be notified when the text
     * in the target text input field changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addTargetTextListener(ChangeListener<String> listener) {
        targetEditor.addTextListener(listener);
    }

    /**
     * Adds a listener which will be notified when the current line index
     * in the source text input field changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addSourceLineIndexListener(ChangeListener<Integer> listener) {
        sourceEditor.addLineIndexListener(listener);
    }

    /**
     * Adds a listener which will be notified when the current line index
     * in the target text input field changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addTargetLineIndexListener(ChangeListener<Integer> listener) {
        targetEditor.addLineIndexListener(listener);
    }

    /**
     * Adds a listener which will be notified when the current column index
     * in the source text input field changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addSourceColumnIndexListener(ChangeListener<Integer> listener) {
        sourceEditor.addColumnIndexListener(listener);
    }

    /**
     * Adds a listener which will be notified when the current column index
     * in the target text input field changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addTargetColumnIndexListener(ChangeListener<Integer> listener) {
        targetEditor.addColumnIndexListener(listener);
    }

    /**
     * Adds a listener which will be notified when the source text input field focus changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addSourceFocusListener(ChangeListener<Boolean> listener) {
        sourceEditor.addFocusListener(listener);
    }

    /**
     * Adds a listener which will be notified when the target text input field focus changes.
     * If the same listener is added more than once, then it will be notified more than once.
     *
     * @param listener the listener to register
     */
    public void addTargetFocusListener(ChangeListener<Boolean> listener) {
        targetEditor.addFocusListener(listener);
    }
}
