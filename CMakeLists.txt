# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

cmake_minimum_required(VERSION 3.2)
project(sqlines)

set(SQLINES_VERSION_MAJOR "3")
set(SQLINES_VERSION_MINOR "1")
set(SQLINES_VERSION_PATCH "107")
set(SQLINES_VERSION_EXTRA "dev")
set(SQLINES_VERSION_RAW "${SQLINES_VERSION_MAJOR}.${SQLINES_VERSION_MINOR}.${SQLINES_VERSION_PATCH}${SQLINES_VERSION_EXTRA}")
string(TIMESTAMP SQLINES_BUILD_DATE "%Y%m%d")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")

include(CMakeParseArguments)
include(ExternalProject)

# This ensures that things like gnu++11 get passed correctly
set(CMAKE_CXX_STANDARD 11)

# We require a C++11 compliant compiler
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# Compatibility with CMake 3.1
if(POLICY CMP0054)
  # http://www.cmake.org/cmake/help/v3.1/policy/CMP0054.html
  cmake_policy(SET CMP0054 NEW)
endif()

set(SQLINES_SO_VERSION "0")
set(SQLINES_ABI_VERSION "${SQLINES_SO_VERSION}.0.0")

set(BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build-support")

find_package(Git)

set(CLANG_FORMAT_VERSION "5.0")
find_package(ClangTools)
if ("$ENV{CMAKE_EXPORT_COMPILE_COMMANDS}" STREQUAL "1" OR CLANG_TIDY_FOUND)
  # Generate a Clang compile_commands.json "compilation database" file for use
  # with various development tools, such as Vim's YouCompleteMe plugin.
  # See http://clang.llvm.org/docs/JSONCompilationDatabase.html
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()

find_package(InferTools)
if ("$ENV{CMAKE_EXPORT_COMPILE_COMMANDS}" STREQUAL "1" OR INFER_FOUND)
  # Generate a Clang compile_commands.json "compilation database" file for use
  # with various development tools, such as Vim's YouCompleteMe plugin.
  # See http://clang.llvm.org/docs/JSONCompilationDatabase.html
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_FOUND})
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_FOUND})
endif(CCACHE_FOUND)

# ----------------------------------------------------------------------
# cmake options

