#include <game.hpp>
#include <graphics/shapes/cube.hpp>
#include <graphics/shapes/sphere.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

void Game::Initialize() {
    std::cout << "Initializing Game...\n";

    m_window.ToggleLockMouse();

    m_window.OnMouseMove = [&](double xpos, double ypos) {
        if (m_window.IsMouseLocked()) m_camera.HandleMouseMovement(xpos, ypos);
    };

    m_window.OnKeyPress = [&](int key) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                m_window.ToggleLockMouse();

                if (!m_window.IsMouseLocked()) m_camera.ResetMouseMovement();
                break;
        }
    };

    m_window.OnSizeChanged = [&](int width, int height) {
        m_camera.SetAspectRatio(width, height);
    };

    m_defaultShader = std::make_unique<Shader>(
        "assets/shaders/default.vert",
        "assets/shaders/default.frag"
    );

    m_shadowShader = std::make_unique<Shader>(
        "assets/shaders/shadow_depth.vert",
        "assets/shaders/shadow_depth.frag"
    );
    
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
    
    m_renderer = std::make_unique<GameRenderer>(*m_defaultShader);
    m_skyboxRenderer = std::make_unique<SkyboxRenderer>(*m_skyboxCubemap);
    m_shadowRenderer = std::make_unique<ShadowRenderer>(*m_shadowShader, 2048);

    auto& windowProps = m_window.GetProperties();
    m_camera.SetAspectRatio(windowProps.Width, windowProps.Height);

    std::cout << "Game initialized!\n";
}

void Game::Update(float deltatime) {

}

void Game::Render() {
    glm::vec3 cameraPos = {1.0f, 2.5f, 1.0f};

    glm::vec3 right = m_camera.GetRight();
    glm::vec3 up = glm::normalize(m_camera.GetUp());

    glm::vec3 lightPos = cameraPos + right * 0.2f - up * -0.1f;

    m_flashlight.SetPosition(lightPos);
    m_flashlight.SetDirection(m_camera.GetFront());

    m_shadowRenderer->Render(*m_renderer, m_flashlight);

    glViewport(0,0,m_window.GetProperties().Width, m_window.GetProperties().Height);

    m_skyboxRenderer->Render(m_camera);
    m_renderer->Render(m_camera, m_flashlight, m_shadowRenderer->GetLightSpaceMatrix(), m_shadowRenderer->GetShadowMap());
}