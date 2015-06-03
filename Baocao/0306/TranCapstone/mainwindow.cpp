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
void *showCamera(void *t){
    while (runnable) {
        if (webSource.grab()) {

            webSource.retrieve(frame);
            //                            frame = Highgui.imread("E:/CapstoneProject/Test/handBGR.png");
            // Mat mainMat = new Mat();
            //  Imgproc.cvtColor(frame, gray, Imgproc.COLOR_RGBA2RGB);
            // Core.flip(frame, frame, 1);

            GaussianBlur(frame, frame, Size(5, 5), 5, 5);
            imwrite("E:/CapstoneProject/Test/frameHand.png", frame);
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

                String entry = hg.featureExtraction(frame, curLabel);

            }
            imwrite("E:/CapstoneProject/Test/show.png", showMat);
            imencode(".bmp", showMat, mem);


        }
    }


}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    initCLowerUpper(50, 50, 10, 10, 10, 10);
    initCBackLowerUpper(50, 50, 3, 3, 3, 3);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_pushButton_2_clicked()
{

}


void MainWindow::on_pushButton_3_clicked()
{

}

void MainWindow::on_pushButton_4_clicked()
{

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
interMat.at<(int)(samplePoints[i][0].y + squareLen / 2),(int) (samplePoints[i][0].x + squareLen / 2)>;
            avgColor[i][j] = (interMat.get(
                                  (int) (samplePoints[i][0].y + squareLen / 2),
                              (int) (samplePoints[i][0].x + squareLen / 2)))[j];

        }

    }
    return img;
}

// Presampling background colors.
// Output is avgBackColor, which is essentially a 7 by 3 matrix storing the
// colors sampled by seven squares
Mat preSampleBack(Mat img) {
    int cols = img.cols();
    int rows = img.rows();
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
    Highgui.imwrite("E:/CapstoneProject/Test/frameBack.png", frame);
    for (int i = 0; i < SAMPLE_NUM; i++) {
        Core.rectangle(img, samplePoints[i][0], samplePoints[i][1], color,
                1);
    }
    for (int i = 0; i < SAMPLE_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            avgBackColor[i][j] = (interMat.get(
                                      (int) (samplePoints[i][0].y + squareLen / 2),
                                  (int) (samplePoints[i][0].x + squareLen / 2)))[j];

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
        lowerBound.set(new double[]{avgColor[i][0] - cLower[i][0],
                    avgColor[i][1] - cLower[i][1],
                    avgColor[i][2] - cLower[i][2]});
        upperBound.set(new double[]{avgColor[i][0] + cUpper[i][0],
                    avgColor[i][1] + cUpper[i][1],
                    avgColor[i][2] + cUpper[i][2]});

        Core.inRange(imgIn, lowerBound, upperBound, sampleMats[i]);

    }

    imgOut.release();
    sampleMats[0].copyTo(imgOut);

    for (int i = 1; i < SAMPLE_NUM; i++) {
        Core.add(imgOut, sampleMats[i], imgOut);
    }

    Imgproc.medianBlur(imgOut, imgOut, 3);
}

// Generates binary image thresholded only by sampled background colors
void produceBinBackImg(Mat imgIn, Mat imgOut) {
    for (int i = 0; i < SAMPLE_NUM; i++) {

        lowerBound.set(new double[]{
            avgBackColor[i][0] - cBackLower[i][0],
                    avgBackColor[i][1] - cBackLower[i][1],
                    avgBackColor[i][2] - cBackLower[i][2]});

        upperBound.set(new double[]{
            avgBackColor[i][0] + cBackUpper[i][0],
                    avgBackColor[i][1] + cBackUpper[i][1],
                    avgBackColor[i][2] + cBackUpper[i][2]});
        Core.inRange(imgIn, lowerBound, upperBound, sampleMats[i]);

    }

    imgOut.release();
    sampleMats[0].copyTo(imgOut);

    for (int i = 1; i < SAMPLE_NUM; i++) {
        Core.add(imgOut, sampleMats[i], imgOut);
    }

    Core.bitwise_not(imgOut, imgOut);

    Imgproc.medianBlur(imgOut, imgOut, 7);

}