# Top level cmake dir
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  set(SQLINES_CXXFLAGS "-fcomment-block-commands=file" CACHE STRING
    "Compiler flags to append when compiling SQLINES")

  option(SQLINES_BUILD_STATIC
    "Build the libSQLINES static libraries"
    ON)

  option(SQLINES_BUILD_SHARED
    "Build the libSQLINES shared libraries"
    ON)

  option(SQLINES_TEST_MEMCHECK
    "Run the test suite using valgrind --tool=memcheck"
    OFF)

  option(SQLINES_BUILD_TESTS
    "Build the SQLINES googletest unit tests"
    ON)

  option(SQLINES_BUILD_BENCHMARKS
    "Build the SQLINES micro benchmarks"
    OFF)

  option(SQLINES_NO_DEPRECATED_API
    "Exclude deprecated APIs from build"
    OFF)

  option(SQLINES_COMPUTE
    "Build the SQLINES Compute Modules"
    ON)

  option(SQLINES_EXTRA_ERROR_CONTEXT
    "Compile with extra error context (line numbers, code)"
    OFF)

  option(SQLINES_IPC
    "Build the SQLINES IPC extensions"
    ON)

  option(SQLINES_GPU
    "Build the SQLINES GPU extensions (requires CUDA installation)"
    OFF)

  option(SQLINES_ORC
    "Build the SQLINES ORC adapter"
    OFF)

  option(SQLINES_JEMALLOC
    "Build the SQLINES jemalloc-based allocator"
    OFF)

  option(SQLINES_HDFS
    "Build the SQLINES HDFS bridge"
    ON)

  option(SQLINES_BOOST_USE_SHARED
    "Rely on boost shared libraries where relevant"
    ON)

  option(SQLINES_BOOST_VENDORED
    "Use vendored Boost instead of existing Boost"
    OFF)

  option(SQLINES_PYTHON
    "Build the SQLINES CPython extensions"
    OFF)

  option(SQLINES_FUZZING
    "Build SQLINES Fuzzing executables"
    OFF)

  option(SQLINES_SSE3
    "Build SQLINES with SSE3"
    ON)

  option(SQLINES_ALTIVEC
    "Build SQLINES with Altivec"
    ON)

  option(SQLINES_BUILD_UTILITIES
    "Build SQLINES commandline utilities"
    ON)

  option(SQLINES_RPATH_ORIGIN
    "Build SQLINES libraries with RATH set to \$ORIGIN"
    OFF)

  option(SQLINES_INSTALL_NAME_RPATH
    "Build SQLINES libraries with install_name set to @rpath"
    ON)

  option(SQLINES_PLASMA
    "Build the plasma object store along with SQLINES"
    OFF)

  option(SQLINES_USE_SSE
    "Build with SSE4 optimizations"
    OFF)

  option(SQLINES_WITH_BROTLI
    "Build with Brotli compression"
    ON)

  option(SQLINES_WITH_LZ4
    "Build with lz4 compression"
    ON)

  option(SQLINES_WITH_SNAPPY
    "Build with Snappy compression"
    ON)

  option(SQLINES_WITH_ZLIB
    "Build with zlib compression"
    ON)

  option(SQLINES_WITH_ZSTD
    "Build with zstd compression"
    ON)

  option(SQLINES_WITH_GRPC
    "Build with GRPC"
    OFF)

  option(SQLINES_VERBOSE_THIRDPARTY_BUILD
    "If off, output from ExternalProjects will be logged to files rather than shown"
    OFF)

  if (MSVC)
    option(SQLINES_USE_CLCACHE
      "Use clcache if available"
      ON)

    set(BROTLI_MSVC_STATIC_LIB_SUFFIX "-static" CACHE STRING
      "Brotli static lib suffix used on Windows with MSVC (default -static)")
    set(SNAPPY_MSVC_STATIC_LIB_SUFFIX "_static" CACHE STRING
      "Snappy static lib suffix used on Windows with MSVC (default is empty string)")
    set(ZLIB_MSVC_STATIC_LIB_SUFFIX "libstatic" CACHE STRING
      "Zlib static lib suffix used on Windows with MSVC (default libstatic)")
    set(LZ4_MSVC_STATIC_LIB_SUFFIX "_static" CACHE STRING
      "Lz4 static lib suffix used on Windows with MSVC (default _static)")
    set(ZSTD_MSVC_STATIC_LIB_SUFFIX "_static" CACHE STRING
      "ZStd static lib suffix used on Windows with MSVC (default _static)")

    option(SQLINES_USE_STATIC_CRT
      "Build SQLINES with statically linked CRT"
      OFF)
  endif()
endif()

if(SQLINES_BUILD_TESTS OR SQLINES_BUILD_BENCHMARKS)
  set(SQLINES_BUILD_STATIC ON)
  set(SQLINES_WITH_BROTLI ON)
  set(SQLINES_WITH_LZ4 ON)
  set(SQLINES_WITH_SNAPPY ON)
  set(SQLINES_WITH_ZLIB ON)
  set(SQLINES_WITH_ZSTD ON)
endif()

if (MSVC)
  # ORC doesn't build on windows
  set(SQLINES_ORC OFF)
endif()

if(SQLINES_ORC)
  set(SQLINES_WITH_LZ4 ON)
  set(SQLINES_WITH_SNAPPY ON)
  set(SQLINES_WITH_ZLIB ON)
endif()

if(NOT SQLINES_BUILD_TESTS)
  set(NO_TESTS 1)
endif()

if(NOT SQLINES_BUILD_BENCHMARKS)
  set(NO_BENCHMARKS 1)
endif()

if (NOT SQLINES_FUZZING)
  set(NO_FUZZING 1)
endif()

if(SQLINES_HDFS)
  set(SQLINES_BOOST_HEADER_ONLY 0)
else()
  set(SQLINES_BOOST_HEADER_ONLY 1)
endif()

