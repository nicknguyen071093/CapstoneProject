#include "frmlist.h"
#include "ui_frmlist.h"

FrmList::FrmList(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrmList)
{
    ui->setupUi(this);
    FillTable();
}

FrmList::~FrmList()
{
    delete ui;
}

void FrmList::FillTable()
{
    LetterGet *wordDAO= new LetterGet();
    QMap <int,WordDTO> wordMap= wordDAO->getLetter();
    QMapIterator<int, WordDTO> iter(wordMap);
    int count= wordMap.size();
    int r=0;
    ui->tableWidget->setRowCount(count);

    while(iter.hasNext())
    {
        iter.next();
        ui->tableWidget->setItem(r,0,new QTableWidgetItem(QString::number(iter.key())));
        ui->tableWidget->setItem(r,1,new QTableWidgetItem(QString::number(iter.value().GetId())));
        ui->tableWidget->setItem(r,2,new QTableWidgetItem(iter.value().GetLetter()));
        ui->tableWidget->setItem(r,3,new QTableWidgetItem(iter.value().GetLink()));
        r++;
    }
    wordDAO->closeConnection();

}
void FrmList::on_pushButton_clicked()
{
    this->hide();
    QString s= ui->tableWidget->item(ui->tableWidget->currentRow(),3)->text();
    dialog = new ImageDialog(this);
    dialog->onsetImageLable(s);
    dialog->exec();
}
