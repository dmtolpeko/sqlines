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
#include "QLabel"
#include "QHBoxLayout"
#include "QDesktopServices"
#include "QUrl"
#include "QComboBox"
#include "QSplitter"
#include "QMessageBox"
#include "QApplication"
#include "QStyleFactory"
#include "QFile"
#include "QTextStream"
#include "QDir"
#include "QFileDialog"

#include "aboutdialog.h"
#include "options.h"
#include "scripttab.h"
#include "file.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _sourceComboBox = NULL;
    _targetComboBox = NULL;

    _running_tab = NULL;

    ui->setupUi(this);
    initializeComponents();
}

MainWindow::~MainWindow()
{
    saveOptions();

    delete ui;

    if(settings)
        delete settings;
}

void MainWindow::initializeComponents()
{
    QCoreApplication::setApplicationName("SQLines-Studio");
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "SQLines",
                             "SQLines-Studio");

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(createNewScript()));
    connect(ui->actionOpen_file, SIGNAL(triggered()), this, SLOT(openFileDialog()));
    connect(ui->actionRun_Conversion, SIGNAL(triggered()), this, SLOT(convert()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(showOptionsDialog()));
    connect(ui->actionOnline_conversion, SIGNAL(triggered()), this, SLOT(openOnlineConversion()));
    connect(ui->actionOnline_help, SIGNAL(triggered()), this, SLOT(openOnlineHelp()));
    connect(ui->actionContact_us, SIGNAL(triggered()), this, SLOT(openContactUs()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitApp()));

    // UI style menu items (View menu)
    connect(ui->actionWindows_7, SIGNAL(triggered()), this, SLOT(setWindows7Style()));
    connect(ui->actionWindows_XP, SIGNAL(triggered()), this, SLOT(setWindowsXPStyle()));
    connect(ui->actionWindows, SIGNAL(triggered()), this, SLOT(setWindowsStyle()));
    connect(ui->actionMacintosh, SIGNAL(triggered()), this, SLOT(setMacintoshStyle()));
    connect(ui->actionCDE, SIGNAL(triggered()), this, SLOT(setCDEStyle()));
    connect(ui->actionMotif, SIGNAL(triggered()), this, SLOT(setMotifStyle()));
    connect(ui->actionPlastigue, SIGNAL(triggered()), this, SLOT(setPlastiqueStyle()));
    connect(ui->actionCleanlooks, SIGNAL(triggered()), this, SLOT(setCleanlooksStyle()));
    connect(ui->actionDefaultStyle, SIGNAL(triggered()), this, SLOT(setDefaultStyle()));

    stylesMap["windowsvista"] = ui->actionWindows_7;
    stylesMap["windowsxp"] = ui->actionWindows_XP;
    stylesMap["windows"] = ui->actionWindows;
    stylesMap["macintosh"] = ui->actionMacintosh;
    stylesMap["cde"] = ui->actionCDE;
    stylesMap["motif"] = ui->actionMotif;
    stylesMap["plastique"] = ui->actionPlastigue;
    stylesMap["cleanlooks"] = ui->actionCleanlooks;
    stylesMap["default"] = ui->actionDefaultStyle;

    connect(ui->mainTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeMainTab(int)));

    connect(&sqlinesProcess, SIGNAL(started()), this, SLOT(sqlinesStarted()));
    connect(&sqlinesProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
                SLOT(sqlinesFinished(int, QProcess::ExitStatus)));
    connect(&sqlinesProcess, SIGNAL(error(QProcess::ProcessError)), this,
                SLOT(sqlinesError(QProcess::ProcessError)));

    createToolbar();
    createCenterArea();

    loadOptions();
}

// Run the conversion in the current tab
void MainWindow::convert()
{
    _running_tab = (ScriptTab*)ui->mainTabWidget->currentWidget();

    if(_running_tab == NULL)
        return;

    ui->actionRun_Conversion->setDisabled(true);

    // Create or save if required and return the name of the source file
    QString source_file = _running_tab->GetSourceFile();

    // Start command line tool
    startSqlines(source_file);
}

// Start SQLines command line tool
void MainWindow::startSqlines(QString &source_file)
{
    if(_sourceComboBox == NULL || _targetComboBox == NULL)
        return;

    QStringList args;

    // Source type
    args.append("-s = " + getCmdType(_sourceComboBox->currentText()));

    // Target type
    args.append("-t = " + getCmdType(_targetComboBox->currentText()));

    // Source file
    args.append("-in = " + source_file);

    sqlinesProcess.start("sqlines.exe", args);
}

// SQLines command line process successfully started
void MainWindow::sqlinesStarted()
{
}

// SQLines command line process finished
void MainWindow::sqlinesFinished(int, QProcess::ExitStatus)
{
   QString out = "convert1_out.sql";

   if(_running_tab != NULL)
       _running_tab->SetTargetFile(out);

   ui->actionRun_Conversion->setDisabled(false);
}

// handling errors while launching SQLines command line tool
void MainWindow::sqlinesError(QProcess::ProcessError)
{
    QMessageBox::critical(this, "SQLines Studio", "Starting sqlines.exe - " + sqlinesProcess.errorString());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->verticalLayoutWidget->resize(ui->centralWidget->size());
    QMainWindow::resizeEvent(event);
}

