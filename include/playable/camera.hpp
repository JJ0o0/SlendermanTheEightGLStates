#pragma once

#include <glm/glm.hpp>

struct CameraProperties {
    float FieldOfView = 90.0f;
    float AspectRatio = 4.0f / 3.0f;
    float NearPlane = 0.1f;
    float FarPlane = 100.0f;

    float MouseSensitivity = 0.15f;
    float MinimumPitchClamp = -89.0f;
	float MaximumPitchClamp = 89.0f;
};

class Camera {
    public:
        Camera(const CameraProperties& properties = {}) : m_properties(properties) {}

        void HandleMouseMovement(double xpos, double ypos);
        void ResetMouseMovement() { m_firstMouse = true; }

        void SetAspectRatio(int width, int height);

        glm::vec3 GetFront() const { return m_front; }
        glm::vec3 GetRight() const;
        glm::mat4 GetViewMatrix(const glm::vec3& position) const;
        glm::mat4 GetProjectionMatrix() const;
    private:
        CameraProperties m_properties;

        bool m_firstMouse = true;

        float m_yaw = -90.0f;
		float m_pitch = 0.0f;
        float m_speed = 0.0f;

		glm::vec2 m_lastMousePosition{0.0f};

        glm::vec3 m_front = { 0, 0, -1};
        glm::vec3 m_up = { 0, 1, 0};
};