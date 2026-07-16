#pragma once

#include <glad/gl.h>
#include <cstdint>
#include <string>

enum class TextureWrapOption {
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
};

enum class TextureFilterOption {
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR, // Trilinear Filtering
};

struct TextureProperties {
    int Id = 0;
    int Width = 0;
    int Height = 0;
    int Channels = 0;
    bool Mipmaps = true;
    bool SRGB = false;
    std::string ImagePath = "assets/textures/default.jpg";
    TextureWrapOption Wrap = TextureWrapOption::Repeat;
    TextureFilterOption MinFilter = TextureFilterOption::LinearMipmapLinear;
    TextureFilterOption MagFilter = TextureFilterOption::Linear;
    int Unit = 0;
};

class Texture {
    public:
        Texture(const TextureProperties& properties = {});
        Texture(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool srgb = false);
        Texture(const uint8_t* data, size_t size, const TextureProperties& properties = {});
        ~Texture();

        void ChangeImage(const std::string& filepath);
        void ChangeWrap(TextureWrapOption option);
        void ChangeMinFilter(TextureFilterOption option);
        void ChangeMagFilter(TextureFilterOption option);

        void Bind(int unit = 0) const;
        void Unbind() const;

        const TextureProperties& GetProperties() const { return m_properties; }
    private:
        TextureProperties m_properties;
        uint32_t m_id = 0;

        template <typename T>
        GLenum toGLenum(T option) {
            return static_cast<GLenum>(option);
        }

        void createSolidColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool srgb);
        void loadFromMemory(const uint8_t* data, size_t size);
        void uploadPixels(uint8_t* pixels, int width, int height, int numChannels);
};