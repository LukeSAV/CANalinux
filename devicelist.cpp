#include "devicelist.h"
#include <iostream>
#include <net/if.h>
#include <string>

DeviceList::DeviceList()
{
}

DeviceList::~DeviceList()
{
}

std::vector<std::string> DeviceList::getIFaces()
{
    std::vector<std::string> iface_names;
    struct if_nameindex *if_nidxs, *intf;
    if_nidxs = if_nameindex();
    if (if_nidxs != nullptr)
    {
        for(intf = if_nidxs; intf->if_index != 0 || intf->if_name != nullptr; intf++)
        {
            std::string if_name = std::string(intf->if_name);

            if(if_name.compare(0,3,"can") == 0) {
                iface_names.push_back(if_name);
            }
        }
        if_freenameindex(if_nidxs);
    }
    return iface_names;
}
