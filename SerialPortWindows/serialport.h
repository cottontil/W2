//
// Created by Aaron Ye on 1/7/23.
//

#ifndef SERIALPORT_SERIALPORT_H
#define SERIALPORT_SERIALPORT_H

#endif //SERIALPORT_SERIALPORT_H

#pragma once

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
// #include <boost/bind.hpp>
# include <boost/bind/bind.hpp> 
#include <boost/thread.hpp>
// #include <thread>
#include "mingw.thread.h"
#include <string>
#include <vector>

using namespace std;

typedef boost::asio::serial_port serial_port;

#define BUFFER_SIZE 512

struct SerialPort {
    boost::asio::io_service io_service;
    serial_port *port;
    thread thread_;
    char buffer[BUFFER_SIZE];
    string read_buf_str;

public:
    char last_read = 0x0;

    bool open_port(const char *port_name, int baud_rate = 9600);

    bool close_port();

    int write_bytes(const char *buf, const int &size);

    void flush();

    void read_bytes();

    void on_receive(const boost::system::error_code &ec, size_t bytes_transferred);
};
