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

#include "options.h"
#include "ui_options.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);

    QCoreApplication::setApplicationName("SQLines-Studio");
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "SQLines",
                             "SQLines-Studio");
    loadOptions();

    connect(ui->workdirButton, SIGNAL(pressed()), this, SLOT(selectWorkingDirectory()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveDialogData()));
}

Options::~Options()
{
    //SaveOptions();
    delete settings;

    delete ui;
}


// select new working directory to save data files
void Options::selectWorkingDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select new working directory"),
      workingDirectory, QFileDialog::ShowDirsOnly);
    if(!dir.isEmpty())
    {
      // we will change working directory variable only on Save, now change UI only
      ui->workdirEdit->setText(dir);
    }
}

// load SQLines Studio options and saved data
void Options::loadOptions()
{
    // get working directory
    workingDirectory = settings->value(OPTION_WORKDIR).toString();
    if(workingDirectory.isEmpty())
    {
        // reading application data settings
        #if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        workingDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        #else
        workingDirectory = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
        #endif
        if(workingDirectory.isEmpty())
            workingDirectory = QDir::toNativeSeparators(QDir::homePath());
    }

    ui->workdirEdit->setText(workingDirectory);
}

// save SQLines Studio options
void Options::saveOptions()
{
    // save working directory
    settings->setValue(OPTION_WORKDIR, workingDirectory);

}

// Save options that was set in the Options dialog

void Options::saveDialogData()
{
    // save working directory
    workingDirectory = ui->workdirEdit->text();
}



