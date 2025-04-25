#include <iostream>
#include <thread>
#include <string>
#include <stdexcept>
#include "IOLoop.hpp"
// #include "SerialPortUploader.hpp"
#include "W2Uploader.hpp"

int main(int argc, char* argv[]) {
    // Check if correct number of arguments are provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <serial_port_path> <binary_file_path>" << std::endl;
        return 1;  // Return error code if arguments are incorrect
    }

    // Get the serial port path and binary file path from command-line arguments
    const std::string portPath = argv[1];
    const std::string binaryPath = argv[2];

    // Create an IOLoop instance
    IOLoop ioLoop;

    // Create a new thread to run the IOLoop (this will handle serial port events)
    std::thread ioLoopThread([&ioLoop]() {
        ioLoop.run();
    });

    // Create the SerialPortUploader instance with IOLoop and serial port parameters
    try {
        // SerialPortUploader uploader(ioLoop, portPath, B9600);  // Adjust baud rate as needed
        W2Uploader uploader(ioLoop, portPath, B9600);  // Adjust baud rate as needed

        // Upload the binary file using the uploader
        uploader.upload(binaryPath);
    } catch (const std::exception& e) {
        std::cerr << "Error during upload: " << e.what() << std::endl;
    }

    // Join the IOLoop thread to wait for it to finish (if necessary)
    ioLoopThread.join();

    return 0;
}
