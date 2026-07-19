#include <graphics/game_renderer.hpp>
#include <graphics/skeleton.hpp>
#include <graphics/shapes/cube.hpp>
#include <glm/ext/matrix_transform.hpp>

GameRenderer::GameRenderer() {
    m_debugLineShader = std::make_unique<Shader>(
        "assets/shaders/debug.vert",
        "assets/shaders/debug.frag"
    );

    m_debugCubeMesh = std::make_unique<Mesh>(CreateCube());
}

GameRenderer::~GameRenderer() {
    for (auto& batch : m_instancedBatches) {
        Mesh::DestroyInstancedVAO(batch.VaoHandle);
    }
}

void GameRenderer::Render(const World& world, Player& player, Flashlight& flashlight, const glm::mat4& lightSpaceMatrix, uint32_t shadowMap, const Cubemap& skybox) {
    if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    flashlight.Bind(3);
    drawScene(
        world,
        player,
        flashlight,
        lightSpaceMatrix,
        shadowMap,
        skybox
    );
    flashlight.Unbind();

    if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    drawColliders(world, player);
}

void GameRenderer::DrawDepth(const World& world, Shader& shader) {
    if (shader.UniformExists("uInstanced")) shader.SetBool("uInstanced", false);

    for (auto& entity : world.GetEntities()) {
        if (!entity->GetMesh()) continue;
        if (entity->IsInstanced()) continue;

        auto& material = entity->GetMaterial();

        material->BindDepth(shader);
            if (entity->HasSkeleton()) shader.SetMat4Array("uBoneMatrices",entity->GetSkeleton()->GetSkinningMatrices());

            shader.SetMat4("uModel", entity->GetWorldModel());
            entity->GetMesh()->Draw();
        material->UnbindDepth(shader);
    }

    if (shader.UniformExists("uInstanced")) shader.SetBool("uInstanced", true);
        constexpr float shadowCullDistance = RenderCullDistance + 20.0f;

        for (auto& batch : m_instancedBatches) {
            float distanceToCamera = glm::length(m_lastCameraPosition - batch.CullCenter);
            if (distanceToCamera > batch.CullRadius + shadowCullDistance) continue;

            batch.MaterialReference->BindDepth(shader);
                Mesh::DrawInstancedVAO(batch.VaoHandle, batch.IndexCount);
            batch.MaterialReference->UnbindDepth(shader);
        }
    if (shader.UniformExists("uInstanced")) shader.SetBool("uInstanced", false);
}

void GameRenderer::AddInstancedBatch(
    const std::shared_ptr<Mesh>& mesh,
    const std::shared_ptr<Material>& material,
    const std::vector<glm::mat4>& transforms,
    const glm::vec3& cullCenter,
    float cullRadius
) {
    InstancedBatch batch;
    batch.MeshReference = mesh;
    batch.MaterialReference = material;
    batch.VaoHandle = mesh->CreateInstancedVAO(transforms);
    batch.IndexCount = mesh->GetIndexCount();
    batch.CullCenter = cullCenter;
    batch.CullRadius = cullRadius;

    m_instancedBatches.push_back(batch);
}

void GameRenderer::drawScene(
    const World& world,
    Player& player,
    Flashlight& flashlight,
    const glm::mat4& lightSpaceMatrix,
    uint32_t shadowMap, 
    const Cubemap& skybox
) {
    m_lastCameraPosition = player.GetEyePosition();

    for (auto& entity : world.GetEntities()) {
        if (!entity->GetMesh()) continue;
        if (entity->IsInstanced()) continue;

        auto& material = entity->GetMaterial();
        Shader& shader = material->GetShader();

        shader.Bind();
            shader.SetBool("uUnlit", m_unlit);
            if (shader.UniformExists("uInstanced")) shader.SetBool("uInstanced", false);

            setCameraUniforms(shader, player);
            setLightUniforms(shader, flashlight);

            shader.SetMat4("uLightSpaceMatrix", lightSpaceMatrix);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, shadowMap);
            shader.SetInt("uShadowMap", 4);

            skybox.Bind(11);
            shader.SetInt("uSkybox", 11);

            if (entity->HasSkeleton()) shader.SetMat4Array("uBoneMatrices", entity->GetSkeleton()->GetSkinningMatrices());

            material->Bind();
                shader.SetMat4("uModel", entity->GetWorldModel());
                entity->GetMesh()->Draw();
            material->Unbind();
        shader.Unbind();
    }

    for (auto& batch : m_instancedBatches) {
        float distanceToCamera = glm::length(m_lastCameraPosition - batch.CullCenter);
        if (distanceToCamera > batch.CullRadius + RenderCullDistance) continue;

        Shader& shader = batch.MaterialReference->GetShader();

        shader.Bind();
            shader.SetBool("uUnlit", m_unlit);
            if (shader.UniformExists("uInstanced")) shader.SetBool("uInstanced", true);

            setCameraUniforms(shader, player);
            setLightUniforms(shader, flashlight);

            shader.SetMat4("uLightSpaceMatrix", lightSpaceMatrix);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, shadowMap);
            shader.SetInt("uShadowMap", 4);

            skybox.Bind(11);
            shader.SetInt("uSkybox", 11);

            batch.MaterialReference->Bind();
                Mesh::DrawInstancedVAO(batch.VaoHandle, batch.IndexCount);
            batch.MaterialReference->Unbind();
        shader.Unbind();
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

void GameRenderer::setCameraUniforms(Shader& shader, Player& player) {
    shader.SetMat4("uView", player.GetCamera().GetViewMatrix(player.GetEyePosition()));
    shader.SetMat4("uProjection", player.GetCamera().GetProjectionMatrix());
    shader.SetVec3("uCameraPosition", player.GetEyePosition());
}

void GameRenderer::setLightUniforms(Shader& shader, Flashlight& flashlight) {
    // Flashlight
    const auto& properties = flashlight.GetProperties();
    shader.SetBool("uLight.Enabled", properties.Enabled);
    shader.SetVec3("uLight.Radiance", properties.Radiance * flashlight.GetIntensityMultiplier());
    shader.SetVec3("uLight.Position", flashlight.GetPosition());
    shader.SetFloat("uLight.InnerCutoff", std::cos(glm::radians(properties.InnerCutoff)));
    shader.SetFloat("uLight.OuterCutoff", std::cos(glm::radians(properties.OuterCutoff)));
    shader.SetVec3("uLight.Direction", flashlight.GetDirection());
    shader.SetInt("uFlashlightCookie", 3);

    // Ambient
    shader.SetVec3("uAmbientColor", {0.01f, 0.01f, 0.015f});
    shader.SetFloat("uAmbientIntensity", 0.02f);
}