#pragma once

#include "shader.h"
#include "transform.h"
#include "model.h"

// base renderer class
class StaticMeshRenderer
{
public:
    StaticMeshRenderer();
    
    void Draw(Shader &shader, Transform &transform, Model &model);
    void DrawShadow(Transform &transform, Model &model);
};
