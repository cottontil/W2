//
// Created by Aaron Ye on 8/12/22.
//

#include "readUtils.h"
#include "writeUtils.h"
#include <thread>

#define CAPACITY 100
#define STX 0x2
#define ETX 0x3

using namespace std;

namespace readUtils {

    void readAddress(unsigned char* bytes, long long address, int words) {
        const char* instr = writeUtils::formattedReadInstruction(address, words);
        writeSerialData(reinterpret_cast<const unsigned char*>(instr), strlen(instr));
        std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
        readResponseFromPort(bytes);
    }

    int readFromPort(unsigned char* bytes) {
        int offset = 0; int started = 0; int read = 0;
        while(!started || read) {
            read = readSerialData(bytes + offset, CAPACITY);
            if(read > 0) started = 1;
            offset += read;
            std::this_thread::sleep_for (std::chrono::milliseconds (WAIT));
        }
        return offset;
    }

    unsigned char readByteFromPort() {
        int read = 0;
        unsigned char buffer;
        while(!read) {
            read = readSerialData(&buffer, 1);
        }
        return buffer;
    }

    int readResponseFromPort(unsigned char* bytes) {
        unsigned char data_start = 0x0;
        while(data_start != STX) {
            readSerialData(&data_start, 1);
        }
        unsigned char read = 0x0;
        int offset = 0;
        while(read != ETX) {
            read = readByteFromPort();
            bytes[offset] = read;
            offset++;
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
            unsigned char response;
            const char* instr = "w1f800b07 1 a5\r"; // unlock flash
            writeSerialData(reinterpret_cast<const unsigned char*>(instr), strlen(instr));
            int offset = readFromPort(&response);
            printf("read byte = %d ascii value = %x\n", offset, response);
        }

        {
            unsigned char response;
            const char* instr = "w10300000 1 00\r"; // erase flash
            writeSerialData(reinterpret_cast<const unsigned char*>(instr), strlen(instr));
            int offset = readFromPort(&response);
            printf("read byte = %d ascii value = %x\n", offset, response);
        }

        {
            unsigned char response;
            const char* instr = "w1f800004 1 40\r";
            writeSerialData(reinterpret_cast<const unsigned char*>(instr), strlen(instr));
            int offset = readFromPort(&response);
            printf("read byte = %d ascii value = %x\n", offset, response);
        }
    }

    void readBin(std::vector<unsigned char>& buffer, const std::string& path) {
        std::ifstream bin_file(path, std::ios::binary);
        if (bin_file.good()) {
            std::vector<uint8_t> v_buf((std::istreambuf_iterator<char>(bin_file)), (std::istreambuf_iterator<char>()));
            buffer = v_buf;
            bin_file.close();
        }
        else throw std::exception();
    }
}