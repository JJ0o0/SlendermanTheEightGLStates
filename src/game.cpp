#include <game.hpp>
#include <graphics/shapes/cube.hpp>
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

    m_mesh = std::make_unique<Mesh>(CreateCube());

    m_albedo = std::make_shared<Texture>(TextureProperties{
        .SRGB = true,
        .ImagePath = "assets/textures/wood_albedo.jpg",
    });

    m_normal = std::make_shared<Texture>(TextureProperties{
        .ImagePath = "assets/textures/wood_normal.jpg",
    });

    m_arm = std::make_shared<Texture>(TextureProperties{
        .ImagePath = "assets/textures/wood_arm.jpg",
    });

    m_material = std::make_unique<PBRMaterial>(*m_defaultShader);
    m_material->SetTextures(PBRTextureSet{
        m_albedo.get(),
        m_normal.get(),
        m_arm.get()
    });

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

    m_skyboxRenderer = std::make_unique<SkyboxRenderer>(*m_skyboxCubemap);

    auto& windowProps = m_window.GetProperties();
    m_camera.SetAspectRatio(windowProps.Width, windowProps.Height);
}

void Game::Update(float deltatime) {
    m_lightTime += deltatime;
}

void Game::Render() {
    m_skyboxRenderer->Render(m_camera);

    m_defaultShader->Bind();
        glm::mat4 model{1.0f};
        model = glm::translate(model, {0.0f, -1.0f, 0.0f});
        model = glm::scale(model, {10.0f, 0.1f, 10.0f});

        float radius = 3.0f;
        
        glm::vec3 lightPosition {
            std::cos(m_lightTime) * radius,
            1.5f,
            std::sin(m_lightTime) * radius - 1.0f
        };

        m_defaultShader->SetMat4("uModel", model);
        m_defaultShader->SetMat4("uView", m_camera.GetViewMatrix({0.0f, 0.0f, 1.0f}));
        m_defaultShader->SetMat4("uProjection", m_camera.GetProjectionMatrix());
        m_defaultShader->SetVec3("uCameraPosition", {0.0f, 0.0f, 1.0f});
        m_defaultShader->SetVec3("uLight.Radiance", glm::vec3(100.0f));
        m_defaultShader->SetVec3("uLight.Position", lightPosition);
        m_defaultShader->SetVec3("uAmbientColor", {0.08f, 0.09f, 0.12f});
        m_defaultShader->SetFloat("uAmbientIntensity", 0.03f);

        m_material->Bind();
            m_mesh->Draw();
        m_material->Unbind();
    m_defaultShader->Unbind();
}