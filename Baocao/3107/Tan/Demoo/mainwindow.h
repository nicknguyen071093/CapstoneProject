#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <displaypinthread.h>
#include <QString>
#include "QPixmap"
#include "dialog.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    DisplayPINThread *dThread;
    QString s; 
    int i;
public slots:
    void onValueChanged(int);
private:
    Ui::MainWindow *ui;
    Dialog *dialog;
};

#endif // MAINWINDOW_H
