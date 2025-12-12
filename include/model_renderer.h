#pragma once

#include "shader.h"
#include "game_object.h"

class ModelRenderer
{
public:
    virtual void Draw(Shader &shader, GameObject &gameObject, float deltaTime) = 0;
};
