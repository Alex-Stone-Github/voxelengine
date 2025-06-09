#include "network.hpp"

#include <array>
#include <vector>
#include <algorithm>
#include <ranges>
#include <print>
#include <cstring>

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
std::array<uint8_t, 4> reverse_endian(Width4Type mnum) {
    static_assert(sizeof(Width4Type) == 4, "Width4Type needs to be 4 bytes");
    uint8_t* lit_end = reinterpret_cast<uint8_t*>(&mnum);
    return {lit_end[3], lit_end[2], lit_end[1], lit_end[0]};
}


void net::spinup(World* world) {
    static char buffer[1024]; // TODO: EXpand
    std::vector<uint8_t> incoming;
    auto len_hint = [&incoming]() -> std::optional<size_t> {
        if (incoming.size() < 4) return std::nullopt;
        int tmp = *reinterpret_cast<int*>(incoming.data());
        return ntohl(tmp);
    };
    while (true) {
        // Read More Bytes
        auto length = read(connection, buffer, sizeof(buffer));
        if (length < 0) continue; // Error Handling
        for (int i = 0; i < length; i ++) {
            incoming.push_back(buffer[i]);
        }

        // Interprete packet
        auto plen = len_hint();
        auto condition = plen.has_value() && plen.value() <= incoming.size();
        if (condition) {
            // Start Work with packet
            //std::println("Length of data: {}: PLEN: {}", incoming.size(), plen.value());
            auto i = sizeof(uint32_t); // Packet Length
            while (i < plen.value()) {
                // What section
                auto section_id = ntohl(*reinterpret_cast<uint32_t*>(&incoming.data()[i])); i += 4;
                //std::println("Uncovered section {}", section_id);
                if (section_id == 0) {
                    auto cidx = ntohl(*reinterpret_cast<int32_t*>(&incoming.data()[i])); i += sizeof(int32_t);
                    auto cidy = ntohl(*reinterpret_cast<int32_t*>(&incoming.data()[i])); i += sizeof(int32_t);
                    auto cidz = ntohl(*reinterpret_cast<int32_t*>(&incoming.data()[i])); i += sizeof(int32_t);

                    // Skip Chunk Data
                    auto skip = sizex * sizey * sizez * sizeof(uint8_t);
                    //std::println("ID{},{},{}   Blocks:{}", cidx, cidy, cidz, skip);

                    ChunkData chunk_data;
                    memcpy(&chunk_data.blocks, &incoming.data()[i], skip);
                    auto id = IndexId(cidx, cidy, cidz);
                    chunk_data.id = id;
                    std::lock_guard<std::mutex> lock(world->lcguard);
                    world->lcchunk.insert(std::pair(
                        id, chunk_data
                    ));
                    lock.~lock_guard();

                    i+=skip;
                }
            }
            incoming.erase(incoming.begin(), incoming.begin()+plen.value());
        }

    }
}


void net::ClientGetChunkUpdate(IndexId chid) { // id 0
    std::vector<uint8_t> sendbuffer;

    // Add a header
    auto size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(IndexId);
    auto header_bytes = reverse_endian(static_cast<uint32_t>(size));
    std::ranges::copy(header_bytes, std::back_inserter(sendbuffer));

    // Add a section id
    uint32_t secid = 0;
    auto secid_bytes = reverse_endian(secid);
    std::ranges::copy(secid_bytes, std::back_inserter(sendbuffer));

    // Add Everything
    auto xbytes = reverse_endian(chid.x);
    auto ybytes = reverse_endian(chid.y);
    auto zbytes = reverse_endian(chid.z);

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
    auto header_bytes = reverse_endian(static_cast<uint32_t>(size));
    std::ranges::copy(header_bytes, std::back_inserter(sendbuffer));

    // Add a section id
    uint32_t secid = 1;
    auto secid_bytes = reverse_endian(secid);
    std::ranges::copy(secid_bytes, std::back_inserter(sendbuffer));

    // Add Everything
    auto xbytes = reverse_endian(chid.x);
    auto ybytes = reverse_endian(chid.y);
    auto zbytes = reverse_endian(chid.z);

    std::ranges::copy(xbytes, std::back_inserter(sendbuffer));
    std::ranges::copy(ybytes, std::back_inserter(sendbuffer));
    std::ranges::copy(zbytes, std::back_inserter(sendbuffer));

    // Packet Ready
    write(connection, sendbuffer.data(), sendbuffer.size());

}
void net::ClientSendChunkUpdate(IndexId chid, IndexId blkid, uint8_t blk) { // id2

}