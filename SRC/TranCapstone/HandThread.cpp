#include "HandThread.h"
#include <QtCore>

HandThread::HandThread(QObject *parent) :
    QThread(parent)
{
    handGesture = new HandGesture();
    cols = 320;
    rows = 240;
    squareLen = rows / 20;
    webSource = VideoCapture(0);
    webSource.set(CV_CAP_PROP_FRAME_WIDTH,cols);
    webSource.set(CV_CAP_PROP_FRAME_HEIGHT,rows);
    initCLowerUpper(50, 50, 10, 10, 10, 10);
    initCBackLowerUpper(50, 50, 3, 3, 3, 3);
    initBackPoints();
    initHandPoints();
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
            //            GaussianBlur(frame, blurMat, Size(5, 5), 5, 5);
              imwrite("/home/nickseven/mau-tay.png",frame);
            GaussianBlur(frame, blurMat, Size(9, 9), 4);
            cvtColor(blurMat, interMat, COLOR_BGR2Lab);
            Mat showMat ;
            if (mode == DETECTION_MODE) { // Third mode which generates the
                // binary image containing the
                // segmented hand represented by
                // white color
                frame = imread("/home/nickseven/2.png");
                GaussianBlur(frame, blurMat, Size(7, 7), 3);
                cvtColor(blurMat, interMat, COLOR_BGR2Lab);
                showMat = produceBinImg();
                //                imwrite("/home/nickseven/re-mau.png",showMat);
                emit binaryImageHandChanged(frame, binMat);
                //   bitwise_and(frame,frame,showMat,showMat);
            } else if (mode == BACKGROUND_MODE) {// First mode which presamples
                // background colors
                //additionFrame = frame.clone();
//                frame = imread("/home/nickseven/1.png");
//                GaussianBlur(frame, blurMat, Size(7, 7), 3);
//                cvtColor(blurMat, interMat, COLOR_BGR2Lab);

                imwrite("/home/nickseven/bg-mau.png",frame);
                showMat = preSampleBack(frame.clone());
            } else if (mode == SAMPLE_MODE) { // Second mode which presamples the colors of
                // the hand
                //getSampleBack();
                frame = imread("/home/nickseven/2.png");
                GaussianBlur(frame, blurMat, Size(7, 7), 3);
                cvtColor(blurMat, interMat, COLOR_BGR2Lab);
                imwrite("/home/nickseven/ha-mau.png",frame);
                showMat = preSampleHand(frame.clone());
            } else if (mode == TRAIN_REC_MODE) {
                frame = imread("/home/nickseven/2.png");
                GaussianBlur(frame, blurMat, Size(7, 7), 3);
                cvtColor(blurMat, interMat, COLOR_BGR2Lab);
                showMat = produceBinImg();
                makeContours();
                //  handGesture->featureExtraction(frame, curLabel);
            } else if (mode == GET_AVG_BACKGROUND) {
                frame = imread("/home/nickseven/1.png");
                GaussianBlur(frame, blurMat, Size(7, 7), 3);
                cvtColor(blurMat, interMat, COLOR_BGR2Lab);
                getSampleBack();
                mode = SAMPLE_MODE;
            } else {
                frame = imread("/home/nickseven/2.png");
                GaussianBlur(frame, blurMat, Size(7, 7), 3);
                cvtColor(blurMat, interMat, COLOR_BGR2Lab);
                getSampleHand();
                mode = DETECTION_MODE;
            }
            imageObject = cvMatToQImage(showMat);
            emit handTrackingChanged(imageObject);
        }
    }
}


void HandThread::releaseAll() {
    webSource.release();
}

void HandThread::initCLowerUpper(double cl1, double cu1, double cl2, double cu2,
                                 double cl3, double cu3) {
    for (int i = 0; i < SAMPLE_NUM; i++) {
        cLower[i][0] = cl1;
        cUpper[i][0] = cu1;
        cLower[i][1] = cl2;
        cUpper[i][1] = cu2;
        cLower[i][2] = cl3;
        cUpper[i][2] = cu3;

    }
}

