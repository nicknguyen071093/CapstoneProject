#include "Words.h"

Words::Words()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../ImageData.sqlite");
    if(!db.open()) {
        qDebug() << "connect to database fail";
        exit(1);
    }
    QSqlQuery q(db);
    q.exec("select * from Alphabet");
    for ( q.first(); q.isValid(); q.next()) {
        WordDTO object(q.value(0).toInt(), q.value(1).toString(),q.value(2).toString());
        wordsMap.insert(q.value(0).toInt(),object);
    }
    QString connection = db.connectionName();
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(connection);
}

QStringList Words::getWords() {
    QStringList words;
    for (int i = 0; i < wordsMap.count(); i++) {
        words << wordsMap[i].GetLetter();
    }
    return words;
}

QString Words::getLinkBySelectedIndex(int index) {
    return wordsMap.values()[index].GetLink();
}

QString Words::getMeanByID(int wordID) {
    return wordsMap.value(wordID).GetLetter();
}




