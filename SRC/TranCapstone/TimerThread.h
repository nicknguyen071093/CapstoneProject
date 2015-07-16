#ifndef TIMERTHREAD_H
#define TIMERTHREAD_H

#include <QThread>
#include <QMutex>

class TimerThread : public QThread
{
    Q_OBJECT
public:
    explicit TimerThread(QObject *parent = 0);

private:
    void run();
    bool isBackgroundNotChanged;
signals:

public slots:

};

#endif // TIMERTHREAD_H
