#include <playable/tree_spawner.hpp>
#include <graphics/model_loader.hpp>
#include <graphics/materials/pbr_material.hpp>
#include <utilities.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace {
    // Grid espacial usado só pelo Scatter.
    struct SpatialGrid {
        float cellSize;
        std::unordered_map<int64_t, std::vector<glm::vec3>> cells;

        explicit SpatialGrid(float cellSize) : cellSize(cellSize) {}

        static int64_t Key(int x, int z) {
            return (int64_t(x) << 32) | uint32_t(z);
        }

        glm::ivec2 CellOf(const glm::vec3& p) const {
            return { (int)std::floor(p.x / cellSize), (int)std::floor(p.z / cellSize) };
        }

        bool IsTooClose(const glm::vec3& candidate, float minSpacingSq) const {
            glm::ivec2 c = CellOf(candidate);
            for (int dx = -1; dx <= 1; ++dx)
                for (int dz = -1; dz <= 1; ++dz) {
                    auto it = cells.find(Key(c.x + dx, c.y + dz));
                    if (it == cells.end()) continue;
                    for (const auto& p : it->second) {
                        float ddx = candidate.x - p.x, ddz = candidate.z - p.z;
                        if (ddx * ddx + ddz * ddz < minSpacingSq) return true;
                    }
                }
            return false;
        }

        void Insert(const glm::vec3& p) {
            glm::ivec2 c = CellOf(p);
            cells[Key(c.x, c.y)].push_back(p);
        }
    };
}

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
    transforms.reserve(count);

    AABB groundBounds = ground.GetColliderAABB();
    float groundY = groundBounds.Maximum.y;

    SpatialGrid grid(minimumSpacing);
    float minSpacingSq = minimumSpacing * minimumSpacing;

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(groundBounds.Minimum.x, groundBounds.Maximum.x);
    std::uniform_real_distribution<float> zDist(groundBounds.Minimum.z, groundBounds.Maximum.z);
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> scaleDist(0.85f, 1.15f);

    int attempts = 0;
    while ((int)transforms.size() < count && attempts < maxAttempts) {
        attempts++;

        glm::vec3 candidate{ xDist(rng), groundY, zDist(rng) };
        if (!TerrainUtilities::IsGrassAt(ground, candidate)) continue;
        if (grid.IsTooClose(candidate, minSpacingSq)) continue;

        float scale = scaleDist(rng);

        Transform transform;
        transform.Position = candidate + glm::vec3(0.0f, asset.GroundOffset * scale, 0.0f);
        transform.RotateEuler({ 0.0f, rotDist(rng), 0.0f });
        transform.Scale = glm::vec3(scale);

        Entity& ghostCollider = world.CreateEntity("TreeCollider");
        ghostCollider.GetTransform().Position = transform.Position;

        if (asset.CollisionShape.has_value()) ghostCollider.SetCollider(*asset.CollisionShape);

        transforms.push_back(transform.GetModel());
        grid.Insert(candidate);
    }

    std::cout << "Generated " << transforms.size() << " trees in " << attempts << " tries.\n";

    constexpr float kChunkSize = 50.0f;
    const float kChunkBoundingRadius = kChunkSize * 0.70711f;

    struct ChunkBucket {
        glm::vec3 Center{0.0f};
        std::vector<glm::mat4> Transforms;
    };

    std::unordered_map<int64_t, ChunkBucket> chunks;

    for (const auto& m : transforms) {
        glm::vec3 pos = glm::vec3(m[3]);

        int cx = (int)std::floor(pos.x / kChunkSize);
        int cz = (int)std::floor(pos.z / kChunkSize);
        int64_t key = (int64_t(cx) << 32) | uint32_t(cz);

        auto& bucket = chunks[key];
        if (bucket.Transforms.empty()) {
            bucket.Center = glm::vec3((cx + 0.5f) * kChunkSize, groundY, (cz + 0.5f) * kChunkSize);
        }
        bucket.Transforms.push_back(m);
    }

    for (const auto& entry : chunks) {
        const auto& bucket = entry.second;
        for (const auto& part : asset.Parts) {
            if (part.MeshReference && part.MaterialReference) {
                renderer.AddInstancedBatch(
                    part.MeshReference, part.MaterialReference, bucket.Transforms,
                    bucket.Center, kChunkBoundingRadius
                );
            }
        }
    }
}