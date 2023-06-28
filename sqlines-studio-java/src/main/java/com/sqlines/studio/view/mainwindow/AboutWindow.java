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

import com.sqlines.studio.view.AbstractWindow;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;
import javafx.stage.Modality;
import javafx.stage.StageStyle;

import java.net.URL;

/**
 * Provides a window with information about the application.
 */
class AboutWindow extends AbstractWindow {

    /**
     * Constructs a new AboutWindow.
     *
     * @throws IllegalStateException if SQLines Studio logo icon was not found in application resources
     */
    public AboutWindow() {
        setUpScene();
        setUpWindow();
    }

    private void setUpScene() {
        ImageView logoImg = createLogoImage();
        Parent appInfo = createAppInfo();
        Parent layout = createMainLayout(logoImg, appInfo);
        setRoot(layout);
    }

    private ImageView createLogoImage() {
        String logoUrl = loadLogoIcon().toExternalForm();
        ImageView logoImg = new ImageView(new Image(logoUrl));
        logoImg.setFitHeight(115);
        logoImg.setFitWidth(110);

        return logoImg;
    }

    private URL loadLogoIcon() {
        URL iconUrl = getClass().getResource("/icons/logo.png");
        if (iconUrl == null) {
            String errorMsg = "File not found in application resources: icons/logo.png";
            throw new IllegalStateException(errorMsg);
        }

        return iconUrl;
    }

    private Parent createAppInfo() {
        Text appInfo = new Text("SQLines Studio\n  Version: 3.0");
        Text copyrightInfo = new Text("© 2021 SQLines\nAll rights reserved");

        VBox infoLayout = new VBox(appInfo, copyrightInfo);
        infoLayout.setAlignment(Pos.CENTER);
        infoLayout.setSpacing(5);

        return infoLayout;
    }

    private Parent createMainLayout(Node logo, Node appInfo) {
        VBox mainLayout = new VBox(logo, appInfo);
        mainLayout.setId("aboutWindow");
        mainLayout.setSpacing(10);
        mainLayout.setAlignment(Pos.CENTER);
        mainLayout.setPadding(new Insets(50, 0, 60, 0));

        return mainLayout;
    }

    private void setUpWindow() {
        initModality(Modality.APPLICATION_MODAL);
        initStyle(StageStyle.UTILITY);
        setTitle("About SQLines Studio");
        setHeight(240);
        setWidth(230);
        setResizable(false);
    }
}
