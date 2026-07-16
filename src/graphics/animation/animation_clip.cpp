#include <graphics/animation/animation_clip.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <utility>

void AnimationClip::AddChannel(AnimationChannel channel) {
    if (!channel.Times.empty()) {
        m_duration = std::max(m_duration, channel.Times.back());
    }

    m_channels.push_back(std::move(channel));
}

void AnimationClip::Apply(float time) const {
    for (const auto& channel : m_channels) {
        if (!channel.Target || channel.Times.empty()) continue;

        auto [index, t] = findKeyframe(channel.Times, time);
        const glm::vec4& a = channel.Values[index];
        const glm::vec4& b = channel.Values[std::min(index + 1, channel.Values.size() - 1)];

        Transform& transform = channel.Target->GetTransform();
        switch (channel.Path) {
            case AnimationPath::Translation:
                transform.Position = glm::mix(glm::vec3(a), glm::vec3(b), t);
                break;
            case AnimationPath::Scale:
                transform.Scale = glm::mix(glm::vec3(a), glm::vec3(b), t);
                break;
            case AnimationPath::Rotation: {
                glm::quat qa(a.w, a.x, a.y, a.z);
                glm::quat qb(b.w, b.x, b.y, b.z);
                glm::quat result = glm::slerp(qa, qb, t);

                transform.Rotation = glm::degrees(glm::eulerAngles(result));
                break;
            }
        }
    }
}

std::pair<size_t, float> AnimationClip::findKeyframe(const std::vector<float>& times, float time) const {
    if (times.size() == 1 || time <= times.front()) return { 0, 0.0f };
    if (time >= times.back()) return { times.size() - 2, 1.0f };

    for (size_t i = 0; i + 1 < times.size(); i++) {
        if (time >= times[i] && time <= times[i + 1]) {
            float span = times[i + 1] - times[i];
            float t = span > 0.0001f ? (time - times[i]) / span : 0.0f;
            return { i, t };
        }
    }

    return { 0, 0.0f };
}
