#pragma once

#include <array>
#include <cstdint>
#include <string>

struct CubemapProperties {
    int Width = 0;
    int Height = 0;
    int Channels = 0;

    std::array<std::string, 6> Faces;
};

class Cubemap {
    public:
        Cubemap(const CubemapProperties& properties = {});
        ~Cubemap();

        void Bind(int unit = 0) const;
        void Unbind() const;
    private:
        uint32_t m_id = 0;

        CubemapProperties m_properties;
};