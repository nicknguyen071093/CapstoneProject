#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_DECLARE_METATYPE(Mat)
Q_DECLARE_METATYPE(Rect)
Q_DECLARE_METATYPE(WordDTO)
Q_DECLARE_METATYPE(QItemSelection)
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //
    words = new Words();
    ui->lwWord->addItems(words->getWords());
    //
    initiateColorSubtractionInterface();
    //
    showingImageThread = new ShowingImageThread(this);
    retrievingFrameThread = new RetrievingFrame(this);
    croppingThread = new CroppingImage(this);


    timerThread = new TimerThread(this);
    recognitionTimerThread = new RecognitionTimerThread(this);

    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Rect>("Rect");
    qRegisterMetaType<WordDTO>("WordDTO");
    qRegisterMetaType<QItemSelection>("QItemSelection");
    connect(retrievingFrameThread,SIGNAL(changingFrame(Mat)),showingImageThread,SLOT(onChangingImage(Mat)),Qt::DirectConnection);

    connect(showingImageThread,SIGNAL(toShow(Mat)),this,SLOT(onToShow(Mat)));
    connect(showingImageThread,SIGNAL(sendImageToCrop(Mat,Mat)),croppingThread,SLOT(receiveBinaryImage(Mat,Mat)),Qt::DirectConnection);

    connect(croppingThread,SIGNAL(sendSignalToChangeLabelTestingResult(QString,QString)),this,SLOT(changeLabelTestingResult(QString,QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalSelectingRecognition()),this,SLOT(changeToRecognitionFunciton()),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalSelectingLearning()),this,SLOT(changeToLearningFunction()),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangingRecognitionResult(double)),this,SLOT(changeRecognitionResult(double)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangingRecognitionColor(QString)),this,SLOT(changeRecognitionColor(QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangingLearningColor(QString)),this,SLOT(changeLearningColor(QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangingUpColor(QString)),this,SLOT(changeUpColor(QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangingDownColor(QString)),this,SLOT(changeDownColor(QString)),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalMovingToUpperWord()),this,SLOT(moveToUpperWord()),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalMovingToLowerWord()),this,SLOT(moveToLowerWord()),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangeToSelectingMode()),this,SLOT(changeToSelectingFunction()),Qt::DirectConnection);
    connect(croppingThread,SIGNAL(sendSignalChangingLearningResult(double)),this,SLOT(changeLearningResult(double)),Qt::DirectConnection);

    connect(timerThread,SIGNAL(sendSignalChangingLabelNotice(QString,QString)),this,SLOT(changeLabelNotice(QString,QString)),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToFrontHandMode()),this,SLOT(changeToFrontHandMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToBackHandMode()),this,SLOT(changeToBackHandMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToBinaryImage()),this,SLOT(changeToBinaryImage()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalCheckingFrontHand()),showingImageThread,SLOT(moveToCheckFrontHand()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalCheckingBackHand()),showingImageThread,SLOT(moveToCheckBackHand()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalChangingToTestingTime()),croppingThread,SLOT(changeToTestingMode()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalGetTestingResult()),croppingThread,SLOT(getTestingResult()),Qt::DirectConnection);
    connect(timerThread,SIGNAL(sendSignalFailTesingResult(QString,QString)),this,SLOT(changeLabelTestingResult(QString,QString)),Qt::DirectConnection);
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

    free(words);

    free(timerThread);
    free(recognitionTimerThread);
    delete ui;
}

