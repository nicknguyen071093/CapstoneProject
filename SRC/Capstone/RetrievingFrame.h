#ifndef RETRIEVINGFRAME_H
#define RETRIEVINGFRAME_H

#include <QThread>
#include <QMutex>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>


using namespace cv;
using namespace std;
class RetrievingFrame : public QThread
{
    Q_OBJECT
public:
    explicit RetrievingFrame(QObject *parent = 0);
    bool STOP;
    void setToDefaults();

private:
    void run();
    Mat frame;
    VideoCapture camera;

signals:
    void changingFrame(Mat);

public slots:

};

#endif // RETRIEVINGFRAME_H
