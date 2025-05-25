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
constexpr float blocksize = 1.0;


struct LiveChunk {
    static constexpr size_t sizex = 32;
    static constexpr size_t sizey = 32;
    static constexpr size_t sizez = 32;
    using BlockData = std::array<std::array<std::array<Block, sizez>, sizey>, sizex>;

    unsigned int vao, vbop, vbot;
    int vertex_count;
    Vector3 position;
    BlockData blocks;
    bool dirty;

    LiveChunk(BlockData const&);
    ~LiveChunk();
    LiveChunk(LiveChunk const&) = delete;
    LiveChunk& operator=(LiveChunk const&) = delete;
    LiveChunk(LiveChunk&&);
    LiveChunk& operator=(LiveChunk&&);

    std::optional<Block*> get_block(size_t ix, size_t iy, size_t iz);
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