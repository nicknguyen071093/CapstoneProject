#ifndef THREADDIALOG_H
#define THREADDIALOG_H

#include <QThread>
#include <QString>
class ThreadDialog : public QThread
{
    Q_OBJECT
public:
    explicit ThreadDialog(QObject *parent = 0);
    void run();
signals:
    void valueChanged(QString);

public slots:

};

#endif // THREADDIALOG_H
