#pragma once

#include <graphics/texture.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

struct FlashlightProperties {
    bool Enabled = true;

    float InnerCutoff = 25.0f;
    float OuterCutoff = 35.0f;

    float PositionSwaySpeed = 9.0f;
    float DirectionSwaySpeed = 8.0f;

    float MaxBattery = 100.0f;
    float DrainRate = 2.0f;
    float LowBatteryThreshold = 0.2f;

    glm::vec3 Radiance{18.0f, 17.5f, 16.5f};
};

class Flashlight {
    public:
        Flashlight(const FlashlightProperties& properties = {})
            : m_cookie(TextureProperties{
                .ImagePath = "assets/textures/cookie.png"
            }), m_properties(properties) {}
        
        void Update(float deltatime);

        void Bind(int unit = 3) const;
        void Unbind() const;

        void Toggle() { m_properties.Enabled = !m_properties.Enabled; }
        void SetEnabled(bool enabled) { m_properties.Enabled = enabled; }
        void SetTargetPosition(const glm::vec3& position) { m_targetPosition = position; }
        void SetTargetDirection(const glm::vec3& direction) { m_targetDirection = direction; }

        const glm::vec3 GetPosition() const { return m_position; }
        const glm::vec3 GetDirection() const { return m_direction; }
        float GetBatteryLevel() const { return m_battery; }

        const FlashlightProperties& GetProperties() const { return m_properties; }
    private:
        Texture m_cookie;

        FlashlightProperties m_properties;
        
        glm::vec3 m_targetPosition{0.0f};
        glm::vec3 m_targetDirection{0.0f};

        glm::vec3 m_position{0.0f};
        glm::vec3 m_direction{0.0f};

        float m_time = 0.0f;
        float m_battery = m_properties.MaxBattery;
};