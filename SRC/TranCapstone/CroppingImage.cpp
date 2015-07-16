#include "CroppingImage.h"

CroppingImage::CroppingImage(QObject *parent) :
    QThread(parent)
{
    STOP = false;
    enableToCrop = false;
    cropImageSize = Size(96,96);
    whiteColor = Scalar(255,255,255);
    blackColor = Scalar(0,0,0);
    handGesture = new HandGesture();
}

void CroppingImage::run() {
    while(true) {
        QMutex mutex;
        mutex.lock();
        if(this->STOP) break;
        mutex.unlock();
        if (enableToCrop) {

            //            tile = ((double) ((double) (handGesture->boundingRect.height) / (double) (handGesture->boundingRect.width)) / 2);
            if (handGesture->detectIsHand(binaryMat)) {
                //                mask = Mat(240, 320, CV_8UC1);
                maskForInner = Mat(240, 320, CV_8UC1);
                maskForXuong = Mat(240, 320, CV_8UC1);
                maskForFrame = Mat(240, 320, CV_8UC1);
                //                mask.setTo(whiteColor);
                maskForInner.setTo(whiteColor);
                maskForXuong.setTo(whiteColor);
                maskForFrame.setTo(blackColor);
                drawContours(maskForFrame, handGesture->contours, handGesture->cMaxId, whiteColor, CV_FILLED);
                //                drawContours(mask, handGesture->contours, handGesture->cMaxId, Scalar(0,0,0), 2, 8, handGesture->hie, 0, Point());

                // Moi bo di
                //                Point inCirlcePoint = handGesture->inCircle;
                //                circle(maskForInner, inCirlcePoint, (int)handGesture->inCircleRadius, Scalar(0,0,0), 2);
                //                circle(maskForInner, inCirlcePoint, 7, Scalar::all(0), -2);
                //                for (int i = 0; i < handGesture->correctDefects.size(); i+=3) {
                //                    Point curPoint = handGesture->correctDefects[i];
                //                    Point curPoint0 = handGesture->correctDefects[i+1];
                //                    Point curPoint1 = handGesture->correctDefects[i+2];
                //                    line(maskForXuong, inCirlcePoint, curPoint0, Scalar(0, 0, 0), 2);
                //                    line(maskForXuong, inCirlcePoint, curPoint1, Scalar(0,0,0), 2);
                //                    line(maskForXuong, curPoint, curPoint0, Scalar(0,0,0), 2);
                //                    line(maskForXuong, curPoint, curPoint1, Scalar(0,0,0), 2);
                //                }
                // End

                // Them vao
                handGesture->drawInnerAndXuong(maskForInner,maskForXuong);
                // End

                Rect rectToCrop = handGesture->boundingRect;
                maskForFrame = maskForFrame(rectToCrop);
                //                croppedBinMat = mask(rectToCrop);
                croppedFrame = frame(rectToCrop);
                maskForInner = maskForInner(rectToCrop);
                maskForXuong = maskForXuong(rectToCrop);
                if (maskForFrame.cols > maskForFrame.rows) {
                    squareLength = maskForFrame.cols + 10;
                    tly = (squareLength - maskForFrame.rows) / 2;
                    tlx =  5;
                } else {
                    squareLength = maskForFrame.rows + 10;
                    tlx = (squareLength - maskForFrame.cols) / 2;
                    tly =  5;
                }
                croppedHand = Mat(squareLength,squareLength,CV_8UC3);
                croppedHand.setTo(blackColor);
                //                croppedBinHand = Mat(squareLength,squareLength,CV_8UC1);
                //                croppedBinHand.setTo(Scalar(255,255,255));
                croppedInnerHand = Mat(squareLength,squareLength,CV_8UC1);
                croppedInnerHand.setTo(whiteColor);
                croppedXuongHand = Mat(squareLength,squareLength,CV_8UC1);
                croppedXuongHand.setTo(whiteColor);
                subROI = croppedHand(Rect(tlx,tly,maskForFrame.cols,maskForFrame.rows));
                croppedFrame.copyTo(subROI,maskForFrame);
                //                subBinROI = croppedBinHand(Rect(tlx,tly,croppedBinMat.cols,croppedBinMat.rows));
                //                croppedBinMat.copyTo(subBinROI);

                // --- Moi them vao ---
                Mat src_gray;
                cvtColor( croppedHand, src_gray, CV_BGR2GRAY );
                blur( src_gray, src_gray, Size(3,3) );
                adaptiveThreshold(src_gray,croppedBinHand,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,9,5);
                Mat element = getStructuringElement(MORPH_ELLIPSE,
                                                    Size(3, 3),Point(1,1));
                erode(croppedBinHand, croppedBinHand, element);
                // --- End ---

                subInnerROI = croppedInnerHand(Rect(tlx,tly,maskForInner.cols,maskForInner.rows));
                maskForInner.copyTo(subInnerROI);
                subXuongROI = croppedXuongHand(Rect(tlx,tly,maskForXuong.cols,maskForXuong.rows));
                maskForXuong.copyTo(subXuongROI);
                if (croppedHand.cols > 96) {
                    cv::resize(croppedHand, croppedHand, cropImageSize, 0, 0, CV_INTER_AREA);
                    cv::resize(croppedBinHand, croppedBinHand, cropImageSize, 0, 0, CV_INTER_AREA);
                    cv::resize(croppedInnerHand, croppedInnerHand, cropImageSize, 0, 0, CV_INTER_AREA);
                    cv::resize(croppedXuongHand, croppedXuongHand, cropImageSize, 0, 0, CV_INTER_AREA);
                } else if (croppedHand.cols < 96) {
                    cv::resize(croppedHand, croppedHand, cropImageSize, 0, 0, CV_INTER_LINEAR);
                    cv::resize(croppedBinHand, croppedBinHand, cropImageSize, 0, 0, CV_INTER_LINEAR);
                    cv::resize(croppedInnerHand, croppedInnerHand, cropImageSize, 0, 0, CV_INTER_LINEAR);
                    cv::resize(croppedXuongHand, croppedXuongHand, cropImageSize, 0, 0, CV_INTER_LINEAR);
                }
                QString str = "";
                int currentFeatures = -1;
                str += handGesture->getHeightFeatures(currentFeatures) + " ";
                double total =  (double) (croppedBinHand.total() - countNonZero(croppedBinHand));
                str += getFeature1of3VerticalAreas(croppedBinHand,currentFeatures,total);
                str += getFeature1of3HorizontalAreas(croppedBinHand,currentFeatures,total);
                str += getFeatures1of2VerticalAndHorizontalAreas(croppedBinHand,currentFeatures,total);
                str += getFeature4CornerAreas(croppedBinHand,currentFeatures,total);
                str += getFeatures4x4(croppedBinHand,currentFeatures,total);
                str += handGesture->getRadiusFeatures(currentFeatures) + " ";
                str += handGesture->getAngleFeatures(currentFeatures)+ " ";
                total =  (double) (croppedInnerHand.total() - countNonZero(croppedInnerHand));
                str += getFeatures3x3(croppedInnerHand,currentFeatures,total);
                str += handGesture->getLinesFeatures(currentFeatures) + " ";
                total =  (double) (croppedXuongHand.total() - countNonZero(croppedXuongHand));
                str += getFeatures4x4(croppedXuongHand,currentFeatures,total);
                emit sendingCroppedImage(croppedHand.clone(),croppedBinHand.clone(),croppedInnerHand.clone(),croppedXuongHand.clone(),str);
            }
            enableToCrop = false;
        }
    }
}


