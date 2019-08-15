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
!win32:LIBS +=  -lboost_regex -lboost_system -lboost_filesystem

include($$TESTS_DIR/gtest_dependency.pri)
include($$JSONIO14_DIR/jsonio14.pri)


HEADERS += \
        $$TESTS_DIR/tst_jsonio.h \
        $$TESTS_DIR/tst_detail.h \
        $$TESTS_DIR/tst_dump.h   \
        $$TESTS_DIR/tst_service.h \
        $$TESTS_DIR/tst_base.h \
        $$TESTS_DIR/tst_builder.h \
        $$TESTS_DIR/tst_parser.h

SOURCES += \
        $$TESTS_DIR/main.cpp
