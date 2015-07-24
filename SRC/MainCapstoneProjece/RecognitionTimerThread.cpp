#include "RecognitionTimerThread.h"

RecognitionTimerThread::RecognitionTimerThread(QObject *parent) :
    QThread(parent)
{
    enableWorking = false;
}

void RecognitionTimerThread::run() {
    while(enableWorking) {
        emit sendSignalCountDownRecognitionTime("3");
        sleep(1);
        emit sendSignalCountDownRecognitionTime("2");
        sleep(1);
        emit sendSignalCountDownRecognitionTime("1");
        sleep(1);
        emit sendSignalUpdatingContent();
    }
}
