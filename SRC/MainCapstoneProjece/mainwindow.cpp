#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_DECLARE_METATYPE(Mat)
Q_DECLARE_METATYPE(Rect)
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btnRecognition->hide();
    ui->btnLearning->hide();

    //
    showingImageThread = new ShowingImageThread(this);
    retrievingFrameThread = new RetrievingFrame(this);
    croppingThread = new CroppingImage(this);
    wordDAO = new LetterGet();
    //    wordMap = wordDAO->getLetter();
    timerThread = new TimerThread(this);

    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Rect>("Rect");

    connect(retrievingFrameThread,SIGNAL(changingFrame(Mat)),showingImageThread,SLOT(onChangingImage(Mat)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalToChangeLabelTestingResult(QString)),this,SLOT(changeLabelTestingResult(QString)),Qt::DirectConnection);
    connect(showingImageThread,SIGNAL(toShow(Mat)),this,SLOT(onToShow(Mat)),Qt::DirectConnection);
    connect(showingImageThread,SIGNAL(sendImageToCrop(Mat,Mat)),croppingThread,SLOT(receiveBinaryImage(Mat,Mat)),Qt::DirectConnection);
//    connect(croppingThread,SIGNAL(sendSignalSendingFeatures(QString)),translatingThread,SLOT(receiveFeatures(QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalToChangeLabelTestingResult(QString)),this,SLOT(changeLabelTestingResult(QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangingBackGroundColor()),this,SLOT(changeBackgroundButton()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingLabelNotice(QString,QString)),this,SLOT(changeLabelNotice(QString,QString)),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToFrontHandMode()),this,SLOT(changeToFrontHandMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToBackHandMode()),this,SLOT(changeToBackHandMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToBinaryImage()),this,SLOT(changeToBinaryImage()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalCheckingFrontHand()),showingImageThread,SLOT(moveToCheckFrontHand()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalCheckingBackHand()),showingImageThread,SLOT(moveToCheckBackHand()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToTestingTime()),croppingThread,SLOT(changeToTestingMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalGetTestingResult()),croppingThread,SLOT(getTestingResult()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalFinishingColorSubtraction(bool)),this,SLOT(onFinishingColorSubtraction(bool)),Qt::DirectConnection);
    connect(showingImageThread,SIGNAL(sendSignalEnableCountDown()),timerThread,SLOT(continueCountDown()),Qt::DirectConnection);
    connect(showingImageThread,SIGNAL(sendSignalChangingLabelNotify(QString)),this,SLOT(changeLabelNotice(QString)),Qt::DirectConnection);

    startThreads();
}

MainWindow::~MainWindow()
{
    free(retrievingFrameThread);
    free(showingImageThread);
    free(croppingThread);
    free(wordDAO);
    free(timerThread);
    delete ui;
}

void MainWindow::onToShow(Mat receivedImage) {
    cv::resize(receivedImage, receivedImage, Size(480,360), 0, 0, CV_INTER_LINEAR);
    QImage handImage = cvMatToQImage(receivedImage);
    if (!handImage.isNull()) {
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbFrame->setPixmap(pixmapObject);
        }
    }
}

void MainWindow::changeLabelNotice(QString noticeStr, QString countDownTime) {
    ui->lbThongbao->setText(noticeStr);
    ui->lbCountDown->setText(countDownTime);
}

void MainWindow::changeBackgroundButton() {
    ui->btnRecognition->setStyleSheet("background-color: rgb(176, 0, 2);");
}

void MainWindow::changeLabelNotice(QString noticeStr) {
    ui->lbThongbao->setText(noticeStr);
}

void MainWindow::changeLabelTestingResult(QString noticeStr) {
    ui->lbTestingResult->setText(noticeStr);
}


void MainWindow::startThreads() {
    retrievingFrameThread->start();
    showingImageThread->start();
    croppingThread->start();
    timerThread->start();

    timerThread->setPriority(QThread::LowPriority);
    showingImageThread->setPriority(QThread::HighPriority);
    croppingThread->setPriority(QThread::HighestPriority);
    retrievingFrameThread->setPriority(QThread::NormalPriority);
}


void MainWindow::on_MainWindow_destroyed()
{
    retrievingFrameThread->STOP = true;
    showingImageThread->STOP = true;
    croppingThread->STOP = true;
}

void MainWindow::changeToFrontHandMode() {
    showingImageThread->setMode(showingImageThread->GET_AVG_BACKGROUND);
}

void MainWindow::changeToBackHandMode() {
    showingImageThread->setMode(showingImageThread->GET_AVG_FRONT_HAND);
}


void MainWindow::changeToBinaryImage() {
    showingImageThread->setMode(showingImageThread->GET_AVG_BACK_HAND);
}

void MainWindow::onFinishingColorSubtraction(bool result) {
    if (result) {
        ui->btnRecognition->show();
        ui->btnLearning->show();
    } else {
        while(!timerThread->isFinished());
        timerThread->start();
        showingImageThread->setToDefaults();
    }
}

