#pragma once

#include <playable/camera.hpp>
#include <playable/flashlight.hpp>
#include <playable/aabb.hpp>
#include <world.hpp>
#include <GLFW/glfw3.h>

enum class MovementState {
    Idle,
    Walking,
    Sprinting
};

struct PlayerProperties {
    float WalkSpeed = 3.0f;
    float SprintSpeed = 5.0f;
    float Gravity = 20.0f;

    float Height = 1.8f;
    float Radius = 0.3f;

    float EyeHeight = 1.7f;

    float MaxStamina = 5.0f;
    float StaminaDrainRate = 1.0f;
    float StaminaRegenRate = 0.6f;
    float ExhaustedThreshold = 0.15f;

    glm::vec2 FlashlightOffset{0.2f, 0.1f};
};

class Player {
    public:
        Player(const PlayerProperties& properties = {})
            : m_properties(properties), 
              m_stamina(properties.MaxStamina) {}

        void Update(float deltatime, const World& world);
        void ProcessInput(GLFWwindow* glfwWindow, float deltatime);
        
        AABB GetCollision() const;

        const glm::vec3& GetPosition() const { return m_position; }
        const glm::vec3& GetEyePosition() const { return m_eyePosition; }
        const glm::vec3& GetVelocity() const { return m_velocity; }
        
        float GetStamina() const { return m_stamina; }

        void ToggleNoclip() { m_noclip = !m_noclip; }
        void SetNoclip(bool enabled) { m_noclip = enabled; }
        bool IsNoclip() const { return m_noclip; }

        MovementState GetMovementState() const { return m_state; }
        const std::string MovementStateToString(MovementState state);

        Camera& GetCamera() { return m_camera; }
        const Camera& GetCamera() const { return m_camera; }

        Flashlight& GetFlashlight() { return m_flashlight; }
        const Flashlight& GetFlashlight() const { return m_flashlight; }

        const PlayerProperties& GetProperties() const { return m_properties; }
    private:
        PlayerProperties m_properties;
        MovementState m_state = MovementState::Idle;
        
        glm::vec3 m_position{0.0f};
        glm::vec3 m_eyePosition{0.0f};
        glm::vec3 m_velocity{0.0f};
        
        bool m_onGround = false;
        bool m_isExhausted = false;
        bool m_noclip = false;

        float m_stamina;
        
        Camera m_camera{};
        Flashlight m_flashlight{};

        void updateCamera();
        void updateFlashlight(float deltatime);
};