#pragma once

#include <world.hpp>
#include <playable/flashlight.hpp>
#include <playable/player.hpp>
#include <graphics/shader.hpp>
#include <graphics/mesh.hpp>
#include <graphics/materials/pbr_material.hpp>

class GameRenderer {
    public:
        GameRenderer(Shader& shader);

        void Render(const World& world, Player& player, Flashlight& flashlight, const glm::mat4& lightSpaceMatrix, uint32_t shadowMap);
        void DrawDepth(const World& world, Shader& shader);

        void ToggleShowColliders() { m_showColliders = !m_showColliders; }
        void SetShowColliders(bool showColliders) { m_showColliders = showColliders; }
        bool IsShowColliders() const { return m_showColliders; }

        void ToggleWireframe() { m_wireframe = !m_wireframe; }
        void SetWireframe(bool wireframe) { m_wireframe = wireframe; }
        bool IsWireframe() const { return m_wireframe; }

        void ToggleUnlit() { m_unlit = !m_unlit; }
        void SetUnlit(bool unlit) { m_unlit = unlit; }
        bool IsUnlit() const { return m_unlit; }
    private:
        Shader& m_shader;

        std::unique_ptr<Shader> m_debugLineShader;
        std::unique_ptr<Mesh> m_debugCubeMesh;
        
        bool m_unlit = false;
        bool m_wireframe = false;
        bool m_showColliders = false;

        void draw(const World& world);
        void drawScene(const World& world, Shader& shader);
        void drawColliders(const World& world, Player& player);
        void setCameraUniforms(Player& player);
        void setLightUniforms(Flashlight& flashlight);
};