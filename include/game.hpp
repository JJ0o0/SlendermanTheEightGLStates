#pragma once

#include <graphics/game_renderer.hpp>
#include <graphics/skybox_renderer.hpp>
#include <graphics/materials/pbr_material.hpp>
#include <graphics/mesh.hpp>
#include <graphics/shader.hpp>
#include <platform/window.hpp>
#include <playable/flashlight.hpp>
#include <playable/camera.hpp>
#include <memory>

class Game {
    public:
        Game(Window& window) : m_window(window) {}

        void Initialize();
        void Update(float deltatime);
        void Render();
    private:
        Window& m_window;

        std::unique_ptr<Shader> m_defaultShader;
        std::unique_ptr<Cubemap> m_skyboxCubemap;
        std::unique_ptr<GameRenderer> m_renderer;
        std::unique_ptr<SkyboxRenderer> m_skyboxRenderer;

        Flashlight m_flashlight{};
        Camera m_camera{};
};