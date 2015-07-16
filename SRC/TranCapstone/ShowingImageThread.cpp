#include "ShowingImageThread.h"


ShowingImageThread::ShowingImageThread(QObject *parent) :
    QThread(parent)
{
    STOP = false;
    enableToShow = false;
    mode = BACKGROUND_MODE;
    blue = Scalar(255,0,0);
    red = Scalar(0,0,255);
    green = Scalar(0,255,0);
    white = Scalar(255,255,255);
    black = Scalar(0,0,0);
    initCLowerUpper(30, 30, 7, 7, 7, 7);
    initCBackLowerUpper(50, 50, 3, 3, 3, 3);
    initBackPoints();
    initHandPoints();
}


void ShowingImageThread::run() {
    while(true) {
        QMutex mutex;
        mutex.lock();
        if(this->STOP) break;
        mutex.unlock();
        if (enableToShow) {
            flip(frame,frame,1);
            if (mode == DETECTION_MODE) {
                GaussianBlur(frame, blurMat, Size(9, 9), 4);
                cvtColor(blurMat, labMat, COLOR_BGR2Lab);
                showMat = produceBinImg();
                emit sendImageToCrop(frame.clone(),showMat.clone());
            } else if (mode == BACKGROUND_MODE) {
                showMat = preSampleBack(frame.clone());
//                imwrite("../xulyanh/sample-background.jpg",showMat);
            } else if (mode == SAMPLE_FRONT_HAND_MODE) {
                showMat = preSampleFrontHand(frame.clone());
//                imwrite("../xulyanh/sample-fronthand.jpg",showMat);
            }  else if (mode == SAMPLE_BACK_HAND_MODE) {
                showMat = preSampleBackHand(frame.clone());
//                imwrite("../xulyanh/sample-backhand.jpg",showMat);
            }  else if (mode == GET_AVG_BACKGROUND) {
                GaussianBlur(frame, blurMat, Size(9, 9), 4);
//                imwrite("../xulyanh/sample-background-blur.jpg",blurMat);
                cvtColor(blurMat, labMat, COLOR_BGR2Lab);
//                imwrite("../xulyanh/sample-background-lab-mat.jpg",labMat);
                getSampleBack();
                mode = SAMPLE_FRONT_HAND_MODE;
            } else if (mode == GET_AVG_FRONT_HAND) {
                GaussianBlur(frame, blurMat, Size(9, 9), 4);
//                imwrite("../xulyanh/sample-fronthand-blur.jpg",blurMat);
                cvtColor(blurMat, labMat, COLOR_BGR2Lab);
//                imwrite("../xulyanh/sample-fronthand-lab.jpg",labMat);
                getSampleHand();
                mode = SAMPLE_BACK_HAND_MODE;
            } else if (mode == GET_AVG_BACK_HAND) {
                GaussianBlur(frame, blurMat, Size(9, 9), 4);
//                imwrite("../xulyanh/sample-backhand-blur.jpg",blurMat);
                cvtColor(blurMat, labMat, COLOR_BGR2Lab);
//                imwrite("../xulyanh/sample-backhand-lab.jpg",labMat);
                getSampleBackHand();
                mode = DETECTION_MODE;
            }
            emit (toShow(showMat.clone()));
            enableToShow = false;
        }
    }
}

Mat ShowingImageThread::preSampleBack(Mat img) {
    for (int i = 0; i < SAMPLE_BACK_NUM; i++) {
        rectangle(img, sampleBackPoints[i][0], sampleBackPoints[i][1], green,
                1);
    }
    return img;
}

Mat ShowingImageThread::preSampleFrontHand(Mat img) {
    drawContours(img, sampleFrontHand, -1, blue, 2, 8, hierarchy, 0, Point());
    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        circle(img, sampleHandPoints[i], 2, red, -5);
    }
    return img;
}

