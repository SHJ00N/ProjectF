#pragma once

#include <glm/glm.hpp>

struct CameraUBO
{
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 cameraPos;
    float padding0 = 0.0f;     // glsl's uniform block pads vec3 to the size of vec4
};

struct LightUBO
{
    // common lighting variables
    glm::vec3 ambient;
    float padding0 = 0.0f;
    glm::vec3 diffuse;
    float padding1 = 0.0f;
    glm::vec3 specular;
    float padding2 = 0.0f;
};

struct DirLightUBO
{
    LightUBO light;
    glm::vec3 direction;
    float padding0 = 0.0f;
};

struct DirLightBlockUBO
{
    DirLightUBO dirLight;
    int hasDirLight = 0;
    glm::vec3 padding0;
};