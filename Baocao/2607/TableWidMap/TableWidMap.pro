#-------------------------------------------------
#
# Project created by QtCreator 2015-07-15T11:41:20
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TableWidMap
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    worddto.cpp \
    lettergetting.cpp \
    dialog.cpp

HEADERS  += mainwindow.h \
    worddto.h \
    lettergetting.h \
    dialog.h

FORMS    += mainwindow.ui \
    dialog.ui
