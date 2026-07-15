#include <graphics/shadow_map.hpp>
#include <glad/gl.h>

void ShadowMap::Begin() {
    m_framebuffer.Bind();

    glViewport(0, 0, m_size, m_size);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::End() {
    m_framebuffer.Unbind();
}