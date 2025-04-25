#pragma once

#include "IOLoop.hpp"
#include <condition_variable>
#include <mutex>
#include <string>
#include <termios.h>

#define ACK 0x06

class SerialPortClient {
  public:
    SerialPortClient(const std::string &portPath, IOLoop &ioLoop,
                     speed_t baud = B115200);
    ~SerialPortClient();

    // First write: Takes a char buffer and its size, then calls the second
    // write in chunks
    void write(const char *buffer, size_t bufferSize);
    bool flush();

  private:
    std::string portPath_;
    speed_t baudRate_;
    int serialFd_ = -1;
    IOLoop &ioLoop_;

    std::vector<char> receivedData_;  // Stores non-ACK data

    bool isWaitingForAck_ = false;
    std::mutex mutex_;
    std::condition_variable ackReceived_;

    void openSerial();
    void onReadable();

    // Private functions for handling chunks and waiting for ACKs
    void writeChunk(const char *buffer, size_t bufferSize);
    bool waitForAck(int timeoutSec = 1);

    void printReceivedData() const;
};
