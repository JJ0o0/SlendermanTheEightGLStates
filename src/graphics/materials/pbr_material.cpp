#include <graphics/materials/pbr_material.hpp>

void PBRMaterial::Bind() {
    if (m_doubleSided) glDisable(GL_CULL_FACE);

    if (m_shader.UniformExists("uAlphaCutout")) {
        m_shader.SetBool("uAlphaCutout", m_alphaCutout);
    }

    int slot = 0;

    auto& fallback = fallbacks();
    if (m_shader.UniformExists("uAlbedoMap")) {
        auto& albedo = m_textures.Albedo ? m_textures.Albedo : fallback.Albedo;
        albedo->Bind(slot);
        m_shader.SetInt("uAlbedoMap", slot++); 
    }

    if (m_shader.UniformExists("uNormalMap")) {
        auto& normal = m_textures.Normal ? m_textures.Normal : fallback.Normal;
        normal->Bind(slot);
        m_shader.SetInt("uNormalMap", slot++); 
    }

    if (m_shader.UniformExists("uARMMap")) {
        auto& arm = m_textures.ARM ? m_textures.ARM : fallback.ARM;
        arm->Bind(slot);
        m_shader.SetInt("uARMMap", slot++);
    }
}

void PBRMaterial::Unbind() {
    if (m_doubleSided) glEnable(GL_CULL_FACE);
}

void PBRMaterial::BindDepth(Shader& shader) {
    if (m_doubleSided) glDisable(GL_CULL_FACE);

    if (m_alphaCutout && shader.UniformExists("uAlbedoMap")) {
        shader.SetBool("uAlphaCutout", m_alphaCutout);

        auto& fallback = fallbacks();
        auto& albedo = m_textures.Albedo ? m_textures.Albedo : fallback.Albedo;
        albedo->Bind(12);
        shader.SetInt("uAlbedoMap", 12);
    }
}

void PBRMaterial::UnbindDepth(Shader& shader) {
    if (m_doubleSided) glEnable(GL_CULL_FACE);
}

PBRTextureSet& PBRMaterial::fallbacks() {
    static PBRTextureSet fallback {
        std::make_shared<Texture>(255, 255, 255, 255, true),
        std::make_shared<Texture>(128, 128, 255, 255, false),
        std::make_shared<Texture>(255, 128, 0, 255, false)
    };

    return fallback;
}