#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QString>

namespace Ui {
class ImageDialog;
}

class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageDialog(QWidget *parent = 0);
    ~ImageDialog();
    void onsetImageLable(const QString &s);

private:
    Ui::ImageDialog *ui;
};

#endif // IMAGEDIALOG_H
