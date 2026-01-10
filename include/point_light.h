#pragma once

#include "light.h"

class PointLight : public Light
{
public:
    glm::vec3 Position;

    // constructor(s)
    PointLight(LightType lightType, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), float intensity = 10.0f) 
    : m_lightType(lightType), Position(position), Light(color, intensity) { }

    // getter
    LightType GetLightType() override { return m_lightType; };

private:
    LightType m_lightType;
};