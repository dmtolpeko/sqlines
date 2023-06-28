#include <process.h>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QAbstractItemView>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "str.h"
#include "os.h"
#include "license.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _current_command = 0;
    _validation_option = 0;

    _settings = NULL;
    _rows_read = 0;
    _rows_written = 0;
    _bytes_written = 0;

    _total_tables = 0;
    _completed_tables = 0;
    _processing_tables = 0;
    _failed_tables = 0;

    _equal_tables = 0;
    _not_equal_tables = 0;
    _failed_tables = 0;

    _ddl_executed = 0;
    _ddl_failed = 0;

    _source_all_rows = 0;
    _target_all_rows = 0;
    _command_start = 0;
    
    _sqlDataCmd.SetLicense(QApplication::arguments().at(0).toStdString().c_str());

    ui->oracleSidComboBox->setInsertPolicy(QComboBox::InsertAtTop);
    ui->sqlServerNameComboBox->setInsertPolicy(QComboBox::InsertAtTop);
    ui->tablesComboBox->setInsertPolicy(QComboBox::InsertAtTop);

    connect(ui->sourceComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(SourceChanged(QString)));
    connect(ui->targetComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(TargetChanged(QString)));

    connect(ui->transferButton, SIGNAL(pressed()), this, SLOT(RunTransfer()));
    connect(ui->validateButton, SIGNAL(pressed()), this, SLOT(RunValidation()));
    connect(ui->exitButton, SIGNAL(pressed()), this, SLOT(ExitApp()));

    connect(ui->sqlServerWindowsAuthCheckBox, SIGNAL(stateChanged(int)), this, SLOT(SqlServerWindowsAuthChecked(int)));
    connect(ui->s_sqlServerWindowsAuthCheckBox, SIGNAL(stateChanged(int)), this, SLOT(SqlServerSWindowsAuthChecked(int)));

    connect(this, SIGNAL(UpdateLogSignal()), this, SLOT(UpdateLog()), Qt::QueuedConnection);

    connect(ui->s_sqlServerTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestSourceConnection()));
    connect(ui->s_oracleTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestSourceConnection()));
    connect(ui->s_db2TestConnectionButton, SIGNAL(pressed()), this, SLOT(TestSourceConnection()));
    connect(ui->s_ifmxTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestSourceConnection()));
    connect(ui->s_mysqlTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestSourceConnection()));
    connect(ui->s_aseTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestSourceConnection()));
    connect(ui->s_asaTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestSourceConnection()));

    connect(ui->t_sqlServerTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestTargetConnection()));
    connect(ui->t_db2TestConnectionButton, SIGNAL(pressed()), this, SLOT(TestTargetConnection()));
    connect(ui->t_oracleTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestTargetConnection()));
    connect(ui->t_mysqlTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestTargetConnection()));
    connect(ui->t_pgTestConnectionButton, SIGNAL(pressed()), this, SLOT(TestTargetConnection()));

    connect(ui->tablesFileButton, SIGNAL(pressed()), this, SLOT(SelectTablesFile()));

      // Disable Terminate button
    ui->terminateButton->setEnabled(false);

    InitTransferLogView();
    InitValidationLogView();

    LoadOptions();

    _sqlDataCmd.SetConsoleFunc(this, ConsoleCallbackS);
    _sqlDataCmd.SetUiCallback(this, CallbackS);
}

MainWindow::~MainWindow()
{
    SaveOptions();

    delete _settings;
    delete ui;
}

int MainWindow::GetDbType(QString name)
{
    int type = 0;

    if(name.compare("Microsoft SQL Server") == 0)
        type = SQLDATA_SQL_SERVER;
    else
    if(name.compare("Oracle") == 0)
        type = SQLDATA_ORACLE;
    else
    if(name.compare("IBM DB2") == 0)
        type = SQLDATA_DB2;
    else
    if(name.compare("MySQL") == 0)
        type = SQLDATA_MYSQL;
    else
    if(name.compare("MariaDB") == 0)
        type = SQLDATA_MYSQL;
    else
    if(name.compare("PostgreSQL") == 0)
        type = SQLDATA_POSTGRESQL;
    else
    if(name.compare("Sybase ASE") == 0)
        type = SQLDATA_SYBASE;
    else
    if(name.compare("Informix") == 0)
        type = SQLDATA_INFORMIX;
    else
    if(name.compare("Sybase SQL Anywhere") == 0)
        type = SQLDATA_ASA;

    return type;
}

void MainWindow::SourceChanged(QString source)
{
    _source_type = GetDbType(source);
    _source = source;

    // Remove all source tabs
    ui->sourceConnectionsTabWidget->clear();

    if(_source_type == SQLDATA_SQL_SERVER)
        ui->sourceConnectionsTabWidget->insertTab(1, ui->sqlServerSourceTab, "Source - SQL Server");
    else
    if(_source_type == SQLDATA_ORACLE)
        ui->sourceConnectionsTabWidget->insertTab(1, ui->oracleSourceTab, "Source - Oracle");
    else
    if(_source_type == SQLDATA_DB2)
        ui->sourceConnectionsTabWidget->insertTab(1, ui->db2SourceTab, "Source - IBM DB2");
    else
    if(_source_type == SQLDATA_MYSQL)
        ui->sourceConnectionsTabWidget->insertTab(1, ui->mysqlSourceTab, "Source - MySQL");
    else
    if(_source_type == SQLDATA_SYBASE)
        ui->sourceConnectionsTabWidget->insertTab(1, ui->sybaseSourceTab, "Source - Sybase ASE");
    else
    if(_source_type == SQLDATA_INFORMIX)
        ui->sourceConnectionsTabWidget->insertTab(1, ui->informixSourceTab, "Source - Informix");
    else
    if(_source_type == SQLDATA_ASA)
        ui->sourceConnectionsTabWidget->insertTab(1, ui->asaSourceTab, "Source - Sybase SQL Anywhere");

    UpdateTitle();
}

void MainWindow::TargetChanged(QString target)
{
    QString title = "Target - " + target;
    _target_type = GetDbType(target);
    _target = target;

    // Remove all source tabs
    ui->targetConnectionsTabWidget->clear();

    if(_target_type == SQLDATA_SQL_SERVER)
        ui->targetConnectionsTabWidget->insertTab(1, ui->sqlServerTargetTab, "Target - SQL Server");
    else
    if(_target_type == SQLDATA_ORACLE)
        ui->targetConnectionsTabWidget->insertTab(1, ui->oracleTargetTab, "Target - Oracle");
    else
    if(_target_type == SQLDATA_DB2)
        ui->targetConnectionsTabWidget->insertTab(1, ui->db2TargetTab, "Target - IBM DB2");
    else
    if(_target_type == SQLDATA_MYSQL)
        ui->targetConnectionsTabWidget->insertTab(1, ui->mysqlTargetTab, title);
    else
    if(_target_type == SQLDATA_POSTGRESQL)
        ui->targetConnectionsTabWidget->insertTab(1, ui->pgTargetTab, "Target - PostgreSQL");

    UpdateTitle();
}

// Update application title with new migration direction
 void MainWindow::UpdateTitle()
 {
    QString title = SQLDATA_VERSION;

    title.append(" - ").append(ui->sourceComboBox->currentText());
    title.append(" to ").append(ui->targetComboBox->currentText());
    
    License *license = _sqlDataCmd.GetLicense();
    
    // Add the license message if required
	if(license != NULL && license->IsLicenseCheckRequired())
	{
		if(license->IsEmpty())
            title.append(" - FOR EVALUATION USE ONLY");
		else
			title.append(" - Licensed to ").append(license->GetName().c_str());
	}

    setWindowTitle(title);
 }

// Run Transfer command
void MainWindow::RunTransfer()
{
    _current_command = SQLDATA_CMD_TRANSFER;
    _cmdline = "-cmd=transfer ";

    PrepareCommand();
    SetTransferOptions();

    // Remove all existing rows
    while(ui->transferTableWidget->rowCount() != 0)
        ui->transferTableWidget->removeRow(0);

    // Activate the Transfer Log tab if the the current tab is not Raw Log
    if(ui->mainTabWidget->currentIndex() != 4)
        ui->mainTabWidget->setCurrentIndex(2);

    _total_tables = 0;
    ui->totalNumberTablesEdit->setText("0");

    _completed_tables = 0;
    ui->tablesCompletedEdit->setText("0");

    _processing_tables = 0;
    ui->tablesProcessingEdit->setText("0");

    _failed_tables = 0;
    ui->tablesFailedEdit->setText("0");

    _rows_read = 0;
    ui->rowsReadEdit->setText("0");

    _rows_written = 0;
    ui->rowsWrittenEdit->setText("0");

    _bytes_written = 0;

    _ddl_executed = 0;
    ui->ddlExecEdit->setText("0");

    _ddl_failed = 0;
    ui->ddlFailedEdit->setText("0");

    ui->transferTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->transferStatusComboBox->clear();
    ui->transferStatusComboBox->insertItem(0, "Reading database metadata.");

    ui->transferTimeEdit->setText("");
    ui->volumeTransferredEdit->setText("");
    ui->transferSpeedBytesEdit->setText("");
    ui->transferSpeedRowsEdit->setText("");

    ui->rawLogTextEdit->clear();

    _avail_tables.clear();
    SaveOptions();

    // Run transfer thread
    _beginthreadex(NULL, 0, &MainWindow::RunCommandThreadS, this, 0, NULL);
}

// Run Validation command
void MainWindow::RunValidation()
{
    _current_command = SQLDATA_CMD_VALIDATE;
    _cmdline="-cmd=validate ";

    PrepareCommand();

    // Set validation option
    _cmdline += " -vopt=";

    if(ui->validationOptionComboBox->currentIndex() == 0)
    {
        _cmdline += "rowcount";
        _validation_option = SQLDATA_OPT_ROWCOUNT;
    }
    else
    if(ui->validationOptionComboBox->currentIndex() == 1)
    {
        _cmdline += "rows";
        _validation_option = SQLDATA_OPT_ROWS;
    }

    // Remove all existing rows
    while(ui->validationTableWidget->rowCount() != 0)
        ui->validationTableWidget->removeRow(0);

    // Activate the Validation Log tab if the the current tab is not Raw Log
    if(ui->mainTabWidget->currentIndex() != 4)
        ui->mainTabWidget->setCurrentIndex(3);

    _total_tables = 0;
    ui->validation_totalNumberTablesEdit->setText("0");

    _equal_tables = 0;
    _not_equal_tables = 0;
    _failed_tables = 0;

    ui->equalRowsCountEdit->setText("0");
    ui->notEqualRowsCountEdit->setText("0");
    ui->failedRowsCountEdit->setText("0");

    ui->rowsInSourceEdit->setText("0");
    ui->rowsInTargetEdit->setText("0");
    ui->rowsDifferenceEdit->setText("0");

    ui->validationTimeEdit->setText("0");

    ui->validationTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->validationStatusComboBox->clear();
    ui->validationStatusComboBox->insertItem(0, "Reading database metadata.");

    _avail_tables.clear();
    ui->rawLogTextEdit->clear();

    SaveOptions();

    // Run validation thread
    _beginthreadex(NULL, 0, &MainWindow::RunCommandThreadS, this, 0, NULL);
}

