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
    signRecogntion = new SignRecognition();
    mode = 0;
}

CroppingImage::~CroppingImage() {
    free(handGesture);
    free(signRecogntion);
}

void CroppingImage::run() {
    //    vector<int> compression_params;
    //    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    //    compression_params.push_back(100);
    while(true) {
        mutex.lock();
        if(this->STOP)
        {
            free(handGesture);
            free(signRecogntion);
            break;
        }
        mutex.unlock();
        if (enableToCrop) {
            //            imwrite("../bin.jpg",binaryMat,compression_params);
            //            imwrite("../frame.jpg",frame,compression_params);
            //            tile = ((double) ((double) (handGesture->boundingRect.height) / (double) (handGesture->boundingRect.width)) / 2);
            if (mode == TESTING_MODE) {
                mutex.lock();
                testingResult = false;
                mutex.unlock();
                //                result = predictTestImage();
                recognizeSign();
//                cout << recognitionResultNumber << endl;
                if (recognitionResultNumber == 1) {
                    // If the "testing" hand sign is recognized.
                    mode = NO_MODE;
                    mutex.lock();
                    testingResult = true;
                    mutex.unlock();
                    emit sendSignalToChangeLabelTestingResult(QString::fromUtf8("Thành Công!"),"color: green");
                }
            } else if (mode == SELECTING_MODE) {
                recognizeSign();
                cout << recognitionResultNumber << endl;
                if (recognitionResultNumber == 2) {  // If the hand sign is "select"
                    selectPoint = handGesture->getSelectPoint();
                    int x = selectPoint.x;
                    int y = selectPoint.y;
                    if (y >= 15 && y <= 50) {
                        if (x >=80 && x <= 155) {
                            if (selectedFunctionNumber != RECOGNITION_FUCNTION) {//if select is !recognition,
                                //send signal changing color recognition button, select = recognition
                                selectedFunctionNumber = RECOGNITION_FUCNTION;
//                                emit sendSignalChangingRecognitionColor(SELECTED_COLOR);
                            }
                        } else if (x >= 175 && x <= 250) {
                            if (selectedFunctionNumber != LEARNING_FUNCTION) {//if select is !learning,
                                // send signal changing color learning button, select = learning
                                selectedFunctionNumber = LEARNING_FUNCTION;
//                                emit sendSignalChangingLearningColor(SELECTED_COLOR);
                            }
                        } else {
                            if (selectedFunctionNumber == RECOGNITION_FUCNTION) { //if select is recognition,
                                // send signal backing recognition button color
                                selectedFunctionNumber = NO_FUNCTION;
//                                emit sendSignalChangingLearningColor(NON_SELECTED_COLOR);
                            } else if (selectedFunctionNumber == LEARNING_FUNCTION) { // else if select is learning,
                                // send signal backing learning button color
                                selectedFunctionNumber = NO_FUNCTION;
//                                emit sendSignalChangingRecognitionColor(NON_SELECTED_COLOR);
                            }
                        }
                    } else {
                        if (selectedFunctionNumber == RECOGNITION_FUCNTION) { //if select is recognition,
                            // send signal backing recognition button color
                            selectedFunctionNumber = NO_FUNCTION;
//                            emit sendSignalChangingLearningColor(NON_SELECTED_COLOR);
                        } else if (selectedFunctionNumber == LEARNING_FUNCTION) { // else if select is learning,
                            // send signal backing learning button color
                            selectedFunctionNumber = NO_FUNCTION;
//                            emit sendSignalChangingRecognitionColor(NON_SELECTED_COLOR);
                        }
                    }
                } else if (recognitionResultNumber == 3) {// If the hand is "done"
                    selectPoint = handGesture->getSelectPoint();
                    int x = selectPoint.x;
                    int y = selectPoint.y;
                    if (y >= 15 && y <= 50) {
                        if (x >=80 && x <= 155) {
                            if (selectedFunctionNumber == RECOGNITION_FUCNTION) {//if select is recognition,
                                // Show the recognition interface
                                emit sendSignalSelectingRecognition();
                                mode = RECOGNITION_MODE;
                            }
                            selectedFunctionNumber = NO_FUNCTION;
                        } else if (x >= 175 && x <= 250) {
                            if (selectedFunctionNumber == LEARNING_FUNCTION) {//if select is learning,
                                //Show the learning interface
                                emit sendSignalSelectingLearning();
                                mode = LEARNING_MODE;
                            }
                            selectedFunctionNumber = NO_FUNCTION;
                        } else {
                            if (selectedFunctionNumber == RECOGNITION_FUCNTION) { //if select is recognition,
                                // send signal backing recognition button color
                                selectedFunctionNumber = NO_FUNCTION;
//                                emit sendSignalChangingLearningColor(NON_SELECTED_COLOR);
                            } else if (selectedFunctionNumber == LEARNING_FUNCTION) { // else if select is learning,
                                // send signal backing learning button color
                                selectedFunctionNumber = NO_FUNCTION;
//                                emit sendSignalChangingRecognitionColor(NON_SELECTED_COLOR);
                            }
                        }
                    } else {
                        if (selectedFunctionNumber == RECOGNITION_FUCNTION) { //if select is recognition,
                            // send signal backing recognition button color
                            selectedFunctionNumber = NO_FUNCTION;
//                            emit sendSignalChangingLearningColor(NON_SELECTED_COLOR);
                        } else if (selectedFunctionNumber == LEARNING_FUNCTION) { // else if select is learning,
                            // send signal backing learning button color
                            selectedFunctionNumber = NO_FUNCTION;
//                            emit sendSignalChangingRecognitionColor(NON_SELECTED_COLOR);
                        }
                    }
                } else {
                    if (selectedFunctionNumber == RECOGNITION_FUCNTION) { //if select is recognition,
                        // send signal backing recognition button color
                        selectedFunctionNumber = NO_FUNCTION;
//                        emit sendSignalChangingLearningColor(NON_SELECTED_COLOR);
                    } else if (selectedFunctionNumber == LEARNING_FUNCTION) { // else if select is learning,
                        // send signal backing learning button color
                        selectedFunctionNumber = NO_FUNCTION;
//                        emit sendSignalChangingRecognitionColor(NON_SELECTED_COLOR);
                    }
                }
            } else if (mode == RECOGNITION_MODE) {
                recognizeSign();
                // send signal edit current result
                emit sendSignalChangingRecognitionResult(recognitionResultNumber);
            } else if (mode == LEARNING_MODE) {
                recognizeSign();
//                cout << recognitionResultNumber << endl;
                if (recognitionResultNumber == 2) {  // If the hand sign is "select"
                    selectPoint = handGesture->getSelectPoint();
                    int x = selectPoint.x;
                    int y = selectPoint.y;
                    if (y >= 15 && y <= 50) {
                        if (x >=80 && x <= 155) {
                            if (selectedFunctionNumber != UP_FUCNTION) {//if select is !up,
                                //send signal changing color up button, select = up
                                selectedFunctionNumber = UP_FUCNTION;
//                                emit sendSignalChangingUpColor(SELECTED_COLOR);
                            }
                        } else if (x >= 175 && x <= 250) {
                            if (selectedFunctionNumber != DOWN_FUCNTION) {//if select is !down,
                                // send signal changing color down button, select = down
                                selectedFunctionNumber = DOWN_FUCNTION;
//                                emit sendSignalChangingDownColor(SELECTED_COLOR);
                            }
                        } else {
                            if (selectedFunctionNumber == UP_FUCNTION) { //if select is up,
                                // send signal backing up button color
                                selectedFunctionNumber = NO_FUNCTION;
//                                emit sendSignalChangingDownColor(NON_SELECTED_COLOR);
                            } else if (selectedFunctionNumber == DOWN_FUCNTION) { // else if select is down,
                                // send signal backing down button color
                                selectedFunctionNumber = NO_FUNCTION;
//                                emit sendSignalChangingUpColor(NON_SELECTED_COLOR);
                            }
                        }
                    } else {
                        if (selectedFunctionNumber == UP_FUCNTION) { //if select is up,
                            // send signal backing up button color
                            selectedFunctionNumber = NO_FUNCTION;
//                            emit sendSignalChangingDownColor(NON_SELECTED_COLOR);
                        } else if (selectedFunctionNumber == DOWN_FUCNTION) { // else if select is down,
                            // send signal backing down button color
                            selectedFunctionNumber = NO_FUNCTION;
//                            emit sendSignalChangingUpColor(NON_SELECTED_COLOR);
                        }
                    }
                } else if (recognitionResultNumber == 3) {// If the hand is "done"
                    selectPoint = handGesture->getSelectPoint();
                    int x = selectPoint.x;
                    int y = selectPoint.y;
                    if (y >= 15 && y <= 50) {
                        if (x >=80 && x <= 155) {
                            if (selectedFunctionNumber == UP_FUCNTION) {//if select is up,
                                // send signal change to upper word
                                emit sendSignalMovingToUpperWord();
                                sleep(0.3);
                            }
                        } else if (x >= 175 && x <= 250) {
                            if (selectedFunctionNumber == DOWN_FUCNTION) {//if select is down,
                                // send signal change to lower word
                                emit sendSignalMovingToLowerWord();
                                sleep(0.3);
                            }
                        } else {
                            if (selectedFunctionNumber == UP_FUCNTION) { //if select is up,
                                // send signal backing up button color
                                selectedFunctionNumber = NO_FUNCTION;
//                                emit sendSignalChangingDownColor(NON_SELECTED_COLOR);
                            } else if (selectedFunctionNumber == DOWN_FUCNTION) { // else if select is down,
                                // send signal backing down button color
                                selectedFunctionNumber = NO_FUNCTION;
//                                emit sendSignalChangingUpColor(NON_SELECTED_COLOR);
                            }
                        }
                    } else {
                        if (selectedFunctionNumber == UP_FUCNTION) { //if select is up,
                            // send signal backing up button color
                            selectedFunctionNumber = NO_FUNCTION;
//                            emit sendSignalChangingDownColor(NON_SELECTED_COLOR);
                        } else if (selectedFunctionNumber == DOWN_FUCNTION) { // else if select is down,
                            // send signal backing down button color
                            selectedFunctionNumber = NO_FUNCTION;
//                            emit sendSignalChangingUpColor(NON_SELECTED_COLOR);
                        }
                    }
                } else if (recognitionResultNumber == 5) { // hand sign is end sign
                    // send signal change to selecting mode
                    emit sendSignalChangeToSelectingMode();
                    // selected is no function
                    selectedFunctionNumber = NO_FUNCTION;
                    // change to selecting mode
                    mode = SELECTING_MODE;
                } else {
                    if (selectedFunctionNumber == UP_FUCNTION) { //if select is up,
                        // send signal backing up button color
                        selectedFunctionNumber = NO_FUNCTION;
//                        emit sendSignalChangingDownColor(NON_SELECTED_COLOR);
                    } else if (selectedFunctionNumber == DOWN_FUCNTION) { // else if select is down,
                        // send signal backing down button color
                        selectedFunctionNumber = NO_FUNCTION;
//                        emit sendSignalChangingUpColor(NON_SELECTED_COLOR);
                    }
                    // send signal changing learning result
                    emit sendSignalChangingLearningResult(recognitionResultNumber);
                }
            }
        }
        enableToCrop = false;
    }
}


