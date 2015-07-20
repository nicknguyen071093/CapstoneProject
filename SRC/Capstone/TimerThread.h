#ifndef TIMERTHREAD_H
#define TIMERTHREAD_H

#include <QThread>
#include <QMutex>

class TimerThread : public QThread
{
    Q_OBJECT
public:
    explicit TimerThread(QObject *parent = 0);
    bool STOP;
    void setToDefaults();

private:
    void run();
    bool isEnableToCountDown;
    //    int countDownAnalyzingBackgroundNumber;
signals:
    void sendSignalChangingToFrontHandMode();
    bool sendSignalToCheckFirst();
    void sendSignalChangingToBackHandMode();
    void sendSignalChangingToBinaryImage();
    void sendSignalCheckingFrontHand();
    void sendSignalCheckingBackHand();
    void sendSignalChangingLabelNotice(QString);
public slots:
    void continueCountDown();

};


#endif // TIMERTHREAD_H
