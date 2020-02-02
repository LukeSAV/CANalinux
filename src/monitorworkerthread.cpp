#include "monitorworkerthread.h"
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

#define MSGID (0x0BC)
#define NFRAMES 1


MonitorWorkerThread::MonitorWorkerThread()
{
}

MonitorWorkerThread::MonitorWorkerThread(QObject* parent) : QThread(parent)
{
}

MonitorWorkerThread::~MonitorWorkerThread()
{
}

void MonitorWorkerThread::readCanFdMessage() {
    const char* iface = iface_.c_str();
    int rc;

    struct sockaddr_can addr;
    struct ifreq ifr;
    int sockfd;

    int enable = 1;

    sockfd = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (-1 == sockfd) {
        std::perror("socket");
    }
    /* Enable reception of CAN FD frame */
    rc = ::setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable, sizeof(enable));

    if (-1 == rc) {
        std::perror("setsockopt CAN FD");
    }

    /* Get the index of the network interface */
    std::strncpy(ifr.ifr_name, iface, IFNAMSIZ);
    if (::ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1) {
        std::perror("ioctl");
    }

    /* Bind the socket to the network interface */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    rc = ::bind(sockfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));

    struct canfd_frame frame;

    int nbytes = ::read(sockfd, &frame, CANFD_MTU);
    std::cout << "Read " << nbytes << " bytes" << std::endl;
    std::cout << "ID: " << frame.can_id << ", Data: ";
    for(unsigned int i = 0; i < frame.len; i++) {
        printf("%x ", frame.data[i]);
    }
    std::cout << std::endl;

    std::stringstream hex_stream_out;
    hex_stream_out << "ID: " << std::hex << static_cast<int>(frame.can_id) << std::endl;
    hex_stream_out << "Payload: ";
    for(unsigned int i = 0; i < frame.len; ++i) {
        if(i % 15 == 0) {
            hex_stream_out << std::endl;
        }
        hex_stream_out << std::hex << static_cast<int>(frame.data[i]);
        hex_stream_out << " ";
    }
    hex_stream_out << std::endl;
    std::string frame_str = hex_stream_out.str();

    emit frameReady(frame_str.c_str());
}

void MonitorWorkerThread::run()
{
    std::cout << "Started monitoring with options Iface: " << this->iface_ << ", BaudRate: " << this->baudrate_ << ", CAN FD: " << this->canfd_ << std::endl;
    while(!this->stop)
    {
        readCanFdMessage();
        usleep(100);
    }
}
