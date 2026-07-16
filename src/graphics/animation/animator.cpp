#include <graphics/animation/animator.hpp>
#include <cmath>

void Animator::Play(const AnimationClip& clip, bool loop) {
    m_clip = &clip;
    m_loop = loop;
    m_time = 0.0f;
}

void Animator::Update(float deltatime) {
    if (!m_clip) return;

    m_time += deltatime;

    float duration = m_clip->GetDuration();
    if (duration <= 0.0f) return;
    if (m_time > duration) m_time = m_loop ? std::fmod(m_time, duration) : duration;

    m_clip->Apply(m_time);
}