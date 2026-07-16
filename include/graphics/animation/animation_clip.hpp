#pragma once

#include <playable/entity.hpp>
#include <glm/glm.hpp>
#include <vector>

enum class AnimationPath { 
    Translation, 
    Rotation, 
    Scale 
};

struct AnimationChannel {
    Entity* Target = nullptr;
    AnimationPath Path;
    std::vector<float> Times;
    std::vector<glm::vec4> Values;
};

class AnimationClip {
    public:
        void AddChannel(AnimationChannel channel);
        void Apply(float time) const;

        float GetDuration() const { return m_duration; }
    private:
        std::vector<AnimationChannel> m_channels;
        
        float m_duration = 0.0f;

        std::pair<size_t, float> findKeyframe(const std::vector<float>& times, float time) const;
};