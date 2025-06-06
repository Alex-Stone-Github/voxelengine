#pragma once

#include <vector>
#include <array>
#include <optional>
#include <ranges>
#include <algorithm>
#include <unordered_map>
//#include <mutex>

#include "chunk.hpp"

ChunkData generate(IndexId id); // TODO: Hopefluly remove


struct World {
    static constexpr size_t render_dist = 2;
    std::vector<LiveChunk> live_chunks;
    //std::mutex lcguard;
    std::unordered_map<IndexId, ChunkData, IndexHash> lcchunk;


    void reload_check(IndexId pos);
    void dirty_check();
    std::optional<LiveChunk*> find_by_id(IndexId id);
    std::array<std::optional<LiveChunk*>, 6> find_neighbors(IndexId id);
};