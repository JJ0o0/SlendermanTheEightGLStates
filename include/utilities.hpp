#pragma once

#include <graphics/materials/pbr_material.hpp>
#include <platform/asset_manager.hpp>
#include <graphics/texture.hpp>
#include <playable/entity.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <format>

class MaterialUtilities {
    public:
        static inline PBRTextureSet CreatePBRTextureSet(const std::string& albedoPath, const std::string& normalPath, const std::string& armPath) {
            auto albedo = AssetManager<Texture>::GetOrLoad(std::format("{}#srgb", albedoPath), [&]() {
                return std::make_shared<Texture>(TextureProperties{
                    .SRGB = true,
                    .ImagePath = albedoPath
                });
            });

            auto normal = AssetManager<Texture>::GetOrLoad(std::format("{}#linear", normalPath), [&]() {
                return std::make_shared<Texture>(TextureProperties{
                    .ImagePath = normalPath
                });
            });

            auto arm = AssetManager<Texture>::GetOrLoad(std::format("{}#linear", armPath), [&]() {
                return std::make_shared<Texture>(TextureProperties{
                    .ImagePath = armPath
                });
            });

            return { albedo, normal, arm };
        }
};

class TerrainUtilities {
    public:
        static bool IsGrassAt(Entity& terrainRoot, const glm::vec3& worldPosition, float dirtThreshold = 0.5f) {
            Entity* meshEntity = nullptr;

            terrainRoot.ForEachDescendant([&](Entity& e) {
                if (meshEntity) return;
                if (e.HasMesh() && e.GetMesh()->HasCachedVertexData()) meshEntity = &e;
            });

            if (!meshEntity) return false;

            glm::mat4 inverseModel = glm::inverse(meshEntity->GetWorldModel());
            glm::vec3 localPos = glm::vec3(inverseModel * glm::vec4(worldPosition, 1.0f));

            glm::vec4 color = meshEntity->GetMesh()->GetNearestCachedColor(localPos);
            return color.r < dirtThreshold;
        }
};