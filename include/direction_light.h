#pragma once

#include "light.h"

class DirLight : public Light
{
public:
    glm::vec3 Direction;

    // constructor(s)
    DirLight(LightType lightType, glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), float intensity = 10.0f) 
    : m_lightType(lightType), Direction(direction), Light(color, intensity) { }

    // getter
    LightType GetLightType() override { return m_lightType; };

private:
    LightType m_lightType;
};