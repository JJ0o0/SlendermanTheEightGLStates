#pragma once

#include <graphics/skybox_renderer.hpp>
#include <graphics/materials/pbr_material.hpp>
#include <graphics/mesh.hpp>
#include <graphics/shader.hpp>
#include <platform/window.hpp>
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
        std::unique_ptr<Mesh> m_mesh;
        std::unique_ptr<PBRMaterial> m_material;
        std::shared_ptr<Texture> m_albedo;
        std::shared_ptr<Texture> m_normal;
        std::shared_ptr<Texture> m_arm;
        std::unique_ptr<Cubemap> m_skyboxCubemap;
        std::unique_ptr<SkyboxRenderer> m_skyboxRenderer;
        Camera m_camera{};

        float m_lightTime = 0.0f;
};