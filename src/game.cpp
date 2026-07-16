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

    glViewport(0,0,m_window.GetProperties().Width, m_window.GetProperties().Height);

    m_skyboxRenderer->Render(m_player);
    m_renderer->Render(m_world, m_player, m_player.GetFlashlight(), m_shadowRenderer->GetLightSpaceMatrix(), m_shadowRenderer->GetShadowMap());
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
    model.GetTransform().Scale = glm::vec3(10.0f);

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