void CroppingImage::recognizeSign() {
    recognitionResultNumber = 0;
    if (handGesture->detectIsHand(binaryMat)) {

        maskForInner = Mat(240, 320, CV_8UC1);
        maskForXuong = Mat(240, 320, CV_8UC1);
        maskForFrame = Mat(240, 320, CV_8UC1);
        maskForInner.setTo(whiteColor);
        maskForXuong.setTo(whiteColor);
        maskForFrame.setTo(blackColor);

        drawContours(maskForFrame, handGesture->contours, handGesture->cMaxId, whiteColor, CV_FILLED);
        handGesture->drawInnerAndXuong(maskForInner,maskForXuong);
        Rect rectToCrop = handGesture->boundingRect;
        maskForFrame = maskForFrame(rectToCrop);
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

        Mat src_gray;
        cvtColor( croppedHand, src_gray, CV_BGR2GRAY );
        blur( src_gray, src_gray, Size(3,3) );
        adaptiveThreshold(src_gray,croppedBinHand,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,9,5);
        Mat element = getStructuringElement(MORPH_ELLIPSE,
                                            Size(3, 3),Point(1,1));
        erode(croppedBinHand, croppedBinHand, element);

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
        int currentFeatures = -1;
        struct svm_node nodeFeatures[NUMBER_FEATURES];

        handGesture->getHeightFeatures(currentFeatures,nodeFeatures);

        double total =  (double) (croppedBinHand.total() - countNonZero(croppedBinHand));
        getFeature1of3VerticalAreas(croppedBinHand,currentFeatures,nodeFeatures,total);
        getFeature1of3HorizontalAreas(croppedBinHand,currentFeatures,nodeFeatures,total);
        getFeatures1of2VerticalAndHorizontalAreas(croppedBinHand,currentFeatures,nodeFeatures,total);
        getFeature4CornerAreas(croppedBinHand,currentFeatures,nodeFeatures,total);
        getFeatures4x4(croppedBinHand,currentFeatures,nodeFeatures,total);

        handGesture->getRadiusFeatures(currentFeatures,nodeFeatures);
        handGesture->getAngleFeatures(currentFeatures,nodeFeatures);

        total =  (double) (croppedInnerHand.total() - countNonZero(croppedInnerHand));
        getFeatures3x3(croppedInnerHand,currentFeatures,nodeFeatures,total);

        handGesture->getLinesFeatures(currentFeatures,nodeFeatures);

        total =  (double) (croppedXuongHand.total() - countNonZero(croppedXuongHand));
        if (total != 0) {
            getFeatures4x4(croppedXuongHand,currentFeatures,nodeFeatures,total);
        } else {
            for (int i = 0; i < 16; i++) {
                currentFeatures++;
                nodeFeatures[currentFeatures].index = (currentFeatures + 1);
                nodeFeatures[currentFeatures].value = 0;
            }
        }

        if (currentFeatures == 73) {
            nodeFeatures[74].index = -1;
            nodeFeatures[74].value = -1;
            mutex.lock();
            recognitionResultNumber = signRecogntion->getRecognitionResult(nodeFeatures);
            mutex.unlock();
            //                    recognitionResultNo =
        }
    }
}

