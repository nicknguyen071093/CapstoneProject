#ifndef DISPLAYPINTHREAD_H
#define DISPLAYPINTHREAD_H

#include <QThread>
#include <bcm2835.h>
#define PIN07 RPI_V2_GPIO_P1_07
#define PIN11 RPI_V2_GPIO_P1_11
#define PIN13 RPI_V2_GPIO_P1_13
#define PIN15 RPI_V2_GPIO_P1_15
class DisplayPINThread : public QThread
{
    Q_OBJECT
public:
    explicit DisplayPINThread(QObject *parent = 0);
    void run();
signals:
    void valueChanged(int);

public slots:

};

#endif // DISPLAYPINTHREAD_H
