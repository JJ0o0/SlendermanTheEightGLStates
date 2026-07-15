#include <graphics/framebuffer.hpp>
#include <platform/error_handling.hpp>
#include <glad/gl.h>

Framebuffer::Framebuffer(const FramebufferProperties& properties) 
    : m_properties(properties) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    createAttachments();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ErrorHandling::ShowError("OpenGL Error", "Framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
    destroyAttachments();
    glDeleteFramebuffers(1, &m_fbo);
}

void Framebuffer::Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); }
void Framebuffer::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void Framebuffer::Resize(int width, int height) {
    m_properties.Width = width;
    m_properties.Height = height;

    destroyAttachments();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    createAttachments();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ErrorHandling::ShowError("OpenGL Error", "Framebuffer is not complete after resize");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::createAttachments() {
    // Color
    if (m_properties.HasColor) {
        glGenTextures(1, &m_color);
        glBindTexture(GL_TEXTURE_2D, m_color);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            m_properties.Width, m_properties.Height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color, 0);
    }

    // Depth
    if (m_properties.HasDepth) {
        glGenTextures(1, &m_depth);
        glBindTexture(GL_TEXTURE_2D, m_depth);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_DEPTH_COMPONENT24,
            m_properties.Width, m_properties.Height,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);
    }

    if (!m_properties.HasColor) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
}

void Framebuffer::destroyAttachments() {
    if (m_properties.HasColor && m_color != 0) {
        glDeleteTextures(1, &m_color);
        m_color = 0;
    }

    if (m_properties.HasDepth && m_depth != 0) {
        glDeleteTextures(1, &m_depth);
        m_depth = 0;
    }
}