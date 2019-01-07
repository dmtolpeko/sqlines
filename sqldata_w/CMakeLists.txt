project( sqldata_w )
cmake_minimum_required( VERSION 3.1 )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )

# Oracle
FIND_PACKAGE(Oracle COMPONENTS OCCI)
IF(ORACLE_FOUND)
  MESSAGE("Oracle OCI Library Found OK")
ELSE(ORACLE_FOUND)
  MESSAGE(STATUS "No Oracle OCI found.")
ENDIF(ORACLE_FOUND)
include_directories(${ORACLE_INCLUDES})

# MySQL
find_package(MySQL REQUIRED)
include_directories(${MYSQL_INCLUDE_DIR})

# PostgreSQL
find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})

# FreeTDS
find_package(FreeTDS REQUIRED)
include_directories(${FreeTDS_INCLUDE_DIR})

# UnixODBC
find_package(UnixODBC REQUIRED)
include_directories(${UnixODBC_INCLUDE_DIRS})

find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Core REQUIRED )
find_package ( Qt5Gui REQUIRED )
find_package ( Qt5Concurrent REQUIRED )
set ( sqldata_w_HDRS
	../sqlcommon/str.h
	../sqldata/sqlsncapi.h
	../sqldata/sqlociapi.h
	../sqldata/sqldb.h
	../sqldata/sqldata.h
	../sqldata/sqlapibase.h
	../sqlcommon/file.h
	../sqldata/sqlodbcapi.h
	../sqlcommon/os.h
	../sqldata/sqlmysqlapi.h
	../sqldata/sqlpgapi.h
	../sqldata/sqlctapi.h
	../sqldata/sqlifmxapi.h
	../sqldata/sqldb2api.h
	../sqldata/sqlasaapi.h
	../sqldata/sqlstdapi.h
	../sqldata/sqldatacmd.h
	../sqlcommon/applog.h
	../sqlcommon/parameters.h
	../sqldata/sqlparserexp.h
	)

set ( sqldata_w_SRCS
	main.cpp
	mainwindow.cpp
	tablewidget.cpp
	../sqlcommon/str.cpp
	../sqldata/sqlsncapi.cpp
	../sqldata/sqlociapi.cpp
	../sqldata/sqldb.cpp
	../sqldata/sqldata.cpp
	../sqldata/sqlapibase.cpp
	../sqlcommon/file.cpp
	../sqldata/sqlodbcapi.cpp
	../sqlcommon/os.cpp
	../sqldata/sqlmysqlapi.cpp
	../sqldata/sqlpgapi.cpp
	../sqldata/sqlctapi.cpp
	../sqldata/sqlifmxapi.cpp
	../sqldata/sqldb2api.cpp
	../sqldata/sqlasaapi.cpp
	../sqldata/sqlstdapi.cpp
	../sqldata/sqldatacmd.cpp
	../sqlcommon/applog.cpp
	../sqlcommon/parameters.cpp
	)

set ( sqldata_w_UIS
	mainwindow.ui
	)
QT5_WRAP_UI(UIS ${sqldata_w_UIS})

set ( sqldata_w_MOCS
	mainwindow.h
	tablewidget.h
	)
QT5_WRAP_CPP(MOCS ${sqldata_w_MOCS})

add_executable ( sqldata_w ${sqldata_w_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( sqldata_w  sqlparser Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent ${ORACLE_LIBRARIES} ${MYSQL_LIBRARIES} ${PostgreSQL_LIBRARIES} ${FreeTDS_LIBRARIES} ${UnixODBC_LIBRARIES} ${Glog_LIBRARIES} ${CMAKE_DL_LIBS})
