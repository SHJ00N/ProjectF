#include "chunk.h"

Chunk::Chunk(int chunkX, int chunkZ, Terrain &terrain, Shader &shader) : chunkX(chunkX), chunkZ(chunkZ), ChunkTerrain(&terrain), m_shader(&shader)
{
    // set chunk position
    ChunkTransform.position = glm::vec3(chunkX * ChunkTerrain->GetWorldWidth(), 0.0f, -chunkZ * ChunkTerrain->GetWorldHeight());
}

float Chunk::GetWorldHeight(float worldX, float worldZ)
{
    glm::vec4 terrainWorldPos = glm::inverse(ChunkTransform.GetModelMatrix()) * glm::vec4(worldX, 0.0f, worldZ, 1.0f);

    return ChunkTerrain->GetLocalHeight(terrainWorldPos.x, terrainWorldPos.z);
}

glm::vec3 Chunk::GetWorldNormal(float worldX, float worldZ)
{
    glm::vec4 terrainWorldPos = glm::inverse(ChunkTransform.GetModelMatrix()) * glm::vec4(worldX, 0.0f, worldZ, 1.0f);

    return ChunkTerrain->GetNormal(terrainWorldPos.x, terrainWorldPos.z);
}

void Chunk::Render()
{
    m_terrainRenderer.Draw(*m_shader, ChunkTransform, *ChunkTerrain);
}

void Chunk::RenderShadow()
{
    m_terrainRenderer.DrawShadow(ChunkTransform, *ChunkTerrain);
}