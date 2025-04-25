#include "SerialPortClient.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

#define ACK 0x06

SerialPortClient::SerialPortClient(const std::string &portPath, IOLoop &ioLoop,
                                   speed_t baud)
    : portPath_(portPath), baudRate_(baud), ioLoop_(ioLoop) {
    openSerial();
    ioLoop_.registerHandle(serialFd_, [this]() { this->onReadable(); });
}

SerialPortClient::~SerialPortClient() {
    if (serialFd_ >= 0) {
        close(serialFd_);
    }
}

bool SerialPortClient::flush() {
#ifdef _WIN32
    if (!PurgeComm(serialHandle_, PURGE_RXCLEAR | PURGE_TXCLEAR)) {
        std::cerr << "Warning: Failed to flush serial port" << std::endl;
    }
#else
    if (tcflush(serialFd_, TCIOFLUSH) != 0) {
        std::cerr << "Warning: Failed to flush serial port" << std::endl;
        return 1;
    }
    return 0;
#endif
}

void SerialPortClient::write(const char *buffer, size_t bufferSize) {
    // size_t offset = 0;
    // const size_t chunkSize = 10; // Send 10 bytes at a time

    // while (offset < bufferSize) {
    //     size_t remainingSize = bufferSize - offset;
    //     size_t writeSize =
    //         (remainingSize > chunkSize) ? chunkSize : remainingSize;

    //     // Call the private writeChunk function to send the current chunk
    //     writeChunk(buffer + offset, writeSize);
    //     offset += writeSize;

    //     // Wait for ACK after sending each chunk
    //     if (!waitForAck()) {
    //         std::cerr << "Failed to receive ACK after sending chunk"
    //                   << std::endl;
    //         return;
    //     }
    // }

    writeChunk(buffer, bufferSize);
    if (!waitForAck()) {
        std::cerr << "Failed to receive ACK after sending chunk"
                    << std::endl;
    }
}

void SerialPortClient::writeChunk(const char *buffer, size_t bufferSize) {
    ssize_t bytesWritten = ::write(serialFd_, buffer, bufferSize);
    if (bytesWritten < 0) {
        throw std::runtime_error("Write failed: " +
                                 std::string(strerror(errno)));
    }

    std::cout << "Sent: " << bufferSize << " bytes to serial port\n";
    isWaitingForAck_ = true;
}

bool SerialPortClient::waitForAck(int timeoutSec) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (ackReceived_.wait_for(lock, std::chrono::seconds(timeoutSec),
                              [this]() { return !isWaitingForAck_; })) {
        return true; // ACK received
    } else {
        std::cerr << "Timeout waiting for ACK" << std::endl;
        return false; // Timeout, return false
    }
}

void SerialPortClient::openSerial() {
    serialFd_ = open(portPath_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serialFd_ < 0) {
        throw std::runtime_error("Failed to open serial port: " +
                                 std::string(strerror(errno)));
    }

    struct termios tty{};
    if (tcgetattr(serialFd_, &tty) != 0) {
        throw std::runtime_error("tcgetattr failed: " +
                                 std::string(strerror(errno)));
    }

    cfsetispeed(&tty, baudRate_);
    cfsetospeed(&tty, baudRate_);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag = IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 5;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(serialFd_, TCSANOW, &tty) != 0) {
        throw std::runtime_error("tcsetattr failed: " +
                                 std::string(strerror(errno)));
    }
}

void SerialPortClient::onReadable() {
    char buf[256];
    int n = ::read(serialFd_, buf, sizeof(buf));
    if (n <= 0) {
        std::cerr << "Read error or no data\n";
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    for (int i = 0; i < n; ++i) {
        unsigned char byte = buf[i];
        if (byte == ACK) {
            std::cout << "ACK received\n";
            isWaitingForAck_ = false;
            ackReceived_.notify_all();  // Notify the waiting thread
        } else {
            receivedData_.push_back(byte);  // Store non-ACK data
        }
    }
    printReceivedData();
    receivedData_.clear();
}

void SerialPortClient::printReceivedData() const {
    if (receivedData_.empty()) {
        std::cout << "No additional data received from the serial port.\n";
        return;
    }

    std::cout << "Non-ACK data received from serial port:\n";
    for (char c : receivedData_) {
        std::cout << std::hex << std::uppercase << "0x" << static_cast<int>(static_cast<unsigned char>(c)) << " ";
    }
    std::cout << std::dec << "\n";  // Reset to decimal output
}
