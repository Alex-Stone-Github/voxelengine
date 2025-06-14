#include "network.hpp"

#include <array>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cstring>
#include <iostream>

template<typename Width4Type>
std::array<uint8_t, 4> reverse_endian(Width4Type mnum) {
    static_assert(sizeof(Width4Type) == 4, "Width4Type needs to be 4 bytes");
    uint8_t* lit_end = reinterpret_cast<uint8_t*>(&mnum);
    return {lit_end[3], lit_end[2], lit_end[1], lit_end[0]};
}

// Tmp
int ntohl(int src) {
    auto bytes = reinterpret_cast<uint8_t*>(&src);
    uint8_t tmp[] = {bytes[3], bytes[2], bytes[1], bytes[0]};
    return *reinterpret_cast<int*>(tmp);
}
float ntohf(float src) {
    auto bytes = reinterpret_cast<uint8_t*>(&src);
    uint8_t tmp[] = {bytes[3], bytes[2], bytes[1], bytes[0]};
    return *reinterpret_cast<float*>(tmp);
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
        auto length = net::pl_read(buffer, sizeof(buffer));
        if (length < 0) continue; // Error Handling
        for (int i = 0; i < length; i ++) {
            incoming.push_back(buffer[i]);
        }

        // Interprete packet
        auto plen = len_hint();
        auto condition = plen.has_value() && plen.value() <= incoming.size();
        if (condition) {
            // Start Work with packet
            auto i = sizeof(uint32_t); // Packet Length

            // Remove Entities! - Every server update
            std::lock_guard<std::mutex> ent_guard(world->entity_guard);
            world->tocreate_entities.clear();
            ent_guard.~lock_guard();

            while (i < plen.value()) { // Iterate through packets of dynamic length
                // What section
                auto section_id = ntohl(*reinterpret_cast<uint32_t*>(&incoming.data()[i])); i += 4;
                if (section_id == 2) { // Entity Info
                    auto x =   ntohf(*reinterpret_cast<float*>(&incoming.data()[i])); i += sizeof(float);
                    auto y =   ntohf(*reinterpret_cast<float*>(&incoming.data()[i])); i += sizeof(float);
                    auto z =   ntohf(*reinterpret_cast<float*>(&incoming.data()[i])); i += sizeof(float);
                    auto yaw = ntohf(*reinterpret_cast<float*>(&incoming.data()[i])); i += sizeof(float);

                    std::lock_guard<std::mutex> ent_guard(world->entity_guard);
                    world->tocreate_entities.emplace_back(EntityTransform(Vector3(x, y, z), yaw));
                    ent_guard.~lock_guard();
                }
                if (section_id == 0) { // Full chunk
                    auto cidx = ntohl(*reinterpret_cast<int32_t*>(&incoming.data()[i])); i += sizeof(int32_t);
                    auto cidy = ntohl(*reinterpret_cast<int32_t*>(&incoming.data()[i])); i += sizeof(int32_t);
                    auto cidz = ntohl(*reinterpret_cast<int32_t*>(&incoming.data()[i])); i += sizeof(int32_t);

                    // Skip Chunk Data
                    auto skip = sizex * sizey * sizez * sizeof(uint8_t);

                    ChunkData chunk_data;
                    memcpy(&chunk_data.blocks, &incoming.data()[i], skip);
                    auto id = IndexId(cidx, cidy, cidz);
                    chunk_data.id = id;
                    std::lock_guard<std::mutex> lock(world->localchunk_guard);
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


/*
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
    net::pl_write(reinterpret_cast<const char*>(sendbuffer.data()), sendbuffer.size());
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
    net::pl_write(reinterpret_cast<const char*>(sendbuffer.data()), sendbuffer.size());

}
*/

int net::clientsection_kind_toid(net::ClientSectionKind kind) {
    if (kind == net::ClientSectionKind::kClientGetChunkUpdate) return 0;
    if (kind == net::ClientSectionKind::kClientGetChunkFull) return 1;
    if (kind == net::ClientSectionKind::kClientSendChunkUpdate) return 2;
    if (kind == net::ClientSectionKind::kClientSendPlayerPosUpdate) return 3;
    std::unreachable();
}
size_t net::clientsection_tosize(net::ClientSectionKind kind) {
    if (kind == net::ClientSectionKind::kClientGetChunkUpdate) return 4*3;
    if (kind == net::ClientSectionKind::kClientGetChunkFull) return  4*3;
    if (kind == net::ClientSectionKind::kClientSendChunkUpdate) return 4*3*2+4;
    if (kind == net::ClientSectionKind::kClientSendPlayerPosUpdate) return 4*4;
    std::unreachable();
}

static std::vector<net::ClientSection> outgoing_sections;
static std::mutex outgoing_section_lck;

void net::enqueue_section(net::ClientSection section) {
    std::lock_guard<std::mutex> _grd(outgoing_section_lck);
    outgoing_sections.push_back(section);
}
void net::sendall() {
    std::lock_guard<std::mutex> _grd(outgoing_section_lck);
    std::vector<uint8_t> sendbuffer;

    // Calculate size
    size_t sendsize = 4; // Packet Header
    for (auto const& section : outgoing_sections) {
        sendsize += 4; // Section Id
        sendsize += clientsection_tosize(section.kind);
    }
    std::ranges::copy(reverse_endian(static_cast<unsigned int>(sendsize)), std::back_inserter(sendbuffer));

    // Iterate through the sections and add them
    for (auto const& section : outgoing_sections) {
        // Encode SectionId
        unsigned int sectionid = net::clientsection_kind_toid(section.kind);
        std::ranges::copy(reverse_endian(sectionid), std::back_inserter(sendbuffer));
        // Encode specifics
        if (section.kind == net::ClientSectionKind::kClientGetChunkUpdate) {
            std::ranges::copy(reverse_endian(section.d.c_g_update.chunkid.x),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_g_update.chunkid.y),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_g_update.chunkid.z),
                std::back_inserter(sendbuffer));
        }
        if (section.kind == net::ClientSectionKind::kClientGetChunkFull) {
            std::ranges::copy(reverse_endian(section.d.c_g_full.chunkid.x),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_g_full.chunkid.y),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_g_full.chunkid.z),
                std::back_inserter(sendbuffer));
        }
        if (section.kind == net::ClientSectionKind::kClientSendChunkUpdate) {
            std::ranges::copy(reverse_endian(section.d.c_s_update.chunkid.x),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_s_update.chunkid.y),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_s_update.chunkid.z),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_s_update.blocklocid.x),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_s_update.blocklocid.y),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_s_update.blocklocid.z),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_s_update.blockid),
                std::back_inserter(sendbuffer));
        }
        if (section.kind == net::ClientSectionKind::kClientSendPlayerPosUpdate) {
            std::ranges::copy(reverse_endian(section.d.c_sp_update.transform.position.x),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_sp_update.transform.position.y),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_sp_update.transform.position.z),
                std::back_inserter(sendbuffer));
            std::ranges::copy(reverse_endian(section.d.c_sp_update.transform.yaw),
                std::back_inserter(sendbuffer));
        }
    }

    // Cleanup
    assert(sendsize == sendbuffer.size());
    net::pl_write(reinterpret_cast<const char*>(sendbuffer.data()), sendbuffer.size());
    outgoing_sections.clear();
}