#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <algorithm>

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

    std::optional<float> RayIntersects(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
        float tMin = 0.0f;
        float tMax = maxDistance;

        for (int axis = 0; axis < 3; axis++) {
            float invDir = 1.0f / direction[axis];

            float t1 = (Minimum[axis] - origin[axis]) * invDir;
            float t2 = (Maximum[axis] - origin[axis]) * invDir;
            
            auto [tEnter, tExit] = std::minmax(t1, t2);
            tMin = std::max(tMin, tEnter);
            tMax = std::min(tMax, tExit);

            if (tMin > tMax) return std::nullopt;
        }

        return tMin;
    }
};

inline AABB MergeAABB(const AABB& a, const AABB& b) {
    return { glm::min(a.Minimum, b.Minimum), glm::max(a.Maximum, b.Maximum) };
}