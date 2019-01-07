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
#include <QLineEdit>

#define INI_MAINWINDOW_POS          "mainwindow_position"
#define INI_SOURCE_TYPE             "source_type"
#define INI_TARGET_TYPE             "target_type"
#define INI_LAST_SOURCE_DIR         "last_source_dir"
#define INI_LAST_CUR_SOURCE_DIR     "last_cur_source_dir"
#define INI_LAST_TARGET_DIR         "last_target_dir"
#define INI_LAST_CUR_TARGET_DIR     "last_cur_target_dir"
#define INI_ITEMS_IN_HISTORY        "items_in_history"
#define INI_SOURCE_DIRS             "source_dirs"
#define INI_TARGET_DIRS             "target_dirs"
#define INI_COMMAND_LINE            "command_line"
#define INI_CUR_COMMAND_LINE        "cur_command_line"
#define INI_CUR_REPORT_TEMPLATE     "cur_assessment_template"
#define INI_OPEN_REPORT             "open_report"
#define INI_SWITCH_TO_LOG           "switch_to_log"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openSourceDirectory();
    void openTargetDirectory();
    void clearAll();
    void migrate();
    void assess();
    void sqlinesStarted();
    void sqlinesFinished(int, QProcess::ExitStatus);
    void showSqlinesOutput();
    void sqlinesError(QProcess::ProcessError);
    void viewResults();
    void openReport();
    void terminate();
    void viewHelp();
    void exitApp();

private:
    Ui::MainWindow *ui;
    QAction *migrateAction;
    QAction *assessAction;
    QAction *helpAction;
    QAction *viewResultsAction;
    QAction *openReportAction;
    QAction *terminateAction;

    QSettings *settings;

    QString lastSelectedSourceDir;
    QString lastSelectedTargetDir;
    QString lastSelectedReportTemplate;
    int itemsInHistory;

    QProcess sqlinesProcess;
    bool isAssessment;
    bool started;
    bool terminated;

    QString reportFile;

private:

    void loadOptions();
    void saveOptions();
    void startSqlines();

    QStringList getSourceTargetList();
    QString getCmdType(QString type);

    void saveComboBoxToIni(char const *optionName, QComboBox *comboBox);

    void saveEditLineToComboBox();
    void saveEditLineToComboBox(QComboBox *comboBox);
};

#pragma clang diagnostic ignored "-Wredundant-parens"

#endif // MAINWINDOW_H
