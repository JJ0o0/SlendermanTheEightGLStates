#include <game.hpp>
#include <graphics/shapes/sphere.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <cmath>

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

    m_mesh = std::make_unique<Mesh>(CreateSphere());

    m_normalMap = std::make_shared<Texture>(TextureProperties{
        .ImagePath = "assets/textures/test_normalmap.jpg"
    });

    m_material = std::make_unique<PBRMaterial>(*m_defaultShader);
    m_material->SetAlbedo({0.95f, 0.71f, 0.2f});
    m_material->SetNormalMap(*m_normalMap);
    m_material->SetMetallic(1.0f);
    m_material->SetRoughness(0.15f);

    auto& windowProps = m_window.GetProperties();
    m_camera.SetAspectRatio(windowProps.Width, windowProps.Height);
}

void Game::Update(float deltatime) {
    m_lightTime += deltatime;
}

void Game::Render() {
    m_defaultShader->Bind();
        glm::mat4 model{1.0f};
        model = glm::rotate(model, glm::radians(45.0f), {1.0f, 1.0f, 0.0f});

        float radius = 3.0f;
        
        glm::vec3 lightPosition {
            std::cos(m_lightTime) * radius,
            1.5f,
            std::sin(m_lightTime) * radius - 1.0f
        };

        glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -3.0f);
        m_defaultShader->SetMat4("uModel", model);
        m_defaultShader->SetMat4("uView", m_camera.GetViewMatrix(cameraPosition));
        m_defaultShader->SetMat4("uProjection", m_camera.GetProjectionMatrix());
        m_defaultShader->SetVec3("uCameraPosition", cameraPosition);
        m_defaultShader->SetVec3("uLight.Radiance", glm::vec3(500.0f));
        m_defaultShader->SetVec3("uLight.Position", lightPosition);

        m_material->Bind();
            m_mesh->Draw();
        m_material->Unbind();
    m_defaultShader->Unbind();
}