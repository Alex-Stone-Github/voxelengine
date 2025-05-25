#include "camera.hpp"
#include "display.hpp"

#include <cmath>
#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

void set_view_matrices(Camera const& camera, unsigned int program) {
    unsigned int location;
    glUseProgram(program);
    auto xlook = -sin(camera.yaw);
    auto ylook = sin(camera.pitch);
    auto zlook = -cos(camera.yaw);
    // View
    glm::mat4 view = glm::lookAt(
        glm::vec3(camera.position.x, camera.position.y, camera.position.z),
        glm::vec3(camera.position.x + xlook, camera.position.y + ylook,
        camera.position.z + zlook), glm::vec3(0, 1, 0));
    location = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(location, 1, GL_FALSE, &view[0][0]);
    // Perspective Matrix
    location = glGetUniformLocation(program, "perspective");
    constexpr float fov = 90.0f;
    auto perspective = glm::perspective(fov/180.0f*3.1415f,
        static_cast<float>(width/height), 0.1f, 10000.0f);
    glUniformMatrix4fv(location, 1, GL_FALSE, &perspective[0][0]);

}
void Camera::clamp() {
    pitch = fmin(pitch, M_PI_2);
    pitch = fmax(pitch, -M_PI_2);
}