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