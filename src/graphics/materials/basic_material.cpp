#include <graphics/materials/basic_material.hpp>

void BasicMaterial::Bind() {
    m_shader.Bind();
    if (m_shader.UniformExists("uMaterial.Color")) m_shader.SetVec3("uMaterial.Color", m_color);
}