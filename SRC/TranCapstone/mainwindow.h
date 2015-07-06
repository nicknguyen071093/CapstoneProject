#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "HandThread.h"
#include "HandTranslating.h"
#include "ImageTranslating.h"
#include "letterget.h"
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

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_MainWindow_destroyed();

    void onHandTrackingChanged(Mat);

    void onSendingBinaryImage(Mat);

    void onTranslatingResultChanged(double);

    void onHandTrackingFinishinished();

    void on_pushButton_5_clicked();

    void on_btnTakepic_clicked();

    void on_btnBackHand_clicked();

private:
    Ui::MainWindow *ui;
    bool takingPic;
    HandThread *handThread;
    LetterGet *wordDAO;
    HandTranslating *translatingThread;
    QMap <int,QString> wordMap;
};

#endif // MAINWINDOW_H
