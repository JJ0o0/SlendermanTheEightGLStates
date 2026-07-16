#include <application.hpp>
#include <game.hpp>
#include <cstdlib>
#include <ctime>

int Application::Run() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    m_window = std::make_unique<Window>();
    if (!m_window->Initialize()) return -1;

    Game game(*m_window);
    game.Initialize();

    float lastTime = glfwGetTime();
    while (!m_window->ShouldClose()) {
        float currTime = glfwGetTime();
        float deltaTime = currTime - lastTime;
        lastTime = currTime;

        deltaTime = std::min(deltaTime, 0.05f);

        m_window->PollEvents();
        game.Update(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.13f, 1.0f);

        game.Render();
        m_window->SwapBuffers();
    }

    m_window->Destroy();
    return 0;
}