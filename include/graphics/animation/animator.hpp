#pragma once

#include <graphics/animation/animation_clip.hpp>

class Animator {
    public:
        void Play(const AnimationClip& clip, bool loop = true);
        void Update(float deltatime);

        bool IsPlaying() const { return m_clip != nullptr; }
    private:
        const AnimationClip* m_clip = nullptr;
        
        bool m_loop = true;

        float m_time = 0.0f;
};