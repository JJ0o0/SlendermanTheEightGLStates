#include <graphics/mesh.hpp>
#include <glad/gl.h>
#include <cstddef>
#include <limits>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : m_indexCount(indices.size()) {
    m_localBounds.Minimum = glm::vec3{std::numeric_limits<float>::max()};
    m_localBounds.Maximum = glm::vec3{std::numeric_limits<float>::lowest()};

    for (const Vertex& vertex : vertices) {
        m_localBounds.Minimum = glm::min(m_localBounds.Minimum, vertex.Position);
        m_localBounds.Maximum = glm::max(m_localBounds.Maximum, vertex.Position);
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    glEnableVertexAttribArray(3);

    // Joint Indices
    glVertexAttribIPointer(4, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, JointIndices));
    glEnableVertexAttribArray(4);

    // Joint Weights
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, JointWeights));
    glEnableVertexAttribArray(5);

    // Color
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(6);

    glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_ebo(other.m_ebo), m_instanceVbo(other.m_instanceVbo), 
      m_indexCount(other.m_indexCount), m_instanceCount(other.m_instanceCount),
      m_cachedPositions(std::move(other.m_cachedPositions)), m_cachedColors(std::move(other.m_cachedColors)) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_instanceVbo = 0;
    other.m_indexCount = 0;
    other.m_instanceCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (&other == this) return *this;

    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_instanceVbo);
    glDeleteVertexArrays(1, &m_vao);

    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    m_instanceVbo = other.m_instanceVbo;
    m_indexCount = other.m_indexCount;
    m_instanceCount = other.m_instanceCount;
    m_cachedPositions = std::move(other.m_cachedPositions);
    m_cachedColors = std::move(other.m_cachedColors);

    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_instanceVbo = 0;
    other.m_indexCount = 0;
    other.m_instanceCount = 0;
    other.m_cachedPositions.clear();
    other.m_cachedColors.clear();

    return *this;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_instanceVbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Mesh::CacheVertexData(const std::vector<Vertex>& vertices) {
    m_cachedPositions.reserve(vertices.size());
    m_cachedColors.reserve(vertices.size());

    for (const Vertex& v : vertices) {
        m_cachedPositions.push_back(v.Position);
        m_cachedColors.push_back(v.Color);
    }
}

void Mesh::SetupInstancing(const std::vector<glm::mat4>& matrices) {
    if (m_vao == 0) return;

    m_instanceCount = matrices.size();

    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), matrices.data(), GL_STATIC_DRAW);

    for (int i = 0; i < 4; i++) {
        GLuint location = 7 + i;
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(
            location, 4, GL_FLOAT, GL_FALSE,
            sizeof(glm::mat4),
            (void*)(i * sizeof(glm::vec4))
        );
        glVertexAttribDivisor(location, 1);
    }

    glBindVertexArray(0);
}

void Mesh::Draw() const {
    if (m_vao == 0) return;

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}

void Mesh::DrawInstanced() const {
    if (m_vao == 0 || m_instanceVbo == 0) return;

    glBindVertexArray(m_vao);
    glDrawElementsInstanced(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr, m_instanceCount);
}

glm::vec4 Mesh::GetNearestCachedColor(const glm::vec3& localPosition) const {
    if (m_cachedPositions.empty()) return glm::vec4(0.0f);

    float nearestDistanceSquared = std::numeric_limits<float>::max();
    int nearestIndex = -1;

    for (int i = 0; i < m_cachedPositions.size(); i++) {
        float dx = localPosition.x - m_cachedPositions[i].x;
        float dz = localPosition.z - m_cachedPositions[i].z;
        float distanceSquared = (dx * dx) + (dz * dz);

        if (distanceSquared < nearestDistanceSquared) {
            nearestDistanceSquared = distanceSquared;
            nearestIndex = i;
        }
    }

    return m_cachedColors[nearestIndex];
}