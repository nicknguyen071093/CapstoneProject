#include "CroppingImage.h"

CroppingImage::CroppingImage(QObject *parent) :
    QThread(parent)
{
    STOP = false;
    enableToCrop = false;
    cropImageSize = Size(96,96);
    white = Scalar(255,255,255);
    black = Scalar(0,0,0);
    handGesture = new HandGesture();
    //    mask = Mat(240, 320, CV_8UC1);
    //    maskForInner = Mat(240, 320, CV_8UC1);
    //    maskForXuong = Mat(240, 320, CV_8UC1);
    //    maskForFrame = Mat(240, 320, CV_8UC1);
}

void CroppingImage::run() {
    while(true) {
        QMutex mutex;
        mutex.lock();
        if(this->STOP) break;
        mutex.unlock();
        if (enableToCrop) {

            //            tile = ((double) ((double) (handGesture->boundingRect.height) / (double) (handGesture->boundingRect.width)) / 2);
            if (handGesture->detectIsHand(binaryMat)) {
                //                mask = Mat(240, 320, CV_8UC1);
                maskForInner = Mat(240, 320, CV_8UC1);
                maskForXuong = Mat(240, 320, CV_8UC1);
                maskForFrame = Mat(240, 320, CV_8UC1);
                //                mask.setTo(white);
                maskForInner.setTo(white);
                maskForXuong.setTo(white);
                maskForFrame.setTo(black);
                drawContours(maskForFrame, handGesture->contours, handGesture->cMaxId, Scalar(255), CV_FILLED);
                //                drawContours(mask, handGesture->contours, handGesture->cMaxId, Scalar(0,0,0), 2, 8, handGesture->hie, 0, Point());

                Point inCirlcePoint = handGesture->inCircle;
                circle(maskForInner, inCirlcePoint, (int)handGesture->inCircleRadius, Scalar(0,0,0), 2);
                circle(maskForInner, inCirlcePoint, 7, Scalar::all(0), -2);
                for (int i = 0; i < handGesture->correctDefects.size(); i+=3) {
                    Point curPoint = handGesture->correctDefects[i];
                    Point curPoint0 = handGesture->correctDefects[i+1];
                    Point curPoint1 = handGesture->correctDefects[i+2];
                    line(maskForXuong, inCirlcePoint, curPoint0, Scalar(0, 0, 0), 2);
                    line(maskForXuong, inCirlcePoint, curPoint1, Scalar(0,0,0), 2);
                    line(maskForXuong, curPoint, curPoint0, Scalar(0,0,0), 2);
                    line(maskForXuong, curPoint, curPoint1, Scalar(0,0,0), 2);
                }
                Rect rectToCrop = handGesture->boundingRect;
                maskForFrame = maskForFrame(rectToCrop);
                //                croppedBinMat = mask(rectToCrop);
                croppedFrame = frame(rectToCrop);
                maskForInner = maskForInner(rectToCrop);
                maskForXuong = maskForXuong(rectToCrop);
                if (maskForFrame.cols > maskForFrame.rows) {
                    squareLength = maskForFrame.cols + 10;
                    tly = (squareLength - maskForFrame.rows) / 2;
                    tlx =  5;
                } else {
                    squareLength = maskForFrame.rows + 10;
                    tlx = (squareLength - maskForFrame.cols) / 2;
                    tly =  5;
                }
                croppedHand = Mat(squareLength,squareLength,CV_8UC3);
                croppedHand.setTo(Scalar(0,0,0));
                croppedBinHand = Mat(squareLength,squareLength,CV_8UC1);
                croppedBinHand.setTo(Scalar(255,255,255));
                croppedInnerHand = Mat(squareLength,squareLength,CV_8UC1);
                croppedInnerHand.setTo(Scalar(255,255,255));
                croppedXuongHand = Mat(squareLength,squareLength,CV_8UC1);
                croppedXuongHand.setTo(Scalar(255,255,255));
                subROI = croppedHand(Rect(tlx,tly,maskForFrame.cols,maskForFrame.rows));
                croppedFrame.copyTo(subROI,maskForFrame);
                //                subBinROI = croppedBinHand(Rect(tlx,tly,croppedBinMat.cols,croppedBinMat.rows));
                //                croppedBinMat.copyTo(subBinROI);
                // --- Moi them vao ---
                Mat src_gray;
                cvtColor( croppedHand, src_gray, CV_BGR2GRAY );
                blur( src_gray, src_gray, Size(3,3) );
                // --- End ---
                subInnerROI = croppedInnerHand(Rect(tlx,tly,maskForInner.cols,maskForInner.rows));
                maskForInner.copyTo(subInnerROI);
                subXuongROI = croppedXuongHand(Rect(tlx,tly,maskForXuong.cols,maskForXuong.rows));
                maskForXuong.copyTo(subXuongROI);
                if (croppedHand.cols > 96) {
                    cv::resize(croppedHand, croppedHand, cropImageSize, 0, 0, CV_INTER_AREA);
                    cv::resize(croppedBinHand, croppedBinHand, cropImageSize, 0, 0, CV_INTER_AREA);
                    cv::resize(croppedInnerHand, croppedInnerHand, cropImageSize, 0, 0, CV_INTER_AREA);
                    cv::resize(croppedXuongHand, croppedXuongHand, cropImageSize, 0, 0, CV_INTER_AREA);
                } else if (croppedHand.cols < 96) {
                    cv::resize(croppedHand, croppedHand, cropImageSize, 0, 0, CV_INTER_LINEAR);
                    cv::resize(croppedBinHand, croppedBinHand, cropImageSize, 0, 0, CV_INTER_LINEAR);
                    cv::resize(croppedInnerHand, croppedInnerHand, cropImageSize, 0, 0, CV_INTER_LINEAR);
                    cv::resize(croppedXuongHand, croppedXuongHand, cropImageSize, 0, 0, CV_INTER_LINEAR);
                }
                emit sendingCroppedImage(croppedHand.clone(),croppedBinHand.clone(),croppedInnerHand.clone(),croppedXuongHand.clone(),"");
            }

            enableToCrop = false;
        }
    }
}

void CroppingImage::receiveBinaryImage(Mat frame, Mat binMat) {
    if (!enableToCrop) {
        this->frame = frame;
        this->binaryMat = binMat;
        enableToCrop = true;
    }
}
