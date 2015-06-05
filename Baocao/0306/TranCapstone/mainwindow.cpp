#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "HandGesture.h"
#include "pthread.h"
#include "opencv2/opencv.hpp"
#include "unistd.h"

int count = 0;
VideoCapture webSource;
Mat frame;
vector<uchar> mem;
String File_path = "";
// Stores all the information about the hand
HandGesture hg;
int squareLen;
const int SAMPLE_NUM = 7;
Scalar mColorsRGB[] = {Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255)};
double cLower[SAMPLE_NUM][3];
double cUpper[SAMPLE_NUM][3];
double cBackLower[SAMPLE_NUM][3];
double cBackUpper[SAMPLE_NUM][3];
Scalar lowerBound(0, 0, 0);
Scalar upperBound(0, 0, 0);
double avgColor[SAMPLE_NUM][3];
double avgBackColor[SAMPLE_NUM][3];
Point samplePoints[SAMPLE_NUM][2];
Mat sampleMats[SAMPLE_NUM];

Mat interMat ;
Mat rgbMat ;
Mat binMat ;
Mat tmpMat ;
Mat binTmpMat;
Mat binTmpMat2 ;
// Mode that presamples hand colors
static int SAMPLE_MODE = 0;
// Mode that generates binary image
static  int DETECTION_MODE = 1;
// Mode that displays color image together with contours, fingertips,
// defect points and so on.
static  int TRAIN_REC_MODE = 2;
// Mode that presamples background colors
static  int BACKGROUND_MODE = 3;
// Mode that is started when user clicks the 'Add Gesture' button.
static  int ADD_MODE = 4;
// Mode that is started when user clicks the 'Test' button.
static  int TEST_MODE = 5;
// Mode that is started when user clicks 'App Test' in the menu.
static  int APP_TEST_MODE = 6;
// Mode that is started when user clicks 'Data Collection' in the menu.
static  int DATA_COLLECTION_MODE = 0;
// Mode that is started when user clicks 'Map Apps' in the menu.
static  int MAP_APPS_MODE = 1;
// Number of frames used for prediction
static  int FRAME_BUFFER_NUM = 1;
// Frame interval between two launching events
static  int APP_TEST_DELAY_NUM = 10;
// Initial mode is BACKGROUND_MODE to presample the colors of the hand
int mode = BACKGROUND_MODE;
int curLabel = 0;

volatile bool runnable = false;
void initCLowerUpper(double , double , double , double ,
                     double , double );
void initCBackLowerUpper(double , double , double , double ,
                         double , double );
Mat preSampleHand(Mat);
Mat preSampleBack(Mat);
void boundariesCorrection();
void produceBinHandImg(Mat , Mat );
void produceBinBackImg(Mat , Mat );
Rect makeBoundingBox(Mat );
void adjustBoundingBox(Rect , Mat );
Mat makeContours();
Mat produceBinImg(Mat , Mat );

inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

        return image;
    }

        // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

        return image.rgbSwapped();
    }

        // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }

        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

        image.setColorTable( sColorTable );

        return image;
    }

    default:
        //qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}

void *showCamera(void *t);


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    webSource = VideoCapture(0);
    webSource.set(CV_CAP_PROP_FRAME_WIDTH,320);
    webSource.set(CV_CAP_PROP_FRAME_WIDTH,240);
    initCLowerUpper(50, 50, 10, 10, 10, 10);
    initCBackLowerUpper(50, 50, 3, 3, 3, 3);
    pthread_t threadCamera;
    pthread_attr_t attr;
    void* status;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    runnable = true;
    int rc = pthread_create(&threadCamera, NULL, showCamera, (void*) (ui->label));
    // pthread_detach(threadCamera);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    runnable = false;
}

void MainWindow::on_pushButton_2_clicked()
{
    mode = SAMPLE_MODE;
}


void MainWindow::on_pushButton_3_clicked()
{
    mode = DETECTION_MODE;
}

void MainWindow::on_pushButton_4_clicked()
{
    mode = TRAIN_REC_MODE;
}


