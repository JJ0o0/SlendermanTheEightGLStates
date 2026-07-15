#version 410 core
in vec3 TexCoord;
out vec4 FragColor;

uniform samplerCube uSkybox;

void main() {
    vec3 color = texture(uSkybox, TexCoord).rgb;
    color *= 0.20;

    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    color = mix(vec3(gray), color, 0.25);

    color *= vec3(0.85, 0.90, 1.0);

    FragColor = vec4(color, 1.0);
}