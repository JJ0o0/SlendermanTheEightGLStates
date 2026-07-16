#include <graphics/texture.hpp>
#include <platform/error_handling.hpp>
#include <stb/stb_image.h>

Texture::Texture(const TextureProperties& properties) : m_properties(properties) {
    glGenTextures(1, &m_id);
    ChangeImage(m_properties.ImagePath);
}

Texture::Texture(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool srgb) : m_properties({}) {
    glGenTextures(1, &m_id);
    createSolidColor(r, g, b, a, srgb);
}

Texture::Texture(const uint8_t* data, size_t size, const TextureProperties& properties) : m_properties(properties) {
    glGenTextures(1, &m_id);
    loadFromMemory(data, size);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_id);
}

void Texture::Bind(int unit) const { 
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::Unbind() const { 
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::ChangeImage(const std::string& filepath) {
    if (filepath.empty()) {
        ErrorHandling::ShowError("OpenGL Error", "Filepath empty");
        return;
    }

    Bind(m_properties.Unit);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLenum(m_properties.Wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLenum(m_properties.Wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLenum(m_properties.MinFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLenum(m_properties.MagFilter));

    int width, height, numChannels;
    uint8_t* data = stbi_load(filepath.c_str(), &width, &height, &numChannels, 0);

    if (!data) {
        ErrorHandling::ShowError("OpenGL Error", "Could not load file at {}", filepath);
        return;
    }

    uploadPixels(data, width, height, numChannels);
    m_properties.ImagePath = filepath;
}

void Texture::ChangeWrap(TextureWrapOption option) {
    m_properties.Wrap = option;

    Bind(m_properties.Unit);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLenum(m_properties.Wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLenum(m_properties.Wrap));
}

void Texture::ChangeMinFilter(TextureFilterOption option) {
    m_properties.MinFilter = option;
    m_properties.Mipmaps = m_properties.MinFilter != TextureFilterOption::Linear && m_properties.MinFilter != TextureFilterOption::Nearest;

    Bind(m_properties.Unit);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLenum(m_properties.MinFilter));
}

void Texture::ChangeMagFilter(TextureFilterOption option) {
    m_properties.MagFilter = option;

    Bind(m_properties.Unit);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLenum(m_properties.MagFilter));
}

void Texture::createSolidColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool srgb) {
    uint8_t pixel[4] { r, g, b, a};

    Bind(m_properties.Unit);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8,
        1, 1, 0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixel
    );

    m_properties.Id = m_id;
    m_properties.Width = 1;
    m_properties.Height = 1;
    m_properties.Channels = 4;
    m_properties.Mipmaps = false;
    m_properties.SRGB = srgb;
    m_properties.Wrap = TextureWrapOption::Repeat;
    m_properties.MinFilter = TextureFilterOption::Nearest;
    m_properties.MagFilter = TextureFilterOption::Nearest;
}

void Texture::loadFromMemory(const uint8_t* data, size_t size) {
    Bind(m_properties.Unit);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLenum(m_properties.Wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLenum(m_properties.Wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLenum(m_properties.MinFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLenum(m_properties.MagFilter));

    int width, height, numChannels;
    uint8_t* pixels = stbi_load_from_memory(data, static_cast<int>(size), &width, &height, &numChannels, 0);

    if (!pixels) {
        ErrorHandling::ShowError("OpenGL Error", "Could not decode embedded image data");
        return;
    }

    uploadPixels(pixels, width, height, numChannels);
    m_properties.ImagePath = "<embedded>";
}

void Texture::uploadPixels(uint8_t* pixels, int width, int height, int numChannels) {
    m_properties.Id = static_cast<int>(m_id);
    m_properties.Width = width;
    m_properties.Height = height;
    m_properties.Channels = numChannels;
    m_properties.Mipmaps = m_properties.MinFilter != TextureFilterOption::Linear && m_properties.MinFilter != TextureFilterOption::Nearest;

    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB8;

    switch (numChannels) {
        case 1: internalFormat = GL_R8; format = GL_RED; break;
        case 2: internalFormat = GL_RG8; format = GL_RG; break;
        case 3: internalFormat = m_properties.SRGB ? GL_SRGB8 : GL_RGB8; format = GL_RGB; break;
        case 4: internalFormat = m_properties.SRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8; format = GL_RGBA; break;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

    if (m_properties.Mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
}