void MainWindow::onToShow(Mat receivedImage) {
//    cv::resize(receivedImage, receivedImage, Size(480,360), 0, 0, CV_INTER_LINEAR);
    QImage handImage = cvMatToQImage(receivedImage);
    if (!handImage.isNull()) {
        framePixmap = QPixmap::fromImage(handImage);
        if (!framePixmap.isNull()) {
            ui->lbFrame->setPixmap(framePixmap);
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

void MainWindow::changeLabelTestingResult(QString noticeStr, QString colorStr) {
    ui->lbTestingResult->setStyleSheet(colorStr);
    ui->lbTestingResult->setText(noticeStr);
}


void MainWindow::startThreads() {
    retrievingFrameThread->start(QThread::LowPriority);
    showingImageThread->start(QThread::HighPriority);
    croppingThread->start(QThread::HighestPriority);
    timerThread->start(QThread::LowestPriority);
    recognitionTimerThread->start(QThread::LowestPriority);
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
        ui->lbTestingResult->setText(QString::fromUtf8("Đang Tiến Hành!"));
        ui->lbTestingResult->setStyleSheet("color: black");
        showingImageThread->setToDefaults();
    }
}

void MainWindow::countDownRecognitionTimer(QString time) {
    ui->lbRecognitionTimer->setText(time);
}

void MainWindow::changeRecognitionResult(double result) {
    QString resultStr = words->getMeanByID(result);
    ui->lbRecognitionResult->setText(resultStr);
}

void MainWindow::changeLearningResult(double result) {
    QString resultStr = words->getMeanByID(result);
    ui->lbLearningResult->setText(resultStr);
}

void MainWindow::updateRecognitionContent() {
    double result = croppingThread->getRegResult();
    if (result != 0) { // no detecting hand
        //  update content
        QString resultStr = words->getMeanByID(result);
        recognitionContent += " " + resultStr;
        ui->lbRecognitionContent->setText(recognitionContent);
        if (result == 5) { // end sign
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

void MainWindow::changeRecognitionColor(QString colorStr) {
    ui->btnRecognition->setStyleSheet(colorStr);
}

void MainWindow::changeLearningColor(QString colorStr) {
    ui->btnLearning->setStyleSheet(colorStr);
}

void MainWindow::changeUpColor(QString colorStr) {
    ui->btnUp->setStyleSheet(colorStr);
}

void MainWindow::changeDownColor(QString colorStr) {
    ui->btnDown->setStyleSheet(colorStr);
}

void MainWindow::moveToLowerWord() {
    int rowNumber = ui->lwWord->currentRow() + 1;
    if (rowNumber < ui->lwWord->count()) {
        ui->lwWord->setCurrentRow(rowNumber);
        changeImageByWordID(rowNumber);
    }
}

void MainWindow::moveToUpperWord() {
    int rowNumber = ui->lwWord->currentRow() -1;
    if (rowNumber >= 0) {
        ui->lwWord->setCurrentRow(rowNumber);
        changeImageByWordID(rowNumber);
    }
}

void MainWindow::changeToSelectingFunction() {
    initiateSelectingFunctionInterface();
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
    initiateLearningInterface();
}

void MainWindow::initiateSelectingFunctionInterface() {
    ui->gbTestingResult->hide();
    ui->gbCountDownTime->hide();

    ui->gbRecognitionContent->hide();
    ui->gbRecognitionNotify->hide();
    ui->gbRecognitionResult->hide();
    ui->gbRecognitionTimer->hide();

    ui->btnDown->hide();
    ui->btnUp->hide();
    ui->gbLearningNotify->hide();
    ui->gbLearningResult->hide();
    ui->gbListWord->hide();


    ui->gbNotify->show();
    ui->lbNotify->setText("Hãy chọn chức năng mong muốn bằng cách đưa ký hiệu hình bên vào vùng chức năng đó!");
    ui->btnLearning->show();
    ui->btnRecognition->show();

    ui->gbTutorial->show();
}

void MainWindow::initiateRecognitionInterface() {
    ui->btnRecognition->hide();
    ui->btnLearning->hide();
    ui->gbTestingResult->hide();
    ui->gbCountDownTime->hide();
    ui->gbNotify->hide();
    ui->gbTutorial->hide();

    ui->gbRecognitionContent->show();
    ui->gbRecognitionNotify->show();
    ui->gbRecognitionResult->show();
    ui->gbRecognitionTimer->show();
}

void MainWindow::initiateLearningInterface() {
    ui->btnRecognition->hide();
    ui->btnLearning->hide();
    ui->gbTestingResult->hide();
    ui->gbCountDownTime->hide();
    ui->gbNotify->hide();
    ui->gbTutorial->hide();
    \
    ui->btnDown->show();
    ui->btnUp->show();
    ui->gbLearningNotify->show();
    ui->gbLearningResult->show();

    ui->lwWord->setCurrentRow(0);
    changeImageByWordID(0);
    ui->gbListWord->show();
}

void MainWindow::changeImageByWordID(int index) {
    QString linkImage = words->getLinkBySelectedIndex(index);
    learningImagePixmap = QPixmap(linkImage);
    if (!learningImagePixmap.isNull()) {
        ui->lbLearningImage->setPixmap(learningImagePixmap);
    }
}

void MainWindow::initiateColorSubtractionInterface() {
    ui->btnRecognition->hide();
    ui->btnLearning->hide();
    ui->gbTutorial->hide();

    ui->gbRecognitionContent->hide();
    ui->gbRecognitionNotify->hide();
    ui->gbRecognitionResult->hide();
    ui->gbRecognitionTimer->hide();

    ui->btnDown->hide();
    ui->btnUp->hide();
    ui->gbLearningNotify->hide();
    ui->gbLearningResult->hide();
    ui->gbListWord->hide();

    ui->lbTestingResult->setText(QString::fromUtf8("Đang Tiến Hành!"));
    ui->lbTestingResult->setStyleSheet("color: black");
}

void MainWindow::closeEvent(QCloseEvent *event) {
    retrievingFrameThread->STOP = true;
    while(!retrievingFrameThread->isFinished());
    showingImageThread->STOP = true;
    while(!showingImageThread->isFinished());
    croppingThread->STOP = true;
    while(!croppingThread->isFinished());
//    timerThread->STOP = true;
//    timerThread->terminate();
//    while(!timerThread->isFinished());
    recognitionTimerThread->STOP = true;
    while(!recognitionTimerThread->isFinished());
    QMainWindow::closeEvent(event);
}
