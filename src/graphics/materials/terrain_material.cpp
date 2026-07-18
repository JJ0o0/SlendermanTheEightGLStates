#include <graphics/materials/terrain_material.hpp>

void TerrainMaterial::Bind() {
    int slot = 5;

    auto& grass = grassFallbacks();
    auto& grassAlbedo = m_textures.Grass.Albedo ? m_textures.Grass.Albedo : grass.Albedo;
    grassAlbedo->Bind(slot);
    m_shader.SetInt("uGrassAlbedoMap", slot++);

    auto& grassNormal = m_textures.Grass.Normal ? m_textures.Grass.Normal : grass.Normal;
    grassNormal->Bind(slot);
    m_shader.SetInt("uGrassNormalMap", slot++);

    auto& grassArm = m_textures.Grass.ARM ? m_textures.Grass.ARM : grass.ARM;
    grassArm->Bind(slot);
    m_shader.SetInt("uGrassARMMap", slot++);

    auto& dirt = dirtFallbacks();
    auto& dirtAlbedo = m_textures.Dirt.Albedo ? m_textures.Dirt.Albedo : dirt.Albedo;
    dirtAlbedo->Bind(slot);
    m_shader.SetInt("uDirtAlbedoMap", slot++);

    auto& dirtNormal = m_textures.Dirt.Normal ? m_textures.Dirt.Normal : dirt.Normal;
    dirtNormal->Bind(slot);
    m_shader.SetInt("uDirtNormalMap", slot++);

    auto& dirtArm = m_textures.Dirt.ARM ? m_textures.Dirt.ARM : dirt.ARM;
    dirtArm->Bind(slot);
    m_shader.SetInt("uDirtARMMap", slot++);
}

PBRTextureSet& TerrainMaterial::grassFallbacks() {
    static PBRTextureSet fallback {
        std::make_shared<Texture>(80, 140, 60, 255, true),
        std::make_shared<Texture>(128, 128, 255, 255, false),
        std::make_shared<Texture>(255, 200, 0, 255, false)
    };

    return fallback;
}

PBRTextureSet& TerrainMaterial::dirtFallbacks() {
    static PBRTextureSet fallback {
        std::make_shared<Texture>(120, 85, 55, 255, true),
        std::make_shared<Texture>(128, 128, 255, 255, false),
        std::make_shared<Texture>(255, 220, 0, 255, false)
    };

    return fallback;
}