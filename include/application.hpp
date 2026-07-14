#pragma once

#include <platform/window.hpp>
#include <memory>

class Application {
    public:
        int Run();
    private:
        std::unique_ptr<Window> m_window;
};