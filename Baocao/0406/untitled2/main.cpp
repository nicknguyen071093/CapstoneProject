#include "mainwindow.h"
#include <QApplication>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <QLabel>
#include <string>
#include <cstring>
#include <stdio.h>

using namespace std;

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();


}
