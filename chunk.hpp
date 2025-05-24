#pragma once

#include <cstdint>
#include <array>
#include <optional>
#include "vector3.hpp"
#include "camera.hpp"

struct BlockProperties {
    bool is_air;
};
enum Block: uint8_t {
    Air,
    Stone,
};
constexpr BlockProperties blockinfo[] = {
    BlockProperties(true),
    BlockProperties(false),
};
constexpr float blocksize = 1.0;


struct Chunk {
    static constexpr size_t sizex = 32;
    static constexpr size_t sizey = 32;
    static constexpr size_t sizez = 32;
    unsigned int vao, vbop, vbot;
    Vector3 position;
    int vertex_count;
    std::array<std::array<std::array<Block, sizez>, sizey>, sizex> blocks;
    Chunk();
    ~Chunk();

    std::optional<Block*> get_block(size_t ix, size_t iy, size_t iz);
};

void draw_chunk(
    Chunk const& chunk,
    Camera const& camera,
    unsigned int program
);
void recompute_mesh(
    Chunk& c,
    Chunk const& north,
    Chunk const& west,
    Chunk const& south,
    Chunk const& east,
    Chunk const& up,
    Chunk const& down
);