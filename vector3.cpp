#include "vector3.hpp"

bool Vector3::operator==(Vector3 const& o) {
    return x == o.x && y == o.y && z == o.z;
}