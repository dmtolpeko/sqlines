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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QComboBox>
#include <QProcess>

#define INI_SOURCE_TYPE             "source_type"
#define INI_TARGET_TYPE             "target_type"
#define INI_LAST_OPEN_DIR           "last_open_dir"

class ScriptTab;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSettings *settings;

    QComboBox *_sourceComboBox;
    QComboBox *_targetComboBox;

    // The tab for which conversion is running
    ScriptTab *_running_tab;

    QString lastOpenDir;

    QProcess sqlinesProcess;

protected:
    void resizeEvent(QResizeEvent *event);

public slots:

    void createNewScript();
    void openFileDialog();
    void convert();
    void showAboutDialog();
    void showOptionsDialog();
    void openOnlineConversion();
    void openOnlineHelp();
    void openContactUs();
    void exitApp();

    void sqlinesStarted();
    void sqlinesFinished(int, QProcess::ExitStatus);
    void sqlinesError(QProcess::ProcessError);

    void setWindows7Style() { setUIStyle(QString("windowsvista")); }
    void setWindowsXPStyle() { setUIStyle(QString("windowsxp")); }
    void setWindowsStyle() { setUIStyle(QString("windows")); }
    void setMacintoshStyle() { setUIStyle(QString("macintosh")); }
    void setMotifStyle() { setUIStyle(QString("motif")); }
    void setCDEStyle() { setUIStyle(QString("cde")); }
    void setPlastiqueStyle() { setUIStyle(QString("plastique")); }
    void setCleanlooksStyle() { setUIStyle(QString("cleanlooks")); }
    void setDefaultStyle() { setUIStyle("default"); }

    void closeMainTab(int);

private:
    QString workingDirectory;
    QString fileNamePrefix;
    // Open files
    QStringList openFiles;

    // Map for source/target types and their file extensions
    QMap<QString, QString> fileExtensionsMap;
    // Map for extensions and new file index number
    QMap<QString, int> extensionsNewIndexMap;

    // Default UI style for the current system
    QString defaultStyle;
    QString currentStyle;

    // Styles keys - actions map
    QMap<QString, QAction*> stylesMap;

private:
    void initializeComponents();

    void setUIStyle(QString style);

    void createToolbar();
    void createCenterArea();

    void createNewScript(QString *file);
    void loadOptions();
    void saveOptions();

    void startSqlines(QString &source_file);

    QStringList getSourceTargetList();
    QString getCmdType(QString type);
};

#pragma clang diagnostic ignored "-Wredundant-parens"

#endif // MAINWINDOW_H
