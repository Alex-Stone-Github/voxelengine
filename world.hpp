#pragma once

#include <vector>
#include <array>
#include <optional>
#include <ranges>
#include <algorithm>
#include <unordered_map>
#include <mutex>

#include "chunk.hpp"
#include "gizmo.hpp"

ChunkData generate(IndexId id); // TODO: Hopefluly remove


struct World {
    static constexpr size_t render_dist = 2;
    std::vector<LiveChunk> live_chunks;

    // Chunks
    std::mutex localchunk_guard;
    std::unordered_map<IndexId, ChunkData, IndexHash> lcchunk;

    // Entities
    std::mutex entity_guard;
    std::vector<EntityTransform> tocreate_entities;
    std::vector<Gizmo> entities;

    void reload_check(IndexId pos);
    void dirty_check();
    std::optional<LiveChunk*> find_by_id(IndexId id);
    std::array<std::optional<LiveChunk*>, 6> find_neighbors(IndexId id);
};