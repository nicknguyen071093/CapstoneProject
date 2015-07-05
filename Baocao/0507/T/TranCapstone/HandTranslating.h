#ifndef HANDTRANSLATING_H
#define HANDTRANSLATING_H

#include <QThread>
#include <QtCore>
#include <SVM.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string.h>
#include <cstring>

using namespace cv;
using namespace std;

class HandTranslating : public QThread
{
    Q_OBJECT
public:
    explicit HandTranslating(QObject *parent = 0);
    void recevingImage(Mat);
    bool STOP;
    bool enableToTranslate;

private:
    void run();
    Mat binaryMat;
    const static int imageWidth = 96;
    const static int imageHeight = 96;
    const string databaseFolderPath = "../Database/Main";
    const string modelFile = "databaseMain.model";
    int svm_type;
    int nr_class;
    int *labels;
    double *prob_estimates;
    struct svm_model *model;
    struct svm_node nodeTest[31];
    void getFeature1To3(int &);
    void getFeature4To6(int &);
    void getFeature7To10(int &);
    void getFeature11To14(int &);
    void getFeature15To30(int &);
    double predictTestImage();

signals:
    void translatingResultChanged(double);

public slots:

};

#endif // HANDTRANSLATING_H