// get feature 1 to 3
void CroppingImage::getFeature1of3VerticalAreas(Mat image, int &currentFeatures, struct svm_node nodeFeatures[],double total) {
    int step = 3;
    int range = 96 / step;
    //    if (total <= 0) {
    //        total =  (double) (image.total() - countNonZero(image));
    //    }
    for (int position = 0; position < 96; position += range) {
        Rect rect(0, position, 96, range);
        Mat subMat = image(rect);
        double feature = (double) (subMat.total()-countNonZero(subMat))
                / total;
        currentFeatures++;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = feature;
        //        str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";

    }
}

// get feature 4 to 6
void CroppingImage::getFeature1of3HorizontalAreas(Mat image, int &currentFeatures, struct svm_node nodeFeatures[], double total) {
    int step = 3;
    int range = 96 / step;
    //    if (total <= 0) {
    //        total =  (double) (image.total() - countNonZero(image));
    //    }
    for (int position = 0; position < 96; position += range) {
        Rect rect(position, 0, range, 96);
        Mat subMat = image(rect);

        double feature = (double) (subMat.total()-countNonZero(subMat))
                / total;
        currentFeatures++;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = feature;
    }
}

// get feature 7 to 10
void CroppingImage::getFeatures1of2VerticalAndHorizontalAreas(Mat image, int &currentFeatures, struct svm_node nodeFeatures[], double total) {
    int halfWidth = 96 / 2;
    int halfHeight = 96 / 2;
    Mat subImage;
    //    if (total <= 0) {
    //        total =  (double) (image.total() - countNonZero(image));
    //    }
    Rect rect1(0, 0, halfWidth, halfHeight);
    subImage = image(rect1);
    double feature = (double) (subImage.total()-countNonZero(subImage))
            / total;
    currentFeatures++;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = feature;

    Rect rect2(halfWidth, 0, halfWidth, halfHeight);
    subImage = image(rect2);
    feature = (double) (subImage.total()-countNonZero(subImage)) / total;
    currentFeatures++;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = feature;

    Rect rect3(0, halfHeight, halfWidth, halfHeight);
    subImage = image(rect3);
    feature = (double) (subImage.total()-countNonZero(subImage))  / total;
    currentFeatures++;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = feature;

    Rect rect4(halfWidth, halfHeight, halfWidth, halfHeight);
    subImage = image(rect4);
    feature = (double) (subImage.total()-countNonZero(subImage)) / total;
    currentFeatures++;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = feature;
}

