#pragma once

#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>

#include "world/chunk.h"
#include "world/world_structures.h"

class ChunkManager
{
public:
    // constructor
    ChunkManager(WorldChunkInfo &worldChunkInfo, HeightMapData &heightMapdata, float heightScale);
    // update active chunks
    void Update(const glm::vec3 &cameraPos);
    // get current active chunk list
    std::vector<Chunk*> GetChunkList() const;

private:
    WorldChunkInfo &m_worldChunkInfo;
    HeightMapData &m_heightMapData;
    float m_heightScale;
    ChunkCoord m_lastChunkPos;  // last camera position in chunk space
    // current generated chunks
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> m_chunks;
    // chunk lifecycle helper
    void createChunk(const ChunkCoord &chunkCoord);
    void destroyChunk(const ChunkCoord &chunkCoord);
    // transform coord world to chunk space
    ChunkCoord worldToChunk(int worldX, int worldZ) const;
};