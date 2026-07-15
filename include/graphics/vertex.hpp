#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 Position{0.0f};
    glm::vec3 Normal{0.0f};
    glm::vec2 TexCoord{0.0f};
    glm::vec3 Tangent{0.0f};
};

struct SkyboxVertex {
    glm::vec3 Position{0.0f};
};

// Futuramente para GUI
/* struct Vertex2D {
    glm::vec2 Position{0.0f};
    glm::vec2 TexCoord{0.0f};
    glm::vec4 Color{1.0f};
};  */