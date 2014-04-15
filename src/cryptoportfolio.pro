#-------------------------------------------------
#
# Project created by QtCreator 2014-03-31T16:29:50
#
#-------------------------------------------------

QT       += sql core gui network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cryptoportfolio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    marketdata.cpp \
    asseteditdelegate.cpp \
    dbhelper.cpp

HEADERS  += mainwindow.h \
    marketdata.h \
    asseteditdelegate.h \
    dbhelper.h

FORMS    += mainwindow.ui
