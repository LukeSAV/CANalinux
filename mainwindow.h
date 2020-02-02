#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "monitorworkerthread.h"
#include "broadcasterworkerthread.h"
#include <QWidget>
#include <QMainWindow>
#include <thread>
#include <atomic>

namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();
    MonitorWorkerThread* monitor_worker;
    BroadcasterWorkerThread* broadcaster_worker;

public slots:
    void beginMonitoring();
    void stopMonitoring();
    void beginBroadcasting();
    void stopBroadcasting();
    void updateReadMsg(const QString& s);
    void setupDevices();
    void fileSelect();
    void updateCyclicState(bool);
    void updateWriteMsg();

private:
    Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
