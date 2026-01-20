#pragma once

#include "shader.h"
#include "transform.h"
#include "terrain.h"

class TerrainRenderer
{
public:
    TerrainRenderer();

    void Draw(Shader &shader, Transform &transform, Terrain &terrain);
    void DrawShadow(Transform &transform, Terrain &terrain);
};