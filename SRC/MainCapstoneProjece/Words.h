#ifndef WORDS_H
#define WORDS_H

#include <QtSql>
#include <QMap>
#include <QStringList>
#include <WordDTO.h>

class Words
{
public:
    Words();
    QStringList getWords();
    QString getLinkByID(int);
    QString getMeanByID(int);
private:
    QMap<int, WordDTO> wordsMap;

};

#endif // WORDS_H
