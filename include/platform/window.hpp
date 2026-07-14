#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <cstdint>

struct WindowProperties {
    std::string Title = "Slenderman: The Eight GL States";
    uint32_t Width = 800;
    uint32_t Height = 600;
    bool VSync = true;
};

class Window {
    public:
        Window(const WindowProperties& properties = {});
        ~Window();

        void PollEvents() const { glfwPollEvents(); }
        void SwapBuffers() const { glfwSwapBuffers(m_handle); }
        void Close() const { glfwSetWindowShouldClose(m_handle, true); }
        bool ShouldClose() const { return glfwWindowShouldClose(m_handle); }

        void ShowError(const std::string& message);

        const WindowProperties& GetProperties() const { return m_properties; }
    private:
        GLFWwindow* m_handle = nullptr;

        WindowProperties m_properties;

        static void errorCallback(int code, const char* message);
};