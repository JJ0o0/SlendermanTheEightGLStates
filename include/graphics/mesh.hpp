#pragma once

#include <graphics/vertex.hpp>
#include <playable/aabb.hpp>
#include <glad/gl.h>
#include <cstdint>
#include <vector>

class Mesh {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();
        
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void SetupInstancing(const std::vector<glm::mat4>& matrices);
        bool IsInstanced() const { return m_instanceVbo != 0; }

        void Draw() const;
        void DrawInstanced() const;

        // NÃO É COLISÃO! São dados dos vertíces para outros fins além de colisão.
        const AABB& GetLocalBounds() const { return m_localBounds; }
    private:
        AABB m_localBounds;

        uint32_t m_vao = 0;
        uint32_t m_vbo = 0;
        uint32_t m_ebo = 0;
        uint32_t m_instanceVbo = 0;

        size_t m_indexCount = 0;
        size_t m_instanceCount = 0;
};