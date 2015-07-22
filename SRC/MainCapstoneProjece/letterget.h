#ifndef LETTERGET_H
#define LETTERGET_H

#include <QtSql>
#include <QString>
#include <QMap>
#include <wordDTO.h>

class LetterGet
{
public:
    LetterGet();
    QMap<int,WordDTO> getLetter();
    void closeConnection();
private:
    QSqlDatabase db;
};

#endif // LETTERGET_H
