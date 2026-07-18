#include <world.hpp>

Entity& World::CreateEntity(const std::string& name, Entity* parent) {
    auto entity = std::make_unique<Entity>(name);
    Entity& reference = *entity;

    if (parent) reference.SetParent(parent);

    m_entities.push_back(std::move(entity));

    return reference;
}

bool World::CheckCollision(const AABB& collider) const {
    for (const auto& entity : m_entities) {
        if (!entity->HasCollider()) continue;
        if (collider.Intersects(entity->GetColliderAABB())) return true;
    }

    return false;
}

std::optional<RaycastHit> World::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, bool testWorldBounds, const Entity* ignore) const {
    std::optional<RaycastHit> closest;

    for (const auto& entity : m_entities) {
        if (entity.get() == ignore) continue;

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