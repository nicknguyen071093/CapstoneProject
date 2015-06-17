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
    const static  int GET_AVG_BACKGROUND = 4;
    // Mode that presamples background colors
    const static  int GET_AVG_HAND = 5;
    // Stores all the information about the hand
    HandGesture *handGesture;
    void setMode(int);
    void releaseAll();

    bool STOP;

private:
    VideoCapture webSource;
    Mat frame;
    vector<uchar> mem;
    int squareLen;
    const static int SAMPLE_NUM = 7;
    Scalar mColorsRGB[3] = {Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255)};
    double cLower[SAMPLE_NUM][3];
    double cUpper[SAMPLE_NUM][3];
    double cBackLower[SAMPLE_NUM][3];
    double cBackUpper[SAMPLE_NUM][3];
    //vector<double[3]> addBackLower;
    //vector<double[3]> addBackUpper;
    double addBackLower[50][3];
    double addBackUpper[50][3];
    Scalar lowerBound;
    Scalar upperBound;
    double avgColor[SAMPLE_NUM][3];
    double avgBackColor[SAMPLE_NUM][3];
    vector<Vec3b> additionAvgBackColor;
    Point samplePoints[SAMPLE_NUM][2];
    Mat sampleMats[SAMPLE_NUM];
//    Mat tmpSampleBackgroundMat;
    vector<Mat> sampleBackgroundMats;
    int cols;
    int rows;
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
    void getSampleBack();
    void getSampleHand();
    bool isClosedToBoundary(Point, Mat);
    void boundariesCorrection();
    void produceBinHandImg();
    void produceBinBackImg();
    Rect makeBoundingBox(Mat &);
    void adjustBoundingBox(Rect , Mat );
    Mat makeContours();
    Mat produceBinImg();

signals:
    void handTrackingChanged(QImage);
    void binaryImageHandChanged (Mat, Mat);
    void handSubtractingChanged(Mat,Mat,Rect);

public slots:
    // void onHandTrackingChanged(QImage);
};

#endif // HANDTHREAD_H
