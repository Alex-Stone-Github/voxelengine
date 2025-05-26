#pragma once

#include <vector>
#include <array>
#include <optional>
#include <ranges>
#include <algorithm>

#include "chunk.hpp"

struct World {
    static constexpr size_t render_dist = 2;
    std::vector<LiveChunk> live_chunks;

    void reload_check(ChunkId pos);
    void dirty_check();
    std::optional<LiveChunk*> find_by_id(ChunkId id);
    std::array<std::optional<LiveChunk*>, 6> find_neighbors(ChunkId id);
};