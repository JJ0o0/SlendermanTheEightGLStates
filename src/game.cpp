#include <game.hpp>
#include <graphics/shapes/cube.hpp>
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

    m_mesh = std::make_unique<Mesh>(CreateCube());

    auto& windowProps = m_window.GetProperties();
    m_camera.SetAspectRatio(windowProps.Width, windowProps.Height);
}

void Game::Update(float deltatime) {

}

void Game::Render() {
    m_defaultShader->Bind();
        glm::mat4 model{1.0f};
        model = glm::rotate(model, glm::radians(45.0f), {1.0f, 1.0f, 0.0f});

        m_defaultShader->SetMat4("uModel", model);
        m_defaultShader->SetMat4("uView", m_camera.GetViewMatrix(glm::vec3(0.0f, 0.0f, 3.0f)));
        m_defaultShader->SetMat4("uProjection", m_camera.GetProjectionMatrix());
        m_mesh->Draw();
    m_defaultShader->Unbind();
}