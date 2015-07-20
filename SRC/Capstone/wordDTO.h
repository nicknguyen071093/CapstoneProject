#ifndef WORDDTO_H
#define WORDDTO_H
#include <QString>

class WordDTO
{
public:
    WordDTO();
    WordDTO(int,QString,QString);
    void SetId(int );

    int GetId() const;

    void SetLink(QString );

    QString GetLink() const;

    void SetLetter(QString );

    QString GetLetter() const;
private:
    int id;
    QString letter;
    QString link;
};

#endif // WORDDTO_H
