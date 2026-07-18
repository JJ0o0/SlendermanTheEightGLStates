#include <playable/entity.hpp>
#include <world.hpp>
#include <limits>

std::optional<AABB> Entity::GetWorldBounds() const {
    if (!HasMesh()) return std::nullopt;

    const AABB& local = m_mesh->GetLocalBounds();
    glm::mat4 world = GetWorldModel();

    glm::vec3 corners[8] = {
        {local.Minimum.x, local.Minimum.y, local.Minimum.z},
        {local.Maximum.x, local.Minimum.y, local.Minimum.z},
        {local.Minimum.x, local.Maximum.y, local.Minimum.z},
        {local.Maximum.x, local.Maximum.y, local.Minimum.z},
        {local.Minimum.x, local.Minimum.y, local.Maximum.z},
        {local.Maximum.x, local.Minimum.y, local.Maximum.z},
        {local.Minimum.x, local.Maximum.y, local.Maximum.z},
        {local.Maximum.x, local.Maximum.y, local.Maximum.z},
    };

    glm::vec3 worldMin{std::numeric_limits<float>::max()};
    glm::vec3 worldMax{std::numeric_limits<float>::lowest()};

    for (const glm::vec3& corner : corners) {
        glm::vec3 transformed = glm::vec3(world * glm::vec4(corner, 1.0f));
        
        worldMin = glm::min(worldMin, transformed);
        worldMax = glm::max(worldMax, transformed);
    }

    return AABB{ worldMin, worldMax };
}

bool Entity::SnapToGround(const World& world, float maxDistance) {
    std::optional<AABB> bounds = world.GetWorldBounds(*this);
    if (!bounds.has_value()) return false;

    glm::vec3 worldPosition = GetWorldPosition();
    float offset = worldPosition.y - bounds->Minimum.y;

    glm::vec3 rayOrigin = worldPosition;
    rayOrigin.y = bounds->Maximum.y + 0.1f;

    auto hit = world.Raycast(rayOrigin, glm::vec3(0.0f, -1.0f, 0.0f), maxDistance, false, this);
    if (!hit.has_value()) return false;

    m_transform.Position.y = hit->Point.y + offset;
    return true;
}