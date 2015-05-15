#include <opencv2/opencv.hpp>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;

int main(int argc, char** argv ){
	int result = 1;
		string name;
		string path;
		string imgName;
		string temp;
		int i;
		while(result == 1){
				cout << right << "Enter the folder name: " << endl;
				cin >> name;

				temp= "/home/seven7up/"+name;
				struct stat st = {0};
				if (stat(temp.c_str(), &st) == -1) {
					 result = mkdir(temp.c_str(), 0777);
				}
		}
		VideoCapture cap(0);
		if(!cap.isOpened()) return -1;
		Mat img;
		namedWindow("webcam");
		while(true){
		cap >> img; // get a new frame from camera
		imshow("webcam", img);
		if(waitKey(30) == 27){
			printf("esc key is pressed by user\n");
			break;
		}
		for(i=1;i<21;i++){

			if (img.empty()){
				 printf("ERROR : Image cannot be loaded..!!\n");
				 return -1;
			}
			vector<int> compression_params;
			compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
			compression_params.push_back(98);
			string str;
			ostringstream convert;
			convert << i;
			str = convert.str();
			path = temp +"/"+str+".jpg";
			bool bSuccess = imwrite(path, img, compression_params);
			if ( !bSuccess )
				{
				   printf("ERROR : Failed to save the image \n");
				}
			}
			waitKey(100);
	}
	return 0;
}
