#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <cassert>
#include <cinttypes>

uint32_t const buffer[] = {
    htonl(4+4*4+4*4), // Size

    /// SECTION 1
    htonl(0), // Block updaet request
    // Chunk id
    htonl(1),
    htonl(2),
    htonl(3),
    /// SECTION 2
    htonl(1), // full chunk block request
    // Chunk id
    htonl(4),
    htonl(5),
    htonl(6),
};
uint32_t const buffer2[] = {
    htonl(4+4*4), // Size

    /// SECTION 1
    htonl(0), // Block updaet request
    // Chunk id
    htonl(69),
    htonl(72),
    htonl(73),
};

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    // Localhost
    struct sockaddr_in localhost8000;
    localhost8000.sin_family = AF_INET;
    localhost8000.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    localhost8000.sin_port = htons(8000);

    assert(connect(fd, (sockaddr const*)&localhost8000, sizeof(struct sockaddr_in)) >= 0);
    write(fd, buffer, sizeof(buffer));
    write(fd, buffer2, sizeof(buffer2));
    sleep(3);
}