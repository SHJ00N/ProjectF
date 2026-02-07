#pragma once

#include <glm/glm.hpp>
#include "transform.h"

class GameObject
{
public:
    // object state
    glm::vec2 Velocity;
    Transform ObjectTransform;
    bool Destroyed;
    
    virtual void Update(float dt) = 0;
    virtual ~GameObject() = default;
};