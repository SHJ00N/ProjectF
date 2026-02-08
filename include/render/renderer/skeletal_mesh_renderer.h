#pragma once

#include "shader.h"

class Model;
class Animator;
class Transform;

class SkeletalMeshRenderer
{
public:
    SkeletalMeshRenderer();

    void Draw(Shader &shader, Transform &transform, Model &model, Animator &animator);
    void DrawShadow(Transform &transform, Model &model, Animator &animator);
};