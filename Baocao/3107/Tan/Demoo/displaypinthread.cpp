#include "displaypinthread.h"
#define PIN07 RPI_V2_GPIO_P1_07
#define PIN11 RPI_V2_GPIO_P1_11
#define PIN13 RPI_V2_GPIO_P1_13
#define PIN15 RPI_V2_GPIO_P1_15

DisplayPINThread::DisplayPINThread(QObject *parent) :
    QThread(parent)
{
}
void DisplayPINThread::run()
{

        //QMutex mutex;
        // prevent other threads from changing the "Stop" value
        //mutex.lock();
        //if(this->Stop) break;
       // mutex.unlock();       
        if (!bcm2835_init()){  return;}
            // Set RPI pin to be an input
            bcm2835_gpio_fsel(PIN07, BCM2835_GPIO_FSEL_INPT);
            bcm2835_gpio_fsel(PIN11, BCM2835_GPIO_FSEL_INPT);
            bcm2835_gpio_fsel(PIN13, BCM2835_GPIO_FSEL_INPT);
            bcm2835_gpio_fsel(PIN15, BCM2835_GPIO_FSEL_INPT);
            while (true) {
                int value1 = bcm2835_gpio_lev(PIN07);
                int value2 = bcm2835_gpio_lev(PIN11);
                int value3 = bcm2835_gpio_lev(PIN13);
                int value4 = bcm2835_gpio_lev(PIN15);
                int value= value1 + value2 + value3 + value4;
                emit valueChanged(value);
                this->msleep(3000);
            }
                bcm2835_close();

}
