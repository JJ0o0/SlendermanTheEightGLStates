#include <game.hpp>
#include <graphics/shapes/cube.hpp>
#include <graphics/shapes/sphere.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <graphics/model_loader.hpp>
#include <iostream>

void Game::Initialize() {
    std::cout << "Initializing Game...\n";

    m_window.ToggleLockMouse();

    m_window.OnMouseMove = [&](double xpos, double ypos) {
        if (m_window.IsMouseLocked()) m_player.GetCamera().HandleMouseMovement(xpos, ypos);
    };

    m_window.OnKeyPress = [&](int key) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                m_window.ToggleLockMouse();

                if (!m_window.IsMouseLocked()) m_player.GetCamera().ResetMouseMovement();
                break;
            case GLFW_KEY_F12:
                m_showDebug = !m_showDebug;
                break;
            case GLFW_KEY_F1:
                if (m_renderer) m_renderer->ToggleWireframe();
                break;
            case GLFW_KEY_F2:
                if (m_renderer) m_renderer->ToggleUnlit();
                break;
            case GLFW_KEY_F3:
                if (m_renderer) m_renderer->ToggleShowColliders();
                break;
            case GLFW_KEY_N:
                m_player.ToggleNoclip();
                break;
            case GLFW_KEY_F:
                if (m_player.GetFlashlight().GetBatteryLevel() > 0) m_player.GetFlashlight().Toggle();
                break;
        }
    };

    m_window.OnSizeChanged = [&](int width, int height) {
        m_player.GetCamera().SetAspectRatio(width, height);
    };

    loadResources();
    
    auto& windowProps = m_window.GetProperties();
    m_player.GetCamera().SetAspectRatio(windowProps.Width, windowProps.Height);

    std::cout << "Game initialized!\n";
}

void Game::Update(float deltatime) {
    m_player.ProcessInput(m_window.GetHandle(), deltatime);
    m_player.Update(deltatime, m_world);
    m_animator.Update(deltatime);
}

void Game::Render() {
    m_shadowRenderer->Render(m_world, *m_renderer, m_player.GetFlashlight());

    // Atualizar viewport porque o shadow renderer mexeu
    glViewport(0,0,m_window.GetProperties().Width, m_window.GetProperties().Height);

    m_skyboxRenderer->Render(m_player);
    m_renderer->Render(m_world, m_player, m_player.GetFlashlight(), m_shadowRenderer->GetLightSpaceMatrix(), m_shadowRenderer->GetShadowMap());
}

void Game::RenderDebugUI() {
    if (!m_showDebug) return;

    ImGui::SetNextWindowPos({15, 15});
    ImGui::SetNextWindowSize({300, 320});
    ImGui::Begin("Debug Statistics", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize
    );

    if (ImGui::BeginTabBar("DebugTabs")) {
        if (ImGui::BeginTabItem("Engine")) { 
            ImGui::Text("VSync: %s", m_window.GetProperties().VSync ? "On" : "Off");
            ImGui::Text("Lit: %s", !m_renderer->IsUnlit() ? "On" : "Off");
            ImGui::Text("Wireframe: %s", m_renderer->IsWireframe() ? "On" : "Off");
            ImGui::Text("Show Colliders: %s", m_renderer->IsShowColliders() ? "On" : "Off");
            ImGui::Text("No Clip: %s", m_player.IsNoclip() ? "On" : "Off");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Player")) {
            ImGui::SeparatorText("Data");

            auto playerPos = m_player.GetPosition();
            ImGui::InputFloat3("Position", &playerPos.x, "%.2f", ImGuiInputTextFlags_ReadOnly);

            auto playerVel = m_player.GetVelocity();
            ImGui::InputFloat3("Velocity", &playerVel.x, "%.2f", ImGuiInputTextFlags_ReadOnly);

            auto playerState = m_player.GetMovementState();
            ImGui::Text("State: %s", m_player.MovementStateToString(playerState).c_str());

            ImGui::SeparatorText("Flashlight");

            auto& flashlight = m_player.GetFlashlight();

            ImGui::Text("Enabled: %s", flashlight.GetProperties().Enabled ? "On" : "Off");

            float batteryPercent = flashlight.GetBatteryLevel() / 100.0f;

            char overlay[16];
            snprintf(overlay, sizeof(overlay), "%.0f%%", batteryPercent * 100.0f);

            ImGui::ProgressBar(batteryPercent, ImVec2(-1, 0), overlay);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("System")) { 
            m_debugUI.DrawSystemInfo(); 
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Game::loadResources() {
    // SHADERS
    m_defaultShader = std::make_unique<Shader>(
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );

    m_shadowShader = std::make_unique<Shader>(
        "assets/shaders/shadow_depth.vert",
        "assets/shaders/shadow_depth.frag"
    );

    // ENTITIES
    auto& floor = m_world.CreateEntity("Floor");
    floor.SetMesh(std::make_shared<Mesh>(CreateCube()));

    auto albedo = std::make_shared<Texture>(TextureProperties{
        .SRGB = true,
        .ImagePath = "assets/textures/wood_albedo.jpg",
    });

    auto normal = std::make_shared<Texture>(TextureProperties{
        .ImagePath = "assets/textures/wood_normal.jpg",
    });

    auto arm = std::make_shared<Texture>(TextureProperties{
        .ImagePath = "assets/textures/wood_arm.jpg",
    });

    auto material = std::make_shared<PBRMaterial>(*m_defaultShader);
    material->SetTextures(PBRTextureSet{
        albedo,
        normal,
        arm
    });

    floor.SetMaterial(material);

    auto& floorTransform = floor.GetTransform();
    floorTransform.Position.y = -1.0f;
    floorTransform.Scale = {50.0f, 0.1f, 50.0f};

    floor.SetCollider(Collider(floorTransform.Scale));

    auto& model = ModelLoader::LoadModelIntoWorld(m_world, "assets/models/spongebob_dancing_18.glb", *m_defaultShader, nullptr, &m_animations);
    //model.GetTransform().RotateEuler({45.0f, 0.0f, 0.0f});
    model.GetTransform().Scale *= 10.0f;
    model.GetTransform().Position.z = 1.5f;
    model.SnapToGround(m_world);
    model.GetTransform().Position.y += 0.1f;

    if (!m_animations.empty()) m_animator.Play(m_animations[0]);

    // CUBEMAPS
    m_skyboxCubemap = std::make_unique<Cubemap>(CubemapProperties{
        .Faces = {
            "assets/textures/skybox/_px.png",
            "assets/textures/skybox/_nx.png",
            "assets/textures/skybox/_ny.png",
            "assets/textures/skybox/_py.png",
            "assets/textures/skybox/_pz.png",
            "assets/textures/skybox/_nz.png"
        }
    });
    
    // RENDERERS
    m_renderer = std::make_unique<GameRenderer>(*m_defaultShader);
    m_skyboxRenderer = std::make_unique<SkyboxRenderer>(*m_skyboxCubemap);
    m_shadowRenderer = std::make_unique<ShadowRenderer>(*m_shadowShader, 2048);
}