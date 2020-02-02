#ifndef DEVICELIST_H
#define DEVICELIST_H
#include <string>
#include <vector>


class DeviceList
{
public:
    DeviceList();
    virtual ~DeviceList();
    std::vector<std::string> iface_list;
    std::vector<std::string> getIFaces();
};

#endif // DEVICELIST_H
