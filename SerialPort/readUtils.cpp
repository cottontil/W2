//
// Created by Aaron Ye on 8/12/22.
//

#include "readUtils.h"
#include "writeUtils.h"
#include <thread>
#include <iostream>

#define CAPACITY 100

using namespace std;

namespace readUtils {

    void readConsecutiveBytes(unsigned char* bytes, long long address, int length) {
        for(int offset = 0; offset < length; offset += WORD_BYTES) {
            const char* instr = writeUtils::formattedReadInstruction(address);
            writeSerialData(reinterpret_cast<const unsigned char*>(instr), strlen(instr));
            std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
            readWordFromPort(bytes + offset);
            address += WORD_BYTES;
        }
    }

    void readWordFromPort(unsigned char* bytes) {
        int read = 0;
        unsigned char dummy;
        while(!read) {
            read = readSerialData(&dummy, 1);
            std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
        }
        printf("dummy = %x\n", int(dummy));
        for(int offset = 0; offset < WORD_BYTES; offset++) {
            int val = readByteFromPort();
            printf("value = %x\n", val);
            bytes[offset] = val;
        }
    }

    int readByteFromPort() {
        int read = 0;
        unsigned char buffer[2];
        while(!read) {
            read = readSerialData(buffer, 3);
            std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
        }
        printf("read %0x %0x\n", buffer[0], buffer[1]);
        return writeUtils::asciiStringToInt(buffer);
    }

    int readFromPort(unsigned char* bytes) {
        // to do implement timeout
        int offset = 0; bool started = 0; int read = 0;
        while(!started || read) {
            read = readSerialData(bytes + offset, CAPACITY);
            if(read > 0) started = 1;
            offset += read;
            std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
        }
        return offset;
    }

    void setupFlash() {
        const char* a = "a";
        writeSerialData(reinterpret_cast<const unsigned char*>(a), strlen(a));

        {
            unsigned char response[BUFFER_SIZE];
            int offset = readFromPort(response);
            printf("read byte = %d\n%s\n\n", offset, response);
        }

        {
            unsigned char response[BUFFER_SIZE];
            const char* instr = "w1f800b07 1 a5\r";
            const unsigned char* unsigned_instr = reinterpret_cast<const unsigned char*>(instr);
            writeSerialData(unsigned_instr, strlen(instr));
            int offset = readFromPort(response);
            printf("read byte = %d ascii value = %x\n", offset, response[0]);
        }

        {
            unsigned char response[BUFFER_SIZE];
            const char* instr = "r10300000 1\r";
            writeSerialData(reinterpret_cast<const unsigned char*>(instr), strlen(instr));
            int offset = readFromPort(response);
//            for(int i = 0; i < 11; i++) {
            printf("read byte = %d ascii value = %x\n", offset, response[10]);
//            }
        }

        {
            unsigned char response[BUFFER_SIZE];
            const char* instr = "w1f800004 1 40\r";
            writeSerialData(reinterpret_cast<const unsigned char*>(instr), strlen(instr));
            int offset = readFromPort(response);
            printf("read byte = %d ascii value = %x\n", offset, response[0]);
        }
    }

    int getBinSize(ifstream& ifs) {
        if(ifs) {
            ifs.seekg(0, ifs.end);
            return ifs.tellg();
        }
        return 0;
    }

    void readBin(unsigned char* buffer, ifstream& ifs, int bytes) {
        if(ifs) {
            ifs.seekg(0, ifs.beg);
            printf("reading %d characters ...\n", bytes);
            ifs.read((char*)(&buffer[0]), bytes);
            if(ifs) cout << "all characters read successfully\n";
            else cout << "error: only " << ifs.gcount() << " could be read\n";
            ifs.close();
        }
        else throw "unable to open stream to file";
    }
}