#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "model.h"
#include "animator.h"
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