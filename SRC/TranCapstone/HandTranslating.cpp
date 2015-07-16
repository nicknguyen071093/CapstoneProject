#include "HandTranslating.h"

HandTranslating::HandTranslating(QObject *parent) :
    QThread(parent)
{
    databaseFolderPath = "../Database/Main";
    modelFile = "databaseMain.model";
    string model_file_name = databaseFolderPath + "/" + modelFile;
    if((model=svm_load_model(model_file_name.c_str()))==0)
    {
        STOP = true;
        cout << "ko mo duoc model\n";
    } else {
        svm_type = svm_get_svm_type(model);
        nr_class = svm_get_nr_class(model);
        labels=(int *) malloc(nr_class*sizeof(int));
        svm_get_labels(model,labels);
        prob_estimates = (double *) malloc(nr_class * sizeof(double));
        STOP = false;
        enableToTranslate = false;
    }
}


void HandTranslating::run() {
    while(true) {
        QMutex mutex;
        mutex.lock();
        if(this->STOP) break;
        mutex.unlock();
        if (enableToTranslate) {
            int currentFeatures = 0;
            Mat gray;
            cvtColor(binaryMat, gray, CV_BGR2GRAY);
            Mat binary;
            threshold(gray, binaryMat, 245, 255, CV_THRESH_BINARY);
            getFeature1To3(currentFeatures);
            getFeature4To6(currentFeatures);
            getFeature7To10(currentFeatures);
            getFeature11To14(currentFeatures);
            getFeature15To30(currentFeatures);
            if (currentFeatures == 30) {
                nodeTest[30].index = -1;
            }
            emit translatingResultChanged(predictTestImage());
            enableToTranslate = false;
        }
    }
}

double HandTranslating::predictTestImage() {

    double predict_label;

    printf("labels");
    for(int j = 0;j < nr_class;j++) {
        printf(" %d",labels[j]);
    }
    printf("\n");
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

void HandTranslating::getFeature1To3(int &currentFeatures) {
    int step = 3;
    int range = imageHeight / step;
    int position = 0;
    for (position = 0; position < imageHeight; position += range) {
        Rect rect(0, position, imageWidth, range);
        Mat subMat = binaryMat(rect);
        double feature = (double) (subMat.total()-countNonZero(subMat))
                / (double) subMat.total();
        currentFeatures++;
        nodeTest[currentFeatures-1].index = currentFeatures;
        nodeTest[currentFeatures-1].value = feature;
    }
}

void HandTranslating::getFeature4To6(int &currentFeatures) {
    int step = 3;
    int range = imageWidth / step;
    int position = 0;
    for (position = 0; position < imageWidth; position += range) {
        Rect rect(position, 0, range, imageHeight);
        Mat subMat = binaryMat(rect);

        double feature = (double) (subMat.total()-countNonZero(subMat))
                / (double) subMat.total();
        currentFeatures++;
        nodeTest[currentFeatures-1].index = currentFeatures;
        nodeTest[currentFeatures-1].value = feature;
    }
}

void HandTranslating::getFeature7To10(int &currentFeatures) {
    int halfWidth = imageWidth / 2;
    int halfHeight = imageHeight / 2;
    Mat subImage;
    Rect rect1(0, 0, halfWidth, halfHeight);
    subImage = binaryMat(rect1);
    double feature = (double) (subImage.total()-countNonZero(subImage))
            / (double) subImage.total();
    currentFeatures++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    Rect rect2(halfWidth, 0, halfWidth, halfHeight);
    subImage = binaryMat(rect2);
    feature = (double) (subImage.total()-countNonZero(subImage)) / (double) subImage.total();
    currentFeatures++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    Rect rect3(0, halfHeight, halfWidth, halfHeight);
    subImage = binaryMat(rect3);
    feature = (double) (subImage.total()-countNonZero(subImage))  / (double) subImage.total();
    currentFeatures++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    Rect rect4(halfWidth, halfHeight, halfWidth, halfHeight);
    subImage = binaryMat(rect4);
    feature = (double) (subImage.total()-countNonZero(subImage)) / (double) subImage.total();
    currentFeatures++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
}

void HandTranslating::getFeature11To14(int &currentFeatures) {
    double total = imageHeight * imageWidth / 2 + (imageHeight / 2);
    int traversalWidth = imageWidth;
    int numberBlackPixels = 0;
    int numberBlackPixels1 = 0;
    for (int row = 0; row < imageHeight; row++) {
        for (int col = 0; col < traversalWidth; col++) {
            if (binaryMat.at<uchar>(row, col) == 255) {
                numberBlackPixels++;
            }
        }
        for (int col = 95; col >= (traversalWidth - 1); col--) {
            if (binaryMat.at<uchar>(row, col) == 255) {
                numberBlackPixels1++;
            }
        }
        traversalWidth--;
    }
    double feature = (double) (total-  numberBlackPixels) / total;
    currentFeatures ++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    feature = (double) (total - numberBlackPixels1) / total;
    currentFeatures ++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;

    // -- ---
    traversalWidth = 0;
    numberBlackPixels = 0;
    numberBlackPixels1 = 0;
    for (int row = 0; row < imageHeight; row++) {
        for (int col = 0; col <= traversalWidth; col++) {
            if (binaryMat.at<uchar>(row, col) == 255) {
                numberBlackPixels++;
            }
        }
        for (int col = 95; col >= traversalWidth; col--) {
            if (binaryMat.at<uchar>(row, col) == 255) {
                numberBlackPixels1++;
            }
        }
        traversalWidth++;
    }
    feature = (double) (total-  numberBlackPixels) / total;
    currentFeatures ++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
    feature = (double) (total-  numberBlackPixels1) / total;
    currentFeatures ++;
    nodeTest[currentFeatures-1].index = currentFeatures;
    nodeTest[currentFeatures-1].value = feature;
}

void HandTranslating::getFeature15To30(int &currentFeatures) {
    int halfWidth = imageWidth / 4;
    int halfHeight = imageHeight / 4;
    Mat subImage;
    for (int row = 0; row < imageHeight; row += halfHeight) {
        for (int col = 0; col < imageWidth; col += halfWidth) {
            Rect rect(row, col, halfWidth, halfHeight);
            subImage = binaryMat(rect);
            double feature = (double) (subImage.total() - countNonZero(subImage))
                    / (double) subImage.total();
            currentFeatures++;
            nodeTest[currentFeatures-1].index = currentFeatures;
            nodeTest[currentFeatures-1].value = feature;
        }
    }

}

void HandTranslating::recevingImage(Mat binary) {
    this->binaryMat = binary;
}
