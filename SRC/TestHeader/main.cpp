#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "speak_lib.h"
#include "SVM.h"
#include "HandGesture.h"
#include <math.h>
#include "QString"

using namespace std;
using namespace cv;

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

void exit_input_error(int line_num)
{

    fprintf(stderr,"Wrong input format at line %d\n", line_num);
    exit(1);
}

void read_problem(const char *filename);
void getFeature1To3(Mat, int &);
void getFeatureOfImage(string);
void createFileTrain();
void trainModel();
void trainTest();
double predictTestImage();

const int imageWidth = 96;
const int imageHeight = 96;
const int numberFeatures = 30;
const string databaseFolderPath = "/home/nickseven/Git/CapstoneProject/trunk/SRC/Database/Main";
const string csvFile = "csv.ext";
const string trainFile = "databaseMain.tr";
const string testFile = "databaseMain.te";
const string modelFile = "databaseMain.model";
string dataFeature;

struct svm_parameter param;		// set by parse_command_line
struct svm_problem prob;		// set by read_problem
struct svm_model *model;
struct svm_node *x_space;
struct svm_node nodeTest[31];
HandGesture *handGesture;
int cross_validation;
int nr_fold;
int testN = 0;

static char *lineDif = NULL;
static int max_line_len;


template <typename T>
string NumberToString(T pNumber)
{
    ostringstream oOStrStream;
    oOStrStream << pNumber;
    return oOStrStream.str();
}

static char* readline(FILE *input)
{
    int len;

    if(fgets(lineDif,max_line_len,input) == NULL)
        return NULL;

    while(strrchr(lineDif,'\n') == NULL)
    {
        max_line_len *= 2;
        lineDif = (char *) realloc(lineDif,max_line_len);
        len = (int) strlen(lineDif);
        if(fgets(lineDif+len,max_line_len-len,input) == NULL)
            break;
    }
    return lineDif;
}

void classifyLine(Point centerPoint, Point outPoint) {
    double dx, dy;
    dx = (double)outPoint.x - (double)centerPoint.x;
    dy = (double)outPoint.y - (double)centerPoint.y;
    double angle = atan2(dy,dx) * 180.0 / CV_PI;
    Point vec0(centerPoint.x - outPoint.x,centerPoint.y - outPoint.y);
    double lenth = sqrt((double) vec0.x * (double) vec0.x + (double) vec0.y
                        * (double) vec0.y);
    cout << angle << endl;
    cout << "le:" << lenth << endl;
    if (angle >= 0 && angle <= 45) {
        handGesture->n0To45Line+=1;
        handGesture->totalLen0To45Line+=lenth;
    } else if (angle >= 46 && angle <= 90) {
        handGesture->n46To90Line+=1;
        handGesture->totalLen46To90Line+=lenth;
    } else if (angle >= 91 && angle <= 135) {
        handGesture->n91To135Line+=1;
        handGesture->totalLen91To135Line+=lenth;
    } else if (angle >= 136 && angle <= 180) {
        handGesture->n136To180Line+=1;
        handGesture->totalLen136To180Line+=lenth;
    } else if (angle >= -45 && angle <= -1) {
        handGesture->nN1ToN45Line+=1;
        handGesture->totalLenN1ToN45Line+=lenth;
    } else if (angle >= -90 && angle <= -46) {
        handGesture->nN46ToN90Line+=1;
        handGesture->totalLenN46ToN90Line+=lenth;
    } else if (angle >= -135 && angle <= -91) {
        handGesture->nN91ToN135Line+=1;
        handGesture->totalLenN91ToN135Line+=lenth;
    } else {
        handGesture->nN136ToN179Line+=1;
        handGesture->totalLenN136ToN179Line+=lenth;
    }
    handGesture->totalLenLine+= lenth;
    handGesture->nTotalLine+=1;
}

QString getLinesFeatures(int& currentFeatures) {
    double total = handGesture->nTotalLine;
    double totalLen = handGesture->totalLenLine;
    QString str = "";
    if (total != 0) {
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->n0To45Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->totalLen0To45Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->n46To90Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->totalLen46To90Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->n91To135Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->totalLen91To135Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->n136To180Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->totalLen136To180Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->nN1ToN45Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->totalLenN1ToN45Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->nN46ToN90Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->totalLenN46ToN90Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->nN91ToN135Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->totalLenN91ToN135Line / totalLen) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->nN136ToN179Line / total) + " ";
        str += QString::number(++currentFeatures) + ":" + QString::number(handGesture->totalLenN136ToN179Line / totalLen);

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

