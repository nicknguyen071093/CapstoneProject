#include "RecognitionTimerThread.h"

RecognitionTimerThread::RecognitionTimerThread(QObject *parent) :
    QThread(parent)
{
    activeWorking = false;
    STOP = false;
}

void RecognitionTimerThread::run() {
    while(true) {
        mutex.lock();
        if (STOP) break;
        mutex.unlock();
        if (activeWorking) {
            emit sendSignalCountDownRecognitionTime("3");
            sleep(1);
            emit sendSignalCountDownRecognitionTime("2");
            sleep(1);
            emit sendSignalCountDownRecognitionTime("1");
            sleep(1);
            emit sendSignalUpdatingContent();
        }
    }
}

void RecognitionTimerThread::enableWorking(bool isWorking) {
    activeWorking = isWorking;
}
