#include <graphics/game_renderer.hpp>
#include <graphics/skeleton.hpp>
#include <graphics/shapes/cube.hpp>
#include <glm/ext/matrix_transform.hpp>

GameRenderer::GameRenderer(Shader& shader)
    : m_shader(shader) {
    m_debugLineShader = std::make_unique<Shader>(
        "assets/shaders/debug.vert",
        "assets/shaders/debug.frag"
    );

    m_debugCubeMesh = std::make_unique<Mesh>(CreateCube());
}

void GameRenderer::Render(const World& world, Player& player, Flashlight& flashlight, const glm::mat4& lightSpaceMatrix, uint32_t shadowMap) {
    if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    m_shader.Bind();
        m_shader.SetBool("uUnlit", m_unlit);

        setCameraUniforms(player);
        setLightUniforms(flashlight);

        m_shader.SetMat4("uLightSpaceMatrix", lightSpaceMatrix);
        
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, shadowMap);

        m_shader.SetInt("uShadowMap", 4);

        flashlight.Bind(3);
            draw(world);
        flashlight.Unbind();
    m_shader.Unbind();

    if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    drawColliders(world, player);
}

void GameRenderer::DrawDepth(const World& world, Shader& shader) {
    drawScene(world, shader);
}

void GameRenderer::draw(const World& world) {
    drawScene(world, m_shader);
}

void GameRenderer::drawScene(const World& world, Shader& shader) {
    for (auto& entity : world.GetEntities()) {
        if (!entity->GetMesh()) continue;

        if (entity->HasSkeleton()) { shader.SetMat4Array("uBoneMatrices", entity->GetSkeleton()->GetSkinningMatrices()); }

        entity->GetMaterial()->Bind();
            shader.SetMat4("uModel", entity->GetWorldModel());
            entity->GetMesh()->Draw();
        entity->GetMaterial()->Unbind();
    }
}

void GameRenderer::drawColliders(const World& world, Player& player) {
    if (!m_showColliders) return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    m_debugLineShader->Bind();
    m_debugLineShader->SetVec3("uColor", glm::vec3(0.0f, 1.0f, 0.0f));
    m_debugLineShader->SetMat4("uView", player.GetCamera().GetViewMatrix(player.GetEyePosition()));
    m_debugLineShader->SetMat4("uProjection", player.GetCamera().GetProjectionMatrix());

    for (const auto& entity : world.GetEntities()) {
        if (!entity->HasCollider()) continue;

        AABB box = entity->GetColliderAABB();
        glm::vec3 center = (box.Minimum + box.Maximum) * 0.5f;
        glm::vec3 size = box.Maximum - box.Minimum;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), size);
        m_debugLineShader->SetMat4("uModel", model);

        m_debugCubeMesh->Draw();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GameRenderer::setCameraUniforms(Player& player) {
    m_shader.SetMat4("uView", player.GetCamera().GetViewMatrix(player.GetEyePosition()));
    m_shader.SetMat4("uProjection", player.GetCamera().GetProjectionMatrix());
    m_shader.SetVec3("uCameraPosition", player.GetEyePosition());
}

void GameRenderer::setLightUniforms(Flashlight& flashlight) {
    // Flashlight
    const auto& properties = flashlight.GetProperties();
    m_shader.SetBool("uLight.Enabled", properties.Enabled);
    m_shader.SetVec3("uLight.Radiance", properties.Radiance * flashlight.GetIntensityMultiplier());
    m_shader.SetVec3("uLight.Position", flashlight.GetPosition());
    m_shader.SetFloat("uLight.InnerCutoff", std::cos(glm::radians(properties.InnerCutoff)));
    m_shader.SetFloat("uLight.OuterCutoff", std::cos(glm::radians(properties.OuterCutoff)));
    m_shader.SetVec3("uLight.Direction", flashlight.GetDirection());
    m_shader.SetInt("uFlashlightCookie", 3);

    // Ambient
    m_shader.SetVec3("uAmbientColor", {0.01f, 0.01f, 0.015f});
    m_shader.SetFloat("uAmbientIntensity", 0.02f);
}