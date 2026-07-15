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

void Camera::UpdateBob(float deltatime, float velocity) {
    float targetOffset = 0.0f;
    float targetRoll = 0.0f;

    if (velocity > 0.1f) {
        m_bob.Timer += m_bob.Frequency * deltatime;

        float vertical = std::sin(m_bob.Timer);
        float horizontal = std::sin(m_bob.Timer);

        float intensity = glm::clamp(velocity / 6.0f, 0.0f, 1.0f);

        float bobAmount = glm::mix(
            m_bob.WalkAmount,
            m_bob.RunAmount,
            intensity
        );

        float tiltAmount = glm::mix(
            m_bob.WalkTilt,
            m_bob.RunTilt,
            intensity
        );

        targetOffset = vertical * bobAmount;
        targetRoll = horizontal * tiltAmount;
    }

    m_bobOffset = glm::mix(
        m_bobOffset,
        targetOffset,
        deltatime * m_bob.Smooth
    );

    m_roll = glm::mix(
        m_roll,
        targetRoll,
        deltatime * m_bob.Smooth
    );
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
        glm::radians(m_properties.FieldOfView),
        m_properties.AspectRatio,
        m_properties.NearPlane,
        m_properties.FarPlane
    );
};