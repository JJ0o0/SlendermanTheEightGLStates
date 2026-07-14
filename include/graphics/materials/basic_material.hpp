#pragma once

#include <graphics/shader.hpp>
#include <graphics/material.hpp>

class BasicMaterial : public Material {
    public:
        BasicMaterial(Shader& shader) : m_shader(shader) {}

        void SetColor(const glm::vec3& color) { m_color = color; }

        void Bind() override;
        void Unbind() override {}
    private:
        glm::vec3 m_color{1.0f};
        Shader& m_shader;
};