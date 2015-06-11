#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_DECLARE_METATYPE(Mat)
Q_DECLARE_METATYPE(Rect)
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    handThread = new HandThread(this);
    qRegisterMetaType<Mat>("Mat");
    qRegisterMetaType<Rect>("Rect");
    connect(handThread,SIGNAL(handTrackingChanged(QImage)),this,SLOT(onHandTrackingChanged(QImage)));
    connect(handThread,SIGNAL(finished()),this,SLOT(onHandTrackingFinishinished()));
    connect(handThread,SIGNAL(handSubtractingChanged(Mat,Rect)),this,SLOT(onHandSubtracted(Mat,Rect)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onHandTrackingFinishinished() {
    cout << "vao roi mung qua";
    ui->lbFrame->clear();
    ui->lbSubtractedHand->clear();
    handThread->releaseAll();
}

void MainWindow::onHandTrackingChanged(QImage receivedImage) {
    if (!receivedImage.isNull()) {
        QPixmap pixmapObject = QPixmap::fromImage(receivedImage);
        if (!pixmapObject.isNull()) {
            ui->lbFrame->setPixmap(pixmapObject.scaled(320,240,Qt::KeepAspectRatio));
        }
    }
}

void MainWindow::onHandSubtracted(Mat frame, Rect boundingHand) {
    cout << "Vao day ko" << endl;
   QImage handImage = cvMatToQImage(Mat(frame,boundingHand));
    if (!handImage.isNull()) {
        QPixmap pixmapObject = QPixmap::fromImage(handImage);
        if (!pixmapObject.isNull()) {
            ui->lbSubtractedHand->setPixmap(pixmapObject.scaled(320,240,Qt::KeepAspectRatio));
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    handThread->start();
}

void MainWindow::on_pushButton_2_clicked()
{
    handThread->setMode(handThread->SAMPLE_MODE);
}


void MainWindow::on_pushButton_3_clicked()
{
    handThread->setMode(handThread->DETECTION_MODE);
}

void MainWindow::on_pushButton_4_clicked()
{
    handThread->setMode(handThread->TRAIN_REC_MODE);
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

