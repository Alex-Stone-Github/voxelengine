#pragma once

#include <vector>
#include <set>

#include "chunk.hpp"


struct World {
    std::vector<LiveChunk> chunks;
};
