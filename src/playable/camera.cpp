#include <playable/camera.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>

void Camera::HandleMouseMovement(double xpos, double ypos) {
    if (m_firstMouse) {
        m_lastMousePosition.x = xpos;
        m_lastMousePosition.y = ypos;
        m_firstMouse = false;
    }

    glm::vec2 offset{
        xpos - m_lastMousePosition.x,
        m_lastMousePosition.y - ypos,
    };

    m_lastMousePosition.x = xpos;
    m_lastMousePosition.y = ypos;

    offset *= m_properties.MouseSensitivity;

    m_yaw += offset.x;
    m_pitch += offset.y;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(direction);
}

void Camera::UpdateBob(float deltatime, float intensity, bool isSprinting) {
    intensity = glm::clamp(intensity, 0.0f, 1.0f);
    
    if (isSprinting && !m_wasSprinting) { 
        m_sprintBurst = 1.0f;
        m_jitterPhaseA = (static_cast<float>(rand()) / RAND_MAX) * glm::two_pi<float>();
        m_jitterPhaseB = (static_cast<float>(rand()) / RAND_MAX) * glm::two_pi<float>();
    }

    m_wasSprinting = isSprinting;

    m_sprintBurst = glm::mix(m_sprintBurst, 0.0f, deltatime * m_bob.SprintBurstDecay);

    float runFrequency = glm::mix(m_bob.RunFrequency,m_bob.SprintStartFrequency,m_sprintBurst);
    float runAmount = glm::mix(m_bob.RunAmount, m_bob.SprintStartAmount,m_sprintBurst);
    float runTilt = glm::mix(m_bob.RunTilt,m_bob.SprintStartTilt,m_sprintBurst);

    float frequency = glm::mix(m_bob.WalkFrequency, runFrequency, intensity);
    m_bob.Timer += frequency * deltatime;

    float vertical = std::sin(m_bob.Timer * 2.0f);
    float horizontal = std::sin(m_bob.Timer);

    float jitter = (
        std::sin(m_bob.Timer * m_bob.JitterFrequencyA + m_jitterPhaseA) * 0.6f + 
        std::sin(m_bob.Timer * m_bob.JitterFrequencyB + m_jitterPhaseB) * 0.4f
    );

    float bobAmount = glm::mix(m_bob.WalkAmount, runAmount, intensity);
    float tiltAmount = glm::mix(m_bob.WalkTilt, runTilt, intensity);

    float targetOffset = vertical * bobAmount * intensity;
    float targetRoll = horizontal * tiltAmount * intensity + jitter * m_bob.SprintJitterAmount * m_sprintBurst;

    m_bobOffset = glm::mix(m_bobOffset, targetOffset, deltatime * m_bob.Smooth);
    m_roll = glm::mix(m_roll, targetRoll, deltatime * m_bob.Smooth);

    float sprintFOVBoost = glm::mix(m_properties.SprintFOVBoost, m_properties.SprintStartFOVBoost, m_sprintBurst);
    float targetFOV = m_properties.FieldOfView + (isSprinting ? sprintFOVBoost : 0.0f);
    m_currentFOV = glm::mix(m_currentFOV, targetFOV, deltatime * m_properties.FOVTransitionSpeed);
}

void Camera::SetAspectRatio(int width, int height) {
    m_properties.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

glm::vec3 Camera::GetRight() const { return glm::normalize(glm::cross(m_front, m_up)); }

glm::mat4 Camera::GetViewMatrix(glm::vec3 position) {
    position.y += m_bobOffset;

    glm::vec3 front = m_front;
    glm::vec3 up = m_up;

    glm::mat4 roll = glm::rotate(glm::mat4(1.0f),glm::radians(m_roll), front);

    front = glm::normalize(glm::vec3(roll * glm::vec4(front, 0.0f)));
    up = glm::normalize(glm::vec3(roll * glm::vec4(up, 0.0f)));

    return glm::lookAt(
        position,
        position + front,
        up
    );
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(
        glm::radians(m_currentFOV),
        m_properties.AspectRatio,
        m_properties.NearPlane,
        m_properties.FarPlane
    );
};