#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>


namespace net {
// int socket;

// Initialize the network (true on success)
bool init(); 

void spinup();
}