#include "IOLoop.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/time.h>

IOLoop::IOLoop() {
    // Create a kqueue
    kq_ = kqueue();
    if (kq_ == -1) {
        throw std::runtime_error("Failed to create kqueue: " + std::string(strerror(errno)));
    }
}

IOLoop::~IOLoop() {
    if (kq_ >= 0) {
        close(kq_);
    }
}

void IOLoop::registerHandle(int fd, std::function<void()> callback) {
    // Register a file descriptor with a callback in the handler map
    handlers_[fd] = callback;

    // Prepare an event structure to monitor the file descriptor for readability (EVFILT_READ)
    struct kevent evSet;
    EV_SET(&evSet, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);

    if (kevent(kq_, &evSet, 1, nullptr, 0, nullptr) == -1) {
        throw std::runtime_error("Failed to register file descriptor with kqueue: " + std::string(strerror(errno)));
    }
}

void IOLoop::run() {
    // Event structure for handling ready events
    struct kevent evList[10];  // Array to hold events (up to 10 at a time)

    while (true) {
        // Block and wait for events
        int numEvents = kevent(kq_, nullptr, 0, evList, 10, nullptr);
        if (numEvents < 0) {
            std::cerr << "kevent failed: " << strerror(errno) << std::endl;
            break;  // Exit the loop if kevent fails
        }

        for (int i = 0; i < numEvents; ++i) {
            int fd = evList[i].ident;
            auto handler_itr = handlers_.find(fd);
            if (handler_itr != handlers_.end()) {
                // Call the callback for the corresponding fd
                handler_itr->second();
            }
        }
    }
}
