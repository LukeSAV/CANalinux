#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QWidget>
#include <QFileDialog>

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <string>

#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>

#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/bcm.h>
#include <linux/can/raw.h>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QString title = QString("canalinux");
    this->setWindowTitle(title);
    ui->setupUi(this);
    QStringList baudrate_list = {"125000", "250000", "500000"};
    this->ui->combo_baudrate->addItems(baudrate_list);
    this->ui->label_ifaceerror->hide();
    this->ui->checkbox_canfd->setChecked(true);
    this->broadcaster_worker = nullptr;
    this->monitor_worker = nullptr;

    QObject::connect(this->ui->pushbutton_monitor, SIGNAL(clicked()), this, SLOT(beginMonitoring()));
    QObject::connect(this->ui->pushbutton_stopmonitor, SIGNAL(clicked()), this, SLOT(stopMonitoring()));
    QObject::connect(this->ui->pushbutton_write, SIGNAL(clicked()), this, SLOT(beginBroadcasting()));
    QObject::connect(this->ui->pushbutton_stopwrite, SIGNAL(clicked()), this, SLOT(stopBroadcasting()));
    QObject::connect(this->ui->pushbutton_devicesetup, SIGNAL(clicked()), this, SLOT(setupDevices()));
    QObject::connect(this->ui->file_menu, SIGNAL(triggered(QAction*)), this, SLOT(fileSelect()));
    QObject::connect(this->ui->checkbox_cyclicwrite, SIGNAL(clicked(bool)), this, SLOT(updateCyclicState(bool)));
    QObject::connect(this->ui->textedit_id, SIGNAL(textChanged()), this, SLOT(updateWriteMsg()));
    QObject::connect(this->ui->textedit_data, SIGNAL(textChanged()), this, SLOT(updateWriteMsg()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::beginMonitoring()
{
    std::string iface = this->ui->combo_devicelist->currentText().toStdString();
    unsigned int baudrate = this->ui->combo_baudrate->currentText().toUInt();
    bool canfd = this->ui->checkbox_canfd->checkState() == Qt::Checked;
    if(iface == "")
    {
        std::cerr << "No interface selected" << std::endl;
        this->ui->label_ifaceerror->show();
        return;
    }
    this->setupDevices();
    this->ui->label_ifaceerror->hide();
    this->monitor_worker = new MonitorWorkerThread(this);
    this->monitor_worker->stop = false;
    this->monitor_worker->iface_ = iface;
    this->monitor_worker->baudrate_ = baudrate;
    this->monitor_worker->canfd_ = canfd;

    QObject::connect(this->monitor_worker, &MonitorWorkerThread::finished, this->monitor_worker, &QObject::deleteLater);
    QObject::connect(this->monitor_worker, &MonitorWorkerThread::frameReady, this, &MainWindow::updateReadMsg);

    this->ui->pushbutton_monitor->setEnabled(false);
    this->monitor_worker->start();
    this->ui->pushbutton_stopmonitor->setEnabled(true);
}

void MainWindow::stopMonitoring()
{
    this->ui->pushbutton_stopmonitor->setEnabled(false);
    this->monitor_worker->stop = true;
    this->ui->pushbutton_monitor->setEnabled(true);
}

void MainWindow::beginBroadcasting()
{
    std::cout << "Broadcast thread" << std::endl;
    std::string iface = this->ui->combo_devicelist->currentText().toStdString();
    this->broadcaster_worker = new BroadcasterWorkerThread(this);
    this->broadcaster_worker->stop = false;
    this->broadcaster_worker->id_to_write_ = this->ui->textedit_id->toPlainText().toStdString();
    this->broadcaster_worker->data_to_write_ = this->ui->textedit_data->toPlainText().toStdString();
    this->broadcaster_worker->is_cyclic_ = this->ui->checkbox_cyclicwrite->isChecked();
    this->broadcaster_worker->cycle_rate_ = this->ui->spinbox_refreshrate->value();

    QObject::connect(this->broadcaster_worker, &BroadcasterWorkerThread::finished, this->broadcaster_worker, &QObject::deleteLater);
    QObject::connect(this->broadcaster_worker, &BroadcasterWorkerThread::broadcastComplete, this, &MainWindow::stopBroadcasting);

    this->ui->pushbutton_write->setEnabled(false);
    this->broadcaster_worker->start();
    this->ui->pushbutton_stopwrite->setEnabled(true);
}

void MainWindow::stopBroadcasting()
{
    this->ui->pushbutton_stopwrite->setEnabled(false);
    this->broadcaster_worker->stop = true;
    this->ui->pushbutton_write->setEnabled(true);
}

void MainWindow::updateReadMsg(const QString& s)
{
    std::cout << "Read message" << std::endl;
    std::cout << s.toStdString() << std::endl;
    this->ui->label_canoutput->setText(s);
}

void MainWindow::updateWriteMsg()
{
    std::cout << "Updating message to write" << std::endl;
    if(this->broadcaster_worker)
    {
        std::cout << "Changing fields of broadcast worker" << std::endl;
        this->broadcaster_worker->id_to_write_ = this->ui->textedit_id->toPlainText().toStdString();
        this->broadcaster_worker->data_to_write_ = this->ui->textedit_data->toPlainText().toStdString();

    }
}

void MainWindow::setupDevices()
{
    std::string iface = this->ui->combo_devicelist->currentText().toStdString();
    unsigned int baudrate = this->ui->combo_baudrate->currentText().toUInt();
    bool canfd = this->ui->checkbox_canfd->checkState() == Qt::Checked;

    std::cout << "Setting up interfaces" << std::endl;

    std::string command = "sudo ip link set can0 down";
    system(command.c_str());
    command = "sudo ip link set can1 down";
    system(command.c_str());

    if(canfd) {
        command = "sudo ip link set can0 type can bitrate " + std::to_string(baudrate) + " dbitrate 8000000 fd on";
        system(command.c_str());
        command = "sudo ip link set can1 type can bitrate " + std::to_string(baudrate) + " dbitrate 8000000 fd on";
        system(command.c_str());
    }
    else {
        command = "sudo ip link set can0 type can bitrate " + std::to_string(baudrate);
        system(command.c_str());
        command = "sudo ip link set can1 type can bitrate " + std::to_string(baudrate);
        system(command.c_str());
    }
    command = "sudo ip link set can0 up";
    system(command.c_str());
    command = "sudo ip link set can1 up";
    system(command.c_str());
}

void MainWindow::fileSelect()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open DBC File"), "/home/", tr("CAN Database Files (*.dbc)"));
    std::cout << "File loaded: " << filename.toStdString() << std::endl;
}

void MainWindow::updateCyclicState(bool checked)
{
    if(checked)
    {
        this->ui->spinbox_refreshrate->setEnabled(true);
        this->ui->label_hz->setEnabled(true);
    }
    else
    {
        this->ui->label_hz->setEnabled(false);
    }
}