// Load application options
void MainWindow::loadOptions()
{
    if(!settings)
        return;

    QString workDir = settings->value("working_directory").toString();
    if(workDir.isEmpty())
        workingDirectory = QDir::toNativeSeparators(
                    QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) +
                    "/SQLines");
    else
        workingDirectory = workDir;

    fileNamePrefix = settings->value("filename_prefix", "Conversion").toString();

    // check which UI styles are available on the current system
    QMapIterator<QString, QAction*> map(stylesMap);
    while (map.hasNext()) {
        map.next();

        if(!QStyleFactory::keys().contains(map.key(), Qt::CaseInsensitive) &&
            QString::compare(map.key(), "default", Qt::CaseInsensitive))
            map.value()->setDisabled(true);
    }

    // define default UI style
    defaultStyle = QApplication::style()->objectName();
    currentStyle = defaultStyle;

    setUIStyle(settings->value("ui_style", "default").toString());

    // The last source type
    QString source = settings->value(INI_SOURCE_TYPE).toString();

    if(source.isEmpty() == false && _sourceComboBox != NULL)
        _sourceComboBox->setCurrentIndex(_sourceComboBox->findText(source));

    // The last target type
    QString target = settings->value(INI_TARGET_TYPE).toString();

    if(target.isEmpty() == false && _targetComboBox != NULL)
        _targetComboBox->setCurrentIndex(_targetComboBox->findText(target));

    // Last directory in Open dialog
    lastOpenDir = settings->value(INI_LAST_OPEN_DIR).toString();
}

// Save application options
void MainWindow::saveOptions()
{
    if(!settings)
        return;

    // Main window size and position
    settings->setValue("window_size", size());
    settings->setValue("window_position", pos());

    settings->setValue("ui_style", currentStyle);

    // Selected source and target types
    settings->setValue(INI_SOURCE_TYPE, _sourceComboBox->currentText());
    settings->setValue(INI_TARGET_TYPE, _targetComboBox->currentText());

    // Last selected directory in "Open File Dialog"
    settings->setValue(INI_LAST_OPEN_DIR, lastOpenDir);
}

// create toolbar
void MainWindow::createToolbar()
{
    QLabel *sourceLabel = new QLabel("<b>Source:</b>");
    _sourceComboBox = new QComboBox();

    QLabel *targetLabel = new QLabel("<b>Target:</b>");
    _targetComboBox = new QComboBox();

    // get source and target types from ui_names.txt
    QStringList typesList = getSourceTargetList();

    // fill source and target types comboboxes
    _sourceComboBox->addItems(typesList);
    _targetComboBox->addItems(typesList);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(sourceLabel);
    layout->addWidget(_sourceComboBox);
    layout->addWidget(targetLabel);
    layout->addWidget(_targetComboBox);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    ui->mainToolBar->insertWidget(NULL, widget);
}

// create center area with splitter
void MainWindow::createCenterArea()
{
    //ui->mainTabWidget->setTabsClosable(true);
    ui->mainTabWidget->setMovable(true);

    createNewScript();
}

// Open a file or multiple files
void MainWindow::openFileDialog()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File(s)"), lastOpenDir, "");

    // Open all selected files
    for(int i = 0; i < fileNames.length(); i++)
    {
        QString path = fileNames.at(i);

        std::string dir;
        std::string file;

        File::SplitDirectoryAndFile(path.toStdString().c_str(), dir, file);

        //lastOpenDir = dir;
    }
}

// create New script tab (called from menu)
void MainWindow::createNewScript()
{
    ScriptTab *tab = new ScriptTab();
    QString tab_name = "convert1.sql";

    tab->SetTabName(tab_name);
    tab->SetSourceFile(tab_name);

    // Create a new tab and activate it
    int newInd = ui->mainTabWidget->addTab(tab, tab_name);
    ui->mainTabWidget->setCurrentIndex(newInd);
}

void MainWindow::createNewScript(QString *)
{
}


// closing a main tab
void MainWindow::closeMainTab(int tabIndex)
{
     // removed from TabWideget only, object still exists
     ui->mainTabWidget->removeTab(tabIndex);
}

// set specified UI style
void MainWindow::setUIStyle(QString inStyle)
{
    if(!QString::compare(currentStyle, inStyle))
        return;

    // change Checked menu item
    stylesMap[currentStyle]->setChecked(false);
    stylesMap[inStyle]->setChecked(true);

    QString style;

    if(QString::compare(inStyle, "default", Qt::CaseInsensitive))
    {
        style = inStyle;
        currentStyle = inStyle;
    }
    else
    {
        style = defaultStyle;
        currentStyle = "default";
    }

    QStyle *qstyle = QStyleFactory::create(style);

    if(qstyle)
    {
       QApplication::setStyle(qstyle);
       QApplication::setPalette(qstyle->standardPalette());
    }
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

// show About Dialog
void MainWindow::showAboutDialog()
{
    AboutDialog about(this);
    about.exec();
}

// show Options Dialog
void MainWindow::showOptionsDialog()
{
    Options options(this);
    options.exec();
}

// open Online conversion
void MainWindow::openOnlineConversion()
{
    QDesktopServices::openUrl(QUrl("http://www.sqlines.com/online"));
}

// open Online help
void MainWindow::openOnlineHelp()
{
    QDesktopServices::openUrl(QUrl("http://www.sqlines.com"));
}

// Open Contact Us page
void MainWindow::openContactUs()
{
    QDesktopServices::openUrl(QUrl("http://www.sqlines.com/contact-us"));
}

// Exit the application
void MainWindow::exitApp()
{
    QApplication::quit();
}
