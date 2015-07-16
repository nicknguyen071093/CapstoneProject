#include "TimerThread.h"

TimerThread::TimerThread(QObject *parent) :
    QThread(parent)
{
}

void TimerThread::run() {
    QMutex mutex;
    mutex.lock();
    if(this->STOP) break;
    mutex.unlock();
    sleep(3);
    sleep(1);
    //
    while(!isBackgroundNotChanged) {
    }
    // emit tang so
    sleep(1);
    while(!isBackgroundNotChanged) {
    }
    // emit tang so
    sleep(1);
    while(!isBackgroundNotChanged) {
    }
    // emit tang so
}