if (MSVC AND SQLINES_USE_CLCACHE AND
     (("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles") OR
      ("${CMAKE_GENERATOR}" STREQUAL "Ninja")))
  find_program(CLCACHE_FOUND clcache)
  if(CLCACHE_FOUND)
    set(CMAKE_CXX_COMPILER ${CLCACHE_FOUND})
  endif(CLCACHE_FOUND)
endif()

############################################################
# Compiler flags
############################################################

# Determine compiler version
include(CompilerInfo)

if (SQLINES_NO_DEPRECATED_API)
  add_definitions(-DSQLINES_NO_DEPRECATED_API)
endif()

if (SQLINES_EXTRA_ERROR_CONTEXT)
  add_definitions(-DSQLINES_EXTRA_ERROR_CONTEXT)
endif()

include(SetupCxxFlags)

############################################################
# Dependencies
############################################################

add_custom_target(sqlines_dependencies)

include(BuildUtils)
enable_testing()

include(ThirdpartyToolchain)

# Add common flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SQLINES_CXXFLAGS}")

if ("${COMPILER_FAMILY}" STREQUAL "clang")
  # Using Clang with ccache causes a bunch of spurious warnings that are
  # purportedly fixed in the next version of ccache. See the following for details:
  #
  #   http://petereisentraut.blogspot.com/2011/05/ccache-and-clang.html
  #   http://petereisentraut.blogspot.com/2011/09/ccache-and-clang-part-2.html
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CLANG_OPTIONS}")
endif()

# ASAN / TSAN / UBSAN
if(SQLINES_FUZZING)
  set(SQLINES_USE_COVERAGE ON)
endif()
include(san-config)

# For any C code, use the same flags.
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")

# Remove --std=c++11 to avoid errors from C compilers
string(REPLACE "-std=c++11" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

# Code coverage
if ("${SQLINES_GENERATE_COVERAGE}")
  if("${CMAKE_CXX_COMPILER}" MATCHES ".*clang.*")
    # There appears to be some bugs in clang 3.3 which cause code coverage
    # to have link errors, not locating the llvm_gcda_* symbols.
    # This should be fixed in llvm 3.4 with http://llvm.org/viewvc/llvm-project?view=revision&revision=184666
    message(SEND_ERROR "Cannot currently generate coverage with clang")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -DCOVERAGE_BUILD")

  # For coverage to work properly, we need to use static linkage. Otherwise,
  # __gcov_flush() doesn't properly flush coverage from every module.
  # See http://stackoverflow.com/questions/28164543/using-gcov-flush-within-a-library-doesnt-force-the-other-modules-to-yield-gc
  if(NOT SQLINES_BUILD_STATIC)
    message(SEND_ERROR "Coverage requires the static lib to be built")
  endif()
endif()

# CMAKE_CXX_FLAGS now fully assembled
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# set compile output directory
string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_SUBDIR_NAME)

# If build in-source, create the latest symlink. If build out-of-source, which is
# preferred, simply output the binaries in the build folder
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
  set(BUILD_OUTPUT_ROOT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build/${BUILD_SUBDIR_NAME}/")
  # Link build/latest to the current build directory, to avoid developers
  # accidentally running the latest debug build when in fact they're building
  # release builds.
  FILE(MAKE_DIRECTORY ${BUILD_OUTPUT_ROOT_DIRECTORY})
  if (NOT APPLE)
    set(MORE_ARGS "-T")
  endif()
EXECUTE_PROCESS(COMMAND ln ${MORE_ARGS} -sf ${BUILD_OUTPUT_ROOT_DIRECTORY}
  ${CMAKE_CURRENT_BINARY_DIR}/build/latest)
else()
  set(BUILD_OUTPUT_ROOT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUBDIR_NAME}/")
endif()

# where to put generated archives (.a files)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_OUTPUT_ROOT_DIRECTORY}")
set(ARCHIVE_OUTPUT_DIRECTORY "${BUILD_OUTPUT_ROOT_DIRECTORY}")

# where to put generated libraries (.so files)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_OUTPUT_ROOT_DIRECTORY}")
set(LIBRARY_OUTPUT_DIRECTORY "${BUILD_OUTPUT_ROOT_DIRECTORY}")

# where to put generated binaries
set(EXECUTABLE_OUTPUT_PATH "${BUILD_OUTPUT_ROOT_DIRECTORY}")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)

