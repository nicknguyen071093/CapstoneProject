#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "QPixmap"
#include "QString"
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void onsetImageLable(const QString &s);
private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    int idImage;

};

#endif // DIALOG_H
