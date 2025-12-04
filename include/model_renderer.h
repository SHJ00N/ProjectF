#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"
#include "shader.h"

class ModelRenderer
{
public:
    void DrawModel();

};

#endif