Rect makeBoundingBox(Mat img) {
    hg.contours.clear();
    Imgproc.findContours(img, hg.contours, hg.hie, Imgproc.RETR_EXTERNAL,
                         Imgproc.CHAIN_APPROX_NONE);
    hg.findBiggestContour();

    if (hg.cMaxId > -1) {

        hg.boundingRect = Imgproc.boundingRect(hg.contours.get(hg.cMaxId));

    }

    if (hg.detectIsHand(frame)) {

        return hg.boundingRect;
    } else {
        return null;
    }
}

void adjustBoundingBox(Rect initRect, Mat img) {
}

boolean isClosedToBoundary(Point pt, Mat img) {
    int margin = 5;
    if ((pt.x > margin) && (pt.y > margin) && (pt.x < img.cols() - margin)
            && (pt.y < img.rows() - margin)) {
        return false;
    }

    return true;
}

Mat makeContours() {
    hg.contours.clear();
    Imgproc.findContours(binMat, hg.contours, hg.hie,
                         Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_NONE);

    // Find biggest contour and return the index of the contour, which is
    // hg.cMaxId
    hg.findBiggestContour();

    if (hg.cMaxId > -1) {

        hg.approxContour.fromList(hg.contours.get(hg.cMaxId).toList());
        //  Highgui.imwrite("E:\\CapstoneProject\\Test\\contour.png", hg.approxContour);
        Imgproc.approxPolyDP(hg.approxContour, hg.approxContour, 2, true);
        hg.contours.get(hg.cMaxId).fromList(hg.approxContour.toList());

        // hg.contours.get(hg.cMaxId) represents the contour of the hand
        Imgproc.drawContours(frame, hg.contours, hg.cMaxId,
                             mColorsRGB[0], 1);

        // Palm center is stored in hg.inCircle, radius of the inscribed
        // circle is stored in hg.inCircleRadius
        hg.findInscribedCircle(frame);

        hg.boundingRect = Imgproc.boundingRect(hg.contours.get(hg.cMaxId));

        Imgproc.convexHull(hg.contours.get(hg.cMaxId), hg.hullI, false);

        hg.hullP.clear();
        for (int i = 0; i < hg.contours.size(); i++) {
            hg.hullP.add(new MatOfPoint());
        }

        int[] cId = hg.hullI.toArray();
        List<Point> lp = new ArrayList<Point>();
        Point contourPts[] = hg.contours.get(hg.cMaxId).toArray();

        for (int i = 0; i < cId.length; i++) {
            lp.add(contourPts[cId[i]]);
            // Core.circle(rgbaMat, contourPts[cId[i]], 2, new Scalar(241,
            // 247, 45), -3);
        }

        // hg.hullP.get(hg.cMaxId) returns the locations of the points in
        // the convex hull of the hand
        hg.hullP.get(hg.cMaxId).fromList(lp);
        lp.clear();

        hg.fingerTips.clear();
        hg.defectPoints.clear();
        hg.defectPointsOrdered.clear();

        hg.fingerTipsOrdered.clear();
        hg.defectIdAfter.clear();

        if ((contourPts.length >= 5) && hg.detectIsHand(frame)
                && (cId.length >= 5)) {
            Imgproc.convexityDefects(hg.contours.get(hg.cMaxId), hg.hullI,
                                     hg.defects);
            List<Integer> dList = hg.defects.toList();

            Point prevPoint = null;
            int number = 0;
            System.out.println("-----Vao-----");
            for (int i = 0; i < dList.size(); i++) {
                int id = i % 4;
                Point curPoint;
                if (id == 2) { // Defect point
                    double depth = (double) dList.get(i + 1) / 256.0;
                    curPoint = contourPts[dList.get(i)];

                    Point curPoint0 = contourPts[dList.get(i - 2)];
                    Point curPoint1 = contourPts[dList.get(i - 1)];
                    Point vec0 = new Point(curPoint0.x - curPoint.x,
                                           curPoint0.y - curPoint.y);
                    Point vec1 = new Point(curPoint1.x - curPoint.x,
                                           curPoint1.y - curPoint.y);
                    double dot = vec0.x * vec1.x + vec0.y * vec1.y;
                    double lenth0 = Math.sqrt(vec0.x * vec0.x + vec0.y
                                              * vec0.y);
                    double lenth1 = Math.sqrt(vec1.x * vec1.x + vec1.y
                                              * vec1.y);
                    double cosTheta = dot / (lenth0 * lenth1);
                    boolean bool1 = isClosedToBoundary(curPoint0, frame);
                    boolean bool2 = isClosedToBoundary(curPoint1, frame);
                    if ((depth > hg.inCircleRadius * 0.7)
                            && (cosTheta >= -0.7) && !bool1 && !bool2) {

                        number++;
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
                            System.out.println("Current Point: " + curPoint.x + " " + curPoint.y);
                            System.out.println("Radius: " + hg.inCircleRadius);
                            System.out.println("dept: " + depth);
                            System.out.println("cosTheta: " + cosTheta);
                            System.out.println("Point 0: " + isClosedToBoundary(curPoint0, frame) + " " + curPoint0.x + " " + curPoint0.y);
                            System.out.println("Point 1: " + isClosedToBoundary(curPoint1, frame) + " " + curPoint1.x + " " + curPoint1.y);
                            hg.defectIdAfter.add((i));

                            Point finVec0 = new Point(curPoint0.x
                                                      - hg.inCircle.x, curPoint0.y
                                                      - hg.inCircle.y);
                            double finAngle0 = Math.atan2(finVec0.y, finVec0.x);
                            Point finVec1 = new Point(curPoint1.x
                                                      - hg.inCircle.x, curPoint1.y
                                                      - hg.inCircle.y);
                            double finAngle1 = Math.atan2(finVec1.y, finVec1.x);

                            if (hg.fingerTipsOrdered.size() == 0) {
                                hg.fingerTipsOrdered.put(finAngle0, curPoint0);
                                hg.fingerTipsOrdered.put(finAngle1, curPoint1);

                            } else {
                                hg.fingerTipsOrdered.put(finAngle0, curPoint0);

                                hg.fingerTipsOrdered.put(finAngle1, curPoint1);

                            }
                        }
                    }

                }
            }
            System.out.println("-----Ketthuc-----");

        }

    }

    if (hg.detectIsHand(frame)) {

        // hg.boundingRect represents four coordinates of the bounding box.
        Core.rectangle(frame, hg.boundingRect.tl(), hg.boundingRect.br(),
                       mColorsRGB[1], 2);
        Imgproc.drawContours(frame, hg.hullP, hg.cMaxId, mColorsRGB[2]);
    }
    return frame;
}

