#pragma once

#include <graphics/shader.hpp>
#include <glm/glm.hpp>

class Material {
    public:
        virtual ~Material() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void BindDepth(Shader& depthShader) {}
        virtual void UnbindDepth(Shader& depthShader) {}

        virtual Shader& GetShader() = 0;
};