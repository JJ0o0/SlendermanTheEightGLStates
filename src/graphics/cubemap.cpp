#include <graphics/cubemap.hpp>
#include <platform/error_handling.hpp>
#include <stb/stb_image.h>
#include <glad/gl.h>

Cubemap::Cubemap(const CubemapProperties& properties)
    : m_properties(properties) {
    glGenTextures(1, &m_id);

    Bind();

    for (uint32_t i = 0; i < properties.Faces.size(); i++) {
        int width, height, channels;

        unsigned char* data = stbi_load(
            properties.Faces[i].c_str(),
            &width,
            &height,
            &channels,
            0
        );

        if (!data) {
            ErrorHandling::ShowError("Cubemap", "Could not load {}", properties.Faces[i]);
            continue;
        }

        GLenum format = GL_RGB;
        switch (channels) {
            case 1: format = GL_RED; break;
            case 2: format = GL_RG; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
        }

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Cubemap::~Cubemap() {
    glDeleteTextures(1, &m_id);
}

void Cubemap::Bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

void Cubemap::Unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}