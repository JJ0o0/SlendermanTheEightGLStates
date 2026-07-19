#pragma once

#include <world.hpp>
#include <playable/flashlight.hpp>
#include <playable/player.hpp>
#include <graphics/shader.hpp>
#include <graphics/mesh.hpp>
#include <graphics/materials/pbr_material.hpp>
#include <graphics/instanced_batch.hpp>

class GameRenderer {
    public:
        GameRenderer();

        void Render(const World& world, Player& player, Flashlight& flashlight, const glm::mat4& lightSpaceMatrix, uint32_t shadowMap);
        void DrawDepth(const World& world, Shader& shader);
        void AddInstancedBatch(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const std::vector<glm::mat4>& transforms);

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
        std::unique_ptr<Shader> m_debugLineShader;
        std::unique_ptr<Mesh> m_debugCubeMesh;
        
        std::vector<InstancedBatch> m_instancedBatches;

        bool m_unlit = false;
        bool m_wireframe = false;
        bool m_showColliders = false;

        void drawScene(
            const World& world,
            Player& player,
            Flashlight& flashlight,
            const glm::mat4& lightSpaceMatrix,
            uint32_t shadowMap
        );

        void drawColliders(const World& world, Player& player);
        void setCameraUniforms(Shader& shader, Player& player);
        void setLightUniforms(Shader& shader, Flashlight& flashlight);
};