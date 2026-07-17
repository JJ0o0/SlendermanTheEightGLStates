#include <playable/player.hpp>

void Player::Update(float deltatime, const World& world) {
    if (!m_noclip) m_velocity.y -= m_properties.Gravity * deltatime;

    glm::vec3 movement = m_velocity * deltatime;

    m_position.x += movement.x;

    if (!m_noclip && world.CheckCollision(GetCollision())) {
        m_position.x -= movement.x;
        m_velocity.x = 0.0f;
    }

    m_position.z += movement.z;
    if (!m_noclip && world.CheckCollision(GetCollision())) {
        m_position.z -= movement.z;
        m_velocity.z = 0.0f;
    }

    m_position.y += movement.y;
    if (!m_noclip && world.CheckCollision(GetCollision())) {
        m_position.y -= movement.y;
        m_velocity.y = 0.0f;
    }

    updateCamera();
    updateFlashlight(deltatime);

    float intensity = 0.0f;
    switch (m_state) {
        case MovementState::Sprinting: intensity = 1.0f; break;
        case MovementState::Walking: intensity = 0.5f; break;
        default: intensity = 0.0f; break;
    }

    if (!m_noclip) m_camera.UpdateBob(deltatime, intensity, m_state == MovementState::Sprinting);
}

void Player::ProcessInput(GLFWwindow* glfwWindow, float deltatime) {
    glm::vec3 forward = m_camera.GetFront();
    if (!m_noclip) forward.y = 0.0f;
    forward = glm::normalize(forward);

    glm::vec3 right = m_camera.GetRight();
    right.y = 0.0f;
    right = glm::normalize(right);

    glm::vec3 direction{0.0f};

    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) direction += forward;
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) direction -= forward;
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) direction += right;
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) direction -= right;

    if (m_noclip) {
        if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS) direction += m_camera.GetUp();
        if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) direction -= m_camera.GetUp();
    }

    bool isMoving = glm::length(direction) > 0.0f;
    if (isMoving) direction = glm::normalize(direction);

    bool wantsSprint = glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    bool canSprint = wantsSprint && isMoving && !m_isExhausted && m_stamina > 0.0f;

    if (canSprint) m_state = MovementState::Sprinting;
    else if (isMoving) m_state = MovementState::Walking;
    else m_state = MovementState::Idle;

    if (m_state == MovementState::Sprinting) {
        m_stamina -= m_properties.StaminaDrainRate * deltatime;

        if (m_stamina <= 0.0f) {
            m_stamina = 0.0f;
            m_isExhausted = true;
        }
    } else {
        m_stamina += m_properties.StaminaRegenRate * deltatime;
        m_stamina = glm::min(m_stamina, m_properties.MaxStamina);

        if (m_isExhausted && m_stamina >= m_properties.MaxStamina * m_properties.ExhaustedThreshold) {
            m_isExhausted = false;
        }
    }

    float speed;
    switch (m_state) {
        case MovementState::Sprinting: speed = m_properties.SprintSpeed; break;
        case MovementState::Walking: speed = m_properties.WalkSpeed; break;
        default: speed = 0.0f; break;
    }

    m_velocity.x = direction.x * speed;
    m_velocity.z = direction.z * speed;
    if (m_noclip) m_velocity.y = direction.y * speed;
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

const std::string Player::MovementStateToString(MovementState state) {
    switch (state) {
        case MovementState::Idle: return "Idle";
        case MovementState::Walking: return "Walk";
        case MovementState::Sprinting: return "Sprint";
        default: return "Unknown";
    }
}

void Player::updateCamera() { m_eyePosition = m_position + glm::vec3(0.0f, m_properties.EyeHeight, 0.0f); }

void Player::updateFlashlight(float deltatime) {
    glm::vec3 right = m_camera.GetRight();
    glm::vec3 up = m_camera.GetUp();

    glm::vec3 position = m_eyePosition + right * m_properties.FlashlightOffset.x - up * m_properties.FlashlightOffset.y;

    m_flashlight.SetTargetPosition(position);
    m_flashlight.SetTargetDirection(m_camera.GetFront());
    m_flashlight.Update(deltatime);
}