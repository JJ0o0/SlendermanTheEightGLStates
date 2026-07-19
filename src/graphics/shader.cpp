#include <graphics/shader.hpp>
#include <platform/error_handling.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

Shader::Shader(const std::string& vsPath, const std::string& fsPath) {
    std::string vsContent = readFile(vsPath);
    std::string fsContent = readFile(fsPath);

    if (vsContent.empty() || fsContent.empty()) return;

    const char* vsSource = vsContent.c_str();
    const char* fsSource = fsContent.c_str();

    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsSource, nullptr);
    glCompileShader(vertexShader);

    if (!verifyShader(vertexShader, false)) {
        glDeleteShader(vertexShader);
        return;
    }

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsSource, nullptr);
    glCompileShader(fragmentShader);

    if (!verifyShader(fragmentShader, false)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    glDetachShader(m_id, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!verifyShader(m_id, true)) {
        glDeleteProgram(m_id);
        m_id = 0;
        return;
    }
}

Shader::~Shader() { glDeleteProgram(m_id); }

void Shader::Bind() const { glUseProgram(m_id); }
void Shader::Unbind() const { glUseProgram(0); }

void Shader::SetInt(const std::string& name, const int& value) {
    glProgramUniform1i(m_id, getUniformLocation(name), value);
}

void Shader::SetBool(const std::string& name, const bool& value) {
    glProgramUniform1i(m_id, getUniformLocation(name), value ? 1 : 0);
}

void Shader::SetFloat(const std::string& name, const float& value) {
    glProgramUniform1f(m_id,getUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
    glProgramUniform2fv(m_id, getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    glProgramUniform3fv(m_id,getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
    glProgramUniform4fv(m_id, getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    glProgramUniformMatrix4fv(m_id, getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4Array(const std::string& name, const std::vector<glm::mat4>& values) {
    if (values.empty()) return;
    glProgramUniformMatrix4fv(m_id, getUniformLocation(name + "[0]"), static_cast<GLsizei>(values.size()), GL_FALSE, glm::value_ptr(values[0]));
}

int Shader::getUniformLocation(const std::string& name) {
    int success = glGetUniformLocation(m_id, name.c_str());
    if (success == -1) std::cerr << "Could not find \"" << name << "\" uniform.\n";

    return success;
}

int Shader::getUniformLocationSilent(const std::string& name) {
    return glGetUniformLocation(m_id, name.c_str());
}

bool Shader::verifyShader(uint32_t shader, bool isProgram) {
    int success;
    char infoLog[512];

    if (isProgram) {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(shader, 512, nullptr, infoLog);

            ErrorHandling::ShowError("OpenGL Error", "Error linking shaders: {}", infoLog);
        }
    } else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            ErrorHandling::ShowError("OpenGL Error", "Error creating shaders: {}", infoLog);
        }
    }

    return success;
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        ErrorHandling::ShowError("Error", "Could not open file at \"{}\"", path);
        return "";
    }

    std::ostringstream oss;
    oss << file.rdbuf();

    return oss.str();
}