#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 6) in vec4 aColor;

out vec3 WorldPos;
out vec2 TexCoord;
out mat3 TBN;

out vec4 FragPosLightSpace;
out vec4 VertexColor;

uniform bool uInstanced;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpaceMatrix;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    WorldPos = worldPos.xyz;
    
    vec3 T = normalize(mat3(uModel) * aTangent);

    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vec3 N = normalize(normalMatrix * aNormal);

    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    TexCoord = aTexCoord;
    VertexColor = aColor;

    FragPosLightSpace = uLightSpaceMatrix * worldPos;

    gl_Position = uProjection * uView * worldPos;
}