############################################################
# Visibility
############################################################
# For generate_export_header() and add_compiler_export_flags().
include(GenerateExportHeader)

# Adapted from Apache Kudu: https://github.com/apache/kudu/commit/bd549e13743a51013585
# Honor visibility properties for all target types. See
# "cmake --help-policy CMP0063" for details.
#
# This policy was only added to cmake in version 3.3, so until the cmake in
# thirdparty is updated, we must check if the policy exists before setting it.
if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

if (PARQUET_BUILD_SHARED)
  if (POLICY CMP0063)
    set_target_properties(sqlines_shared
      PROPERTIES
      C_VISIBILITY_PRESET hidden
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1)
  else()
    # Sets -fvisibility=hidden for gcc
    add_compiler_export_flags()
  endif()
endif()

############################################################
# "make ctags" target
############################################################
if (UNIX)
  add_custom_target(ctags ctags -R --languages=c++,c)
endif (UNIX)

############################################################
# "make etags" target
############################################################
if (UNIX)
  add_custom_target(tags etags --members --declarations
  `find ${CMAKE_CURRENT_SOURCE_DIR}/src
   -name \\*.cc -or -name \\*.hh -or -name \\*.cpp -or -name \\*.h -or -name \\*.c -or
   -name \\*.f`)
  add_custom_target(etags DEPENDS tags)
endif (UNIX)

