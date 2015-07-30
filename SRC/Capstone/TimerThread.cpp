#include "TimerThread.h"

TimerThread::TimerThread(QObject *parent) :
    QThread(parent)
{
    isEnableToCountDown = false;
    STOP = false;
}

void TimerThread::run() {
    //    QMutex mutex;
    //    mutex.lock();
    //    if(this->STOP) break;
    //    mutex.unlock();
    sleep(3);
    //    countDownAnalyzingBackgroundNumber = 3;
    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay trước trong vòng 5 giây."));
    sleep(1);
    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay trước trong vòng 4 giây."));
    sleep(1);
    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay trước trong vòng 3 giây."));
    sleep(1);
    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay trước trong vòng 2 giây."));
    sleep(1);
    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay trước trong vòng 1 giây."));
    sleep(1);
    if (!STOP) emit sendSignalChangingToFrontHandMode();

    // front hand time
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    //        sleep(0.25);
//    //        if (!STOP) emit (isEnableToCountDown = sendSignalToCheckFirst());
//    //    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay sau trong vòng 5 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingFrontHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay sau trong vòng 4 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingFrontHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay sau trong vòng 3 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingFrontHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay sau trong vòng 2 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingFrontHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ lấy màu tay sau trong vòng 1 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingFrontHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingToBackHandMode();

//    // back hand time
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ ảnh nhị phân trong vòng 5 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingBackHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ ảnh nhị phân trong vòng 4 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingBackHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ ảnh nhị phân trong vòng 3 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingBackHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ ảnh nhị phân trong vòng 2 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingBackHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Thông báo: Sẽ chuyển sang chế độ ảnh nhị phân trong vòng 1 giây."));
//    sleep(1);
//    isEnableToCountDown = false;
//    if (!STOP) emit sendSignalCheckingBackHand();
//    while (!isEnableToCountDown) {
//        QMutex mutex;
//        mutex.lock();
//        if(this->STOP) break;
//        mutex.unlock();
//    }
//    if (!STOP) emit sendSignalChangingToBinaryImage();
}

void TimerThread::continueCountDown() {
    isEnableToCountDown = true;
}

void TimerThread::setToDefaults() {
    isEnableToCountDown = false;
    STOP = false;
}


