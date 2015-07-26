#ifndef RECOGNITIONTIMERTHREAD_H
#define RECOGNITIONTIMERTHREAD_H

#include <QThread>
#include <QMutex>

class RecognitionTimerThread : public QThread
{
    Q_OBJECT
public:
    explicit RecognitionTimerThread(QObject *parent = 0);
    bool STOP;
    void enableWorking(bool);

private:
    void run();
    QMutex mutex;
    bool activeWorking;
signals:
    void sendSignalUpdatingContent();
    void sendSignalCountDownRecognitionTime(QString);
public slots:

};

#endif // RECOGNITIONTIMERTHREAD_H
