TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += thread
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

win32 {
  INCLUDEPATH   += "C:\usr\local\include"
  DEPENDPATH   += "C:\usr\local\include"
  LIBPATH += "C:\usr\local\lib"
  INCLUDEPATH   += "C:\usr\local\boost\include\boost-1_70"
  DEPENDPATH   += "C:\usr\local\boost\include\boost-1_70"
  LIBPATH += "C:\usr\local\boost\lib"
}

# Define the directory where jsonio14 source code is located
JSONIO14_DIR =  $$PWD/src
JSONIO14_HEADERS_DIR =  $$PWD/include/jsonio14
TESTS_DIR =  $$PWD/tests

DEPENDPATH   += $$JSONIO14_DIR
DEPENDPATH   += $$JSONIO14_HEADERS_DIR
DEPENDPATH   += $$TESTS_DIR

INCLUDEPATH   += $$JSONIO14_DIR
INCLUDEPATH   += $$JSONIO14_HEADERS_DIR
INCLUDEPATH   += $$TESTS_DIR

#LIBS +=  -lcurl  -lvelocypack
win32:LIBS +=   -lboost_system-mgw73-mt-d-x64-1_70 -lboost_filesystem-mgw73-mt-d-x64-1_70
!win32:LIBS +=  -lboost_system -lboost_filesystem

OBJECTS_DIR   = obj

include($$TESTS_DIR/gtest_dependency.pri)
include($$JSONIO14_DIR/jsonio14.pri)

HEADERS += \
        $$TESTS_DIR/tst_jsonio.h \
        $$TESTS_DIR/tst_service.h \
        $$TESTS_DIR/tst_detail.h \
        $$TESTS_DIR/tst_dump.h   \
        $$TESTS_DIR/tst_base.h \
        $$TESTS_DIR/tst_builder.h \
        $$TESTS_DIR/tst_parser.h

SOURCES += \
        $$TESTS_DIR/main.cpp
