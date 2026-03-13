#pragma once

#include "object/collider/box_collider.h"

class Collider;

class Collidable
{
public:
    BoxCollider collider;
    bool CollidableDestroyed = false;
    virtual void OnCollisionEnter(Collider *other) {};
    virtual void OnCollisionStay(Collider *other) {};
    virtual void OnCollisionExit(Collider *other) {};
};