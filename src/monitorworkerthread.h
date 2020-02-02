#ifndef MONITORWORKERTHREAD_H
#define MONITORWORKERTHREAD_H
#include <QThread>
#include <QObject>
#include <atomic>
#include <string>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/bcm.h>

class MonitorWorkerThread : public QThread
{
    Q_OBJECT
public:
    MonitorWorkerThread();
    MonitorWorkerThread(QObject*);
    virtual ~MonitorWorkerThread();
    void run() override;
    std::atomic<bool> stop;
    std::string iface_;
    unsigned int baudrate_;
    bool canfd_;
    void readCanFdMessage();
signals:
    void frameReady(const QString& s);
};

#endif // MONITORWORKERTHREAD_H
