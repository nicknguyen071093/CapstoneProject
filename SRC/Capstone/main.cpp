#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //    LetterGet *wordDAO = new LetterGet();
    //    QMap <int,QString> wordMap = wordDAO->getLetter();
    //    cout << "Tong:" << wordMap.value(1);
    //    return 0;
    //    int currentExitCode = 0;

    //    do {
    //        QApplication a(argc, argv);
    //        MainWindow w;
    //        w.show();
    //        currentExitCode = a.exec();
    //    } while( currentExitCode == MainWindow::EXIT_CODE_REBOOT );

    //    return currentExitCode;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
    //    Mat image = imread("/home/nickseven/mau-tay-moi.png");
    //    /// Convert the image to Gray
    //    Mat src_gray;
    //    cvtColor( image, src_gray, CV_BGR2GRAY );
    //    vector<Vec4i> hierarchy;
    //    vector< vector<Point> > contours;
    //    Mat dst;
    //    threshold(src_gray, dst, 128, 255,0);
    //    findContours(dst,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
    //    Mat handMask = Mat::zeros(image.rows,image.cols, CV_8UC3);
    //    drawContours( handMask, contours, -1, Scalar(255,0,0), 2, 8, hierarchy, 0, Point());
    //    Point point1(69,135);
    //    Point point1a(74,140);
    //    Point point2(106,175);
    //    Point point2a(111,180);
    //    Point point3(115,51);
    //    Point point3a(120,56);
    //    Point point4(135,118);
    //    Point point4a(140,123);
    //    Point point5(161,33);
    //    Point point5a(166,38);
    //    Point point6(163,113);
    //    Point point6a(168,118);
    //    Point point7(196,44);
    //    Point point7a(201,49);
    //    Point point8(188,118);
    //    Point point8a(193,123);
    //    Point point9(239,72);
    //    Point point9a(244,77);
    //    Point point10(213,131);
    //    Point point10a(218,136);
    //    Point point11(130,196);
    //    Point point11a(135,201);
    //    Point point12(201,196);
    //    Point point12a(206,201);
    //    Point point13(167,162);
    //    Point point13a(172,167);
    //    rectangle(handMask,point1,point1a,Scalar(255,0,0),1);
    //    rectangle(handMask,point2,point2a,Scalar(255,0,0),1);
    //    rectangle(handMask,point3,point3a,Scalar(255,0,0),1);
    //    rectangle(handMask,point4,point4a,Scalar(255,0,0),1);
    //    rectangle(handMask,point5,point5a,Scalar(255,0,0),1);
    //    rectangle(handMask,point6,point6a,Scalar(255,0,0),1);
    //    rectangle(handMask,point7,point7a,Scalar(255,0,0),1);
    //    rectangle(handMask,point8,point8a,Scalar(255,0,0),1);
    //    rectangle(handMask,point9,point9a,Scalar(255,0,0),1);
    //    rectangle(handMask,point10,point10a,Scalar(255,0,0),1);
    //    rectangle(handMask,point11,point11a,Scalar(255,0,0),1);
    //    rectangle(handMask,point12,point12a,Scalar(255,0,0),1);
    //    rectangle(handMask,point13,point13a,Scalar(255,0,0),1);
    //    imshow("thu thoi",handMask);
    //    waitKey(0);
    //    return 0;
}
