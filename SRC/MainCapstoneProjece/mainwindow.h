#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ImageTranslating.h"
#include "RetrievingFrame.h"
#include "ShowingImageThread.h"
#include "CroppingImage.h"

#include "letterget.h"

#include "TimerThread.h"
#include "RecognitionTimerThread.h"

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

    void countDownRecognitionTimer(QString);

    void updateRecognitionContent();

    void changeToSelectingFunction();

    void changeToRecognitionFunciton();

    void changeToLearningFunction();

    void changeRecognitionResult(QString);

private:
    Ui::MainWindow *ui;

    TimerThread *timerThread;
    RecognitionTimerThread *recognitionTimerThread;

    RetrievingFrame *retrievingFrameThread;
    ShowingImageThread *showingImageThread;
    CroppingImage *croppingThread;

    QMap <int,QString> wordMap;
    LetterGet *wordDAO;

    QString recognitionContent;

    void startThreads();

    void initiateSelectingFunctionInterface();
    void initiateRecognitionInterface();
    void initiateLearningInterface();
    void initiateColorSubtractionInterface();

};

#endif // MAINWINDOW_H
