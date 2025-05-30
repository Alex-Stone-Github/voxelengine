#include "network.hpp"

// File descriptor for network
static int connection;

bool net::init() {
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

void net::spinup() {
    //
}