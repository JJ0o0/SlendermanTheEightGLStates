#pragma once

#include <graphics/vertex.hpp>
#include <glad/gl.h>
#include <cstdint>
#include <vector>

class SkyboxMesh {
    public:
        SkyboxMesh(const std::vector<SkyboxVertex>& vertices);
        ~SkyboxMesh();
        
        SkyboxMesh(const SkyboxMesh&) = delete;
        SkyboxMesh& operator=(const SkyboxMesh&) = delete;
        SkyboxMesh(SkyboxMesh&& other) noexcept;
        SkyboxMesh& operator=(SkyboxMesh&& other) noexcept;

        void Draw() const;
    private:
        uint32_t m_vao = 0;
        uint32_t m_vbo = 0;

        uint32_t m_vertexCount = 0;
};