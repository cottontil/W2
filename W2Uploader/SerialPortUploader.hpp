#pragma once

#include "IOLoop.hpp"
#include "SerialPortClient.hpp"
#include <memory>
#include <string>


class SerialPortUploader {
  public:
    // Struct to represent a buffer for upload, with its size
    struct UploadBuffer {
        char* buffer; // Raw buffer
        size_t size;                    // Number of bytes in the buffer
    };

    // Constructor that initializes the uploader with the I/O loop and serial
    // port parameters
    SerialPortUploader(IOLoop &ioLoop, const std::string &portPath,
                       speed_t baud);

    // Uploads the binary data to the serial port
    virtual void upload(const std::string &binaryPath);

  protected:
    // Reads the binary file into a buffer and returns the buffer along with its
    // size
    UploadBuffer readBinaryFile(const std::string &binaryPath);

    // Member variables
    IOLoop &ioLoop_; // Reference to the IOLoop instance
    SerialPortClient
        client_; // SerialPortClient for managing serial communication
};