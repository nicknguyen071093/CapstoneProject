#include "HandThread.h"
#include <QtCore>

HandThread::HandThread(QObject *parent) :
    QThread(parent)
{
    handGesture = new HandGesture();
    webSource = VideoCapture(0);
    webSource.set(CV_CAP_PROP_FRAME_WIDTH,320);
    webSource.set(CV_CAP_PROP_FRAME_HEIGHT,240);
    initCLowerUpper(50, 50, 10, 10, 10, 10);
    initCBackLowerUpper(50, 50, 3, 3, 3, 3);
    this->STOP = false;
    this->mode = BACKGROUND_MODE;
}

void HandThread::setMode(int mode) {
    this->mode = mode;
}

void HandThread::run() {
    while(true) {
        QMutex mutex;
        mutex.lock();
        if(this->STOP) break;
        mutex.unlock();
        if (webSource.grab()) {
            webSource.retrieve(frame);
            flip(frame,frame,1);
            GaussianBlur(frame, blurMat, Size(5, 5), 5, 5);
            cvtColor(blurMat, interMat, COLOR_BGR2Lab);
            Mat showMat ;
            if (mode == SAMPLE_MODE) { // Second mode which presamples the colors of
                // the hand
                showMat = preSampleHand(frame).clone();
            } else if (mode == BACKGROUND_MODE) {// First mode which presamples
                // background colors
                showMat = preSampleBack(frame).clone();
            } else if (mode == DETECTION_MODE) { // Third mode which generates the
                // binary image containing the
                // segmented hand represented by
                // white color
                showMat = produceBinImg(interMat, binMat).clone();
            } else if (mode == TRAIN_REC_MODE) {
                produceBinImg(interMat, binMat);
                showMat = makeContours();

                //  handGesture->featureExtraction(frame, curLabel);

            }
            imageObject = cvMatToQImage(showMat);

            emit handTrackingChanged(imageObject);

        }
    }
}


void HandThread::releaseAll() {
    webSource.release();
}

inline QImage HandThread::cvMatToQImage( const cv::Mat &inMat )
{
    switch (inMat.type())
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32);

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
        if (sColorTable.isEmpty())
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

void HandThread::initCLowerUpper(double cl1, double cu1, double cl2, double cu2,
                                 double cl3, double cu3) {
    cLower[0][0] = cl1;
    cUpper[0][0] = cu1;
    cLower[0][1] = cl2;
    cUpper[0][1] = cu2;
    cLower[0][2] = cl3;
    cUpper[0][2] = cu3;
}

