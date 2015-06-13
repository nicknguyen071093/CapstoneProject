/*
 * ConvertRGB2BW.cpp
 *
 *  Created on: May 14, 2015
 *      Author: binhle
 */

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

int thresh = 100;
RNG rng(12345);
int main(int argc, char **argv) {
	//Convert an RGB image im_rgb into a grayscale image

	//Mat im_rgb = imread("/home/binhle/workspace/ConvertRGB2BW/"
	//		"img/Turkish.b737-800.tc-jgp.arp.jpg");

	Mat im_rgb = imread("/home/binhle/workspace/ConvertRGB2BW/img/IMG_2382.jpg",
			1);
	//Convert an RGB image im_rgb into a grayscale image

	Mat im_gray;
	cvtColor(im_rgb, im_gray, CV_RGB2GRAY);

	/*		//Convert to binary

	 Mat img_bw1;

	 adaptiveThreshold(im_gray, img_bw1, 255, CV_ADAPTIVE_THRESH_MEAN_C,
	 CV_THRESH_BINARY, 35, 5);
	 namedWindow("MEAN_C", WINDOW_NORMAL);
	 imshow("MEAN_C", img_bw1);

	 Mat img_bw2;
	 adaptiveThreshold(im_gray, img_bw2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,
	 CV_THRESH_BINARY, 35, 5);
	 namedWindow("GAUSSIAN_C", WINDOW_NORMAL);
	 imshow("GAUSSIAN_C", img_bw2);

	 namedWindow("original", WINDOW_NORMAL);
	 imshow("original", im_rgb);

	 waitKey(0);*/

	/*		//findCoutour_demo
	 Mat canny_output;
	 vector<vector<Point> > contours;
	 vector<Vec4i> hierarchy;

	 /// Detect edges using canny
	 Canny(im_gray, canny_output, thresh, thresh * 2, 3);

	 //adaptiveThreshold(im_gray, canny_output, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,
	 //	 CV_THRESH_BINARY, 35, 5);

	 /// Find contours
	 findContours(canny_output, contours, hierarchy, CV_RETR_TREE,
	 CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	 /// Draw contours
	 Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	 for (int i = 0; i < contours.size(); i++) {
	 Scalar color = Scalar(0, 0, 255);
	 drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
	 }

	 /// Show in a window
	 namedWindow("Contours", WINDOW_NORMAL);
	 imshow("Contours", drawing);
	 waitKey(0);*/

	/*		//convexHull(
	 * Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Detect edges using Threshold
	//Canny(im_gray, threshold_output, 10, 50, 3);
	threshold(im_gray, threshold_output, thresh, 255, THRESH_BINARY);

	/// Find contours
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Find the convex hull object for each contour
	vector<Vec4i> defects;
	vector<vector<Point> > hull(contours.size());

	for (int i = 0; i < contours.size(); i++) {
		convexHull(Mat(contours[i]), hull[i], false);

	}

	/// Draw contours + hull results
	Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++) {
		Scalar color1 = Scalar(0, 0, 255);
		Scalar color2 = Scalar(0, 255, 0);
		Scalar color3 = Scalar(255, 0, 0);
		drawContours(drawing, contours, i, color1, 10, 8, vector<Vec4i>(), 0,
				Point());
		drawContours(drawing, hull, i, color2, 20, 8, vector<Vec4i>(), 0,
				Point());

	}

	/// Show in a window
	namedWindow("Hull demo", WINDOW_OPENGL);
	imshow("Hull demo", im_rgb);

	namedWindow("Hull demo1", WINDOW_OPENGL);
	imshow("Hull demo1", drawing);
	waitKey(0);*/


}

