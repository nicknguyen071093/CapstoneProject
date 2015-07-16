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
    //    int centerX = 0;
    //    int centerY = 0;
    //    if (boundingRect.area() > 0) {
    //        centerX = boundingRect.x + boundingRect.width / 2;
    //        centerY = boundingRect.y + boundingRect.height / 2;
    //    }
    //    if (cMaxId == -1) {
    //        isHand = false;
    //    } else if (boundingRect.area() == 0) {
    //        isHand = false;
    //    } else if ((boundingRect.height == 0) || (boundingRect.width == 0)) {
    //        isHand = false;
    //    } else if ((centerX < img.cols / 4) || (centerX > img.cols * 3 / 4)) {
    //        isHand = false;
    //    } else {
    //        isHand = true;
    //    }
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
        } else if (boundingRect.height < 70 || boundingRect.width < 70) {
            isHand = false;
        } else {
            ratioHeightAndWidth = (double) boundingRect.height / (double) boundingRect.width;
            if (ratioHeightAndWidth > 2.4 || ratioHeightAndWidth < 0.417) {
                isHand = false;
            } else if (boundingRect.tl().x <= 10 || boundingRect.tl().y <= 10 || boundingRect.br().x >= 310 || boundingRect.br().y >= 230){
                isHand = false;
            } else {
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
}

QString HandGesture::getHeightFeatures(int& currentFeatures) {
    currentFeatures++;
    double ratio = ((double) (boundingRect.height) / (double) (boundingRect.width)) / 2;
    if (ratio > 1) {
        ratio = 1 - (ratio - 1.03);
    }
    return QString::number(currentFeatures) + ":" + QString::number(ratio);
}

QString HandGesture::getRadiusFeatures(int& currentFeatures) {
    currentFeatures++;
    double ratio;
    if (boundingRect.width > boundingRect.height) {
        ratio = (double)inCircleRadius / ((double)boundingRect.height/2);
    } else {
        ratio = (double)inCircleRadius / ((double)boundingRect.width/2);
    }
    return QString::number(currentFeatures) + ":" + QString::number(ratio);
}

QString HandGesture::getAngleFeatures(int& currentFeatures) {
    currentFeatures++;
    double ratio = ((double)correctDefects.size() / 3) / 5;
    return QString::number(currentFeatures) + ":" + QString::number(ratio);
}

QString HandGesture::getLinesFeatures(int& currentFeatures) {
    double total = nTotalLine;
    double totalLen = totalLenLine;
    QString str = "";
    if (total > 0) {
        str += QString::number(++currentFeatures) + ":" + QString::number(n0To45Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(totalLen0To45Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(n46To90Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(totalLen46To90Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(n91To135Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(totalLen91To135Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(n136To180Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(totalLen136To180Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(nN1ToN45Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(totalLenN1ToN45Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(nN46ToN90Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(totalLenN46ToN90Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(nN91ToN135Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(totalLenN91ToN135Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(nN136ToN179Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(totalLenN136ToN179Line / totalLen);
    } else {
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0 ";
        str += QString::number(++currentFeatures) + ":0";
    }
    return str;
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


//Convert the feature indicated by label to the string used in SVM input file
//string HandGesture::feature2SVMString(int label) {
//    string ret = Integer.toString(label) + " ";
//    int i;
//    for (i = 0; i < features.size(); i++) {
//        int id = i + 1;
//        ret = ret + id + ":" + features.get(i) + " ";
//    }
//    ret = ret + "\n";
//    return ret;
//}

//Extract hand features from img
string HandGesture::featureExtraction(Mat &img, int label) {
    string ret = "";
    //    if ((detectIsHand(img))) {

    //        Mat(defectPoints).copyTo(defectMat);
    // defectMat.fromList(d efectPoints);

    // List<Integer> dList = defects.toList();
    //Point contourPts[] = contours.get(cMaxId).toArray();
    Point prevDefectVec;
    int i;
    //        for (i = 0; i < defectIdAfter.size(); i++) {
    for (i = 0; i < defectIdAfter.size(); i++) {
        int curDlistId = defectIdAfter[i];
        // int curId = dList.get(curDlistId);
        //            int curId = defects[i][2];
        Point curDefectPoint = contours[cMaxId][defects[curDlistId][2]];
        Point curDefectVec;
        curDefectVec.x = curDefectPoint.x - inCircle.x;
        curDefectVec.y = curDefectPoint.y - inCircle.y;

        //            if (prevDefectVec != NULL) {
        double dotProduct = curDefectVec.x * prevDefectVec.x
                + curDefectVec.y * prevDefectVec.y;
        double crossProduct = (double) curDefectVec.x * (double) prevDefectVec.y
                - (double) prevDefectVec.x * (double) curDefectVec.y;

        if (crossProduct <= 0) {
            break;
        }
        //          }


        prevDefectVec = curDefectVec;

    }

    int startId = i;
    int countId = 0;

    //ArrayList<Point> finTipsTemp = new ArrayList<Point>();
    vector<Point> finTipsTemp;
    //        if (defectIdAfter.size() > 0) {
    if (defectIdAfter.size() > 0) {
        bool end = false;

        for (int j = startId;; j++) {
            //     if (j == defectIdAfter.size()) {
            if (j == defectIdAfter.size()) {
                if (end == false) {
                    j = 0;
                    end = true;
                } else {
                    break;
                }
            }



            if ((j == startId) && (end == true)) {
                break;
            }

            int curDlistId = defectIdAfter[j];
            //                int curId = dList.get(curDlistId);
            //int curDlistId = defects[j];
            //                int curId = defects[j][2];

            //   Point curDefectPoint = contourPts[curId];
            Point curDefectPoint = contours[cMaxId][defects[curDlistId][2]];
            Point fin0 = contours[cMaxId][defects[curDlistId][0]];
            Point fin1 = contours[cMaxId][defects[curDlistId][1]];
            //                Point fin0 = contourPts[dList.get(curDlistId - 2)];
            //                Point fin1 = contourPts[dList.get(curDlistId - 1)];
            //                finTipsTemp.add(fin0);
            //                finTipsTemp.add(fin1);
            finTipsTemp.push_back(fin0);
            finTipsTemp.push_back(fin1);
            //Valid defect point is stored in curDefectPoint
            circle(img, curDefectPoint, 2, Scalar(0, 0, 255), -5);

            countId++;
        }

    }

    int count = 0;
    features.clear();
    for (int fid = 0; fid < finTipsTemp.size();) {
        if (count > 5) {
            break;
        }

        Point curFinPoint = finTipsTemp[fid];

        if ((fid % 2 == 0)) {

            if (fid != 0) {
                //  Point prevFinPoint = finTipsTemp.get(fid - 1);
                Point prevFinPoint = finTipsTemp[fid - 1];
                curFinPoint.x = (curFinPoint.x + prevFinPoint.x) / 2;
                curFinPoint.y = (curFinPoint.y + prevFinPoint.y) / 2;
            }


            if (fid == (finTipsTemp.size() - 2)) {
                fid++;
            } else {
                fid += 2;
            }
        } else {
            fid++;
        }


        Point disFinger(curFinPoint.x - inCircle.x, curFinPoint.y - inCircle.y);
        double dis = sqrt(disFinger.x * disFinger.x + disFinger.y * disFinger.y);
        //            Double f1 = (disFinger.x) / inCircleRadius;
        //            Double f2 = (disFinger.y) / inCircleRadius;
        double f1 = (disFinger.x) / inCircleRadius;
        double f2 = (disFinger.y) / inCircleRadius;
        //            features.add(f1);
        //            features.add(f2);
        features.push_back(f1);
        features.push_back(f2);

        //curFinPoint stores the location of the finger tip
        line(img, inCircle, curFinPoint, Scalar(24, 77, 9), 2);
        circle(img, curFinPoint, 2, Scalar::all(0), -5);

        putText(img, NumberToString(count), Point(curFinPoint.x - 10, curFinPoint.y - 10),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar::all(0));

        count++;

    }
    //ret = feature2SVMString(label);
    //    }

    return ret;
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