// get feature 1 to 3
QString CroppingImage::getFeature1of3VerticalAreas(Mat image, int &currentFeatures, double total) {
    int step = 3;
    int range = 96 / step;
    //    if (total <= 0) {
    //        total =  (double) (image.total() - countNonZero(image));
    //    }
    QString str = "";
    for (int position = 0; position < 96; position += range) {
        Rect rect(0, position, 96, range);
        Mat subMat = image(rect);
        double feature = (double) (subMat.total()-countNonZero(subMat))
                / total;
        currentFeatures++;
        str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    }
    return str;
}

// get feature 4 to 6
QString CroppingImage::getFeature1of3HorizontalAreas(Mat image, int &currentFeatures, double total) {
    int step = 3;
    int range = 96 / step;
    //    if (total <= 0) {
    //        total =  (double) (image.total() - countNonZero(image));
    //    }
    QString str = "";
    for (int position = 0; position < 96; position += range) {
        Rect rect(position, 0, range, 96);
        Mat subMat = image(rect);

        double feature = (double) (subMat.total()-countNonZero(subMat))
                / total;
        currentFeatures++;
        str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    }
    return str;
}

// get feature 7 to 10
QString CroppingImage::getFeatures1of2VerticalAndHorizontalAreas(Mat image, int &currentFeatures, double total) {
    int halfWidth = 96 / 2;
    int halfHeight = 96 / 2;
    Mat subImage;
    //    if (total <= 0) {
    //        total =  (double) (image.total() - countNonZero(image));
    //    }
    QString str = "";
    Rect rect1(0, 0, halfWidth, halfHeight);
    subImage = image(rect1);
    double feature = (double) (subImage.total()-countNonZero(subImage))
            / total;
    currentFeatures++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    Rect rect2(halfWidth, 0, halfWidth, halfHeight);
    subImage = image(rect2);
    feature = (double) (subImage.total()-countNonZero(subImage)) / total;
    currentFeatures++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    Rect rect3(0, halfHeight, halfWidth, halfHeight);
    subImage = image(rect3);
    feature = (double) (subImage.total()-countNonZero(subImage))  / total;
    currentFeatures++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    Rect rect4(halfWidth, halfHeight, halfWidth, halfHeight);
    subImage = image(rect4);
    feature = (double) (subImage.total()-countNonZero(subImage)) / total;
    currentFeatures++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    return str;
}

