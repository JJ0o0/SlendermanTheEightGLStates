#include <playable/tree_spawner.hpp>
#include <graphics/model_loader.hpp>
#include <graphics/materials/pbr_material.hpp>
#include <utilities.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include <vector>

TreeSpawner::SpeciesAssets TreeSpawner::LoadSpecies(
    World& world, Shader& shader, 
    const std::string& path, 
    const std::function<bool(const std::string&)>& isFoliagePart
) {
    auto& tree = ModelLoader::LoadModelIntoWorld(world, path, shader);
    tree.SetInstanced(true);
    tree.SnapToGround(world);

    SpeciesAssets asset;
    asset.CollisionShape = tree.HasCollider() ? tree.GetCollider() : std::nullopt;

    float minimumY = std::numeric_limits<float>::max();

    tree.ForEachDescendant([&](Entity& e) {
        if (!e.GetMesh() || !e.GetMaterial()) { e.SetInstanced(true); return; }

        if (isFoliagePart(e.GetName())) {
            if (auto pbrMaterial = e.GetMaterialAs<PBRMaterial>()) {
                pbrMaterial->SetDoubleSided(true);
                pbrMaterial->SetAlphaCutout(true); // só pra ter certeza
            }
        }

        minimumY = std::min(minimumY, e.GetMesh()->GetLocalBounds().Minimum.y);
        asset.Parts.push_back({ e.GetMesh(), e.GetMaterial() });

        e.SetInstanced(true);
    });

    asset.GroundOffset = (minimumY == std::numeric_limits<float>::max()) ? 0.0f : -minimumY;
    return asset;
}

void TreeSpawner::Scatter(
    World& world, GameRenderer& renderer, Entity& ground,
    const SpeciesAssets& asset, 
    int count, float minimumSpacing, int maxAttempts
) {
    std::vector<glm::mat4> transforms;
    std::vector<glm::vec3> acceptedPositions;

    AABB groundBounds = ground.GetColliderAABB();

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(groundBounds.Minimum.x, groundBounds.Maximum.x);
    std::uniform_real_distribution<float> zDist(groundBounds.Minimum.z, groundBounds.Maximum.z);
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> scaleDist(0.85f, 1.15f);

    int attempts = 0;
    while ((int)transforms.size() < count && attempts < maxAttempts) {
        attempts++;

        glm::vec3 candidate{ xDist(rng), 0.0f, zDist(rng) };
        if (!TerrainUtilities::IsGrassAt(ground, candidate)) continue;

        bool tooClose = false;
        for (const auto& accepted : acceptedPositions) {
            float dx = candidate.x - accepted.x;
            float dz = candidate.z - accepted.z;
            if (dx * dx + dz * dz < minimumSpacing * minimumSpacing) { tooClose = true; break; }
        }

        if (tooClose) continue;

        glm::vec3 rayOrigin = candidate;
        rayOrigin.y = groundBounds.Maximum.y + 5.0f;

        auto hit = world.Raycast(rayOrigin, glm::vec3(0.0f, -1.0f, 0.0f), 100.0f, false);
        if (!hit.has_value()) continue;

        float scale = scaleDist(rng);
        
        Transform transform;
        transform.Position = hit->Point + glm::vec3(0.0f, asset.GroundOffset * scale, 0.0f);
        transform.RotateEuler({ 0.0f, rotDist(rng), 0.0f });
        transform.Scale = glm::vec3(scale);

        Entity& ghostCollider = world.CreateEntity("TreeCollider");
        ghostCollider.GetTransform().Position = transform.Position;

        if (asset.CollisionShape.has_value()) ghostCollider.SetCollider(*asset.CollisionShape);

        transforms.push_back(transform.GetModel());
        acceptedPositions.push_back(hit->Point);
    }

    std::cout << "Generated " << transforms.size() << " trees in " << attempts << " tries.\n";

    for (const auto& part : asset.Parts) {
        if (part.MeshReference && part.MaterialReference) { 
            renderer.AddInstancedBatch(part.MeshReference, part.MaterialReference, transforms); 
        }
    }
}