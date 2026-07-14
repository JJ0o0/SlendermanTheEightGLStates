#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
    public:
        Shader(const std::string& vsPath, const std::string& fsPath);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetInt(const std::string& name, const int& value);
        void SetBool(const std::string& name, const bool& value);
        void SetFloat(const std::string& name, const float& value);
        void SetVec2(const std::string& name, const glm::vec2& value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

        bool UniformExists(const std::string& name) { return getUniformLocation(name) != -1; }
    private:
        uint32_t m_id = 0;
        uint32_t m_fragment = 0;

        int getUniformLocation(const std::string& name);
        bool verifyShader(uint32_t shader, bool isProgram);
        std::string readFile(const std::string& path);
};