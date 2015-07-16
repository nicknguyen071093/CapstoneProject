#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "HandThread.h"
#include "HandTranslating.h"
#include "ImageTranslating.h"
#include "RetrievingFrame.h"
#include "letterget.h"
#include "ShowingImageThread.h"
#include "CroppingImage.h"
#include "QDir"
#include "QFile"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_MainWindow_destroyed();

    void onHandTrackingChanged(Mat);

    void onSendingBinaryImage(Mat,Mat);

    void onTranslatingResultChanged(double);

    void onHandTrackingFinishinished();

    void onToShow(Mat);

    void recceiveCroppedImage(Mat,Mat,Mat,Mat,QString);

    void on_pushButton_5_clicked();

    void on_btnTakepic_clicked();

    void on_btnBackHand_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    bool takingPic;
    HandThread *handThread;
    RetrievingFrame *retrievingFrameThread;
    LetterGet *wordDAO;
    ShowingImageThread *showingImageThread;
    HandTranslating *translatingThread;
    CroppingImage *croppingThread;
    QMap <int,QString> wordMap;
    QFile *file;
    QTextStream *ghiout;
    vector<int> compression_params;
    QString trainPath;
    int takingNumber;
    void getFeature1To3(Mat);
};

#endif // MAINWINDOW_H
