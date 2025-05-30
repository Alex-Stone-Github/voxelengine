#include "network.hpp"

#include <array>
#include <vector>
#include <algorithm>
#include <ranges>
#include <print>

// File descriptor for network
static int connection = 1; // stdout lol

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

template<typename Width4Type>
std::array<uint8_t, 4> to_net_bytes(Width4Type mnum) {
    static_assert(sizeof(Width4Type) == 4, "Width4Type needs to be 4 bytes");
    uint8_t* lit_end = reinterpret_cast<uint8_t*>(&mnum);
    return {lit_end[3], lit_end[2], lit_end[1], lit_end[0]};
}

void net::spinup() {
    static char buffer[1024]; // TODO: EXpand
    std::vector<uint8_t> incoming;
Repeat:
    auto length = read(connection, buffer, sizeof(buffer));
    if (length < 0) goto Repeat; // Error Handling
    for (int i = 0; i < length; i ++) {
        incoming.push_back(buffer[i]);
    }
    // Print and clear incoming
    std::ranges::for_each(incoming, [](uint8_t byte) {
        std::print("{}, ", byte);
    });
    std::println("");
    sleep(2);
    incoming.clear();

    goto Repeat;
}


void net::ClientGetChunkUpdate(IndexId chid) { // id 0
    std::vector<uint8_t> sendbuffer;

    // Add a header
    auto size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(IndexId);
    auto header_bytes = to_net_bytes(static_cast<uint32_t>(size));
    std::ranges::copy(header_bytes, std::back_inserter(sendbuffer));

    // Add a section id
    uint32_t secid = 0;
    auto secid_bytes = to_net_bytes(secid);
    std::ranges::copy(secid_bytes, std::back_inserter(sendbuffer));

    // Add Everything
    auto xbytes = to_net_bytes(chid.x);
    auto ybytes = to_net_bytes(chid.y);
    auto zbytes = to_net_bytes(chid.z);

    std::ranges::copy(xbytes, std::back_inserter(sendbuffer));
    std::ranges::copy(ybytes, std::back_inserter(sendbuffer));
    std::ranges::copy(zbytes, std::back_inserter(sendbuffer));

    // Packet Ready
    write(connection, sendbuffer.data(), sendbuffer.size());
}
void net::ClientGetChunkFull(IndexId chid) { // id 1
    std::vector<uint8_t> sendbuffer;

    // Add a header
    auto size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(IndexId);
    auto header_bytes = to_net_bytes(static_cast<uint32_t>(size));
    std::ranges::copy(header_bytes, std::back_inserter(sendbuffer));

    // Add a section id
    uint32_t secid = 1;
    auto secid_bytes = to_net_bytes(secid);
    std::ranges::copy(secid_bytes, std::back_inserter(sendbuffer));

    // Add Everything
    auto xbytes = to_net_bytes(chid.x);
    auto ybytes = to_net_bytes(chid.y);
    auto zbytes = to_net_bytes(chid.z);

    std::ranges::copy(xbytes, std::back_inserter(sendbuffer));
    std::ranges::copy(ybytes, std::back_inserter(sendbuffer));
    std::ranges::copy(zbytes, std::back_inserter(sendbuffer));

    // Packet Ready
    write(connection, sendbuffer.data(), sendbuffer.size());

}
void net::ClientSendChunkUpdate(IndexId chid, IndexId blkid, uint8_t blk) { // id2

}