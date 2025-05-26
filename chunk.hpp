#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <optional>
#include "vector3.hpp"
#include "camera.hpp"

enum Block: uint8_t {
    Air,
    Stone,
};

static constexpr size_t sizex = 32;
static constexpr size_t sizey = 32;
static constexpr size_t sizez = 32;
constexpr float blocksize = 1.0;

struct ChunkId {
    int x, y, z;
    bool operator==(ChunkId const&) const;
};
struct ChunkData {
    ChunkId id;
    std::array<std::array<std::array<Block, sizez>, sizey>, sizex> blocks;

    std::optional<Block*> get_block(size_t ix, size_t iy, size_t iz);
};

struct LiveChunk {
    unsigned int vao, vbop, vbot;
    int vertex_count;
    Vector3 position;
    bool dirty;

    ChunkData block_data;

    LiveChunk(ChunkData const&);
    ~LiveChunk();
    LiveChunk(LiveChunk const&) = delete;
    LiveChunk& operator=(LiveChunk const&) = delete;
    LiveChunk(LiveChunk&&);
    LiveChunk& operator=(LiveChunk&&);
};

void draw_chunk(
    LiveChunk const& chunk,
    Camera const& camera,
    unsigned int program
);
void recompute_mesh(
    LiveChunk& c,
    std::optional<LiveChunk const*> north,
    std::optional<LiveChunk const*> west,
    std::optional<LiveChunk const*> south,
    std::optional<LiveChunk const*> east,
    std::optional<LiveChunk const*> up,
    std::optional<LiveChunk const*> down
);