// Build a command line string
int MainWindow::PrepareCommand()
{
    std::string source;
    GetSourceConnectionString(source);

    // Source database option
    _cmdline += "-sd=";
    _cmdline += source;

    std::string target;
    GetTargetConnectionString(target);

    // Target database option
    _cmdline += " -td=";
    _cmdline += target;

    // List of tables option
    if(ui->tablesListRadioButton->isChecked())
    {
        _cmdline += " -t=";
        _cmdline += ui->tablesComboBox->currentText().toStdString();
    }
    // List from a file
    else
    {
        _cmdline += " -tf=";
        _cmdline += ui->tablesFileComboBox->currentText().toStdString();
    }

    // Tables to exclude option
    std::string texcl = ui->tablesExcludeComboBox->currentText().toStdString();

    if(texcl.empty() == false)
    {
        _cmdline += " -texcl=";
        _cmdline += texcl;
    }

    std::string smap = ui->schemaMappingComboBox->currentText().toStdString();

    // Add mapping to te default schema
    if(ui->defaultSchemaCheckBox->isChecked() == true)
    {
        if(smap.empty() == false)
            smap += ',';

        smap += "*:";
    }

    // Set schema mapping
    if(smap.empty() == false)
    {
        _cmdline += " -smap=";
        _cmdline += smap;
    }

    // Number of concurrent sessions
    _cmdline += " -ss=";
    _cmdline += QString::number(ui->sessionsSpinBox->value()).toStdString();

    // Disable transfer and validate commands
    ui->transferButton->setEnabled(false);
    ui->validateButton->setEnabled(false);

    _source_all_rows = 0;
    _target_all_rows = 0;

    _command_start = (int)Os::GetTickCount();

    return 0;
}

void MainWindow::GetSourceConnectionString(std::string &source)
{
    if(_source_type == SQLDATA_ORACLE)
        GetOracleConnectionString(source, true);
    else
    if(_source_type == SQLDATA_SQL_SERVER)
        GetSqlServerConnectionString(source, true);
    else
    if(_source_type == SQLDATA_SYBASE)
        GetAseConnectionString(source);
    else
    if(_source_type == SQLDATA_INFORMIX)
        GetIfmxConnectionString(source);
    else
    if(_source_type == SQLDATA_DB2)
        GetDb2ConnectionString(source, true);
    else
    if(_source_type == SQLDATA_MYSQL)
        GetMysqlConnectionString(source, true);
    else
    if(_source_type == SQLDATA_ASA)
        GetAsaConnectionString(source);
}

void MainWindow::GetTargetConnectionString(std::string &target)
{
    if(_target_type == SQLDATA_SQL_SERVER)
        GetSqlServerConnectionString(target, false);
    else
    if(_target_type == SQLDATA_ORACLE)
        GetOracleConnectionString(target, false);
    else
    if(_target_type == SQLDATA_MYSQL)
        GetMysqlConnectionString(target, false);
    else
    if(_target_type == SQLDATA_POSTGRESQL)
        GetPgConnectionString(target);
    else
    if(_target_type == SQLDATA_DB2)
        GetDb2ConnectionString(target, false);
}

