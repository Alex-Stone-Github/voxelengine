#include "gizmo.hpp"

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <iostream>

Gizmo::Gizmo(Vector3 const* vertices, Vector2 const* uvcords, size_t vertex_count, Vector3 position):
    vertex_count(vertex_count), position(position) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbop);
    glBindBuffer(GL_ARRAY_BUFFER, vbop);
    glBufferData(GL_ARRAY_BUFFER, vertex_count*sizeof(Vector3), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbot);
    glBindBuffer(GL_ARRAY_BUFFER, vbot);
    glBufferData(GL_ARRAY_BUFFER, vertex_count*sizeof(Vector2), uvcords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);
    glEnableVertexAttribArray(1);
}
void draw_gizmo(Gizmo const& gizmo, Camera const& camera, unsigned int program) {
    glUseProgram(program);

    // TODO: Transform matrix
    auto location = glGetUniformLocation(program, "transform");
    auto translate = glm::translate(glm::vec3(gizmo.position.x, gizmo.position.y, gizmo.position.z));
    auto rotate_yaw = glm::rotate(3.1415f/4.0f, glm::vec3(0.0, 1.0, 0.0));
    auto transform = translate * rotate_yaw;
    glUniformMatrix4fv(location, 1, GL_FALSE, &transform[0][0]);
    set_view_matrices(camera, program);

    // Draw Calls
    glUseProgram(program);
    glBindVertexArray(gizmo.vao);
    glDrawArrays(GL_QUADS, 0, gizmo.vertex_count);
}
Gizmo::~Gizmo() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbop);
    glDeleteBuffers(1, &vbot);
}
Gizmo::Gizmo(Gizmo&& other):
vertex_count(other.vertex_count), position(other.position) {
    vao = other.vao;
    vbop = other.vbop;
    vbot = other.vbot;
    other.vao = 0;
    other.vbop = 0;
    other.vbot = 0;
}
Gizmo& Gizmo::operator=(Gizmo&& other) {
    vertex_count = other.vertex_count;
    position = other.position;
    vao = other.vao;
    vbop = other.vbop;
    vbot = other.vbot;
    other.vao = 0;
    other.vbop = 0;
    other.vbot = 0;
    return *this;

}