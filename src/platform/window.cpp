#include <platform/window.hpp>
#include <platform/error_handling.hpp>
#include <stb/stb_image.h>
#include <iostream>
#include <format>

void Window::errorCallback(int code, const char* message) {
    ErrorHandling::ShowError("GLFW Error", "Code {}: {}", code, message);
}

void Window::framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    glViewport(0, 0, width, height);

    if (window->OnSizeChanged) window->OnSizeChanged(width, height);

    window->m_properties.Width = static_cast<uint32_t>(width);
    window->m_properties.Height = static_cast<uint32_t>(height);
}

void Window::mouseMoveCallback(GLFWwindow* glfwWindow, double xpos, double ypos) {
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    if (window->OnMouseMove) window->OnMouseMove(xpos, ypos);
}

void Window::keyPressCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    switch (action) {
        case GLFW_PRESS: 
            if (window->OnKeyPress) window->OnKeyPress(key);
            break;
    }
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

    glfwSetWindowUserPointer(m_handle, this);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_handle, mouseMoveCallback);
    glfwSetKeyCallback(m_handle, keyPressCallback);
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