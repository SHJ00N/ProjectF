#pragma once

#include "animator.h"

class Animatable
{
public:
    virtual void UpdateAnimation(float dt) = 0;
    virtual void SetAnimation(Animation* animation) = 0;
    virtual ~Animatable() = default;
};