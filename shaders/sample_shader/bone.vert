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
out vec3 FragPos;
out vec3 ViewPos;

layout (std140, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
    float cameraFov;
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
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if(boneIds[i] == -1) continue;
        if(boneIds[i] >= MAX_BONES)
        {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
        totalNormal += localNormal * weights[i];
    }

    mat3 normalMatrix3 = mat3(normalMatrix);
    vec3 T = normalize(normalMatrix3 * aTangent);
    vec3 B = normalize(normalMatrix3 * aBitangent);
    vec3 N = normalize(normalMatrix3 * totalNormal);
    
    TBN = mat3(T, B, N);
    TexCoords = aTexCoords;
    ViewPos = cameraPos;
    FragPos = vec3(model * totalPosition);

    mat4 viewModel = view * model;
    gl_Position = projection * viewModel * totalPosition;
}