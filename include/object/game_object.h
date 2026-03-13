#pragma once

#include <glm/glm.hpp>
#include "object/entity.h"

struct ObjectUpdateContext
{
    float deltaTime;
    class World *world;
    class Camera *camera;
};

enum class Layer
{
    None,
    Player,
    Enemy,
    Weapon
};

class GameObject : public Entity
{
public:
    Layer ObjectLayer = Layer::None;
    // object state
    glm::vec2 Velocity;
    bool ObjectDestroyed = false;
    
    virtual void Update(const ObjectUpdateContext &context) = 0;
    virtual ~GameObject() = default;
};