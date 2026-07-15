#pragma once

#include <glm/glm.hpp>

struct AABB {
    glm::vec3 Minimum;
    glm::vec3 Maximum;

    bool Intersects(const AABB& other) const {
        return
            Minimum.x <= other.Maximum.x &&
            Maximum.x >= other.Minimum.x &&
            Minimum.y <= other.Maximum.y &&
            Maximum.y >= other.Minimum.y &&
            Minimum.z <= other.Maximum.z &&
            Maximum.z >= other.Minimum.z;
    }
};