void *showCamera(void *t) {
   // Ui::MainWindow *ui = (Ui::MainWindow *)t;
    QLabel *label = (QLabel *)t;
    QImage imageObject;
    QPixmap pixmapObject;
    while (runnable) {
        if (webSource.grab()) {

            webSource.retrieve(frame);
            flip(frame,frame,1);
            //                            frame = Highgui.imread("E:/CapstoneProject/Test/handBGR.png");
            // Mat mainMat = new Mat();
            //  Imgproc.cvtColor(frame, gray, Imgproc.COLOR_RGBA2RGB);
            // Core.flip(frame, frame, 1);

            GaussianBlur(frame, frame, Size(5, 5), 5, 5);
            //imwrite("E:/CapstoneProject/Test/frameHand.png", frame);
            //rgbMat = frame.clone();
            //                            Imgproc.cvtColor(mainMat, rgbMat, Imgproc.COLOR_RGBA2RGB);
            cvtColor(frame, interMat, COLOR_BGR2Lab);
            Mat showMat ;
            if (mode == SAMPLE_MODE) { // Second mode which presamples the colors of
                // the hand

                showMat = preSampleHand(frame).clone();

            } else if (mode == BACKGROUND_MODE) {// First mode which presamples
                // background colors
                //                                frame = Highgui.imread("E:/CapstoneProject/Test/backgroundBGR.png");
                //                                Imgproc.cvtColor(frame, interMat, Imgproc.COLOR_BGR2Lab);
                showMat = preSampleBack(frame).clone();
            } else if (mode == DETECTION_MODE) { // Third mode which generates the
                // binary image containing the
                // segmented hand represented by
                // white color

                showMat = produceBinImg(interMat, binMat).clone();

            } else if (mode == TRAIN_REC_MODE) {
                produceBinImg(interMat, binMat);
                showMat = makeContours();

                //String entry = hg.featureExtraction(frame, curLabel);

            }


            imageObject = cvMatToQImage(showMat);
            if (!imageObject.isNull()) {
                pixmapObject = QPixmap::fromImage(imageObject);
                if (!pixmapObject.isNull()) {
                    label->setPixmap(pixmapObject.scaled(320,240,Qt::KeepAspectRatio));
                }
            }

            // imshow("Camera",showMat);
            // imwrite("E:/CapstoneProject/Test/show.png", showMat);

            //if(cv::waitKey(30) == char('q')) break;
            // imencode(".bmp", showMat, mem);

        }
    }
    runnable = false;
    pthread_exit(NULL);
}

void initCLowerUpper(double cl1, double cu1, double cl2, double cu2,
                     double cl3, double cu3) {
    cLower[0][0] = cl1;
    cUpper[0][0] = cu1;
    cLower[0][1] = cl2;
    cUpper[0][1] = cu2;
    cLower[0][2] = cl3;
    cUpper[0][2] = cu3;
}

void initCBackLowerUpper(double cl1, double cu1, double cl2, double cu2,
                         double cl3, double cu3) {
    cBackLower[0][0] = cl1;
    cBackUpper[0][0] = cu1;
    cBackLower[0][1] = cl2;
    cBackUpper[0][1] = cu2;
    cBackLower[0][2] = cl3;
    cBackUpper[0][2] = cu3;
}

// Presampling hand colors.
// Output is avgColor, which is essentially a 7 by 3 matrix storing the
// colors sampled by seven squares
Mat preSampleHand(Mat img) {
    int cols = img.cols;
    int rows = img.rows;
    squareLen = rows / 20;
    Scalar color = mColorsRGB[1]; // Green Outline

    samplePoints[0][0].x = cols / 2;
    samplePoints[0][0].y = rows / 4;
    samplePoints[1][0].x = cols * 5 / 12;
    samplePoints[1][0].y = rows * 5 / 12;
    samplePoints[2][0].x = cols * 7 / 12;
    samplePoints[2][0].y = rows * 5 / 12;
    samplePoints[3][0].x = cols / 2;
    samplePoints[3][0].y = rows * 7 / 12;
    samplePoints[4][0].x = cols / 1.5;
    samplePoints[4][0].y = rows * 7 / 12;
    samplePoints[5][0].x = cols * 4 / 9;
    samplePoints[5][0].y = rows * 3 / 4;
    samplePoints[6][0].x = cols * 5 / 9;
    samplePoints[6][0].y = rows * 3 / 4;

    for (int i = 0; i < SAMPLE_NUM; i++) {
        samplePoints[i][1].x = samplePoints[i][0].x + squareLen;
        samplePoints[i][1].y = samplePoints[i][0].y + squareLen;
    }

    for (int i = 0; i < SAMPLE_NUM; i++) {
        rectangle(img, samplePoints[i][0], samplePoints[i][1], color,
                1);
    }
    for (int i = 0; i < SAMPLE_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            Vec3b intensity = interMat.at<Vec3b>((int) (samplePoints[i][0].y + squareLen / 2),(int) (samplePoints[i][0].x + squareLen / 2));
            avgColor[i][j] = intensity.val[j];
            //            avgColor[i][j] = (interMat.get(
            //                                  (int) (samplePoints[i][0].y + squareLen / 2),
            //                              (int) (samplePoints[i][0].x + squareLen / 2)))[j];

        }

    }
    return img;
}

