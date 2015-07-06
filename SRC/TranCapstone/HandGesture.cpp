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
}

void HandGesture::findBiggestContour() {
    int idx = -1;
    int cNum = 0;

    for (int i = 0; i < contours.size(); i++) {
        //  int curNum = contours.get(i).toList().size();
        int curNum = contours[i].size();
        if (curNum > cNum) {
            idx = i;
            cNum = curNum;
        }
    }

    cMaxId = idx;
}

bool HandGesture::detectIsHand(Mat img) {
    int centerX = 0;
    int centerY = 0;
    if (boundingRect.area() > 0) {
        centerX = boundingRect.x + boundingRect.width / 2;
        centerY = boundingRect.y + boundingRect.height / 2;
    }
    if (cMaxId == -1) {
        isHand = false;
    } else if (boundingRect.area() == 0) {
        isHand = false;
    } else if ((boundingRect.height == 0) || (boundingRect.width == 0)) {
        isHand = false;
    } else if ((centerX < img.cols / 4) || (centerX > img.cols * 3 / 4)) {
        isHand = false;
    } else {
        isHand = true;
    }
    return isHand;
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
    if ((detectIsHand(img))) {

        Mat(defectPoints).copyTo(defectMat);
        // defectMat.fromList(defectPoints);

        // List<Integer> dList = defects.toList();
        //Point contourPts[] = contours.get(cMaxId).toArray();
        Point prevDefectVec;
        int i;
        //        for (i = 0; i < defectIdAfter.size(); i++) {
        for (i = 0; i < defects.size(); i++) {
            // int curDlistId = defectIdAfter.get(i);
            // int curId = dList.get(curDlistId);
            int curId = defects[i][2];
            Point curDefectPoint = contours[cMaxId][curId];
            Point curDefectVec;
            curDefectVec.x = curDefectPoint.x - inCircle.x;
            curDefectVec.y = curDefectPoint.y - inCircle.y;

            //            if (prevDefectVec != NULL) {
            double dotProduct = curDefectVec.x * prevDefectVec.x
                    + curDefectVec.y * prevDefectVec.y;
            double crossProduct = curDefectVec.x * prevDefectVec.y
                    - prevDefectVec.x * curDefectVec.y;

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
        if (defects.size() > 0) {
            bool end = false;

            for (int j = startId;; j++) {
                //     if (j == defectIdAfter.size()) {
                if (j == defects.size()) {
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

                //                int curDlistId = defectIdAfter.get(j);
                //                int curId = dList.get(curDlistId);
                //int curDlistId = defects[j];
                int curId = defects[j][2];

                //   Point curDefectPoint = contourPts[curId];
                Point curDefectPoint = contours[cMaxId][curId];
                Point fin0 = contours[cMaxId][defects[j][0]];
                Point fin1 = contours[cMaxId][defects[j][1]];
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


    }

    return ret;
}



// Find the location of inscribed circle and return the radius and the center location
void HandGesture::findInscribedCircle(Mat &img) {
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

    circle(img, inCircle, (int) inCircleRadius, Scalar(240, 240, 45, 0), 2);
    circle(img, inCircle, 3, Scalar::all(0), -2);
}



