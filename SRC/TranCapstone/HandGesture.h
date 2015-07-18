#ifndef HANDGESTURE_H
#define HANDGESTURE_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "vector"
#include <iostream>
#include <string.h>
#include <cstring>
#include <QString>
#include <map>
#include <utility>

using namespace std;
using namespace cv;


class HandGesture {
private:
    //
    double nTotalLine;
    double totalLenLine;
    double n0To45Line;
    double totalLen0To45Line;
    double n46To90Line;
    double totalLen46To90Line;
    double n91To135Line;
    double totalLen91To135Line;
    double n136To180Line;
    double totalLen136To180Line;
    double nN1ToN45Line;
    double totalLenN1ToN45Line;
    double nN46ToN90Line;
    double totalLenN46ToN90Line;
    double nN91ToN135Line;
    double totalLenN91ToN135Line;
    double nN136ToN179Line;
    double totalLenN136ToN179Line;
    // Color White
    Scalar whiteColor;
    // Color Black
    Scalar blackColor;

public:
    HandGesture();
    //public List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
    vector<vector<Point> > contours;
    vector<Point > biggestApporxContours;
    //public int cMaxId = -1;
    int cMaxId;
    // public Mat hie = new Mat();
    Mat hie;
    //public List<MatOfPoint> hullP = new ArrayList<MatOfPoint>();
    vector<vector<Point> > hullP;
    //public MatOfInt hullI = new MatOfInt();
    vector<int> hullI;
    //public Rect boundingRect;
    Rect boundingRect;
    // public MatOfInt4 defects = new MatOfInt4();
    vector<Vec4i> defects;
    //
    vector<Point> correctDefects;
    vector<Point> defectPoints;
    vector<Point2f> approxContour;
    Point inCircle;
    double inCircleRadius;
    //
    double ratioHeightAndWidth;
    bool isHand;
    // find out the biggest contour in image
    void findBiggestContour();
    // check whether the biggest contour is hand
    bool detectIsHand(Mat) ;
    // Find the location of inscribed circle and return the radius and the center location
    void findInscribedCircle() ;
    // draw Inner and Xuong
    void drawInnerAndXuong(Mat& ,Mat& );
    //
    QString getHeightFeatures(int&);
    //
    QString getRadiusFeatures(int&);
    //
    QString getAngleFeatures(int&);
    //
    QString getLinesFeatures(int&);
    //
    void classifyLine(Point, Point);

};

#endif // HANDGESTURE_H
