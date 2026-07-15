#pragma once

#include <playable/camera.hpp>
#include <playable/flashlight.hpp>
#include <playable/aabb.hpp>
#include <world.hpp>
#include <GLFW/glfw3.h>

struct PlayerProperties {
    float WalkSpeed = 3.0f;
    float SprintSpeed = 5.0f;
    float Gravity = 20.0f;

    float Height = 1.8f;
    float Radius = 0.3f;

    float EyeHeight = 1.7f;

    glm::vec2 FlashlightOffset{0.2f, 0.1f};
};

class Player {
    public:
        Player(const PlayerProperties& properties = {})
            : m_properties(properties) {}

        void Update(float deltatime, const World& world);
        void ProcessInput(GLFWwindow* glfwWindow);
        
        AABB GetCollision() const;

        const glm::vec3& GetPosition() const { return m_position; }
        const glm::vec3& GetEyePosition() const { return m_eyePosition; }
        const glm::vec3& GetVelocity() const { return m_velocity; }
        
        Camera& GetCamera() { return m_camera; }
        const Camera& GetCamera() const { return m_camera; }

        Flashlight& GetFlashlight() { return m_flashlight; }
        const Flashlight& GetFlashlight() const { return m_flashlight; }
    private:
        PlayerProperties m_properties;
        
        glm::vec3 m_position{0.0f};
        glm::vec3 m_eyePosition{0.0f};
        glm::vec3 m_velocity{0.0f};
        
        bool m_onGround = false;
        
        Camera m_camera{};
        Flashlight m_flashlight{};

        void updateCamera();
        void updateFlashlight();
};