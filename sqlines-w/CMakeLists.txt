project( sqlines-w )
cmake_minimum_required( VERSION 3.1 )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Core REQUIRED )
find_package ( Qt5Gui REQUIRED )
set ( sqlines-w_HDRS
	../sqlcommon/str.h
	../sqlcommon/file.h
	)

set ( sqlines-w_SRCS
	main.cpp
	mainwindow.cpp
	../sqlcommon/str.cpp
	../sqlcommon/file.cpp
	)

set ( sqlines-w_UIS
	mainwindow.ui
	)
QT5_WRAP_UI(UIS ${sqlines-w_UIS})

set ( sqlines-w_RSCS
	sqlines-w.qrc
	)
QT5_ADD_RESOURCES(RSCS ${sqlines-w_RSCS})

set ( sqlines-w_MOCS
	mainwindow.h
	)
QT5_WRAP_CPP(MOCS ${sqlines-w_MOCS})

add_executable ( sqlines-w ${sqlines-w_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( sqlines-w  Qt5::Core Qt5::Gui Qt5::Widgets )
