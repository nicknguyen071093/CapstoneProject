#include "HandGesture.h"

template <typename T>
string NumberToString(T pNumber)
{
    ostringstream oOStrStream;
    oOStrStream << pNumber;
    return oOStrStream.str();
}

HandGesture::HandGesture(){
    cMaxId = -1;
    isHand = false;
    whiteColor = Scalar(255,255,255);
    blackColor = Scalar(0,0,0);
}

void HandGesture::findBiggestContour() {
    int idx = -1;
    int cNum = 0;
    for (int i = 0; i < contours.size(); i++) {
        int curNum = contours[i].size();
        if (curNum > cNum) {
            idx = i;
            cNum = curNum;
        }
    }

    cMaxId = idx;
}

bool HandGesture::detectIsHand(Mat binaryMat) {
    isHand = true;
    contours.clear();
    findContours(binaryMat, contours, hie,
                 RETR_EXTERNAL, CHAIN_APPROX_NONE);
    findBiggestContour();
    if (cMaxId <= 0) {
        isHand = false;
    } else {
        boundingRect = cv::boundingRect(contours[cMaxId]);
        if (boundingRect.area() == 0) {
            isHand = false;
        } else if (boundingRect.height < 50 || boundingRect.width < 50) {
            isHand = false;
        } else {
            ratioHeightAndWidth = (double) boundingRect.height / (double) boundingRect.width;
            if (ratioHeightAndWidth > 2.4 || ratioHeightAndWidth < 0.417) {
                isHand = false;
            }
            else if (boundingRect.tl().x <= 10 || boundingRect.tl().y <= 10 || boundingRect.br().x >= 310 || boundingRect.br().y >= 230){
                isHand = false;
            }
            else {
                Mat(contours[cMaxId]).copyTo(approxContour);
                approxPolyDP(approxContour, approxContour, 2, true);
                Mat(approxContour).copyTo(biggestApporxContours);
                findInscribedCircle();
                if (inCircleRadius < 10) {
                    isHand = false;
                } else {
                    convexHull(biggestApporxContours, hullI, false);
                    if (biggestApporxContours.size() < 5 && hullI.size() < 5)
                    {
                        isHand = false;
                    } else {
                        defects.clear();
                        correctDefects.clear();
                        convexityDefects(biggestApporxContours, hullI, defects);

                        for (int i = 0; i < defects.size(); i++) {
                            // nhớ thêm vào điều k
                            Vec4i defect = defects[i] ;
                            double depth = (double) defect[3] / 256.0;
                            Point curPoint = biggestApporxContours[defect[2]];
                            Point curPoint0 = biggestApporxContours[defect[0]];
                            Point curPoint1 = biggestApporxContours[defect[1]];
                            Point vec0(curPoint0.x - curPoint.x,curPoint0.y - curPoint.y);
                            Point vec1(curPoint1.x - curPoint.x,curPoint1.y - curPoint.y);
                            double dot = (double) vec0.x * (double) vec1.x + (double) vec0.y * (double) vec1.y;
                            double lenth0 = sqrt((double) vec0.x * (double) vec0.x + (double) vec0.y
                                                 * (double) vec0.y);
                            double lenth1 = sqrt((double) vec1.x * (double) vec1.x + (double) vec1.y
                                                 * (double) vec1.y);
                            double cosTheta = dot / (lenth0 * lenth1);
                            //                            if ((depth > inCircleRadius * 0.6) && (cosTheta >= -0.7)) {
                            if ((depth > inCircleRadius * 0.7) && (cosTheta >= -0.7)) {
                                correctDefects.push_back(curPoint);
                                correctDefects.push_back(curPoint0);
                                correctDefects.push_back(curPoint1);
                            }
                        }
                        if ((correctDefects.size() / 3) > 4) {
                            isHand = false;
                        }
                    }
                }
            }
        }
        //            Mat(handGesture->approxContour).copyTo(handGesture->contours[handGesture->cMaxId]);
    }
    return isHand;
}

