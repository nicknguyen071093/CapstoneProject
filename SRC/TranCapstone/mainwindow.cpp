#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_DECLARE_METATYPE(Mat)
Q_DECLARE_METATYPE(Rect)
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    takingPic = false;
    handThread = new HandThread(this);
    translatingThread = new HandTranslating(this);
    wordDAO = new LetterGet();
    wordMap = wordDAO->getLetter();
    translatingThread->start();
    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Rect>("Rect");
    connect(handThread,SIGNAL(handTrackingChanged(Mat)),this,SLOT(onHandTrackingChanged(Mat)));
    connect(handThread,SIGNAL(finished()),this,SLOT(onHandTrackingFinishinished()));
    connect(handThread,SIGNAL(sendingBinaryImage(Mat)),this,SLOT(onSendingBinaryImage(Mat)));
    connect(translatingThread,SIGNAL(translatingResultChanged(double)),this,SLOT(onTranslatingResultChanged(double)));
    connect(handThread,SIGNAL(binaryImageHandChanged(Mat,Mat)),this,SLOT(onBinaryImageHandChanged(Mat,Mat)));
    connect(handThread,SIGNAL(handSubtractingChanged(Mat,Mat,Rect)),this,SLOT(onHandSubtracted(Mat,Mat,Rect)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onHandTrackingFinishinished() {
    ui->lbFrame->clear();
    ui->lbSubtractedHand->clear();
    handThread->releaseAll();
}

void MainWindow::onHandTrackingChanged(Mat receivedImage) {
    QImage handImage = cvMatToQImage(receivedImage);
    if (!handImage.isNull()) {
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbFrame->setPixmap(pixmapObject.scaled(320,240,Qt::KeepAspectRatio));
        }
    }
}


void MainWindow::onSendingBinaryImage(Mat binMat) {
    handThread->handGesture->contours.clear();
    findContours(binMat.clone(), handThread->handGesture->contours, handThread->handGesture->hie,
                 RETR_EXTERNAL, CHAIN_APPROX_NONE);
    // Find biggest contour and return the index of the contour, which is
    // handGesture->cMaxId
    handThread->handGesture->findBiggestContour();
    if (handThread->handGesture->cMaxId > -1) {
        handThread->handGesture->boundingRect = boundingRect(handThread->handGesture->contours[handThread->handGesture->cMaxId]);
    }
//    Mat mask = Mat(binMat.rows,binMat.cols,CV_8UC1);
     Mat mask = Mat(binMat.rows,binMat.cols,CV_8UC3);
    mask.setTo(Scalar(255,255,255));
    drawContours(mask, handThread->handGesture->contours, handThread->handGesture->cMaxId, Scalar(0,0,0), 2, 8, handThread->handGesture->hie, 0, Point());
    Mat croppedBinMat = mask(handThread->handGesture->boundingRect);
    int squareLength;
    int tly, tlx;
    Mat croppedHand;
    if (croppedBinMat.cols > croppedBinMat.rows) {
        squareLength = croppedBinMat.cols + 10;
        tly = (squareLength - croppedBinMat.rows) / 2;
        tlx =  5;
    } else {
        squareLength = croppedBinMat.rows + 10;
        tlx = (squareLength - croppedBinMat.cols) / 2;
        tly =  5;
    }
//    croppedHand = Mat(squareLength,squareLength,CV_8UC1);
    croppedHand = Mat(squareLength,squareLength,CV_8UC3);
    croppedHand.setTo(Scalar(255,255,255));
    Mat subROI = croppedHand(Rect(tlx,tly,croppedBinMat.cols,croppedBinMat.rows));
    croppedBinMat.copyTo(subROI);
    cv::resize(croppedHand, croppedHand, Size(96,96), 0, 0, CV_INTER_AREA);
    if (translatingThread->enableToTranslate != true) {
        translatingThread->recevingImage(croppedHand.clone());
        translatingThread->enableToTranslate = true;
    }
    QImage handImage = cvMatToQImage(croppedHand);
    if (!handImage.isNull()) {
        ui->lbWarning->setText("Chup duoc roi");
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbSubtractedHand->setPixmap(pixmapObject.scaled(96,96,Qt::KeepAspectRatio));
        }
        if (takingPic) {
            QDir databaseDirectory("../Database/" + ui->txtType->text());
            if (databaseDirectory.exists()) {
                imwrite(databaseDirectory.path().toStdString() + "/" + ui->txtSTT->text().toStdString() + ".jpg",croppedHand);
            } else {
                ui->lbWarning->setText(QString::fromUtf8("Thư mục không tồn tại!"));
            }
            takingPic = false;
        }
    } else {
        ui->lbWarning->setText("null roi");
    }
}