// Generates binary image containing user's hand
Mat produceBinImg(Mat imgIn, Mat imgOut) {
    int colNum = imgIn.cols();
    int rowNum = imgIn.rows();
    int boxExtension = 0;

    boundariesCorrection();

    produceBinHandImg(imgIn, binTmpMat);

    produceBinBackImg(imgIn, binTmpMat2);

    Core.bitwise_and(binTmpMat, binTmpMat2, binTmpMat);
    binTmpMat.copyTo(tmpMat);
    binTmpMat.copyTo(imgOut);

    Rect roiRect = makeBoundingBox(tmpMat);
    adjustBoundingBox(roiRect, binTmpMat);

    if (roiRect != null) {
        roiRect.x = Math.max(0, roiRect.x - boxExtension);
        roiRect.y = Math.max(0, roiRect.y - boxExtension);
        roiRect.width = Math.min(roiRect.width + boxExtension, colNum);
        roiRect.height = Math.min(roiRect.height + boxExtension, rowNum);

        Mat roi1 = new Mat(binTmpMat, roiRect);
        Mat roi3 = new Mat(imgOut, roiRect);
        imgOut.setTo(Scalar.all(0));

        roi1.copyTo(roi3);

        Mat element = Imgproc.getStructuringElement(Imgproc.MORPH_ELLIPSE,
                                                    new Size(3, 3));
        Imgproc.dilate(roi3, roi3, element, new Point(-1, -1), 2);

        Imgproc.erode(roi3, roi3, element, new Point(-1, -1), 2);

    }

    // cropBinImg(imgOut, imgOut);
    return imgOut;
}
