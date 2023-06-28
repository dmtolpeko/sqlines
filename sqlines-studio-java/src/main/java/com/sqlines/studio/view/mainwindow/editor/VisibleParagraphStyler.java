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

package com.sqlines.studio.view.mainwindow.editor;

import javafx.application.Platform;

import org.fxmisc.richtext.GenericStyledArea;
import org.fxmisc.richtext.model.Paragraph;
import org.fxmisc.richtext.model.StyleSpans;

import org.reactfx.collection.ListModification;

import java.util.function.Consumer;
import java.util.function.Function;

/**
 * Highlights the current paragraph in the text-editing field.
 *
 * @param <PS> the type of the paragraph style
 * @param <SEG> the type of the content segments in the paragraph (e.g. String)
 * @param <S> the type of the style of individual segments
 */
class VisibleParagraphStyler<PS, SEG, S>
        implements Consumer<ListModification<? extends Paragraph<PS, SEG, S>>> {
    private final GenericStyledArea<PS, SEG, S> area;
    private final Function<String, StyleSpans<S>> computeStyles;
    private int prevParagraph;
    private int prevTextLength;

    /**
     * Constructs a new VisibleParagraphStyler with the specified
     * text-editing area and highlighting function.
     *
     * @param area text-editing area to set
     * @param computeStyles highlighting function to set
     */
    public VisibleParagraphStyler(GenericStyledArea<PS, SEG, S> area,
                                  Function<String, StyleSpans<S>> computeStyles) {
        this.computeStyles = computeStyles;
        this.area = area;
    }

    /**
     * Applies the highlighting function to the current paragraph in the text-editing field.
     */
    @Override
    public void accept(ListModification<? extends Paragraph<PS, SEG, S>> modification) {
        if (modification.getAddedSize() > 0) {
            int paragraph = Math.min(area.firstVisibleParToAllParIndex() + modification.getFrom(),
                    area.getParagraphs().size() - 1);
            String text = area.getText(paragraph, 0, paragraph, area.getParagraphLength(paragraph));
            if (paragraph != prevParagraph || text.length() != prevTextLength) {
                int startPos = area.getAbsolutePosition(paragraph, 0);
                Platform.runLater( () -> area.setStyleSpans(startPos, computeStyles.apply(text)) );

                prevTextLength = text.length();
                prevParagraph = paragraph;
            }
        }
    }
}
