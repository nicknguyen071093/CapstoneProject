#include "SignRecognition.h"


SignRecognition::SignRecognition()
{
    model = svm_load_model(modelFilePath);
    int nr_class=svm_get_nr_class(model);
    probEstimates = (double *) malloc(nr_class*sizeof(double));
    //    probEstimates =
}

double SignRecognition::getRecognitionResult(struct svm_node *nodeFeatures) {
    return svm_predict_probability(model,nodeFeatures,probEstimates);
}
