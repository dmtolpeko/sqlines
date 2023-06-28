/**
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

#include <QApplication>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QAction>
#include <QFont>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>

#include "mainwindow.hpp"
#include "centralwidget.hpp"

using namespace ui;

MainWindow::MainWindow(const QStringList& sourceModes,
                       const QStringList& targetModes,
                       QWidget* parent) noexcept
 :  QMainWindow(parent)
{
    this->sourceComboBox.addItems(sourceModes);
    this->targetComboBox.addItems(targetModes);

    connect(&this->tabWidget, &ui::TabWidget::tabCloseRequested,
            this, &ui::MainWindow::closeTabPressed);

    connect(this->tabWidget.tabBar(), &QTabBar::tabMoved,
            this, &ui::MainWindow::tabMoved);

    connect(&this->tabWidget, &ui::TabWidget::currentChanged,
            this, &ui::MainWindow::tabChanged);

    connect(&this->sourceComboBox, &QComboBox::currentTextChanged,
            this, [this] {
                emit sourceModeChanged(this->tabWidget.currentIndex(),
                                       this->sourceComboBox.currentText());
            });
    
    connect(&this->targetComboBox, &QComboBox::currentTextChanged,
            this, [this] {
                emit targetModeChanged(this->tabWidget.currentIndex(),
                                       this->targetComboBox.currentText());
            });
    
    paint();
    makeToolBar();
    makeMenuBar();
}

void MainWindow::showConversionStart(int tabIndex) noexcept
{
    ui::CentralWidget& widget = this->tabWidget.getWidget(tabIndex);
    widget.makeInactive();
}

void MainWindow::showConversionEnd(int tabIndex) noexcept
{
    ui::CentralWidget& widget = this->tabWidget.getWidget(tabIndex);
    widget.makeActive();
    widget.focusOn(CentralWidget::FieldInFocus::TargetField);
}

void MainWindow::showConversionError(int tabIndex,
                                     const QString& errorString) noexcept
{
    QString tabNumber = QString::number(tabIndex + 1);
    QMessageBox::critical(this,
                          "Conversion error",
                          "Conversion error in tab " + tabNumber + "\n\n" +
                          errorString);

    ui::CentralWidget& widget = this->tabWidget.getWidget(tabIndex);
    widget.makeActive();
    widget.focusOn(CentralWidget::FieldInFocus::SourceField);
}

void MainWindow::showFileOpeningError(const QString& errorString) noexcept
{
    QMessageBox::critical(this, "Filesystem error", errorString);
}

void MainWindow::showTabTitle(int tabIndex, const QString& data) noexcept
{
    // Prevent emission of the tabTitleChanged signals
    QSignalBlocker block(this);

    this->tabWidget.setTabTitle(tabIndex, data);
}

void MainWindow::showSourceMode(const QString& mode) noexcept
{
    // Prevent emission of the sourceModeChanged signals
    QSignalBlocker block(this);

    this->sourceComboBox.setCurrentText(mode);
}

void MainWindow::showTargetMode(const QString& mode) noexcept
{
    // Prevent emission of the targetModeChanged signals
    QSignalBlocker block(this);

    this->targetComboBox.setCurrentText(mode);
}

void MainWindow::showSourceData(int tabIndex, const QString& data) noexcept
{
    // Prevent emission of the sourceDataChanged signals
    QSignalBlocker block(this);

    CentralWidget& widget = this->tabWidget.getWidget(tabIndex);
    widget.setSourceData(data);
}

void MainWindow::showTargetData(int tabIndex, const QString& data) noexcept
{
    // Prevent emission of the targetDataChanged signals
    QSignalBlocker block(this);

    CentralWidget& widget = this->tabWidget.getWidget(tabIndex);
    widget.setTargetData(data);
}

void MainWindow::showFilePath(const QString& path) noexcept
{
    // Show message in the statusbar
    QString message;
    if (path.isEmpty()) {
        message = "Source: Editor";
    } else {
        message = "Source: " + path;
    }

    statusBar()->showMessage(message);
}

void MainWindow::openNewTab(bool lineNumberEnabled,
                            bool highlighterEnabled,
                            bool wrappingEnabled) noexcept
{
    auto centralWidget = new CentralWidget(lineNumberEnabled,
                                           highlighterEnabled,
                                           wrappingEnabled,
                                           this);

    connect(centralWidget, &ui::CentralWidget::sourceDataChanged,
            this, [this](const QTextDocument& data)
            { emit sourceDataChanged(this->tabWidget.currentIndex(),
                                     data.toPlainText()); });

    connect(centralWidget, &ui::CentralWidget::targetDataChanged,
            this, [this](const QTextDocument& data)
            { emit targetDataChanged(this->tabWidget.currentIndex(),
                                     data.toPlainText()); });

    connect(centralWidget, &ui::CentralWidget::fileOpened,
            this, [this](const QString& filePath)
            { emit fileOpened(this->tabWidget.currentIndex(),
                              this->tabWidget.currentWidget().sourceData().toPlainText(),
                              filePath); });

    connect(centralWidget, &ui::CentralWidget::fileOpened,
            this, [this](const QString& filePath)
            { emit tabTitleChanged(this->tabWidget.currentIndex(),
                                   QFileInfo(filePath).fileName()); });

    connect(centralWidget, &ui::CentralWidget::fileOpened,
            &this->tabWidget, [this](const QString& filePath)
            { this->tabWidget.setTabTitle(this->tabWidget.currentIndex(),
                                          QFileInfo(filePath).fileName()); });

    auto focusChange = [this](CentralWidget::FieldInFocus inFocus) {
        if (inFocus == CentralWidget::FieldInFocus::SourceField) {
            emit focusChanged(this->tabWidget.currentIndex(),
                              view::IMainWindow::FieldInFocus::SourceField);

        } else if (inFocus == CentralWidget::FieldInFocus::TargetField) {
            emit focusChanged(this->tabWidget.currentIndex(),
                              view::IMainWindow::FieldInFocus::TargetField);
        }
    };

    connect(centralWidget, &ui::CentralWidget::focusChanged,
            this, focusChange);

    this->tabWidget.addTab(*centralWidget);
}

void MainWindow::closeTab(int tabIndex) noexcept
{
    if (this->tabWidget.count() != 0) {
        // Prevent emission of the tabChanged signals
        QSignalBlocker block(this);

        delete &this->tabWidget.getWidget(tabIndex);
    }

    if (this->tabWidget.count() != 0) {
        this->tabWidget.currentWidget().focusOn(CentralWidget::FieldInFocus::SourceField);
    }
}

QString MainWindow::chooseFileToOpen(const QString& currentDir) noexcept
{
    return QFileDialog::getOpenFileName(this, "", currentDir);
}

QString MainWindow::choseFileToCreate(const QString& currentDir) noexcept
{
    return QFileDialog::getSaveFileName(this, "", currentDir);
}

int MainWindow::currentTabIndex() const noexcept
{
    return this->tabWidget.currentIndex();
}

QString MainWindow::currentTabTitle() const noexcept
{
    return this->tabWidget.tabTitle(this->tabWidget.currentIndex());
}

QString MainWindow::currentSourceMode() const noexcept
{
    return this->sourceComboBox.currentText();
}

QString MainWindow::currentTargetMode() const noexcept
{
    return this->targetComboBox.currentText();
}

view::IMainWindow::FieldInFocus MainWindow::inFocus() const noexcept
{
    CentralWidget::FieldInFocus inFocus = this->tabWidget.currentWidget().inFocus();

    if (inFocus == CentralWidget::FieldInFocus::SourceField) {
        return view::IMainWindow::FieldInFocus::SourceField;
    } else {
        return view::IMainWindow::FieldInFocus::TargetField;
    }
}

void MainWindow::reset() noexcept
{
    // Prevent emission of the tabChanged signals
    QSignalBlocker block(this);

    for (auto i = 0; i < this->tabWidget.count(); i++) {
        closeTab(i);
    }
}

int MainWindow::height() const noexcept
{
    return QMainWindow::height();
}

int MainWindow::width() const noexcept
{
    return QMainWindow::width();
}

int MainWindow::posX() const noexcept
{
    return x();
}

int MainWindow::posY() const noexcept
{
    return y();
}

void MainWindow::resize(int width, int height) noexcept
{
    QMainWindow::resize(width, height);
}

void MainWindow::move(int x, int y) noexcept
{
    QMainWindow::move(x, y);
}

void MainWindow::changeTitle(const QString& title) noexcept
{
    // Prevent emission of the tabTitleChanged signals
    QSignalBlocker block(this);

    setWindowTitle(title);
}

void MainWindow::changeNumberArea(bool isOn) noexcept
{
    for (auto i = 0; i < this->tabWidget.count(); i++) {
        CentralWidget& widget = this->tabWidget.getWidget(i);
        widget.changeNumberArea(isOn);
    }
}

void MainWindow::changeHighlighting(bool isOn) noexcept
{
    for (auto i = 0; i < this->tabWidget.count(); i++) {
        CentralWidget& widget = this->tabWidget.getWidget(i);
        widget.changeHighlighting(isOn);
    }
}

void MainWindow::changeWrapping(bool isOn) noexcept
{
    for (auto i = 0; i < this->tabWidget.count(); i++) {
        CentralWidget& widget = this->tabWidget.getWidget(i);
        widget.changeWrapping(isOn);
    }
}

void MainWindow::paint() noexcept
{
    setWindowTitle(QApplication::applicationName());
    setMinimumSize(665, 250);
    resize(685, 500);
    setCentralWidget(&this->tabWidget);
    showFilePath(QString());
    show();
}

void MainWindow::makeToolBar() noexcept
{
    auto toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    setIconSize(QSize(20, 20));
    
    makeTabsSection(toolBar);
    toolBar->addSeparator();
    makeFileSection(toolBar);
    toolBar->addSeparator();
    makeToolsSection(toolBar);
    
    addToolBar(Qt::TopToolBarArea, toolBar);
}

void MainWindow::makeTabsSection(QToolBar* parent) noexcept
{
    auto newTab = new QAction("Open new tab", parent);
    newTab->setIcon(QIcon(":/opentab.svg"));
    connect(newTab, &QAction::triggered,
            this, &MainWindow::openNewTabPressed);
    
    parent->addAction(newTab);
}

void MainWindow::makeFileSection(QToolBar* parent) noexcept
{
    auto openFile = new QAction("Open file", parent);
    openFile->setIcon(QIcon(":/openfile.svg"));
    connect(openFile, &QAction::triggered,
            this, [this] { emit openFilePressed(this->tabWidget.currentIndex()); });
    
    auto saveFile = new QAction("Save file", parent);
    QIcon saveIcon(":/savefile.svg");
    saveFile->setIcon(saveIcon);
    connect(saveFile, &QAction::triggered,
            this, [this] { emit saveFilePressed(this->tabWidget.currentIndex()); });
    
    parent->addAction(openFile);
    parent->addAction(saveFile);
}

void MainWindow::makeToolsSection(QToolBar* parent) noexcept
{
    auto convert = new QAction("Run conversion", parent);
    convert->setIcon(QIcon(":/run.svg"));
    connect(convert, &QAction::triggered,
            this, [this] { emit convertPressed(this->tabWidget.currentIndex()); });

    QFont titleFont;
    titleFont.setBold(true);

    auto sourcesBoxTitle = new QLabel("Source:", parent);
    sourcesBoxTitle->setFont(titleFont);
    sourcesBoxTitle->setObjectName("ToolBarLabel");

    auto targetsBoxTitle = new QLabel("   Target:", parent);
    targetsBoxTitle->setFont(titleFont);
    targetsBoxTitle->setObjectName("ToolBarLabel");
    
    parent->addAction(convert);
    parent->addSeparator();
    parent->addWidget(sourcesBoxTitle);
    parent->addWidget(&this->sourceComboBox);
    parent->addWidget(targetsBoxTitle);
    parent->addWidget(&this->targetComboBox);
}
 
void MainWindow::makeMenuBar() noexcept
{
    auto menuBar = new QMenuBar(this);

    menuBar->addMenu(makeMainMenuTab(menuBar));
    menuBar->addMenu(makeFileMenuTab(menuBar));
    menuBar->addMenu(makeEditMenuTab(menuBar));
    menuBar->addMenu(makeViewMenuTab(menuBar));
    menuBar->addMenu(makeToolsMenuTab(menuBar));
    menuBar->addMenu(makeHelpMenuTab(menuBar));
    
    setMenuBar(menuBar);
}

QMenu* MainWindow::makeMainMenuTab(QMenuBar* parent) noexcept
{
    auto mainMenu = new QMenu("SQLines Studio", parent);
    
    auto about = new QAction("About SQLines Studio...", mainMenu);
    connect(about, &QAction::triggered,
            this, &ui::MainWindow::openAboutPressed);
    
    auto preferences = new QAction("Preferences...", mainMenu);
    preferences->setShortcut(QKeySequence::Preferences);
    connect(preferences, &QAction::triggered,
            this, &ui::MainWindow::openSettingsPressed);
    
    mainMenu->addAction(about);
    mainMenu->addSeparator();
    mainMenu->addAction(preferences);
    
    return mainMenu;
}

QMenu* MainWindow::makeFileMenuTab(QMenuBar* parent) noexcept
{
    auto fileMenu = new QMenu("File", parent);
    
    auto newTab = new QAction("New tab", fileMenu);
    newTab->setShortcut(QKeySequence::AddTab);
    connect(newTab, &QAction::triggered,
            this, &MainWindow::openNewTabPressed);

    auto closeTab = new QAction("Close tab", fileMenu);
    closeTab->setShortcut(QKeySequence::Close);
    connect(closeTab, &QAction::triggered,
            this, [this] { emit closeTabPressed(this->tabWidget.currentIndex()); });
    
    auto nextTab = new QAction("Next tab", fileMenu);
    nextTab->setShortcut(QKeySequence::Forward);
    connect(nextTab, &QAction::triggered,
            &this->tabWidget, &ui::TabWidget::nextTab);
    
    auto prevTab = new QAction("Previous tab", fileMenu);
    prevTab->setShortcut(QKeySequence::Back);
    connect(prevTab, &QAction::triggered,
            &this->tabWidget, &ui::TabWidget::prevTab);
    
    auto openFile = new QAction("Open file...", fileMenu);
    openFile->setShortcut(QKeySequence::Open);
    connect(openFile, &QAction::triggered,
            this, [this] { emit openFilePressed(this->tabWidget.currentIndex()); });
    
    auto saveFile = new QAction("Save file", fileMenu);
    saveFile->setShortcut(QKeySequence::Save);
    connect(saveFile, &QAction::triggered,
            this, [this] { emit saveFilePressed(this->tabWidget.currentIndex()); });

    auto saveFileAs = new QAction("Save as...", fileMenu);
    saveFileAs->setShortcut(QKeySequence::SaveAs);
    connect(saveFileAs, &QAction::triggered,
            this, [this] { emit saveFileAsPressed(this->tabWidget.currentIndex()); });

    fileMenu->addAction(newTab);
    fileMenu->addAction(closeTab);
    fileMenu->addAction(nextTab);
    fileMenu->addAction(prevTab);
    fileMenu->addSeparator();
    fileMenu->addAction(openFile);
    fileMenu->addAction(saveFile);
    fileMenu->addAction(saveFileAs);
    
    return fileMenu;
}

QMenu* MainWindow::makeEditMenuTab(QMenuBar* parent) noexcept
{
    auto editMenu = new QMenu("Edit", parent);
    
    auto selectAll = new QAction("Select all", editMenu);
    selectAll->setShortcut(QKeySequence::SelectAll);
    connect(selectAll, &QAction::triggered,
            this, [this] { this->tabWidget.currentWidget().inFocus().selectAll(); });
    
    auto undo = new QAction("Undo", editMenu);
    undo->setShortcut(QKeySequence::Undo);
    connect(undo, &QAction::triggered,
            this, [this] { this->tabWidget.currentWidget().inFocus().undo(); });

    auto redo = new QAction("Redo", editMenu);
    redo->setShortcut(QKeySequence::Redo);
    connect(redo, &QAction::triggered,
            this, [this] { this->tabWidget.currentWidget().inFocus().redo(); });
    
    auto cut = new QAction("Cut", editMenu);
    cut->setShortcut(QKeySequence::Cut);
    connect(cut, &QAction::triggered,
            this, [this] { this->tabWidget.currentWidget().inFocus().cut(); });
    
    auto copy = new QAction("Copy", editMenu);
    copy->setShortcut(QKeySequence::Copy);
    connect(copy, &QAction::triggered,
            this, [this] { this->tabWidget.currentWidget().inFocus().copy(); });
    
    auto paste = new QAction("Paste", editMenu);
    paste->setShortcut(QKeySequence::Paste);
    connect(paste, &QAction::triggered,
            this, [this] { this->tabWidget.currentWidget().inFocus().paste(); });
   
    editMenu->addAction(selectAll);
    editMenu->addSeparator();
    editMenu->addAction(undo);
    editMenu->addAction(redo);
    editMenu->addAction(cut);
    editMenu->addAction(copy);
    editMenu->addAction(paste);
    
    return editMenu;
}

QMenu* MainWindow::makeToolsMenuTab(QMenuBar* parent) noexcept
{
    auto toolsMenu = new QMenu("Tools", parent);
    
    auto runConversion = new QAction("Run conversion", toolsMenu);
    runConversion->setShortcut(QKeySequence::Refresh);
    connect(runConversion, &QAction::triggered,
            this, [this] { emit convertPressed(this->tabWidget.currentIndex()); });

    auto conversionUtility = new QAction("Conversion utility...", toolsMenu);
    connect(conversionUtility, &QAction::triggered,
            this, &MainWindow::openConvertUtilityPressed);

    toolsMenu->addAction(runConversion);
    toolsMenu->addSeparator();
    toolsMenu->addAction(conversionUtility);
    
    return toolsMenu;
}

QMenu* MainWindow::makeViewMenuTab(QMenuBar* parent) noexcept
{
    auto viewMenu = new QMenu("View", parent);
    
    auto zoomIn = new QAction("Zoom in", viewMenu);
    zoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(zoomIn, &QAction::triggered,
            this, [this] { this->tabWidget.currentWidget().zoomIn(); });
    
    auto zoomOut = new QAction("Zoom out", viewMenu);
    zoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOut, &QAction::triggered,
            this, [this] { this->tabWidget.currentWidget().zoomOut(); });
    
    viewMenu->addAction(zoomIn);
    viewMenu->addAction(zoomOut);
    
    return viewMenu;
}

QMenu* MainWindow::makeHelpMenuTab(QMenuBar* parent) noexcept
{
    auto helpMenu = new QMenu("Help", parent);
    
    auto onlineHelp = new QAction("Online help...", helpMenu);
    onlineHelp->setShortcut(QKeySequence::HelpContents);
    connect(onlineHelp, &QAction::triggered,
            this, &ui::MainWindow::openOnlineHelpPressed);
    
    auto contactUs = new QAction("Contact us...", helpMenu);
    connect(contactUs, &QAction::triggered,
            this, &ui::MainWindow::openContactUsPressed);

    auto openSite = new QAction("Open official site...", helpMenu);
    connect(openSite, &QAction::triggered,
            this, &ui::MainWindow::openSitePressed);

    auto openSourceCode = new QAction("View source code...", helpMenu);
    connect(openSourceCode, &QAction::triggered,
            this, &ui::MainWindow::openSourceCodePressed);
    
    helpMenu->addAction(onlineHelp);
    helpMenu->addAction(contactUs);
    helpMenu->addSeparator();
    helpMenu->addAction(openSite);
    helpMenu->addAction(openSourceCode);
    
    return helpMenu;
}
