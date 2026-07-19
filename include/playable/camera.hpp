#pragma once

#include <glm/glm.hpp>

struct CameraBob {
    float Timer = 0.0f;

    float WalkFrequency = 6.0f;
    float RunFrequency = 10.5f;
    float SprintStartFrequency = 15.0f;
    float SprintBurstDecay = 4.0f;

    float WalkAmount = 0.012f;
    float RunAmount = 0.030f;
    float SprintStartAmount = 0.045f;

    float WalkTilt = 0.5f;
    float RunTilt = 1.6f;
    float SprintStartTilt = 2.6f;

    float JitterFrequencyA = 17.0f;
    float JitterFrequencyB = 23.0f;
    float SprintJitterAmount = 1.4f;

    float Smooth = 9.0f;
};

struct CameraProperties {
    float FieldOfView = 90.0f;
    float AspectRatio = 4.0f / 3.0f;
    float NearPlane = 0.1f;
    float FarPlane = 100.0f;

    float SprintFOVBoost = 4.0f;
    float SprintStartFOVBoost = 10.0f;
    float FOVTransitionSpeed = 2.5f;

    float MouseSensitivity = 0.15f;
    float MinimumPitchClamp = -89.0f;
	float MaximumPitchClamp = 89.0f;
};

class Camera {
    public:
        Camera(const CameraProperties& properties = {}) : m_properties(properties) {}

        void HandleMouseMovement(double xpos, double ypos);
        void ResetMouseMovement() { m_firstMouse = true; }

        void UpdateBob(float deltatime, float intensity, bool isSprinting);

        void SetAspectRatio(int width, int height);

        glm::vec3 GetFront() const { return m_front; }
        glm::vec3 GetUp() const { return glm::cross(GetRight(), m_front); }
        glm::vec3 GetWorldUp() const { return m_up; }
        glm::vec3 GetRight() const;
        glm::mat4 GetViewMatrix(glm::vec3 position);
        glm::mat4 GetProjectionMatrix() const;
    private:
        CameraProperties m_properties;
        CameraBob m_bob;

        bool m_firstMouse = true;
        bool m_wasSprinting = false;
        
        float m_yaw = -90.0f;
		float m_pitch = 0.0f;
        float m_roll = 0.0f;
        float m_speed = 0.0f;
        float m_bobOffset = 0.0f;
        float m_jitterPhaseA = 0.0f;
        float m_jitterPhaseB = 0.0f;
        float m_sprintBurst = 0.0f;
        float m_currentFOV = m_properties.FieldOfView;

		glm::vec2 m_lastMousePosition{0.0f};

        glm::vec3 m_front = { 0, 0, -1};
        glm::vec3 m_up = { 0, 1, 0};
};