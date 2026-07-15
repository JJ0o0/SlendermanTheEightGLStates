#pragma once

#include <graphics/framebuffer.hpp>
#include <glm/glm.hpp>

class ShadowMap {
    public:
        ShadowMap(uint32_t size)
            : m_framebuffer(FramebufferProperties{
                .Width = static_cast<int>(size),
                .Height = static_cast<int>(size),
                .HasColor = false,
                .HasDepth = true
            }),
              m_size(size) {}

        void Begin();
        void End();

        uint32_t GetDepthTexture() const { return m_framebuffer.GetDepthAttachment(); }

        void SetLightSpaceMatrix(const glm::mat4& matrix) { m_lightSpaceMatrix = matrix; }
        const glm::mat4& GetLightSpaceMatrix() const { return m_lightSpaceMatrix; }
    private:
        Framebuffer m_framebuffer;

        glm::mat4 m_lightSpaceMatrix{1.0f};
        uint32_t m_size;
};