#pragma once

#include <graphics/game_renderer.hpp>
#include <graphics/shadow_map.hpp>

class ShadowRenderer {
    public:
        ShadowRenderer(Shader& shader, uint32_t shadowSize) 
            : m_depthShader(shader), m_shadowMap(shadowSize) {}

        void Render(const World& world, GameRenderer& renderer, const Flashlight& flashlight);

        uint32_t GetShadowMap() const { return m_shadowMap.GetDepthTexture(); }
        glm::mat4 GetLightSpaceMatrix() const { return m_lightSpaceMatrix; }
    private:
        Shader& m_depthShader;
        ShadowMap m_shadowMap;

        glm::mat4 m_lightSpaceMatrix{1.0f};
        glm::mat4 calculateLightSpace(const Flashlight& flashlight) const;
};