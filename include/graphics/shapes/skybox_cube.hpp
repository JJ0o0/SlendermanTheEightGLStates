#pragma once

#include <graphics/skybox_mesh.hpp>
#include <graphics/vertex.hpp>

inline SkyboxMesh CreateSkyboxCube(float size = 10.0f) {
    std::vector<SkyboxVertex> vertices{
        // Back - Z-
        {{-size,  size, -size}},
        {{-size, -size, -size}},
        {{ size, -size, -size}},

        {{ size, -size, -size}},
        {{ size,  size, -size}},
        {{-size,  size, -size}},

        // Left - X-
        {{-size, -size,  size}},
        {{-size, -size, -size}},
        {{-size,  size, -size}},

        {{-size,  size, -size}},
        {{-size,  size,  size}},
        {{-size, -size,  size}},

        // Right - X+
        {{ size, -size, -size}},
        {{ size, -size,  size}},
        {{ size,  size,  size}},

        {{ size,  size,  size}},
        {{ size,  size, -size}},
        {{ size, -size, -size}},

        // Front - Z+
        {{-size, -size,  size}},
        {{-size,  size,  size}},
        {{ size,  size,  size}},

        {{ size,  size,  size}},
        {{ size, -size,  size}},
        {{-size, -size,  size}},

        // Top - Y+
        {{-size,  size, -size}},
        {{ size,  size, -size}},
        {{ size,  size,  size}},

        {{ size,  size,  size}},
        {{-size,  size,  size}},
        {{-size,  size, -size}},

        // Bottom - Y-
        {{-size, -size, -size}},
        {{-size, -size,  size}},
        {{ size, -size, -size}},

        {{ size, -size, -size}},
        {{-size, -size,  size}},
        {{ size, -size,  size}},
    };

    return SkyboxMesh(vertices);
}