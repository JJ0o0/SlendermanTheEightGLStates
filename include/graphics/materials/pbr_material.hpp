#pragma once

#include <graphics/shader.hpp>
#include <graphics/texture.hpp>
#include <graphics/material.hpp>
#include <memory>

struct PBRTextureSet {
    std::shared_ptr<Texture> Albedo;
    std::shared_ptr<Texture> Normal;
    std::shared_ptr<Texture> ARM;
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

        static PBRTextureSet& fallbacks();
};