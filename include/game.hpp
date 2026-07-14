#pragma once

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
        std::shared_ptr<Texture> m_normalMap;
        Camera m_camera{};

        float m_lightTime = 0.0f;
};