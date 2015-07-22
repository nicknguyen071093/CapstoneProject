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

    void onToShow(Mat);

    void changeLabelNotice(QString,QString);

    void changeLabelNotice(QString);

    void changeLabelTestingResult(QString);

    void changeToFrontHandMode();

    void changeToBackHandMode();

    void changeToBinaryImage();

    void onFinishingColorSubtraction(bool);

    void changeBackgroundButton();

private:
    Ui::MainWindow *ui;
    TimerThread *timerThread;
    RetrievingFrame *retrievingFrameThread;
    LetterGet *wordDAO;
    ShowingImageThread *showingImageThread;
    CroppingImage *croppingThread;
    QMap <int,QString> wordMap;
    void startThreads();
};

#endif // MAINWINDOW_H
