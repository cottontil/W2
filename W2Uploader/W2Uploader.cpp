#include "W2Uploader.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <iostream>

W2Uploader::W2Uploader(IOLoop& ioLoop, const std::string& portPath, speed_t baud) :  SerialPortUploader(ioLoop, portPath, baud) 
{ }

void W2Uploader::upload(const std::string& binaryPath) {
    UploadBuffer uploadBuffer = readBinaryFile(binaryPath);
    if (uploadBuffer.size % WORD_SIZE != 0) {
        throw std::runtime_error("binary size must be word-aligned");
    } 

    bool flushErr = client_.flush();
    if (flushErr) {
        throw std::runtime_error(
            "Failed to flush serial port, not uploading. Terminating: " +
            std::string(strerror(errno)));
    }

    // Record the start time
    auto start = std::chrono::high_resolution_clock::now();

    sendStart();
    setUpFlash();

    size_t offset = 0;
    size_t chunkSize = 256 * WORD_SIZE; // write 256 words at a time
    while (offset < uploadBuffer.size) {
        size_t remainingSize = uploadBuffer.size - offset;
        size_t writeSize =
            (remainingSize > chunkSize) ? chunkSize : remainingSize;

        std::string fmtBuffer = bytesToLittleEndianHexString(uploadBuffer.buffer + offset, writeSize);

        // uploaded the formatted buffer
        auto startChunk = std::chrono::high_resolution_clock::now();

        try {
            client_.write(fmtBuffer.c_str(), fmtBuffer.size()); // Pass the raw buffer and its size
        } catch (const std::exception &e) {
            std::cerr << "Error during upload: " << e.what() << std::endl;
        }

        // Record the end time
        auto endChunk = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endChunk - startChunk;

        // Log the total time taken for the upload
        std::cout << "Finished chunk upload! Time taken to upload: " << duration.count() << " seconds." << std::endl;

        offset += writeSize;
    }

    // Record the end time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Log the total time taken for the upload
    std::cout << "Finished upload!" << std::endl;
    std::cout << "Time taken to upload: " << duration.count() << " seconds."
              << std::endl;
}

void W2Uploader::sendStart() {
    client_.write("\n", 1);
}

std::string W2Uploader::charToHexString(char c) {
    std::ostringstream oss;
    oss << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(static_cast<unsigned char>(c)));
    return oss.str();
}

std::string W2Uploader::bytesToLittleEndianHexString(const char* buffer, size_t size) {
    if (size % WORD_SIZE != 0) {
        throw std::runtime_error("Buffer size must be word-aligned");
    }
    size_t numWords = size / 4;

    std::ostringstream oss;
    // format write to address 10000000 <numWords>
    oss << "w10000000 " << std::hex << numWords << " ";

    for (size_t i = 0; i < size; i += WORD_SIZE) {
        for (int j = WORD_SIZE-1; j >= 0; --j) {
            oss << charToHexString(buffer[i + j]);
        }

        // Add separator unless it's the last group
        if (i + WORD_SIZE < size) {
            oss << ' ';
        }
    }

    oss << '\r';  // Final terminator
    return oss.str();
}

void W2Uploader::setUpFlash() {
    const char* disableSepcialSeqInstr = "w1f800004 1 40\r"; // disable special sequence
    const char* unlockFlashInstr = "w1f800b07 1 a5\r"; // unlock flash
    const char* eraseFlashInstr = "w10300000 1 00\r"; // erase whole flash

    client_.write("a", 1); // console mode
    client_.write(disableSepcialSeqInstr, 15);
    client_.write(unlockFlashInstr, 15);
    client_.write(eraseFlashInstr, 15);
}

void W2Uploader::startExecute() {
    const char* jumpExecuteInstr = "j10000200 \r";
    client_.write(jumpExecuteInstr, 11);
}
