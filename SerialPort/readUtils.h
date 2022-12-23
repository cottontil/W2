//
// Created by Aaron Ye on 8/12/22.
//

#ifndef W2_READUTILS_H
#define W2_READUTILS_H

#include "serialport.h"
#include <fstream>
#include <vector>
#include <string>

#define BUFFER_SIZE 500
#define CAPACITY 100

namespace readUtils {

    void readAddress(unsigned char* bytes, long long address, int length);

    unsigned char readByteFromPort();

    int readFromPort(unsigned char* bytes);

    int readResponseFromPort(unsigned char* bytes);

    void setupFlash();

    int getBinSize(std::ifstream& stream);

    void readBin(std::vector<unsigned char>& buffer, const std::string& path);
}

#endif //W2_READUTILS_H