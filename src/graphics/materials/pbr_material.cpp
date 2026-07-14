#include <graphics/materials/pbr_material.hpp>

void PBRMaterial::Bind() {
    m_shader.Bind();

    if (m_shader.UniformExists("uMaterial.Albedo")) m_shader.SetVec3("uMaterial.Albedo", m_albedo);
    if (m_shader.UniformExists("uMaterial.Metallic")) m_shader.SetFloat("uMaterial.Metallic", m_metallic);
    if (m_shader.UniformExists("uMaterial.Roughness")) m_shader.SetFloat("uMaterial.Roughness", m_roughness);
    if (m_shader.UniformExists("uMaterial.AO")) m_shader.SetFloat("uMaterial.AO", m_ao);
    
    if (m_shader.UniformExists("uNormalMap") && m_normalMap != nullptr) { 
        m_normalMap->Bind(1);
        m_shader.SetInt("uNormalMap", 1); 
    }
}