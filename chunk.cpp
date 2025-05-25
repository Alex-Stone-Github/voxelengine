#include <vector>
#include <ranges>
#include <algorithm>
#include <print>
#include "chunk.hpp"
#include "display.hpp"

#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

LiveChunk::LiveChunk(BlockData const& block_data):
    vertex_count(0), dirty(true) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbop);
    glBindBuffer(GL_ARRAY_BUFFER, vbop);
    glBufferData(vbop, 0, 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbot);
    glBindBuffer(GL_ARRAY_BUFFER, vbot);
    glBufferData(vbot, 0, 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);
    glEnableVertexAttribArray(1);

    blocks = block_data; // copy
}
LiveChunk::~LiveChunk() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbop);
    glDeleteBuffers(1, &vbot);
}
LiveChunk::LiveChunk(LiveChunk&& other): vao(other.vao), vbop(other.vbop),
    vbot(other.vbot), vertex_count(other.vertex_count),
    position(other.position), blocks(other.blocks), dirty(other.dirty) {
    other.vao = 0;
    other.vbop = 0;
    other.vbot = 0;
}
LiveChunk& LiveChunk::operator=(LiveChunk&& other) {
    position = other.position;
    vertex_count = other.vertex_count;
    vao = other.vao;
    vbop = other.vbop;
    vbot = other.vbot;
    blocks = other.blocks;
    dirty = other.dirty;
    other.vao = 0;
    other.vbop = 0;
    other.vbot = 0;
    return *this;
}
void draw_chunk(
    LiveChunk const& chunk,
    Camera const& camera,
    unsigned int program
) {
    glUseProgram(program);

    // TODO: Transform matrix
    auto location = glGetUniformLocation(program, "transform");
    auto transform = glm::translate(glm::vec3(chunk.position.x, chunk.position.y, chunk.position.z));
    glUniformMatrix4fv(location, 1, GL_FALSE, &transform[0][0]);
    set_view_matrices(camera, program);

    // Draw Calls
    glUseProgram(program);
    glBindVertexArray(chunk.vao);
    glDrawArrays(GL_QUADS, 0, chunk.vertex_count);
}
std::optional<Block*> LiveChunk::get_block(size_t ix, size_t iy, size_t iz) {
    if (!(ix < sizex && ix >= 0)) return std::nullopt;
    if (!(iy < sizey && iy >= 0)) return std::nullopt;
    if (!(iz < sizez && iz >= 0)) return std::nullopt;
    return &blocks[ix][iy][iz];
}
void recompute_mesh(
    LiveChunk& c,
    std::optional<LiveChunk const*> north,
    std::optional<LiveChunk const*> west,
    std::optional<LiveChunk const*> south,
    std::optional<LiveChunk const*> east,
    std::optional<LiveChunk const*> up,
    std::optional<LiveChunk const*> down
) {
    std::vector<Vector3> vertices;
    std::vector<Vector2> vertices_uv;
    // Generation
    for (size_t ix = 0; ix < LiveChunk::sizex; ix ++) {
        for (size_t iy = 0; iy < LiveChunk::sizey; iy ++) {
            for (size_t iz = 0; iz < LiveChunk::sizez; iz ++) {
                auto block = c.get_block(ix, iy, iz).value(); // know it has a block
                if (*block != Air) continue;

                // Calculate Corners
                auto ox = static_cast<float>(ix);
                auto oy = static_cast<float>(iy);
                auto oz = static_cast<float>(iz);
                Vector3 lsw(ox*blocksize, oy*blocksize, -oz*blocksize);
                Vector3 usw(lsw.x, lsw.y+blocksize, lsw.z);
                Vector3 lse(lsw.x+blocksize, lsw.y, lsw.z);
                Vector3 use(lsw.x+blocksize, lsw.y+blocksize, lsw.z);
                Vector3 lnw(ox*blocksize, oy*blocksize, -oz*blocksize-blocksize);
                Vector3 unw(lsw.x, lsw.y+blocksize, lsw.z-blocksize);
                Vector3 lne(lsw.x+blocksize, lsw.y, lsw.z-blocksize);
                Vector3 une(lsw.x+blocksize, lsw.y+blocksize, lsw.z-blocksize);

                auto ptexcords = [&vertices_uv](){
                    vertices_uv.emplace_back(Vector2(1.0, 1.0));
                    vertices_uv.emplace_back(Vector2(0.0, 1.0));
                    vertices_uv.emplace_back(Vector2(0.0, 0.0));
                    vertices_uv.emplace_back(Vector2(1.0, 0.0));

                };
                auto check_spot = [&c](size_t x, size_t y, size_t z) {
                    std::optional<Block*> block = c.get_block(x, y, z);
                    return block.has_value() && *block.value() != Air;
                };
                // North
                if (check_spot(ix, iy, iz+1)) {
                    vertices.emplace_back(une);
                    vertices.emplace_back(unw);
                    vertices.emplace_back(lnw);
                    vertices.emplace_back(lne);
                    ptexcords();
                }
                // West
                if (check_spot(ix-1, iy, iz)) {
                    vertices.emplace_back(usw);
                    vertices.emplace_back(unw);
                    vertices.emplace_back(lnw);
                    vertices.emplace_back(lsw);
                    ptexcords();
                }
                // South
                if (check_spot(ix, iy, iz-1)) {
                    vertices.emplace_back(use);
                    vertices.emplace_back(usw);
                    vertices.emplace_back(lsw);
                    vertices.emplace_back(lse);
                    ptexcords();
                }
                // East
                if (check_spot(ix+1, iy, iz)) {
                    vertices.emplace_back(une);
                    vertices.emplace_back(use);
                    vertices.emplace_back(lse);
                    vertices.emplace_back(lne);
                    ptexcords();
                }
                // Up
                if (check_spot(ix, iy+1, iz)) {
                    vertices.emplace_back(une);
                    vertices.emplace_back(unw);
                    vertices.emplace_back(usw);
                    vertices.emplace_back(use);
                    ptexcords();
                }
                // Down
                if (check_spot(ix, iy-1, iz)) {
                    vertices.emplace_back(lne);
                    vertices.emplace_back(lse);
                    vertices.emplace_back(lsw);
                    vertices.emplace_back(lnw);
                    ptexcords();
                }
            }
        }
    }
    // Upload
    // vbop
    c.vertex_count = vertices.size();
    glBindVertexArray(c.vao);
    glBindBuffer(GL_ARRAY_BUFFER, c.vbop);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vector3), vertices.data(),
        GL_DYNAMIC_DRAW);
    // vbot
    glBindBuffer(GL_ARRAY_BUFFER, c.vbot);
    glBufferData(GL_ARRAY_BUFFER, vertices_uv.size()*sizeof(Vector2), vertices_uv.data(),
        GL_DYNAMIC_DRAW);

}