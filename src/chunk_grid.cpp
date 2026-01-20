#include "chunk_grid.h"

ChunkGrid::ChunkGrid(int gridSize) : gridSize(gridSize)
{
    Grid.resize(gridSize * gridSize);
}

void ChunkGrid::SetupChunkTerrain(int x, int z, Terrain &terrain, Shader &shader)
{
    int index = getIndexFromChunkCoords(x, z);
    if(index == -1) return;
    Grid[index] = std::make_shared<Chunk>(x, z, terrain, shader);
}

Chunk* ChunkGrid::GetChunk(int x, int z)
{
    int index = getIndexFromChunkCoords(x, z);
    if (index == -1)
        return nullptr;
    return Grid[index].get();
}

Chunk* ChunkGrid::GetChunk(float worldX, float worldZ)
{
    glm::ivec2 chunkCoords = getChunkCoordsFromWorldPos(worldX, worldZ);
    return GetChunk(chunkCoords.x, chunkCoords.y);
}

int ChunkGrid::getIndexFromChunkCoords(int chunkX, int chunkZ)
{
    if(chunkX < 0 || chunkX >= gridSize || chunkZ < 0 || chunkZ >= gridSize)
        return -1;
    return chunkZ * gridSize + chunkX;
}

glm::ivec2 ChunkGrid::getChunkCoordsFromWorldPos(float worldX, float worldZ)
{
    float chunkWidth = Grid[0]->ChunkTerrain->GetWorldWidth();
    float chunkHeight = Grid[0]->ChunkTerrain->GetWorldHeight();

    float halfWidth = chunkWidth * 0.5f;
    float halfHeight = chunkHeight * 0.5f;

    int chunkX = static_cast<int>(floor((worldX + halfWidth) / chunkWidth));
    int chunkZ = static_cast<int>(floor((-worldZ + halfHeight) / chunkHeight));
    return glm::ivec2(chunkX, chunkZ);
}