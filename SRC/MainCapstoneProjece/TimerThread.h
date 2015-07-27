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
    void sendSignalChangingToBackHandMode();
    void sendSignalChangingToBinaryImage();
     void sendSignalChangingToTestingTime();
     void sendSignalFinishingColorSubtraction(bool);

    void sendSignalCheckingFrontHand();
    void sendSignalCheckingBackHand();

    bool sendSignalGetTestingResult();

    void sendSignalChangingLabelNotice(QString,QString);
    void sendSignalFailTesingResult(QString,QString);

public slots:
    void continueCountDown();

};


#endif // TIMERTHREAD_H
