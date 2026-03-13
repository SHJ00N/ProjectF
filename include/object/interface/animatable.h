#pragma once

#include "animation/animator.h"

class Animation;

class Animatable
{
public:
    Animator Animator3D;
    virtual void UpdateAnimation(float dt)
    {
        Animator3D.UpdateAnimation(dt);
    };
    virtual void SetAnimation(Animation* animation, bool force = false)
    {
        if(animation == nullptr) return;
        Animator3D.PlayAnimation(animation, force);
    };
    virtual void AddAnimation(const std::string &name, Animation *animation)
    {
        Animator3D.registAnimation(name, animation);
    };
    virtual ~Animatable() = default;
};