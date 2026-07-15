#include <graphics/skybox_renderer.hpp>

void SkyboxRenderer::Render(Camera& camera) {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glCullFace(GL_FRONT);
        m_shader.Bind();
            m_shader.SetMat4("uView", camera.GetViewMatrix({0.0f, 0.0f, 1.0f}));
            m_shader.SetMat4("uProjection", camera.GetProjectionMatrix());
            m_cubemap.Bind();
                m_mesh.Draw();
        m_shader.Unbind();
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}