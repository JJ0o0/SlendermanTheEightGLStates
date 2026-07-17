#pragma once

#include <GLFW/glfw3.h>
#include <cmath>

class DebugUI {
    public:
        void Initialize(GLFWwindow* glfwWindow);
        void Terminate();

        void BeginFrame(float deltatime);
        void EndFrame() const;

        void DrawSystemInfo();
        private:
        // RAM
        static constexpr int RAM_HISTORY_SIZE = 120;
        float m_ramHistory[RAM_HISTORY_SIZE] = {};
        float m_ramSampleTimer = 0.0f;
        int m_ramOffset = 0;
        
        // FPS
        float m_fps = 0.0f;
        float m_fpsTimer = 0.0f;
        int m_frameCount = 0;
        
        int getAproximateFPS() { return std::round(m_fps); }
        void calculateFPS(float deltatime);
        
        void drawRamGraph();
        void setTheme();
};