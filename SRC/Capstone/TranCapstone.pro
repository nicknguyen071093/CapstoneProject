#-------------------------------------------------
#
# Project created by QtCreator 2015-06-03T10:00:57
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TranCapstone
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib
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

SOURCES += main.cpp\
        mainwindow.cpp \
    HandGesture.cpp \
    ImageTranslating.cpp \
    SVM.cpp \
    HandTranslating.cpp \
    letterget.cpp \
    wordDTO.cpp \
    ShowingImageThread.cpp \
    RetrievingFrame.cpp \
    CroppingImage.cpp \
    TimerThread.cpp


HEADERS  += mainwindow.h \
    HandGesture.h \
    ImageTranslating.h \
    SVM.h \
    HandTranslating.h \
    letterget.h \
    wordDTO.h \
    ShowingImageThread.h \
    RetrievingFrame.h \
    CroppingImage.h \
    TimerThread.h

FORMS    += mainwindow.ui

#INCLUDEPATH += /usr/local/include/espeak
#LIBS += -lespeak
