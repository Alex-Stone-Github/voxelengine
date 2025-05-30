#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdint>

#include "chunk.hpp"

namespace net {
// Initialize the network (true on success)
bool init(); 
void spinup();

// Protocol Names (hence different specific naming case)
// TODO: New packet for bulk requests
void ClientGetChunkUpdate(IndexId);
void ClientGetChunkFull(IndexId);
void ClientSendChunkUpdate(IndexId, IndexId, uint8_t);

}