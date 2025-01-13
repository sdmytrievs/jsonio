TEMPLATE = app
CONFIG += thread console c++2a
CONFIG -= app_bundle
CONFIG -= qt
#CONFIG += sanitizer sanitize_thread
# https://resources.qt.io/qt-world-summit-2018/qtws18-kdabs-opensource-tools-for-qt-milian-wolff-kdab
#Clazy
#GammaRay
#Heaptrack
#Hotspot


QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

!win32 {
  DEFINES += __unix
QMAKE_CFLAGS += pedantic -Wall -Wextra -Wwrite-strings -Werror
#QMAKE_CXXFLAGS += -ansi -pedantic -Wall -Wextra -Weffc++
QMAKE_CXXFLAGS += -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith \
 -Wmissing-declarations -Winline -Wundef \ #-Weffc++ \
 -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter \
 -Wfloat-equal -pedantic -ansi

#QMAKE_CXXFLAGS += -pg  # gprof information

}

macx-g++ {
  DEFINES += __APPLE__
}

macx-clang {
  DEFINES += __APPLE__
  INCLUDEPATH   += "/usr/local/include"
  DEPENDPATH   += "/usr/local/include"
  LIBPATH += "/usr/local/lib/"
}

win32 {
  #CONFIG += staticlib
  #QMAKE_CXXFLAGS_DEBUG += -MTd
  INCLUDEPATH   += "C:\usr\local\include"
  DEPENDPATH   += "C:\usr\local\include"
  LIBPATH += "C:\usr\local\lib"
}

# Define the directory where jsonArango source code is located
JSONIO_DIR =  $$PWD/src
JSONIO_HEADERS_DIR =  $$PWD/include

DEPENDPATH   += $$JSONIO_DIR
DEPENDPATH   += $$JSONIO_HEADERS_DIR

INCLUDEPATH   += $$JSONIO_DIR
INCLUDEPATH   += $$JSONIO_HEADERS_DIR

#LIBS += -pg  # gprof information

unix:!macx-clang:LIBS += -pg  -lstdc++fs
win32:LIBS +=  -larango-cpp-static -llibcurl
!win32:LIBS += -larango-cpp -lcurl
LIBS +=   -lvelocypack

OBJECTS_DIR   = obj

include($$JSONIO_DIR/jsonio.pri)

SOURCES += \
#       main.cpp
#      examples/create_example.cpp
      examples/schema_example.cpp
#      examples/builder_example.cpp
#       examples/parser_example.cpp
#       examples/time_parser.cpp
#       examples/coll_query_example.cpp
#       examples/coll_time_test.cpp

DISTFILES += \
    Resources/docs/source.md \
    Resources/thrift/schema_test.thrift



#thrift -r -v --gen json
