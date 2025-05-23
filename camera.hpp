#pragma once

#include <glm/glm.hpp>

#include "vector3.hpp"

struct Camera {
    Vector3 position;
    double yaw, pitch;
    void clamp();
};
void set_view_matrices(Camera const& camera, unsigned int program);