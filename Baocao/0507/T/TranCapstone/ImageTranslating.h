#ifndef IMAGETRANSLATING_H
#define IMAGETRANSLATING_H

#include <QImage>
#include <opencv2/core/core.hpp>

QImage  cvMatToQImage( const cv::Mat &inMat );

#endif // IMAGETRANSLATING_H
