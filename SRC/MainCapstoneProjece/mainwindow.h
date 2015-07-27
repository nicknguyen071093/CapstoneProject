#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ImageTranslating.h"
#include "RetrievingFrame.h"
#include "ShowingImageThread.h"
#include "CroppingImage.h"

#include "TimerThread.h"
#include "RecognitionTimerThread.h"

#include "Words.h"

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

    void onToShow(Mat);

    void changeLabelNotice(QString,QString);
    void changeLabelNotice(QString);
    void changeLabelTestingResult(QString,QString);

    void countDownRecognitionTimer(QString);
    void changeRecognitionResult(double);
    void updateRecognitionContent();
    void changeRecognitionColor(QString);
    void changeLearningColor(QString);

    void changeToSelectingFunction();
    void changeToRecognitionFunciton();
    void changeToLearningFunction();
    void changeToFrontHandMode();
    void changeToBackHandMode();
    void changeToBinaryImage();
    void onFinishingColorSubtraction(bool);

    void changeUpColor(QString);
    void changeDownColor(QString);
    void moveToUpperWord();
    void moveToLowerWord();
    void changeLearningResult(double);


protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    TimerThread *timerThread;
    RecognitionTimerThread *recognitionTimerThread;

    RetrievingFrame *retrievingFrameThread;
    ShowingImageThread *showingImageThread;
    CroppingImage *croppingThread;

    QString recognitionContent;

    Words *words;

    QPixmap framePixmap;
    QPixmap learningImagePixmap;

    void startThreads();

    void initiateSelectingFunctionInterface();
    void initiateRecognitionInterface();
    void initiateLearningInterface();
    void initiateColorSubtractionInterface();

    void changeImageByWordID(int);

};

#endif // MAINWINDOW_H
