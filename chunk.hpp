#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <optional>
#include "vector3.hpp"
#include "camera.hpp"

enum Cardinal {
    North,
    West,
    South,
    East,
    Up,
    Down,
};

enum Block: uint8_t {
    Air = 0,
    Stone = 1,
    Grass = 2,
};
struct BlockInfo {
    static constexpr float ts = 1.0f / 4.0f; // texture size
    Vector2 side_bl; // texture cordinate bottom left
    Vector2 up_bl; // texture cordinate bottom left
};
constexpr BlockInfo block_lookup[] = {
    BlockInfo(Vector2(0, 0), Vector2(0, 0)), // Air: Null 
    BlockInfo(Vector2(BlockInfo::ts*3, BlockInfo::ts*3), Vector2(0, 0)), // Stone
    BlockInfo(Vector2(BlockInfo::ts, BlockInfo::ts*3), Vector2(0, BlockInfo::ts*3)), // Grass
};

static constexpr size_t sizex = 32;
static constexpr size_t sizey = 32;
static constexpr size_t sizez = 32;
constexpr float blocksize = 1.0;

struct IndexId {
    int x, y, z;
    bool operator==(IndexId const&) const;
};
struct ChunkData {
    IndexId id;
    std::array<std::array<std::array<Block, sizez>, sizey>, sizex> blocks;

    std::optional<Block> get_block(ssize_t ix, ssize_t iy, ssize_t iz) const;
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
struct IndexHash{size_t operator()(IndexId const&) const;};