// get feature 11 to 14
void CroppingImage::getFeature4CornerAreas(Mat image, int &currentFeatures, struct svm_node nodeFeatures[], double total) {
    double TOTAL_CORNER_PIXELS = 96 * 96 / 2 + (96 / 2);
    int traversalWidth = 96;
    double numberBlackPixels = 0;
    double numberBlackPixels1 = 0;
    //    if (total <= 0) {
    //        total =  (double) (image.total() - countNonZero(image));
    //    }
    //    cout << "total:" << total;
    for (int row = 0; row < 96; row++) {
        for (int col = 0; col < traversalWidth; col++) {
            if (image.at<uchar>(row, col) != 0) {
                numberBlackPixels++;
            }
        }
        for (int col = 95; col >= (traversalWidth - 1); col--) {
            if (image.at<uchar>(row, col) != 0) {
                numberBlackPixels1++;
            }
        }
        traversalWidth--;
    }

    double feature = (TOTAL_CORNER_PIXELS - numberBlackPixels) / total;
    currentFeatures ++;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = feature;

    feature = (TOTAL_CORNER_PIXELS - numberBlackPixels1) / total;
    currentFeatures ++;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = feature;
    // -- ---
    traversalWidth = 0;
    numberBlackPixels = 0;
    numberBlackPixels1 = 0;
    for (int row = 0; row < 96; row++) {
        for (int col = 0; col <= traversalWidth; col++) {
            if (image.at<uchar>(row, col) != 0) {
                numberBlackPixels++;
            }
        }
        for (int col = 95; col >= traversalWidth; col--) {
            if (image.at<uchar>(row, col) != 0) {
                numberBlackPixels1++;
            }
        }
        traversalWidth++;
    }

    feature = (TOTAL_CORNER_PIXELS - numberBlackPixels)  / total;
    currentFeatures ++;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = feature;

    feature =  (TOTAL_CORNER_PIXELS - numberBlackPixels1) / total;
    currentFeatures ++;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = feature;
}

