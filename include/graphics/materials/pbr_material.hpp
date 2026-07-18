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
        void Unbind() override;

        void BindDepth(Shader& shader) override;
        void UnbindDepth(Shader& shader) override;

        void SetDoubleSided(bool value) { m_doubleSided = value; }
        bool IsDoubleSided() const { return m_doubleSided; }

        void SetAlphaCutout(bool value) { m_alphaCutout = value; }
        bool IsAlphaCutout() const { return m_alphaCutout; }

        Shader& GetShader() override { return m_shader; }
    private:
        Shader& m_shader;

        bool m_alphaCutout = true;
        bool m_doubleSided = false;

        PBRTextureSet m_textures;
        static PBRTextureSet& fallbacks();
};