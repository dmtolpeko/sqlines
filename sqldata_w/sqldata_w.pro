#-------------------------------------------------
#
# Project created by QtCreator 2012-09-26T16:42:07
#
#-------------------------------------------------

QT       += core gui

TARGET = sqldata_w
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
		tablewidget.cpp \
    ..\\sqldata\\str.cpp \
    ..\\sqldata\\sqlsncapi.cpp \
    ..\\sqldata\\sqlociapi.cpp \
    ..\\sqldata\\sqldb.cpp \
    ..\\sqldata\\sqldata.cpp \
    ..\\sqldata\\sqlapibase.cpp \
    ..\\sqlcommon\\file.cpp \
    ..\\sqldata\\sqlodbcapi.cpp \
    ..\\sqldata\\os.cpp \
    ..\\sqldata\\sqlmysqlapi.cpp \
    ..\\sqldata\\sqlpgapi.cpp \
    ..\\sqldata\\sqlctapi.cpp \
    ..\\sqldata\\sqlifmxapi.cpp \
    ..\\sqldata\\sqldb2api.cpp \
    ..\\sqldata\\sqlasaapi.cpp \
    ..\\sqldata\\sqlstdapi.cpp \
    ..\\sqldata\\sqldatacmd.cpp \
    ..\\sqldata\\applog.cpp \
    ..\\sqldata\\parameters.cpp \
    ..\\sqldata\\license.cpp

HEADERS  += mainwindow.h \
	tablewidget.h \
    ..\\sqldata\\str.h \
    ..\\sqldata\\sqlsncapi.h \
    ..\\sqldata\\sqlociapi.h \
    ..\\sqldata\\sqldb.h \
    ..\\sqldata\\sqldata.h \
    ..\\sqldata\\sqlapibase.h \
    ..\\sqlcommon\\file.h \
    ..\\sqldata\\sqlodbcapi.h \
    ..\\sqldata\\os.h \
    ..\\sqldata\\sqlmysqlapi.h \
    ..\\sqldata\\sqlpgapi.h \
    ..\\sqldata\\sqlctapi.h \
    ..\\sqldata\\sqlifmxapi.h \
    ..\\sqldata\\sqldb2api.h \
    ..\\sqldata\\sqlasaapi.h \
    ..\\sqldata\\sqlstdapi.h \
    ..\\sqldata\\sqldatacmd.h \
    ..\\sqldata\\applog.h \
    ..\\sqldata\\parameters.h \
    ..\\sqldata\\sqlparserexp.h \
    ..\\sqldata\\license.h

FORMS    += mainwindow.ui

INCLUDEPATH += ..\\sqldata\\db_api_headers\\oraclexe_11_2_0\\oci\\include \
               ..\\sqldata\\db_api_headers\\sql_server\\100\\SDK\\Include \
               ..\\sqldata\\db_api_headers\\mysql57\\include \
               ..\\sqldata\\db_api_headers\\postgresql90\\include \
               ..\\sqldata\\db_api_headers\\sybase15_0\\OCS-15_0\\include \
               ..\\sqldata \
               ..\\sqlcommon

DEFINES += _CRT_SECURE_NO_WARNINGS \
           _WIN64 \
           WIN64

DEFINES -= UNICODE

LIBS += odbc32.lib \
        advapi32.lib \
        sqlparser\\Release_x64\\sqlparser.lib

# Disable warnings
# C4100: unreferenced formal parameter
# C4189: local variable is initialized but not referenced
QMAKE_CXXFLAGS_DEBUG += -wd4100 -wd4189
QMAKE_CXXFLAGS_RELEASE += -wd4100 -wd4189




















