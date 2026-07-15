#include <graphics/skybox_renderer.hpp>

void SkyboxRenderer::Render(Player& player) {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glCullFace(GL_FRONT);
        m_shader.Bind();
            m_shader.SetMat4("uView", player.GetCamera().GetViewMatrix(player.GetEyePosition()));
            m_shader.SetMat4("uProjection", player.GetCamera().GetProjectionMatrix());
            m_cubemap.Bind();
                m_mesh.Draw();
        m_shader.Unbind();
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}