#ifndef HANDTHREAD_H
#define HANDTHREAD_H

#include <QThread>
#include "HandGesture.h"
#include "ImageTranslating.h"
#include "unistd.h"

class HandThread : public QThread
{
    Q_OBJECT
public:
    explicit HandThread(QObject *parent = 0);
    // Mode that presamples hand colors
    const static int SAMPLE_MODE = 0;
    // Mode that generates binary image
    const static  int DETECTION_MODE = 1;
    // Mode that displays color image together with contours, fingertips,
    // defect points and so on.
    const static  int TRAIN_REC_MODE = 2;
    // Mode that presamples background colors
    const static  int BACKGROUND_MODE = 3;
    void setMode(int);
    void releaseAll();
    bool STOP;

private:
    VideoCapture webSource;
    Mat frame;
    vector<uchar> mem;
    // Stores all the information about the hand
    HandGesture *handGesture;
    int squareLen;
    const static int SAMPLE_NUM = 7;
    Scalar mColorsRGB[3] = {Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255)};
    double cLower[SAMPLE_NUM][3];
    double cUpper[SAMPLE_NUM][3];
    double cBackLower[SAMPLE_NUM][3];
    double cBackUpper[SAMPLE_NUM][3];
    Scalar lowerBound;
    Scalar upperBound;
    double avgColor[SAMPLE_NUM][3];
    double avgBackColor[SAMPLE_NUM][3];
    Point samplePoints[SAMPLE_NUM][2];
    Mat sampleMats[SAMPLE_NUM];

    Mat interMat;
    Mat blurMat;
    Mat rgbMat;
    Mat binMat;
    Mat tmpMat;
    Mat binTmpMat;
    Mat binTmpMat2 ;
    int mode;
    int curLabel;
    QImage imageObject;
    void run();
    //void finished();

    void initCLowerUpper(double , double , double , double ,
                         double , double );
    void initCBackLowerUpper(double , double , double , double ,
                             double , double );
    Mat preSampleHand(Mat);
    Mat preSampleBack(Mat);
    bool isClosedToBoundary(Point, Mat);
    void boundariesCorrection();
    void produceBinHandImg(Mat &, Mat &);
    void produceBinBackImg(Mat &, Mat &);
    Rect makeBoundingBox(Mat &);
    void adjustBoundingBox(Rect , Mat );
    void makeContours();
    Mat produceBinImg(Mat &, Mat &);

signals:
    void handTrackingChanged(QImage);
    void handSubtractingChanged(Mat,Rect);

public slots:
    // void onHandTrackingChanged(QImage);
};

#endif // HANDTHREAD_H
