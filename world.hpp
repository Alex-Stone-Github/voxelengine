#pragma once

#include <vector>
#include <array>
#include <optional>
#include <ranges>
#include <algorithm>
#include <unordered_map>
#include <map>

#include "chunk.hpp"

struct World {
    static constexpr size_t render_dist = 2;
    std::vector<LiveChunk> live_chunks;
    std::map<IndexId, ChunkData> lcchunks; // local chunks


    void reload_check(IndexId pos);
    void dirty_check();
    std::optional<LiveChunk*> find_by_id(IndexId id);
    std::array<std::optional<LiveChunk*>, 6> find_neighbors(IndexId id);
};