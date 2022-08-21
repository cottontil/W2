//
// Created by Aaron Ye on 7/31/22.
//

#ifndef W2_SERIALPORT_H
#define W2_SERIALPORT_H

////SerialPort.hpp
//
//#ifndef __SERIAL_PORT_HPP__
//#define __SERIAL_PORT_HPP__


#include <unistd.h> //ssize_t



int openAndConfigureSerialPort(const char* portPath, int baudRate);



bool serialPortIsOpen();



ssize_t flushSerialData();



ssize_t writeSerialData(const unsigned char* bytes, size_t length);



ssize_t readSerialData(unsigned char* bytes, size_t length);



ssize_t closeSerialPort(void);



int getSerialFileDescriptor(void);


#endif //W2_SERIALPORT_H

//#endif //__SERIAL_PORT_HPP__