#ifndef RECOGNITIONTIMERTHREAD_H
#define RECOGNITIONTIMERTHREAD_H

#include <QThread>

class RecognitionTimerThread : public QThread
{
    Q_OBJECT
public:
    explicit RecognitionTimerThread(QObject *parent = 0);

private:
    bool enableWorking;
signals:
    void sendSignalUpdatingContent();
    void sendSignalCountDownRecognitionTime(QString);
public slots:

};

#endif // RECOGNITIONTIMERTHREAD_H
