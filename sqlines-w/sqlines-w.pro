#-------------------------------------------------
#
# Project created by QtCreator 2010-11-12T23:32:25
#
#-------------------------------------------------

QT       += core gui
greaterThan(4, QT_MAJOR_VERSION): QT += widgets

TARGET = sqlines-w
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../sqlcommon/str.cpp \
    ../sqlcommon/file.cpp

HEADERS  += mainwindow.h \
    ../sqlcommon/str.h \
    ../sqlcommon/file.h

FORMS    += mainwindow.ui

DEFINES += _CRT_SECURE_NO_WARNINGS

RESOURCES     = sqlines-w.qrc

RC_FILE = sqlines-w.rc






