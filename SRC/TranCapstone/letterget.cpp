#include "letterget.h"

LetterGet::LetterGet()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../ImageData.sqlite");
    if(!db.open()){
        qDebug() << "connect to database fail";
    }
}


QMap<int, QString> LetterGet::getLetter()
{
    QMap<int, QString> mapWords;
    QSqlQuery q(db);
    q.exec("select * from Alphabet");
    for ( q.first(); q.isValid(); q.next()) {
        mapWords.insert(q.value(0).toInt(),q.value(1).toString());
    }
    return mapWords;
}
