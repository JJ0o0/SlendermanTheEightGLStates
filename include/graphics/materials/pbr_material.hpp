#pragma once

#include <graphics/shader.hpp>
#include <graphics/texture.hpp>
#include <graphics/material.hpp>

class PBRMaterial : public Material {
    public:
        PBRMaterial(Shader& shader) : m_shader(shader) {}

        void SetAlbedo(const glm::vec3& albedo) { m_albedo = albedo; }
        void SetNormalMap(Texture& texture) { m_normalMap = &texture; }
        void SetMetallic(float metallic) { m_metallic = glm::clamp(metallic, 0.0f, 1.0f); }
        void SetRoughness(float roughness) { m_roughness = glm::clamp(roughness, 0.0f, 1.0f); }
        void SetAO(float ao) { m_ao = glm::clamp(ao, 0.0f, 1.0f); }

        void Bind() override;
        void Unbind() override {}
    private:
        Shader& m_shader;

        glm::vec3 m_albedo{1.0f};
        Texture* m_normalMap = nullptr;
        float m_metallic = 0.0f;
        float m_roughness = 0.5f;
        float m_ao = 1.0f;
};