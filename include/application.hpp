#pragma once

#include <platform/window.hpp>
#include <graphics/ui/debug_ui.hpp>
#include <memory>

class Application {
    public:
        int Run();
    private:
        std::unique_ptr<Window> m_window;
        std::unique_ptr<DebugUI> m_debugUI;
};