// Presampling background colors.
// Output is avgBackColor, which is essentially a 7 by 3 matrix storing the
// colors sampled by seven squares
Mat preSampleBack(Mat img) {
    int cols = img.cols;
    int rows = img.rows;
    squareLen = rows / 20;
    Scalar color = mColorsRGB[0]; // Blue Outline

    samplePoints[0][0].x = cols / 6;
    samplePoints[0][0].y = rows / 3;
    samplePoints[1][0].x = cols / 6;
    samplePoints[1][0].y = rows * 2 / 3;
    samplePoints[2][0].x = cols / 2;
    samplePoints[2][0].y = rows / 6;
    samplePoints[3][0].x = cols / 2;
    samplePoints[3][0].y = rows / 2;
    samplePoints[4][0].x = cols / 2;
    samplePoints[4][0].y = rows * 5 / 6;
    samplePoints[5][0].x = cols * 5 / 6;
    samplePoints[5][0].y = rows / 3;
    samplePoints[6][0].x = cols * 5 / 6;
    samplePoints[6][0].y = rows * 2 / 3;

    for (int i = 0; i < SAMPLE_NUM; i++) {
        samplePoints[i][1].x = samplePoints[i][0].x + squareLen;
        samplePoints[i][1].y = samplePoints[i][0].y + squareLen;
    }
    imwrite("E:/CapstoneProject/Test/frameBack.png", frame);
    for (int i = 0; i < SAMPLE_NUM; i++) {
        rectangle(img, samplePoints[i][0], samplePoints[i][1], color,
                1);
    }
    for (int i = 0; i < SAMPLE_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            Vec3b intensity = interMat.at<Vec3b>((int) (samplePoints[i][0].y + squareLen / 2),(int) (samplePoints[i][0].x + squareLen / 2));
            avgColor[i][j] = intensity.val[j];
            //            avgBackColor[i][j] = (interMat.get(
            //                                      (int) (samplePoints[i][0].y + squareLen / 2),
            //                                  (int) (samplePoints[i][0].x + squareLen / 2)))[j];

        }
    }

    return img;
}

void boundariesCorrection() {
    for (int i = 1; i < SAMPLE_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            cLower[i][j] = cLower[0][j];
            cUpper[i][j] = cUpper[0][j];

            cBackLower[i][j] = cBackLower[0][j];
            cBackUpper[i][j] = cBackUpper[0][j];
        }
    }

    for (int i = 0; i < SAMPLE_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            if (avgColor[i][j] - cLower[i][j] < 0) {
                cLower[i][j] = avgColor[i][j];
            }

            if (avgColor[i][j] + cUpper[i][j] > 255) {
                cUpper[i][j] = 255 - avgColor[i][j];
            }

            if (avgBackColor[i][j] - cBackLower[i][j] < 0) {
                cBackLower[i][j] = avgBackColor[i][j];
            }

            if (avgBackColor[i][j] + cBackUpper[i][j] > 255) {
                cBackUpper[i][j] = 255 - avgBackColor[i][j];
            }
        }
    }
}

