#pragma once

#include <glm/glm.hpp>
#include "object/entity.h"

class GameObject : public Entity
{
public:
    // object state
    glm::vec2 Velocity;
    bool Destroyed;
    
    virtual void Update(float dt) = 0;
    virtual ~GameObject() = default;
};