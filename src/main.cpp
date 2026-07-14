#include <platform/window.hpp>

int main() {
    Window window{};
    
    while (!window.ShouldClose()) {
        window.PollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.13f, 1.0f);

        window.SwapBuffers();
    }

    return 0;
}