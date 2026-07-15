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

        void Render(const World& world, const Player& player, Flashlight& flashlight, const glm::mat4& lightSpaceMatrix, uint32_t shadowMap);
        void DrawDepth(const World& world, Shader& shader);
    private:
        Shader& m_shader;
        
        void draw(const World& world);
        void drawScene(const World& world, Shader& shader);
        void setCameraUniforms(const Player& camera);
        void setLightUniforms(const Camera& camera, Flashlight& flashlight);
};