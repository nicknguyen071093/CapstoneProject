#ifndef HANDGESTURE_H
#define HANDGESTURE_H
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include "opencv2/opencv.hpp"
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

  public:
    HandGesture(){};
    //public List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
    vector<vector<Point> > contours;
    //public int cMaxId = -1;
    int cMaxId = -1;
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
      //public List<Double> features = new ArrayList<Double>();
      vector<double> features;
      // private boolean isHand = false;
      bool isHand = false;
      //private float[] palmCircleRadius = {0};


        void findBiggestContour() ;

        bool detectIsHand(Mat img) ;

        //Convert the feature indicated by label to the string used in SVM input file
        String feature2SVMString(int label);
        //Extract hand features from img
        String featureExtraction(Mat img, int label) ;



        // Find the location of inscribed circle and return the radius and the center location
        void findInscribedCircle(Mat img) ;


};

#endif // HANDGESTURE_H
