#include "letterget.h"

LetterGet::LetterGet()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../ImageData.sqlite");
    if(!db.open()){
        qDebug() << "connect to database fail";
    }
}
void LetterGet::closeConnection(){
    QString connection;
        connection = db.connectionName();
        db.close();
        db = QSqlDatabase();
        db.removeDatabase(connection);
}
QMap<int, WordDTO> LetterGet::getLetter()
{

    QMap<int, WordDTO> mapWords;
    QSqlQuery q(db);
    q.exec("select * from Alphabet");
    for ( q.first(); q.isValid(); q.next()) {
        WordDTO object(q.value(0).toInt(), q.value(1).toString(),q.value(2).toString());
        mapWords.insert(q.value(0).toInt(),object);
    }

    return mapWords;
}
