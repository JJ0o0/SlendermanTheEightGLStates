#pragma once

#include <graphics/shader.hpp>
#include <graphics/texture.hpp>
#include <graphics/material.hpp>
#include <graphics/materials/pbr_material.hpp>
#include <algorithm>

struct TerrainTextureSet {
    PBRTextureSet Grass;
    PBRTextureSet Dirt;
};

class TerrainMaterial : public Material {
    public:
        TerrainMaterial(Shader& shader) : m_shader(shader) {}

        void SetTextures(const TerrainTextureSet& set) { m_textures = set; }
        const TerrainTextureSet GetTextures() const { return m_textures; }

        void SetTiling(float tiling) { 
            m_tiling = tiling;
            m_tiling = std::clamp(m_tiling, 0.0f, 100.0f);
        }

        float GetTiling() const { return m_tiling; }

        void Bind() override;
        void Unbind() override {}

        Shader& GetShader() override { return m_shader; }
    private:
        Shader& m_shader;

        TerrainTextureSet m_textures;

        float m_tiling = 0.25f;

        static PBRTextureSet& grassFallbacks();
        static PBRTextureSet& dirtFallbacks();
};