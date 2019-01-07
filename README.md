[![Build Status](https://secure.travis-ci.org/renesugar/sqlines.svg)](http://travis-ci.org/renesugar/sqlines)

# SQLines SQL Converter

SQLines SQL Converter is an open source tool (Apache License 2.0) that allows you to convert database schema (DDL), queries and DML statements, views, stored procedures, packages, functions and triggers between Microsoft SQL Server, Oracle, MariaDB, MySQL, PostgreSQL, IBM DB2, Sybase, Informix, Teradata, Greenplum and Netezza.

You can also try it online at http://www.sqlines.com/online

## Build SQLines SQL Converter

```
mkdir build
cd build
CMAKE_PREFIX_PATH=$QT_ROOT/clang_64/ cmake -DCMAKE_C_COMPILER=/usr/local/opt/llvm/bin/clang -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ -DSQLINES_USE_ASAN=ON .. -D_CMAKE_TOOLCHAIN_PREFIX=llvm-
make

(where $QT_ROOT is the path to the Qt library)

/usr/local/opt/llvm/bin/llvm-dsymutil ./sqlines


https://github.com/google/sanitizers/issues/207

"Right now llvm-symbolizer is unable to extract the debug info from the object files
accompanying the executable (to do so we basically need to reimplement dsymutil), so
the users must run dsymutil themselves."

http://clang.llvm.org/docs/AddressSanitizer.html

"Note that on OS X you may need to run dsymutil on your binary to have the file:line info in the AddressSanitizer reports."

Please run CMake with

-DBUILD_WARNING_LEVEL=CHECKIN

to avoid failures due to compiler warnings.
```

## Development

There are several `make` targets that can be used to run development tools.

* Runs `cpplint` through the build system:

```
make lint
```

* Runs `clang-format` and updates files in place:

```
make format
```

* Runs `clang-format` and returns an error if any files need to be reformatted:

```
make check-format
```

* Runs `clang-tidy` and updates files in place:

```
make clang-tidy
```

* Runs `clang-tidy` and returns an error if any errors are found:

```
make check-clang-tidy
```

* After building, you can run the [infer](http://fbinfer.com/docs/getting-started.html) static analysis tool.

```
brew update
brew install infer
```

Make targets have been added to run the **infer** steps.

Other ways to use **infer** can be found in the article [Recommended flow for CI](http://fbinfer.com/docs/steps-for-ci.html).


1. First run the capture step using:
```
make infer
```
2. Next, run the analyze step using:
```
make infer-analyze
```
Note: The analyze step can take a very long time.

3. Next, run the report step using:
```
make infer-report
```

## Support

For technical support and custom development, please contact us at support@sqlines.com

