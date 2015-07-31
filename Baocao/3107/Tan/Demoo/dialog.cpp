#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    diaThread = new ThreadDialog(this);
    diaThread->start();
    connect(diaThread, SIGNAL(valueChanged(QString)),
                 this, SLOT(onValueChanged(QString)));
    connect(diaThread, SIGNAL(finished()),
                 this, SLOT(onFinish()));
}

Dialog::~Dialog()
{
    free(diaThread);
    delete ui;
}
void Dialog::onValueChanged(QString s){
    ui->label_2->setText(s);
}
void Dialog::onFinish(){
    this->close();
}
