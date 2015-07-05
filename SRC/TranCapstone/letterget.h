#ifndef LETTERGET_H
#define LETTERGET_H

#include <QtSql>
#include <QString>
#include <QMap>


class LetterGet
{
public:
    LetterGet();
    QSqlDatabase db;
    QString getLetter(int);
};

#endif // LETTERGET_H
