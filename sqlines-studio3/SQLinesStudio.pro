QT += core \
      gui \
      widgets

CONFIG += qt \
          c++11 \
	    release \
          optimize_full \
	    warn_on

TARGET = SQLines" "Studio
TEMPLATE = app

SOURCES += src/aboutwidget.cpp \
           src/application.cpp \
           src/centralwidget.cpp \
           src/converter.cpp \
           src/coreprocess.cpp \
           src/highlighter.cpp \
           src/inputfield.cpp \
           src/license.cpp \
           src/main.cpp \
           src/mainwindow.cpp \
           src/mainwindowpresenter.cpp \
           src/settings.cpp \
           src/settingsloader.cpp \
           src/settingspresenter.cpp \
           src/settingswidget.cpp \
           src/snapshot.cpp \
           src/tabwidget.cpp \
           src/convertutilitywidget.cpp \
           src/convertutilitypresenter.cpp

HEADERS += src/aboutwidget.hpp \
           src/centralwidget.hpp \
           src/converter.hpp \
           src/convertutilitypresenter.hpp \
           src/convertutilitywidget.hpp \
           src/coreprocess.hpp \
           src/highlighter.hpp \
           src/iconverterobserver.hpp \
           src/iconvertutilitywidget.hpp \
           src/ilicenseobserver.hpp \
           src/imainwindow.hpp \
           src/imainwindowsettings.hpp \
           src/inputfield.hpp \
           src/isettingswidget.hpp \
           src/license.hpp \
           src/mainwindow.hpp \
           src/mainwindowpresenter.hpp \
           src/settings.hpp \
           src/settingsloader.hpp \
           src/settingspresenter.hpp \
           src/settingswidget.hpp \
           src/snapshot.hpp \
           src/tabwidget.hpp

RESOURCES = resources/sqlines.qrc

mac: ICON = resources/logo.ico
win32: RC_ICONS = resources/logo.ico
