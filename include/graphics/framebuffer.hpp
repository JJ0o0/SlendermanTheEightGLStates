#pragma once

#include <cstdint>

struct FramebufferProperties {
    int Width = 800;
    int Height = 600;

    bool HasColor = true;
    bool HasDepth = true;
};

class Framebuffer {
    public:
        Framebuffer(const FramebufferProperties& properties = {});
        ~Framebuffer();

        void Resize(int width, int height);

        void Bind() const;
        void Unbind() const;

        uint32_t GetColorAttachment() const { return m_color; }
        uint32_t GetDepthAttachment() const { return m_depth; }
    private:
        FramebufferProperties m_properties;

        uint32_t m_fbo = 0;
        uint32_t m_color = 0;
        uint32_t m_depth = 0;

        void createAttachments();
        void destroyAttachments();
};