void HandGesture::drawInnerAndXuong(Mat& innerImage, Mat& xuongImage) {
    circle(innerImage, inCircle, (int)inCircleRadius, blackColor, 2);
    circle(innerImage, inCircle, 7, Scalar::all(0), -2);
    nTotalLine = 0;
    totalLenLine = 0;
    n0To45Line = 0;
    totalLen0To45Line = 0;
    n46To90Line = 0;
    totalLen46To90Line = 0;
    n91To135Line = 0;
    totalLen91To135Line = 0;
    n136To180Line = 0;
    totalLen136To180Line = 0;
    nN1ToN45Line = 0;
    totalLenN1ToN45Line = 0;
    nN46ToN90Line = 0;
    totalLenN46ToN90Line = 0;
    nN91ToN135Line = 0;
    totalLenN91ToN135Line = 0;
    nN136ToN179Line = 0;
    totalLenN136ToN179Line = 0;

    for (int i = 0; i < correctDefects.size(); i+=3) {
        Point curPoint = correctDefects[i];
        Point curPoint0 = correctDefects[i+1];
        Point curPoint1 = correctDefects[i+2];
        line(xuongImage, inCircle, curPoint0, blackColor, 2);
        line(xuongImage, inCircle, curPoint1, blackColor, 2);
        line(xuongImage, curPoint, curPoint0, blackColor, 2);
        line(xuongImage, curPoint, curPoint1, blackColor, 2);
        classifyLine(inCircle,curPoint0);
        classifyLine(inCircle,curPoint1);
        classifyLine(curPoint,curPoint0);
        classifyLine(curPoint,curPoint1);
    }
    //    cout << "------Haha-----\n" << "x:" << selectPoint.x << " y:" << selectPoint.y << "\n--------\n";
}

void HandGesture::getHeightFeatures(int& currentFeatures, struct svm_node nodeFeatures[]) {
    currentFeatures++;
    double ratio = ((double) (boundingRect.height) / (double) (boundingRect.width)) / 2;
    if (ratio > 1) {
        ratio = 1 - (ratio - 1.03);
    }
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = ratio;
//    return QString::number(currentFeatures) + ":" + QString::number(ratio);
}

void HandGesture::getRadiusFeatures(int& currentFeatures, struct svm_node nodeFeatures[]) {
    currentFeatures++;
    double ratio;
    if (boundingRect.width > boundingRect.height) {
        ratio = (double)inCircleRadius / ((double)boundingRect.height/2);
    } else {
        ratio = (double)inCircleRadius / ((double)boundingRect.width/2);
    }
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = ratio;
}

void HandGesture::getAngleFeatures(int& currentFeatures, struct svm_node nodeFeatures[]) {
    currentFeatures++;
    double ratio = ((double)correctDefects.size() / 3) / 5;
    nodeFeatures[currentFeatures].index = (currentFeatures+1);
    nodeFeatures[currentFeatures].value = ratio;
}

void HandGesture::getLinesFeatures(int& currentFeatures, struct svm_node nodeFeatures[]) {
    double total = nTotalLine;
    double totalLen = totalLenLine;
    if (total > 0) {
//        str += QString::number(++currentFeatures) + ":" + QString::number(n0To45Line / total) + " ";
        currentFeatures++;
        double ratio = n0To45Line / total;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(totalLen0To45Line / totalLen) + " ";
        currentFeatures++;
        ratio = totalLen0To45Line / totalLen;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(n46To90Line / total) + " ";
        currentFeatures++;
        ratio = n46To90Line / total;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(totalLen46To90Line / totalLen) + " ";
        currentFeatures++;
        ratio = totalLen46To90Line / totalLen;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(n91To135Line / total) + " ";
        currentFeatures++;
        ratio = n91To135Line / total;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(totalLen91To135Line / totalLen) + " ";
        currentFeatures++;
        ratio = totalLen91To135Line / totalLen;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(n136To180Line / total) + " ";
        currentFeatures++;
        ratio = n136To180Line / total;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(totalLen136To180Line / totalLen) + " ";
        currentFeatures++;
        ratio = totalLen136To180Line / totalLen;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(nN1ToN45Line / total) + " ";
        currentFeatures++;
        ratio = nN1ToN45Line / total;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(totalLenN1ToN45Line / totalLen) + " ";
        currentFeatures++;
        ratio = totalLenN1ToN45Line / totalLen;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(nN46ToN90Line / total) + " ";
        currentFeatures++;
        ratio = nN46ToN90Line / total;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(totalLenN46ToN90Line / totalLen) + " ";
        currentFeatures++;
        ratio = totalLenN46ToN90Line / totalLen;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(nN91ToN135Line / total) + " ";
        currentFeatures++;
        ratio = nN91ToN135Line / total;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(totalLenN91ToN135Line / totalLen) + " ";
        currentFeatures++;
        ratio = totalLenN91ToN135Line / totalLen;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(nN136ToN179Line / total) + " ";
        currentFeatures++;
        ratio = nN136ToN179Line / total;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
//        str += QString::number(++currentFeatures) + ":" + QString::number(totalLenN136ToN179Line / totalLen);
        currentFeatures++;
        ratio = totalLenN136ToN179Line / totalLen;
        nodeFeatures[currentFeatures].index = (currentFeatures+1);
        nodeFeatures[currentFeatures].value = ratio;
    } else {
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0 ";
//        str += QString::number(++currentFeatures) + ":0";
        for (int i = 0; i < 16;i++) {
            currentFeatures++;
            nodeFeatures[currentFeatures].index = (currentFeatures+1);
            nodeFeatures[currentFeatures].value = 0;
        }
    }
}

