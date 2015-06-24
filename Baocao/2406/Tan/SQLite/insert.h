#ifndef INSERT_H
#define INSERT_H

#include <QDialog>
#include <mainwindow.h>


namespace Ui {
class Insert;
}

class Insert : public QDialog
{
    Q_OBJECT

public:
    explicit Insert(QWidget *parent = 0);
    ~Insert();
    MainWindow main;

private slots:
    void on_btnInsert_clicked();

private:
    Ui::Insert *ui;
};

#endif // INSERT_H
