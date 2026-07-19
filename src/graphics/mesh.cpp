#include <graphics/mesh.hpp>
#include <glad/gl.h>
#include <cstddef>
#include <limits>

void Mesh::setupVertexAttributes() {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    glEnableVertexAttribArray(3);

    glVertexAttribIPointer(4, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, JointIndices));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, JointWeights));
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(6);
}

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

    setupVertexAttributes();

    glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_ebo(other.m_ebo), 
      m_indexCount(other.m_indexCount),
      m_cachedPositions(std::move(other.m_cachedPositions)), m_cachedColors(std::move(other.m_cachedColors)) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (&other == this) return *this;

    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);

    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    m_indexCount = other.m_indexCount;
    m_cachedPositions = std::move(other.m_cachedPositions);
    m_cachedColors = std::move(other.m_cachedColors);

    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_indexCount = 0;
    other.m_cachedPositions.clear();
    other.m_cachedColors.clear();

    return *this;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
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

Mesh::InstancedVAOHandle Mesh::CreateInstancedVAO(const std::vector<glm::mat4>& matrices) const {
    InstancedVAOHandle handle;
    if (m_vao == 0) return handle;

    handle.InstanceCount = matrices.size();

    glGenVertexArrays(1, &handle.Vao);
    glBindVertexArray(handle.Vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    setupVertexAttributes();

    glGenBuffers(1, &handle.InstanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, handle.InstanceVbo);
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
    return handle;
}

void Mesh::DrawInstancedVAO(const InstancedVAOHandle& handle, size_t indexCount) {
    if (handle.Vao == 0) return;

    glBindVertexArray(handle.Vao);
    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, handle.InstanceCount);
}

void Mesh::DestroyInstancedVAO(InstancedVAOHandle& handle) {
    if (handle.InstanceVbo) glDeleteBuffers(1, &handle.InstanceVbo);
    if (handle.Vao) glDeleteVertexArrays(1, &handle.Vao);
    handle = {};
}

void Mesh::Draw() const {
    if (m_vao == 0) return;

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
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