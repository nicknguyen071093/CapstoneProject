#ifndef SHOWCONTENT_H
#define SHOWCONTENT_H
#include <QString>
#include <QDebug>
#include <malloc.h>
#include <speak_lib.h>
class ShowContent
{
public:
    ShowContent();
    QString concatenateString(QString , QString );
    void speechString(QString);
};

#endif // SHOWCONTENT_H
