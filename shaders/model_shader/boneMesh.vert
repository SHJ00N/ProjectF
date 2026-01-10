#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

out vec2 TexCoords;
out mat3 TBN;

layout (std140, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
};

uniform mat4 model;
uniform mat4 normalMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    vec3 totalTangent = vec3(0.0f);
    vec3 totalBitangent = vec3(0.0f);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if(boneIds[i] == -1) continue;
        if(boneIds[i] >= MAX_BONES)
        {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }

        mat4 bone = finalBonesMatrices[boneIds[i]];

        totalPosition += (bone * vec4(aPos, 1.0f)) * weights[i];
        totalNormal += (mat3(bone) * aNormal) * weights[i];
        totalTangent += (mat3(bone) * aTangent) * weights[i];
        totalBitangent += (mat3(bone) * aBitangent) * weights[i];
    }

    mat3 normalMatrix3 = mat3(normalMatrix);
    vec3 N = normalize(normalMatrix3 * totalNormal);
    vec3 T = normalize(normalMatrix3 * totalTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    TBN = mat3(T, B, N);
    TexCoords = aTexCoords;

    mat4 viewModel = view * model;
    gl_Position = projection * viewModel * totalPosition;
}