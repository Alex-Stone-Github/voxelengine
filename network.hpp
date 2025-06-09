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

// Protocol Names (hence different specific naming case)
// TODO: New packet for bulk requests
void ClientGetChunkUpdate(IndexId);
void ClientGetChunkFull(IndexId);
void ClientSendChunkUpdate(IndexId, IndexId, uint8_t);

}