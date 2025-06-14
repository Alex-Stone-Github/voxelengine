#pragma once

#include <cstdint>

#include "chunk.hpp"
#include "world.hpp"

namespace net {
// Plateform Overrides
bool pl_open();
ssize_t pl_write(char const* buffer, size_t count);
ssize_t pl_read(char* buffer, size_t count);
void pl_close();

// Initialize the network (true on success)
bool init(); 
void spinup(World*);


// Sending ---------------------------
enum ClientSectionKind {
    kClientGetChunkUpdate,
    kClientGetChunkFull,
    kClientSendChunkUpdate,
    kClientSendPlayerPosUpdate,
};
int clientsection_kind_toid(ClientSectionKind kind);
/// Just the size of the payload - not including section id
size_t clientsection_tosize(ClientSectionKind kind);
struct ClientGetChunkUpdate {
    IndexId chunkid;
};
struct ClientGetChunkFull {
    IndexId chunkid;
};
struct ClientSendChunkUpdate {
    IndexId chunkid, blocklocid;
    unsigned int blockid;
};
struct ClientSendPlayerPosUpdate {
    EntityTransform transform;
};
union ClientSectionData {
    ClientGetChunkUpdate      c_g_update;
    ClientGetChunkFull        c_g_full;
    ClientSendChunkUpdate     c_s_update;
    ClientSendPlayerPosUpdate c_sp_update;
};
struct ClientSection {
    ClientSectionKind kind;
    ClientSectionData d;
};
void enqueue_section(ClientSection section);
void sendall();

}