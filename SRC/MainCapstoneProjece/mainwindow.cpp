#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_DECLARE_METATYPE(Mat)
Q_DECLARE_METATYPE(Rect)
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initiateColorSubtractionInterface();
    //
    showingImageThread = new ShowingImageThread(this);
    retrievingFrameThread = new RetrievingFrame(this);
    croppingThread = new CroppingImage(this);

    wordDAO = new LetterGet();
    //    wordMap = wordDAO->getLetter();

    timerThread = new TimerThread(this);
    recognitionTimerThread = new RecognitionTimerThread(this);

    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Rect>("Rect");

    connect(retrievingFrameThread,SIGNAL(changingFrame(Mat)),showingImageThread,SLOT(onChangingImage(Mat)),Qt::DirectConnection);

    connect(showingImageThread,SIGNAL(toShow(Mat)),this,SLOT(onToShow(Mat)),Qt::DirectConnection);
    connect(showingImageThread,SIGNAL(sendImageToCrop(Mat,Mat)),croppingThread,SLOT(receiveBinaryImage(Mat,Mat)),Qt::DirectConnection);

    //    connect(croppingThread,SIGNAL(sendSignalSendingFeatures(QString)),translatingThread,SLOT(receiveFeatures(QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalToChangeLabelTestingResult(QString)),this,SLOT(changeLabelTestingResult(QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalSelectingRecognition()),this,SLOT(changeToRecognitionFunciton()),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangingRecognitionResult(QString)),this,SLOT(changeRecognitionResult(QString)),Qt::DirectConnection);

    connect(timerThread,SIGNAL(sendSignalChangingLabelNotice(QString,QString)),this,SLOT(changeLabelNotice(QString,QString)),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToFrontHandMode()),this,SLOT(changeToFrontHandMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToBackHandMode()),this,SLOT(changeToBackHandMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToBinaryImage()),this,SLOT(changeToBinaryImage()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalCheckingFrontHand()),showingImageThread,SLOT(moveToCheckFrontHand()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalCheckingBackHand()),showingImageThread,SLOT(moveToCheckBackHand()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToTestingTime()),croppingThread,SLOT(changeToTestingMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalGetTestingResult()),croppingThread,SLOT(getTestingResult()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalFinishingColorSubtraction(bool)),this,SLOT(onFinishingColorSubtraction(bool)),Qt::DirectConnection);

    connect(recognitionTimerThread,SIGNAL(sendSignalCountDownRecognitionTime(QString)),this,SLOT(countDownRecognitionTimer(QString)),Qt::DirectConnection);
    connect(recognitionTimerThread,SIGNAL(sendSignalUpdatingContent()),this,SLOT(updateRecognitionContent()),Qt::DirectConnection);

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
    free(recognitionTimerThread);
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
    ui->lbNotify->setText(noticeStr);
    ui->lbCountDownTime->setText(countDownTime);
}

void MainWindow::changeLabelNotice(QString noticeStr) {
    ui->lbNotify->setText(noticeStr);
}

void MainWindow::changeLabelTestingResult(QString noticeStr) {
    ui->lbTestingResult->setText(noticeStr);
}


void MainWindow::startThreads() {
    retrievingFrameThread->start(QThread::LowPriority);
    showingImageThread->start(QThread::HighPriority);
    croppingThread->start(QThread::HighestPriority);
    timerThread->start(QThread::LowestPriority);
    recognitionTimerThread->start(QThread::LowestPriority);
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
        initiateSelectingFunctionInterface();
        croppingThread->changeToSelectingFunctionMode();
    } else {
        showingImageThread->setToDefaults();
    }
}

void MainWindow::countDownRecognitionTimer(QString time) {
    ui->lbRecognitionTimer->setText(time);
}

void MainWindow::changeRecognitionResult(QString result) {
    ui->lbRecognitionResult->setText(result);
}

void MainWindow::updateRecognitionContent() {
    QString result = croppingThread->getRegResult();
    if (result != "0") { // no detecting hand
        //  update content
        recognitionContent += " " + result;
        ui->lbRecognitionContent->setText(recognitionContent);
        if (result == "5") { // end sign
            // speak
            // initiate selecting interface
            initiateSelectingFunctionInterface();
            // stop recognition timer
            recognitionTimerThread->enableWorking(false);
            // change to select
            croppingThread->changeToSelectingFunctionMode();
        }
    }
}

void MainWindow::changeToSelectingFunction() {

}

void MainWindow::changeToRecognitionFunciton() {
    // init interface
    initiateRecognitionInterface();
    // init recognition content
    recognitionContent = "";
    ui->lbRecognitionContent->setText("");
    // change to recognition mode
//    croppingThread->changeToRecognitionMode();
    // enable timer
    recognitionTimerThread->enableWorking(true);
}

void MainWindow::changeToLearningFunction() {

}

void MainWindow::initiateSelectingFunctionInterface() {
    ui->gbTestingResult->hide();
    ui->gbCountDownTime->hide();

    ui->gbRecognitionContent->hide();
    ui->gbRecognitionNotify->hide();
    ui->gbRecognitionResult->hide();
    ui->gbRecognitionTimer->hide();

    ui->gbNotify->show();
    ui->lbNotify->setText("Hãy chọn chức năng mong muốn bằng cách đưa ký hiệu hình bên vào vùng chức năng đó!");
    ui->btnLearning->show();
    ui->btnRecognition->show();
}

void MainWindow::initiateRecognitionInterface() {
    ui->btnRecognition->hide();
    ui->btnLearning->hide();

    ui->gbTestingResult->hide();
    ui->gbCountDownTime->hide();
    ui->gbNotify->hide();

    ui->gbRecognitionContent->show();
    ui->gbRecognitionNotify->show();
    ui->gbRecognitionResult->show();
    ui->gbRecognitionTimer->show();
}

void MainWindow::initiateLearningInterface() {

}

void MainWindow::initiateColorSubtractionInterface() {
    ui->btnRecognition->hide();
    ui->btnLearning->hide();

    ui->gbRecognitionContent->hide();
    ui->gbRecognitionNotify->hide();
    ui->gbRecognitionResult->hide();
    ui->gbRecognitionTimer->hide();
}
