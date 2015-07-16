#ifndef FRMLIST_H
#define FRMLIST_H

#include <QMainWindow>
#include <QString>
#include <letterget.h>
#include <imagedialog.h>

namespace Ui {
class FrmList;
}

class FrmList : public QMainWindow
{
    Q_OBJECT

public:
    explicit FrmList(QWidget *parent = 0);
    ~FrmList();
    void FillTable();

private slots:
    void on_pushButton_clicked();

private:
    Ui::FrmList *ui;
    ImageDialog *dialog;
};

#endif // FRMLIST_H
