#pragma once

#include <glm/glm.hpp>

#include "chunk.h"

#include <vector>
#include <memory>

class ChunkGrid
{
public:
    std::vector<std::shared_ptr<Chunk>> Grid;

    // constructor
    ChunkGrid(int gridSize);

    // getter
    int GetGridSize() { return gridSize; }

    Chunk* GetChunk(int x, int z);
    Chunk* GetChunk(float worldX, float worldZ);
    glm::ivec2 getChunkCoordsFromWorldPos(float worldX, float worldZ);

    void SetupChunkTerrain(int x, int z, Terrain &terrain);
    void Draw(Shader &shader);
private:
    int gridSize; // N X N grid

    int getIndexFromChunkCoords(int chunkX, int chunkZ);

};