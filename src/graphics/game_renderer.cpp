#include <graphics/game_renderer.hpp>
#include <graphics/shapes/cube.hpp>
#include <glm/ext/matrix_transform.hpp>

GameRenderer::GameRenderer(Shader& shader)
    : m_shader(shader) {
    m_mesh = std::make_unique<Mesh>(CreateCube());

    m_materialAlbedo = std::make_shared<Texture>(TextureProperties{
        .SRGB = true,
        .ImagePath = "assets/textures/wood_albedo.jpg",
    });

    m_materialNormal = std::make_shared<Texture>(TextureProperties{
        .ImagePath = "assets/textures/wood_normal.jpg",
    });

    m_materialARM = std::make_shared<Texture>(TextureProperties{
        .ImagePath = "assets/textures/wood_arm.jpg",
    });

    m_material = std::make_unique<PBRMaterial>(m_shader);
    m_material->SetTextures(PBRTextureSet{
        m_materialAlbedo.get(),
        m_materialNormal.get(),
        m_materialARM.get()
    });
}

void GameRenderer::Render(const Camera& camera, Flashlight& flashlight, const glm::mat4& lightSpaceMatrix, uint32_t shadowMap) {
    m_shader.Bind();
        setCameraUniforms(camera);
        setLightUniforms(camera, flashlight);

        m_shader.SetMat4("uLightSpaceMatrix", lightSpaceMatrix);
        
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, shadowMap);

        m_shader.SetInt("uShadowMap", 4);

        draw(flashlight);
    m_shader.Unbind();
}

void GameRenderer::DrawDepth(Shader& shader) {
    drawScene(shader);
}

void GameRenderer::draw(const Flashlight& flashlight) {
    m_material->Bind();
        flashlight.Bind(3);
            drawScene(m_shader);
        flashlight.Unbind();
    m_material->Unbind();
}

void GameRenderer::drawScene(Shader& shader) {
    shader.SetMat4("uModel", getModelMatrix());
    m_mesh->Draw();

    auto cube = glm::mat4(1.0f);
    cube = glm::translate(cube, {0.0f, 1.0f, 0.0f});
    cube = glm::scale(cube, glm::vec3{0.8f});

    shader.SetMat4("uModel", cube);
    m_mesh->Draw();
}

void GameRenderer::setCameraUniforms(const Camera& camera) {
    glm::vec3 cameraPos = {1.0f, 2.5f, 1.0f};

    m_shader.SetMat4("uView", camera.GetViewMatrix(cameraPos));
    m_shader.SetMat4("uProjection", camera.GetProjectionMatrix());
    m_shader.SetVec3("uCameraPosition", cameraPos);
}

void GameRenderer::setLightUniforms(const Camera& camera, Flashlight& flashlight) {
    // Flashlight
    const auto& properties = flashlight.GetProperties();
    m_shader.SetBool("uLight.Enabled", properties.Enabled);
    m_shader.SetVec3("uLight.Radiance", properties.Radiance);
    m_shader.SetVec3("uLight.Position", flashlight.GetPosition());
    m_shader.SetFloat("uLight.InnerCutoff", std::cos(glm::radians(properties.InnerCutoff)));
    m_shader.SetFloat("uLight.OuterCutoff", std::cos(glm::radians(properties.OuterCutoff)));
    m_shader.SetVec3("uLight.Direction", flashlight.GetDirection());
    m_shader.SetInt("uFlashlightCookie", 3);

    // Ambient
    m_shader.SetVec3("uAmbientColor", {0.02f, 0.03f, 0.05f});
    m_shader.SetFloat("uAmbientIntensity", 0.03f);
}

glm::mat4 GameRenderer::getModelMatrix() {
    glm::mat4 model{1.0f};
    model = glm::translate(model, {0,-1,0});
    model = glm::scale(model, {10,0.1,10});

    return model;
}