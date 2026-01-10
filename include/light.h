#pragma once

#include <glm/glm.hpp>

enum LightType
{
    Direction, Point, Spot
};

// base light class
class Light
{
public:
    // lighting variables
    glm::vec3 Ambient, Diffuse, Specular;
    //constructor(s)
    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Ambient(ambient), Diffuse(diffuse), Specular(specular) { };

    // child's light type getter
    virtual LightType GetLightType() = 0;
};