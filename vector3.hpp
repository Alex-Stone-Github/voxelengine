#pragma once

struct Vector2{float x, y;};
struct Vector3 {
    float x, y, z;
    bool operator==(Vector3 const& other);
};