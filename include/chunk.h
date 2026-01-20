#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "transform.h"
#include "shader.h"
#include "terrain.h"
#include "renderable.h"
#include "terrain_renderer.h"

#include <memory>

class Chunk : public Renderable
{
public:
    Transform ChunkTransform;
    Terrain *ChunkTerrain;
    
    // constructor
    Chunk(int chunkX, int chunkZ, Terrain &terrain, Shader &shader);

    // getter
    float GetWorldHeight(float worldX, float worldZ);
    glm::vec3 GetWorldNormal(float worldX, float worldZ);
    
    // rendering
    void Render() override;
    void RenderShadow() override;
private:
    // rendering
    Shader *m_shader;
    TerrainRenderer m_terrainRenderer;
    // chunk grid coordinates
    int chunkX, chunkZ;
};