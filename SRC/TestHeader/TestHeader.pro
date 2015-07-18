#-------------------------------------------------
#
# Project created by QtCreator 2015-05-20T22:32:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestHeader
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/espeak
LIBS += -L/usr/local/lib
LIBS += -lespeak
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann
LIBS += -lopencv_nonfree
LIBS += -lopencv_video
LIBS += -lopencv_videostab



SOURCES += main.cpp\
        mainwindow.cpp \
    SVM.cpp \
    HandGesture.cpp

HEADERS  += mainwindow.h \
    SVM.h \
    HandGesture.h

FORMS    += mainwindow.ui