void HandThread::initCBackLowerUpper(double cl1, double cu1, double cl2, double cu2,
                                     double cl3, double cu3) {

    for (int i = 0; i < HAND_NUM; i++) {
        cBackLower[i][0] = cl1;
        cBackUpper[i][0] = cu1;
        cBackLower[i][1] = cl2;
        cBackUpper[i][1] = cu2;
        cBackLower[i][2] = cl3;
        cBackUpper[i][2] = cu3;
    }
}

void HandThread::initHandPoints() {
    sampleHandPoints[0][0].x = cols / 2;
    sampleHandPoints[0][1].x = cols / 2 + squareLen;
    sampleHandPoints[0][0].y = rows / 4;
    sampleHandPoints[0][1].y = rows / 4 + squareLen;
    sampleHandPoints[1][0].x = cols * 5 / 12;
    sampleHandPoints[1][1].x = cols * 5 / 12 + squareLen;
    sampleHandPoints[1][0].y = rows * 5 / 12;
    sampleHandPoints[1][1].y = rows * 5 / 12 + squareLen;
    sampleHandPoints[2][0].x = cols * 7 / 12;
    sampleHandPoints[2][1].x = cols * 7 / 12 + squareLen;
    sampleHandPoints[2][0].y = rows * 5 / 12;
    sampleHandPoints[2][1].y = rows * 5 / 12 + squareLen;
    sampleHandPoints[3][0].x = cols / 2;
    sampleHandPoints[3][1].x = cols / 2 + squareLen;
    sampleHandPoints[3][0].y = rows * 7 / 12;
    sampleHandPoints[3][1].y = rows * 7 / 12 + squareLen;
    sampleHandPoints[4][0].x = cols / 3;
    sampleHandPoints[4][1].x = cols / 3 + squareLen;
    sampleHandPoints[4][0].y = rows * 7 / 12;
    sampleHandPoints[4][1].y = rows * 7 / 12 + squareLen;
    sampleHandPoints[5][0].x = cols * 4 / 9;
    sampleHandPoints[5][1].x = cols * 4 / 9 + squareLen;
    sampleHandPoints[5][0].y = rows * 3 / 4;
    sampleHandPoints[5][1].y = rows * 3 / 4 + squareLen;
    sampleHandPoints[6][0].x = cols * 5 / 9;
    sampleHandPoints[6][1].x = cols * 5 / 9 + squareLen;
    sampleHandPoints[6][0].y = rows * 3 / 4;
    sampleHandPoints[6][1].y = rows * 3 / 4 + squareLen;
}

