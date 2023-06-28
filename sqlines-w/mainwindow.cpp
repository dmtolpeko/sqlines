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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->sourceDirComboBox->setInsertPolicy(QComboBox::InsertAtTop);
    ui->targetDirComboBox->setInsertPolicy(QComboBox::InsertAtTop);

    QCoreApplication::setApplicationName("sqlinesw");
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "SQLines", "sqlinesw");

    loadOptions();

    connect(ui->sourceDirButton, SIGNAL(pressed()), this, SLOT(openSourceDirectory()));
    connect(ui->targetDirButton, SIGNAL(pressed()), this, SLOT(openTargetDirectory()));

    connect(ui->migrateButton, SIGNAL(pressed()), this, SLOT(migrate()));
    connect(ui->viewResultsButton, SIGNAL(pressed()), this, SLOT(viewResults()));
    connect(ui->exitButton, SIGNAL(pressed()), this, SLOT(exitApp()));

    helpAction = new QAction(this);
    helpAction->setShortcut(QKeySequence::HelpContents);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(viewHelp()));
    this->addAction(helpAction);

    migrateAction = new QAction(this);
    migrateAction->setShortcut(QKeySequence("F5"));
    connect(migrateAction, SIGNAL(triggered()), this, SLOT(migrate()));
    this->addAction(migrateAction);

    assessAction = new QAction(this);
    assessAction->setShortcut(QKeySequence("F6"));
    connect(assessAction, SIGNAL(triggered()), this, SLOT(assess()));
    this->addAction(assessAction);

    viewResultsAction = new QAction(this);
    viewResultsAction->setShortcut(QKeySequence("F3"));
    connect(viewResultsAction, SIGNAL(triggered()), this, SLOT(viewResults()));
    this->addAction(viewResultsAction);

    openReportAction = new QAction(this);
    openReportAction->setShortcut(QKeySequence("F4"));
    connect(openReportAction, SIGNAL(triggered()), this, SLOT(openReport()));
    this->addAction(openReportAction);

    terminateAction = new QAction(this);
    terminateAction->setShortcut(QKeySequence("Esc"));
    connect(terminateAction, SIGNAL(triggered()), this, SLOT(terminate()));
    this->addAction(terminateAction);

    connect(&sqlinesProcess, SIGNAL(started()), this, SLOT(sqlinesStarted()));
    connect(&sqlinesProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(sqlinesFinished(int, QProcess::ExitStatus)));
    connect(&sqlinesProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(showSqlinesOutput()));
    connect(&sqlinesProcess, SIGNAL(error(QProcess::ProcessError)), this,
            SLOT(sqlinesError(QProcess::ProcessError)));

    isAssessment = false;
    started = false;
    terminated = false;
}

MainWindow::~MainWindow()
{
    saveOptions();

    if(settings)
     delete settings;

    if(migrateAction)
     delete migrateAction;

    if(assessAction)
     delete assessAction;

    if(helpAction)
     delete helpAction;

    if(viewResultsAction)
     delete viewResultsAction;

    if(openReportAction)
     delete openReportAction;

    if(terminateAction)
     delete terminateAction;

    delete ui;
}

// Run migration
void MainWindow::migrate()
{
   isAssessment = false;

   startSqlines();
}

// Run assessment
void MainWindow::assess()
{
    isAssessment = true;

    startSqlines();
}

// Start SQLines command line tool
void MainWindow::startSqlines()
{
    // Save options as some edit lines may be not in comboboxes yet (currentText will return null)
    saveOptions();

    QStringList args;
    QString error;

    // Check that a source file is specified
    if(ui->sourceDirComboBox->currentText().isEmpty())
        error = "Specify a source SQL file or directory.";

    if(!error.isEmpty())
    {
        QMessageBox::critical(this, "SQLines", error);
        return;
    }

    // Source type
    if(ui->sourceTypeComboBox->currentText().isEmpty() == false)
        args.append("-s = " + getCmdType(ui->sourceTypeComboBox->currentText()));

    // Target type
    if(ui->targetTypeComboBox->currentText().isEmpty() == false)
        args.append("-t = " + getCmdType(ui->targetTypeComboBox->currentText()));

    // Source files
    args.append("-in = " + ui->sourceDirComboBox->currentText());

    // Optional target files
    if(ui->targetDirComboBox->currentText().isEmpty() == false)
        args.append("-out = " + ui->targetDirComboBox->currentText());

    sqlinesProcess.start("sqlines.exe", args);
}

