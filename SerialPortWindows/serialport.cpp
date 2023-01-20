//
// Created by Aaron Ye on 1/7/23.
//

// g++ -std=c++11 -w -D_WIN32_WINNT=0x0501 -I ..\..\boost_1_81_0 -I ..\..\mingw_threads serialport.cpp upload.cpp utils.cpp -o upload -lws2_32

#include "serialport.h"
#include "windows.h"
#include <iostream>

void flush_serial_port(boost::asio::serial_port& serial_port, boost::system::error_code& error) {
    // if (0 == ::tcflush(serial_port.lowest_layer().native_handle(), what)) {
    if (0 == PurgeComm(serial_port.lowest_layer().native_handle(), PURGE_RXCLEAR | PURGE_TXCLEAR)) {
        error = boost::system::error_code();
    }
    else {
        error = boost::system::error_code(errno,boost::asio::error::get_system_category());
    }
}

bool SerialPort::open_port(const char *port_name, int baud_rate) {
    // if(port) {
    //     cout << "Error: port is already open!\n";
    //     return 0;
    // }
    port = new serial_port (io_service);
    boost::system::error_code ec;
    port->open(port_name, ec);
    if(ec) {
        cout << "Error: "<< port_name << " failed to open! Error = " << ec.message() << "\n";
        return 0;
    }
    port->set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    port->set_option(boost::asio::serial_port_base::character_size(8));
    port->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    port->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    port->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    flush_serial_port(*port, ec);

    read_bytes();
    std::thread t( [this] () {this->io_service.run(); });
    // thread_.swap(t);
    std::swap(thread_,t);

    cout << "port opened!\n";
    return 1;
}

bool SerialPort::close_port() {
    if(port) {
        port->cancel();
        port->close();
    }
    if (thread_.joinable()) {
        thread_.join();
    }
    io_service.stop();
    cout << "port closed\n";
    return 1;
}

int SerialPort::write_bytes(const char *buf, const int &size) {
    boost::system::error_code ec;

    if (!port) return -1;
    if (size == 0) return 0;

    int w = port->write_some(boost::asio::buffer(buf, size), ec);
    assert (w > 0);
    return w;
}

void SerialPort::on_receive(const boost::system::error_code& ec, size_t bytes_transferred) {
    if (!port->is_open()) return;
    if (ec) {
        read_bytes();
        return;
    }
    for (unsigned int i = 0; i < bytes_transferred; ++i) {
        char c = buffer[i];
        printf("%c",c);
        last_read = c;
    }
    read_bytes();
}

void SerialPort::read_bytes() {
    if (!port->is_open()) return;
    port->async_read_some(
            boost::asio::buffer(buffer, BUFFER_SIZE),
            boost::bind(
                    &SerialPort::on_receive,
                    this, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}