void HandThread::initBackPoints() {
    int width = cols / 5;
    int height = rows / 4;
    sampleBackPoints[0][0].x = cols / 9;
    sampleBackPoints[0][0].y = rows / 7;
    sampleBackPoints[0][1].x = cols / 9 + squareLen;
    sampleBackPoints[0][1].y = rows / 7 + squareLen;
    sampleBackPoints[1][0].x = cols / 3;
    sampleBackPoints[1][0].y = rows / 7;
    sampleBackPoints[1][1].x = cols / 3 + squareLen;
    sampleBackPoints[1][1].y = rows / 7 + squareLen;
    sampleBackPoints[2][0].x = cols / 1.7;
    sampleBackPoints[2][0].y = rows / 7 ;
    sampleBackPoints[2][1].x = cols / 1.7 + squareLen;
    sampleBackPoints[2][1].y = rows / 7  + squareLen;
    sampleBackPoints[3][0].x = cols / 9 * 7.5;
    sampleBackPoints[3][0].y = rows / 7;
    sampleBackPoints[3][1].x = cols / 9 * 7.5 + squareLen;
    sampleBackPoints[3][1].y = rows / 7 + squareLen;
    sampleBackPoints[4][0].x = cols / 9;
    sampleBackPoints[4][0].y = rows / 2.1;
    sampleBackPoints[4][1].x = cols / 9 + squareLen;
    sampleBackPoints[4][1].y = rows / 2.1 + squareLen;
    sampleBackPoints[5][0].x = cols / 3;
    sampleBackPoints[5][0].y = rows / 2.1;
    sampleBackPoints[5][1].x = cols / 3+ squareLen;
    sampleBackPoints[5][1].y = rows / 2.1 + squareLen;
    sampleBackPoints[6][0].x = cols / 1.7;
    sampleBackPoints[6][0].y = rows / 2.1;
    sampleBackPoints[6][1].x = cols / 1.7 + squareLen;
    sampleBackPoints[6][1].y = rows / 2.1 + squareLen;
    sampleBackPoints[7][0].x = cols / 9 * 7.5;
    sampleBackPoints[7][0].y = rows / 2.1;
    sampleBackPoints[7][1].x = cols / 9 * 7.5 + squareLen;
    sampleBackPoints[7][1].y = rows / 2.1  + squareLen;
    sampleBackPoints[8][0].x = cols / 9;
    sampleBackPoints[8][0].y = rows / 1.25;
    sampleBackPoints[8][1].x = cols / 9 + squareLen;
    sampleBackPoints[8][1].y = rows / 1.25 + squareLen;
    sampleBackPoints[9][0].x = cols / 3;
    sampleBackPoints[9][0].y = rows / 1.25;
    sampleBackPoints[9][1].x = cols / 3 + squareLen;
    sampleBackPoints[9][1].y = rows / 1.25+ squareLen;
    sampleBackPoints[10][0].x = cols / 1.7;
    sampleBackPoints[10][0].y = rows / 1.25;
    sampleBackPoints[10][1].x = cols / 1.7 + squareLen;
    sampleBackPoints[10][1].y = rows / 1.25 + squareLen;
    sampleBackPoints[11][0].x = cols / 9 * 7.5;
    sampleBackPoints[11][0].y = rows / 1.25;
    sampleBackPoints[11][1].x = cols / 9 * 7.5 + squareLen;
    sampleBackPoints[11][1].y = rows / 1.25 + squareLen;
    //    sampleBackPoints[0][0].x = cols / 6;
    //    sampleBackPoints[0][0].y = rows / 3;
    //    sampleBackPoints[1][0].x = cols / 6;
    //    sampleBackPoints[1][0].y = rows * 2 / 3;
    //    sampleBackPoints[2][0].x = cols / 2;
    //    sampleBackPoints[2][0].y = rows / 6;
    //    sampleBackPoints[3][0].x = cols / 2;
    //    sampleBackPoints[3][0].y = rows / 2;
    //    sampleBackPoints[4][0].x = cols / 2;
    //    sampleBackPoints[4][0].y = rows * 5 / 6;
    //    sampleBackPoints[5][0].x = cols * 5 / 6;
    //    sampleBackPoints[5][0].y = rows / 3;
    //    sampleBackPoints[6][0].x = cols * 5 / 6;
    //    sampleBackPoints[6][0].y = rows * 2 / 3;

    //    sampleBackPoints[0][1].x = cols / 6 + squareLen;
    //    sampleBackPoints[0][1].y = rows / 3 + squareLen;
    //    sampleBackPoints[1][1].x = cols / 6 + squareLen;
    //    sampleBackPoints[1][1].y = rows * 2 / 3 + squareLen;
    //    sampleBackPoints[2][1].x = cols / 2 + squareLen;
    //    sampleBackPoints[2][1].y = rows / 6 + squareLen;
    //    sampleBackPoints[3][1].x = cols / 2 + squareLen;
    //    sampleBackPoints[3][1].y = rows / 2 + squareLen;
    //    sampleBackPoints[4][1].x = cols / 2 + squareLen;
    //    sampleBackPoints[4][1].y = rows * 5 / 6 + squareLen;
    //    sampleBackPoints[5][1].x = cols * 5 / 6 + squareLen;
    //    sampleBackPoints[5][1].y = rows / 3 + squareLen;
    //    sampleBackPoints[6][1].x = cols * 5 / 6 + squareLen;
    //    sampleBackPoints[6][1].y = rows * 2 / 3 + squareLen;
}

