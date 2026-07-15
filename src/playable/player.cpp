#include <playable/player.hpp>

void Player::Update(float deltatime, const World& world) {
    m_velocity.y -= m_properties.Gravity * deltatime;

    glm::vec3 movement = m_velocity * deltatime;

    m_position.x += movement.x;

    if (world.CheckCollision(GetCollision())) {
        m_position.x -= movement.x;
        m_velocity.x = 0.0f;
    }

    m_position.z += movement.z;
    if (world.CheckCollision(GetCollision())) {
        m_position.z -= movement.z;
        m_velocity.z = 0.0f;
    }

    m_position.y += movement.y;
    if( world.CheckCollision(GetCollision())) {
        m_position.y -= movement.y;
        m_velocity.y = 0.0f;
    }

    updateCamera();
    updateFlashlight();
}

void Player::ProcessInput(GLFWwindow* glfwWindow) {
    glm::vec3 forward = m_camera.GetFront();
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    glm::vec3 right = m_camera.GetRight();
    right.y = 0.0f;
    right = glm::normalize(right);

    glm::vec3 direction{0.0f};

    float speed;
    if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = m_properties.SprintSpeed;
    else speed = m_properties.WalkSpeed;

    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) direction += forward;
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) direction -= forward;
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) direction += right;
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) direction -= right;

    if (glm::length(direction) > 0.0f) direction = glm::normalize(direction);

    m_velocity.x = direction.x * speed;
    m_velocity.z = direction.z * speed;
}

AABB Player::GetCollision() const {
    glm::vec3 min = m_position;
    glm::vec3 max = m_position + glm::vec3(
        m_properties.Radius,
        m_properties.Height,
        m_properties.Radius
    );

    return {min,max};
}

void Player::updateCamera() { m_eyePosition = m_position + glm::vec3(0.0f, m_properties.EyeHeight, 0.0f); }

void Player::updateFlashlight() {
    glm::vec3 right = m_camera.GetRight();
    glm::vec3 up = m_camera.GetUp();

    glm::vec3 position = m_eyePosition + right * m_properties.FlashlightOffset.x - up * m_properties.FlashlightOffset.y;

    m_flashlight.SetPosition(position);
    m_flashlight.SetDirection(m_camera.GetFront());
}