// SQLines command line process successfully started
void MainWindow::sqlinesStarted()
{
    ui->logPlainTextEdit->clear();

    ui->migrateButton->setEnabled(false);

    started = true;
    terminated = false;
}

// SQLines command line process finished
void MainWindow::sqlinesFinished(int, QProcess::ExitStatus)
{
    ui->migrateButton->setEnabled(true);

    started = false;
    terminated = false;
}

// Terminate SQLines command line process
void MainWindow::terminate()
{
    if(!started)
        return;

    sqlinesProcess.kill();
    terminated = true;

    ui->logPlainTextEdit->appendHtml("<br><b>Terminated by user</b>");
}

// show SQLines command line output
void MainWindow::showSqlinesOutput()
{
    QString fullOutput = sqlinesProcess.readAllStandardOutput();
    QString output;

    // the output can contain progress information that is updated
    // using ASCII \b sequences (backspace)

    ui->logPlainTextEdit->setOverwriteMode(true);

    for(int i=0; i < fullOutput.size(); i++)
    {
        QChar cur = fullOutput.at(i);

        // backspace found in the output
        if(cur == '\b')
        {
            // flush current data
            if(!output.isEmpty())
            {
                ui->logPlainTextEdit->insertPlainText(output);
                output.clear();
            }

            // move cursor one character backward
            // move with selection so the next insert will remove selected characters
            ui->logPlainTextEdit->moveCursor(QTextCursor::Left, QTextCursor::KeepAnchor);
        }
        else output.append(cur);

    }

    if(!output.isEmpty())
     ui->logPlainTextEdit->insertPlainText(output);

    ui->logPlainTextEdit->moveCursor(QTextCursor::End);
}

// handling errors while launching SQLines command line tool
void MainWindow::sqlinesError(QProcess::ProcessError)
{
    // this function is also called when process is terminated by user
    // do not show message box is this case
    if(terminated)
        return;

    QMessageBox::critical(this, "SQLines", "Starting sqlines command line - " + sqlinesProcess.errorString());
}

// Open dialog to select a source directory
void MainWindow::openSourceDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select a Source Directory"),
                                                    lastSelectedSourceDir);

    if(!dir.isEmpty())
    {
       lastSelectedSourceDir = dir;
       ui->sourceDirComboBox->insertItem(0, QDir::toNativeSeparators(dir + "/*.*"));
       ui->sourceDirComboBox->setCurrentIndex(0);
    }
}

// Open dialog to select a target directory
void MainWindow::openTargetDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select a Target Directory"),
                                                    lastSelectedTargetDir);

    if(!dir.isEmpty())
    {
       lastSelectedTargetDir = dir;
       ui->targetDirComboBox->insertItem(0, QDir::toNativeSeparators(dir));
       ui->targetDirComboBox->setCurrentIndex(0);
    }

}

