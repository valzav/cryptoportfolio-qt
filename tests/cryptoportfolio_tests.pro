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
    ../marketdata.cpp \
    testmarketdata.cpp \
    ../dbhelper.cpp

HEADERS  += \
    ../marketdata.h \
    testmarketdata.h \
    ../dbhelper.h

