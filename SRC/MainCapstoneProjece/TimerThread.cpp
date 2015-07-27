#include "TimerThread.h"

TimerThread::TimerThread(QObject *parent) :
    QThread(parent)
{
    isEnableToCountDown = false;
    STOP = false;
    testingResult = false;
}

void TimerThread::run() {
    //    QMutex mutex;
    //    mutex.lock();
    //    if(this->STOP) break;
    //    mutex.unlock();
    while (!testingResult) {
        sleep(3);
        //    countDownAnalyzingBackgroundNumber = 3;
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Người dùng vui lòng di chuyển ra khỏi vùng camera đang theo dõi."),"5");
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Người dùng vui lòng di chuyển ra khỏi vùng camera đang theo dõi."),"4");
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Người dùng vui lòng di chuyển ra khỏi vùng camera đang theo dõi."),"3");
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Người dùng vui lòng di chuyển ra khỏi vùng camera đang theo dõi."),"2");
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Người dùng vui lòng di chuyển ra khỏi vùng camera đang theo dõi."),"1");
        sleep(1);
        if (!STOP) emit sendSignalChangingToFrontHandMode();
        // front hand time
        while (!isEnableToCountDown) {

            mutex.lock();
            if(this->STOP) break;
            mutex.unlock();
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"5");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingFrontHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"4");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingFrontHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"3");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingFrontHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"2");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingFrontHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"1");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingFrontHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingToBackHandMode();

        // back hand time
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"5");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingBackHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"4");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingBackHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"3");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingBackHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"2");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingBackHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn vào vùng bàn tay được hiển thị trên màn hình LCD."),"1");
        sleep(1);
        isEnableToCountDown = false;
        if (!STOP) emit sendSignalCheckingBackHand();
        while (!isEnableToCountDown && !STOP) {
        }
        if (!STOP)
        {
            emit sendSignalChangingToBinaryImage();
            emit sendSignalChangingToTestingTime();
        }
        //testing time
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn theo kí hiệu “kiểm tra” trong hướng dẫn."),"5");
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn theo kí hiệu “kiểm tra” trong hướng dẫn."),"4");
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn theo kí hiệu “kiểm tra” trong hướng dẫn."),"3");
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn theo kí hiệu “kiểm tra” trong hướng dẫn."),"2");
        sleep(1);
        if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Vui lòng điều chỉnh bàn tay của bạn theo kí hiệu “kiểm tra” trong hướng dẫn."),"1");
        sleep(1);

        //wait for back first step
        if (!STOP) {
            emit (testingResult = sendSignalGetTestingResult());
        }
        if (!testingResult) {
            emit sendSignalFailTesingResult(QString::fromUtf8("Thất bại"),"color: red");
            if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Hệ thống lấy bàn tay của bạn đã thất bại. Vui lòng làm theo hướng dẫn lấy mẫu bàn tay trong sổ hướng dẫn. Hệ thống sẽ tự động quay lại bước lấy mẫu."),"5");
            sleep(1);
            if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Hệ thống lấy bàn tay của bạn đã thất bại. Vui lòng làm theo hướng dẫn lấy mẫu bàn tay trong sổ hướng dẫn. Hệ thống sẽ tự động quay lại bước lấy mẫu."),"4");
            sleep(1);
            if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Hệ thống lấy bàn tay của bạn đã thất bại. Vui lòng làm theo hướng dẫn lấy mẫu bàn tay trong sổ hướng dẫn. Hệ thống sẽ tự động quay lại bước lấy mẫu."),"3");
            sleep(1);
            if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Hệ thống lấy bàn tay của bạn đã thất bại. Vui lòng làm theo hướng dẫn lấy mẫu bàn tay trong sổ hướng dẫn. Hệ thống sẽ tự động quay lại bước lấy mẫu."),"2");
            sleep(1);
            if (!STOP) emit sendSignalChangingLabelNotice(QString::fromUtf8("Hệ thống lấy bàn tay của bạn đã thất bại. Vui lòng làm theo hướng dẫn lấy mẫu bàn tay trong sổ hướng dẫn. Hệ thống sẽ tự động quay lại bước lấy mẫu."),"1");
            sleep(1);
        }
        if (!STOP) emit sendSignalFinishingColorSubtraction(testingResult);
    }
}

void TimerThread::continueCountDown() {
    isEnableToCountDown = true;
}

