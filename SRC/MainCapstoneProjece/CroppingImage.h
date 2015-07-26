#ifndef CROPPINGIMAGE_H
#define CROPPINGIMAGE_H

#include <QThread>
#include <QMutex>
#include <HandGesture.h>
#include <SignRecognition.h>
#include <iostream>

#define NUMBER_FEATURES 75
#define SELECTED_COLOR "white"
#define NON_SELECTED_COLOR "rgb(179, 0, 0)"

using namespace cv;
using namespace std;

class CroppingImage : public QThread
{
    Q_OBJECT
public:
    explicit CroppingImage(QObject *parent = 0);
    ~CroppingImage();
    bool STOP;
    const static int NO_MODE = 0;
    const static int TESTING_MODE = 1;
    const static int SELECTING_MODE = 2;
    const static int RECOGNITION_MODE = 3;
    const static int LEARNING_MODE = 4;

    const static int NO_FUNCTION = 0;
    const static int RECOGNITION_FUCNTION = 1;
    const static int LEARNING_FUNCTION = 2;
    const static int UP_FUCNTION = 3;
    const static int DOWN_FUCNTION = 3;

    QString getRegResult();

    void changeToRecognitionMode();
    void changeToSelectingFunctionMode();

private:
    void run();
    int mode;
    int selectedFunctionNumber;
    QMutex mutex;
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
    double recognitionResultNumber;


    void getFeature1of3VerticalAreas(Mat,int&,struct svm_node[],double);
    void getFeature1of3HorizontalAreas(Mat,int&,struct svm_node[],double);
    void getFeatures1of2VerticalAndHorizontalAreas(Mat,int&,struct svm_node[],double);
    void getFeature4CornerAreas(Mat,int&,struct svm_node[],double);
    void getFeatures4x4(Mat,int&,struct svm_node[],double);
    void getFeatures3x3(Mat,int&,struct svm_node[],double);

    void recognizeSign();

signals:
    void sendSignalToChangeLabelTestingResult(QString);

    void sendSignalSelectingRecognition();
    void sendSignalSelectingLearning();

    void sendSignalChangingRecognitionResult(QString);

    void sendSignalChangingRecognitionColor(QString);
    void sendSignalChangingLearningColor(QString);
public slots:
    void changeToTestingMode();
    void receiveBinaryImage(Mat,Mat);
    bool getTestingResult();
};

#endif // CROPPINGIMAGE_H
