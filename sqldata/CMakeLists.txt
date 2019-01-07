cmake_minimum_required(VERSION 3.7)
set(RUNTIME_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

## Boost
#find_package(Boost COMPONENTS system filesystem program_options regex REQUIRED QUIET)

if(POLICY CMP0066)
  cmake_policy(SET CMP0066 NEW)
endif()

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

add_executable(sqldata ../sqlcommon/applog.cpp ../sqlcommon/file.cpp main.cpp ../sqlcommon/os.cpp ../sqlcommon/parameters.cpp sqlapibase.cpp sqlctapi.cpp sqldatacmd.cpp sqldata.cpp sqldb2api.cpp sqldb.cpp sqlifmxapi.cpp sqlmysqlapi.cpp sqlsncapi.cpp sqlstdapi.cpp sqlociapi.cpp sqlpgapi.cpp ../sqlcommon/str.cpp)

target_link_libraries(sqldata sqlparser ${ORACLE_LIBRARIES} ${MYSQL_LIBRARIES} ${PostgreSQL_LIBRARIES} ${FreeTDS_LIBRARIES} ${UnixODBC_LIBRARIES} ${Glog_LIBRARIES} ${CMAKE_DL_LIBS})

if (UNIX AND NOT APPLE)
  target_link_libraries(sqldata rt)
endif()

install(TARGETS sqldata DESTINATION bin)
