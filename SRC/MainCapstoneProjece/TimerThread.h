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

private:
    void run();
    QMutex mutex;
    bool isEnableToCountDown;
    bool testingResult;
    //    int countDownAnalyzingBackgroundNumber;
signals:
    void sendSignalChangingToFrontHandMode();
    bool sendSignalToCheckFirst();
    void sendSignalChangingToBackHandMode();
    void sendSignalChangingToBinaryImage();
    void sendSignalCheckingFrontHand();
    void sendSignalChangingToTestingTime();
    bool sendSignalGetTestingResult();
    void sendSignalFinishingColorSubtraction(bool);
    void sendSignalCheckingBackHand();
    void sendSignalChangingLabelNotice(QString,QString);
public slots:
    void continueCountDown();

};


#endif // TIMERTHREAD_H
