QT += testlib network core
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_qtjsonsocketrefactory.cpp \
    qtjsonsocketlib_v3.cpp \
    Dependency/qt_json_socket_lib.cpp

HEADERS += \
    qtjsonsocketlib_v3.h \
    Dependency/qt_json_socket_lib.h \
    jsonsockettestserver.h
