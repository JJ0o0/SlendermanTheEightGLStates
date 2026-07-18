#pragma once

#include <graphics/materials/pbr_material.hpp>
#include <graphics/texture.hpp>
#include <platform/asset_manager.hpp>
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