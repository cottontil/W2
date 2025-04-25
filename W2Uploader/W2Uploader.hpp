#pragma once

#include "SerialPortUploader.hpp"

#define EXECUTION_ADDRESS 0x10000000
#define WORD_SIZE 4

class W2Uploader : public SerialPortUploader {
public:
    // Constructor that initializes the uploader with the I/O loop and serial port parameters
    W2Uploader(IOLoop& ioLoop, const std::string& portPath, speed_t baud);

    // Uploads the binary data to the serial port
    void upload(const std::string& binaryPath) override;

private:
    std::string charToHexString(char c);
    std::string bytesToLittleEndianHexString(const char* buffer, size_t size);

    void sendStart();
    void setUpFlash();
    void startExecute();
};