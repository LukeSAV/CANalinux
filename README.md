# CANalinux
This repository contains a simple utility for reading/writing single CAN FD frames using [SocketCAN](https://en.wikipedia.org/wiki/SocketCAN).

Tested with Qt 5.12.6.

Additional dependencies:
```bash
$ apt-get install build-essential
$ apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```
Then simply clone this repository and add the GCC/G++ compiler to your Kit in Qt Creator or preferred IDE.


## Screenshots
![CANalinux home page](https://github.com/LukeSAV/canalinux/blob/master/img/canalinux_home.png)


## WIP
  * Add DBC file loader and parser
  * Add graph visualizer of data 
  * Add data logger capability
