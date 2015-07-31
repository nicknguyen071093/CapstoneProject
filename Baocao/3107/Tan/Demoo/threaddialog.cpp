#include "threaddialog.h"

ThreadDialog::ThreadDialog(QObject *parent) :
    QThread(parent)
{
}
void ThreadDialog::run(){
    emit valueChanged("5");
    sleep(1);
    emit valueChanged("4");
    sleep(1);
    emit valueChanged("3");
    sleep(1);
    emit valueChanged("2");
    sleep(1);
    emit valueChanged("1");
    sleep(1);
}
