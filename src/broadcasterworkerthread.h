#ifndef BROADCASTERWORKERTHREAD_H
#define BROADCASTERWORKERTHREAD_H
#include <QThread>
#include <QObject>
#include <atomic>
#include <string>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/bcm.h>


class BroadcasterWorkerThread : public QThread
{
    Q_OBJECT
public:
    BroadcasterWorkerThread();
    BroadcasterWorkerThread(QObject*);
    virtual ~BroadcasterWorkerThread();
    void run() override;
    void writeCanFdMessage();

    std::atomic<bool> stop;
    bool is_cyclic_;
    int cycle_rate_;
    std::string iface_;
    std::string id_to_write_;
    std::string data_to_write_;
signals:
    void broadcastComplete();

};

#endif // BROADCASTERWORKERTHREAD_H
