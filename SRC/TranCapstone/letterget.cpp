#include "letterget.h"

LetterGet::LetterGet()
{
}
QString LetterGet::getLetter(int id)
{
    QString a;
    db = QSqlDatabase::addDatabase("QSQLITE");
     db.setDatabaseName("../ImageData.sqlite");
    QMap<int, QString> Map;
    if(!db.open()){
        qDebug() << "connect to database fail";
    }
    else{

        QSqlQuery q(db);
        q.exec("select * from (LinkImage join Alphabet) where (LinkImage.idLetter = Alphabet.id)");
        for ( q.first(); q.isValid(); q.next()) {
            Map.insert(q.value(2).toInt(),q.value(4).toString());
        }
        foreach(int i, Map.keys()) if(i= id) a = Map[i];
    }
    return a;
}
