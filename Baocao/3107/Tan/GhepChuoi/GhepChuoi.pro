#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T08:55:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = GhepChuoi
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    showcontent.cpp
INCLUDEPATH += /usr/local/include/espeak
LIBS += -lespeak

HEADERS += \
    showcontent.h
