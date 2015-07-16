#ifndef HANDGESTURE_H
#define HANDGESTURE_H

#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "vector"
#include <iostream>
#include <string.h>
#include <cstring>
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
    //public ArrayList<Integer> defectIdAfter = new ArrayList<Integer>();
    vector<int> defectIdAfter;
    //public List<Point> fingerTips = new ArrayList<Point>();
    vector<Point> fingerTips ;
    //public List<Point> fingerTipsOrder = new ArrayList<Point>();
    vector<Point> fingerTipsOrder;
    //public Map<Double, Point> fingerTipsOrdered = new TreeMap<Double, Point>();
    map<double, Point> fingerTipsOrdered;
    //public MatOfPoint2f defectMat = new MatOfPoint2f();
    vector<Point2f> defectMat;
    //public List<Point> defectPoints = new ArrayList<Point>();
    vector<Point> defectPoints;
    //public Map<Double, Integer> defectPointsOrdered = new TreeMap<Double, Integer>();
    map<double, int> defectPointsOrdered;
    //public Point palmCenter = new Point();
    Point palmCenter;
    //public MatOfPoint2f hullCurP = new MatOfPoint2f();
    vector<Point2f> hullCurP;
    //public MatOfPoint2f approxHull = new MatOfPoint2f();
    vector<Point2f> approxHull;
    //public MatOfPoint2f approxContour = new MatOfPoint2f();
    vector<Point2f> approxContour;
    //public MatOfPoint palmDefects = new MatOfPoint();
    vector<Point> palmDefects;
    //public Point momentCenter = new Point();
    Point momentCenter;
    //public double momentTiltAngle;
    double momentTiltAngle;
    //public Point inCircle = new Point();
    Point inCircle;
    // public double inCircleRadius;
    double inCircleRadius;
    //
    double ratioHeightAndWidth;
    //public List<Double> features = new ArrayList<Double>();
    vector<double> features;
    // private boolean isHand = false;
    bool isHand;
    // find out the biggest contour in image
    void findBiggestContour();
    // check whether the biggest contour is hand
    bool detectIsHand(Mat) ;
    //Extract hand features from img
    String featureExtraction(Mat &, int) ;
    // Find the location of inscribed circle and return the radius and the center location
    void findInscribedCircle() ;


};

#endif // HANDGESTURE_H
