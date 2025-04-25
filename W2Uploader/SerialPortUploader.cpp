#include "SerialPortUploader.hpp"
#include "SerialPortClient.hpp"
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

SerialPortUploader::SerialPortUploader(IOLoop &ioLoop,
                                       const std::string &portPath,
                                       speed_t baud)
    : ioLoop_(ioLoop), client_(portPath, ioLoop, baud) {}

SerialPortUploader::UploadBuffer
SerialPortUploader::readBinaryFile(const std::string &binaryPath) {
    // Open the binary file for reading
    std::ifstream file(binaryPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open binary file.");
    }

    // Get the size of the file
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate memory for the file content using a unique_ptr
    char* buffer = new char(fileSize);
    file.read(buffer, fileSize);
    file.close();

    if (!file) {
        throw std::runtime_error("Error reading binary file.");
    }

    // Return the UploadBuffer struct containing the buffer and its size
    return {buffer, fileSize};
}

void SerialPortUploader::upload(const std::string &binaryPath) {
    // Read the binary file into a buffer
    UploadBuffer uploadBuffer = readBinaryFile(binaryPath);
    
    // Flush existing input and output buffers
    bool flushErr = client_.flush();
    if (flushErr) {
        throw std::runtime_error(
            "Failed to flush serial port, not uploading. Terminating: " +
            std::string(strerror(errno)));
    }

    // Record the start time
    auto start = std::chrono::high_resolution_clock::now();

    // Upload the buffer data to the serial port
    try {
        client_.write(uploadBuffer.buffer, uploadBuffer.size); // Pass the raw buffer and its size
    } catch (const std::exception &e) {
        std::cerr << "Error during upload: " << e.what() << std::endl;
    }

    // Record the end time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Log the total time taken for the upload
    std::cout << "Finished upload!" << std::endl;
    std::cout << "Time taken to upload: " << duration.count() << " seconds."
              << std::endl;
}
