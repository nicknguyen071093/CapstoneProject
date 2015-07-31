#include <QCoreApplication>
#include <showcontent.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString mu="MU";
    QString rau="RAU";
    QString str= "Kho";
    ShowContent show;
    QString m = show.concatenateString(mu,str);
    m=m.append("ng");
    qDebug() << m;
    show.speechString(m);
    return a.exec();
}
