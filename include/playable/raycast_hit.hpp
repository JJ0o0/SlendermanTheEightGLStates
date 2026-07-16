#pragma once

#include <playable/entity.hpp>
#include <glm/glm.hpp>

struct RaycastHit {
    Entity* HitEntity;
    glm::vec3 Point;
    float Distance;
};