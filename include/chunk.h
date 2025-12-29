#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "transform.h"
#include "shader.h"
#include "terrain.h"

#include <memory>

class Chunk
{
public:
    Transform ChunkTransform;
    Terrain *ChunkTerrain;
    
    // constructor
    Chunk(int chunkX, int chunkZ, Terrain &terrain);

    float GetWorldHeight(float worldX, float worldZ);
    glm::vec3 GetWorldNormal(float worldX, float worldZ);
    void Draw(Shader &shader);
private:
    // chunk grid coordinates
    int chunkX, chunkZ;
};