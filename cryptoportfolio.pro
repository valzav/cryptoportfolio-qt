#-------------------------------------------------
#
# Project created by QtCreator 2014-03-31T16:29:50
#
#-------------------------------------------------

QT       += sql core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cryptoportfolio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    assetdelegate.cpp \
    marketdata.cpp

HEADERS  += mainwindow.h \
    initdb.h \
    assetdelegate.h \
    marketdata.h

FORMS    += mainwindow.ui
