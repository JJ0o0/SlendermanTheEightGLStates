#pragma once

#include <graphics/mesh.hpp>
#include <graphics/material.hpp>
#include <glm/glm.hpp>
#include <limits>
#include <memory>

struct InstancedBatch {
    std::shared_ptr<Mesh> MeshReference;
    std::shared_ptr<Material> MaterialReference;

    glm::vec3 CullCenter{0.0f};
    float CullRadius = std::numeric_limits<float>::max(); // sem culling por padrão
};