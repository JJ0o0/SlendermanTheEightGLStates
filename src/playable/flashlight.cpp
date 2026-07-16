#include <playable/flashlight.hpp>
#include <algorithm>

void Flashlight::Update(float deltatime) {
    m_time += deltatime;

    if (m_properties.Enabled) { 
        m_battery -= m_properties.DrainRate * deltatime; 
        m_battery = std::clamp(m_battery, 0.0f, m_properties.MaxBattery);
        
        if (m_battery <= 0) SetEnabled(false);
    }

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