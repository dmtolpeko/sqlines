#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSettings>
#include <QComboBox>
#include <QMutex>
#include <stdarg.h>
#include "../sqldata/sqldata.h"
#include "../sqldata/sqldatacmd.h"

#define INI_MAINWINDOW_POS              "mainwindow_position"
#define INI_MAINTAB_INDEX               "maintab_index"
#define INI_SOURCE                      "source"
#define INI_TARGET                      "target"
#define INI_ORACLE_CONNECTIONS          "oracle_connections"
#define INI_ORACLE_T_CONNECTIONS        "t_oracle_connections"
#define INI_SQL_SERVER_CONNECTIONS      "sql_server_connections"
#define INI_SQL_SERVER_S_CONNECTIONS    "s_sql_server_connections"
#define INI_MYSQL_CONNECTIONS           "mysql_connections"
#define INI_MYSQL_S_CONNECTIONS         "s_mysql_connections"
#define INI_PG_CONNECTIONS              "pg_connections"
#define INI_ASE_CONNECTIONS             "ase_connections"
#define INI_ASA_CONNECTIONS             "asa_connections"
#define INI_IFMX_CONNECTIONS            "ifmx_connections"
#define INI_DB2_CONNECTIONS             "db2_connections"
#define INI_DB2_T_CONNECTIONS           "t_db2_connections"

#define INI_LAST_ORACLE_CONNECTION      "last_oracle_connection"
#define INI_LAST_SQL_SERVER_CONNECTION  "last_sql_server_connection"

#define INI_LAST_SQL_SERVER_S_DATABASE  "last_sql_server_s_database"
#define INI_LAST_SQL_SERVER_DATABASE    "last_sql_server_database"

#define INI_ORACLE_USERNAME             "ou"
#define INI_ORACLE_T_USERNAME           "t_ou"
#define INI_ORACLE_PASSWORD             "op"
#define INI_ORACLE_T_PASSWORD           "t_op"
#define INI_ORACLE_SAVE_PASSWORD        "sop"
#define INI_ORACLE_T_SAVE_PASSWORD      "t_sop"

#define INI_SQL_SERVER_USERNAME         "su"
#define INI_SQL_SERVER_S_USERNAME       "s_su"
#define INI_SQL_SERVER_PASSWORD         "sp"
#define INI_SQL_SERVER_S_PASSWORD       "s_sp"
#define INI_SQL_SERVER_SAVE_PASSWORD    "ssp"
#define INI_SQL_SERVER_S_SAVE_PASSWORD  "s_ssp"
#define INI_SQL_SERVER_AUTH             "sql_server_win_auth"
#define INI_SQL_SERVER_S_AUTH           "s_sql_server_win_auth"
#define INI_SQL_SERVER_DATABASES        "sql_server_databases"
#define INI_SQL_SERVER_S_DATABASES      "s_sql_server_databases"

#define INI_MYSQL_USERNAME              "mu"
#define INI_MYSQL_S_USERNAME            "s_mu"
#define INI_MYSQL_PASSWORD              "mp"
#define INI_MYSQL_S_PASSWORD            "s_mp"
#define INI_MYSQL_SAVE_PASSWORD         "ssmp"
#define INI_MYSQL_S_SAVE_PASSWORD       "s_smp"
#define INI_MYSQL_PORT                  "mysql_port"
#define INI_MYSQL_S_PORT                "s_mysql_port"
#define INI_MYSQL_DATABASES             "mysql_databases"
#define INI_MYSQL_S_DATABASES           "s_mysql_databases"

#define INI_PG_USERNAME                 "pu"
#define INI_PG_PASSWORD                 "pp"
#define INI_PG_SAVE_PASSWORD            "spp"
#define INI_PG_PORT                     "pg_port"
#define INI_PG_DATABASES                "pg_databases"
#define INI_ASE_USERNAME                "au"
#define INI_ASE_PASSWORD                "ap"
#define INI_ASE_SAVE_PASSWORD           "sap"
#define INI_ASE_DATABASES               "ase_databases"
#define INI_ASA_USERNAME                "asau"
#define INI_ASA_PASSWORD                "asap"
#define INI_ASA_SAVE_PASSWORD           "sasap"
#define INI_ASA_DATASOURCES             "asa_datasources"
#define INI_IFMX_USERNAME               "iu"
#define INI_IFMX_PASSWORD               "ip"
#define INI_IFMX_SAVE_PASSWORD          "sip"
#define INI_IFMX_DATABASES              "ifmx_databases"

#define INI_DB2_USERNAME                "du"
#define INI_DB2_T_USERNAME              "t_du"
#define INI_DB2_PASSWORD                "dp"
#define INI_DB2_T_PASSWORD              "t_dp"
#define INI_DB2_SAVE_PASSWORD           "sdp"
#define INI_DB2_T_SAVE_PASSWORD         "t_sdp"
#define INI_DB2_DATABASES               "db2_databases"
#define INI_DB2_T_DATABASES             "t_db2_databases"