// Presampling hand colors.
// Output is avgColor, which is essentially a 7 by 3 matrix storing the
// colors sampled by seven squares
Mat HandThread::preSampleHand(Mat img) {


    for (int i = 0; i < SAMPLE_NUM; i++) {
        rectangle(img, sampleHandPoints[i][0], sampleHandPoints[i][1], mColorsRGB[1],
                1);
    }

    return img;
}

void HandThread::getSampleHand() {
    Vec3b intensity;
    for (int i = 0; i < SAMPLE_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            intensity = interMat.at<Vec3b>((int) (sampleHandPoints[i][0].y + squareLen / 2),(int) (sampleHandPoints[i][0].x + squareLen / 2));
            avgColor[i][j] = intensity.val[j];
        }
    }
}

// Presampling background colors.
// Output is avgBackColor, which is essentially a 7 by 3 matrix storing the
// colors sampled by seven squares
Mat HandThread::preSampleBack(Mat img) {

    for (int i = 0; i < HAND_NUM; i++) {
        rectangle(img, sampleBackPoints[i][0], sampleBackPoints[i][1], mColorsRGB[0],
                1);
    }
    return img;
}

void HandThread::getSampleBack() {
    Vec3b intensity;
    for (int i = 0; i < HAND_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            intensity = interMat.at<Vec3b>((int) (sampleBackPoints[i][0].y + squareLen / 2),(int) (sampleBackPoints[i][0].x + squareLen / 2));
            avgBackColor[i][j] = intensity.val[j];
        }
    }
    Mat src_gray, binary;
    /// Convert image to gray and blur it
    cvtColor(frame, src_gray, CV_BGR2GRAY );
    GaussianBlur(src_gray,src_gray,Size(7,7),3);

    adaptiveThreshold(src_gray,binary,255,ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,15,5);
    Mat element = getStructuringElement( MORPH_CROSS,
                                         Size( 1*1 + 1, 1*1+1 ),
                                         Point( 1, 1 ) );
    /// Apply the erosion operation
    erode(binary, binary, element );
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    /// Find contours
    Mat clone = binary.clone();
    findContours(clone, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    //vector<Point> additionalPoint;
    int x,y;
    Rect rect;
    for (int i = 0; i < contours.size();i++) {
        rect = boundingRect(contours[i]);
        if (rect.width >= 20 || rect.height >= 20) {
            //  rectangle(frame,rect,Scalar(255,0,0),2);
            x = rect.x + (rect.width/2);
            y = rect.y + (rect.height/2);
            if (pointPolygonTest(contours[i],Point(x,y),false) > 0) {
                intensity = interMat.at<Vec3b>(y,x);
                additionAvgBackColor.push_back(intensity);
                rectangle(frame,rect,Scalar(255,0,0),2);
            } else if(pointPolygonTest(contours[i],Point(x/2,y),false) > 0) {
                intensity = interMat.at<Vec3b>(y,x / 2);
                additionAvgBackColor.push_back(intensity);
                rectangle(frame,rect,Scalar(0,255,0),2);
            } else if (pointPolygonTest(contours[i],Point(x+(x/2),y),false) > 0){
                intensity = interMat.at<Vec3b>(y,x + (x / 2));
                additionAvgBackColor.push_back(intensity);
                rectangle(frame,rect,Scalar(0,0,255),2);
            }

        }
    }
    for (int i = 0; i < additionAvgBackColor.size(); i++) {
        addBackLower[i][0] = cBackLower[0][0];
        addBackUpper[i][0] = cBackUpper[0][0];
        addBackLower[i][1] = cBackLower[0][1];
        addBackUpper[i][1] = cBackUpper[0][1];
        addBackLower[i][2] = cBackLower[0][2];
        addBackUpper[i][2] = cBackUpper[0][2];
    }
    imwrite("/home/nickseven/add.png",frame);
    cout << "size : " << additionAvgBackColor.size() << endl;
}