void MainWindow::TestSourceConnection()
{
    std::string source;
    GetSourceConnectionString(source);

    // Disable test connection buttons
    ui->s_sqlServerTestConnectionButton->setEnabled(false);
    ui->s_oracleTestConnectionButton->setEnabled(false);
    ui->s_db2TestConnectionButton->setEnabled(false);
    ui->s_ifmxTestConnectionButton->setEnabled(false);
    ui->s_mysqlTestConnectionButton->setEnabled(false);
    ui->s_aseTestConnectionButton->setEnabled(false);
    ui->s_asaTestConnectionButton->setEnabled(false);

    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // Try to connect
    TestConnection(source);

    // Enable test connection buttons
    ui->s_sqlServerTestConnectionButton->setEnabled(true);
    ui->s_oracleTestConnectionButton->setEnabled(true);
    ui->s_db2TestConnectionButton->setEnabled(true);
    ui->s_ifmxTestConnectionButton->setEnabled(true);
    ui->s_mysqlTestConnectionButton->setEnabled(true);
    ui->s_aseTestConnectionButton->setEnabled(true);
    ui->s_asaTestConnectionButton->setEnabled(true);

    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::TestTargetConnection()
{
    std::string target;
    GetTargetConnectionString(target);

    // Disable test connection buttons
    ui->t_sqlServerTestConnectionButton->setEnabled(false);
    ui->t_oracleTestConnectionButton->setEnabled(false);
    ui->t_db2TestConnectionButton->setEnabled(false);
    ui->t_mysqlTestConnectionButton->setEnabled(false);
    ui->t_pgTestConnectionButton->setEnabled(false);

    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // Try to connect
    TestConnection(target);

    // Enable test connection buttons
    ui->t_sqlServerTestConnectionButton->setEnabled(true);
    ui->t_oracleTestConnectionButton->setEnabled(true);
    ui->t_db2TestConnectionButton->setEnabled(true);
    ui->t_mysqlTestConnectionButton->setEnabled(true);
    ui->t_pgTestConnectionButton->setEnabled(true);

    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::TestConnection(std::string &conn)
{
    std::string error;
    std::string driver_loaded;
    std::list<std::string> search_path;

    size_t time_spent = 0;

    // Set .cfg options that may be used for connection
    _sqlDataCmd.DefineOptions(NULL);

    // Try to connect
    int rc = _sqlDataCmd.TestConnection(conn, error, driver_loaded, search_path, &time_spent);

    char time_fmt[21];
    Str::FormatTime(time_spent, time_fmt);

    QMessageBox msg;
    msg.setWindowTitle("Test Connection");

    QString text;

    // Connection failed
    if(rc == -1)
    {
        msg.setIcon(QMessageBox::Critical);

        text = "Error - ";
        text.append(error.c_str()).append(" (").append(time_fmt).append(").");
    }
    else
    {
        msg.setIcon(QMessageBox::Information);

        text = "Connection was successful (";
        text.append(time_fmt).append(").");
    }

    if(driver_loaded.empty() == false)
        text.append("\n\nClient Library:\n  ").append(driver_loaded.c_str());

    if(search_path.empty() == false)
    {
        text.append("\n\nSearch Path:");

        for(std::list<std::string>::iterator i = search_path.begin(); i != search_path.end(); i++)
            text.append("\n  ").append((*i).c_str());
    }

    msg.setText(text);
    msg.exec();
}

void MainWindow::GetOracleConnectionString(std::string &conn, bool source)
{
    conn = "oracle,";

    if(source == true)
        conn += ui->oracleUserNameLineEdit->text().toStdString();
    else
        conn += ui->t_oracleUserNameLineEdit->text().toStdString();

    conn += "/";

    if(source == true)
        conn += ui->oraclePasswordLineEdit->text().toStdString();
    else
        conn += ui->t_oraclePasswordLineEdit->text().toStdString();

    conn += "@";

    if(source == true)
        conn += ui->oracleSidComboBox->currentText().toStdString();
    else
        conn += ui->t_oracleSidComboBox->currentText().toStdString();
}

void MainWindow::GetSqlServerConnectionString(std::string &conn, bool source)
{
    conn = "sql,";

    if(source == true)
    {
        if(ui->s_sqlServerWindowsAuthCheckBox->isChecked() == false)
        {
            conn += ui->s_sqlServerUserNameLineEdit->text().toStdString();
            conn += "/";
            conn += ui->s_sqlServerPasswordLineEdit->text().toStdString();
        }
        else
            conn += "trusted";
    }
    else
    {
        if(ui->sqlServerWindowsAuthCheckBox->isChecked() == false)
        {
            conn += ui->sqlServerUserNameLineEdit->text().toStdString();
            conn += "/";
            conn += ui->sqlServerPasswordLineEdit->text().toStdString();
        }
        else
            conn += "trusted";
    }

    conn += "@";

    if(source == true)
        conn += ui->s_sqlServerNameComboBox->currentText().toStdString();
    else
        conn += ui->sqlServerNameComboBox->currentText().toStdString();

    if(source == true)
    {
        if(ui->s_sqlServerDatabaseComboBox->currentText().isEmpty() == false)
        {
            conn += ',';
            conn += ui->s_sqlServerDatabaseComboBox->currentText().toStdString();
        }
    }
    else
    {
        if(ui->sqlServerDatabaseComboBox->currentText().isEmpty() == false)
        {
            conn += ',';
            conn += ui->sqlServerDatabaseComboBox->currentText().toStdString();
        }
    }
}

void MainWindow::GetMysqlConnectionString(std::string &conn, bool source)
{
    conn = "mysql,";
    
    if((source && _source.compare(QString("MariaDB"), Qt::CaseInsensitive) == 0) ||
       (!source && _target.compare(QString("MariaDB"), Qt::CaseInsensitive) == 0))
        conn = "mariadb,";

    QLineEdit *user = source ? ui->s_mysqlUserNameLineEdit : ui->mysqlUserNameLineEdit;
    QLineEdit *pwd = source ? ui->s_mysqlPasswordLineEdit : ui->mysqlPasswordLineEdit;
    QComboBox *srv = source ? ui->s_mysqlServerNameComboBox : ui->mysqlServerNameComboBox;
    QLineEdit *port = source ? ui->s_mysqlPortLineEdit : ui->mysqlPortLineEdit;
    QComboBox *db = source ? ui->s_mysqlDatabaseComboBox : ui->mysqlDatabaseComboBox;

    conn += user->text().toStdString();

    if(pwd->text().isEmpty() == false)
    {
        conn += "/";
        conn += pwd->text().toStdString();
    }

    conn += "@";
    conn += srv->currentText().toStdString();

    if(port->text().isEmpty() == false)
    {
        conn += ':';
        conn += port->text().toStdString();
    }

    if(db->currentText().isEmpty() == false)
    {
        conn += ',';
        conn += db->currentText().toStdString();
    }
}

void MainWindow::GetPgConnectionString(std::string &conn)
{
    conn = "pg,";

    conn += ui->pgUserNameLineEdit->text().toStdString();

    if(ui->pgPasswordLineEdit->text().isEmpty() == false)
    {
        conn += "/";
        conn += ui->pgPasswordLineEdit->text().toStdString();
    }

    conn += "@";
    conn += ui->pgServerNameComboBox->currentText().toStdString();

    if(ui->pgPortLineEdit->text().isEmpty() == false)
    {
        conn += ':';
        conn += ui->pgPortLineEdit->text().toStdString();
    }

    if(ui->pgDatabaseComboBox->currentText().isEmpty() == false)
    {
        conn += ',';
        conn += ui->pgDatabaseComboBox->currentText().toStdString();
    }
}

void MainWindow::GetAseConnectionString(std::string &conn)
{
    conn = "sybase,";

    conn += ui->aseUserNameLineEdit->text().toStdString();

    if(ui->asePasswordLineEdit->text().isEmpty() == false)
    {
        conn += "/";
        conn += ui->asePasswordLineEdit->text().toStdString();
    }

    conn += "@";
    conn += ui->aseServerNameComboBox->currentText().toStdString();

    if(ui->aseDatabaseComboBox->currentText().isEmpty() == false)
    {
        conn += ',';
        conn += ui->aseDatabaseComboBox->currentText().toStdString();
    }
}

void MainWindow::GetAsaConnectionString(std::string &conn)
{
    conn = "asa,";

    conn += ui->asaUserNameLineEdit->text().toStdString();

    if(ui->asaPasswordLineEdit->text().isEmpty() == false)
    {
        conn += "/";
        conn += ui->asaPasswordLineEdit->text().toStdString();
    }

    conn += "@";
    conn += ui->asaDatasourceComboBox->currentText().toStdString();
}

void MainWindow::GetIfmxConnectionString(std::string &conn)
{
    conn = "informix,";

    conn += ui->ifmxUserNameLineEdit->text().toStdString();

    if(ui->ifmxPasswordLineEdit->text().isEmpty() == false)
    {
        conn += "/";
        conn += ui->ifmxPasswordLineEdit->text().toStdString();
    }

    conn += "@";
    conn += ui->ifmxServerNameComboBox->currentText().toStdString();

    if(ui->ifmxDatabaseComboBox->currentText().isEmpty() == false)
    {
        conn += ',';
        conn += ui->ifmxDatabaseComboBox->currentText().toStdString();
    }
}

void MainWindow::GetDb2ConnectionString(std::string &conn, bool source)
{
    conn = "db2,";

    if(source == true)
        conn += ui->db2UserNameLineEdit->text().toStdString();
    else
        conn += ui->t_db2UserNameLineEdit->text().toStdString();


    if(source == true)
    {
        if(ui->db2PasswordLineEdit->text().isEmpty() == false)
        {
            conn += "/";
            conn += ui->db2PasswordLineEdit->text().toStdString();
        }
    }
    else
    {
        if(ui->t_db2PasswordLineEdit->text().isEmpty() == false)
        {
            conn += "/";
            conn += ui->t_db2PasswordLineEdit->text().toStdString();
        }
    }

    if(source == true)
    {
        if(ui->db2DatabaseComboBox->currentText().isEmpty() == false)
        {
            conn += "@";
            conn += ui->db2DatabaseComboBox->currentText().toStdString();
        }
    }
    else
    {
        if(ui->t_db2DatabaseComboBox->currentText().isEmpty() == false)
        {
            conn += "@";
            conn += ui->t_db2DatabaseComboBox->currentText().toStdString();
        }
    }
}

// Thread performing transfer or validation command
int MainWindow::RunCommandThread()
{
    _sqlDataCmd.DefineOptions(_cmdline.c_str());

    // Run the command
    int rc = _sqlDataCmd.Run();

    return rc;
}

unsigned int __stdcall MainWindow::RunCommandThreadS(void *object)
{
    MainWindow *mainWindow = (MainWindow*)object;

    if(mainWindow == NULL)
            return -1;

    return mainWindow->RunCommandThread();
}

void MainWindow::UpdateTransferLog(SqlDataReply &reply)
{
    // Notification about total number of tables for transfer
    if(reply._cmd_subtype == SQLDATA_CMD_NUMBER_OF_TABLES)
    {
        _total_tables = reply._int1;

        ui->totalNumberTablesEdit->setText(QString::number(_total_tables));

        // Tables found
        if(_total_tables > 0)
        {
            ui->transferStatusComboBox->addItem("Transferring tables.");
            ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);
        }
        else
        {
            // Enable transfer and validate commands
            ui->transferButton->setEnabled(true);
            ui->validateButton->setEnabled(true);

            ui->transferStatusComboBox->addItem(QString(reply._s_name).append("No tables found."));
            ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);
        }

        return;
    }
    else
    // All operations are completed
    if(reply._cmd_subtype == SQLDATA_CMD_ALL_COMPLETE)
    {
        char time_fmt[21];
        Str::FormatTime(reply._int1, time_fmt);

        ui->transferTimeEdit->setText(time_fmt);

        int speed_rows = 0;
        double speed_bytes = 0;

        if(reply._int1 != 0)
        {
            speed_rows = ((double)reply._t_int1)/((double)reply._int1)*1000.0;
            speed_bytes = ((double)reply._t_bigint1)/((double)reply._int1)*1000.0;
        }

        char bytes_fmt[21], speed_bytes_fmt[21];

        Str::FormatByteSize((double)reply._t_bigint1, bytes_fmt);
        Str::FormatByteSize(speed_bytes, speed_bytes_fmt);

        ui->volumeTransferredEdit->setText(QString(bytes_fmt));
        ui->transferSpeedBytesEdit->setText(QString(speed_bytes_fmt).append("/sec"));
        ui->transferSpeedRowsEdit->setText(QString::number(speed_rows).append(" rows/sec"));

        // Enable transfer and validate commands
        ui->transferButton->setEnabled(true);
        ui->validateButton->setEnabled(true);

        ui->transferStatusComboBox->addItem(QString(reply._s_name).append("Transfer complete."));
        ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);

        return;
    }

    int idx = GetTableIndex(reply._s_name);

    if(idx == -1)
        idx = AddTableForTransfer(reply._s_name);

    // Set processing status
    QTableWidgetItem *item = ui->transferTableWidget->item(idx, 1);

    if(item == NULL)
        return;

    QString col;

    if(reply._cmd_subtype == SQLDATA_CMD_STARTED)
    {
        item->setText("Started");

        _processing_tables++;
        ui->tablesProcessingEdit->setText(QString::number(_processing_tables));

        ui->transferStatusComboBox->addItem(QString(reply._s_name).append(" started."));
        ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);
    }
    else
    if(reply._cmd_subtype == SQLDATA_CMD_OPEN_CURSOR)
    {
        item->setText("Open cursor");
        ui->transferStatusComboBox->addItem(QString(reply._s_name).append(" cursor open."));
        ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);
    }
    else
    if(reply._cmd_subtype == SQLDATA_CMD_DROP)
    {
        item->setText("Drop target");
        ui->transferStatusComboBox->addItem(QString(reply._s_name).append(" target table dropped."));
        ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);

        _ddl_executed++;
        ui->ddlExecEdit->setText(QString::number(_ddl_executed));
    }
    else
    // Error during DROP or CREATE TABLE etc.
    if(reply._cmd_subtype == SQLDATA_CMD_COMPLETE_WITH_DDL_ERROR)
    {
        item->setText("Failed");

        _failed_tables++;
        ui->tablesFailedEdit->setText(QString::number(_failed_tables));

        ui->transferStatusComboBox->addItem(QString(reply._s_name).append(" failed."));

        if(reply._s_rc == -1)
            ui->transferStatusComboBox->addItem(reply.s_native_error_text);
        else
            ui->transferStatusComboBox->addItem(reply.t_native_error_text);

        _processing_tables--;
        ui->tablesProcessingEdit->setText(QString::number(_processing_tables));

        _ddl_executed++;
        ui->ddlExecEdit->setText(QString::number(_ddl_executed));

        _ddl_failed++;
        ui->ddlFailedEdit->setText(QString::number(_ddl_failed));

        ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);
    }
    else
    if(reply._cmd_subtype == SQLDATA_CMD_TRUNCATE)
    {
        item->setText("Truncate target");
        ui->transferStatusComboBox->addItem(QString(reply._s_name).append(" target table truncated."));
        ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);
    }
    else
    if(reply._cmd_subtype == SQLDATA_CMD_CREATE)
    {
        item->setText("Create target");
        ui->transferStatusComboBox->addItem(QString(reply._s_name).append(" target table created."));
        ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);

        _ddl_executed++;
        ui->ddlExecEdit->setText(QString::number(_ddl_executed));
    }
    else
    if(reply._cmd_subtype == SQLDATA_CMD_IN_PROGRESS)
    {
        item->setText("In Progress");
    }
    else
    // DDL command
    if(reply._cmd_subtype == SQLDATA_CMD_ADD_DEFAULT || reply._cmd_subtype == SQLDATA_CMD_ADD_CHECK_CONSTRAINT ||
       reply._cmd_subtype == SQLDATA_CMD_ADD_PRIMARY_KEY || reply._cmd_subtype == SQLDATA_CMD_ADD_UNIQUE_KEY ||
       reply._cmd_subtype == SQLDATA_CMD_ADD_FOREIGN_KEY || reply._cmd_subtype == SQLDATA_CMD_ADD_COMMENT ||
       reply._cmd_subtype == SQLDATA_CMD_CREATE_INDEX || reply._cmd_subtype == SQLDATA_CMD_CREATE_SEQUENCE ||
       reply._cmd_subtype == SQLDATA_CMD_CREATE_TRIGGER)
    {
        _ddl_executed++;
        ui->ddlExecEdit->setText(QString::number(_ddl_executed));

        if(reply.rc == -1)
        {
            _ddl_failed++;
            ui->ddlFailedEdit->setText(QString::number(_ddl_failed));
        }
    }
    else
    if(reply._cmd_subtype == SQLDATA_CMD_COMPLETE)
    {
        if(reply.rc == 0)
        {
            item->setText("Complete");

            _completed_tables++;
            ui->tablesCompletedEdit->setText(QString::number(_completed_tables));

            _rows_read += reply._s_int1;
            ui->rowsReadEdit->setText(QString::number(_rows_read));

            _rows_written += reply._t_int1;
            ui->rowsWrittenEdit->setText(QString::number(_rows_written));

            _bytes_written += reply._t_bigint1;

            ui->transferStatusComboBox->addItem(QString(reply._s_name).append(" data transfer complete."));
        }
        else
        {
            item->setText("Failed");

            _failed_tables++;
            ui->tablesFailedEdit->setText(QString::number(_failed_tables));

            ui->transferStatusComboBox->addItem(QString(reply._s_name).append(" failed."));

            if(reply._s_rc == -1)
                ui->transferStatusComboBox->addItem(reply.s_native_error_text);
            else
                ui->transferStatusComboBox->addItem(reply.t_native_error_text);
        }

        _processing_tables--;
        ui->tablesProcessingEdit->setText(QString::number(_processing_tables));

        ui->transferStatusComboBox->setCurrentIndex(ui->transferStatusComboBox->count()-1);
    }

    // Rows read
    item = ui->transferTableWidget->item(idx, 2);

    if(item == NULL)
        return;

    if(reply._cmd_subtype == SQLDATA_CMD_IN_PROGRESS || reply._cmd_subtype == SQLDATA_CMD_COMPLETE)
    {
        double speed_rows = 0;

        if(reply._s_int1 != 0 && reply._s_int2 != 0)
            speed_rows = ((double)reply._s_int1)/((double)reply._s_int2) * 1000.0;

        col.sprintf("%d (%.0lf rows/sec)", reply._s_int1, speed_rows);
        item->setText(col);
    }

    // Rows written
    item = ui->transferTableWidget->item(idx, 3);

    if(item == NULL)
        return;

    if(reply._cmd_subtype == SQLDATA_CMD_IN_PROGRESS || reply._cmd_subtype == SQLDATA_CMD_COMPLETE)
    {
        double speed_rows = 0;

        if(reply._t_int1 != 0 && reply._t_int2 != 0)
            speed_rows = ((double)reply._t_int1)/((double)reply._t_int2) * 1000.0;

        col.sprintf("%d (%.0lf rows/sec)", reply._t_int1, speed_rows);
        item->setText(col);
    }

    // Amount transferred
    item = ui->transferTableWidget->item(idx, 4);

    if(item == NULL)
        return;

    if(reply._cmd_subtype == SQLDATA_CMD_IN_PROGRESS || reply._cmd_subtype == SQLDATA_CMD_COMPLETE)
    {
        double speed_bytes = 0;
        char bytes_fmt[21], speed_bytes_fmt[21];

        if(reply._t_bigint1 !=0 && reply._t_int2 != 0)
            speed_bytes = ((double)reply._t_bigint1)/((double)reply._t_int2)*1000.0;

        Str::FormatByteSize((double)reply._t_bigint1, bytes_fmt);
        Str::FormatByteSize(speed_bytes, speed_bytes_fmt);

        col.sprintf("%s (%s/sec)", bytes_fmt, speed_bytes_fmt);
        item->setText(col);
    }

    // Transfer time
    item = ui->transferTableWidget->item(idx, 5);

    if(item == NULL)
        return;

    if(reply._cmd_subtype == SQLDATA_CMD_IN_PROGRESS || reply._cmd_subtype == SQLDATA_CMD_COMPLETE)
    {
        char tr_time_fmt[21], r_time_fmt[21], w_time_fmt[21];

        // Format time
        Str::FormatTime(reply._int1, tr_time_fmt);
        Str::FormatTime(reply._s_int2, r_time_fmt);
        Str::FormatTime(reply._t_int2, w_time_fmt);

        col.sprintf("%s (%s read, %s write)", tr_time_fmt, r_time_fmt, w_time_fmt);
        item->setText(col);
    }

    size_t elapsed = Os::GetTickCount() - _command_start;

    // Set intermediate transfer time
    char time_fmt[21];
    Str::FormatTime(elapsed, time_fmt);

    ui->transferTimeEdit->setText(time_fmt);

    // Set intermediate speed summary
    if(reply._cmd_subtype == SQLDATA_CMD_IN_PROGRESS || reply._cmd_subtype == SQLDATA_CMD_COMPLETE)
    {
        int speed_rows = ((double)_rows_written)/((double)elapsed)*1000.0;
        double speed_bytes = ((double)_bytes_written)/((double)elapsed)*1000.0;

        char bytes_fmt[21], speed_bytes_fmt[21];

        Str::FormatByteSize(_bytes_written, bytes_fmt);
        Str::FormatByteSize(speed_bytes, speed_bytes_fmt);

        ui->volumeTransferredEdit->setText(QString(bytes_fmt));
        ui->transferSpeedBytesEdit->setText(QString(speed_bytes_fmt).append("/sec"));
        ui->transferSpeedRowsEdit->setText(QString::number(speed_rows).append(" rows/sec"));
    }

    if(idx > 13)
       ui->transferTableWidget->setCurrentCell(idx, 0);
}