#define INI_TABLE_LIST                  "table_list"
#define INI_LAST_TABLE_LIST             "last_table_list"
#define INI_TABLE_LIST_RADIO            "table_list_radio"
#define INI_TABLE_FILE                  "table_file"
#define INI_TABLE_FILE_RADIO            "table_file_radio"
#define INI_EXCL_TABLE_LIST             "excl_table_list"
#define INI_LAST_EXCL_TABLE_LIST        "last_excl_table_list"
#define INI_SCHEMA_MAPPING_LIST         "schema_mapping"
#define INI_LAST_SCHEMA_MAPPING_LIST    "last_schema_mapping"
#define INI_DEFAULT_SCHEMA              "default_schema"
#define INI_CREATE_TABLE                "create_table"
#define INI_RECREATE_TABLE              "recreate_table"
#define INI_TRUNCATE_TABLE              "truncate_table"
#define INI_LOAD_TO_EXISTING            "load_to_existing"
#define INI_VALIDATION_OPTION           "validation_option"
#define INI_CONCURRENT_SESSIONS         "concurrent_sessions"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QSettings *_settings;
    SqlData _sqlData;
    SqlDataCmd _sqlDataCmd;

    // Source and target databases
    int _source_type;
    int _target_type;
    QString _source;
    QString _target;

    // Current command line
    std::string _cmdline;

    // Current list of processing tables;
    std::list<std::string> _avail_tables;

    QStandardItemModel _transferLogModel;
    QStandardItemModel _validationLogModel;

    std::list<SqlDataReply> _replies;
    std::list<QString> _raw_log_replies;

    int64_t _rows_read;
    int64_t _rows_written;
    int64_t _bytes_written;

    int64_t _total_tables;
    int64_t _completed_tables;
    int64_t _processing_tables;

    int64_t _equal_tables;
    int64_t _not_equal_tables;
    int64_t _failed_tables;

    int64_t _ddl_executed;
    int64_t _ddl_failed;

    // Current command (Transfer, Validate) is being executed
    int _current_command;

    int _validation_option;

    QMutex _log_update_mutex;

    // Intermediate transfer and validation stats
    int _command_start;
    int _source_all_rows;
    int _target_all_rows;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void SetLicense(bool eval, const char *version, const char *name);

public slots:
    void SourceChanged(QString source);
    void TargetChanged(QString target);
    void RunTransfer();
    void RunValidation();
    void ExitApp();
    void UpdateLog();
    void UpdateRawLog();
    void SqlServerWindowsAuthChecked(int state);
    void SqlServerSWindowsAuthChecked(int state);
    void TestSourceConnection();
    void TestTargetConnection();
    void SelectTablesFile();

Q_SIGNALS:
    void UpdateLogSignal();
    void UpdateRawLogSignal();

private:
    Ui::MainWindow *ui;

    int PrepareCommand();
    void TestConnection(std::string &conn);

    void GetSourceConnectionString(std::string &source);
    void GetTargetConnectionString(std::string &target);
    void GetOracleConnectionString(std::string &conn, bool source);
    void GetSqlServerConnectionString(std::string &conn, bool source);
    void GetMysqlConnectionString(std::string &conn, bool source);
    void GetPgConnectionString(std::string &conn);
    void GetAseConnectionString(std::string &conn);
    void GetAsaConnectionString(std::string &conn);
    void GetIfmxConnectionString(std::string &conn);
    void GetDb2ConnectionString(std::string &conn, bool source);

    void UpdateTransferLog(SqlDataReply &reply);
    void UpdateValidationRowCount(SqlDataReply &reply);
    void UpdateValidationRows(SqlDataReply &reply);

    void LoadOptions();
    void SaveOptions();

    void InitTransferLogView();
    void InitValidationLogView();

    void SetTransferOptions();

    // Get index of the table in the list of all tables
    int GetTableIndex(const char *name);
    int AddTableForTransfer(const char *name);
    int AddTableForValidation(const char *name);

    int GetDbType(QString name);

    void SaveEditLineToComboBox(QComboBox *comboBox);
    void SaveComboBoxToIni(char const *optionName, QComboBox *comboBox);

    int RunCommandThread();
    #if defined(WIN32) || defined(WIN64)
    static unsigned int __stdcall RunCommandThreadS(void *object);
    #else
    static unsigned int RunCommandThreadS(void *object);
    #endif
    void UpdateTitle();

    // Callbacks (Already called from a critical section)
    void Callback(SqlDataReply *reply);
    static void CallbackS(void *object, SqlDataReply *reply);

    // Console output callback
    void ConsoleCallback(const char *format, va_list args);
    static void ConsoleCallbackS(void *object, const char *format, va_list args);

    QString EncodePassword(const QString &password);
    QString DecodePassword(const QString &encoded);
};

#pragma clang diagnostic ignored "-Wredundant-parens"

#endif // MAINWINDOW_H
