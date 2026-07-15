#include <graphics/shadow_renderer.hpp>

void ShadowRenderer::Render(GameRenderer& renderer, const Flashlight& flashlight) {
    m_lightSpaceMatrix = calculateLightSpace(flashlight);
    
    m_shadowMap.Begin();
        m_depthShader.Bind();
            m_depthShader.SetMat4("uLightSpaceMatrix", m_lightSpaceMatrix);
            renderer.DrawDepth(m_depthShader);
        m_depthShader.Unbind();
    m_shadowMap.End();
}

glm::mat4 ShadowRenderer::calculateLightSpace(const Flashlight& flashlight) const {
    const auto& properties = flashlight.GetProperties();

    glm::mat4 view = glm::lookAt(
        flashlight.GetPosition(), 
        flashlight.GetPosition() + flashlight.GetDirection(), 
        glm::vec3(0.0f, 1.0f, 0.0)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(properties.OuterCutoff * 2.0f + 5.0f), 
        1.0f, 
        0.05f, 
        50.0f
    );

    return projection * view;
}