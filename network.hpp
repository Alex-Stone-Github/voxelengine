#pragma once

#include <cstdint>

#include "chunk.hpp"
#include "world.hpp"

namespace net {

// Plateform Specific Overides
bool plateform_init();
ssize_t plateform_read(void* buffer, size_t count);
ssize_t plateform_write(void const* buffer, size_t count);
void plateform_cleanup();

///////////////////////
///Generic Functions///
///////////////////////
// Initialize the network (true on success)
void spinup(World*);
// Protocol Names (hence different specific naming case)
void ClientGetChunkUpdate(IndexId);
void ClientGetChunkFull(IndexId);
void ClientSendChunkUpdate(IndexId, IndexId, uint8_t);

}