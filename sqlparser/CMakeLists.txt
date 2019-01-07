add_library(sqlparser clauses.cpp dllmain.cpp guess.cpp mysql.cpp postgresql.cpp sqlparser.cpp storage.cpp token.cpp cobol.cpp helpers.cpp oracle.cpp procedures.cpp sqlserver.cpp ../sqlcommon/str.cpp datatypes.cpp functions.cpp informix.cpp patterns.cpp report.cpp statements.cpp sybase.cpp db2.cpp greenplum.cpp language.cpp post.cpp select.cpp stats.cpp teradata.cpp)
target_link_libraries(sqlparser ${Glog_LIBRARIES} ${CMAKE_DL_LIBS})

install(DIRECTORY . DESTINATION bin
        FILES_MATCHING PATTERN "sqlines_report.tpl")
