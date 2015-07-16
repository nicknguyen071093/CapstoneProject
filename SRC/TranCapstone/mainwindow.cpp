#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_DECLARE_METATYPE(Mat)
Q_DECLARE_METATYPE(Rect)
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    trainPath = "../Database/Main";
    file = new QFile("../Database/Main/main-csv.ext");
    if (!file->open(QIODevice::WriteOnly)) {
        cout << "Cannot open file for writing: "
             << qPrintable(file->errorString()) << std::endl;
        return;
    }
    ghiout = new QTextStream(file);
    takingPic = false;
    handThread = new HandThread(this);
    translatingThread = new HandTranslating(this);
    showingImageThread = new ShowingImageThread(this);
    retrievingFrameThread = new RetrievingFrame(this);
    croppingThread = new CroppingImage(this);
    wordDAO = new LetterGet();
    wordMap = wordDAO->getLetter();
    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Rect>("Rect");
    connect(handThread,SIGNAL(handTrackingChanged(Mat)),this,SLOT(onHandTrackingChanged(Mat)));
    connect(handThread,SIGNAL(finished()),this,SLOT(onHandTrackingFinishinished()));
    connect(handThread,SIGNAL(sendingBinaryImage(Mat,Mat)),this,SLOT(onSendingBinaryImage(Mat,Mat)));
    //    connect(handThread,SIGNAL(thuPhatThoi(Mat)),showingImageThread,SLOT(receivedImage(Mat)));
    connect(retrievingFrameThread,SIGNAL(changingFrame(Mat)),showingImageThread,SLOT(onChangingImage(Mat)));
    connect(translatingThread,SIGNAL(translatingResultChanged(double)),this,SLOT(onTranslatingResultChanged(double)));
    connect(showingImageThread,SIGNAL(toShow(Mat)),this,SLOT(onToShow(Mat)));
    connect(showingImageThread,SIGNAL(sendImageToCrop(Mat,Mat)),croppingThread,SLOT(receiveBinaryImage(Mat,Mat)));
    connect(croppingThread,SIGNAL(sendingCroppedImage(Mat,Mat,Mat,Mat,QString)),this,SLOT(recceiveCroppedImage(Mat,Mat,Mat,Mat,QString)));
    takingNumber = 0;
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

void MainWindow::onToShow(Mat receivedImage) {
    QImage handImage = cvMatToQImage(receivedImage);
    if (!handImage.isNull()) {
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbFrame->setPixmap(pixmapObject.scaled(320,240,Qt::KeepAspectRatio));
        }
    }
}

int imageHeight = 96;
int imageWidth = 96;
// get feature 1 to 3
void MainWindow::getFeature1To3(Mat image) {
    image = imread("../Database/Moi/A/11-bin.jpg",CV_LOAD_IMAGE_GRAYSCALE);
    int step = 3;
    int range = imageHeight / step;
    int position = 0;
    for (position = 0; position < imageHeight; position += range) {
        Rect rect(0, position, imageWidth, range);
        Mat subMat = image(rect);
        double feature = (double) (subMat.total()-countNonZero(subMat))
                / (double) subMat.total();
        *ghiout << feature << " ";
    }
    *ghiout << endl;
}

void MainWindow::recceiveCroppedImage(Mat croppedHand, Mat croppedBinHand,Mat croppedInner, Mat croppedXuong, QString trainStr) {
    QImage handImage = cvMatToQImage(croppedHand);
    QImage handBinImage = cvMatToQImage(croppedBinHand);
    QImage innerHandImage = cvMatToQImage(croppedInner);
    QImage xuongImage = cvMatToQImage(croppedXuong);
    if (!handImage.isNull()) {
        ui->lbWarning->setText("Chup duoc roi");
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbSubtractedHand->setPixmap(pixmapObject.scaled(96,96,Qt::KeepAspectRatio));
        }
        pixmapObject = QPixmap::fromImage(handBinImage);
        if (!pixmapObject.isNull()) {
            ui->imgBin->setPixmap(pixmapObject.scaled(96,96,Qt::KeepAspectRatio));
        }
        pixmapObject = QPixmap::fromImage(innerHandImage);
        if (!pixmapObject.isNull()) {
            ui->imgInner->setPixmap(pixmapObject.scaled(96,96,Qt::KeepAspectRatio));
        }
        pixmapObject = QPixmap::fromImage(xuongImage);
        if (!pixmapObject.isNull()) {
            ui->imgXuong->setPixmap(pixmapObject.scaled(96,96,Qt::KeepAspectRatio));
        }
        //        imwrite("../xulyanh/origin-hand.jpg",croppedHand);
        //        imwrite("../xulyanh/hand-shape.jpg",croppedBinHand);
        //        imwrite("../xulyanh/inner-hand.jpg",croppedInner);
        //        imwrite("../xulyanh/xuong-hand.jpg",croppedXuong);
        if (takingPic) {
            QDir databaseDirectory("../Database/Moi/" + ui->txtType->text());
            if (databaseDirectory.exists()) {
                takingNumber++;
                imwrite(databaseDirectory.path().toStdString() + "/" + ui->txtName->text().toStdString() + "-" + QString::number(takingNumber).toStdString() + ".jpg",croppedHand,compression_params);
                imwrite(databaseDirectory.path().toStdString() + "/" + ui->txtName->text().toStdString() + "-" + QString::number(takingNumber).toStdString()  + "-bin.jpg",croppedBinHand,compression_params);
                imwrite(databaseDirectory.path().toStdString() + "/" + ui->txtName->text().toStdString() + "-" + QString::number(takingNumber).toStdString() + "-inner.jpg",croppedInner,compression_params);
                imwrite(databaseDirectory.path().toStdString() + "/" + ui->txtName->text().toStdString() + "-" + QString::number(takingNumber).toStdString()  + "-xuong.jpg",croppedXuong,compression_params);
                *ghiout << ui->txtType->text() + " " + trainStr << endl;
                if (takingNumber == 10) {
                    *ghiout << "---------------------" << endl;
                    takingNumber = 0;
                    takingPic = false;
                }
            } else {
                ui->lbWarning->setText(QString::fromUtf8("Thư mục không tồn tại!"));
            }
        }
    } else {
        ui->lbWarning->setText("null roi");
    }
}



