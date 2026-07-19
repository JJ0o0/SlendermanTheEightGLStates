#pragma once

#include <graphics/mesh.hpp>
#include <graphics/material.hpp>
#include <memory>

struct InstancedBatch {
    std::shared_ptr<Mesh> MeshReference;
    std::shared_ptr<Material> MaterialReference;
};