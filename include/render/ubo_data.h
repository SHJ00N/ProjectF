#pragma once

#include <glm/glm.hpp>

struct CameraUBO
{
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 cameraPos;
    float cameraFov;
};

struct DirLightUBO
{
    glm::vec4 colorIntensity; // light color and intensity
    glm::vec3 direction;
    float padding0 = 0.0f;
};

struct DirLightBlockUBO
{
    DirLightUBO light;
    int hasDirLight = 0;
    glm::vec3 padding0;
};

struct PointLightUBO
{
    glm::vec4 colorIntensity; // light color and intensity
    glm::vec3 position;
    float padding0 = 0.0f;
};