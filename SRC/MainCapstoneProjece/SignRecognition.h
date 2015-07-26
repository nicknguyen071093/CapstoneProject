#ifndef SIGNRECOGNITION_H
#define SIGNRECOGNITION_H
#include <SVM.h>
#include <QtCore>

#define modelFilePath "/home/nickseven/long.model"


class SignRecognition
{
public:
    SignRecognition();
    double getRecognitionResult(struct svm_node[]);

private:
    struct svm_model *model;
    double *probEstimates;
    int numberClass;

//    struct svm_node nodeTest[31];
};

#endif // SIGNRECOGNITION_H
