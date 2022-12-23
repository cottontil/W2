//
// Created by Aaron Ye on 7/31/22.
//

//  main.cpp

// A serial port utility for MacOS X

// For communicating with serial devices (e.g. microcontrollers)
// usually via USB serial.

//     * Read using readSerialData(char* bytes, size_t length)
//
//     * Write using writeSerialData(const char* bytes, size_t length)
//
//     * Remember to flush potentially buffered data when necessary
//
//     * Close serial port when done

#include "serialport.h"
#include "typeabbreviations.h"
#include "readUtils.h"
#include "writeUtils.h"

#define BAUD_RATE 9600
#define BIN_PATH "../Binary/test.bin"

using namespace std;

void printResponseBuffer(unsigned char* buffer, int words) {
    int offset = 0;
    for(int i = 0; i < words; i++) {
        for (int j = 0; j < WORD_BYTES*2; j++) {
            printf("%c", buffer[i*8 + j + offset]);
        }
        offset++;
        printf("\n");
    }
}

void testRegister() {
    int length = 4;
    unsigned char response[length];
    long long address = 0x1f800b07;
    readUtils::readAddress(response, address, length);

    unsigned char response2[length];
    long long address2 = 0x1f800004;
    readUtils::readAddress(response2, address2, length);
}

void testReadWrite() {
    int words = 1;

    long long address = 0x10000000;
    unsigned char buffer[4];
    buffer[0] = 0x31; buffer[1] = 0x32; buffer[2] = 0x33; buffer[3] = 0x34;

    writeUtils::writeBytes(address, buffer, words);
    printf("finished writing buffer1\n");

    long long address2 = 0x10000004;
    unsigned char buffer2[4];
    buffer2[0] = 0x35; buffer2[1] = 0x36; buffer2[2] = 0x37; buffer2[3] = 0x38;

    writeUtils::writeBytes(address2, buffer2, words);
    printf("finished writing buffer2\n");

    unsigned char response[CAPACITY];
    readUtils::readAddress(response, address, 2);

    printResponseBuffer(response, 2);
}

void testReadBin() {
    printf("\nstarting to read from bin\n\n");
    vector<unsigned char> buffer;
    readUtils::readBin(buffer, BIN_PATH);

    printf("finished reading from bin\n");

    long long address = 0x10000000;
    writeUtils::uploadBin(address, buffer);
    printf("finished uploading buffer\n");

    unsigned char response[buffer.size()*4];
    readUtils::readAddress(response, address, buffer.size()/4);
    printf("finished reading buffer\n");
    printResponseBuffer(response, buffer.size()/4);
}

int main(int argc, const char * argv[]) {

//     Open port, and connect to a device
    const char* devicePathStr = "/dev/tty.usbserial-14510";
    const int baudRate = BAUD_RATE;
    int sfd = openAndConfigureSerialPort(devicePathStr, baudRate);
    if (sfd < 0) {
        if (sfd == -1) {
            printf("Unable to connect to serial port.\n");
        }
        else { //sfd == -2
            printf("Error setting serial port attributes.\n");
        }
        return 0;
    }

    printf("serial port is open: %d\n", serialPortIsOpen());
    flushSerialData();

    {
        unsigned char bytes[BUFFER_SIZE];
        int offset = readUtils::readFromPort(bytes);
        printf("read byte = %d\n%s\n\n", offset, bytes);
    }

    readUtils::setupFlash();

//    testRegister();
//    testReadWrite();
//    testReadBin();

    closeSerialPort();
    return 0;
}