void MainWindow::onTranslatingResultChanged(double result) {
    ui->lbResult->setText(QString::fromUtf8("Kết quả: ") + wordMap.value((int)result));
}

void MainWindow::onBinaryImageHandChanged(Mat frame, Mat binMat) {
    // Rect rect;
    handThread->handGesture->contours.clear();
    //  Mat cloneBin = binMat.clone();
    //    vector<vector<Point> > contours;
    //    Mat hie;
    findContours(binMat.clone(), handThread->handGesture->contours, handThread->handGesture->hie,
                 RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // Find biggest contour and return the index of the contour, which is
    // handGesture->cMaxId
    handThread->handGesture->findBiggestContour();
    if (handThread->handGesture->cMaxId > -1) {
        handThread->handGesture->boundingRect = boundingRect(handThread->handGesture->contours[handThread->handGesture->cMaxId]);
    }
    Mat mask = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
    drawContours(mask, handThread->handGesture->contours, handThread->handGesture->cMaxId, Scalar(255), CV_FILLED);
    // imwrite("/home/nickseven/hand-mask.png",croppedBinMat);
    //    cout << "chet htat ak" << endl;``
    Mat croppedBinMat = mask(handThread->handGesture->boundingRect);
    //    cout << "ak chua" << endl;
    // Mat croppedBinMat = binMat(handThread->handGesture->boundingRect).clone();
    Mat croppedFrame = frame(handThread->handGesture->boundingRect).clone();
    int squareLength;
    int tly, tlx;
    Mat croppedHand;
    if (croppedBinMat.cols > croppedBinMat.rows) {
        squareLength = croppedBinMat.cols + 10;
        tly = (squareLength - croppedBinMat.rows) / 2;
        tlx =  5;
    } else {
        squareLength = croppedBinMat.rows + 10;
        tlx = (squareLength - croppedBinMat.cols) / 2;
        tly =  5;
    }
    croppedHand = Mat(squareLength,squareLength,CV_8UC3);
    croppedHand.setTo(Scalar(0,255,0));
    Mat subROI = croppedHand(Rect(tlx,tly,croppedBinMat.cols,croppedBinMat.rows));
    croppedFrame.copyTo(subROI,croppedBinMat);
    cv::resize(croppedHand, croppedHand, Size(100,100), 0, 0, CV_INTER_AREA);
    QImage handImage = cvMatToQImage(croppedHand);
    if (!handImage.isNull()) {
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbSubtractedHand->setPixmap(pixmapObject.scaled(100,100,Qt::KeepAspectRatio));
        }
    } else {
        ui->lbSubtractedHand->setText("null roi");
    }
}

