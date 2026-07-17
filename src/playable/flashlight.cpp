#include <playable/flashlight.hpp>
#include <algorithm>
#include <cmath>

void Flashlight::Update(float deltatime) {
    m_time += deltatime;

    if (m_properties.Enabled) { 
        m_battery -= m_properties.DrainRate * deltatime; 
        m_battery = std::clamp(m_battery, 0.0f, m_properties.MaxBattery);
        
        if (m_battery <= 0) SetEnabled(false);

        float percentage = m_battery / m_properties.MaxBattery;

        float targetMultiplier = computeBaseDimming(percentage)
                                * computeSubtleFlicker()
                                * computeLowBatteryFlicker(percentage, deltatime);

        m_intensityMultiplier = glm::mix(m_intensityMultiplier, targetMultiplier, deltatime * m_properties.IntensitySmoothing);
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

float Flashlight::computeBaseDimming(float percentage) const {
    return glm::mix(m_properties.MinIntensityAtEmptyBattery, 1.0f, std::sqrt(percentage));
}

float Flashlight::computeSubtleFlicker() const {
    return 1.0f - m_properties.BaseFlickerAmplitude
        * (0.5f * (1.0f + std::sin(m_time * m_properties.BaseFlickerFrequency)));
}

float Flashlight::computeLowBatteryFlicker(float percentage, float deltatime) {
    if (percentage >= m_properties.LowBatteryThreshold) {
        m_spikeTime = 0.0f;
        return 1.0f;
    }

    float t = std::clamp(1.0f - (percentage / m_properties.LowBatteryThreshold), 0.0f, 1.0f);
    float freq = std::lerp(m_properties.FlickerFrequencyMin, m_properties.FlickerFrequencyMax, t);
    float ampl = std::lerp(m_properties.FlickerAmplitudeMin, m_properties.FlickerAmplitudeMax, t);

    float osci = 1.0f - ampl * (0.5f * (1.0f + std::sin(m_time * freq)));

    if (m_spikeTime < m_properties.SpikeDuration) {
        m_spikeTime += deltatime;
        return m_properties.SpikeMinIntensity;
    }

    float startChance = m_properties.SpikeChanceMax * t * deltatime;
    if ((static_cast<float>(rand()) / RAND_MAX) < startChance) m_spikeTime = 0.0f;

    return osci;
}