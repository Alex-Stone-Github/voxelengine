#include "network.hpp"


/// This file contains plateform specific network implementations
#ifdef WIN
#include <winsock2.h>
#include <ws2tcpip.h>
static WSADATA wsa_data;
static SOCKET connection;
bool net::pl_open(char const* address, uint16_t port) {
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) return false;
    connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connection == INVALID_SOCKET) return false;

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, address, &dest.sin_addr);
    dest.sin_port = htons(port);

    auto success = connect(connection,
        (sockaddr const*)&dest, 
        sizeof(struct sockaddr_in));
    if (success == SOCKET_ERROR) return false;
    return true;
}
ssize_t net::pl_write(char const* buffer, size_t count) {
    return send(connection, buffer, count, 0);
}
ssize_t net::pl_read(char* buffer, size_t count) {
    return recv(connection, buffer, count, 0);
}
void net::pl_close() {
    closesocket(connection);
    WSACleanup();
}
#else
// UNIX Code
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

// File descriptor for network
static int connection = 1; // stdout lol

bool net::pl_open(char const* address, uint16_t port) {
    connection = socket(AF_INET, SOCK_STREAM, 0);
    if (connection == -1) return false;

    // Localhost
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, address, &dest.sin_addr);
    dest.sin_port = htons(port);

    auto success = connect(connection,
        (sockaddr const*)&dest, 
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