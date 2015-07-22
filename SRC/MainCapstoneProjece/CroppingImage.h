#ifndef CROPPINGIMAGE_H
#define CROPPINGIMAGE_H

#include <QThread>
#include <QMutex>
#include <HandGesture.h>
#include <SignRecognition.h>

#define NUMBER_FEATURES 75
using namespace cv;

class CroppingImage : public QThread
{
    Q_OBJECT
public:
    explicit CroppingImage(QObject *parent = 0);
    bool STOP;
    void setToDefaults();
    const static int TESTING_MODE = 1;
    const static int SELECTING_MODE = 2;

private:
    void run();
    int mode;
    bool testingResult;
    // recieved binary image from ShowingImageThread
    Mat binaryMat;
    // recieved frame image from ShowingImageThread
    Mat frame;
    //    Mat croppedBinMat;
    Mat croppedFrame;
    Mat croppedHand, croppedBinHand, croppedInnerHand, croppedXuongHand;
    Mat subROI,subBinROI, subInnerROI, subXuongROI;
    //    Mat mask;
    Mat maskForFrame;
    Mat maskForInner;
    Mat maskForXuong;
    //    Rect rectToCrop;
    int squareLength;
    int tly, tlx;
    double tile, dem;
    Size cropImageSize;
    //
    HandGesture *handGesture;
    SignRecognition *signRecogntion;
    // check whether we can crop image
    bool enableToCrop;
    // Color White
    Scalar whiteColor;
    // Color Black
    Scalar blackColor;
    // position of "select" hand sign
    Point selectPoint;
    //
    void getFeature1of3VerticalAreas(Mat,int&,struct svm_node[],double);
    void getFeature1of3HorizontalAreas(Mat,int&,struct svm_node[],double);
    void getFeatures1of2VerticalAndHorizontalAreas(Mat,int&,struct svm_node[],double);
    void getFeature4CornerAreas(Mat,int&,struct svm_node[],double);
    void getFeatures4x4(Mat,int&,struct svm_node[],double);
    void getFeatures3x3(Mat,int&,struct svm_node[],double);
signals:
    void sendSignalToChangeLabelTestingResult(QString);
    void sendSignalChangingBackGroundColor();
public slots:
    void changeToTestingMode();
    void receiveBinaryImage(Mat,Mat);
    bool getTestingResult();
};

#endif // CROPPINGIMAGE_H
