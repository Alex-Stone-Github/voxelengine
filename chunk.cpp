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

Chunk::Chunk(): vertex_count(0) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbop);
    glBindBuffer(GL_ARRAY_BUFFER, vbop);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbot);
    glBindBuffer(GL_ARRAY_BUFFER, vbot);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    for (size_t ix = 0; ix < Chunk::sizex; ix ++) {
        for (size_t iy = 0; iy < Chunk::sizey; iy ++) {
            for (size_t iz = 0; iz < Chunk::sizez; iz ++) {
                *(get_block(ix, iy, iz).value()) = Stone;
            }
        }
    }
    *get_block(0, 0, 0).value() = Air;
}
Chunk::~Chunk() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbop);
    glDeleteBuffers(1, &vbot);
}
void draw_chunk(
    Chunk const& chunk,
    Camera const& camera,
    unsigned int program
) {
    glUseProgram(program);

    // TODO: Transform matrix
    auto location = glGetUniformLocation(program, "transform");
    auto transform = glm::translate(glm::vec3(0, 0, -1));
    glUniformMatrix4fv(location, 1, GL_FALSE, &transform[0][0]);
    set_view_matrices(camera, program);

    // Draw Calls
    glUseProgram(program);
    glBindVertexArray(chunk.vao);
    glDrawElements(GL_QUADS, chunk.vertex_count, GL_UNSIGNED_INT, 0);
}
std::optional<Block*> Chunk::get_block(size_t ix, size_t iy, size_t iz) {
    if (!(ix < sizex && ix >= 0)) return std::nullopt;
    if (!(iy < sizey && iy >= 0)) return std::nullopt;
    if (!(iz < sizez && iz >= 0)) return std::nullopt;
    return &blocks[ix][iy][iz];
}
void recompute_mesh(
    Chunk& c,
    Chunk const& north,
    Chunk const& west,
    Chunk const& south,
    Chunk const& east,
    Chunk const& up,
    Chunk const& down
) {
    std::vector<Vector3> vertices;
    std::vector<Vector2> vertices_uv;
    // Generation
    for (size_t ix = 0; ix < Chunk::sizex; ix ++) {
        for (size_t iy = 0; iy < Chunk::sizey; iy ++) {
            for (size_t iz = 0; iz < Chunk::sizez; iz ++) {
                auto block = c.get_block(ix, iy, iz).value(); // know it has a block
                if (blockinfo[*block].is_air) continue;

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

                // Push vertices for a quad
                vertices.emplace_back(use);
                vertices.emplace_back(usw);
                vertices.emplace_back(lsw);
                vertices.emplace_back(lse);
                vertices_uv.emplace_back(Vector2(1.0, 1.0));
                vertices_uv.emplace_back(Vector2(0.0, 1.0));
                vertices_uv.emplace_back(Vector2(0.0, 0.0));
                vertices_uv.emplace_back(Vector2(1.0, 0.0));

                // Top Too
                vertices.emplace_back(une);
                vertices.emplace_back(unw);
                vertices.emplace_back(usw);
                vertices.emplace_back(use);
                vertices_uv.emplace_back(Vector2(1.0, 1.0));
                vertices_uv.emplace_back(Vector2(0.0, 1.0));
                vertices_uv.emplace_back(Vector2(0.0, 0.0));
                vertices_uv.emplace_back(Vector2(1.0, 0.0));
            }
        }
    }
    // Compression
    std::vector<Vector3> cvert;
    std::vector<Vector2> cvertuv;
    std::vector<unsigned int> cindex;
    auto has_vertex = [&cvert](Vector3 v) -> std::optional<size_t> {
        auto location = std::find(cvert.begin(), cvert.end(), v);
        if (location == cvert.end()) return std::nullopt;
        return std::distance(cvert.begin(), location);
    };
    for (size_t i = 0; i < vertices.size(); i ++) {
        auto vertex = vertices[i];
        auto vertex_uv = vertices_uv[i];
        auto idx = has_vertex(vertex);
        if (idx.has_value()) {
            cindex.emplace_back(idx.value());
        }
        if (!idx.has_value()) {
            cvert.emplace_back(vertex);
            cvertuv.emplace_back(vertex_uv);

            auto nindex = cvert.size()-1; // Safe because we just pushed
            cindex.emplace_back(nindex);
        }
    }
    // Upload
    // vbop
    glBindVertexArray(c.vao);
    glBindBuffer(GL_ARRAY_BUFFER, c.vbop);
    glBufferData(GL_ARRAY_BUFFER, cvert.size()*sizeof(Vector3), cvert.data(),
        GL_STATIC_DRAW);
    // vbot
    glBindBuffer(GL_ARRAY_BUFFER, c.vbot);
    glBufferData(GL_ARRAY_BUFFER, cvertuv.size()*sizeof(Vector2), cvertuv.data(),
        GL_STATIC_DRAW);
    // ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cindex.size()*sizeof(unsigned int),
        cindex.data(), GL_STATIC_DRAW);

    c.vertex_count = cindex.size();

    std::println("{}\n{}\n{}", vertices.size(), cindex.size(), cvert.size());
    std::ranges::for_each(cindex, [](unsigned int i){
        std::println("{}", i);
    });
    std::ranges::for_each(cvert, [](Vector3 v){
        std::println("{}, {}, {}", v.x, v.y, v.z);
    });
    std::ranges::for_each(cvertuv, [](Vector2 v){
        std::println("{}, {}", v.x, v.y);
    });
}