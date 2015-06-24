#include "insert.h"
#include "ui_insert.h"

Insert::Insert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Insert)
{
    ui->setupUi(this);
}

Insert::~Insert()
{
    delete ui;
}

void Insert::on_btnInsert_clicked()
{
    QString name= ui->txtName->text();
    int age = ui->txtAge->text().toInt();
    qDebug() << age;
    QSqlQuery q(main.db);
    q.prepare("INSERT INTO person (name,age) VALUES ('"+name+"', ':age')");
    //q.bindValue(":name",name);
    //q.bindValue(":age",age);
    //if(!q.exec()) qDebug() << q.lastError().text();
    main.FillTable();
}
