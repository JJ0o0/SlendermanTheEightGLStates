#include <graphics/skybox_mesh.hpp>
#include <glad/gl.h>

SkyboxMesh::SkyboxMesh(const std::vector<SkyboxVertex>& vertices)
    : m_vertexCount(vertices.size()) {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkyboxVertex), vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkyboxVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

SkyboxMesh::SkyboxMesh(SkyboxMesh&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_vertexCount(other.m_vertexCount) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_vertexCount = 0;
}

SkyboxMesh& SkyboxMesh::operator=(SkyboxMesh&& other) noexcept {
    if (&other == this) return *this;

    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);

    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_vertexCount = other.m_vertexCount;

    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_vertexCount = 0;

    return *this;
}

SkyboxMesh::~SkyboxMesh() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void SkyboxMesh::Draw() const {
    if (m_vao == 0) return;

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}