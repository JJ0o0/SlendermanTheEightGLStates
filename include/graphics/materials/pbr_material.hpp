#pragma once

#include <graphics/shader.hpp>
#include <graphics/texture.hpp>
#include <graphics/material.hpp>

struct PBRTextureSet {
    Texture* Albedo = nullptr;
    Texture* Normal = nullptr;
    Texture* ARM = nullptr;
};

class PBRMaterial : public Material {
    public:
        PBRMaterial(Shader& shader) : m_shader(shader) {}

        void SetTextures(const PBRTextureSet& set) { m_textures = set; }
        const PBRTextureSet GetTextures() const { return m_textures; }

        void Bind() override;
        void Unbind() override {}
    private:
        Shader& m_shader;

        PBRTextureSet m_textures;
};