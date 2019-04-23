
TEMPLATE = app

CONFIG += thread console c++14
CONFIG -= app_bundle
CONFIG -= qt

!win32 {
  DEFINES += __unix
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

# Define the directory where jsonArango source code is located
JSONIO14_DIR =  $$PWD/src
JSONIO14_HEADERS_DIR =  $$PWD/include/jsonio14

DEPENDPATH   += $$JSONIO14_DIR
DEPENDPATH   += $$JSONIO14_HEADERS_DIR

INCLUDEPATH   += $$JSONIO14_DIR
INCLUDEPATH   += $$JSONIO14_HEADERS_DIR

#LIBS +=  -lcurl  -lvelocypack
!win32:LIBS +=  -lboost_regex -lboost_system -lboost_filesystem

OBJECTS_DIR   = obj

SOURCES += \
        main.cpp

include($$JSONIO14_DIR/jsonio14.pri)
