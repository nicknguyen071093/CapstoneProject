#ifndef CROPPINGIMAGE_H
#define CROPPINGIMAGE_H

#include <QThread>
#include <QMutex>
#include <HandGesture.h>

using namespace cv;

class CroppingImage : public QThread
{
    Q_OBJECT
public:
    explicit CroppingImage(QObject *parent = 0);
    bool STOP;
    void setToDefaults();

private:
    void run();
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
    // check whether we can crop image
    bool enableToCrop;
    // Color White
    Scalar whiteColor;
    // Color Black
    Scalar blackColor;
    QString getFeature1of3VerticalAreas(Mat,int&,double);
    QString getFeature1of3HorizontalAreas(Mat,int&,double);
    QString getFeatures1of2VerticalAndHorizontalAreas(Mat,int&,double);
    QString getFeature4CornerAreas(Mat,int&,double);
    QString getFeatures4x4(Mat,int&,double);
    QString getFeatures3x3(Mat,int&,double);
signals:
    void sendingCroppedImage(Mat,Mat,Mat,Mat,QString);
public slots:
    void receiveBinaryImage(Mat,Mat);
};

#endif // CROPPINGIMAGE_H
