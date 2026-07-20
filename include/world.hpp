#pragma once

#include <playable/raycast_hit.hpp>
#include <playable/entity.hpp>
#include <functional>
#include <memory>
#include <vector>
#include <string>

class World {
    public:
        Entity& CreateEntity(const std::string& name, Entity* parent = nullptr);
        bool CheckCollision(const AABB& collider) const;
        std::optional<RaycastHit> Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, bool testWorldBounds = false, const Entity* ignore = nullptr) const;

        std::optional<AABB> GetWorldBounds(const Entity& entity) const;
        const std::vector<std::unique_ptr<Entity>>& GetEntities() const { return m_entities; }

        void BuildCollisionGrid(float cellSize = 20.0f);
    private:
        std::vector<std::unique_ptr<Entity>> m_entities{};

        float m_collisionCellSize = 20.0f;
        std::unordered_map<int64_t, std::vector<Entity*>> m_collisionGrid;

        static int64_t cellKey(int cx, int cz);
        void forEachOverlappedCell(const AABB& box, const std::function<void(int, int)>& fn) const;
};