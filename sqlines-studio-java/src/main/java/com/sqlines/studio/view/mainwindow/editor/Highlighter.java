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

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.Collection;
import java.util.Collections;
import java.util.StringTokenizer;
import java.util.List;
import java.util.LinkedList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import org.fxmisc.richtext.model.StyleSpans;
import org.fxmisc.richtext.model.StyleSpansBuilder;

/**
 * Highlights the specified text according to the highlighting patterns.
 */
class Highlighter {
    private static final Logger logger = LogManager.getLogger(Highlighter.class);
    private static Pattern pattern;

    static {
        try {
            compilePattern();
        } catch (Exception e) {
            logger.warn(e.getMessage());
        }
    }

    private static void compilePattern() throws IOException {
        String keywordRegex = "\\b(" + String.join("|", loadKeywords()) + ")\\b";
        String digitRegex = "\\b[0-9]+\\b";
        String stringRegex = "\"([^\"\\\\]|\\\\.)*\"";
        String charRegex = "'.*'";
        String commentRegex = "//[^\n]*" + "|" + "/\\*(.|\\R)*?\\*/"
                + "|" + "/\\*[^\\v]*" + "|" + "^\\h*\\*([^\\v]*|/)" + "|" + "--[^\n]*";

        pattern = Pattern.compile(
                "(?<KEYWORD>" + keywordRegex + ")"
                        + "|(?<DIGIT>" + digitRegex + ")"
                        + "|(?<STRING>" + stringRegex + ")"
                        + "|(?<CHAR>" + charRegex + ")"
                        + "|(?<COMMENT>" + commentRegex + ")",
                Pattern.CASE_INSENSITIVE
        );
    }

    private static String[] loadKeywords() throws IOException {
        try (InputStream stream = Highlighter.class.getResourceAsStream("/keywords.txt")) {
            if (stream == null) {
                String errorMsg = "File not found in application resources: keywords.txt";
                throw new IllegalStateException(errorMsg);
            }

            String data = new String(stream.readAllBytes(), StandardCharsets.UTF_8);
            return extractKeywords(data);
        }
    }

    private static String[] extractKeywords(String keywords) {
        StringTokenizer tokenizer = new StringTokenizer(keywords, ", ");
        List<String> words = new LinkedList<>();
        while (tokenizer.hasMoreTokens()) {
            String word = tokenizer.nextToken().replace('\n', ' ').trim();
            words.add(word);
        }

        words.removeIf(String::isEmpty);
        String[] wordsArray = new String[words.size()];
        words.toArray(wordsArray);
        return wordsArray;
    }

    /**
     * Highlights the specified text according to the highlighting patterns.
     *
     * @param text text to highlight
     *
     * @return style spans
     */
    public StyleSpans<Collection<String>> computeHighlighting(String text) {
        StyleSpansBuilder<Collection<String>> spansBuilder = new StyleSpansBuilder<>();
        if (pattern == null) {
            spansBuilder.create();
        }

        int lastKnownEnd = 0;
        Matcher matcher = pattern.matcher(text);
        while (matcher.find()) {
            String styleClass = matcher.group("KEYWORD") != null ? "keyword" :
                    matcher.group("DIGIT") != null ? "digit" :
                        matcher.group("STRING") != null ? "string" :
                            matcher.group("CHAR") != null ? "char" : "comment";

            spansBuilder.add(Collections.emptyList(), matcher.start() - lastKnownEnd);
            spansBuilder.add(Collections.singleton(styleClass), matcher.end() - matcher.start());
            lastKnownEnd = matcher.end();
        }

        spansBuilder.add(Collections.emptyList(), text.length() - lastKnownEnd);
        return spansBuilder.create();
    }
}