QString getAngleFeatures(int& currentFeatures) {
    currentFeatures++;
    double ratio = ((double)handGesture->correctDefects.size() / 3) / 5;
    return QString::number(currentFeatures) + ":" + QString::number(ratio);
}


QString getHeightFeatures(int& currentFeatures) {
    currentFeatures++;
    double ratio = ((double) ((double) (handGesture->boundingRect.height) / (double) (handGesture->boundingRect.width)) / 2);
    if (ratio > 1) {
        ratio = 1 - (ratio - 1.03);
    }
    return QString::number(currentFeatures) + ":" + QString::number(ratio);
}

QString getRadiusFeatures(int& currentFeatures) {
    currentFeatures++;
    double ratio;
    if (handGesture->boundingRect.width > handGesture->boundingRect.height) {
        ratio = (double)handGesture->inCircleRadius / ((double)handGesture->boundingRect.height/2);
    } else {
        ratio = (double)handGesture->inCircleRadius / ((double)handGesture->boundingRect.width/2);
    }
    return QString::number(currentFeatures) + ":" + QString::number(ratio);
}

// get feature 1 to 3
QString getFeature1of3VerticalAreas(Mat image, int &currentFeatures) {
    int step = 3;
    int range = imageHeight / step;
    double total =  (double) (image.total() - countNonZero(image));
    QString str = "";
    for (int position = 0; position < imageHeight; position += range) {
        Rect rect(0, position, imageWidth, range);
        Mat subMat = image(rect);
        double feature = (double) (subMat.total()-countNonZero(subMat))
                / total;
        currentFeatures++;
        str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    }
    return str;
}

// get feature 4 to 6
QString getFeature1of3HorizontalAreas(Mat image, int &currentFeatures) {
    int step = 3;
    int range = imageWidth / step;
    double total =  (double) (image.total() - countNonZero(image));
    QString str = "";
    for (int position = 0; position < imageWidth; position += range) {
        Rect rect(position, 0, range, imageHeight);
        Mat subMat = image(rect);

        double feature = (double) (subMat.total()-countNonZero(subMat))
                / total;
        currentFeatures++;
        str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    }
    return str;
}

