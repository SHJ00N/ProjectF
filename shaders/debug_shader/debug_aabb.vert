#version 460 core
layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
    float cameraFov;
};

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
}