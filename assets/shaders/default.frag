#version 410 core

out vec4 FragColor;
in vec3 Normal;
in vec2 TexCoord;

void main() {
    FragColor = vec4(1.0, 0.8, 0.25, 1.0);
}