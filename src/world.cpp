#include <world.hpp>

Entity& World::CreateEntity(const std::string& name, Entity* parent) {
    auto entity = std::make_unique<Entity>(name);
    Entity& reference = *entity;

    if (parent) reference.SetParent(parent);

    m_entities.push_back(std::move(entity));

    return reference;
}

bool World::CheckCollision(const AABB& collider) const {
    bool found = false;

    forEachOverlappedCell(collider, [&](int cx, int cz) {
        if (found) return;

        auto it = m_collisionGrid.find(cellKey(cx, cz));
        if (it == m_collisionGrid.end()) return;

        for (Entity* entity : it->second) {
            if (collider.Intersects(entity->GetColliderAABB())) {
                found = true;
                return;
            }
        }
    });

    return found;
}

std::optional<RaycastHit> World::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, bool testWorldBounds, const Entity* ignore) const {
    std::optional<RaycastHit> closest;

    for (const auto& entity : m_entities) {
        if (entity.get() == ignore) continue;
        if (entity->IsInstanced()) continue;

        std::optional<AABB> bounds;

        if (testWorldBounds) { if (entity->HasMesh()) { bounds = GetWorldBounds(*entity); } }
        else { if (entity->HasCollider()) { bounds = entity->GetColliderAABB(); } }

        if (!bounds.has_value()) continue;

        std::optional<float> hit = bounds->RayIntersects(origin, direction, maxDistance);
        if (!hit.has_value()) continue;

        if (!closest.has_value() || (hit.value() < closest->Distance)) {
            closest = RaycastHit{entity.get(),origin + direction * hit.value(),hit.value()};
        }
    }

    return closest;
}

std::optional<AABB> World::GetWorldBounds(const Entity& entity) const {
    std::optional<AABB> result = entity.GetWorldBounds();

    for (const auto& candidate : m_entities) {
        if (candidate->GetParent() != &entity) continue;

        std::optional<AABB> childBounds = GetWorldBounds(*candidate);
        if (!childBounds.has_value()) continue;

        result = result.has_value() ? MergeAABB(result.value(), childBounds.value()) : childBounds;
    }

    return result;
}

void World::BuildCollisionGrid(float cellSize) {
    m_collisionCellSize = cellSize;
    m_collisionGrid.clear();

    for (auto& entity : m_entities) {
        if (entity->IsInstanced()) continue;
        if (!entity->HasCollider()) continue;

        AABB box = entity->GetColliderAABB();

        forEachOverlappedCell(box, [&](int cx, int cz) {
            m_collisionGrid[cellKey(cx, cz)].push_back(entity.get());
        });
    }
}

int64_t World::cellKey(int cx, int cz) {
    return (int64_t(cx) << 32) | uint32_t(cz);
}

void World::forEachOverlappedCell(const AABB& box, const std::function<void(int, int)>& fn) const {
    int minCx = (int)std::floor(box.Minimum.x / m_collisionCellSize);
    int maxCx = (int)std::floor(box.Maximum.x / m_collisionCellSize);
    int minCz = (int)std::floor(box.Minimum.z / m_collisionCellSize);
    int maxCz = (int)std::floor(box.Maximum.z / m_collisionCellSize);

    for (int cx = minCx; cx <= maxCx; cx++) {
        for (int cz = minCz; cz <= maxCz; cz++) {
            fn(cx, cz);
        }
    }
}