Mat ShowingImageThread::preSampleBackHand(Mat img) {
    drawContours(img, sampleBackHand, -1, blue, 2, 8, hierarchy, 0, Point());
    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        circle(img, sampleBackHandPoints[i], 2, red, -5);
    }
    return img;
}


void ShowingImageThread::getSampleBack() {
    Vec3b intensity;
    for (int i = 0; i < SAMPLE_BACK_NUM; i++) {
        intensity = labMat.at<Vec3b>((int) (sampleBackPoints[i][0].y + SQUARE_LENGTH / 2),(int) (sampleBackPoints[i][0].x + SQUARE_LENGTH / 2));
        for (int j = 0; j < 3; j++) {
            avgBackColor[i][j] = intensity.val[j];
        }
    }
    Mat src_gray, binary;
    /// Convert image to gray and blur it
    cvtColor(frame, src_gray, CV_BGR2GRAY );
    GaussianBlur(src_gray,src_gray,Size(7,7),3);
    adaptiveThreshold(src_gray,binary,255,ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,15,5);
    Mat element = getStructuringElement(MORPH_CROSS,
                                        Size( 1*1 + 1, 1*1+1 ),
                                        Point( 1, 1 ) );
    /// Apply the erosion operation
    erode(binary, binary, element);
    vector<vector<Point> > contours;
    /// Find contours
    Mat clone = binary.clone();
    findContours(clone, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    int x,y;
    Rect rect;
    for (int i = 0; i < contours.size();i++) {
        rect = boundingRect(contours[i]);
        if (rect.width >= 20 || rect.height >= 20) {
            x = rect.x + (rect.width/2);
            y = rect.y + (rect.height/2);
            if (pointPolygonTest(contours[i],Point(x,y),false) > 0) {
                intensity = labMat.at<Vec3b>(y,x);
                additionAvgBackColor.push_back(intensity);
            } else if(pointPolygonTest(contours[i],Point(x/2,y),false) > 0) {
                intensity = labMat.at<Vec3b>(y,x / 2);
                additionAvgBackColor.push_back(intensity);
            } else if (pointPolygonTest(contours[i],Point(x+(x/2),y),false) > 0) {
                intensity = labMat.at<Vec3b>(y,x + (x / 2));
                additionAvgBackColor.push_back(intensity);
            }

        }
    }
}

void ShowingImageThread::getSampleHand() {
    Vec3b intensity;
    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        intensity = labMat.at<Vec3b>((int) (sampleHandPoints[i].y),(int) (sampleHandPoints[i].x));
        for (int j = 0; j < 3; j++) {
            avgColor[i][j] = intensity.val[j];
        }
    }
}

void ShowingImageThread::getSampleBackHand() {
    Vec3b intensity;
    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        intensity = labMat.at<Vec3b>((int) (sampleBackHandPoints[i].y),(int) (sampleBackHandPoints[i].x));
        for (int j = 0; j < 3; j++) {
            avgBackHandColor[i][j] = intensity.val[j];
        }
    }
}

Mat ShowingImageThread::produceBinImg() {
    //  int boxExtension = 0;
    boundariesCorrection();
    produceBinHandImg();
    produceBinBackImg();
    bitwise_and(binTmpMat, binTmpMat2, binTmpMat);
    imwrite("../xulyanh/subtracted-binary-image.jpg",binTmpMat);
    binTmpMat.copyTo(tmpMat);
    binTmpMat.copyTo(binMat);
    //    Rect roiRect = makeBoundingBox(tmpMat);
    //    if (roiRect.area() > 0) {

    //        roiRect.x = max(0, roiRect.x);
    //        roiRect.y = max(0, roiRect.y);
    //        roiRect.width = min(roiRect.width, cols);
    //        roiRect.height = min(roiRect.height, rows);

    //        Mat roi1(binTmpMat, roiRect);
    //        Mat roi3(binMat, roiRect);
    //        // imgOut.setTo(Scalar.all(0));
    //        binMat.setTo(Scalar::all(0));

    //        roi1.copyTo(roi3);

    //        Mat element = getStructuringElement(MORPH_ELLIPSE,
    //                                            Size(3, 3));
    //        dilate(roi3, roi3, element, Point(-1, -1), 2);

    //        erode(roi3, roi3, element, Point(-1, -1), 2);

    //    }
    // cropBinImg(imgOut, imgOut);
    return binMat;
}