void MainWindow::onSendingBinaryImage(Mat frame, Mat binMat) {

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
    //    Mat mask = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
    //    drawContours(mask, handThread->handGesture->contours, handThread->handGesture->cMaxId, Scalar(255), CV_FILLED);
    // imwrite("/home/nickseven/hand-mask.png",croppedBinMat);
    //    cout << "chet htat ak" << endl;
    Mat croppedBinMat = binMat(handThread->handGesture->boundingRect).clone();
    //    cout << "ak chua" << endl;
    // Mat croppedBinMat = binMat(handThread->handGesture->boundingRect).clone();
    Mat croppedFrame = frame(handThread->handGesture->boundingRect).clone();
    int squareLength;
    int tly, tlx;
    Mat croppedHand, croppedBinHand;
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
    croppedHand.setTo(Scalar(0,0,0));
    croppedBinHand = Mat(squareLength,squareLength,CV_8UC1);
    croppedBinHand.setTo(Scalar(0,0,0));
    Mat subROI = croppedHand(Rect(tlx,tly,croppedBinMat.cols,croppedBinMat.rows));
    croppedFrame.copyTo(subROI,croppedBinMat);
    Mat subBinROI = croppedBinHand(Rect(tlx,tly,croppedBinMat.cols,croppedBinMat.rows));
    croppedBinMat.copyTo(subBinROI);
    if (croppedHand.cols > 96) {
        cv::resize(croppedHand, croppedHand, Size(96,96), 0, 0, CV_INTER_AREA);
        cv::resize(croppedBinHand, croppedBinHand, Size(96,96), 0, 0, CV_INTER_AREA);
    } else if (croppedHand.cols < 96) {
        cv::resize(croppedHand, croppedHand, Size(96,96), 0, 0, CV_INTER_LINEAR);
        cv::resize(croppedBinHand, croppedBinHand, Size(96,96), 0, 0, CV_INTER_LINEAR);
    }

    //    handThread->handGesture->contours.clear();
    //    findContours(binMat.clone(), handThread->handGesture->contours, handThread->handGesture->hie,
    //                 CV_RETR_TREE, CHAIN_APPROX_NONE);
    //    // Find biggest contour and return the index of the contour, which is
    //    // handGesture->cMaxId
    //    handThread->handGesture->findBiggestContour();
    //    if (handThread->handGesture->cMaxId > -1) {
    //        handThread->handGesture->boundingRect = boundingRect(handThread->handGesture->contours[handThread->handGesture->cMaxId]);
    //    }
    //    //    Mat mask = Mat(binMat.rows,binMat.cols,CV_8UC1);
    //    Mat mask = Mat(binMat.rows,binMat.cols,CV_8UC3);
    //    mask.setTo(Scalar(255,255,255));
    //    drawContours(mask, handThread->handGesture->contours, handThread->handGesture->cMaxId, Scalar(0,0,0), 2, 8, handThread->handGesture->hie, 0, Point());

    //    // ----- Radius ----
    //    //    handThread->handGesture->findInscribedCircle(mask);
    //    // ---- End Radius ----

    //    Mat croppedBinMat = mask(handThread->handGesture->boundingRect);

    //    int squareLength;
    //    int tly, tlx;
    //    Mat croppedHand;
    //    if (croppedBinMat.cols > croppedBinMat.rows) {
    //        squareLength = croppedBinMat.cols + 10;
    //        tly = (squareLength - croppedBinMat.rows) / 2;
    //        tlx =  5;
    //    } else {
    //        squareLength = croppedBinMat.rows + 10;
    //        tlx = (squareLength - croppedBinMat.cols) / 2;
    //        tly =  5;
    //    }
    //    //    croppedHand = Mat(squareLength,squareLength,CV_8UC1);
    //    croppedHand = Mat(squareLength,squareLength,CV_8UC3);
    //    croppedHand.setTo(Scalar(255,255,255));
    //    Mat subROI = croppedHand(Rect(tlx,tly,croppedBinMat.cols,croppedBinMat.rows));
    //    croppedBinMat.copyTo(subROI);
    //    cv::resize(croppedHand, croppedHand, Size(96,96), 0, 0, CV_INTER_AREA);


    // --- Translating ---
    //    if (translatingThread->enableToTranslate != true) {
    //        translatingThread->recevingImage(croppedHand.clone());
    //        translatingThread->enableToTranslate = true;
    //    }
    // --- End Translating ---
    QImage handImage = cvMatToQImage(croppedHand);
    QImage handBinImage = cvMatToQImage(croppedBinHand);
    if (!handImage.isNull()) {
        ui->lbWarning->setText("Chup duoc roi");
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbSubtractedHand->setPixmap(pixmapObject.scaled(96,96,Qt::KeepAspectRatio));
        }
        pixmapObject = QPixmap::fromImage(handBinImage);
        if (!pixmapObject.isNull()) {
            ui->imgBin->setPixmap(pixmapObject.scaled(96,96,Qt::KeepAspectRatio));
        }
        if (takingPic) {
            QDir databaseDirectory("../Database/Moi/" + ui->txtType->text());
            if (databaseDirectory.exists()) {

                takingNumber++;
                imwrite(databaseDirectory.path().toStdString() + "/" + QString::number(takingNumber).toStdString() + ".jpg",croppedHand);
                imwrite(databaseDirectory.path().toStdString() + "/" + QString::number(takingNumber).toStdString()  + "-bin.jpg",croppedBinHand);
                if (takingNumber == 10) {
                    takingPic = false;
                }
            } else {
                ui->lbWarning->setText(QString::fromUtf8("Thư mục không tồn tại!"));
            }
        }
    } else {
        ui->lbWarning->setText("null roi");
    }
}