// get feature 11 to 14
QString CroppingImage::getFeature4CornerAreas(Mat image, int &currentFeatures, double total) {
    double TOTAL_CORNER_PIXELS = 96 * 96 / 2 + (96 / 2);
    int traversalWidth = 96;
    double numberBlackPixels = 0;
    double numberBlackPixels1 = 0;
    QString str = "";
    //    if (total <= 0) {
    //        total =  (double) (image.total() - countNonZero(image));
    //    }
    //    cout << "total:" << total;
    for (int row = 0; row < 96; row++) {
        for (int col = 0; col < traversalWidth; col++) {
            if (image.at<uchar>(row, col) != 255) {
                numberBlackPixels++;
            }
        }
        for (int col = 95; col >= (traversalWidth - 1); col--) {
            if (image.at<uchar>(row, col) != 255) {
                numberBlackPixels1++;
            }
        }
        traversalWidth--;
    }
    double feature = (TOTAL_CORNER_PIXELS - numberBlackPixels) / total;
    currentFeatures ++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    feature = (TOTAL_CORNER_PIXELS - numberBlackPixels1) / total;
    currentFeatures ++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    // -- ---
    traversalWidth = 0;
    numberBlackPixels = 0;
    numberBlackPixels1 = 0;
    for (int row = 0; row < 96; row++) {
        for (int col = 0; col <= traversalWidth; col++) {
            if (image.at<uchar>(row, col) != 255) {
                numberBlackPixels++;
            }
        }
        for (int col = 95; col >= traversalWidth; col--) {
            if (image.at<uchar>(row, col) != 255) {
                numberBlackPixels1++;
            }
        }
        traversalWidth++;
    }
    feature = (TOTAL_CORNER_PIXELS - numberBlackPixels)  / total;
    currentFeatures ++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    feature =  (TOTAL_CORNER_PIXELS - numberBlackPixels1) / total;
    currentFeatures ++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    return str;
}

QString CroppingImage::getFeatures3x3(Mat image, int& currentFeatures, double total) {
    int halfWidth = 96 / 3;
    int halfHeight = 96 / 3;
    Mat subImage;
    QString str = "";
    //    double total =  (double) (image.total() - countNonZero(image));
    for (int row = 0; row < 96; row += halfHeight) {
        for (int col = 0; col < 96; col += halfWidth) {
            Rect rect(row, col, halfWidth, halfHeight);
            subImage = image(rect);
            double feature = (double) (subImage.total() - countNonZero(subImage))
                    / total;
            currentFeatures++;
            //            nodeTest[currentFeatures-1].index = currentFeatures;
            //            nodeTest[currentFeatures-1].value = feature;
            str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
        }
    }
    return str;
}

QString CroppingImage::getFeatures4x4(Mat image, int &currentFeatures, double total) {
    int halfWidth = 96 / 4;
    int halfHeight = 96 / 4;
    Mat subImage;
    QString str = "";
    //    double total =  (double) (image.total() - countNonZero(image));
    for (int row = 0; row < 96; row += halfHeight) {
        for (int col = 0; col < 96; col += halfWidth) {
            Rect rect(row, col, halfWidth, halfHeight);
            subImage = image(rect);
            double feature = (double) (subImage.total() - countNonZero(subImage))
                    / total;
            currentFeatures++;
            //            nodeTest[currentFeatures-1].index = currentFeatures;
            //            nodeTest[currentFeatures-1].value = feature;
            str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
        }
    }
    return str;
}

void CroppingImage::receiveBinaryImage(Mat frame, Mat binMat) {
    if (!enableToCrop) {
        this->frame = frame;
        this->binaryMat = binMat;
        enableToCrop = true;
    }
}


