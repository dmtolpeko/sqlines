#-------------------------------------------------
#
# Project created by QtCreator 2012-09-26T16:42:07
#
#-------------------------------------------------

QT       += core gui
QT       += concurrent

TARGET = sqldata_w
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
		tablewidget.cpp \
    ..\\sqlcommon\\str.cpp \
    ..\\sqldata\\sqlsncapi.cpp \
    ..\\sqldata\\sqlociapi.cpp \
    ..\\sqldata\\sqldb.cpp \
    ..\\sqldata\\sqldata.cpp \
    ..\\sqldata\\sqlapibase.cpp \
    ..\\sqlcommon\\file.cpp \
    ..\\sqldata\\sqlodbcapi.cpp \
    ..\\sqlcommon\\os.cpp \
    ..\\sqldata\\sqlmysqlapi.cpp \
    ..\\sqldata\\sqlpgapi.cpp \
    ..\\sqldata\\sqlctapi.cpp \
    ..\\sqldata\\sqlifmxapi.cpp \
    ..\\sqldata\\sqldb2api.cpp \
    ..\\sqldata\\sqlasaapi.cpp \
    ..\\sqldata\\sqlstdapi.cpp \
    ..\\sqldata\\sqldatacmd.cpp \
    ..\\sqlcommon\\applog.cpp \
    ..\\sqlcommon\\parameters.cpp

HEADERS  += mainwindow.h \
	tablewidget.h \
    ..\\sqlcommon\\str.h \
    ..\\sqldata\\sqlsncapi.h \
    ..\\sqldata\\sqlociapi.h \
    ..\\sqldata\\sqldb.h \
    ..\\sqldata\\sqldata.h \
    ..\\sqldata\\sqlapibase.h \
    ..\\sqlcommon\\file.h \
    ..\\sqldata\\sqlodbcapi.h \
    ..\\sqlcommon\\os.h \
    ..\\sqldata\\sqlmysqlapi.h \
    ..\\sqldata\\sqlpgapi.h \
    ..\\sqldata\\sqlctapi.h \
    ..\\sqldata\\sqlifmxapi.h \
    ..\\sqldata\\sqldb2api.h \
    ..\\sqldata\\sqlasaapi.h \
    ..\\sqldata\\sqlstdapi.h \
    ..\\sqldata\\sqldatacmd.h \
    ..\\sqlcommon\\applog.h \
    ..\\sqlcommon\\parameters.h \
    ..\\sqldata\\sqlparserexp.h

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




