void ShowingImageThread::boundariesCorrection() {
    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        for (int j = 0; j < 3; j++) {
            if (avgColor[i][j] - cFrontHandLower[i][j] < 0) {
                cFrontHandLower[i][j] = avgColor[i][j];
            }
            if (avgColor[i][j] + cFrontHandUpper[i][j] > 255) {
                cFrontHandUpper[i][j] = 255 - avgColor[i][j];
            }
            if (avgBackHandColor[i][j] - cBackHandLower[i][j] < 0) {
                cBackHandLower[i][j] = avgBackHandColor[i][j];
            }
            if (avgBackHandColor[i][j] + cBackHandUpper[i][j] > 255) {
                cBackHandUpper[i][j] = 255 - avgBackHandColor[i][j];
            }
        }
    }

    for (int i = 0; i < SAMPLE_BACK_NUM; i++) {
        for (int j = 0; j < 3; j++) {

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

void ShowingImageThread::produceBinHandImg() {
    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        lowerBound.val[0] = avgColor[i][0] - cFrontHandLower[i][0];
        lowerBound.val[1] = avgColor[i][1] - cFrontHandLower[i][1];
        lowerBound.val[2] = avgColor[i][2] - cFrontHandLower[i][2];

        upperBound.val[0] = avgColor[i][0] + cFrontHandUpper[i][0];
        upperBound.val[1] = avgColor[i][1] + cFrontHandUpper[i][1];
        upperBound.val[2] = avgColor[i][2] + cFrontHandUpper[i][2];
        inRange(labMat, lowerBound, upperBound, sampleMats[i]);
    }

    binTmpMat.release();
    sampleMats[0].copyTo(binTmpMat);

    for (int i = 1; i < SAMPLE_HAND_NUM; i++) {
        add(binTmpMat, sampleMats[i], binTmpMat);
    }

    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        lowerBound.val[0] = avgBackHandColor[i][0] - cBackHandLower[i][0];
        lowerBound.val[1] = avgBackHandColor[i][1] - cBackHandLower[i][1];
        lowerBound.val[2] = avgBackHandColor[i][2] - cBackHandLower[i][2];

        upperBound.val[0] = avgBackHandColor[i][0] + cBackHandUpper[i][0];
        upperBound.val[1] = avgBackHandColor[i][1] + cBackHandUpper[i][1];
        upperBound.val[2] = avgBackHandColor[i][2] + cBackHandUpper[i][2];
        inRange(labMat, lowerBound, upperBound, sampleMats[i]);
    }

    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        add(binTmpMat, sampleMats[i], binTmpMat);
    }
//    imwrite("../xulyanh/hand-image-in-range.jpg",binTmpMat);
    Mat element = getStructuringElement(MORPH_RECT, Size( 2*3 + 1, 2*3 + 1 ), Point( 3, 3 ));

    /// Apply the specified morphology operation
    morphologyEx(binTmpMat, binTmpMat, CV_MOP_OPEN, element );
//    imwrite("../xulyanh/morpholy-hand-image-in-range.jpg",binTmpMat);
}