void HandThread::boundariesCorrection() {


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

    for (int i = 0; i < additionAvgBackColor.size(); i++) {
        for (int j = 0; j < 3; j++) {

            if (additionAvgBackColor[i].val[j] - addBackLower[i][j] < 0) {
                addBackLower[i][j] = additionAvgBackColor[i].val[j] ;
            }


            if (additionAvgBackColor[i].val[j]  + addBackUpper[i][j] > 255) {
                addBackUpper[i][j] = 255 - additionAvgBackColor[i].val[j] ;
            }

        }
    }
}

// Generates binary image thresholded only by sampled hand colors
void HandThread::produceBinHandImg() {
    for (int i = 0; i < SAMPLE_NUM; i++) {
        lowerBound.val[0] = avgColor[i][0] - cLower[i][0];
        lowerBound.val[1] = avgColor[i][1] - cLower[i][1];
        lowerBound.val[2] = avgColor[i][2] - cLower[i][2];

        upperBound.val[0] = avgColor[i][0] + cUpper[i][0];
        upperBound.val[1] = avgColor[i][1] + cUpper[i][1];
        upperBound.val[2] = avgColor[i][2] + cUpper[i][2];
        inRange(interMat, lowerBound, upperBound, sampleMats[i]);

    }

    binTmpMat.release();
    sampleMats[0].copyTo(binTmpMat);

    for (int i = 1; i < SAMPLE_NUM; i++) {
        add(binTmpMat, sampleMats[i], binTmpMat);
    }
    imwrite("/home/nickseven/h1.png",binTmpMat);
    medianBlur(binTmpMat, binTmpMat, 3);
    imwrite("/home/nickseven/h2.png",binTmpMat);
}

// Generates binary image thresholded only by sampled background colors
void HandThread::produceBinBackImg() {
    sampleBackgroundMats.clear();
    //    imwrite("/home/nickseven/imgIn.png",imgIn);
    for (int i = 0; i < HAND_NUM; i++) {
        lowerBound.val[0] = avgBackColor[i][0] - cBackLower[i][0];
        lowerBound.val[1] = avgBackColor[i][1] - cBackLower[i][1];
        lowerBound.val[2] = avgBackColor[i][2] - cBackLower[i][2];

        upperBound.val[0] = avgBackColor[i][0] + cBackUpper[i][0];
        upperBound.val[1] = avgBackColor[i][1] + cBackUpper[i][1];
        upperBound.val[2] = avgBackColor[i][2] + cBackUpper[i][2];

        Mat tmpSampleBackgroundMat;
        inRange(interMat, lowerBound, upperBound, tmpSampleBackgroundMat);
        sampleBackgroundMats.push_back(tmpSampleBackgroundMat);
        imwrite("/home/nickseven/sample-"+ QString::number(i).toStdString() +".png",tmpSampleBackgroundMat);
    }
    for (int i = 0; i < additionAvgBackColor.size();i++) {
        lowerBound.val[0] = (double) additionAvgBackColor[i].val[0] - addBackLower[i][0];
        lowerBound.val[1] = (double) additionAvgBackColor[i].val[1] - addBackLower[i][1];
        lowerBound.val[2] = (double) additionAvgBackColor[i].val[2] - addBackLower[i][2];

        upperBound.val[0] = (double) additionAvgBackColor[i].val[0] + addBackUpper[i][0];
        upperBound.val[1] = (double) additionAvgBackColor[i].val[1] + addBackUpper[i][1];
        upperBound.val[2] = (double) additionAvgBackColor[i].val[2] + addBackUpper[i][2];

        Mat tmpSampleBackgroundMat;
        inRange(interMat, lowerBound, upperBound, tmpSampleBackgroundMat);
        sampleBackgroundMats.push_back(tmpSampleBackgroundMat);
        imwrite("/home/nickseven/sample-after-"+ QString::number(i).toStdString() +".png",tmpSampleBackgroundMat);
    }
    binTmpMat2.release();

    sampleBackgroundMats[0].copyTo(binTmpMat2);
    for (int i = 1; i < sampleBackgroundMats.size(); i++) {
        add(binTmpMat2, sampleBackgroundMats[i], binTmpMat2);
    }
    imwrite("/home/nickseven/bg-bin.png",binTmpMat2);
    bitwise_not(binTmpMat2, binTmpMat2);
    imwrite("/home/nickseven/bg-bin-1.png",binTmpMat2);
    medianBlur(binTmpMat2, binTmpMat2, 7);
    imwrite("/home/nickseven/bg-bin-2.png",binTmpMat2);
}

