#include <graphics/materials/pbr_material.hpp>

void PBRMaterial::Bind() {
    m_shader.Bind();

    int slot = 0;
    if (m_shader.UniformExists("uAlbedoMap") && m_textures.Albedo != nullptr) { 
        m_textures.Albedo->Bind(slot);
        m_shader.SetInt("uAlbedoMap", slot++); 
    }

    if (m_shader.UniformExists("uNormalMap") && m_textures.Normal != nullptr) { 
        m_textures.Normal->Bind(slot);
        m_shader.SetInt("uNormalMap", slot++); 
    }

    if (m_shader.UniformExists("uARMMap") && m_textures.ARM != nullptr) { 
        m_textures.ARM->Bind(slot);
        m_shader.SetInt("uARMMap", slot++); 
    }
}