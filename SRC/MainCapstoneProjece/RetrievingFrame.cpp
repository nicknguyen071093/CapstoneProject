#include "RetrievingFrame.h"

RetrievingFrame::RetrievingFrame(QObject *parent) :
    QThread(parent)
{
    camera = VideoCapture(0);
    camera.set(CV_CAP_PROP_FRAME_WIDTH,320);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT,240);
    camera.set(CV_CAP_PROP_SATURATION,0.7);
    STOP = false;
}

void RetrievingFrame::run() {
    while(true) {
        QMutex mutex;
        mutex.lock();
        if(this->STOP)
        {
            camera.release();
            break;
        }
        mutex.unlock();
        if (camera.read(frame)) {
            emit changingFrame(frame.clone());
        }
    }
}

void RetrievingFrame::setToDefaults() {
    camera = VideoCapture(0);
    camera.set(CV_CAP_PROP_FRAME_WIDTH,320);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT,240);
    camera.set(CV_CAP_PROP_SATURATION,0.7);
    STOP = false;
}
