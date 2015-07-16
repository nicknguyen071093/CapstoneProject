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

private:
    void run();
    // Color White
    Scalar white;
    // Color Black
    Scalar black;
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

signals:
    void sendingCroppedImage(Mat,Mat,Mat,Mat,QString);
public slots:
    void receiveBinaryImage(Mat,Mat);
};

#endif // CROPPINGIMAGE_H