// get feature 7 to 10
QString getFeatures1of2VerticalAndHorizontalAreas(Mat image, int &currentFeatures) {
    int halfWidth = imageWidth / 2;
    int halfHeight = imageHeight / 2;
    Mat subImage;
    double total =  (double) (image.total() - countNonZero(image));
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
QString getFeature4CornerAreas(Mat image, int &currentFeatures) {
    double totalCorner = imageHeight * imageWidth / 2 + (imageHeight / 2);
    int traversalWidth = imageWidth;
    double numberBlackPixels = 0;
    double numberBlackPixels1 = 0;
    QString str = "";
    double total =  (double) (image.total() - countNonZero(image));
    for (int row = 0; row < imageHeight; row++) {
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
    double feature = (totalCorner - numberBlackPixels) / total;
    currentFeatures ++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    feature = (totalCorner- numberBlackPixels1) / total;
    currentFeatures ++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    // -- ---
    traversalWidth = 0;
    numberBlackPixels = 0;
    numberBlackPixels1 = 0;
    for (int row = 0; row < imageHeight; row++) {
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
    feature = (totalCorner - numberBlackPixels) / total;
    currentFeatures ++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    feature =  (totalCorner - numberBlackPixels1) / total;
    currentFeatures ++;
    str += QString::number(currentFeatures) + ":" + QString::number(feature) + " ";
    return str;
}

QString getFeatures3x3(Mat image, int& currentFeatures) {
    int halfWidth = imageWidth / 3;
    int halfHeight = imageHeight / 3;
    Mat subImage;
    QString str = "";
    double total =  (double) (image.total() - countNonZero(image));
    for (int row = 0; row < imageHeight; row += halfHeight) {
        for (int col = 0; col < imageWidth; col += halfWidth) {
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

QString getFeatures4x4(Mat image, int &currentFeatures) {
    int halfWidth = imageWidth / 4;
    int halfHeight = imageHeight / 4;
    Mat subImage;
    QString str = "";
    double total =  (double) (image.total() - countNonZero(image));
    for (int row = 0; row < imageHeight; row += halfHeight) {
        for (int col = 0; col < imageWidth; col += halfWidth) {
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


int main()
{
    Mat thre = imread("../Database/Moi/L/long.tr-1-bin.jpg",CV_LOAD_IMAGE_GRAYSCALE);
//    Mat src_gray;
//    cvtColor( src, src_gray, CV_BGR2GRAY );
//    blur( src_gray, src_gray, Size(3,3) );
//    Mat thresh, thre;
//    adaptiveThreshold(src_gray,thresh,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,9,5);
//    Mat element = getStructuringElement(MORPH_ELLIPSE,
//                                        Size(3, 3),Point(1,1));
//    erode(thresh, thre, element);

    handGesture = new HandGesture();
    Mat gray = imread("../Database/L/testL.jpg",CV_LOAD_IMAGE_GRAYSCALE);
    findContours(gray.clone(), handGesture->contours, handGesture->hie,
                 RETR_EXTERNAL, CHAIN_APPROX_NONE);
    Mat maskForInner = Mat(100,100,CV_8UC1);
    maskForInner.setTo(Scalar(255,255,255));
    Mat maskForXuong = Mat(100,100,CV_8UC1);
    maskForXuong.setTo(Scalar(255,255,255));
    handGesture->findBiggestContour();
    handGesture->boundingRect = cv::boundingRect(handGesture->contours[handGesture->cMaxId]);
    Mat(handGesture->contours[handGesture->cMaxId]).copyTo(handGesture->approxContour);
    approxPolyDP(handGesture->approxContour, handGesture->approxContour, 2, true);
    Mat(handGesture->approxContour).copyTo(handGesture->biggestApporxContours);
    handGesture->findInscribedCircle();
    circle(maskForInner, handGesture->inCircle, (int)handGesture->inCircleRadius, Scalar(0,0,0), 2);
    circle(maskForInner, handGesture->inCircle, 7, Scalar::all(0), -2);
    convexHull(handGesture->biggestApporxContours, handGesture->hullI, false);
    Point inCirlcePoint = handGesture->inCircle;
    if (handGesture->biggestApporxContours.size() >= 5 && handGesture->hullI.size() >= 5)
    {
        convexityDefects(handGesture->biggestApporxContours, handGesture->hullI, handGesture->defects);
        handGesture->nTotalLine = 0;
        handGesture->totalLenLine = 0;
        handGesture->n0To45Line = 0;
        handGesture->totalLen0To45Line = 0;
        handGesture->n46To90Line = 0;
        handGesture->totalLen46To90Line = 0;
        handGesture->n91To135Line = 0;
        handGesture->totalLen91To135Line = 0;
        handGesture->n136To180Line = 0;
        handGesture->totalLen136To180Line = 0;
        handGesture->nN1ToN45Line = 0;
        handGesture->totalLenN1ToN45Line = 0;
        handGesture->nN46ToN90Line = 0;
        handGesture->totalLenN46ToN90Line = 0;
        handGesture->nN91ToN135Line = 0;
        handGesture->totalLenN91ToN135Line = 0;
        handGesture->nN136ToN179Line = 0;
        handGesture->totalLenN136ToN179Line = 0;
        for (int i = 0; i < handGesture->defects.size(); i++) {
            Vec4i defect = handGesture->defects[i] ;
            double depth = (double) defect[3] / 256.0;
            Point curPoint = handGesture->biggestApporxContours[defect[2]];
            Point curPoint0 = handGesture->biggestApporxContours[defect[0]];
            Point curPoint1 = handGesture->biggestApporxContours[defect[1]];
            Point vec0(curPoint0.x - curPoint.x,curPoint0.y - curPoint.y);
            Point vec1(curPoint1.x - curPoint.x,curPoint1.y - curPoint.y);
            double dot = (double) vec0.x * (double) vec1.x + (double) vec0.y * (double) vec1.y;
            double lenth0 = sqrt((double) vec0.x * (double) vec0.x + (double) vec0.y
                                 * (double) vec0.y);
            double lenth1 = sqrt((double) vec1.x * (double) vec1.x + (double) vec1.y
                                 * (double) vec1.y);
            double cosTheta = dot / (lenth0 * lenth1);
            if ((depth > handGesture->inCircleRadius * 0.6) && (cosTheta >= -0.7)) {
                handGesture->correctDefects.push_back(curPoint);
                handGesture->correctDefects.push_back(curPoint0);
                handGesture->correctDefects.push_back(curPoint1);
                line(maskForXuong, inCirlcePoint, curPoint0, Scalar(0, 0, 0), 2);
                line(maskForXuong, inCirlcePoint, curPoint1, Scalar(0,0,0), 2);
                line(maskForXuong, curPoint, curPoint0, Scalar(0,0,0), 2);
                line(maskForXuong, curPoint, curPoint1, Scalar(0,0,0), 2);
                classifyLine(inCirlcePoint,curPoint0);
                classifyLine(inCirlcePoint,curPoint1);
                classifyLine(curPoint,curPoint0);
                classifyLine(curPoint,curPoint1);
            }
        }
        cout << "ttLi:" << handGesture->nTotalLine << " ttLe:" << handGesture->totalLenLine << endl;
    }
    int soNay = -1;
    cv::resize(maskForInner, maskForInner, Size(96,96), 0, 0, CV_INTER_AREA);
    cv::resize(maskForXuong, maskForXuong, Size(96,96), 0, 0, CV_INTER_AREA);
    cv::resize(thre, thre, Size(96,96), 0, 0, CV_INTER_AREA);
    cout << getHeightFeatures(soNay).toStdString() + " ";
    cout << getFeature1of3VerticalAreas(thre,soNay).toStdString();
    cout << getFeature1of3HorizontalAreas(thre,soNay).toStdString();
    cout << getFeatures1of2VerticalAndHorizontalAreas(thre,soNay).toStdString();
    cout << getFeature4CornerAreas(thre,soNay).toStdString();
    cout << getFeatures4x4(thre,soNay).toStdString();
    cout << getRadiusFeatures(soNay).toStdString() + " ";
    cout << getAngleFeatures(soNay).toStdString() + " ";
    cout << getFeatures3x3(maskForInner,soNay).toStdString();
    cout << getLinesFeatures(soNay).toStdString() + " ";
    cout << getFeatures4x4(maskForXuong,soNay).toStdString() << endl;
    imshow("Gray",gray);
    imshow("2",thre);
    imshow("Xuong",maskForXuong);
    imshow("Inner",maskForInner);
    waitKey(0);
    return 0;
}

void trainTest() {

    string pathTest = databaseFolderPath + "/" + testFile;
    FILE *output = fopen(pathTest.c_str(),"w");
    if(output == NULL)
    {
        printf("can't open output file %s\n");
        exit(1);
    }
    int numberTestFolder = 5;
    int numberTestImage = 6;
    int listTest[numberTestFolder + 1][numberTestFolder + 1];
    for (int i = 1; i <= numberTestFolder; i++) {
        for (int j = 1; j <= numberTestFolder;j++) {
            listTest[i][j] = 0;
        }
    }
    dataFeature = "";
    for (int i = 1;i <= numberTestFolder;i++) {
        for (int j = 1; j<= numberTestImage;j++) {
            dataFeature += NumberToString(i);
            dataFeature += " ";
            string path = "/home/nickseven/Database/T/" + NumberToString(i) + "/" + NumberToString(j) + ".png";
            getFeatureOfImage(path);
            double result = predictTestImage();
            listTest[i][(int)result] = listTest[i][(int) result] + 1;
            dataFeature += "\n";
            cout << "Ketqua: " << result << endl << endl;
        }
    }
    cout << "\nProbability: \n";
    double proba;
    for (int i = 1; i <= numberTestFolder; i++) {
        cout << "Mau : " << i << " ";
        proba = ((double) listTest[i][i] / (double) numberTestImage) * 100;
        cout << "prob:" << proba << " ";
        for (int j = 1; j <= numberTestFolder; j++) {
            proba = ((double) listTest[i][j] / (double) numberTestImage) * 100;
            cout << j << ":" << proba << " ";
        }
        cout << endl << endl;
    }
    cout << endl;
    fprintf(output,dataFeature.c_str());
    fclose(output);
}

double predictTestImage() {
    string model_file_name = databaseFolderPath + "/" + modelFile;
    if((model=svm_load_model(model_file_name.c_str()))==0)
    {
        printf("can't open model file\n");
        exit(1);
    }

    int svm_type = svm_get_svm_type(model);
    int nr_class = svm_get_nr_class(model);
    double predict_label;
    double *prob_estimates=NULL;

    int *labels=(int *) malloc(nr_class*sizeof(int));
    svm_get_labels(model,labels);
    prob_estimates = (double *) malloc(nr_class * sizeof(double));
    printf("labels");
    for(int j = 0;j < nr_class;j++) {
        printf(" %d",labels[j]);
    }
    printf("\n");
    free(labels);

    if (svm_type==C_SVC || svm_type==NU_SVC)
    {

        predict_label = svm_predict_probability(model,nodeTest,prob_estimates);
        printf("%g",predict_label);
        for(int j = 0;j < nr_class;j++)
        {
            printf(" %g",prob_estimates[j]);
        }
        printf("\n");
    }
    return predict_label;
}

void createFileTrain() {
    string databasePath = databaseFolderPath + "/" + trainFile;
    FILE *output = fopen(databasePath.c_str(),"w");
    if(output == NULL)
    {
        printf("can't open output file %s\n");
        exit(1);
    }

    dataFeature = "";
    string fullPathCSV = databaseFolderPath + "/" + csvFile;
    std::ifstream fileCsv(fullPathCSV.c_str(),ifstream::in);
    char separator = ';';
    if (!fileCsv) {
        string error_message =
                "No valid input file was given, please check the given filename.";
        cout << error_message;
        // CV_Error(CV_StsBadArg, error_message);
    } else {
        string line, path, classlabel;
        while (getline(fileCsv, line)) {
            stringstream lineStream(line);
            getline(lineStream, path, separator);
            getline(lineStream, classlabel);
            if (!path.empty() && !classlabel.empty()) {
                dataFeature += classlabel;
                dataFeature += " ";
                getFeatureOfImage(path);
                dataFeature += "\n";
            }
        }
    }
    fprintf(output,dataFeature.c_str());
    fclose(output);
}

void trainModel() {
    // default values
    param.svm_type = NU_SVC;
    param.kernel_type = RBF;
    param.degree = 3;
    param.gamma = 0;	// 1/num_features
    param.coef0 = 0;
    param.nu = 0.5;
    param.cache_size = 100;
    param.C = 1;
    param.eps = 1e-3;
    param.p = 0.1;
    param.shrinking = 1;
    param.probability = 1;
    param.nr_weight = 0;
    param.weight_label = NULL;
    param.weight = NULL;

    // createFileTrain();

    string fileProblemPath = databaseFolderPath + "/" + trainFile;
    string model_file_name = databaseFolderPath + "/" + modelFile;
    read_problem(fileProblemPath.c_str());

    //check param
    const char *error_msg;
    error_msg = svm_check_parameter(&prob,&param);
    if(error_msg)
    {
        fprintf(stderr,"ERROR: %s\n",error_msg);
        exit(1);
    }
    double *target = Malloc(double,prob.l);
    svm_cross_validation(&prob,&param,5,target);
    // get model and save model
    model = svm_train(&prob,&param);
    if(svm_save_model(model_file_name.c_str(),model))
    {
        //printf("can't save model to file %s\n", model_file_name);
        cout << "can't save model";
        exit(1);
    }

    // fre memory
    svm_free_and_destroy_model(&model);
    free(prob.y);
    free(prob.x);
    free(x_space);
    free(lineDif);
}

// get feature 1 to 3
void getFeature1To3(Mat image, int &currentFeatures) {
    int step = 3;
    int range = imageHeight / step;
    int position = 0;
    for (position = 0; position < imageHeight; position += range) {
        Rect rect(0, position, imageWidth, range);
        Mat subMat = image(rect);
        double feature = (double) (subMat.total()-countNonZero(subMat))
                / (double) subMat.total();
        currentFeatures++;

        nodeTest[currentFeatures-1].index = currentFeatures;
        nodeTest[currentFeatures-1].value = feature;
        dataFeature += NumberToString(currentFeatures);
        dataFeature += ":";
        dataFeature += NumberToString(feature);
        dataFeature += " ";
    }
}

// get feature 4 to 6
void getFeature4To6(Mat image, int &currentFeatures) {
    int step = 3;
    int range = imageWidth / step;
    int position = 0;
    for (position = 0; position < imageWidth; position += range) {
        Rect rect(position, 0, range, imageHeight);
        Mat subMat = image(rect);

        double feature = (double) (subMat.total()-countNonZero(subMat))
                / (double) subMat.total();
        currentFeatures++;
        nodeTest[currentFeatures-1].index = currentFeatures;
        nodeTest[currentFeatures-1].value = feature;
        dataFeature += NumberToString(currentFeatures);
        dataFeature += ":";
        dataFeature += NumberToString(feature);
        dataFeature += " ";
    }
}

// get feature 7 to 10
void getFeature7To10(Mat image, int &currentFeatures) {
    int halfWidth = imageWidth / 2;
    int halfHeight = imageHeight / 2;
    Mat subImage;
    Rect rect1(0, 0, halfWidth, halfHeight);
    subImage = image(rect1);
    double feature = (double) (subImage.total()-countNonZero(subImage))
            / (double) subImage.total();
    currentFeatures++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    dataFeature += NumberToString(currentFeatures);
    dataFeature += ":";
    dataFeature += NumberToString(feature);
    dataFeature += " ";
    Rect rect2(halfWidth, 0, halfWidth, halfHeight);
    subImage = image(rect2);
    feature = (double) (subImage.total()-countNonZero(subImage)) / (double) subImage.total();
    currentFeatures++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    dataFeature += NumberToString(currentFeatures);
    dataFeature += ":";
    dataFeature += NumberToString(feature);
    dataFeature += " ";
    Rect rect3(0, halfHeight, halfWidth, halfHeight);
    subImage = image(rect3);
    feature = (double) (subImage.total()-countNonZero(subImage))  / (double) subImage.total();
    currentFeatures++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    dataFeature += NumberToString(currentFeatures);
    dataFeature += ":";
    dataFeature += NumberToString(feature);
    dataFeature += " ";
    Rect rect4(halfWidth, halfHeight, halfWidth, halfHeight);
    subImage = image(rect4);
    feature = (double) (subImage.total()-countNonZero(subImage)) / (double) subImage.total();
    currentFeatures++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    dataFeature += NumberToString(currentFeatures);
    dataFeature += ":";
    dataFeature += NumberToString(feature);
    dataFeature += " ";
}

// get feature 11 to 14
void getFeature11To14(Mat image, int &currentFeatures) {
    double total = imageHeight * imageWidth / 2 + (imageHeight / 2);
    int traversalWidth = imageWidth;
    int numberBlackPixels = 0;
    int numberBlackPixels1 = 0;
    for (int row = 0; row < imageHeight; row++) {
        for (int col = 0; col < traversalWidth; col++) {
            if (image.at<uchar>(row, col) == 255) {
                numberBlackPixels++;
            }
        }
        for (int col = 95; col >= (traversalWidth - 1); col--) {
            if (image.at<uchar>(row, col) == 255) {
                numberBlackPixels1++;
            }
        }
        traversalWidth--;
    }
    double feature = (double) (total-  numberBlackPixels) / total;
    currentFeatures ++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    dataFeature += NumberToString(currentFeatures);
    dataFeature += ":";
    dataFeature += NumberToString(feature);
    dataFeature += " ";
    feature = (double) (total - numberBlackPixels1) / total;
    currentFeatures ++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    dataFeature += NumberToString(currentFeatures);
    dataFeature += ":";
    dataFeature += NumberToString(feature);
    dataFeature += " ";

    // -- ---
    traversalWidth = 0;
    numberBlackPixels = 0;
    numberBlackPixels1 = 0;
    for (int row = 0; row < imageHeight; row++) {
        for (int col = 0; col <= traversalWidth; col++) {
            if (image.at<uchar>(row, col) == 255) {
                numberBlackPixels++;
            }
        }
        for (int col = 95; col >= traversalWidth; col--) {
            if (image.at<uchar>(row, col) == 255) {
                numberBlackPixels1++;
            }
        }
        traversalWidth++;
    }
    feature = (double) (total-  numberBlackPixels) / total;
    currentFeatures ++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    dataFeature += NumberToString(currentFeatures);
    dataFeature += ":";
    dataFeature += NumberToString(feature);
    dataFeature += " ";
    feature = (double) (total-  numberBlackPixels1) / total;
    currentFeatures ++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    dataFeature += NumberToString(currentFeatures);
    dataFeature += ":";
    dataFeature += NumberToString(feature);
    dataFeature += " ";
}

void getFeature15To30(Mat image, int &currentFeatures) {
    int halfWidth = imageWidth / 4;
    int halfHeight = imageHeight / 4;
    Mat subImage;
    for (int row = 0; row < imageHeight; row += halfHeight) {
        for (int col = 0; col < imageWidth; col += halfWidth) {
            Rect rect(row, col, halfWidth, halfHeight);
            subImage = image(rect);
            double feature = (double) (subImage.total() - countNonZero(subImage))
                    / (double) subImage.total();
            currentFeatures++;
            nodeTest[currentFeatures-1].index = currentFeatures;
            nodeTest[currentFeatures-1].value = feature;
            if (currentFeatures!=30) {
                dataFeature += NumberToString(currentFeatures);
                dataFeature += ":";
                dataFeature += NumberToString(feature);
                dataFeature += " ";
            } else {
                dataFeature += NumberToString(currentFeatures);
                dataFeature += ":";
                dataFeature += NumberToString(feature);
            }
        }
    }
}

void getFeatureOfImage(string pathImage) {
    int currentFeatures = 0;
    Mat image = imread(pathImage);
    Mat gray, edge;
    resize(image,edge,Size(imageHeight,imageWidth),0,0,CV_INTER_AREA);
    cvtColor(edge, gray, CV_BGR2GRAY);
    Mat binary;
    threshold(gray, binary, 245, 255, CV_THRESH_BINARY);
    //    testN += 1;
    //    imwrite("/home/nickseven/testing/thu.jpg",binary);
    getFeature1To3(binary, currentFeatures);
    getFeature4To6(binary, currentFeatures);
    getFeature7To10(binary, currentFeatures);
    getFeature11To14(binary, currentFeatures);
    getFeature15To30(binary, currentFeatures);
    cout << "la bao nhieu : " << currentFeatures;
    nodeTest[30].index = -1;
}

// read in a problem (in svmlight format)

void read_problem(const char *filename)
{
    int max_index, inst_max_index, i;
    size_t elements, j;
    FILE *fp = fopen(filename,"r");
    char *endptr;
    char *idx, *val, *label;

    if(fp == NULL)
    {
        fprintf(stderr,"can't open input file %s\n",filename);
        exit(1);
    }

    prob.l = 0;
    elements = 0;

    max_line_len = 1024;
    lineDif = Malloc(char,max_line_len);
    lineDif = Malloc(char,max_line_len);
    while(readline(fp)!=NULL)
    {
        char *p = strtok(lineDif," \t"); // label

        // features
        while(1)
        {
            p = strtok(NULL," \t");
            if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
                break;
            ++elements;
        }
        ++elements;
        ++prob.l;
    }
    rewind(fp);

    prob.y = Malloc(double,prob.l);
    prob.x = Malloc(struct svm_node *,prob.l);
    x_space = Malloc(struct svm_node,elements);

    max_index = 0;
    j=0;
    for(i=0;i<prob.l;i++)
    {
        inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
        readline(fp);
        prob.x[i] = &x_space[j];
        label = strtok(lineDif," \t\n");
        if(label == NULL) // empty line
            exit_input_error(i+1);

        prob.y[i] = strtod(label,&endptr);
        if(endptr == label || *endptr != '\0')
            exit_input_error(i+1);

        while(1)
        {
            idx = strtok(NULL,":");
            val = strtok(NULL," \t");

            if(val == NULL)
                break;

            errno = 0;
            x_space[j].index = (int) strtol(idx,&endptr,10);
            if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
                exit_input_error(i+1);
            else
                inst_max_index = x_space[j].index;

            errno = 0;
            x_space[j].value = strtod(val,&endptr);
            if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
                exit_input_error(i+1);

            ++j;
        }

        if(inst_max_index > max_index)
            max_index = inst_max_index;
        x_space[j++].index = -1;
    }

    //printf("soluong %d", max_index);
    if(param.gamma == 0 && max_index > 0)
        param.gamma = 1.0/max_index;
    printf("type %d pre %d ", param.kernel_type, PRECOMPUTED);
    if(param.kernel_type == PRECOMPUTED)
    {
        for(i=0;i<prob.l;i++)
        {
            if (prob.x[i][0].index != 0)
            {
                fprintf(stderr,"Wrong input format: first column must be 0:sample_serial_number\n");
                exit(1);
            }
            if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
            {
                fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
                exit(1);
            }
        }
    }
    fclose(fp);
}
