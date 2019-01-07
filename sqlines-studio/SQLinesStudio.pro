#-------------------------------------------------
#
# Project created by QtCreator 2010-10-12T23:53:33
#
#-------------------------------------------------

QT       += core gui
QT       += widgets 

TARGET = sqlines-studio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    options.cpp \
    scripttab.cpp \
    ../sqlcommon/str.cpp \
    ../sqlcommon/file.cpp \
    highlighter.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    options.h \
    scripttab.h \
    ../sqlcommon/str.h \
    ../sqlcommon/file.h \
    highlighter.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    options.ui \
    scripttab.ui

RESOURCES     = sqlines.qrc

DEFINES += _CRT_SECURE_NO_WARNINGS

RC_FILE = sqlines.rc




