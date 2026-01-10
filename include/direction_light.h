#pragma once

#include "light.h"

class DirLight : public Light
{
public:
    glm::vec3 Direction;

    // constructor(s)
    DirLight(LightType lightType, glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3 ambient = glm::vec3(0.1f), glm::vec3 diffuse = glm::vec3(0.5f), glm::vec3 specular = glm::vec3(1.0f));

    // getter
    LightType GetLightType() override;

private:
    LightType m_lightType;
};