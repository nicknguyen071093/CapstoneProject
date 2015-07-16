#include "imagedialog.h"
#include "ui_imagedialog.h"

ImageDialog::ImageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageDialog)
{
    ui->setupUi(this);
}

ImageDialog::~ImageDialog()
{
    delete ui;
}
void ImageDialog::onsetImageLable(const QString &s){
    QPixmap image(s);
    ui->label->setPixmap(image);
}
