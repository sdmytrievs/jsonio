TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt

#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

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
}

# Define the directory where jsonio17 source code is located
JSONIO17_DIR =  $$PWD/src
JSONIO17_HEADERS_DIR =  $$PWD/include
TESTS_DIR =  $$PWD/tests

DEPENDPATH   += $$JSONIO17_DIR
DEPENDPATH   += $$JSONIO17_HEADERS_DIR
DEPENDPATH   += $$TESTS_DIR

INCLUDEPATH   += $$JSONIO17_DIR
INCLUDEPATH   += $$JSONIO17_HEADERS_DIR
INCLUDEPATH   += $$TESTS_DIR

win32:LIBS +=  -ljsonarango-static
!win32:LIBS += -ljsonarango

#unix:!macx-clang:LIBS += -lstdc++fs
#win32:LIBS +=  -ljsonarango-static -llibcurl
#!win32:LIBS += -ljsonarango -lcurl
#LIBS +=   -lvelocypack

OBJECTS_DIR   = obj

include($$TESTS_DIR/gtest_dependency.pri)
include($$JSONIO17_DIR/jsonio17.pri)

HEADERS += \
        $$TESTS_DIR/example_schema.h \
        $$TESTS_DIR/tst_jsonio.h \
        $$TESTS_DIR/tst_service.h \
        $$TESTS_DIR/tst_detail.h \
        $$TESTS_DIR/tst_dump.h   \
        $$TESTS_DIR/tst_base.h \
        $$TESTS_DIR/tst_base_api.h \
        $$TESTS_DIR/tst_builder.h \
        $$TESTS_DIR/tst_parser.h \
        $$TESTS_DIR/tst_base_complex.h \
        $$TESTS_DIR/tst_schema.h \
        $$TESTS_DIR/tst_jsonschema.h \
        $$TESTS_DIR/tst_dbquery.h

SOURCES += \
        $$TESTS_DIR/main.cpp

DISTFILES += \
    Resources/docs/source.md
