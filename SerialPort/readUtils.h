//
// Created by Aaron Ye on 8/12/22.
//

#ifndef W2_READUTILS_H
#define W2_READUTILS_H

#include "serialport.h"
#include <fstream>

#define BUFFER_SIZE 500
#define CAPACITY 100

namespace readUtils {

    void readConsecutiveBytes(unsigned char* bytes, long long address, int length);

    void readWordFromPort(unsigned char* bytes);

    int readByteFromPort();

    int readFromPort(unsigned char* bytes);

    void setupFlash();

    int getBinSize(std::ifstream& stream);

    void readBin(unsigned char* buffer, std::ifstream& stream, int bytes);
}

#endif //W2_READUTILS_H