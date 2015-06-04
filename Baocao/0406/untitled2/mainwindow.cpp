#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/opencv.hpp"
#include <cv.h>
#include <highgui.h>
#include <QLabel>
#include <string>
#include <cstring>
#include <stdio.h>
#include <QImage>
#include <QPixmap>

using namespace std;
using namespace cv;

inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

        return image;
    }

        // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

        return image.rgbSwapped();
    }

        // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }

        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

        image.setColorTable( sColorTable );

        return image;
    }

    default:
        //qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnDisplay_clicked()
{



    Mat src = imread("/home/flyc/3.jpg");
    Mat dst = src.clone();
    double angle = 0;
    double scale = 2;
    Point2f center(src.cols/2, src.rows/2);
    Mat mat_rot = getRotationMatrix2D(center, angle, scale);
    warpAffine(src, dst, mat_rot, src.size());
    imshow("Anh goc", src);
    imshow("Anh sau phep bien doi", dst);
    waitKey(0);






  //  QImage imageObject;
 //   string imagePath = "/home/flyc/2.jpg";

 //   Mat imageM = imread(imagePath);

  //  int w = ui->label1->width();
 //   int h = ui->label1->height();

 //   imageObject = cvMatToQImage(imageM);
 //   QPixmap pixmapObject = QPixmap::fromImage(imageObject);
 //   ui->label1->setPixmap(pixmapObject.scaled(w,h,Qt::KeepAspectRatio));

}



