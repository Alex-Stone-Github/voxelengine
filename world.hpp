#pragma once

#include <vector>
#include <set>
#include <ranges>
#include <algorithm>

#include "chunk.hpp"

struct World {
    static constexpr size_t render_dist = 2;
    std::vector<LiveChunk> live_chunks;

    void reload_check(ChunkId pos);
    void dirty_check();
};