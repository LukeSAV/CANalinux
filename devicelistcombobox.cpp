#include "devicelistcombobox.h"
#include "devicelist.h"

#include <QStringList>

#include <iostream>
#include <stdio.h>
#include <array>
#include <memory>
#include <stdexcept>

DeviceListComboBox::DeviceListComboBox(QWidget* parent) : QComboBox(parent)
{
    this->device_list = DeviceList();
}

DeviceListComboBox::~DeviceListComboBox()
{

}

void DeviceListComboBox::showPopup()
{
    this->clear();
    int chan_num = 0;
    while(chan_num < 4) // Check for up to 4 connected devices
    {
        std::string command = "sudo ip link set can" + std::to_string(chan_num) + " type can bitrate 125000 dbitrate 8000000 fd on";
        system(command.c_str());
        command = "suso ip link set can" + std::to_string(chan_num) + " up";
        chan_num++;
    }
    std::vector<std::string> ifaces = this->device_list.getIFaces();
    if (ifaces.size())
    {
        for(std::string iface : ifaces)
        {
            QString q_iface = QString(iface.c_str());
            this->addItem(q_iface);
        }
    }
    QComboBox::showPopup();
}
