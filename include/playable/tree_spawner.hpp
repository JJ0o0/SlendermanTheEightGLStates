#pragma once

#include <graphics/game_renderer.hpp>
#include <graphics/material.hpp>
#include <playable/collider.hpp>
#include <playable/entity.hpp>
#include <graphics/shader.hpp>
#include <graphics/mesh.hpp>
#include <world.hpp>
#include <memory>

class TreeSpawner {
    public:
        struct Part {
            std::shared_ptr<Mesh> MeshReference;
            std::shared_ptr<Material> MaterialReference;
        };

        struct SpeciesAssets {
            std::vector<Part> Parts;
            std::optional<Collider> CollisionShape;
            float GroundOffset = 0.0f;
        };

        // isFoliagePart decide se deve virar Double-Sided (Para as folhas)
        static SpeciesAssets LoadSpecies(
            World& world, Shader& shader, 
            const std::string& path, 
            const std::function<bool(const std::string&)>& isFoliagePart
        );

        static void Scatter(
            World& world, GameRenderer& renderer, Entity& ground,
            const SpeciesAssets& asset, 
            int count, float minimumSpacing, int maxAttempts
        );
};