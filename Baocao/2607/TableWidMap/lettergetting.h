#ifndef LETTERGETTING_H
#define LETTERGETTING_H
#include <QtSql>
#include <QString>
#include <QMap>
#include <worddto.h>

class LetterGetting
{
public:
    LetterGetting();
    QMap<int,WordDTO> getLetter();
    void closeConnection();
private:
    QSqlDatabase db;
};


#endif // LETTERGETTING_H
