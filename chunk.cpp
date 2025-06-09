#include <vector>
#include <ranges>
#include <algorithm>
#include <print>
#include "chunk.hpp"
#include "display.hpp"

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

LiveChunk::LiveChunk(ChunkData const& block_data):
    vertex_count(0), dirty(true) {
    this->block_data = block_data;

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
}
LiveChunk::~LiveChunk() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbop);
    glDeleteBuffers(1, &vbot);
}
LiveChunk::LiveChunk(LiveChunk&& other): vao(other.vao), vbop(other.vbop),
    vbot(other.vbot), vertex_count(other.vertex_count),
    position(other.position), dirty(other.dirty), block_data(other.block_data) {
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
    block_data = other.block_data;
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
std::optional<Block> ChunkData::get_block(size_t ix, size_t iy, size_t iz) const {
    if (!(ix < sizex && ix >= 0)) return std::nullopt;
    if (!(iy < sizey && iy >= 0)) return std::nullopt;
    if (!(iz < sizez && iz >= 0)) return std::nullopt;
    return blocks[ix][iy][iz];
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
    for (int ix = 0; ix < static_cast<int>(sizex); ix ++) {
        for (int iy = 0; iy < static_cast<int>(sizey); iy ++) {
            for (int iz = 0; iz < static_cast<int>(sizez); iz ++) {
                auto block = c.block_data.get_block(ix, iy, iz).value(); // know it has a block
                if (block != Air) continue;

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

                auto ptexcords = [&vertices_uv](Block block, Cardinal dir){
                    auto bd = block_lookup[block];
                    // Bottom Left
                    auto& bl = bd.side_bl;
                    if (dir == Up || dir == Down) bl = bd.up_bl;
                    vertices_uv.emplace_back(Vector2(bl.x+BlockInfo::ts, bl.y+BlockInfo::ts));
                    vertices_uv.emplace_back(Vector2(bl.x, bl.y+BlockInfo::ts));
                    vertices_uv.emplace_back(Vector2(bl.x, bl.y));
                    vertices_uv.emplace_back(Vector2(bl.x+BlockInfo::ts, bl.y));

                };
                auto get_solid_block = [&](int x, int y, int z) -> std::optional<Block> {
                    std::optional<Block> block = c.block_data.get_block(x, y, z);
                    if (block.has_value() && block.value() != Air) return block.value();

                    // check all the neighbors 
                    // West - East
                    if (west.has_value() && x < 0)
                        block = west.value()->block_data.get_block(x+sizex, y, z);
                    if (east.has_value() && x >= static_cast<int>(sizex))
                        block = east.value()->block_data.get_block(x-sizex, y, z);
                    // South - North
                    if (south.has_value() && z < 0)
                        block = south.value()->block_data.get_block(x, y, z+sizez);
                    if (north.has_value() && z >= static_cast<int>(sizez))
                        block = north.value()->block_data.get_block(x, y, z-sizez);
                    // Down - Up
                    if (down.has_value() && y < 0)
                        block = down.value()->block_data.get_block(x, y+sizey, z);
                    if (up.has_value() && y >= static_cast<int>(sizey))
                        block = up.value()->block_data.get_block(x, y-sizey, z);

                    // Check if we found an alternative - if not we are on the border
                    if (block.has_value() && block.value() != Air) return block.value();
                    return std::nullopt;
                };
                std::optional<Block> neighbor;
                // North
                neighbor = get_solid_block(ix, iy, iz+1);
                if (neighbor.has_value()) {
                    vertices.emplace_back(une);
                    vertices.emplace_back(unw);
                    vertices.emplace_back(lnw);
                    vertices.emplace_back(lne);
                    ptexcords(neighbor.value(), North);
                }
                // West
                neighbor = get_solid_block(ix-1, iy, iz);
                if (neighbor.has_value()) {
                    vertices.emplace_back(usw);
                    vertices.emplace_back(unw);
                    vertices.emplace_back(lnw);
                    vertices.emplace_back(lsw);
                    ptexcords(neighbor.value(), West);
                }
                // South
                neighbor = get_solid_block(ix, iy, iz-1);
                if (neighbor.has_value()) {
                    vertices.emplace_back(use);
                    vertices.emplace_back(usw);
                    vertices.emplace_back(lsw);
                    vertices.emplace_back(lse);
                    ptexcords(neighbor.value(), South);
                }
                // East
                neighbor = get_solid_block(ix+1, iy, iz);
                if (neighbor.has_value()) {
                    vertices.emplace_back(une);
                    vertices.emplace_back(use);
                    vertices.emplace_back(lse);
                    vertices.emplace_back(lne);
                    ptexcords(neighbor.value(), East);
                }
                // Up
                neighbor = get_solid_block(ix, iy+1, iz);
                if (neighbor.has_value()) {
                    vertices.emplace_back(une);
                    vertices.emplace_back(unw);
                    vertices.emplace_back(usw);
                    vertices.emplace_back(use);
                    ptexcords(neighbor.value(), Up);
                }
                // Down
                neighbor = get_solid_block(ix, iy-1, iz);
                if (neighbor.has_value()) {
                    vertices.emplace_back(lne);
                    vertices.emplace_back(lse);
                    vertices.emplace_back(lsw);
                    vertices.emplace_back(lnw);
                    ptexcords(neighbor.value(), Down);
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
    c.dirty = false;
}
bool IndexId::operator==(IndexId const& other) const {
    return x == other.x && y == other.y && z == other.z;
}
size_t IndexHash::operator()(IndexId const& idx) const {
    return std::hash<int>()(idx.x) ^ std::hash<int>()(idx.y << 1) ^ std::hash<int>()(idx.z << 2);

}