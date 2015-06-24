#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSqlDatabase db;
    void FillTable();

private slots:
    void on_btnDelete_clicked();

    void on_btnUpdate_clicked();

    void on_btnInsert_clicked();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
