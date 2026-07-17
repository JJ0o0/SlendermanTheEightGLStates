#pragma once

#include <graphics/game_renderer.hpp>
#include <graphics/skybox_renderer.hpp>
#include <graphics/shadow_renderer.hpp>

#include <graphics/ui/debug_ui.hpp>

#include <graphics/materials/pbr_material.hpp>

#include <graphics/mesh.hpp>
#include <graphics/shader.hpp>

#include <graphics/animation/animator.hpp>
#include <graphics/animation/animation_clip.hpp>

#include <platform/window.hpp>

#include <playable/flashlight.hpp>
#include <playable/player.hpp>

#include <world.hpp>
#include <imgui.h>

#include <memory>

class Game {
    public:
        Game(Window& window, DebugUI& debugUI) : m_window(window), m_debugUI(debugUI) {}

        void Initialize();
        void Update(float deltatime);
        void Render();
        void RenderDebugUI();
    private:
        Window& m_window;
        DebugUI& m_debugUI;

        float m_fps = 0.0f;
        float m_fpsTimer = 0.0f;
        int m_frameCount = 0;

        bool m_showDebug = false;

        std::unique_ptr<Shader> m_defaultShader;
        std::unique_ptr<Shader> m_shadowShader;

        std::unique_ptr<Cubemap> m_skyboxCubemap;

        std::unique_ptr<GameRenderer> m_renderer;
        std::unique_ptr<SkyboxRenderer> m_skyboxRenderer;
        std::unique_ptr<ShadowRenderer> m_shadowRenderer;

        std::vector<AnimationClip> m_animations;
        Animator m_animator;
        
        World m_world{};

        Player m_player{};

        void loadResources();
        void calculateFPS(float deltatime);
        int getAproximateFPS() { return std::round(m_fps); }
};