#include <platform/window.hpp>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <iostream>
#include <format>

void Window::errorCallback(int code, const char* message) {
    std::string formattedMessage = std::format("{}: {}", code, message);

    tinyfd_messageBox(
        "GLFW Error",
        formattedMessage.c_str(),
        "ok",
        "error",
        1
    );
}

Window::Window(const WindowProperties& properties)
    : m_properties(properties) {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_handle = glfwCreateWindow(
        m_properties.Width, m_properties.Height,
        m_properties.Title.c_str(),
        nullptr, nullptr
    );

    if (!m_handle) { 
        glfwTerminate(); 
        return; 
    }

    glfwMakeContextCurrent(m_handle);

    if (!gladLoadGL(glfwGetProcAddress)) {
        ShowError("Could not initialize OpenGL");
        glfwDestroyWindow(m_handle);
        glfwTerminate();
        return;
    }

    glViewport(0, 0, m_properties.Width, m_properties.Height);
    glfwSwapInterval(m_properties.VSync);

    std::cout << "Created Window with " << m_properties.Width << "x" << m_properties.Height << "\n";
}

Window::~Window() {
    std::cout << "Destroying Window...\n";

    if (m_handle) glfwDestroyWindow(m_handle); m_handle = nullptr;
    glfwTerminate();
}

void Window::ShowError(const std::string& message) {
    tinyfd_messageBox(
        "Error",
        message.c_str(),
        "ok",
        "error",
        1
    );
}