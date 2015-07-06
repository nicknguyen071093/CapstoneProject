#ifndef LETTERGET_H
#define LETTERGET_H

#include <QtSql>
#include <QString>
#include <QMap>


class LetterGet
{
public:
    LetterGet();
    QMap<int,QString> getLetter();
private:
    QSqlDatabase db;
};

#endif // LETTERGET_H