// Load options, previous settings and history
void MainWindow::loadOptions()
{
    // read main window position
    QPoint pos = settings->value(INI_MAINWINDOW_POS, QPoint(0,0)).toPoint();

    // if position is not set, center the window
    if(pos.isNull())
    {
      QRect r = geometry();
      r.moveCenter(QApplication::desktop()->availableGeometry().center());
      setGeometry(r);
    }
    // otherwise move to the previous position
    else
      move(pos);

    // Get source and target types
    QStringList typesList = getSourceTargetList();

    // Fill source and target types comboboxes
    ui->sourceTypeComboBox->addItems(typesList);
    ui->targetTypeComboBox->addItems(typesList);

    QString prevSourceType = settings->value(INI_SOURCE_TYPE).toString();
    // restore the value only if exists in the list
    if(!prevSourceType.isEmpty() && typesList.contains(prevSourceType, Qt::CaseInsensitive))
     ui->sourceTypeComboBox->setCurrentIndex(ui->sourceTypeComboBox->findText(prevSourceType));

    QString prevTargetType = settings->value(INI_TARGET_TYPE).toString();
    // restore the value only if exists in the list
    if(!prevTargetType.isEmpty() && typesList.contains(prevTargetType, Qt::CaseInsensitive))
     ui->targetTypeComboBox->setCurrentIndex(ui->targetTypeComboBox->findText(prevTargetType));

    // Last source directory in Open dialog
    lastSelectedSourceDir = settings->value(INI_LAST_SOURCE_DIR).toString();

    QStringList sourceDirs = settings->value(INI_SOURCE_DIRS).toStringList();

    if(sourceDirs.isEmpty() == true)
        ui->sourceDirComboBox->insertItem(0, "c:\\scripts\\*.sql");
    else
        ui->sourceDirComboBox->addItems(sourceDirs);

    ui->sourceDirComboBox->setCurrentIndex(settings->value(INI_LAST_CUR_SOURCE_DIR).toInt());

    // target directories history
    lastSelectedTargetDir = settings->value(INI_LAST_TARGET_DIR).toString();

    QStringList targetDirs = settings->value(INI_TARGET_DIRS).toStringList();

    if(targetDirs.isEmpty() == true)
        ui->targetDirComboBox->insertItem(0, "c:\\temp\\scripts\\");
    else
        ui->targetDirComboBox->addItems(targetDirs);

    ui->targetDirComboBox->setCurrentIndex(settings->value(INI_LAST_CUR_TARGET_DIR).toInt());
}

// Save options, settings and history
void MainWindow::saveOptions()
{
    // main window position
    settings->setValue(INI_MAINWINDOW_POS, pos());

    // save data from edit lines to combo box list
    saveEditLineToComboBox();

    // selected source and target types
    settings->setValue(INI_SOURCE_TYPE, ui->sourceTypeComboBox->currentText());
    settings->setValue(INI_TARGET_TYPE, ui->targetTypeComboBox->currentText());

    // last selected source directory in "Browse for Dir" and edit line
    settings->setValue(INI_LAST_SOURCE_DIR, lastSelectedSourceDir);
    settings->setValue(INI_LAST_CUR_SOURCE_DIR, ui->sourceDirComboBox->currentIndex());
    saveComboBoxToIni(INI_SOURCE_DIRS, ui->sourceDirComboBox);

    // last selected target directory in "Browse for Dir" and edit line
    settings->setValue(INI_LAST_TARGET_DIR, lastSelectedTargetDir);
    settings->setValue(INI_LAST_CUR_TARGET_DIR, ui->targetDirComboBox->currentIndex());
    saveComboBoxToIni(INI_TARGET_DIRS, ui->targetDirComboBox);

    // command line combobox and current value
    //saveComboBoxToIni(INI_COMMAND_LINE, ui->commandLineComboBox);
    //settings->setValue(INI_CUR_COMMAND_LINE, ui->commandLineComboBox->currentIndex());

    // other options
    //settings->setValue(INI_OPEN_REPORT, ui->openReportCheckBox->isChecked());
    //settings->setValue(INI_SWITCH_TO_LOG, ui->switchToLogCheckBox->isChecked());
    //settings->setValue(INI_ITEMS_IN_HISTORY, ui->itemsInHistoryLineEdit->text());
}

