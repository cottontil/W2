# pragma once

#include <sys/event.h>
#include <unordered_map>
#include <functional>
#include <unistd.h>

class IOLoop {
public:
    IOLoop();
    ~IOLoop();

    // Register a file descriptor with a callback
    void registerHandle(int fd, std::function<void()> callback);

    // Run the event loop
    void run();

private:
    int kq_;  // kqueue descriptor
    std::unordered_map<int, std::function<void()>> handlers_;  // Map of fd to handlers
};
