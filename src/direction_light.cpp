#include "direction_light.h"

DirLight::DirLight(LightType lightType, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specualr) 
: m_lightType(lightType), Direction(direction), Light(ambient, diffuse, specualr)
{
}

LightType DirLight::GetLightType()
{
    return m_lightType;
}