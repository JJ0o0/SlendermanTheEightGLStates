#include <platform/window.hpp>
#include <platform/error_handling.hpp>
#include <stb/stb_image.h>
#include <iostream>
#include <format>

void Window::errorCallback(int code, const char* message) {
    std::string formattedMessage = std::format("{}: {}", code, message);

    ErrorHandling::ShowError("GLFW Error", formattedMessage);
}

bool Window::Initialize(const WindowProperties& properties) {
    m_properties = properties;

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) return false;

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
        return false; 
    }

    glfwMakeContextCurrent(m_handle);

    if (!gladLoadGL(glfwGetProcAddress)) {
        ErrorHandling::ShowError("OpenGL Error", "Could not initialize OpenGL");
        glfwDestroyWindow(m_handle);
        glfwTerminate();
        return false;
    }

    glViewport(0, 0, m_properties.Width, m_properties.Height);
    glfwSwapInterval(m_properties.VSync);

    stbi_set_flip_vertically_on_load(true);

    std::cout << "Created Window with " << m_properties.Width << "x" << m_properties.Height << "\n";
    return true;
}

void Window::Destroy() {
    std::cout << "Destroying Window...\n";

    if (m_handle) glfwDestroyWindow(m_handle); m_handle = nullptr;
    glfwTerminate();
}