// Get the list of available source and target types
QStringList MainWindow::getSourceTargetList()
{
    QStringList typesList;

    typesList.append("Greenplum");
    typesList.append("Hive");
    typesList.append("IBM DB2");
    typesList.append("Informix");
    typesList.append("MariaDB");
    typesList.append("Microsoft SQL Server");
    typesList.append("MySQL");
    typesList.append("Oracle");
    typesList.append("Netezza");
    typesList.append("PostgreSQL");
    typesList.append("Sybase ASE");
    typesList.append("Sybase SQL Anywhere");
    typesList.append("Sybase Advantage");
    typesList.append("Teradata");
    typesList.append("EsgynDB");

    return typesList;
}

// Get command line database type from name
QString MainWindow::getCmdType(QString type)
{
    QString cmd;

    if(type == "Oracle")
        cmd = "oracle";
    else
    if(type == "Microsoft SQL Server")
        cmd = "sql";
    else
    if(type == "IBM DB2")
        cmd = "db2";
    else
    if(type == "Sybase ASE")
        cmd = "sybase";
    else
    if(type == "Sybase SQL Anywhere")
        cmd = "asa";
    else
    if(type == "Sybase Advantage")
        cmd = "ads";
    else
    if(type == "Informix")
        cmd = "informix";
    else
    if(type == "MariaDB")
        cmd = "mariadb";
    else
    if(type == "MySQL")
        cmd = "mysql";
    else
    if(type == "PostgreSQL")
        cmd = "postgresql";
    else
    if(type == "Teradata")
        cmd = "teradata";
    else
    if(type == "Greenplum")
        cmd = "greenplum";
    else
    if(type == "Netezza")
        cmd = "netezza";
    else
    if(type == "Hive")
        cmd = "hive";
    else
    if(type == "EsgynDB")
        cmd = "esgyndb";

    return cmd;
}

// clear all history items
void MainWindow::clearAll()
{
    ui->sourceTypeComboBox->setCurrentIndex(0);
    ui->sourceDirComboBox->clear();
    ui->sourceDirComboBox->clearEditText();

    ui->targetTypeComboBox->setCurrentIndex(0);
    ui->targetDirComboBox->clear();
    ui->targetDirComboBox->clearEditText();

    ui->logPlainTextEdit->clear();
}

// open OS file browser to see the conversion results
void MainWindow::viewResults()
{
    QString path = ui->targetDirComboBox->currentText();

    if(!path.isEmpty())
    {
      if(QDesktopServices::openUrl(QUrl("file:///" + path)))
       return;
    }

    // path can be empty or incorrect
    path = QDir::toNativeSeparators(QApplication::applicationDirPath());
    QDesktopServices::openUrl(QUrl("file:///" + path));
}

// Open report
void MainWindow::openReport()
{
    if(!reportFile.isEmpty())
    {
      if(QDesktopServices::openUrl(QUrl("file:///" + reportFile)))
       return;
    }
}

// view online help
void MainWindow::viewHelp()
{
    QDesktopServices::openUrl(QUrl("http://www.sqlines.com/doc/sqlines-w-guide"));
}

// exit the application
void MainWindow::exitApp()
{
    QApplication::quit();
}

// save combobox data to INI file
void MainWindow::saveComboBoxToIni(char const *optionName, QComboBox *comboBox)
{
    if(!comboBox->count())
      return;

    QStringList list;

    for(int i = 0; i < comboBox->count(); i++)
      list.append(comboBox->itemText(i));

    settings->setValue(QString(optionName), list);
}

// save data from edit lines to combo box list
void MainWindow::saveEditLineToComboBox()
{
    saveEditLineToComboBox(ui->sourceDirComboBox);
    saveEditLineToComboBox(ui->targetDirComboBox);
}

void MainWindow::saveEditLineToComboBox(QComboBox *comboBox)
{
    QString line = comboBox->lineEdit()->text();

    // check that current edit line value is not in combo box
    if(!line.isEmpty() && comboBox->findText(line) == -1)
    {
        comboBox->insertItem(0, line);
        comboBox->setCurrentIndex(0);
    }
}
