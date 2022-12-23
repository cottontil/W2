//
// Created by Aaron Ye on 8/13/22.
//

#include "serialport.h"
#include "writeUtils.h"
#include <thread>
#include <sstream>
#include <iostream>

namespace writeUtils {

    void writeBytes(long long address, unsigned char* buffer, int bytes) {
        for(int offset = 0; offset < bytes; offset += WORD_BYTES) {
            writeWord(address, buffer + offset);
            address += WORD_BYTES;
        }
    }

    std::string byteToAsciiString(unsigned char c) {
        int ascii_val = static_cast<int>(c);
        std::stringstream hex_value;
        if(ascii_val <= 0xf) hex_value << "0";
        hex_value << std::hex << ascii_val;
        return hex_value.str();
    }

    void writeWord(long long address, unsigned char* word) {
        std::stringstream stream;
        stream << WRITE << std::hex << address << " " << 1 << " ";
        for(int offset = WORD_BYTES-1; offset >= 0; offset--) {
            stream << byteToAsciiString(*(word + offset));
        }
        stream << "\r";
        std::string bytes = stream.str();
        writeSerialData(reinterpret_cast<const unsigned char*>(bytes.c_str()), bytes.size());
        std::cout << bytes << "\n";
        std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
    }

    const char* formattedReadInstruction(long long address, int bytes) {
        std::stringstream stream;
        stream << READ << std::hex << address << " " << bytes << " \r";
        std::cout << "instruction from stream = " << stream.str() << "\n";
        return stream.str().c_str();
    }

    void uploadBin(long long address, std::vector<unsigned char>& buffer) {
        writeBytes(address, &buffer[0], buffer.size());
    }
}