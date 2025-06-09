#include "network.hpp"


/// This file contains plateform specific network implementations
#ifdef WIN
// Windows Code
bool net::pl_open() {return false;}
ssize_t net::pl_write(char const* buffer, size_t count) {return 0;}
ssize_t net::pl_read(char* buffer, size_t count) {return 0;}
void net::pl_close() {}
#else
// UNIX Code
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

// File descriptor for network
static int connection = 1; // stdout lol

bool net::pl_open() {
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

ssize_t net::pl_write(char const* buffer, size_t count) {
    return write(connection, buffer, count);
}
ssize_t net::pl_read(char* buffer, size_t count) {
    return read(connection, buffer, count);
}
void net::pl_close() {
    close(connection);
}
#endif