// Generates binary image thresholded only by sampled hand colors
void produceBinHandImg(Mat imgIn, Mat imgOut) {
    for (int i = 0; i < SAMPLE_NUM; i++) {
        lowerBound.val[0] = avgColor[i][0] - cLower[i][0];
        lowerBound.val[1] = avgColor[i][1] - cLower[i][1];
        lowerBound.val[1] = avgColor[i][2] - cLower[i][2];
        //                lowerBound.set(double[3] {avgColor[i][0] - cLower[i][0],
        //                    avgColor[i][1] - cLower[i][1],
        //                    avgColor[i][2] - cLower[i][2]});
        //                upperBound(avgColor[i][0] + cUpper[i][0],
        //                avgColor[i][1] + cUpper[i][1],
        //                avgColor[i][2] + cUpper[i][2]);
        upperBound.val[0] = avgColor[i][0] + cUpper[i][0];
        upperBound.val[1] = avgColor[i][1] + cUpper[i][1];
        upperBound.val[1] = avgColor[i][2] + cUpper[i][2];
        inRange(imgIn, lowerBound, upperBound, sampleMats[i]);

    }

    imgOut.release();
    sampleMats[0].copyTo(imgOut);

    for (int i = 1; i < SAMPLE_NUM; i++) {
        add(imgOut, sampleMats[i], imgOut);
    }

    medianBlur(imgOut, imgOut, 3);
}

// Generates binary image thresholded only by sampled background colors
void produceBinBackImg(Mat imgIn, Mat imgOut) {
    for (int i = 0; i < SAMPLE_NUM; i++) {
        lowerBound.val[0] = avgBackColor[i][0] - cBackLower[i][0];
        lowerBound.val[1] = avgBackColor[i][1] - cBackLower[i][1];
        lowerBound.val[2] = avgBackColor[i][2] - cBackLower[i][2];
        //        lowerBound.set(new double[]{
        //            avgBackColor[i][0] - cBackLower[i][0],
        //                    avgBackColor[i][1] - cBackLower[i][1],
        //                    avgBackColor[i][2] - cBackLower[i][2]});
        upperBound.val[0] = avgBackColor[i][0] + cBackUpper[i][0];
        upperBound.val[1] = avgBackColor[i][1] + cBackUpper[i][1];
        upperBound.val[2] = avgBackColor[i][2] + cBackUpper[i][2];
        //        upperBound.set(new double[]{
        //            avgBackColor[i][0] + cBackUpper[i][0],
        //                    avgBackColor[i][1] + cBackUpper[i][1],
        //                    avgBackColor[i][2] + cBackUpper[i][2]});
        inRange(imgIn, lowerBound, upperBound, sampleMats[i]);

    }

    imgOut.release();
    sampleMats[0].copyTo(imgOut);

    for (int i = 1; i < SAMPLE_NUM; i++) {
        add(imgOut, sampleMats[i], imgOut);
    }
    bitwise_not(imgOut, imgOut);

    medianBlur(imgOut, imgOut, 7);

}

Rect makeBoundingBox(Mat img) {
    hg.contours.clear();
    findContours(img, hg.contours, hg.hie, RETR_EXTERNAL,
                 CHAIN_APPROX_NONE);
    hg.findBiggestContour();
    if (hg.cMaxId > -1) {
        hg.boundingRect = boundingRect(hg.contours[hg.cMaxId]);
    }
    if (hg.detectIsHand(frame)) {
        return hg.boundingRect;
    } else {
        return Rect();
    }

}

void adjustBoundingBox(Rect initRect, Mat img) {
}

bool isClosedToBoundary(Point pt, Mat img) {
    int margin = 5;
    if ((pt.x > margin) && (pt.y > margin) && (pt.x < img.cols- margin)
            && (pt.y < img.rows - margin)) {
        return false;
    }

    return true;
}

