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
    translatingThread = new HandTranslating(this);
    showingImageThread = new ShowingImageThread(this);
    retrievingFrameThread = new RetrievingFrame(this);
    croppingThread = new CroppingImage(this);
    wordDAO = new LetterGet();
    //    wordMap = wordDAO->getLetter();
    timerThread = new TimerThread(this);

    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Rect>("Rect");
    connect(retrievingFrameThread,SIGNAL(changingFrame(Mat)),showingImageThread,SLOT(onChangingImage(Mat)));
    connect(translatingThread,SIGNAL(translatingResultChanged(double)),this,SLOT(onTranslatingResultChanged(double)));
    connect(showingImageThread,SIGNAL(toShow(Mat)),this,SLOT(onToShow(Mat)));
    connect(showingImageThread,SIGNAL(sendImageToCrop(Mat,Mat)),croppingThread,SLOT(receiveBinaryImage(Mat,Mat)));
    connect(croppingThread,SIGNAL(sendingCroppedImage(Mat,Mat,Mat,Mat,QString)),this,SLOT(recceiveCroppedImage(Mat,Mat,Mat,Mat,QString)));
    connect(timerThread,SIGNAL(sendSignalChangingLabelNotice(QString)),this,SLOT(changeLabelNotice(QString)));
    connect(timerThread,SIGNAL(sendSignalChangingToFrontHandMode()),this,SLOT(changeToFrontHandMode()));
    connect(timerThread,SIGNAL(sendSignalChangingToBackHandMode()),this,SLOT(changeToBackHandMode()));
    connect(timerThread,SIGNAL(sendSignalChangingToBinaryImage()),this,SLOT(changeToBinaryImage()));
    connect(timerThread,SIGNAL(sendSignalCheckingFrontHand()),showingImageThread,SLOT(moveToCheckFrontHand()));
    connect(timerThread,SIGNAL(sendSignalCheckingBackHand()),showingImageThread,SLOT(moveToCheckBackHand()));
    connect(showingImageThread,SIGNAL(sendSignalEnableCountDown()),timerThread,SLOT(continueCountDown()));
    //    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(slotReboot()));
    takingNumber = 0;
    startThreads();
}

MainWindow::~MainWindow()
{
    delete ui;
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



void MainWindow::changeLabelNotice(QString noticeStr) {
    ui->lbThongbao->setText(noticeStr);
}



void MainWindow::onTranslatingResultChanged(double result) {
    ui->lbResult->setText(QString::fromUtf8("Kết quả: ") + wordMap.value((int)result));
}

void MainWindow::startThreads() {
    retrievingFrameThread->start();
    showingImageThread->start();
    translatingThread->start();
    croppingThread->start();
    timerThread->start();
    timerThread->setPriority(QThread::LowPriority);
    showingImageThread->setPriority(QThread::HighPriority);
    croppingThread->setPriority(QThread::HighPriority);
    retrievingFrameThread->setPriority(QThread::NormalPriority);
    translatingThread->setPriority(QThread::HighestPriority);
}

void MainWindow::on_pushButton_5_clicked()
{
    //        retrievingFrameThread->STOP = true;
    showingImageThread->STOP = true;
    while (!showingImageThread->isFinished());
    showingImageThread->setToDefaults();
    timerThread->STOP = true;
    //    ui->lbThongbao->setText(QString::fromUtf8("Thông báo: Sẽ lấy loại mẫu trong vòng 5s."));
    //    QThread::sleep(1);
    //    ui->lbThongbao->setText(QString::fromUtf8("Thông báo: Sẽ lấy loại mẫu trong vòng 4s."));
    //    QThread::sleep(1);
    //    ui->lbThongbao->setText(QString::fromUtf8("Thông báo: Sẽ lấy loại mẫu trong vòng 3s."));
    //    QThread::sleep(1);
    //    ui->lbThongbao->setText(QString::fromUtf8("Thông báo: Sẽ lấy loại mẫu trong vòng 2s."));
    //    QThread::sleep(1);
    //    ui->lbThongbao->setText(QString::fromUtf8("Thông báo: Sẽ lấy loại mẫu trong vòng 1s."));
    //    QThread::sleep(1);
//    if (!timerThread->isFinished()){
//        timerThread->terminate();
//    }
    while (!timerThread->isFinished())
    {
//        cout << "dien roi\n";
        ui->lbThongbao->setText(QString::fromUtf8("Thông báo: Sẽ lấy loại mẫu trong vòng 1s."));
    };
    timerThread->setToDefaults();
    timerThread->start();
    showingImageThread->start();
    //    translatingThread->STOP = true;
    //    croppingThread->STOP = true;
    //    timerThread->terminate();
    //    QThread::sleep(2);
    //    retrievingFrameThread->setToDefaults();

    //    translatingThread->setToDefaults();
    //    croppingThread->setToDefaults();

    //    startThreads();
}

void MainWindow::on_MainWindow_destroyed()
{
    retrievingFrameThread->STOP = true;
    showingImageThread->STOP = true;
    translatingThread->STOP = true;
    croppingThread->STOP = true;

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


void MainWindow::changeToFrontHandMode() {
//    showingImageThread->setMode(showingImageThread->GET_AVG_BACKGROUND);
    showingImageThread->setMode(showingImageThread->GETTING_BACKGROUND_MODE);
}

void MainWindow::changeToBackHandMode() {
    showingImageThread->setMode(showingImageThread->GET_AVG_FRONT_HAND);
}


void MainWindow::changeToBinaryImage() {
    showingImageThread->setMode(showingImageThread->GET_AVG_BACK_HAND);
}

void MainWindow::slotReboot()
{
    retrievingFrameThread->STOP = true;
    showingImageThread->STOP = true;
    translatingThread->STOP = true;
    croppingThread->STOP = true;
    timerThread->terminate();
//    QThread::sleep(3);
    qDebug() << "Performing application reboot...";
    qApp->exit( MainWindow::EXIT_CODE_REBOOT );
}