Rect HandThread::makeBoundingBox(Mat &img) {
    handGesture->contours.clear();

    findContours(img, handGesture->contours, handGesture->hie, CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

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
    //  Mat cloneBin = binMat.clone();
    findContours(binMat.clone(), handGesture->contours, handGesture->hie,
                 RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // Find biggest contour and return the index of the contour, which is
    // handGesture->cMaxId
    handGesture->findBiggestContour();

    if (handGesture->cMaxId > -1) {
        //biggestContours.
        //  Mat(handGesture->contours[handGesture->cMaxId]).copyTo(handGesture->approxContour);
        // approxPolyDP(handGesture->approxContour, handGesture->approxContour, 2, true);
        // Mat(handGesture->approxContour).copyTo(handGesture->contours[handGesture->cMaxId]);
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
        //cout << "vao day nhi" << endl;

        // handGesture->boundingRect represents four coordinates of the bounding box.
        //   rectangle(frame, handGesture->boundingRect.tl(), handGesture->boundingRect.br(),
        //           mColorsRGB[1], 2);
        //drawContours(frame, handGesture->hullP, handGesture->cMaxId, mColorsRGB[2]);
        //emit handSubtractingChanged(frame, binMat,handGesture->boundingRect);
    }
    return binMat;
}

// Generates binary image containing user's hand
Mat HandThread::produceBinImg() {
    //  int boxExtension = 0;

    boundariesCorrection();

    produceBinHandImg();
    //    imwrite("/home/nickseven/bin1.png",binTmpMat);

    produceBinBackImg();
    //    imwrite("/home/nickseven/bin2.png",binTmpMat2);
    bitwise_and(binTmpMat, binTmpMat2, binTmpMat);
    //    imwrite("/home/nickseven/result.png",binTmpMat);
    imwrite("/home/nickseven/re-and.png",binTmpMat);
    binTmpMat.copyTo(tmpMat);
    binTmpMat.copyTo(binMat);

    Rect roiRect = makeBoundingBox(tmpMat);

    // adjustBoundingBox(roiRect, binTmpMat);

    if (roiRect.area() > 0) {

        roiRect.x = max(0, roiRect.x);
        roiRect.y = max(0, roiRect.y);
        roiRect.width = min(roiRect.width, cols);
        roiRect.height = min(roiRect.height, rows);

        Mat roi1(binTmpMat, roiRect);
        Mat roi3(binMat, roiRect);
        // imgOut.setTo(Scalar.all(0));
        binMat.setTo(Scalar::all(0));

        roi1.copyTo(roi3);

        Mat element = getStructuringElement(MORPH_ELLIPSE,
                                            Size(3, 3));
        dilate(roi3, roi3, element, Point(-1, -1), 2);

        erode(roi3, roi3, element, Point(-1, -1), 2);

    }
    imwrite("/home/nickseven/re-fi.png",binMat);
    // cropBinImg(imgOut, imgOut);
    return binMat;
}