void ShowingImageThread::produceBinBackImg() {
    sampleBackgroundMats.clear();
    for (int i = 0; i < SAMPLE_BACK_NUM; i++) {
        lowerBound.val[0] = avgBackColor[i][0] - cBackLower[i][0];
        lowerBound.val[1] = avgBackColor[i][1] - cBackLower[i][1];
        lowerBound.val[2] = avgBackColor[i][2] - cBackLower[i][2];

        upperBound.val[0] = avgBackColor[i][0] + cBackUpper[i][0];
        upperBound.val[1] = avgBackColor[i][1] + cBackUpper[i][1];
        upperBound.val[2] = avgBackColor[i][2] + cBackUpper[i][2];
        Mat tmpSampleBackgroundMat;
        inRange(labMat, lowerBound, upperBound, tmpSampleBackgroundMat);
        sampleBackgroundMats.push_back(tmpSampleBackgroundMat);
    }
    for (int i = 0; i < additionAvgBackColor.size();i++) {
        lowerBound.val[0] = (double) additionAvgBackColor[i].val[0] - addBackLower[i][0];
        lowerBound.val[1] = (double) additionAvgBackColor[i].val[1] - addBackLower[i][1];
        lowerBound.val[2] = (double) additionAvgBackColor[i].val[2] - addBackLower[i][2];

        upperBound.val[0] = (double) additionAvgBackColor[i].val[0] + addBackUpper[i][0];
        upperBound.val[1] = (double) additionAvgBackColor[i].val[1] + addBackUpper[i][1];
        upperBound.val[2] = (double) additionAvgBackColor[i].val[2] + addBackUpper[i][2];

        Mat tmpSampleBackgroundMat;
        inRange(labMat, lowerBound, upperBound, tmpSampleBackgroundMat);
        sampleBackgroundMats.push_back(tmpSampleBackgroundMat);
    }
    binTmpMat2.release();
    sampleBackgroundMats[0].copyTo(binTmpMat2);
    for (int i = 1; i < sampleBackgroundMats.size(); i++) {
        add(binTmpMat2, sampleBackgroundMats[i], binTmpMat2);
    }
//    imwrite("../xulyanh/background-in-range.jpg",binTmpMat2);
    bitwise_not(binTmpMat2, binTmpMat2);
//    imwrite("../xulyanh/background-in-range-to-subtract.jpg",binTmpMat2);
    Mat element = getStructuringElement(MORPH_RECT, Size( 2*3 + 1, 2*3 + 1 ), Point( 3, 3 ) );
    /// Apply the specified morphology operation
    morphologyEx( binTmpMat2, binTmpMat2, CV_MOP_OPEN, element );
//    imwrite("../xulyanh/morpho-background-in-range-to-subtract.jpg",binTmpMat2);
}

void ShowingImageThread::initCLowerUpper(double cl1, double cu1, double cl2, double cu2,
                                         double cl3, double cu3) {
    for (int i = 0; i < SAMPLE_HAND_NUM; i++) {
        cFrontHandLower[i][0] = cl1;
        cFrontHandUpper[i][0] = cu1;
        cFrontHandLower[i][1] = cl2;
        cFrontHandUpper[i][1] = cu2;
        cFrontHandLower[i][2] = cl3;
        cFrontHandUpper[i][2] = cu3;
        cBackHandLower[i][0] = cl1;
        cBackHandUpper[i][0] = cu1;
        cBackHandLower[i][1] = cl2;
        cBackHandUpper[i][1] = cu2;
        cBackHandLower[i][2] = cl3;
        cBackHandUpper[i][2] = cu3;
    }
}

void ShowingImageThread::initCBackLowerUpper(double cl1, double cu1, double cl2, double cu2,
                                             double cl3, double cu3) {
    for (int i = 0; i < SAMPLE_BACK_NUM; i++) {
        cBackLower[i][0] = cl1;
        cBackUpper[i][0] = cu1;
        cBackLower[i][1] = cl2;
        cBackUpper[i][1] = cu2;
        cBackLower[i][2] = cl3;
        cBackUpper[i][2] = cu3;
    }
    for (int i = 0; i < 50; i++) {
        addBackLower[i][0] = cl1;
        addBackUpper[i][0] = cu1;
        addBackLower[i][1] = cl2;
        addBackUpper[i][1] = cu2;
        addBackLower[i][2] = cl3;
        addBackUpper[i][2] = cu3;
    }
}