Mat makeContours() {
    hg.contours.clear();
    findContours(binMat, hg.contours, hg.hie,
                 RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // Find biggest contour and return the index of the contour, which is
    // hg.cMaxId
    hg.findBiggestContour();

    if (hg.cMaxId > -1) {
        // vector<Point> biggestContours = hg.contours[hg.cMaxId];
        //biggestContours.
        Mat(hg.contours[hg.cMaxId]).copyTo(hg.approxContour);
        //        hg.approxContour.fromList(hg.contours.get(hg.cMaxId).toList());

        //  Highgui.imwrite("E:\\CapstoneProject\\Test\\contour.png", hg.approxContour);
        approxPolyDP(hg.approxContour, hg.approxContour, 2, true);
        Mat(hg.approxContour).copyTo(hg.contours[hg.cMaxId]);
        //  hg.contours.get(hg.cMaxId).fromList(hg.approxContour.toList());

        // hg.contours.get(hg.cMaxId) represents the contour of the hand
        drawContours(frame, hg.contours, hg.cMaxId,
                     mColorsRGB[0], 1);

        // Palm center is stored in hg.inCircle, radius of the inscribed
        // circle is stored in hg.inCircleRadius
        hg.findInscribedCircle(frame);

        hg.boundingRect = boundingRect(hg.contours[hg.cMaxId]);

        convexHull(hg.contours[hg.cMaxId], hg.hullI, false);

        hg.hullP.clear();
        for (int i = 0; i < hg.contours.size(); i++) {
            //            hg.hullP.add(vector<Point>());
            hg.hullP.push_back(vector<Point>());
        }

        //        int cId[] = hg.hullI.toArray();
        //        List<Point> lp = new ArrayList<Point>();
        //        Point contourPts[] = hg.contours.get(hg.cMaxId).toArray();

        //        for (int i = 0; i < hg.hullI.length; i++) {
        //            lp.add(contourPts[cId[i]]);
        //            // Core.circle(rgbaMat, contourPts[cId[i]], 2, new Scalar(241,
        //            // 247, 45), -3);
        //        }
        vector<Point> lp;
        for (int i = 0; i < hg.hullI.size(); i++) {
            lp.push_back(hg.contours[hg.cMaxId][hg.hullI[i]]);
            // Core.circle(rgbaMat, contourPts[cId[i]], 2, new Scalar(241,
            // 247, 45), -3);
        }

        // hg.hullP.get(hg.cMaxId) returns the locations of the points in
        // the convex hull of the hand
        // hg.hullP.get(hg.cMaxId).fromList(lp);
        lp.clear();

        hg.fingerTips.clear();
        hg.defectPoints.clear();
        hg.defectPointsOrdered.clear();

        hg.fingerTipsOrdered.clear();
        hg.defectIdAfter.clear();

        //        if ((contourPts.length >= 5) && hg.detectIsHand(frame)
        //                && (cId.length >= 5))
        if ((hg.contours[hg.cMaxId].size() >= 5) &&
                (hg.detectIsHand(frame))
                && (hg.hullI.size() >= 5))
        {
            //            convexityDefects(hg.contours.get(hg.cMaxId), hg.hullI,
            //                                     hg.defects);
            convexityDefects(hg.contours[hg.cMaxId], hg.hullI,
                    hg.defects);
            // List<Integer> dList = hg.defeonvexcts.toList();

            // Point prevPoint = null;
            // int number = 0;
            // System.out.println("-----Vao-----");
            // for (int i = 0; i < dList.size(); i++) {
            for (int i = 0; i < hg.defects.size(); i++) {
                // int id = i % 4;
                Vec4i defect = hg.defects[i];
                //if (id == 2) { // Defect point
                double depth = (double) defect[3] / 256.0;
                // Point curPoint = contourPts[dList.get(i)];
                Point curPoint = hg.contours[hg.cMaxId][defect[2]];
                Point curPoint0 = hg.contours[hg.cMaxId][defect[0]];
                Point curPoint1 = hg.contours[hg.cMaxId][defect[1]];
                //                Point curPoint0 = contourPts[dList.get(i - 2)];
                //                Point curPoint1 = contourPts[dList.get(i - 1)];
                //                Point vec0 = new Point(curPoint0.x - curPoint.x,
                //                                       curPoint0.y - curPoint.y);
                //                Point vec1 = new Point(curPoint1.x - curPoint.x,
                //                                       curPoint1.y - curPoint.y);
                Point vec0(curPoint0.x - curPoint.x,curPoint0.y - curPoint.y);
                Point vec1(curPoint1.x - curPoint.x,curPoint1.y - curPoint.y);
                double dot = vec0.x * vec1.x + vec0.y * vec1.y;
                double lenth0 = sqrt(vec0.x * vec0.x + vec0.y
                                     * vec0.y);
                double lenth1 = sqrt(vec1.x * vec1.x + vec1.y
                                     * vec1.y);
                double cosTheta = dot / (lenth0 * lenth1);
                bool bool1 = isClosedToBoundary(curPoint0, frame);
                bool bool2 = isClosedToBoundary(curPoint1, frame);
                if ((depth > hg.inCircleRadius * 0.7)
                        && (cosTheta >= -0.7) && !bool1 && !bool2) {

                    //number++;
                    //                            System.out.println("number: " + number);
                    //                            System.out.println("---Oke----");
                    //                            System.out.println("Current Point: " + curPoint.x + " " + curPoint.y);
                    //                            System.out.println("dept: " + depth);
                    //                            System.out.println("cosTheta: " + cosTheta);
                    //                            System.out.println("Point 0: " + isClosedToBoundary(curPoint0, frame));
                    //                            System.out.println("Point 0: " + isClosedToBoundary(curPoint1, frame));
                    //                            System.out.println("---Oke End----");


                } else {
                    if (((!bool1 && bool2) || (bool1 && !bool2)) && (depth > (hg.inCircleRadius * 0.2)) && (cosTheta <= 0)) {
                        //                        System.out.println("Current Point: " + curPoint.x + " " + curPoint.y);
                        //                        System.out.println("Radius: " + hg.inCircleRadius);
                        //                        System.out.println("dept: " + depth);
                        //                        System.out.println("cosTheta: " + cosTheta);
                        //                        System.out.println("Point 0: " + isClosedToBoundary(curPoint0, frame) + " " + curPoint0.x + " " + curPoint0.y);
                        //                        System.out.println("Point 1: " + isClosedToBoundary(curPoint1, frame) + " " + curPoint1.x + " " + curPoint1.y);
                        // hg.defectIdAfter.add((i));
                        hg.defectIdAfter.push_back(i);
                        Point finVec0(curPoint0.x - hg.inCircle.x, curPoint0.y - hg.inCircle.y);
                        double finAngle0 = atan2(finVec0.y, finVec0.x);
                        Point finVec1 (curPoint1.x - hg.inCircle.x, curPoint1.y - hg.inCircle.y);
                        double finAngle1 = atan2(finVec1.y, finVec1.x);

                        if (hg.fingerTipsOrdered.size() == 0) {
                            hg.fingerTipsOrdered.insert(pair<double,Point>(finAngle0, curPoint0));
                            hg.fingerTipsOrdered.insert(pair<double,Point>(finAngle1, curPoint1));
                            //  hg.fingerTipsOrdered.put(finAngle1, curPoint1);

                        } else {
                            hg.fingerTipsOrdered.insert(pair<double,Point>(finAngle0, curPoint0));
                            hg.fingerTipsOrdered.insert(pair<double,Point>(finAngle1, curPoint1));
                            //                            hg.fingerTipsOrdered.put(finAngle0, curPoint0);

                            //                            hg.fingerTipsOrdered.put(finAngle1, curPoint1);

                        }
                    }
                }

                // }
            }
            //  System.out.println("-----Ketthuc-----");

        }

    }

    if (hg.detectIsHand(frame)) {

        // hg.boundingRect represents four coordinates of the bounding box.
        rectangle(frame, hg.boundingRect.tl(), hg.boundingRect.br(),
                  mColorsRGB[1], 2);
        drawContours(frame, hg.hullP, hg.cMaxId, mColorsRGB[2]);
    }
    return frame;
}

// Generates binary image containing user's hand
Mat produceBinImg(Mat imgIn, Mat imgOut) {
    int colNum = imgIn.cols;
    int rowNum = imgIn.rows;
    int boxExtension = 0;

    boundariesCorrection();

    produceBinHandImg(imgIn, binTmpMat);

    produceBinBackImg(imgIn, binTmpMat2);

    bitwise_and(binTmpMat, binTmpMat2, binTmpMat);
    binTmpMat.copyTo(tmpMat);
    binTmpMat.copyTo(imgOut);

    Rect roiRect = makeBoundingBox(tmpMat);

    adjustBoundingBox(roiRect, binTmpMat);

    if (roiRect.area() > 0) {

        roiRect.x = max(0, roiRect.x - boxExtension);
        roiRect.y = max(0, roiRect.y - boxExtension);
        roiRect.width = min(roiRect.width + boxExtension, colNum);
        roiRect.height = min(roiRect.height + boxExtension, rowNum);

        Mat roi1(binTmpMat, roiRect);
        Mat roi3(imgOut, roiRect);
        // imgOut.setTo(Scalar.all(0));
        imgOut.setTo(Scalar::all(0));

        roi1.copyTo(roi3);

        Mat element = getStructuringElement(MORPH_ELLIPSE,
                                            Size(3, 3));
        dilate(roi3, roi3, element, Point(-1, -1), 2);

        erode(roi3, roi3, element, Point(-1, -1), 2);

    }

    // cropBinImg(imgOut, imgOut);
    return imgOut;
}

void MainWindow::on_MainWindow_destroyed()
{
    webSource.release();
}
