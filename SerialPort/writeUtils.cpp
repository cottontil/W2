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
        for(int offset = 3; offset < bytes; offset += WORD_BYTES) {
            printf("writing to address %llx, offset = %d\n", address, offset);
            printf("char at buffer[%d] = %x\n", offset, int(*(buffer + offset)));
            writeWord(address, buffer + offset);
            address += WORD_BYTES;
        }
    }

    const char* byteToAsciiString(unsigned char c) {
        int asciiVal = static_cast<int>(c);
        std::stringstream stream;
        if(asciiVal < 16) stream << "0";
        stream << std::hex << asciiVal;
        return stream.str().c_str();
    }

    int asciiStringToInt(unsigned char* bytes) {
        printf("coverting bytes[0] = %x, bytes[1] = %x\n", int(bytes[0]), int(bytes[1]));
        std::stringstream stream; stream << std::hex << bytes;
        int val; stream >> val;
        return val;
    }

    void writeWord(long long address, unsigned char* word) {
        std::stringstream stream;
        stream << WRITE << std::hex << address << " 1 ";
        const char* instruction = stream.str().c_str();
        writeSerialData(reinterpret_cast<const unsigned char*>(instruction), std::strlen(instruction));
        std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));

        for(int offset = 0; offset < WORD_BYTES; offset++) {
            // read little endian format
            const char* hexVal = byteToAsciiString(*(word - offset));
            printf("byte %x to hex %x%x\n", int(*(word - offset)), int(hexVal[0]), int(hexVal[1]));
            writeSerialData(reinterpret_cast<const unsigned char*>(hexVal), strlen(hexVal));
            std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
        }
        unsigned char carriageReturn = '\r';
        printf("\nsending carriage return\n\n");
        writeSerialData(&carriageReturn, 1);
        std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
    }

    const char* formattedReadInstruction(long long address) {
        std::stringstream stream;
        stream << READ << std::hex << address << " 1\r";
        std::cout << "instruction from stream = " << stream.str() << "\n";
        return stream.str().c_str();
    }
}