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

package com.sqlines.studio.view;

import javafx.geometry.Insets;
import javafx.scene.Parent;
import javafx.scene.control.ToolBar;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;
import javafx.scene.control.Button;
import javafx.scene.layout.HBox;
import javafx.scene.text.Text;
import javafx.stage.Modality;
import javafx.stage.StageStyle;

import java.net.URL;

/**
 * Provides a message for informing the user about an occurred error.
 */
public class ErrorWindow extends AbstractWindow {

    /**
     * Constructs a new ErrorWindow with the specified window title and message.
     *
     * @param title window title to set
     * @param message error message to show
     *
     * @throws IllegalStateException if error icon was not found in application resources
     */
    public ErrorWindow(String title, String message) {
        setUpScene(message);
        setUpWindow(title);
    }

    private void setUpScene(String message) {
        Parent errorImage = createErrorImageLayout();
        Parent textLayout = createTextLayout(message);
        Parent buttonLayout = createButtonlayout();
        Parent mainLayout = createMainLayout(errorImage, textLayout, buttonLayout);

        setRoot(mainLayout);
    }

    private Parent createErrorImageLayout() {
        String imageUrl = loadErrorIcon().toExternalForm();
        ImageView image = new ImageView(new Image(imageUrl));
        image.setFitWidth(60);
        image.setFitHeight(60);

        HBox imageLayout = new HBox(image);
        imageLayout.setPadding(new Insets(15, 15, 15, 10));

        return imageLayout;
    }

    private URL loadErrorIcon() {
        URL iconUrl = getClass().getResource("/icons/error.png");
        if (iconUrl == null) {
            String errorMsg = "File not found in application resources: icons/error.png";
            throw new IllegalStateException(errorMsg);
        }

        return iconUrl;
    }

    private Parent createTextLayout(String message) {
        VBox textLayout = new VBox(new Text(message));
        textLayout.setSpacing(15);
        textLayout.setPadding(new Insets(30, 15, 15, 0));

        return textLayout;
    }

    private Parent createButtonlayout() {
        Button okButton = new Button("Ok");
        okButton.setOnAction(event -> close());

        HBox buttonLayout = new HBox(okButton);
        buttonLayout.setPadding(new Insets(0, 0, 0, 6));

        return buttonLayout;
    }

    private Parent createMainLayout(Parent errorImage, Parent textLayout, Parent buttonLayout) {
        BorderPane mainLayout = new BorderPane();
        mainLayout.setId("errorWindow");
        mainLayout.setLeft(errorImage);
        mainLayout.setCenter(textLayout);
        mainLayout.setBottom(new ToolBar(buttonLayout));

        return mainLayout;
    }

    private void setUpWindow(String title) {
        initModality(Modality.APPLICATION_MODAL);
        initStyle(StageStyle.UTILITY);
        setTitle(title);
        sizeToScene();
        setResizable(false);
    }
}
