#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //    cout << "xuong day chua";
    //    int s = 5;
    //    do {

    //        cin >> s;
    //       cout << "vao ko";
    //        if (s == 1) {
    //            mode = SAMPLE_MODE;
    //        } else if (s==2 ) {
    //               mode = DETECTION_MODE;
    //        } else if (s== 3) {
    //           mode = TRAIN_REC_MODE;
    //        }
    //    } while (s == 0);
    return a.exec();
}
