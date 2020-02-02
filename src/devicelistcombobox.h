#ifndef DEVICELISTCOMBOBOX_H
#define DEVICELISTCOMBOBOX_H
#include <QComboBox>
#include <devicelist.h>

class DeviceListComboBox : public QComboBox
{
public:
    DeviceListComboBox(QWidget* parent);
    virtual ~DeviceListComboBox();
    void showPopup() override;
    DeviceList device_list;
};

#endif // DEVICELISTCOMBOBOX_H