void MainWindow::onHandSubtracted(Mat frame, Mat binMat, Rect boundingHand) {
    Rect rectROI(boundingHand.x-2,boundingHand.y-2,boundingHand.width + 4, boundingHand.height + 4);
    Mat croppedBinMat = binMat(rectROI).clone();
    Mat croppedFrame = frame(rectROI).clone();
    Mat croppedHand;
    int squareLength;
    int tly, tlx;
    if (croppedBinMat.cols > croppedBinMat.rows) {
        squareLength = croppedBinMat.cols;
        //        int moveUpHeight = squareLength - boundingHand.height;
        tly = (squareLength - croppedBinMat.rows) / 2;
        tlx =  0;
        croppedHand = Mat(squareLength,squareLength,CV_8UC3);
        croppedHand.setTo(Scalar(0,255,0));
        Mat subROI = croppedHand(Rect(tlx,tly,squareLength,croppedBinMat.rows));
        croppedFrame.copyTo(subROI,croppedBinMat);
    } else {
        squareLength = croppedBinMat.rows;
        //        int moveLeftWidth = squareLength - boundingHand.width;
        tlx = (squareLength - croppedBinMat.cols) / 2;
        tly =  0;
        croppedHand = Mat(squareLength,squareLength,CV_8UC3);
        croppedHand.setTo(Scalar(0,255,0));
        Mat subROI = croppedHand(Rect(tlx,tly,croppedBinMat.cols,squareLength));
        croppedFrame.copyTo(subROI,croppedBinMat);
    }
    cv::resize(croppedHand, croppedHand, Size(100,100), 0, 0, CV_INTER_AREA);
    QImage handImage = cvMatToQImage(croppedHand);
    if (!handImage.isNull()) {
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbSubtractedHand->setPixmap(pixmapObject.scaled(100,100,Qt::KeepAspectRatio));
        }
    } else {
        ui->lbSubtractedHand->setText("null roi");
    }

    //    if (tlx < 0 || tly < 0 || (squareLength + tlx) > 320 || (squareLength + tly) > 240) {
    //        ui->lbWarning->setText("Please moving your hand to the middle");
    //    } else {
    //        ui->lbWarning->setText("Oke");
    //        Rect adjustHandRect = Rect(tlx,tly,squareLength,squareLength);
    //        cv::resize(Mat(frame,adjustHandRect), frame, Size(100,100), 0, 0, CV_INTER_AREA);
    //        Mat gray, edge;
    //        cvtColor(frame, gray, CV_BGR2GRAY);
    //        GaussianBlur(gray,edge,Size(3,3),2);

    //        //blur (gray,edge,Size(3,3));
    //        Mat binary;
    //        //    Canny(edge,edge,40,120,3);
    //        //dst = Scalar::all(0);
    //        //image.copyTo(dst,edge);
    //        //adaptiveThreshold(edge,edge,255,ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,9,5);
    //        adaptiveThreshold(edge,edge,255,ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,9,5);
    //        Mat element = getStructuringElement( MORPH_CROSS,
    //                                             Size( 1*1 + 1, 1*1+1 ),
    //                                             Point( 1, 1 ) );
    //        /// Apply the erosion operation
    //        erode(edge, binary, element );
    //        QImage handImage = cvMatToQImage(binary);
    //        if (!handImage.isNull()) {
    //            QPixmap pixmapObject = QPixmap::fromImage(handImage);
    //            if (!pixmapObject.isNull()) {
    //                ui->lbSubtractedHand->setPixmap(pixmapObject.scaled(100,100,Qt::KeepAspectRatio));
    //            }
    //        }
    //    }
}

void MainWindow::on_pushButton_clicked()
{
    handThread->start();
}

void MainWindow::on_pushButton_2_clicked()
{
    //handThread->getSampleBack();
    handThread->setMode(handThread->GET_AVG_BACKGROUND);
}


void MainWindow::on_pushButton_3_clicked()
{
    //handThread->getSampleHand();
    handThread->setMode(handThread->GET_AVG_HAND);
}

void MainWindow::on_btnBackHand_clicked()
{
    handThread->setMode(handThread->GET_AVG_BACK_HAND);
}

void MainWindow::on_pushButton_4_clicked()
{
}


void MainWindow::on_pushButton_5_clicked()
{
    handThread->STOP = true;
}

void MainWindow::on_MainWindow_destroyed()
{
    handThread->quit();
    //webSource.release();
}


void MainWindow::on_btnTakepic_clicked()
{
    takingPic = true;
}


