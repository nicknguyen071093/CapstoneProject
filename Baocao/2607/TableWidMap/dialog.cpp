#include "dialog.h"
#include "ui_dialog.h"
#include "mainwindow.h"
#include "QDebug"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    //qDebug() << idImage;

}

Dialog::~Dialog()
{
    delete ui;
}
void Dialog::onsetImageLable(const QString &s){
    QPixmap image(s);
    ui->label->setPixmap(image);
}
void Dialog::on_pushButton_clicked()
{


}

