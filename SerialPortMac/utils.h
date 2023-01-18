//
// Created by Aaron Ye on 1/8/23.
//

#ifndef SERIALPORT_UTILS_H
#define SERIALPORT_UTILS_H

#endif //SERIALPORT_UTILS_H

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "serialport.h"
#include <iostream>

#define WORD_BYTES 4
#define WRITE "w"
#define WAIT 25

namespace utils {

    bool find_args(const char* argv[], int argc, const char* arg);
    void upload_bin(SerialPort& sp, long long address, std::vector<unsigned char>& buffer) ;
    void read_bin(std::vector<unsigned char>& buffer, const std::string& path);
    void set_up_flash(SerialPort& sp);
    const char* getPort();
    void jump(SerialPort& sp, long long address);
}
