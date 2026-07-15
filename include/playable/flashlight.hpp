#pragma once

#include <graphics/texture.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

struct FlashlightProperties {
    bool Enabled = true;

    float InnerCutoff = 25.0f;
    float OuterCutoff = 35.0f;

    glm::vec3 Radiance{18.0f, 17.5f, 16.5f};
};

class Flashlight {
    public:
        Flashlight(const FlashlightProperties& properties = {})
            : m_cookie(TextureProperties{
                .ImagePath = "assets/textures/cookie.png"
            }), m_properties(properties) {}
        
        void Bind(int unit = 3) const;
        void Unbind() const;

        void SetPosition(const glm::vec3& position) { m_position = position; }
        void SetDirection(const glm::vec3& direction) { m_direction = direction; }

        const glm::vec3 GetPosition() const { return m_position; }
        const glm::vec3 GetDirection() const { return m_direction; }

        const FlashlightProperties& GetProperties() const { return m_properties; }
    private:
        Texture m_cookie;

        FlashlightProperties m_properties;
        
        glm::vec3 m_position{0.0f};
        glm::vec3 m_direction{0.0f};
};