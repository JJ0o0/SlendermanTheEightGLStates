#pragma once

#include <playable/camera.hpp>
#include <playable/flashlight.hpp>
#include <graphics/shader.hpp>
#include <graphics/mesh.hpp>
#include <graphics/materials/pbr_material.hpp>
#include <memory>

class GameRenderer {
    public:
        GameRenderer(Shader& shader);

        void Render(const Camera& camera, Flashlight& flashlight);
        void DrawDepth(Shader& shader);
    private:
        Shader& m_shader;
        
        std::unique_ptr<Mesh> m_mesh;

        std::unique_ptr<PBRMaterial> m_material;
        std::shared_ptr<Texture> m_materialAlbedo;
        std::shared_ptr<Texture> m_materialNormal;
        std::shared_ptr<Texture> m_materialARM;

        void draw(const Flashlight& flashlight);
        void setCameraUniforms(const Camera& camera);
        void setLightUniforms(const Camera& camera, Flashlight& flashlight);

        // TEMPORÁRIO
        glm::mat4 getModelMatrix();
};