void MainWindow::onTranslatingResultChanged(double result) {
    ui->lbResult->setText(QString::fromUtf8("Kết quả: ") + wordMap.value((int)result));
}

void MainWindow::on_pushButton_clicked()
{
    //    handThread->start();
    retrievingFrameThread->STOP = false;
    showingImageThread->STOP = false;
    translatingThread->STOP = false;
    croppingThread->STOP = false;
    retrievingFrameThread->start();
    showingImageThread->start();
    translatingThread->start();
    croppingThread->start();

}

void MainWindow::on_pushButton_2_clicked()
{
    //handThread->getSampleBack();
    //    handThread->setMode(handThread->GET_AVG_BACKGROUND);
    showingImageThread->setMode(showingImageThread->GET_AVG_BACKGROUND);
}


void MainWindow::on_pushButton_3_clicked()
{
    //handThread->getSampleHand();
    //    handThread->setMode(handThread->GET_AVG_HAND);
    showingImageThread->setMode(showingImageThread->GET_AVG_FRONT_HAND);
}

void MainWindow::on_btnBackHand_clicked()
{
    //    handThread->setMode(handThread->GET_AVG_BACK_HAND);
    showingImageThread->setMode(showingImageThread->GET_AVG_BACK_HAND);
}

void MainWindow::on_pushButton_4_clicked()
{
    handThread->setMode(handThread->TRAIN_REC_MODE);
}


void MainWindow::on_pushButton_5_clicked()
{
    //    handThread->STOP = true;
    retrievingFrameThread->STOP = true;
    showingImageThread->STOP = true;
    translatingThread->STOP = true;
    croppingThread->STOP = true;
}

void MainWindow::on_MainWindow_destroyed()
{
    //    handThread->quit();
    showingImageThread->quit();
    translatingThread->quit();
    retrievingFrameThread->quit();
    croppingThread->quit();
    //webSource.release();
}


void MainWindow::on_btnTakepic_clicked()
{
    takingPic = true;
}



void MainWindow::on_pushButton_6_clicked()
{
    file->close();
    file = new QFile(trainPath + "/" + ui->txtName->text());
    if (!file->open(QIODevice::WriteOnly)) {
        cerr << "Cannot open file for writing: "
             << qPrintable(file->errorString()) << std::endl;
        return;
    }
    ghiout = new QTextStream(file);
}
