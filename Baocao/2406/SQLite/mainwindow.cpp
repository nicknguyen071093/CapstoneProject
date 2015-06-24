#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "insert.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/seven7up/database.db3");

    if(!db.open()){
        qDebug() << db.lastError().text();
    }
    FillTable();

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::FillTable(){
    int numRow, r, c;
    QSqlQuery q(db);
    if(!q.exec("SELECT count(id) as numRow FROM person"))  qDebug() << q.lastError().text();
    q.first();
    numRow = q.value(0).toInt();
    ui->tableWidget->setRowCount(numRow);

    if(!q.exec("SELECT * FROM person"))  qDebug() << q.lastError().text();
    for (r = 0, q.first(); q.isValid(); q.next(),++r) {
        for (c = 0; c < 3; ++c) {
            ui->tableWidget->setItem(r,c,new QTableWidgetItem(q.value(c).toString()));
        }
    }
}

void MainWindow::on_btnDelete_clicked()
{
    int delId = ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text().toInt();

    QSqlQuery q(db);
    q.prepare("DELETE FROM person WHERE id = :id");
    q.bindValue(":id",delId);
    if(!q.exec()) qDebug() << q.lastError().text();
    FillTable();
}

void MainWindow::on_btnUpdate_clicked()
{
    int upId = ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text().toInt();
    QSqlQuery q(db);
    q.prepare("UPDATE person SET name =:name, age =:age WHERE id = :id");
    q.bindValue(":id",ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text().toInt());
    q.bindValue(":name",ui->tableWidget->item(ui->tableWidget->currentRow(),1)->text());
    q.bindValue(":age",ui->tableWidget->item(ui->tableWidget->currentRow(),2)->text().toInt());
    if(!q.exec()) qDebug() << q.lastError().text();
    FillTable();
}

void MainWindow::on_btnInsert_clicked()
{
    QSqlQuery q(db);
    q.prepare("INSERT INTO person (name,age) VALUES ('','')");
    if(!q.exec()) qDebug() << q.lastError().text();
    FillTable();

}