void MainWindow::UpdateValidationRowCount(SqlDataReply &reply)
{
    // Notification about total number of tables for transfer
    if(reply._cmd_subtype == SQLDATA_CMD_NUMBER_OF_TABLES)
    {
        _total_tables = reply._int1;

        ui->validation_totalNumberTablesEdit->setText(QString::number(_total_tables));

        ui->validationStatusComboBox->addItem("Validating tables.");
        ui->validationStatusComboBox->setCurrentIndex(ui->validationStatusComboBox->count()-1);
    }
    else
    // All operations are completed
    if(reply._cmd_subtype == SQLDATA_CMD_ALL_COMPLETE)
    {
        char time_fmt[21];
        Str::FormatTime(reply._int1, time_fmt);

        ui->rowsInSourceEdit->setText(QString::number(reply._s_int1));
        ui->rowsInTargetEdit->setText(QString::number(reply._t_int1));
        ui->rowsDifferenceEdit->setText(QString::number(abs(reply._s_int1 - reply._t_int1)));

        ui->validationTimeEdit->setText(QString(time_fmt));

        // Enable transfer and validate commands
        ui->transferButton->setEnabled(true);
        ui->validateButton->setEnabled(true);

        ui->validationStatusComboBox->addItem("Validation complete.");
        ui->validationStatusComboBox->setCurrentIndex(ui->validationStatusComboBox->count()-1);
    }
    else
    if(reply._cmd_subtype == SQLDATA_CMD_STARTED)
    {
        int idx = GetTableIndex(reply._s_name);

        if(idx == -1)
            idx = AddTableForValidation(reply._s_name);

        QTableWidgetItem *item = ui->validationTableWidget->item(idx, 1);

        if(item == NULL)
            return;

        item->setText("Started");
    }
    else
    // A single table validated
    if(reply._cmd_subtype == SQLDATA_CMD_COMPLETE)
    {
        int idx = GetTableIndex(reply._s_name);

        if(idx == -1)
            idx = AddTableForValidation(reply._s_name);

        QTableWidgetItem *item = ui->validationTableWidget->item(idx, 1);

        if(item == NULL)
            return;

        // Set validation status
        if(reply._s_rc == -1 || reply._t_rc == -1)
        {
            item->setText("Failed");
            ui->failedRowsCountEdit->setText(QString::number(++_failed_tables));
        }
        else
        // There is a difference in row count
        if(reply._s_int1 != reply._t_int1)
        {
            item->setText("Not Equal");
            ui->notEqualRowsCountEdit->setText(QString::number(++_not_equal_tables));
        }
        else
        {
            item->setText("Ok");
            ui->equalRowsCountEdit->setText(QString::number(++_equal_tables));
        }

        char time_fmt[21], s_time_fmt[21], t_time_fmt[21];

        // Format time
        Str::FormatTime(reply._time_spent, time_fmt);
        Str::FormatTime(reply._s_time_spent, s_time_fmt);
        Str::FormatTime(reply._t_time_spent, t_time_fmt);

        // Rows in source
        item = ui->validationTableWidget->item(idx, 2);

        if(item != NULL && reply._s_rc == 0)
            item->setText(QString::number(reply._s_int1));

        // Rows in target
        item = ui->validationTableWidget->item(idx, 3);

        if(item != NULL && reply._t_rc == 0)
            item->setText(QString::number(reply._t_int1));

        // Validation time
        item = ui->validationTableWidget->item(idx, 4);

        if(item != NULL)
            item->setText(QString(time_fmt) + " (" + s_time_fmt + ", " + t_time_fmt + ")");

        if(idx > 13)
            ui->validationTableWidget->setCurrentCell(idx, 0);

        _source_all_rows += reply._s_int1;
        _target_all_rows += reply._t_int1;

        ui->rowsInSourceEdit->setText(QString::number(_source_all_rows));
        ui->rowsInTargetEdit->setText(QString::number(_target_all_rows));
        ui->rowsDifferenceEdit->setText(QString::number(abs(_source_all_rows - _target_all_rows)));

        char all_time_fmt[21];
        Str::FormatTime(Os::GetTickCount() - _command_start, all_time_fmt);

        ui->validationTimeEdit->setText(QString(all_time_fmt));
    }
}

