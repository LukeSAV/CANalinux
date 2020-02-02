#include "broadcasterworkerthread.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <string>
#include <sys/socket.h>

#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/bcm.h>


BroadcasterWorkerThread::BroadcasterWorkerThread()
{
}

BroadcasterWorkerThread::BroadcasterWorkerThread(QObject* parent) : QThread(parent)
{
}

BroadcasterWorkerThread::~BroadcasterWorkerThread()
{
}

void BroadcasterWorkerThread::writeCanFdMessage()
{
    std::string iface = this->iface_;

    if(iface == "can0")
    {
        iface = "can1";
    } else
    {
        iface = "can0";
    }

    int s;
    int enable_canfd = 1;
    struct sockaddr_can addr;
    struct canfd_frame frame;
    struct ifreq ifr;

    /* open socket */
    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        return;
    }

    strcpy(ifr.ifr_name, iface.c_str());
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd)))
    {
        printf("error when enabling CAN FD support\n");
        return;
    }

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind failed");
        return;
    }
    std::string id_to_write = id_to_write_;
    std::string data_to_write = data_to_write_;

    if(!(id_to_write.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos))
    {
        std::cerr << "Payload doesn't conform to hexadecimal notation" << std::endl;
        return;
    }

    frame.can_id = std::stoul("0x" + id_to_write, nullptr, 16);

    if(data_to_write.length() % 2)
    {
        std::cerr << "Payload length must be evenly divisible by 2" << std::endl;
        return;
    }
    if(data_to_write.length() > 128 || data_to_write.length() == 0)
    {
        std::cerr << " Payload length invalid" << std::endl;
        return;
    }
    if(!(data_to_write.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos))
    {
        std::cerr << "Payload doesn't conform to hexadecimal notation" << std::endl;
        return;
    }
    frame.len = data_to_write.length() / 2;

    /* Pack data into frame */
    for(unsigned int i = 0; i < data_to_write.length(); i+=2)
    {
        std::string byte = "0x" + data_to_write.substr(i, 2);
        frame.data[i/2] = std::stoi(byte, nullptr, 16);
    }

    write(s, &frame, sizeof(struct canfd_frame));
}


void BroadcasterWorkerThread::run()
{
    do {
        writeCanFdMessage();
        usleep(1000000.0f / static_cast<float>(this->cycle_rate_));
    } while(!this->stop && this->is_cyclic_);
    emit this->broadcastComplete();
}
