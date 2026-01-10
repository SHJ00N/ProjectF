#pragma once

#include <glm/glm.hpp>

enum LightType
{
    Direction, Point
};

// base light class
class Light
{
public:
    // lighting variables
    glm::vec3 Color;
    float Intensity;
    //constructor(s)
    Light(glm::vec3 color, float intensity) : Color(color), Intensity(intensity) { };

    // child's light type getter
    virtual LightType GetLightType() = 0;
};