void MainWindow::UpdateValidationRows(SqlDataReply &reply)
{
    // Notification about total number of tables for transfer
    if(reply._cmd_subtype == SQLDATA_CMD_NUMBER_OF_TABLES)
    {
        _total_tables = reply._int1;

        ui->validation_totalNumberTablesEdit->setText(QString::number(_total_tables));

        ui->validationStatusComboBox->addItem("Validating table data.");
        ui->validationStatusComboBox->setCurrentIndex(ui->validationStatusComboBox->count()-1);
    }
    else
    // All operations are completed
    if(reply._cmd_subtype == SQLDATA_CMD_ALL_COMPLETE)
    {
        char time_fmt[21];
        Str::FormatTime(Os::GetTickCount() - _command_start, time_fmt);

        ui->validationTimeEdit->setText(QString(time_fmt));

        // Enable transfer and validate commands
        ui->transferButton->setEnabled(true);
        ui->validateButton->setEnabled(true);

        ui->validationStatusComboBox->addItem("Data validation complete.");
        ui->validationStatusComboBox->setCurrentIndex(ui->validationStatusComboBox->count()-1);
    }
    else
    if(reply._cmd_subtype == SQLDATA_CMD_STARTED)
    {
        int idx = GetTableIndex(reply._s_name);

        if(idx == -1)
            idx = AddTableForValidation(reply._s_name);

        QTableWidgetItem *item = ui->validationTableWidget->item(idx, 1);

        if(item == NULL)
            return;

        item->setText("Started");
    }
    else
    // A single table validated
    if(reply._cmd_subtype == SQLDATA_CMD_COMPLETE)
    {
        int idx = GetTableIndex(reply._s_name);

        if(idx == -1)
            idx = AddTableForValidation(reply._s_name);

        QTableWidgetItem *item = ui->validationTableWidget->item(idx, 1);

        if(item == NULL)
            return;

        // Set validation status
        if(reply._s_rc == -1 || reply._t_rc == -1)
        {
            item->setText("Failed");
            ui->failedRowsCountEdit->setText(QString::number(++_failed_tables));
        }
        else
        // There is a difference in row count or data
        if(reply._s_int1 != reply._t_int1 || reply._int1 != 0)
        {
            item->setText("Not Equal");
            ui->notEqualRowsCountEdit->setText(QString::number(++_not_equal_tables));
        }
        else
        {
            item->setText("Ok");
            ui->equalRowsCountEdit->setText(QString::number(++_equal_tables));
        }

        char time_fmt[21], s_time_fmt[21], t_time_fmt[21];

        // Format time
        Str::FormatTime(reply._time_spent, time_fmt);
        Str::FormatTime(reply._s_time_spent, s_time_fmt);
        Str::FormatTime(reply._t_time_spent, t_time_fmt);

        // Rows in source
        item = ui->validationTableWidget->item(idx, 2);

        if(item != NULL && reply._s_rc != -1)
            item->setText(QString::number(reply._s_int1));

        // Rows in target
        item = ui->validationTableWidget->item(idx, 3);

        if(item != NULL && reply._t_rc != -1)
            item->setText(QString::number(reply._t_int1));

        // Validation time
        item = ui->validationTableWidget->item(idx, 4);

        if(item != NULL)
            item->setText(QString(time_fmt) + " (" + s_time_fmt + ", " + t_time_fmt + ")");

        if(idx > 13)
            ui->validationTableWidget->setCurrentCell(idx, 0);

        _source_all_rows += reply._s_int1;
        _target_all_rows += reply._t_int1;

        ui->rowsInSourceEdit->setText(QString::number(_source_all_rows));
        ui->rowsInTargetEdit->setText(QString::number(_target_all_rows));
        ui->rowsDifferenceEdit->setText(QString::number(abs(_source_all_rows - _target_all_rows)));

        char all_time_fmt[21];
        Str::FormatTime(Os::GetTickCount() - _command_start, all_time_fmt);

        ui->validationTimeEdit->setText(QString(all_time_fmt));
    }
}

void MainWindow::UpdateLog()
{
    _log_update_mutex.lock();

    // Handle each reply request
    for(std::list<SqlDataReply>::iterator i = _replies.begin(); i != _replies.end(); i++)
    {
        SqlDataReply reply = *i;

        if(reply._cmd == SQLDATA_CMD_TRANSFER)
           UpdateTransferLog(reply);
        else
        if(reply._cmd == SQLDATA_CMD_VALIDATE)
        {
            if(_validation_option == SQLDATA_OPT_ROWCOUNT)
                    UpdateValidationRowCount(reply);
            else
            if(_validation_option == SQLDATA_OPT_ROWS)
                    UpdateValidationRows(reply);
        }
    }

    _replies.clear();

    UpdateRawLog();

    _log_update_mutex.unlock();
}

 void MainWindow::UpdateRawLog()
 {
     ui->rawLogTextEdit->moveCursor(QTextCursor::End);

     // Handle each message
     for(std::list<QString>::iterator i = _raw_log_replies.begin(); i != _raw_log_replies.end(); i++)
         ui->rawLogTextEdit->insertPlainText(*i);

     ui->rawLogTextEdit->moveCursor(QTextCursor::End);

     _raw_log_replies.clear();
 }

// Get index of the table in the list of all tables
int MainWindow::GetTableIndex(const char *name)
{
    if(name == NULL)
        return -1;

    int k = 0;

    for(std::list<std::string>::iterator i = _avail_tables.begin(); i != _avail_tables.end(); i++, k++)
    {
        if(_stricmp((*i).c_str(), name) == 0)
            return k;
    }

    return -1;
}

