#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "HandThread.h"
#include "HandTranslating.h"
#include "ImageTranslating.h"
#include "QDir"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    HandThread *handThread;
    HandTranslating *translatingThread;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_MainWindow_destroyed();

    void onHandTrackingChanged(Mat);

    void onSendingBinaryImage(Mat);

    void onTranslatingResultChanged(double);

    void onBinaryImageHandChanged (Mat, Mat);

    void onHandSubtracted(Mat,Mat,Rect);

    void onHandTrackingFinishinished();

    void on_pushButton_5_clicked();

    void on_btnTakepic_clicked();

private:
    Ui::MainWindow *ui;
    bool takingPic;
};

#endif // MAINWINDOW_H
