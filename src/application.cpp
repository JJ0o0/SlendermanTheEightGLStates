#include <application.hpp>

int Application::Run() {
    m_window = std::make_unique<Window>();
    
    if (!m_window->Initialize()) return -1;

    while (!m_window->ShouldClose()) {
        m_window->PollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.13f, 1.0f);

        m_window->SwapBuffers();
    }

    m_window->Destroy();
    return 0;
}