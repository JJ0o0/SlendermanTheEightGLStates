#pragma once

#include <playable/entity.hpp>
#include <memory>
#include <vector>
#include <string>

class World {
    public:
        Entity& CreateEntity(const std::string& name, Entity* parent = nullptr);
        bool CheckCollision(const AABB& collider) const;

        const std::vector<std::unique_ptr<Entity>>& GetEntities() const { return m_entities; }
    private:
        std::vector<std::unique_ptr<Entity>> m_entities{};
};