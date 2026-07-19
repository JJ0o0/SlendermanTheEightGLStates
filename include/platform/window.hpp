#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <cstdint>
#include <string>

struct WindowProperties {
    std::string Title = "Slenderman: The Eight GL States";
    uint32_t Width = 800;
    uint32_t Height = 600;
    bool VSync = true;
};

class Window {
    public:
        bool Initialize(const WindowProperties& properties = {});
        void Destroy();

        void PollEvents() const { glfwPollEvents(); }
        void SwapBuffers() const { glfwSwapBuffers(m_handle); }

        void Close() const { glfwSetWindowShouldClose(m_handle, true); }
        bool ShouldClose() const { return glfwWindowShouldClose(m_handle); }

        void ToggleVsync() { m_properties.VSync = !m_properties.VSync; glfwSwapInterval(m_properties.VSync);}
        bool IsVsyncOn() const { return m_properties.VSync; }

        void ToggleLockMouse() { glfwSetInputMode(m_handle, GLFW_CURSOR, IsMouseLocked() ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED); }
        bool IsMouseLocked() const { return glfwGetInputMode(m_handle, GLFW_CURSOR) == GLFW_CURSOR_DISABLED; }

        std::function<void(int, int)> OnSizeChanged;
        std::function<void(double, double)> OnMouseMove;
        std::function<void(int)> OnKeyPress;

        GLFWwindow* GetHandle() { return m_handle; }
        const WindowProperties& GetProperties() const { return m_properties; }
    private:
        GLFWwindow* m_handle = nullptr;

        WindowProperties m_properties;

        static void errorCallback(int code, const char* message);
        static void framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
        static void mouseMoveCallback(GLFWwindow* glfwWindow, double xpos, double ypos);
        static void keyPressCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
};