void HandGesture::classifyLine(Point centerPoint, Point outPoint) {
    double dx, dy;
    dx = (double)outPoint.x - (double)centerPoint.x;
    dy = (double)outPoint.y - (double)centerPoint.y;
    double angle = atan2(dy,dx) * 180.0 / CV_PI;
    Point vec0(centerPoint.x - outPoint.x,centerPoint.y - outPoint.y);
    double lenth = sqrt((double) vec0.x * (double) vec0.x + (double) vec0.y
                        * (double) vec0.y);
    if (angle >= 0 && angle <= 45) {
        n0To45Line+=1;
        totalLen0To45Line+=lenth;
    } else if (angle >= 46 && angle <= 90) {
        n46To90Line+=1;
        totalLen46To90Line+=lenth;
    } else if (angle >= 91 && angle <= 135) {
        n91To135Line+=1;
        totalLen91To135Line+=lenth;
    } else if (angle >= 136 && angle <= 180) {
        n136To180Line+=1;
        totalLen136To180Line+=lenth;
    } else if (angle >= -45 && angle <= -1) {
        nN1ToN45Line+=1;
        totalLenN1ToN45Line+=lenth;
    } else if (angle >= -90 && angle <= -46) {
        nN46ToN90Line+=1;
        totalLenN46ToN90Line+=lenth;
    } else if (angle >= -135 && angle <= -91) {
        nN91ToN135Line+=1;
        totalLenN91ToN135Line+=lenth;
    } else {
        nN136ToN179Line+=1;
        totalLenN136ToN179Line+=lenth;
    }
    totalLenLine+= lenth;
    nTotalLine+=1;
}


// Find the location of inscribed circle and return the radius and the center location
void HandGesture::findInscribedCircle() {
    //    Mat(contours[cMaxId]).copyTo(approxContour);
    //    approxPolyDP(approxContour, approxContour, 2, true);
    Point tl = boundingRect.tl();
    Point br = boundingRect.br();
    //        inCircleRadius = findInscribedCircleJNI(img.getNativeObjAddr(), tl.x, tl.y, br.x, br.y, cirx, ciry,
    //                approxContour.getNativeObjAddr());

    // MatOfPoint2f newMat = new MatOfPoint2f();
    vector<Point2f> newMat;
    //    approxContour.copyTo(newMat);
    Mat(approxContour).copyTo(newMat);
    double r = 0;
    double targetX = 0;
    double targetY = 0;
    for (int y = (int)tl.y; y < (int)br.y; y++)
    {
        for (int x = (int)tl.x; x < (int)br.x; x++)
        {
            double curDist = pointPolygonTest(newMat,Point(x, y), true);

            if (curDist > r) {
                r = curDist;
                targetX = x;
                targetY = y;
            }
        }
    }
    inCircleRadius = r;
    inCircle.x = targetX;
    inCircle.y = targetY;

    //    circle(img, inCircle, (int) inCircleRadius, Scalar(0,0,0), 2);
    //    circle(img, inCircle, 5, Scalar::all(0), -2);
}

// Find select point of the "select" hand sign
Point HandGesture::getSelectPoint() {
    Point selectPoint;
    int min = 240;
    for (int i = 0; i < hullI.size(); i++) {
        Point point = biggestApporxContours[hullI[i]];
        if (point.y < min) {
            min = point.y;
            selectPoint = point;
        }
    }
    return selectPoint;
}



