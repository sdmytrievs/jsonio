TEMPLATE = app
CONFIG += thread console c++14
CONFIG -= app_bundle
CONFIG -= qt
# https://resources.qt.io/qt-world-summit-2018/qtws18-kdabs-opensource-tools-for-qt-milian-wolff-kdab
#Clazy
#GammaRay
#Heaptrack
#Hotspot

!win32 {
  DEFINES += __unix
QMAKE_CFLAGS += pedantic -Wall -Wextra -Wwrite-strings -Werror
QMAKE_CXXFLAGS += -ansi -pedantic -Wall -Wextra -Weffc++
#QMAKE_CXXFLAGS += -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith \
# -Wmissing-declarations -Winline -Wundef -Weffc++ \
# -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter \
#-Wfloat-equal -pedantic -ansi
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
  INCLUDEPATH   += "C:\usr\local\include"
  DEPENDPATH   += "C:\usr\local\include"
  LIBPATH += "C:\usr\local\lib"
  INCLUDEPATH   += "C:\usr\local\boost\include\boost-1_70"
  DEPENDPATH   += "C:\usr\local\boost\include\boost-1_70"
  LIBPATH += "C:\usr\local\boost\lib"
}

# Define the directory where jsonArango source code is located
JSONIO14_DIR =  $$PWD/src
JSONIO14_HEADERS_DIR =  $$PWD/include

DEPENDPATH   += $$JSONIO14_DIR
DEPENDPATH   += $$JSONIO14_HEADERS_DIR

INCLUDEPATH   += $$JSONIO14_DIR
INCLUDEPATH   += $$JSONIO14_HEADERS_DIR

#LIBS +=  -lcurl  -lvelocypack
win32:LIBS +=   -lboost_system-mgw73-mt-d-x64-1_70 -lboost_filesystem-mgw73-mt-d-x64-1_70
!win32:LIBS +=  -lboost_system -lboost_filesystem -lstdc++fs
LIBS +=   -lvelocypack

OBJECTS_DIR   = obj

include($$JSONIO14_DIR/jsonio14.pri)

SOURCES += \
       main.cpp
#      examples/create_example.cpp \
#      examples/exbuilder.cpp
#      examples/exparser.cpp
#      examples/time_parser.cpp

DISTFILES += \
    Resources/docs/source.md \
    Resources/thrift/schema_test.thrift



