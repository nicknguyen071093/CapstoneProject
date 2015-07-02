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

using namespace std;
using namespace cv;

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

void exit_input_error(int line_num)
{

    fprintf(stderr,"Wrong input format at line %d\n", line_num);
    exit(1);
}

void read_problem(const char *filename);
void getFeatureOfImage(string);
void createFileTrain();
void trainModel();
void trainTest();
double predictTestImage();

const int imageWidth = 60;
const int imageHeight = 60;
const int numberFeatures = 30;
const string databaseFolderPath = "/home/nickseven/Database/Main";
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
int cross_validation;
int nr_fold;

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

int main()
{
    //espeak_Initialize()
    Mat image = imread("/home/nickseven/Database/Database/A/cholong-1.png");
    Mat gray, edge, contrastImage;
    Mat resizedImage;
//    resize(image, resizedImage, Size(100,100), 0, 0, CV_INTER_AREA);
    //    image.convertTo(contrastImage,-1,1.5,0);
    cvtColor(image, gray, CV_BGR2GRAY);
    //   GaussianBlur(gray,edge,Size(3,3),2);

//    blur (gray,edge,Size(3,3));
//    imshow("xam,",edge);
    Mat binary;
    //        Canny(edge,edge,50,150,3);
    //dst = Scalar::all(0);
    //image.copyTo(dst,edge);
    threshold( gray, binary, 140, 255,0 );
//    adaptiveThreshold(edge,edge,255,ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,9,5);
    Mat element = getStructuringElement( MORPH_CROSS,
                                         Size( 1*1 + 1, 1*1+1 ),
                                         Point( 1, 1 ) );
    //    morphologyEx(edge,edge,MORPH_CLOSE,element);
    /// Apply the erosion operation
    //    dilate(edge, binary, element);
    //    cvtColor(image, image, CV_BGR2GRAY);


    //    Mat binary;
    ////    threshold(image, binary, 254, 255, CV_THRESH_BINARY);
    //     adaptiveThreshold(image,binary,255,ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,9,5);
    //    //  imshow("display",binary);
    //    Size size(imageHeight, imageWidth);
    //    Mat resizedImage;
    //    resize(binary, resizedImage, size, 0, 0, CV_INTER_AREA);
    imshow("Thanh cong nha",binary);
    //    imshow("Contrast",contrastImage);
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

    createFileTrain();

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
        for (int col = 59; col >= (traversalWidth - 1); col--) {
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
        for (int col = 59; col >= traversalWidth; col--) {
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
    cvtColor(image, gray, CV_BGR2GRAY);
    GaussianBlur(gray,edge,Size(3,3),2);

    //blur (gray,edge,Size(3,3));
    Mat binary;
    //    Canny(edge,edge,40,120,3);
    //dst = Scalar::all(0);
    //image.copyTo(dst,edge);
    adaptiveThreshold(edge,edge,255,ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,9,5);
    Mat element = getStructuringElement( MORPH_CROSS,
                                         Size( 1*1 + 1, 1*1+1 ),
                                         Point( 1, 1 ) );
    /// Apply the erosion operation
    erode(edge, binary, element );
    //    cvtColor(image, image, CV_BGR2GRAY);


    //    Mat binary;
    ////    threshold(image, binary, 254, 255, CV_THRESH_BINARY);
    //     adaptiveThreshold(image,binary,255,ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,9,5);
    //    //  imshow("display",binary);
    Size size(imageHeight, imageWidth);
    Mat resizedImage;
    resize(binary, resizedImage, size, 0, 0, CV_INTER_AREA);
    // imshow("xem",resizedImage);
    //    Mat element = getStructuringElement( MORPH_CROSS,
    //                                         Size( 2*1 + 1, 2*1+1 ),
    //                                         Point( 1, 1 ) );
    //    //Mat src;
    //    /// Apply the erosion operation
    //    //erode(image, src, element );
    //    //imshow("display",binary);
    getFeature1To3(resizedImage,currentFeatures);
    getFeature4To6(resizedImage, currentFeatures);
    getFeature7To10(resizedImage, currentFeatures);
    getFeature11To14(resizedImage, currentFeatures);
    getFeature15To30(resizedImage, currentFeatures);
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
