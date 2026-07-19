#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 4) in ivec4 aJoints;
layout (location = 5) in vec4 aWeights;
layout (location = 7) in mat4 iModel;

out vec2 TexCoord;

uniform bool uInstanced;
uniform mat4 uModel;
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

    mat4 model = uInstanced ? iModel : uModel;

    TexCoord = aTexCoord;
    gl_Position = uLightSpaceMatrix * model * skinMatrix * vec4(aPos, 1.0);
}