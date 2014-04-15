#-------------------------------------------------
#
# Project created by QtCreator 2014-03-31T16:29:50
#
#-------------------------------------------------

QT       += sql core network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cryptoportfolio_test
TEMPLATE = app


SOURCES += \
    ../src/marketdata.cpp \
    testmarketdata.cpp \
    ../src/dbhelper.cpp

HEADERS  += \
    ../src/marketdata.h \
    testmarketdata.h \
    ../src/dbhelper.h

