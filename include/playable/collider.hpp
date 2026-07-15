#pragma once

#include <playable/aabb.hpp>
#include <glm/glm.hpp>

class Collider {
    public:
        Collider(const glm::vec3& size) : m_halfSize(size * 0.5f) {}

        AABB GetAABB(const glm::vec3& position) const {
            return {
                position - m_halfSize,
                position + m_halfSize
            };
        }
    private:
        glm::vec3 m_halfSize;
};