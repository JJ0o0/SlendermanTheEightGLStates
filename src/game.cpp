#include <game.hpp>
#include <platform/config_file.hpp>
#include <platform/asset_manager.hpp>
#include <graphics/model_loader.hpp>
#include <graphics/shapes/cube.hpp>
#include <graphics/shapes/sphere.hpp>
#include <graphics/materials/terrain_material.hpp>
#include <utilities.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

void Game::Initialize() {
    std::cout << "Initializing Game...\n";

    m_window.ToggleLockMouse();

    m_window.OnMouseMove = [&](double xpos, double ypos) {
        if (m_window.IsMouseLocked()) m_player.GetCamera().HandleMouseMovement(xpos, ypos);
    };

    auto debugEnabled = ConfigFile::GetBool("Debug", "DebugTools", false);
    m_window.OnKeyPress = [&, debugEnabled](int key) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                m_window.ToggleLockMouse();

                if (!m_window.IsMouseLocked()) m_player.GetCamera().ResetMouseMovement();
                break;
            case GLFW_KEY_F:
                if (m_player.GetFlashlight().GetBatteryLevel() > 0) m_player.GetFlashlight().Toggle();
                break;
        }

        if (!debugEnabled) return;

        switch (key) {
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
            case GLFW_KEY_F8:
                m_timeStop = !m_timeStop;
                break;
            case GLFW_KEY_N:
                m_player.ToggleNoclip();
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

    if (!m_timeStop) { 
        // CÓDIGOS QUE PODEM SER PARADOS
    }
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
            ImGui::Text("%s", m_timeStop ? "Time Stopped" : "Time Running");
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

            float staminaPercent = m_player.GetStamina() / m_player.GetProperties().MaxStamina;
            char staminaOverlay[16];
            snprintf(staminaOverlay, sizeof(staminaOverlay), "%.0f%%", staminaPercent * 100.0f);

            ImGui::Text("Stamina:");
            ImGui::ProgressBar(staminaPercent, ImVec2(-1, 0), staminaOverlay);

            ImGui::SeparatorText("Flashlight");

            auto& flashlight = m_player.GetFlashlight();

            ImGui::Text("Enabled: %s", flashlight.GetProperties().Enabled ? "On" : "Off");

            float batteryPercent = flashlight.GetBatteryLevel() / 100.0f;

            char batteryOverlay[16];
            snprintf(batteryOverlay, sizeof(batteryOverlay), "%.0f%%", batteryPercent * 100.0f);

            ImGui::ProgressBar(batteryPercent, ImVec2(-1, 0), batteryOverlay);

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

    m_terrainShader = std::make_unique<Shader>(
        "assets/shaders/terrain.vert",
        "assets/shaders/terrain.frag"
    );

    // ENTITIES
    auto& floor = ModelLoader::LoadModelIntoWorld(m_world, "assets/models/MapFloor/map.gltf", *m_terrainShader, true);

    auto grassSet = MaterialUtilities::CreatePBRTextureSet(
        "assets/textures/ground/grass/grass_floor_albedo.jpg",
        "assets/textures/ground/grass/grass_floor_normal.jpg",
        "assets/textures/ground/grass/grass_floor_arm.jpg"
    );

    auto dirtSet = MaterialUtilities::CreatePBRTextureSet(
        "assets/textures/ground/dirt/dirt_floor_albedo.jpg",
        "assets/textures/ground/dirt/dirt_floor_normal.jpg",
        "assets/textures/ground/dirt/dirt_floor_arm.jpg"
    );

    auto terrainMaterial = std::make_shared<TerrainMaterial>(*m_terrainShader);
    
    terrainMaterial->SetTextures(TerrainTextureSet{
        grassSet,
        dirtSet
    });

    floor.ForEachDescendant([&, terrainMaterial](Entity& e) {
        e.SetMaterial(terrainMaterial);
    });

    auto& floorTransform = floor.GetTransform();
    floorTransform.Position.y = -1.0f;

    floor.SetCollider(Collider({320.0f, 0.1f, 190.0f}));

    auto& tree01 = ModelLoader::LoadModelIntoWorld(m_world, "assets/models/Tree01/Tree01.gltf", *m_defaultShader);
    
    auto& tree01Transform = tree01.GetTransform();
    tree01.SnapToGround(m_world);

    tree01.ForEachDescendant([&](Entity& e) {
        if (e.GetName() == "Tree_Branches_02") {
            if (auto pbr = e.GetMaterialAs<PBRMaterial>()) {
                pbr->SetDoubleSided(true);
                pbr->SetAlphaCutout(true);
            }
        }
    });

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
    m_renderer = std::make_unique<GameRenderer>();
    m_skyboxRenderer = std::make_unique<SkyboxRenderer>(*m_skyboxCubemap);
    m_shadowRenderer = std::make_unique<ShadowRenderer>(*m_shadowShader, 2048);
}