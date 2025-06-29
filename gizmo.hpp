#pragma once

#include <cstddef>

#include "camera.hpp"
#include "vector3.hpp"

// This class represents a gizmo (pretty much anything that's not a chunk)
struct Gizmo {
    unsigned int vao, vbop, vbot;
    int vertex_count;
    EntityTransform transform;

    Gizmo(Vector3 const* vertices, Vector2 const* uvcords, size_t vertex_count, EntityTransform transform);
    ~Gizmo();

    // No move or copy for now
    Gizmo(Gizmo const &) = delete;
    Gizmo& operator=(Gizmo const &) = delete;
    Gizmo(Gizmo&&);
    Gizmo& operator=(Gizmo&&);
};
void draw_gizmo(Gizmo const& gizmo, Camera const& camera, unsigned int program);