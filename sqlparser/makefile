override CXXFLAGS += -fpermissive

OBJS = \
  dllmain.o \
  cobol.o \
  clauses.o \
  datatypes.o \
  db2.o \
  functions.o \
  greenplum.o \
  guess.o \
  informix.o \
  language.o \
  mysql.o \
  oracle.o \
  postgresql.o \
  report.o \
  select.o \
  helpers.o \
  patterns.o \
  post.o \
  procedures.o \
  storage.o \
  sqlparser.o \
  sqlserver.o \
  statements.o \
  stats.o \
  sybase.o \
  teradata.o \
  token.o

all: sqlparser.a

.cpp.o:
	g++ $(CXXFLAGS) -o $@ -c $<

sqlparser.a: $(OBJS)
	ar rcs $@ $(OBJS)

clean:
	rm -f sqlparser.a *.o
