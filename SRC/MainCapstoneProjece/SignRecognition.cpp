#include "SignRecognition.h"

SignRecognition::SignRecognition()
{
    model=svm_load_model(modelFilePath);
    numberClass = svm_get_nr_class(model);
    probEstimates = (double *) malloc(numberClass * sizeof(double));
}

double SignRecognition::getRecognitionResult(struct svm_node nodeTest[]) {
    return svm_predict_probability(model,nodeTest,probEstimates);
}
