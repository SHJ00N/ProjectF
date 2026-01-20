#pragma once

#include "shader.h"
#include "transform.h"
#include "model.h"
#include "animator.h"

class SkeletalMeshRenderer
{
public:
    SkeletalMeshRenderer();

    void Draw(Shader &shader, Transform &transform, Model &model, Animator &animator);
    void DrawShadow(Transform &transform, Model &model, Animator &animator);
};