// Add table to transfer view and list
int MainWindow::AddTableForTransfer(const char *name)
{
    if(name == NULL)
        return -1;

    QFont font;
    font.setPointSize(8);

    // Add to list
    _avail_tables.push_back(name);

    int idx = (int)_avail_tables.size() - 1;

    ui->transferTableWidget->insertRow(idx);
    ui->transferTableWidget->verticalHeader()->resizeSection(idx, 25);

    QTableWidgetItem *item = new QTableWidgetItem(name);
    item->setFont(font);
    ui->transferTableWidget->setItem(idx, 0, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->transferTableWidget->setItem(idx, 1, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->transferTableWidget->setItem(idx, 2, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->transferTableWidget->setItem(idx, 3, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->transferTableWidget->setItem(idx, 4, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->transferTableWidget->setItem(idx, 5, item);

    return idx;
}

// Add table to validation view and list
int MainWindow::AddTableForValidation(const char *name)
{
    if(name == NULL)
        return -1;

    QFont font;
    font.setPointSize(8);

    // Add to list
    _avail_tables.push_back(name);

    int idx = (int)_avail_tables.size() - 1;

    ui->validationTableWidget->insertRow(idx);
    ui->validationTableWidget->verticalHeader()->resizeSection(idx, 25);

    QTableWidgetItem *item = new QTableWidgetItem(name);
    item->setFont(font);
    ui->validationTableWidget->setItem(idx, 0, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->validationTableWidget->setItem(idx, 1, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->validationTableWidget->setItem(idx, 2, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->validationTableWidget->setItem(idx, 3, item);

    item = new QTableWidgetItem("");
    item->setFont(font);
    ui->validationTableWidget->setItem(idx, 4, item);

    return idx;
}

// Callback (already called from a critical section)
void MainWindow::Callback(SqlDataReply *reply)
{
    _log_update_mutex.lock();

    int initial_size = (int)_replies.size();

    _replies.push_back(*reply);

    if(initial_size == 0)
    {
        emit UpdateLogSignal();
    }

    _log_update_mutex.unlock();
}

void MainWindow::CallbackS(void *object, SqlDataReply *reply)
{
    if(object == NULL)
        return;

    MainWindow *mainWindow = (MainWindow*)object;

    mainWindow->Callback(reply);
}

// Console output callbacks
void MainWindow::ConsoleCallback(const char *format, va_list args)
{
    char out[2048];
    QString outs;

    vsprintf(out, format, args);
    outs = out;

    _log_update_mutex.lock();

    _raw_log_replies.push_back(outs);

    _log_update_mutex.unlock();

    //emit UpdateRawLogSignal();
}

void MainWindow::ConsoleCallbackS(void *object, const char *format, va_list args)
{
    if(object == NULL)
        return;

    MainWindow *mainWindow = (MainWindow*)object;

    mainWindow->ConsoleCallback(format, args);
}


void MainWindow::SetTransferOptions()
{
    _cmdline += " -topt=";

    if(ui->createTablesRadioButton->isChecked() == true)
        _cmdline += "create";
    else
    if(ui->recreateTablesRadioButton->isChecked() == true)
        _cmdline += "recreate";
    else
    if(ui->truncateTablesRadioButton->isChecked() == true)
        _cmdline += "truncate";
    else
        _cmdline += "none";
}

// Initialize Tranfer Log view window
void MainWindow::InitTransferLogView()
{
    // Set headers
    QStringList headers;
    headers << "Table Name" << "Status" << "Rows Read" << "Rows Written" << "Transferred" << "Transfer Time";

    ui->transferTableWidget->setColumnCount(6);
    ui->transferTableWidget->setHorizontalHeaderLabels(headers);

    ui->transferTableWidget->horizontalHeader()->resizeSection(0, 150);
    ui->transferTableWidget->horizontalHeader()->resizeSection(1, 100);
    ui->transferTableWidget->horizontalHeader()->resizeSection(2, 160);
    ui->transferTableWidget->horizontalHeader()->resizeSection(3, 160);
    ui->transferTableWidget->horizontalHeader()->resizeSection(4, 160);

    QFont font = ui->transferTableWidget->horizontalHeader()->font();
    font.setPointSize(8);
    ui->transferTableWidget->horizontalHeader()->setFont(font);

    font = ui->transferTableWidget->verticalHeader()->font();
    font.setPointSize(8);
    ui->transferTableWidget->verticalHeader()->setFont(font);

    ui->transferTableWidget->horizontalHeader()->setStretchLastSection(true);
 }

// Initialize Validation Log view window
void MainWindow::InitValidationLogView()
{
    // Set headers
    QStringList headers;
    headers << "Table Name" << "Status" << "Rows in Source" << "Rows in Target" << "Validation Time";

    ui->validationTableWidget->setColumnCount(5);
    ui->validationTableWidget->setHorizontalHeaderLabels(headers);

    ui->validationTableWidget->horizontalHeader()->resizeSection(0, 270);
    ui->validationTableWidget->horizontalHeader()->resizeSection(1, 100);
    ui->validationTableWidget->horizontalHeader()->resizeSection(2, 160);
    ui->validationTableWidget->horizontalHeader()->resizeSection(3, 160);

    QFont font = ui->validationTableWidget->horizontalHeader()->font();
    font.setPointSize(8);
    ui->validationTableWidget->horizontalHeader()->setFont(font);

    font = ui->validationTableWidget->verticalHeader()->font();
    font.setPointSize(8);
    ui->validationTableWidget->verticalHeader()->setFont(font);

    ui->validationTableWidget->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::LoadOptions()
{
    QCoreApplication::setApplicationName("SQLData");
    _settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "SQLines", "SQLData");

    // Read main window position
    QPoint pos = _settings->value(INI_MAINWINDOW_POS, QPoint(0,0)).toPoint();

    // If position is not set, center the window
    if(pos.isNull())
    {
        QRect r = geometry();
        r.moveCenter(QApplication::desktop()->availableGeometry().center());
        setGeometry(r);
    }
    // Otherwise move to the previous position
    else move(pos);

    // Initial active tab
    ui->mainTabWidget->setCurrentIndex(_settings->value(INI_MAINTAB_INDEX, 0).toInt());

    // Source and target database types
    int sourceIdx = ui->sourceComboBox->findText(_settings->value(INI_SOURCE).toString());
    int targetIdx = ui->targetComboBox->findText(_settings->value(INI_TARGET).toString());

    // First set indexes to -1 to force calling signals when first item will be set
    ui->sourceComboBox->setCurrentIndex(-1);
    ui->targetComboBox->setCurrentIndex(-1);

    // Oracle to SQL Server is the default direction
    ui->sourceComboBox->setCurrentIndex(sourceIdx != -1 ? sourceIdx : 1);
    ui->targetComboBox->setCurrentIndex(targetIdx != -1 ? targetIdx : 0);

    // Oracle connection strings
    QStringList oracleList = _settings->value(INI_ORACLE_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(oracleList.isEmpty() == true)
    {
        ui->oracleSidComboBox->insertItem(0, "orcl");
        ui->oracleSidComboBox->insertItem(1, "localhost/orcl");
        ui->oracleSidComboBox->insertItem(2, "localhost:1521/orcl");
        ui->oracleSidComboBox->insertItem(3, "192.168.2.1:1521/orcl");
    }
    else
        ui->oracleSidComboBox->addItems(oracleList);

     ui->oracleSidComboBox->setCurrentIndex(_settings->value(INI_LAST_ORACLE_CONNECTION).toInt());

    // Oracle list (as target)
    oracleList = _settings->value(INI_ORACLE_T_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(oracleList.isEmpty() == true)
    {
        ui->t_oracleSidComboBox->insertItem(0, "orcl");
        ui->t_oracleSidComboBox->insertItem(1, "localhost/orcl");
        ui->t_oracleSidComboBox->insertItem(2, "localhost:1521/orcl");
        ui->t_oracleSidComboBox->insertItem(3, "192.168.2.1:1521/orcl");
    }
    else
        ui->t_oracleSidComboBox->addItems(oracleList);

    // SQL Server connection strings
    QStringList sqlServerList = _settings->value(INI_SQL_SERVER_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(sqlServerList.isEmpty() == true)
    {
        ui->sqlServerNameComboBox->insertItem(0, "localhost");
        ui->sqlServerNameComboBox->insertItem(1, "localhost\\instance1");
    }
    else
        ui->sqlServerNameComboBox->addItems(sqlServerList);

    ui->sqlServerNameComboBox->setCurrentIndex(_settings->value(INI_LAST_SQL_SERVER_CONNECTION).toInt());

    // SQL Server (as source) connection strings
    sqlServerList = _settings->value(INI_SQL_SERVER_S_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(sqlServerList.isEmpty() == true)
    {
        ui->s_sqlServerNameComboBox->insertItem(0, "localhost");
        ui->s_sqlServerNameComboBox->insertItem(1, "localhost\\instance1");
    }
    else
        ui->s_sqlServerNameComboBox->addItems(sqlServerList);

    // MySQL server names
    QStringList mysqlList = _settings->value(INI_MYSQL_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(mysqlList.isEmpty() == true)
    {
        ui->mysqlServerNameComboBox->insertItem(0, "localhost");
    }
    else
        ui->mysqlServerNameComboBox->addItems(mysqlList);

    // MySQL server names (as source)
    mysqlList = _settings->value(INI_MYSQL_S_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(mysqlList.isEmpty() == true)
    {
        ui->s_mysqlServerNameComboBox->insertItem(0, "localhost");
    }
    else
        ui->s_mysqlServerNameComboBox->addItems(mysqlList);

    // PostgreSQL server names
    QStringList pgList = _settings->value(INI_PG_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(pgList.isEmpty() == true)
    {
        ui->pgServerNameComboBox->insertItem(0, "localhost");
    }
    else
        ui->pgServerNameComboBox->addItems(pgList);

    // Sybase ASE server names
    QStringList aseList = _settings->value(INI_ASE_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(aseList.isEmpty() == true)
    {
    }
    else
        ui->aseServerNameComboBox->addItems(aseList);

    // Informix server names
    QStringList ifmxList = _settings->value(INI_IFMX_CONNECTIONS).toStringList();

    // If list is empty, add predefined values
    if(ifmxList.isEmpty() == true)
    {
        ui->ifmxServerNameComboBox->insertItem(0, "ol_informix");
    }
    else
        ui->ifmxServerNameComboBox->addItems(ifmxList);

    // Logins and other connection info
    QString userName;
    ui->oracleUserNameLineEdit->setText(_settings->value(INI_ORACLE_USERNAME).toString());
    ui->t_oracleUserNameLineEdit->setText(_settings->value(INI_ORACLE_T_USERNAME).toString());
    ui->sqlServerUserNameLineEdit->setText(_settings->value(INI_SQL_SERVER_USERNAME).toString());
    ui->s_sqlServerUserNameLineEdit->setText(_settings->value(INI_SQL_SERVER_S_USERNAME).toString());
    ui->mysqlUserNameLineEdit->setText(_settings->value(INI_MYSQL_USERNAME).toString());
    ui->s_mysqlUserNameLineEdit->setText(_settings->value(INI_MYSQL_S_USERNAME).toString());
    ui->pgUserNameLineEdit->setText(_settings->value(INI_PG_USERNAME).toString());
    ui->aseUserNameLineEdit->setText(_settings->value(INI_ASE_USERNAME).toString());
    ui->asaUserNameLineEdit->setText(_settings->value(INI_ASA_USERNAME).toString());

    // Informix user name
    userName = _settings->value(INI_IFMX_USERNAME).toString();

    if(userName.isEmpty() == true)
        userName = "informix";

    ui->ifmxUserNameLineEdit->setText(userName);

    ui->db2UserNameLineEdit->setText(_settings->value(INI_DB2_USERNAME).toString());
    ui->t_db2UserNameLineEdit->setText(_settings->value(INI_DB2_T_USERNAME).toString());

    bool save_ora_password = _settings->value(INI_ORACLE_SAVE_PASSWORD, false).toBool();
    bool save_ora_t_password = _settings->value(INI_ORACLE_T_SAVE_PASSWORD, false).toBool();
    bool save_sql_password = _settings->value(INI_SQL_SERVER_SAVE_PASSWORD, false).toBool();
    bool save_sql_s_password = _settings->value(INI_SQL_SERVER_S_SAVE_PASSWORD, false).toBool();
    bool save_mysql_password = _settings->value(INI_MYSQL_SAVE_PASSWORD, false).toBool();
    bool save_mysql_s_password = _settings->value(INI_MYSQL_S_SAVE_PASSWORD, false).toBool();
    bool save_pg_password = _settings->value(INI_PG_SAVE_PASSWORD, false).toBool();
    bool save_ase_password = _settings->value(INI_ASE_SAVE_PASSWORD, false).toBool();
    bool save_asa_password = _settings->value(INI_ASA_SAVE_PASSWORD, false).toBool();
    bool save_ifmx_password = _settings->value(INI_IFMX_SAVE_PASSWORD, false).toBool();
    bool save_db2_password = _settings->value(INI_DB2_SAVE_PASSWORD, false).toBool();
    bool save_db2_t_password = _settings->value(INI_DB2_T_SAVE_PASSWORD, false).toBool();

    if(save_ora_password == true)
        ui->oraclePasswordLineEdit->setText(DecodePassword(_settings->value(INI_ORACLE_PASSWORD).toString()));

    if(save_ora_t_password == true)
        ui->t_oraclePasswordLineEdit->setText(DecodePassword(_settings->value(INI_ORACLE_T_PASSWORD).toString()));

    if(save_sql_password == true)
        ui->sqlServerPasswordLineEdit->setText(DecodePassword(_settings->value(INI_SQL_SERVER_PASSWORD).toString()));

    if(save_sql_s_password == true)
        ui->s_sqlServerPasswordLineEdit->setText(DecodePassword(_settings->value(INI_SQL_SERVER_S_PASSWORD).toString()));

    if(save_mysql_password == true)
        ui->mysqlPasswordLineEdit->setText(DecodePassword(_settings->value(INI_MYSQL_PASSWORD).toString()));

    if(save_mysql_s_password == true)
        ui->s_mysqlPasswordLineEdit->setText(DecodePassword(_settings->value(INI_MYSQL_S_PASSWORD).toString()));

    if(save_pg_password == true)
        ui->pgPasswordLineEdit->setText(DecodePassword(_settings->value(INI_PG_PASSWORD).toString()));

    if(save_ase_password == true)
        ui->asePasswordLineEdit->setText(DecodePassword(_settings->value(INI_ASE_PASSWORD).toString()));

    if(save_asa_password == true)
        ui->asaPasswordLineEdit->setText(DecodePassword(_settings->value(INI_ASA_PASSWORD).toString()));

    if(save_ifmx_password == true)
        ui->ifmxPasswordLineEdit->setText(DecodePassword(_settings->value(INI_IFMX_PASSWORD).toString()));

    if(save_db2_password == true)
        ui->db2PasswordLineEdit->setText(DecodePassword(_settings->value(INI_DB2_PASSWORD).toString()));

    if(save_db2_t_password == true)
        ui->t_db2PasswordLineEdit->setText(DecodePassword(_settings->value(INI_DB2_T_PASSWORD).toString()));

    ui->oracleSavePasswordCheckBox->setChecked(save_ora_password);
    ui->t_oracleSavePasswordCheckBox->setChecked(save_ora_t_password);
    ui->sqlServerSavePasswordCheckBox->setChecked(save_sql_password);
    ui->s_sqlServerSavePasswordCheckBox->setChecked(save_sql_s_password);
    ui->mysqlSavePasswordCheckBox->setChecked(save_mysql_password);
    ui->s_mysqlSavePasswordCheckBox->setChecked(save_mysql_password);
    ui->pgSavePasswordCheckBox->setChecked(save_pg_password);
    ui->aseSavePasswordCheckBox->setChecked(save_ase_password);
    ui->asaSavePasswordCheckBox->setChecked(save_asa_password);
    ui->ifmxSavePasswordCheckBox->setChecked(save_ifmx_password);
    ui->db2SavePasswordCheckBox->setChecked(save_db2_password);
    ui->t_db2SavePasswordCheckBox->setChecked(save_db2_t_password);

    ui->sqlServerWindowsAuthCheckBox->setChecked(_settings->value(INI_SQL_SERVER_AUTH, false).toBool());
    ui->s_sqlServerWindowsAuthCheckBox->setChecked(_settings->value(INI_SQL_SERVER_S_AUTH, false).toBool());

    // SQL Server database list
    QStringList sqlServerDatabases = _settings->value(INI_SQL_SERVER_DATABASES).toStringList();
    ui->sqlServerDatabaseComboBox->addItems(sqlServerDatabases);
    ui->sqlServerDatabaseComboBox->setCurrentIndex(_settings->value(INI_LAST_SQL_SERVER_DATABASE).toInt());

    // SQL Server (as source) database list
    sqlServerDatabases = _settings->value(INI_SQL_SERVER_S_DATABASES).toStringList();
    ui->s_sqlServerDatabaseComboBox->addItems(sqlServerDatabases);
    ui->s_sqlServerDatabaseComboBox->setCurrentIndex(_settings->value(INI_LAST_SQL_SERVER_S_DATABASE).toInt());

    // MySQL database list
    QStringList mysqlDatabases = _settings->value(INI_MYSQL_DATABASES).toStringList();
    ui->mysqlDatabaseComboBox->addItems(mysqlDatabases);

    // MySQL database list (as source)
    mysqlDatabases = _settings->value(INI_MYSQL_S_DATABASES).toStringList();
    ui->s_mysqlDatabaseComboBox->addItems(mysqlDatabases);

    // PostgreSQL database list
    QStringList pgDatabases = _settings->value(INI_PG_DATABASES).toStringList();
    ui->pgDatabaseComboBox->addItems(pgDatabases);

    // Sybase ASE database list
    QStringList aseDatabases = _settings->value(INI_ASE_DATABASES).toStringList();
    ui->aseDatabaseComboBox->addItems(aseDatabases);

    // Sybase ASA datasources
    QStringList asaDatasources = _settings->value(INI_ASA_DATASOURCES).toStringList();
    ui->asaDatasourceComboBox->addItems(asaDatasources);

    // Informix database list
    QStringList ifmxDatabases = _settings->value(INI_IFMX_DATABASES).toStringList();

    if(ifmxDatabases.isEmpty() == false)
        ui->ifmxDatabaseComboBox->addItems(ifmxDatabases);
    else
        ui->ifmxDatabaseComboBox->addItem("stores_demo");

    // DB2 database list
    QStringList db2Databases = _settings->value(INI_DB2_DATABASES).toStringList();
    ui->db2DatabaseComboBox->addItems(db2Databases);

    db2Databases = _settings->value(INI_DB2_T_DATABASES).toStringList();
    ui->t_db2DatabaseComboBox->addItems(db2Databases);

    // MySQL port
    QString mysqlPort = _settings->value(INI_MYSQL_PORT).toString();
    ui->mysqlPortLineEdit->setText(mysqlPort);

    // MySQL port (as source)
    mysqlPort = _settings->value(INI_MYSQL_S_PORT).toString();
    ui->s_mysqlPortLineEdit->setText(mysqlPort);

    // PostgreSQL port
    QString pgPort = _settings->value(INI_PG_PORT).toString();
    ui->pgPortLineEdit->setText(pgPort);

    // Table list
    QStringList tableList = _settings->value(INI_TABLE_LIST).toStringList();

    // If list is empty, add predefined values
    if(tableList.isEmpty() == true)
        ui->tablesComboBox->insertItem(0, "*.*");
    else
        ui->tablesComboBox->addItems(tableList);

     ui->tablesComboBox->setCurrentIndex(_settings->value(INI_LAST_TABLE_LIST).toInt());

    // Table list files
    tableList = _settings->value(INI_TABLE_FILE).toStringList();

    if(tableList.isEmpty() == false)
        ui->tablesFileComboBox->addItems(tableList);

    // Table list or file
    bool tab_list = _settings->value(INI_TABLE_LIST_RADIO, false).toBool();
    bool tab_file = _settings->value(INI_TABLE_FILE_RADIO, false).toBool();

    if(tab_list || (!tab_list && !tab_file))
        ui->tablesListRadioButton->setChecked(true);
    else
        ui->tablesFileRadioButton->setChecked(true);

    // Exclude table list
    tableList = _settings->value(INI_EXCL_TABLE_LIST).toStringList();

    if(tableList.isEmpty() == false)
        ui->tablesExcludeComboBox->addItems(tableList);

    ui->tablesExcludeComboBox->setCurrentIndex(_settings->value(INI_LAST_EXCL_TABLE_LIST).toInt());

    // Schema name mapping
    QStringList schemaMappingList = _settings->value(INI_SCHEMA_MAPPING_LIST).toStringList();

    if(schemaMappingList.isEmpty() == false)
        ui->schemaMappingComboBox->addItems(schemaMappingList);

    ui->schemaMappingComboBox->setCurrentIndex(_settings->value(INI_LAST_SCHEMA_MAPPING_LIST).toInt());

    // Load to the default schema option
    ui->defaultSchemaCheckBox->setChecked(_settings->value(INI_DEFAULT_SCHEMA, true).toBool());

    // Table creation options
    ui->createTablesRadioButton->setChecked(_settings->value(INI_CREATE_TABLE, false).toBool());
    ui->recreateTablesRadioButton->setChecked(_settings->value(INI_RECREATE_TABLE, true).toBool());
    ui->truncateTablesRadioButton->setChecked(_settings->value(INI_TRUNCATE_TABLE, false).toBool());
    ui->loadExistingRadioButton->setChecked(_settings->value(INI_LOAD_TO_EXISTING, false).toBool());

    // Validation options
    ui->validationOptionComboBox->setCurrentIndex(_settings->value(INI_VALIDATION_OPTION).toInt());

    ui->sessionsSpinBox->setValue(_settings->value(INI_CONCURRENT_SESSIONS, 4).toInt());
}

void MainWindow::SaveOptions()
{
    // Save main window position
    _settings->setValue(INI_MAINWINDOW_POS, pos());

    _settings->setValue(INI_MAINTAB_INDEX, ui->mainTabWidget->currentIndex());

    _settings->setValue(INI_SOURCE, ui->sourceComboBox->currentText());
    _settings->setValue(INI_TARGET, ui->targetComboBox->currentText());

    SaveEditLineToComboBox(ui->oracleSidComboBox);
    SaveEditLineToComboBox(ui->t_oracleSidComboBox);
    SaveEditLineToComboBox(ui->sqlServerNameComboBox);
    SaveEditLineToComboBox(ui->s_sqlServerNameComboBox);
    SaveEditLineToComboBox(ui->mysqlServerNameComboBox);
    SaveEditLineToComboBox(ui->s_mysqlServerNameComboBox);
    SaveEditLineToComboBox(ui->pgServerNameComboBox);
    SaveEditLineToComboBox(ui->aseServerNameComboBox);
    SaveEditLineToComboBox(ui->ifmxServerNameComboBox);

    SaveEditLineToComboBox(ui->sqlServerDatabaseComboBox);
    SaveEditLineToComboBox(ui->s_sqlServerDatabaseComboBox);
    SaveEditLineToComboBox(ui->mysqlDatabaseComboBox);
    SaveEditLineToComboBox(ui->s_mysqlDatabaseComboBox);
    SaveEditLineToComboBox(ui->pgDatabaseComboBox);
    SaveEditLineToComboBox(ui->aseDatabaseComboBox);
    SaveEditLineToComboBox(ui->asaDatasourceComboBox);
    SaveEditLineToComboBox(ui->ifmxDatabaseComboBox);
    SaveEditLineToComboBox(ui->db2DatabaseComboBox);
    SaveEditLineToComboBox(ui->t_db2DatabaseComboBox);

    SaveEditLineToComboBox(ui->tablesComboBox);
    SaveEditLineToComboBox(ui->tablesFileComboBox);
    SaveEditLineToComboBox(ui->tablesExcludeComboBox);
    SaveEditLineToComboBox(ui->schemaMappingComboBox);

    SaveComboBoxToIni(INI_ORACLE_CONNECTIONS, ui->oracleSidComboBox);
    SaveComboBoxToIni(INI_ORACLE_T_CONNECTIONS, ui->t_oracleSidComboBox);
    SaveComboBoxToIni(INI_SQL_SERVER_CONNECTIONS, ui->sqlServerNameComboBox);
    SaveComboBoxToIni(INI_SQL_SERVER_S_CONNECTIONS, ui->s_sqlServerNameComboBox);
    SaveComboBoxToIni(INI_MYSQL_CONNECTIONS, ui->mysqlServerNameComboBox);
    SaveComboBoxToIni(INI_MYSQL_S_CONNECTIONS, ui->s_mysqlServerNameComboBox);
    SaveComboBoxToIni(INI_PG_CONNECTIONS, ui->pgServerNameComboBox);
    SaveComboBoxToIni(INI_ASE_CONNECTIONS, ui->aseServerNameComboBox);
    SaveComboBoxToIni(INI_IFMX_CONNECTIONS, ui->ifmxServerNameComboBox);

    _settings->setValue(INI_LAST_ORACLE_CONNECTION, ui->oracleSidComboBox->currentIndex());
    _settings->setValue(INI_LAST_SQL_SERVER_CONNECTION, ui->sqlServerNameComboBox->currentIndex());

    bool save_ora_password = ui->oracleSavePasswordCheckBox->isChecked();
    bool save_ora_t_password = ui->t_oracleSavePasswordCheckBox->isChecked();
    bool save_sql_password = ui->sqlServerSavePasswordCheckBox->isChecked();
    bool save_sql_s_password = ui->s_sqlServerSavePasswordCheckBox->isChecked();
    bool save_mysql_password = ui->mysqlSavePasswordCheckBox->isChecked();
    bool save_mysql_s_password = ui->s_mysqlSavePasswordCheckBox->isChecked();
    bool save_pg_password = ui->pgSavePasswordCheckBox->isChecked();
    bool save_ase_password = ui->aseSavePasswordCheckBox->isChecked();
    bool save_asa_password = ui->asaSavePasswordCheckBox->isChecked();
    bool save_ifmx_password = ui->ifmxSavePasswordCheckBox->isChecked();
    bool save_db2_password = ui->db2SavePasswordCheckBox->isChecked();
    bool save_db2_t_password = ui->t_db2SavePasswordCheckBox->isChecked();

    // Save logins and other connection info
    _settings->setValue(INI_ORACLE_USERNAME, ui->oracleUserNameLineEdit->text());
    _settings->setValue(INI_ORACLE_T_USERNAME, ui->t_oracleUserNameLineEdit->text());
    _settings->setValue(INI_ORACLE_PASSWORD, (save_ora_password == true) ? EncodePassword(ui->oraclePasswordLineEdit->text()) : "");
    _settings->setValue(INI_ORACLE_T_PASSWORD, (save_ora_t_password == true) ? EncodePassword(ui->t_oraclePasswordLineEdit->text()) : "");
    _settings->setValue(INI_ORACLE_SAVE_PASSWORD, save_ora_password);
    _settings->setValue(INI_ORACLE_T_SAVE_PASSWORD, save_ora_t_password);

    _settings->setValue(INI_SQL_SERVER_USERNAME, ui->sqlServerUserNameLineEdit->text());
    _settings->setValue(INI_SQL_SERVER_S_USERNAME, ui->s_sqlServerUserNameLineEdit->text());
    _settings->setValue(INI_SQL_SERVER_PASSWORD, (save_sql_password == true) ? EncodePassword(ui->sqlServerPasswordLineEdit->text()) : "");
    _settings->setValue(INI_SQL_SERVER_S_PASSWORD, (save_sql_s_password == true) ? EncodePassword(ui->s_sqlServerPasswordLineEdit->text()) : "");
    _settings->setValue(INI_SQL_SERVER_SAVE_PASSWORD, save_sql_password);
    _settings->setValue(INI_SQL_SERVER_S_SAVE_PASSWORD, save_sql_s_password);
    _settings->setValue(INI_SQL_SERVER_AUTH, ui->sqlServerWindowsAuthCheckBox->isChecked());
    _settings->setValue(INI_SQL_SERVER_S_AUTH, ui->s_sqlServerWindowsAuthCheckBox->isChecked());
    SaveComboBoxToIni(INI_SQL_SERVER_DATABASES, ui->sqlServerDatabaseComboBox);
    SaveComboBoxToIni(INI_SQL_SERVER_S_DATABASES, ui->s_sqlServerDatabaseComboBox);
    _settings->setValue(INI_LAST_SQL_SERVER_DATABASE, ui->sqlServerDatabaseComboBox->currentIndex());
    _settings->setValue(INI_LAST_SQL_SERVER_S_DATABASE, ui->s_sqlServerDatabaseComboBox->currentIndex());

    _settings->setValue(INI_MYSQL_USERNAME, ui->mysqlUserNameLineEdit->text());
    _settings->setValue(INI_MYSQL_S_USERNAME, ui->s_mysqlUserNameLineEdit->text());
    _settings->setValue(INI_MYSQL_PASSWORD, (save_mysql_password == true) ? EncodePassword(ui->mysqlPasswordLineEdit->text()) : "");
    _settings->setValue(INI_MYSQL_S_PASSWORD, (save_mysql_s_password == true) ? EncodePassword(ui->s_mysqlPasswordLineEdit->text()) : "");
    _settings->setValue(INI_MYSQL_SAVE_PASSWORD, save_mysql_password);
    _settings->setValue(INI_MYSQL_S_SAVE_PASSWORD, save_mysql_s_password);
    _settings->setValue(INI_MYSQL_PORT, ui->mysqlPortLineEdit->text());
    _settings->setValue(INI_MYSQL_S_PORT, ui->s_mysqlPortLineEdit->text());
    SaveComboBoxToIni(INI_MYSQL_DATABASES, ui->mysqlDatabaseComboBox);
    SaveComboBoxToIni(INI_MYSQL_S_DATABASES, ui->s_mysqlDatabaseComboBox);

    _settings->setValue(INI_PG_USERNAME, ui->pgUserNameLineEdit->text());
    _settings->setValue(INI_PG_PASSWORD, (save_pg_password == true) ? EncodePassword(ui->pgPasswordLineEdit->text()) : "");
    _settings->setValue(INI_PG_SAVE_PASSWORD, save_pg_password);
    _settings->setValue(INI_PG_PORT, ui->pgPortLineEdit->text());
    SaveComboBoxToIni(INI_PG_DATABASES, ui->pgDatabaseComboBox);

    _settings->setValue(INI_ASE_USERNAME, ui->aseUserNameLineEdit->text());
    _settings->setValue(INI_ASE_PASSWORD, (save_ase_password == true) ? EncodePassword(ui->asePasswordLineEdit->text()) : "");
    _settings->setValue(INI_ASE_SAVE_PASSWORD, save_ase_password);
    SaveComboBoxToIni(INI_ASE_DATABASES, ui->aseDatabaseComboBox);

    _settings->setValue(INI_ASA_USERNAME, ui->asaUserNameLineEdit->text());
    _settings->setValue(INI_ASA_PASSWORD, (save_asa_password == true) ? EncodePassword(ui->asaPasswordLineEdit->text()) : "");
    _settings->setValue(INI_ASA_SAVE_PASSWORD, save_asa_password);
    SaveComboBoxToIni(INI_ASA_DATASOURCES, ui->asaDatasourceComboBox);

    _settings->setValue(INI_IFMX_USERNAME, ui->ifmxUserNameLineEdit->text());
    _settings->setValue(INI_IFMX_PASSWORD, (save_ifmx_password == true) ? EncodePassword(ui->ifmxPasswordLineEdit->text()) : "");
    _settings->setValue(INI_IFMX_SAVE_PASSWORD, save_ifmx_password);
    SaveComboBoxToIni(INI_IFMX_DATABASES, ui->ifmxDatabaseComboBox);

    _settings->setValue(INI_DB2_USERNAME, ui->db2UserNameLineEdit->text());
    _settings->setValue(INI_DB2_T_USERNAME, ui->t_db2UserNameLineEdit->text());
    _settings->setValue(INI_DB2_PASSWORD, (save_db2_password == true) ? EncodePassword(ui->db2PasswordLineEdit->text()) : "");
    _settings->setValue(INI_DB2_T_PASSWORD, (save_db2_t_password == true) ? EncodePassword(ui->t_db2PasswordLineEdit->text()) : "");
    _settings->setValue(INI_DB2_SAVE_PASSWORD, save_db2_password);
    _settings->setValue(INI_DB2_T_SAVE_PASSWORD, save_db2_t_password);
    SaveComboBoxToIni(INI_DB2_DATABASES, ui->db2DatabaseComboBox);
    SaveComboBoxToIni(INI_DB2_T_DATABASES, ui->t_db2DatabaseComboBox);

    SaveComboBoxToIni(INI_TABLE_LIST, ui->tablesComboBox);
    SaveComboBoxToIni(INI_TABLE_FILE, ui->tablesFileComboBox);
    SaveComboBoxToIni(INI_EXCL_TABLE_LIST, ui->tablesExcludeComboBox);
    SaveComboBoxToIni(INI_SCHEMA_MAPPING_LIST, ui->schemaMappingComboBox);

    _settings->setValue(INI_LAST_TABLE_LIST, ui->tablesComboBox->currentIndex());

    if(ui->tablesExcludeComboBox->currentText().isEmpty() == false)
        _settings->setValue(INI_LAST_EXCL_TABLE_LIST, ui->tablesExcludeComboBox->currentIndex());
    else
        _settings->setValue(INI_LAST_EXCL_TABLE_LIST, -1);

    if(ui->schemaMappingComboBox->currentText().isEmpty() == false)
        _settings->setValue(INI_LAST_SCHEMA_MAPPING_LIST, ui->schemaMappingComboBox->currentIndex());
    else
        _settings->setValue(INI_LAST_SCHEMA_MAPPING_LIST, -1);

    _settings->setValue(INI_DEFAULT_SCHEMA, ui->defaultSchemaCheckBox->isChecked());

    // Table list or file
    _settings->setValue(INI_TABLE_LIST_RADIO, ui->tablesListRadioButton->isChecked());
    _settings->setValue(INI_TABLE_FILE_RADIO, ui->tablesFileRadioButton->isCheckable());

    // Save table creation options
    _settings->setValue(INI_CREATE_TABLE, ui->createTablesRadioButton->isChecked());
    _settings->setValue(INI_RECREATE_TABLE, ui->recreateTablesRadioButton->isChecked());
    _settings->setValue(INI_TRUNCATE_TABLE, ui->truncateTablesRadioButton->isChecked());
    _settings->setValue(INI_LOAD_TO_EXISTING, ui->loadExistingRadioButton->isChecked());

    _settings->setValue(INI_VALIDATION_OPTION, ui->validationOptionComboBox->currentIndex());

    _settings->setValue(INI_CONCURRENT_SESSIONS, ui->sessionsSpinBox->value());
}

// Select a file with table list
void MainWindow::SelectTablesFile()
{
    QString file = QFileDialog::getOpenFileName(this, "Select a File with List of Tables",
         "");

    if(file.isEmpty() == false)
    {
        ui->tablesFileComboBox->insertItem(0, QDir::toNativeSeparators(file));
        ui->tablesFileComboBox->setCurrentIndex(0);
    }
}

// Save edit line content to combobox list
void MainWindow::SaveEditLineToComboBox(QComboBox *comboBox)
{
    QString line = comboBox->lineEdit()->text();

    // Check that current edit line value is not in combo box
    if(!line.isEmpty() && comboBox->findText(line) == -1)
    {
        comboBox->insertItem(0, line);
        comboBox->setCurrentIndex(0);
    }
}

// Save combobox items to INI file
void MainWindow::SaveComboBoxToIni(char const *optionName, QComboBox *comboBox)
{
    int count = comboBox->count();

    if(count == 0)
      return;

    QStringList list;

    for(int i = 0; i < count; i++)
      list.append(comboBox->itemText(i));

    _settings->setValue(QString(optionName), list);
}

void MainWindow::SqlServerWindowsAuthChecked(int state)
{
    // If checked disable user name and password inputs
    if(state != 0)
    {
        ui->sqlServerUserNameLineEdit->setEnabled(false);
        ui->sqlServerPasswordLineEdit->setEnabled(false);
    }
    else
    {
        ui->sqlServerUserNameLineEdit->setEnabled(true);
        ui->sqlServerPasswordLineEdit->setEnabled(true);
    }
}

void MainWindow::SqlServerSWindowsAuthChecked(int state)
{
    // If checked disable user name and password inputs
    if(state != 0)
    {
        ui->s_sqlServerUserNameLineEdit->setEnabled(false);
        ui->s_sqlServerPasswordLineEdit->setEnabled(false);
    }
    else
    {
        ui->s_sqlServerUserNameLineEdit->setEnabled(true);
        ui->s_sqlServerPasswordLineEdit->setEnabled(true);
    }
}

QString MainWindow::EncodePassword(QString &password)
{
    QString encoded;
    char ch;

    for(int i = 0; i < password.size(); i++)
    {
        ch = password.at(i).toAscii() ^ (i + 1);
        encoded.append(ch);
    }

    return encoded;
}

QString MainWindow::DecodePassword(QString &encoded)
{
    QString password;
    char ch;

    for(int i = 0; i < encoded.size(); i++)
    {
        ch = encoded.at(i).toAscii() ^ (i + 1);
        password.append(ch);
    }

    return password;
}

// Exit from the application
void MainWindow::ExitApp()
{
    SaveOptions();
    QApplication::quit();
}
