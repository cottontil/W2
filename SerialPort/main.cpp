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
#define BIN_PATH "../Binary/w2_hello.ino.ld.bin"

using namespace std;

int main(int argc, const char * argv[]) {

//     Open port, and connect to a device
    const char* devicePathStr = "/dev/tty.usbserial-14410";
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
    int length = 4;
    unsigned char response[length];
    long long address = 0x1f800b07;
    readUtils::readConsecutiveBytes(response, address, length);

//    ifstream ifs (BIN_PATH, std::ifstream::binary);
//    int length = readUtils::getBinSize(ifs);
//    unsigned char buffer[length]; memset(buffer,0,sizeof(buffer));
//    readUtils::readBin(buffer, ifs, length);

//    int length = 4;
//    unsigned char buffer[length];
//    buffer[0] = 0x12; buffer[1] = 0x34; buffer[2] = 0x56; buffer[3] = 0x78;
//
//
//    long long address = 0x10000000;
//    writeUtils::writeBytes(address, buffer, length);
//
//    printf("finished writing bytes\n");
//
//    unsigned char response[length];
//    readUtils::readConsecutiveBytes(response, address, length);

//    for(int i = 0; i < length; i++) {
//        printf("%d = %d\n", i, int(response[i]));
//    }
//    printf("finished reading bytes\n");
    closeSerialPort();
    return 0;
}

