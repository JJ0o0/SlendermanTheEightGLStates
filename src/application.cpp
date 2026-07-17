#include <application.hpp>
#include <game.hpp>
#include <platform/system_info.hpp>
#include <algorithm>
#include <cstdlib>
#include <ctime>

int Application::Run() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    m_window = std::make_unique<Window>();
    if (!m_window->Initialize()) return -1;

    SystemInfo::Initialize();

    m_debugUI = std::make_unique<DebugUI>();
    m_debugUI->Initialize(m_window->GetHandle());

    Game game(*m_window, *m_debugUI);
    game.Initialize();

    float lastTime = glfwGetTime();
    while (!m_window->ShouldClose()) {
        float currTime = glfwGetTime();
        float deltaTime = currTime - lastTime;
        lastTime = currTime;

        deltaTime = std::min(deltaTime, 0.05f);

        m_window->PollEvents();
        m_debugUI->BeginFrame(deltaTime);
            game.Update(deltaTime);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.1f, 0.1f, 0.13f, 1.0f);

            game.RenderDebugUI();
            game.Render();
        m_debugUI->EndFrame();
        m_window->SwapBuffers();
    }

    m_debugUI->Terminate();
    m_window->Destroy();
    return 0;
}