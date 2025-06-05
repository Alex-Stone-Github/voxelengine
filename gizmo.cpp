#include "gizmo.hpp"

#ifdef WIN
#include "GL/glew.h"
#else
#include <GL/glew.h>
#endif
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

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
    auto transform = glm::translate(glm::vec3(gizmo.position.x, gizmo.position.y, gizmo.position.z));
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