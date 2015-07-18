#ifndef SHOWINGIMAGETHREAD_H
#define SHOWINGIMAGETHREAD_H

#include <QThread>
#include <QMutex>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace  std;

class ShowingImageThread : public QThread
{
    Q_OBJECT
public:
    explicit ShowingImageThread(QObject *parent = 0);
    bool STOP;
    // Mode that presamples background
    const static  int BACKGROUND_MODE = 0;
    // Mode that stores background features
    const static  int GET_AVG_BACKGROUND = 1;
    // Mode that presamples the front of hand
    const static int SAMPLE_FRONT_HAND_MODE = 2;
    // Mode that stores front of hand features
    const static  int GET_AVG_FRONT_HAND = 3;
    // Mode that presamples the back of hand
    const static int SAMPLE_BACK_HAND_MODE = 4;
    // Mode that stores b of hand features
    const static  int GET_AVG_BACK_HAND = 5;
    // Mode that generates binary image
    const static  int DETECTION_MODE = 6;
    // Mode that displays color image together with contours, fingertips,
    // defect points and so on.
    const static  int TRAIN_REC_MODE = 7;
    //
    const static int GETTING_FIRST_AVG_FRONT_HAND = 8;
    //
    const static int GETTING_FIRST_AVG_BACK_HAND = 10;
    //
    const static int CHECKING_CHANGING_AVG_FRONT_HAND = 9;
    //
    const static int CHECKING_CHANGING_AVG_BACK_HAND = 11;
    void setMode(int);
    void setToDefaults();

private:
    void run();
    // check whether to execute
    bool enableToShow;
    // frame is received from camera, image after bluring, lab Image, image is shown
    Mat frame, blurMat, labMat, showMat;
    // binary hand image
    Mat binMat;
    // temporary binary hand image
    Mat tmpMat;
    // temporary binary mat of hand is used to generate binary mat contains hand.
    Mat binTmpMat;
    // temporary binary mat of background is used to generate binary mat contains hand.
    Mat binTmpMat2 ;
    // current mode
    int mode;
    // width of image
    const static int IMAGE_COLS = 320;
    // height of image
    const static int IMAGE_ROWS = 240;
    // length of square to ressample background
    const static int SQUARE_LENGTH = IMAGE_ROWS / 20;
    // Color Blue
    Scalar blue;
    // Color Red
    Scalar red;
    // Color Green
    Scalar green;
    // Color White
    Scalar white;
    // Color Black
    Scalar black;
    // number of background's features
    const static int SAMPLE_BACK_NUM = 12;
    // number of hand's features
    const static int SAMPLE_HAND_NUM = 13;
    // array to store the frond hand's feature range
    double cFrontHandLower[SAMPLE_HAND_NUM][3];
    double cFrontHandUpper[SAMPLE_HAND_NUM][3];
    // array to store the frond hand's feature range
    double chekingRangeFrontHandLower[SAMPLE_HAND_NUM][3];
    double chekingRangeFrontHandUpper[SAMPLE_HAND_NUM][3];
    // array to store the back hand's feature range
    double cBackHandLower[SAMPLE_HAND_NUM][3];
    double cBackHandUpper[SAMPLE_HAND_NUM][3];
    // array to store specific background's feature range
    double cBackLower[SAMPLE_BACK_NUM][3];
    double cBackUpper[SAMPLE_BACK_NUM][3];
    // array to store additional background's feature range which will be identified within current background
    double addBackLower[50][3];
    double addBackUpper[50][3];
    // array to store the position of the background features
    Point sampleBackPoints[SAMPLE_BACK_NUM][2];
    // array to store the position of the front hand features
    Point sampleHandPoints[SAMPLE_HAND_NUM];
    // array to store the position of the back hand features
    Point sampleBackHandPoints[SAMPLE_HAND_NUM];
    // sample of the front hand to draw
    vector < vector<Point> > sampleFrontHand;
    // sample of the back hand to draw
    vector < vector<Point> > sampleBackHand;
    // is used to find contours
    vector<Vec4i> hierarchy;
    // array to store resampling colors of background
    double avgBackColor[SAMPLE_BACK_NUM][3];
    // array to store resampling colors of background which is identified within current background
    vector<Vec3b> additionAvgBackColor;
    // array to store resampling colors of the front hand
    double avgColor[SAMPLE_HAND_NUM][3];
    // array to store resampling colors of the back hand
    double avgBackHandColor[SAMPLE_HAND_NUM][3];
    // array of Mats which stores points in range of hand.
    Mat sampleMats[SAMPLE_HAND_NUM];
    // array of Mats which stores points in range of background.
    vector<Mat> sampleBackgroundMats;
    // to store lower value of range of a specific sample
    Scalar lowerBound;
    // to store upper value of range of a specific sample
    Scalar upperBound;
    // initiate range of sample color of hand
    void initCLowerUpper(double , double , double , double ,
                         double , double );
    // initiate range of sample color of hand
    void initCBackLowerUpper(double , double , double , double ,
                             double , double );
    // identify the position of hand features
    void initHandPoints();
    // identify the position of background features
    void initBackPoints();
    // Draw specific feature points of background.
    Mat preSampleBack(Mat);
    // Draw specific feature points of the front of hand.
    Mat preSampleFrontHand(Mat);
    // Draw specific feature points of the back of hand.
    Mat preSampleBackHand(Mat);
    // Presampling background colors.
    // Output is avgBackColor, additionAvgBackColor
    void getSampleBack();
    //
    void getRangeForCheckingFrontHand();
    //
    void getRangeForCheckingBackHand();
    //
    bool checkCurrentAvgFrontHandIsInRange();
    //
    bool checkCurrentAvgBackHandIsInRange();
    // Presampling the front hand colors.
    // Output is avgColor,
    void getSampleHand();
    // Presampling the back hand colors.
    // Output is avgBackHandColor,
    void getSampleBackHand();
    // Generates binary image containing user's hand
    Mat produceBinImg();
    // recalculate range of features.
    void boundariesCorrection();
    // Generates binary image thresholded only by sampled hand colors
    void produceBinHandImg();
    // Generates binary image thresholded only by sampled background colors
    void produceBinBackImg();
    //    bool isGettingFirstFinished;

signals:
    void toShow(Mat);
    void sendImageToCrop(Mat,Mat);
    void sendSignalEnableCountDown();
public slots:
    void onChangingImage(Mat);
    void moveToCheckFrontHand();
    void moveToCheckBackHand();
};

#endif // SHOWINGIMAGETHREAD_H
