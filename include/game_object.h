#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "model.h"
#include "animator.h"

class GameObject
{
public:
    // object state
    glm::vec2 Velocity;
    glm::vec3 Position, Size, RotationAxis;
    float RotationAngle;
    bool Destroyed;
    // render state
    Model* Model3D;
    Animator Animator3D;
    // constructor(s)
    GameObject();
    GameObject(Model &model, glm::vec3 position, glm::vec3 size, glm::vec3 RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f), float rotationAngle = 0.0f, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
};