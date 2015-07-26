#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FillTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::FillTable(){
    LetterGetting *wordDAO= new LetterGetting();
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
    for (int i = 0; i < wordMap.size(); i++) {
        ui->listWidget->addItem(wordMap[i].GetLetter());
    }
        ui->listWidget->setCurrentRow(0);
        ui->label->setText(QString::number(ui->listWidget->count()));
    wordDAO->closeConnection();
}
void MainWindow::on_pushButton_clicked()
{

    this->hide();
    QString s= ui->tableWidget->item(ui->tableWidget->currentRow(),3)->text();
    dialog = new Dialog(this);
    dialog->onsetImageLable(s);
    dialog->exec();



}


void MainWindow::on_pushButton_2_clicked()
{
    int rowNumber = ui->listWidget->currentRow() -1;
    if (rowNumber >= 0) {
        ui->listWidget->setCurrentRow(rowNumber);
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    int rowNumber = ui->listWidget->currentRow() + 1;
    if (rowNumber < ui->listWidget->count()) {
        ui->listWidget->setCurrentRow(rowNumber);
    }
}
