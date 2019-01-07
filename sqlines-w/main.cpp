/**
 * Copyright (c) 2016 SQLines
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

//#if QT_VERSION >= 0x050000
    #include <QApplication>
//#else
//    #include <QtGui/QApplication>
//#endif
#include <QStyle>
#include <QStyleFactory>

#include "mainwindow.h"

#define SQLINESW_VERSION      "SQLines SQL Converter"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Do not allow to resize the main window
    w.setFixedWidth(900);
    w.setFixedHeight(565);

    w.setWindowTitle(SQLINESW_VERSION);
    w.show();

    return a.exec();
}
