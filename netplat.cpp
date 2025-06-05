#include "network.hpp"

// This file contains plateform specific networking code

#ifdef WIN
#include <iostream>
// How can I spoof this nonsense
bool net::plateform_init() {
    std::cout << "Inited network for windows" << std::endl;
    return true;
}
ssize_t net::plateform_read(void* buffer, size_t count) {
    return 0;
}
ssize_t net::plateform_write(void const* buffer, size_t count) {
    return 0;
}
void net::plateform_cleanup() {
}
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
static int connection = 1; // stdout lol
bool net::plateform_init() {
    connection = socket(AF_INET, SOCK_STREAM, 0);
    if (connection == -1) return false;

    // Localhost
    struct sockaddr_in localhost8000;
    localhost8000.sin_family = AF_INET;
    localhost8000.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    localhost8000.sin_port = htons(8000);

    auto success = connect(connection,
        (sockaddr const*)&localhost8000, 
        sizeof(struct sockaddr_in)) >= 0;
    if (!success) return false;
    return true;
}
ssize_t net::plateform_read(void* buffer, size_t count) {
    return read(connection, buffer, count);
}
ssize_t net::plateform_write(void const* buffer, size_t count) {
    return write(connection, buffer, count);
}
void net::plateform_cleanup() {close(connection);}
#endif