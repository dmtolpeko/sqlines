project( sqlines-studio )
cmake_minimum_required( VERSION 3.1 )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Core REQUIRED )
find_package ( Qt5Gui REQUIRED )
set ( sqlines-studio_HDRS
	../sqlcommon/str.h
	../sqlcommon/file.h
	)

set ( sqlines-studio_SRCS
	main.cpp
	mainwindow.cpp
	aboutdialog.cpp
	options.cpp
	scripttab.cpp
	../sqlcommon/str.cpp
	../sqlcommon/file.cpp
	highlighter.cpp
	)

set ( sqlines-studio_UIS
	mainwindow.ui
	aboutdialog.ui
	options.ui
	scripttab.ui
	)
QT5_WRAP_UI(UIS ${sqlines-studio_UIS})

set ( sqlines-studio_RSCS
	sqlines.qrc
	)
QT5_ADD_RESOURCES(RSCS ${sqlines-studio_RSCS})

set ( sqlines-studio_MOCS
	mainwindow.h
	aboutdialog.h
	options.h
	scripttab.h
	highlighter.h
	)
QT5_WRAP_CPP(MOCS ${sqlines-studio_MOCS})

add_executable ( sqlines-studio ${sqlines-studio_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( sqlines-studio  Qt5::Core Qt5::Gui Qt5::Widgets )
