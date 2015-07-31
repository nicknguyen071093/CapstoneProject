#-------------------------------------------------
#
# Project created by QtCreator 2015-07-22T10:07:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demoo
TEMPLATE = app
LIBS += -lbcm2835

SOURCES += main.cpp\
        mainwindow.cpp \
    displaypinthread.cpp \
    dialog.cpp \
    threaddialog.cpp

HEADERS  += mainwindow.h \
    displaypinthread.h \
    dialog.h \
    threaddialog.h

FORMS    += mainwindow.ui \
    dialog.ui
