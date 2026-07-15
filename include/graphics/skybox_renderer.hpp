#pragma once

#include <graphics/cubemap.hpp>
#include <graphics/shader.hpp>
#include <playable/player.hpp>
#include <graphics/skybox_mesh.hpp>
#include <graphics/shapes/skybox_cube.hpp>

class SkyboxRenderer {
    public:
        SkyboxRenderer(Cubemap& cubemap) 
            : m_shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag"), 
              m_mesh(CreateSkyboxCube()),
              m_cubemap(cubemap) {}
        
        void Render(Player& player);
    private:
        Cubemap& m_cubemap;
        Shader m_shader;
        SkyboxMesh m_mesh;
};