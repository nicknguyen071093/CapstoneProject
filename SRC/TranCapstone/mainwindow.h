#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "HandTranslating.h"
#include "ImageTranslating.h"
#include "RetrievingFrame.h"
#include "letterget.h"
#include "ShowingImageThread.h"
#include "CroppingImage.h"
#include "TimerThread.h"
#include "QDir"
#include "QFile"
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    static int const EXIT_CODE_REBOOT = -123456789;
    ~MainWindow();

private slots:

    void on_MainWindow_destroyed();

    void onTranslatingResultChanged(double);

    void onToShow(Mat);

    void recceiveCroppedImage(Mat,Mat,Mat,Mat,QString);

    void changeLabelNotice(QString);

    void on_pushButton_5_clicked();

    void on_btnTakepic_clicked();

    void on_pushButton_6_clicked();

    void changeToFrontHandMode();

    void changeToBackHandMode();

    void changeToBinaryImage();
    void slotReboot();


private:
    Ui::MainWindow *ui;

    bool takingPic;
    TimerThread *timerThread;
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
    void startThreads();
};

#endif // MAINWINDOW_H