void HandThread::initCBackLowerUpper(double cl1, double cu1, double cl2, double cu2,
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
Mat HandThread::preSampleHand(Mat img) {
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
Mat HandThread::preSampleBack(Mat img) {
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
    //    imwrite("E:/CapstoneProject/Test/frameBack.png", frame);
    for (int i = 0; i < SAMPLE_NUM; i++) {
        rectangle(img, samplePoints[i][0], samplePoints[i][1], color,
                1);
    }
    for (int i = 0; i < SAMPLE_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            Vec3b intensity = interMat.at<Vec3b>((int) (samplePoints[i][0].y + squareLen / 2),(int) (samplePoints[i][0].x + squareLen / 2));
            avgBackColor[i][j] = intensity.val[j];
            //            avgBackColor[i][j] = (interMat.get(
            //                                      (int) (samplePoints[i][0].y + squareLen / 2),
            //                                  (int) (samplePoints[i][0].x + squareLen / 2)))[j];

        }
    }

    return img;
}

void HandThread::boundariesCorrection() {
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
void HandThread::produceBinHandImg(Mat &imgIn, Mat &imgOut) {
    for (int i = 0; i < SAMPLE_NUM; i++) {
        lowerBound.val[0] = avgColor[i][0] - cLower[i][0];
        lowerBound.val[1] = avgColor[i][1] - cLower[i][1];
        lowerBound.val[2] = avgColor[i][2] - cLower[i][2];
        //                lowerBound.set(double[3] {avgColor[i][0] - cLower[i][0],
        //                    avgColor[i][1] - cLower[i][1],
        //                    avgColor[i][2] - cLower[i][2]});
        //                upperBound(avgColor[i][0] + cUpper[i][0],
        //                avgColor[i][1] + cUpper[i][1],
        //                avgColor[i][2] + cUpper[i][2]);
        upperBound.val[0] = avgColor[i][0] + cUpper[i][0];
        upperBound.val[1] = avgColor[i][1] + cUpper[i][1];
        upperBound.val[2] = avgColor[i][2] + cUpper[i][2];
        // imwrite("/home/flyc/imgIn.png",imgIn);
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
void HandThread::produceBinBackImg(Mat &imgIn, Mat &imgOut) {
    imwrite("/home/nickseven/imgIn.png",imgIn);
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

Rect HandThread::makeBoundingBox(Mat &img) {
    handGesture->contours.clear();
    //cout<<img.type();
    //imwrite("/home/flyc/2.png",img);
    findContours(img, handGesture->contours, handGesture->hie, CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

    //  cout<<"alo 123";
    handGesture->findBiggestContour();
    if (handGesture->cMaxId > -1) {
        handGesture->boundingRect = boundingRect(handGesture->contours[handGesture->cMaxId]);
    }
    if (handGesture->detectIsHand(frame)) {
        return handGesture->boundingRect;
    } else {
        return Rect();
    }

}

bool HandThread::isClosedToBoundary(Point pt, Mat img) {
    int margin = 5;
    if ((pt.x > margin) && (pt.y > margin) && (pt.x < img.cols- margin)
            && (pt.y < img.rows - margin)) {
        return false;
    }

    return true;
}

Mat HandThread::makeContours() {
    handGesture->contours.clear();
    findContours(binMat, handGesture->contours, handGesture->hie,
                 RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // Find biggest contour and return the index of the contour, which is
    // handGesture->cMaxId
    handGesture->findBiggestContour();

    if (handGesture->cMaxId > -1) {
        //biggestContours.
        Mat(handGesture->contours[handGesture->cMaxId]).copyTo(handGesture->approxContour);
        approxPolyDP(handGesture->approxContour, handGesture->approxContour, 2, true);
        Mat(handGesture->approxContour).copyTo(handGesture->contours[handGesture->cMaxId]);
        drawContours(frame, handGesture->contours, handGesture->cMaxId,
                     mColorsRGB[0], 1);
        // Palm center is stored in handGesture->inCircle, radius of the inscribed
        // circle is stored in handGesture->inCircleRadius
//        handGesture->findInscribedCircle(frame);
        handGesture->boundingRect = boundingRect(handGesture->contours[handGesture->cMaxId]);
//        convexHull(handGesture->contours[handGesture->cMaxId], handGesture->hullI, false);
//        handGesture->hullP.clear();
//        for (int i = 0; i < handGesture->contours.size(); i++) {
//            handGesture->hullP.push_back(vector<Point>());
//        }
//        vector<Point> lp;
//        for (int i = 0; i < handGesture->hullI.size(); i++) {
//            lp.push_back(handGesture->contours[handGesture->cMaxId][handGesture->hullI[i]]);
//        }

//        // handGesture->hullP.get(handGesture->cMaxId) returns the locations of the points in
//        // the convex hull of the hand
//        lp.clear();

//        handGesture->fingerTips.clear();
//        handGesture->defectPoints.clear();
//        handGesture->defectPointsOrdered.clear();

//        handGesture->fingerTipsOrdered.clear();
//        handGesture->defectIdAfter.clear();

//        if ((handGesture->contours[handGesture->cMaxId].size() >= 5) &&
//                (handGesture->detectIsHand(frame))
//                && (handGesture->hullI.size() >= 5))
//        {
//            convexityDefects(handGesture->contours[handGesture->cMaxId], handGesture->hullI,
//                    handGesture->defects);
//            for (int i = 0; i < handGesture->defects.size(); i++) {
//                Vec4i defect = handGesture->defects[i];
//                double depth = (double) defect[3] / 256.0;
//                Point curPoint = handGesture->contours[handGesture->cMaxId][defect[2]];
//                Point curPoint0 = handGesture->contours[handGesture->cMaxId][defect[0]];
//                Point curPoint1 = handGesture->contours[handGesture->cMaxId][defect[1]];
//                Point vec0(curPoint0.x - curPoint.x,curPoint0.y - curPoint.y);
//                Point vec1(curPoint1.x - curPoint.x,curPoint1.y - curPoint.y);
//                double dot = vec0.x * vec1.x + vec0.y * vec1.y;
//                double lenth0 = sqrt(vec0.x * vec0.x + vec0.y
//                                     * vec0.y);
//                double lenth1 = sqrt(vec1.x * vec1.x + vec1.y
//                                     * vec1.y);
//                double cosTheta = dot / (lenth0 * lenth1);
//                bool bool1 = isClosedToBoundary(curPoint0, frame);
//                bool bool2 = isClosedToBoundary(curPoint1, frame);
//                if ((depth > handGesture->inCircleRadius * 0.7)
//                        && (cosTheta >= -0.7) && !bool1 && !bool2) {
//                    Point finVec0(curPoint0.x - handGesture->inCircle.x, curPoint0.y - handGesture->inCircle.y);
//                    double finAngle0 = atan2(finVec0.y, finVec0.x);
//                    Point finVec1 (curPoint1.x - handGesture->inCircle.x, curPoint1.y - handGesture->inCircle.y);
//                    double finAngle1 = atan2(finVec1.y, finVec1.x);

//                    if (handGesture->fingerTipsOrdered.size() == 0) {
//                        handGesture->fingerTipsOrdered.insert(pair<double,Point>(finAngle0, curPoint0));
//                        handGesture->fingerTipsOrdered.insert(pair<double,Point>(finAngle1, curPoint1));

//                    } else {
//                        handGesture->fingerTipsOrdered.insert(pair<double,Point>(finAngle0, curPoint0));
//                        handGesture->fingerTipsOrdered.insert(pair<double,Point>(finAngle1, curPoint1));

//                    }
//                }
//                //                else {
//                //                    if (((!bool1 && bool2) || (bool1 && !bool2)) && (depth > (handGesture->inCircleRadius * 0.2)) && (cosTheta <= 0)) {
//                //                        //                        System.out.println("Current Point: " + curPoint.x + " " + curPoint.y);
//                //                        //                        System.out.println("Radius: " + handGesture->inCircleRadius);
//                //                        //                        System.out.println("dept: " + depth);
//                //                        //                        System.out.println("cosTheta: " + cosTheta);
//                //                        //                        System.out.println("Point 0: " + isClosedToBoundary(curPoint0, frame) + " " + curPoint0.x + " " + curPoint0.y);
//                //                        //                        System.out.println("Point 1: " + isClosedToBoundary(curPoint1, frame) + " " + curPoint1.x + " " + curPoint1.y);
//                //                        // handGesture->defectIdAfter.add((i));
//                //                        handGesture->defectIdAfter.push_back(i);
//                //                        Point finVec0(curPoint0.x - handGesture->inCircle.x, curPoint0.y - handGesture->inCircle.y);
//                //                        double finAngle0 = atan2(finVec0.y, finVec0.x);
//                //                        Point finVec1 (curPoint1.x - handGesture->inCircle.x, curPoint1.y - handGesture->inCircle.y);
//                //                        double finAngle1 = atan2(finVec1.y, finVec1.x);

//                //                        if (handGesture->fingerTipsOrdered.size() == 0) {
//                //                            handGesture->fingerTipsOrdered.insert(pair<double,Point>(finAngle0, curPoint0));
//                //                            handGesture->fingerTipsOrdered.insert(pair<double,Point>(finAngle1, curPoint1));
//                //                            //  handGesture->fingerTipsOrdered.put(finAngle1, curPoint1);

//                //                        } else {
//                //                            handGesture->fingerTipsOrdered.insert(pair<double,Point>(finAngle0, curPoint0));
//                //                            handGesture->fingerTipsOrdered.insert(pair<double,Point>(finAngle1, curPoint1));
//                //                            //                            handGesture->fingerTipsOrdered.put(finAngle0, curPoint0);

//                //                            //                            handGesture->fingerTipsOrdered.put(finAngle1, curPoint1);

//                //                        }
//                //                    }
//                //                }

//                // }
//            }
//        }

    }

    if (handGesture->detectIsHand(frame)) {

        // handGesture->boundingRect represents four coordinates of the bounding box.
        rectangle(frame, handGesture->boundingRect.tl(), handGesture->boundingRect.br(),
                  mColorsRGB[1], 2);
        // drawContours(frame, handGesture->hullP, handGesture->cMaxId, mColorsRGB[2]);
    }
    return frame;
}

// Generates binary image containing user's hand
Mat HandThread::produceBinImg(Mat &imgIn, Mat &imgOut) {
    int colNum = imgIn.cols;
    int rowNum = imgIn.rows;
    int boxExtension = 0;

    boundariesCorrection();

    produceBinHandImg(imgIn, binTmpMat);
    imwrite("/home/nickseven/bin1.png",binTmpMat);

    produceBinBackImg(imgIn, binTmpMat2);
    imwrite("/home/nickseven/bin2.png",binTmpMat2);
    bitwise_and(binTmpMat, binTmpMat2, binTmpMat);
    imwrite("/home/nickseven/result.png",binTmpMat);
    binTmpMat.copyTo(tmpMat);
    binTmpMat.copyTo(imgOut);

    Rect roiRect = makeBoundingBox(tmpMat);

    // adjustBoundingBox(roiRect, binTmpMat);

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

