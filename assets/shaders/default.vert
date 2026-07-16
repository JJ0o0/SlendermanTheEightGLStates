#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aJoints;
layout (location = 5) in vec4 aWeights;

out vec3 WorldPos;
out vec2 TexCoord;
out mat3 TBN;

out vec4 FragPosLightSpace;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpaceMatrix;

#define MAX_JOINTS 100
uniform mat4 uBoneMatrices[MAX_JOINTS];

void main() {
    float totalWeight = aWeights.x + aWeights.y + aWeights.z + aWeights.w;

    mat4 skinMatrix;
    if (totalWeight > 0.0001) {
        skinMatrix =
            aWeights.x * uBoneMatrices[aJoints.x] +
            aWeights.y * uBoneMatrices[aJoints.y] +
            aWeights.z * uBoneMatrices[aJoints.z] +
            aWeights.w * uBoneMatrices[aJoints.w];
    } else {
        skinMatrix = mat4(1.0);
    }
    
    vec4 skinnedPos = skinMatrix * vec4(aPos, 1.0);
    vec3 skinnedNormal = mat3(skinMatrix) * aNormal;
    vec3 skinnedTangent = mat3(skinMatrix) * aTangent;

    vec4 worldPos = uModel * skinnedPos;
    WorldPos = worldPos.xyz;
    
    vec3 T = normalize(mat3(uModel) * skinnedTangent);

    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vec3 N = normalize(normalMatrix * skinnedNormal);

    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    TexCoord = aTexCoord;

    FragPosLightSpace = uLightSpaceMatrix * worldPos;

    gl_Position = uProjection * uView * worldPos;
}