############################################################
# "make cscope" target
############################################################
if (UNIX)
  add_custom_target(cscope find ${CMAKE_CURRENT_SOURCE_DIR}
  ( -name \\*.cc -or -name \\*.hh -or -name \\*.cpp -or
    -name \\*.h -or -name \\*.c -or -name \\*.f )
  -exec echo \"{}\" \; > cscope.files && cscope -q -b VERBATIM)
endif (UNIX)

############################################################
# "make lint" target
############################################################
if (UNIX)

  file(GLOB_RECURSE LINT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )

  FOREACH(item ${LINT_FILES})
    IF(NOT ((item MATCHES "_generated.h") OR
            (item MATCHES "pyarrow_api.h") OR
            (item MATCHES "xxhash.h") OR
            (item MATCHES "xxhash.cc") OR
            (item MATCHES "config.h") OR
            (item MATCHES "util/variant") OR
            (item MATCHES "zmalloc.h") OR
            (item MATCHES "ae.h")))
      LIST(APPEND FILTERED_LINT_FILES ${item})
    ENDIF()
  ENDFOREACH(item ${LINT_FILES})

  find_program(CPPLINT_BIN NAMES cpplint cpplint.py HINTS ${BUILD_SUPPORT_DIR})
  message(STATUS "Found cpplint executable at ${CPPLINT_BIN}")

  # Full lint
  # Balancing act: cpplint.py takes a non-trivial time to launch,
  # so process 12 files per invocation, while still ensuring parallelism
  add_custom_target(lint echo ${FILTERED_LINT_FILES} | xargs -n12 -P8
  ${CPPLINT_BIN}
  --verbose=2
  --linelength=90
  --filter=-whitespace/comments,-readability/todo,-build/header_guard,-build/c++11,-runtime/references,-build/include_order
  )
endif (UNIX)


############################################################
# "make format" and "make check-format" targets
############################################################

# runs clang format and updates files in place.
add_custom_target(format ${BUILD_SUPPORT_DIR}/run_clang_format.py
  ${CLANG_FORMAT_BIN}
  ${BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
  ${CMAKE_CURRENT_SOURCE_DIR})

# runs clang format and exits with a non-zero exit code if any files need to be reformatted

# TODO(wesm): Make this work in run_clang_format.py
add_custom_target(check-format ${BUILD_SUPPORT_DIR}/run_clang_format.py
   ${CLANG_FORMAT_BIN}
   ${BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
   ${CMAKE_CURRENT_SOURCE_DIR} 1)

############################################################
# "make clang-tidy" and "make check-clang-tidy" targets
############################################################
if (${CLANG_TIDY_FOUND})
  # runs clang-tidy and attempts to fix any warning automatically
  add_custom_target(clang-tidy ${BUILD_SUPPORT_DIR}/run-clang-tidy.sh ${CLANG_TIDY_BIN} ${CMAKE_BINARY_DIR}/compile_commands.json 1
  `find ${CMAKE_CURRENT_SOURCE_DIR} -name \\*.cpp | sed -e '/_generated/g'`)
  # runs clang-tidy and exits with a non-zero exit code if any errors are found.
  add_custom_target(check-clang-tidy ${BUILD_SUPPORT_DIR}/run-clang-tidy.sh ${CLANG_TIDY_BIN} ${CMAKE_BINARY_DIR}/compile_commands.json
  0 `find ${CMAKE_CURRENT_SOURCE_DIR} -name \\*.cpp |grep -v -F -f ${CMAKE_CURRENT_SOURCE_DIR}/src/.clang-tidy-ignore | sed -e '/_generated/g'`)

endif()

############################################################
# "make infer" target
############################################################

if (${INFER_FOUND})
  # runs infer capture
  add_custom_target(infer ${BUILD_SUPPORT_DIR}/run-infer.sh ${INFER_BIN} ${CMAKE_BINARY_DIR}/compile_commands.json 1)
  # runs infer analyze
  add_custom_target(infer-analyze ${BUILD_SUPPORT_DIR}/run-infer.sh ${INFER_BIN} ${CMAKE_BINARY_DIR}/compile_commands.json 2)
  # runs infer report
  add_custom_target(infer-report ${BUILD_SUPPORT_DIR}/run-infer.sh ${INFER_BIN} ${CMAKE_BINARY_DIR}/compile_commands.json 3)
endif()

############################################################
# "make iwyu" target
############################################################
if(UNIX)
  add_custom_target(iwyu ${BUILD_SUPPORT_DIR}/iwyu/iwyu.sh)
endif(UNIX)

############################################################
# Linker and Dependencies
############################################################

set(SQLINES_STATIC_LINK_LIBS)

if (SQLINES_WITH_BROTLI)
  SET(SQLINES_STATIC_LINK_LIBS
    brotli_dec
    brotli_enc
    brotli_common
    ${SQLINES_STATIC_LINK_LIBS})
endif()

if (SQLINES_WITH_LZ4)
  SET(SQLINES_STATIC_LINK_LIBS lz4_static ${SQLINES_STATIC_LINK_LIBS})
endif()

if (SQLINES_WITH_SNAPPY)
  SET(SQLINES_STATIC_LINK_LIBS snappy ${SQLINES_STATIC_LINK_LIBS})
endif()

if (SQLINES_WITH_ZLIB)
  SET(SQLINES_STATIC_LINK_LIBS zlib ${SQLINES_STATIC_LINK_LIBS})
endif()

if (SQLINES_WITH_ZSTD)
  SET(SQLINES_STATIC_LINK_LIBS zstd_static ${SQLINES_STATIC_LINK_LIBS})
endif()

if (SQLINES_WITH_GRPC)
  SET(SQLINES_STATIC_LINK_LIBS
    grpc_grp
    grpc_grpc
    grpc_grpcpp
    ${SQLINES_STATIC_LINK_LIBS})
endif()

if (SQLINES_ORC)
  SET(SQLINES_STATIC_LINK_LIBS
    orc
    protobuf
    ${SQLINES_STATIC_LINK_LIBS})
endif()

if (SQLINES_STATIC_LINK_LIBS)
  add_dependencies(sqlines_dependencies ${SQLINES_STATIC_LINK_LIBS})
endif()

set(SQLINES_BENCHMARK_LINK_LIBS
  sqlines_static
  sqlines_benchmark_main
  gtest
  ${SQLINES_STATIC_LINK_LIBS})

set(SQLINES_LINK_LIBS
  ${SQLINES_STATIC_LINK_LIBS})

set(SQLINES_SHARED_PRIVATE_LINK_LIBS
  ${BOOST_SYSTEM_LIBRARY}
  ${BOOST_FILESYSTEM_LIBRARY}
  ${BOOST_REGEX_LIBRARY})

set(SQLINES_STATIC_PRIVATE_LINK_LIBS
  ${BOOST_SYSTEM_LIBRARY}
  ${BOOST_FILESYSTEM_LIBRARY}
  ${BOOST_REGEX_LIBRARY})

if (NOT MSVC)
  set(SQLINES_LINK_LIBS
    ${SQLINES_LINK_LIBS}
    ${CMAKE_DL_LIBS})
endif()

set(SQLINES_MIN_TEST_LIBS
  sqlines_static
  ${SQLINES_STATIC_LINK_LIBS}
  gtest
  gtest_main)

if(NOT MSVC)
  set(SQLINES_MIN_TEST_LIBS
    ${SQLINES_MIN_TEST_LIBS}
    ${CMAKE_DL_LIBS})
endif()

set(SQLINES_TEST_LINK_LIBS ${SQLINES_MIN_TEST_LIBS})

if (SQLINES_JEMALLOC)
  add_definitions(-DSQLINES_JEMALLOC)
  add_definitions(-DSQLINES_JEMALLOC_INCLUDE_DIR=${JEMALLOC_INCLUDE_DIR})

  if (CMAKE_COMPILER_IS_GNUCXX)
    set(SQLINES_JEMALLOC_LINK_LIBS
      jemalloc_static
      # For glibc <2.17 we need to link to librt.
      # As we compile with --as-needed by default, the linker will omit this
      # dependency if not required.
      ${PTHREAD_LIBRARY}
      rt
      )
  else()
    set(SQLINES_JEMALLOC_LINK_LIBS
      jemalloc_static
      )
  endif()
  set(SQLINES_SHARED_PRIVATE_LINK_LIBS
    ${SQLINES_SHARED_PRIVATE_LINK_LIBS}
    ${SQLINES_JEMALLOC_LINK_LIBS})
  set(SQLINES_STATIC_LINK_LIBS
    ${SQLINES_STATIC_LINK_LIBS}
    ${SQLINES_JEMALLOC_LINK_LIBS})
elseif (NOT MSVC)
  # We need to separate this as otherwise CMake would mess with the library
  # linking order.
  set(SQLINES_LINK_LIBS
    ${SQLINES_LINK_LIBS}
    ${PTHREAD_LIBRARY})
  set(SQLINES_STATIC_LINK_LIBS
    ${SQLINES_STATIC_LINK_LIBS}
    ${PTHREAD_LIBRARY})
endif()

############################################################
# Subdirectories
############################################################

add_subdirectory(sqlparser)

add_subdirectory(sqlines)

add_subdirectory(sqlines-w)

add_subdirectory(sqldata)

add_subdirectory(sqldata_w)

add_subdirectory(sqlines-studio)

execute_process(
  # NOTE: Uncomment following line after checking into GIT
  COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
  # TODO: Delete following line after checking into GIT
  #COMMAND echo "ccccccc"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE SQLINES_GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
file(WRITE ${CMAKE_BINARY_DIR}/SQLINES_GIT_HASH.txt "${SQLINES_GIT_HASH}\n")
file(STRINGS ${CMAKE_BINARY_DIR}/SQLINES_GIT_HASH.txt SQLINES_GIT_HASH)
set(CPACK_PACKAGE_VERSION "${SQLINES_VERSION_RAW}-${SQLINES_BUILD_DATE}-${SQLINES_GIT_HASH}")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/sqlcommon/release.h"
  "${CMAKE_BINARY_DIR}/sqlines_release.h"
  @ONLY
  )
list(APPEND ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/SQLINES_GIT_HASH.txt)
list(APPEND ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/sqlines_release.h)
# required to force regen of SQLINES_GIT_HASH.txt, sqlines_release.h
add_custom_target(rerun_cmake ALL
  COMMAND cmake .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
add_dependencies(sqlines rerun_cmake)

add_custom_target(clean-all
  COMMAND ${CMAKE_BUILD_TOOL} clean
  )

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADDITIONAL_MAKE_CLEAN_FILES}")

install(FILES ${CMAKE_BINARY_DIR}/SQLINES_GIT_HASH.txt DESTINATION ".")
