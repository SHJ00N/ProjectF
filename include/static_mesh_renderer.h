#pragma once

#include "model_renderer.h"
#include "shader.h"
#include "game_object.h"

// base renderer class
class StaticMeshRenderer : public ModelRenderer
{
public:
    void Draw(Shader &shader, GameObject &gameObject, float deltaTime) override;
};
