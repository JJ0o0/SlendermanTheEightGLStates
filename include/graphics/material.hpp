#pragma once

#include <glm/glm.hpp>

class Material {
    public:
        virtual ~Material() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;
};