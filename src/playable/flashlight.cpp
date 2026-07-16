#include <playable/flashlight.hpp>

void Flashlight::Update(float deltatime) {
    m_position = glm::mix(m_position, m_targetPosition, deltatime * m_properties.PositionSwaySpeed);
    m_direction = glm::mix(m_direction, m_targetDirection, deltatime * m_properties.DirectionSwaySpeed);
    
    if (glm::length(m_direction) > 0.0001f) m_direction = glm::normalize(m_direction);
}

void Flashlight::Bind(int unit) const {
    m_cookie.Bind(unit);
}

void Flashlight::Unbind() const {
    m_cookie.Unbind();
}