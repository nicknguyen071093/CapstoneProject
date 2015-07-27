#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T15:54:32
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MainCapstoneProjece
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
    ShowingImageThread.cpp \
    RetrievingFrame.cpp \
    CroppingImage.cpp \
    TimerThread.cpp \
    SignRecognition.cpp \
    RecognitionTimerThread.cpp \
    WordDTO.cpp \
    Words.cpp


HEADERS  += mainwindow.h \
    HandGesture.h \
    ImageTranslating.h \
    SVM.h \
    ShowingImageThread.h \
    RetrievingFrame.h \
    CroppingImage.h \
    TimerThread.h \
    SignRecognition.h \
    RecognitionTimerThread.h \
    WordDTO.h \
    Words.h

FORMS    += mainwindow.ui

#INCLUDEPATH += /usr/local/include/espeak
#LIBS += -lespeak
