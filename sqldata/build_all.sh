g++ -m32 -I. -I./db_api_headers/oraclexe_11_2_0/oci/include -I./db_api_headers/mysql57/include -I./db_api_headers/postgresql90/include -I./db_api_headers/sybase15_0/OCS-15_0/include -I./db_api_headers/win_odbc_compat/Include ../sqlcommon/applog.cpp ../sqlcommon/file.cpp main.cpp ../sqlcommon/os.cpp ../sqlcommon/parameters.cpp sqlapibase.cpp sqlctapi.cpp sqldatacmd.cpp sqldata.cpp sqldb2api.cpp sqldb.cpp sqlifmxapi.cpp sqlmysqlapi.cpp  sqlociapi.cpp sqlpgapi.cpp ../sqlcommon/str.cpp -ldl -lrt ../sqlparser/sqlparser.a -o sqldata
