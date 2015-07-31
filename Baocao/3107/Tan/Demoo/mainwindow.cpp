#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dThread = new DisplayPINThread(this);
    dThread->start();
    i=0;
    connect(dThread, SIGNAL(valueChanged(int)),
                 this, SLOT(onValueChanged(int)));   
     //s= "../images.png";
    //QPixmap image(s);
    QPixmap image("../images.png");
    ui->label->setPixmap(image);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onValueChanged(int value)
{

    ui->label_2->setText(QString::number(value));
    if(value==4){
        s=("../images.png");
    }else if(value==3){
        s=("../images1.png");
    }else if(value==2){
        s=("../images2.png");
    }else if(value==1){
        qDebug() << i;
        i++;
        s=("../images3.png");
        dialog = new Dialog(this);
        dialog->setModal(true);
        dialog->show();

    }else if(value==0){
        s=("../images4.png");

        dialog = new Dialog(this);
        dialog->show();
    }
    QPixmap image(s);
    ui->label->setPixmap(image);

}

