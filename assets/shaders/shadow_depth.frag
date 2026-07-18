#version 410 core

in vec2 TexCoord;

uniform sampler2D uAlbedoMap;
uniform bool uAlphaCutout;

void main() {
    if (uAlphaCutout && texture(uAlbedoMap, TexCoord).a < 0.5) discard;
}