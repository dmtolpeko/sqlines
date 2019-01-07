SRC = \
  ../sqlcommon/applog.cpp \
  ../sqlcommon/file.cpp \
  filelist.cpp \
  main.cpp \
  ../sqlcommon/os.cpp \
  ../sqlcommon/parameters.cpp \
  sqlines.cpp \
  ../sqlcommon/str.cpp

LIBS = ../sqlparser/sqlparser.a
LDFLAGS = -ldl

sqlines: $(SRC) $(LIBS)
	g++ $(CXXFLAGS) $(SRC) $(LIBS) $(LDFLAGS) -o $@

clean:
	rm -f sqlines