void ShowingImageThread::initBackPoints() {
    sampleBackPoints[0][0].x = IMAGE_COLS / 9;
    sampleBackPoints[0][0].y = IMAGE_ROWS / 7;
    sampleBackPoints[0][1].x = IMAGE_COLS / 9 + SQUARE_LENGTH;
    sampleBackPoints[0][1].y = IMAGE_ROWS / 7 + SQUARE_LENGTH;
    sampleBackPoints[1][0].x = IMAGE_COLS / 3;
    sampleBackPoints[1][0].y = IMAGE_ROWS / 7;
    sampleBackPoints[1][1].x = IMAGE_COLS / 3 + SQUARE_LENGTH;
    sampleBackPoints[1][1].y = IMAGE_ROWS / 7 + SQUARE_LENGTH;
    sampleBackPoints[2][0].x = IMAGE_COLS / 1.7;
    sampleBackPoints[2][0].y = IMAGE_ROWS / 7 ;
    sampleBackPoints[2][1].x = IMAGE_COLS / 1.7 + SQUARE_LENGTH;
    sampleBackPoints[2][1].y = IMAGE_ROWS / 7  + SQUARE_LENGTH;
    sampleBackPoints[3][0].x = IMAGE_COLS / 9 * 7.5;
    sampleBackPoints[3][0].y = IMAGE_ROWS / 7;
    sampleBackPoints[3][1].x = IMAGE_COLS / 9 * 7.5 + SQUARE_LENGTH;
    sampleBackPoints[3][1].y = IMAGE_ROWS / 7 + SQUARE_LENGTH;
    sampleBackPoints[4][0].x = IMAGE_COLS / 9;
    sampleBackPoints[4][0].y = IMAGE_ROWS / 2.1;
    sampleBackPoints[4][1].x = IMAGE_COLS / 9 + SQUARE_LENGTH;
    sampleBackPoints[4][1].y = IMAGE_ROWS / 2.1 + SQUARE_LENGTH;
    sampleBackPoints[5][0].x = IMAGE_COLS / 3;
    sampleBackPoints[5][0].y = IMAGE_ROWS / 2.1;
    sampleBackPoints[5][1].x = IMAGE_COLS / 3+ SQUARE_LENGTH;
    sampleBackPoints[5][1].y = IMAGE_ROWS / 2.1 + SQUARE_LENGTH;
    sampleBackPoints[6][0].x = IMAGE_COLS / 1.7;
    sampleBackPoints[6][0].y = IMAGE_ROWS / 2.1;
    sampleBackPoints[6][1].x = IMAGE_COLS / 1.7 + SQUARE_LENGTH;
    sampleBackPoints[6][1].y = IMAGE_ROWS / 2.1 + SQUARE_LENGTH;
    sampleBackPoints[7][0].x = IMAGE_COLS / 9 * 7.5;
    sampleBackPoints[7][0].y = IMAGE_ROWS / 2.1;
    sampleBackPoints[7][1].x = IMAGE_COLS / 9 * 7.5 + SQUARE_LENGTH;
    sampleBackPoints[7][1].y = IMAGE_ROWS / 2.1  + SQUARE_LENGTH;
    sampleBackPoints[8][0].x = IMAGE_COLS / 9;
    sampleBackPoints[8][0].y = IMAGE_ROWS / 1.25;
    sampleBackPoints[8][1].x = IMAGE_COLS / 9 + SQUARE_LENGTH;
    sampleBackPoints[8][1].y = IMAGE_ROWS / 1.25 + SQUARE_LENGTH;
    sampleBackPoints[9][0].x = IMAGE_COLS / 3;
    sampleBackPoints[9][0].y = IMAGE_ROWS / 1.25;
    sampleBackPoints[9][1].x = IMAGE_COLS / 3 + SQUARE_LENGTH;
    sampleBackPoints[9][1].y = IMAGE_ROWS / 1.25+ SQUARE_LENGTH;
    sampleBackPoints[10][0].x = IMAGE_COLS / 1.7;
    sampleBackPoints[10][0].y = IMAGE_ROWS / 1.25;
    sampleBackPoints[10][1].x = IMAGE_COLS / 1.7 + SQUARE_LENGTH;
    sampleBackPoints[10][1].y = IMAGE_ROWS / 1.25 + SQUARE_LENGTH;
    sampleBackPoints[11][0].x = IMAGE_COLS / 9 * 7.5;
    sampleBackPoints[11][0].y = IMAGE_ROWS / 1.25;
    sampleBackPoints[11][1].x = IMAGE_COLS / 9 * 7.5 + SQUARE_LENGTH;
    sampleBackPoints[11][1].y = IMAGE_ROWS / 1.25 + SQUARE_LENGTH;
}

