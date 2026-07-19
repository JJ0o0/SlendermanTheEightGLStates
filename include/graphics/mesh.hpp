#pragma once

#include <graphics/vertex.hpp>
#include <playable/aabb.hpp>
#include <glad/gl.h>
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

class Mesh {
    public:
        struct InstancedVAOHandle {
            uint32_t Vao = 0;
            uint32_t InstanceVbo = 0;
            size_t InstanceCount = 0;
        };

        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();
        
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void CacheVertexData(const std::vector<Vertex>& vertices);
        bool HasCachedVertexData() const { return !m_cachedPositions.empty(); }

        InstancedVAOHandle CreateInstancedVAO(const std::vector<glm::mat4>& matrices) const;
        static void DrawInstancedVAO(const InstancedVAOHandle& handle, size_t indexCount);
        static void DestroyInstancedVAO(InstancedVAOHandle& handle);

        void Draw() const;
        size_t GetIndexCount() const { return m_indexCount; }

        glm::vec4 GetNearestCachedColor(const glm::vec3& localPosition) const;

        // NÃO É COLISÃO! São dados dos vertíces para outros fins além de colisão.
        const AABB& GetLocalBounds() const { return m_localBounds; }
    private:
        AABB m_localBounds;

        uint32_t m_vao = 0;
        uint32_t m_vbo = 0;
        uint32_t m_ebo = 0;

        size_t m_indexCount = 0;

        std::vector<glm::vec3> m_cachedPositions;
        std::vector<glm::vec4> m_cachedColors;

        static void setupVertexAttributes();
};