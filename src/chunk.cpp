#include "chunk.h"

Chunk::Chunk(int chunkX, int chunkZ, Terrain &terrain) : chunkX(chunkX), chunkZ(chunkZ), ChunkTerrain(&terrain)
{
    // set chunk position
    ChunkTransform.position = glm::vec3(chunkX * ChunkTerrain->GetWorldWidth(), 0.0f, -chunkZ * ChunkTerrain->GetWorldHeight());
}

float Chunk::GetWorldHeight(float worldX, float worldZ)
{
    glm::vec4 localPos = glm::inverse(ChunkTransform.GetModelMatrix()) * glm::vec4(worldX, 0.0f, worldZ, 1.0f);

    return ChunkTerrain->GetLocalHeight(localPos.x, localPos.z);
}

void Chunk::Draw(Shader &shader)
{
    // set model matrix
    shader.Use();
    shader.SetMatrix4("model", ChunkTransform.GetModelMatrix());

    // draw terrain in this chunk
    ChunkTerrain->Draw(shader);
}