void CroppingImage::getFeatures3x3(Mat image, int& currentFeatures, struct svm_node nodeFeatures[], double total) {
    int halfWidth = 96 / 3;
    int halfHeight = 96 / 3;
    Mat subImage;
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
            nodeFeatures[currentFeatures].index = (currentFeatures+1);
            nodeFeatures[currentFeatures].value = feature;
        }
    }
}

void CroppingImage::getFeatures4x4(Mat image, int &currentFeatures, struct svm_node nodeFeatures[], double total) {
    int halfWidth = 96 / 4;
    int halfHeight = 96 / 4;
    Mat subImage;
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
            nodeFeatures[currentFeatures].index = (currentFeatures+1);
            nodeFeatures[currentFeatures].value = feature;
        }
    }
}

void CroppingImage::receiveBinaryImage(Mat frame, Mat binMat) {
    if (!enableToCrop && mode != NO_MODE) {
        this->frame = frame;
        this->binaryMat = binMat;
        enableToCrop = true;
    }
}


void CroppingImage::changeToTestingMode() {
    mutex.lock();
    mode = TESTING_MODE;
    mutex.unlock();
}

void CroppingImage::changeToRecognitionMode() {
    mutex.lock();
    mode = RECOGNITION_MODE;
    mutex.unlock();
}

void CroppingImage::changeToSelectingFunctionMode() {
    //    mutex.lock();
    mode = SELECTING_MODE;
    //    mutex.unlock();
}

bool CroppingImage::getTestingResult() {
    mode = NO_MODE;
    return testingResult;
}

double CroppingImage::getRegResult() {
    mutex.lock();
    double result = recognitionResultNumber;
    mutex.unlock();
    return result;
}





