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
    void UpdateChunkColliderList(struct BoxCollider &collider);
    // getter
    std::vector<Chunk*> GetChunkList() const; // get current active chunk list
    std::vector<Chunk*> GetPhysicsChunkList() const;
    Chunk* GetChunk(const glm::vec3 &worldPos) const;
    Chunk* GetChunk(const ChunkCoord &chunkCoord) const;
    // transform coord world to chunk space
    ChunkCoord WorldToChunk(const glm::vec3 &pos) const;

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
};