void ShowingImageThread::initHandPoints() {
    // position of front hand's features
    sampleHandPoints[0].x = 69;
    sampleHandPoints[0].y = 135;
    sampleHandPoints[1].x = 106;
    sampleHandPoints[1].y = 175;
    sampleHandPoints[2].x = 115;
    sampleHandPoints[2].y = 51;
    sampleHandPoints[3].x = 135;
    sampleHandPoints[3].y = 118;
    sampleHandPoints[4].x = 161;
    sampleHandPoints[4].y = 33;
    sampleHandPoints[5].x = 163;
    sampleHandPoints[5].y = 113;
    sampleHandPoints[6].x = 196;
    sampleHandPoints[6].y = 44;
    sampleHandPoints[7].x = 188;
    sampleHandPoints[7].y = 118;
    sampleHandPoints[8].x = 239;
    sampleHandPoints[8].y = 72;
    sampleHandPoints[9].x = 213;
    sampleHandPoints[9].y = 131;
    sampleHandPoints[10].x = 130;
    sampleHandPoints[10].y = 196;
    sampleHandPoints[11].x = 201;
    sampleHandPoints[11].y = 196;
    sampleHandPoints[12].x = 167;
    sampleHandPoints[12].y = 162;

    // position of back hand's features
    sampleBackHandPoints[0].x = 125;
    sampleBackHandPoints[0].y = 93;
    sampleBackHandPoints[1].x =  160;
    sampleBackHandPoints[1].y = 77;
    sampleBackHandPoints[2].x = 185;
    sampleBackHandPoints[2].y = 77;
    sampleBackHandPoints[3].x = 120;
    sampleBackHandPoints[3].y = 150;
    sampleBackHandPoints[4].x = 154;
    sampleBackHandPoints[4].y = 131;
    sampleBackHandPoints[5].x = 188;
    sampleBackHandPoints[5].y = 130;
    sampleBackHandPoints[6].x = 125;
    sampleBackHandPoints[6].y = 171;
    sampleBackHandPoints[7].x = 155;
    sampleBackHandPoints[7].y = 170;
    sampleBackHandPoints[8].x = 193;
    sampleBackHandPoints[8].y = 158;
    sampleBackHandPoints[9].x = 226;
    sampleBackHandPoints[9].y = 155;
    sampleBackHandPoints[10].x = 131;
    sampleBackHandPoints[10].y = 203;
    sampleBackHandPoints[11].x = 177;
    sampleBackHandPoints[11].y = 206;
    sampleBackHandPoints[12].x = 206;
    sampleBackHandPoints[12].y = 179;
    Mat image, src_gray, dst;
    image = imread("../Database/mau-tay-moi.png");
    cvtColor( image, src_gray, CV_BGR2GRAY );
    threshold(src_gray, dst, 128, 255,0);
    findContours(dst,sampleFrontHand,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
    image = imread("../Database/back-hand-mau.png");
    cvtColor( image, src_gray, CV_BGR2GRAY );
    threshold(src_gray, dst, 128, 255,0);
    findContours(dst,sampleBackHand,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
}

void ShowingImageThread::setMode(int mode) {
    this->mode = mode;
}

void ShowingImageThread::onChangingImage(Mat binary) {
    if (!enableToShow) {
        frame = binary;
        enableToShow = true;
    }
}
