#include "world/chunk_manager.h"

ChunkManager::ChunkManager(WorldChunkInfo &worldChunkInfo, HeightMapData &heightMapdata, float heightScale) 
: m_worldChunkInfo(worldChunkInfo), m_heightMapData(heightMapdata), m_heightScale(heightScale)
{
    m_lastChunkPos.x = 0;
    m_lastChunkPos.z = 0;
}

std::vector<Chunk*> ChunkManager::GetChunkList() const
{
    std::vector<Chunk*> list;
    list.reserve(m_chunks.size());
    for(auto it = m_chunks.begin(); it != m_chunks.end(); ++it)
    {
        list.push_back(it->second.get());
    }
    return list;
}

void ChunkManager::Update(const glm::vec3 &cameraPos)
{
    // camera position in chunk space
    ChunkCoord center = worldToChunk(cameraPos.x, cameraPos.z);

    if(center == m_lastChunkPos) return;
    m_lastChunkPos = center;

    // chunk radius
    int viewDistance = m_worldChunkInfo.viewDistance;
    int minX = center.x - viewDistance;
    int minZ = center.z - viewDistance;
    int maxX = center.x + viewDistance;
    int maxZ = center.z + viewDistance;
    // create chunk
    for(int i = minZ; i <= maxZ; ++i)
    {
        for(int j = minX; j <= maxX; ++j)
        {
            ChunkCoord chunkCoord = {j, i};
            if(m_chunks.find(chunkCoord) == m_chunks.end())
            {
                createChunk(chunkCoord);
            }
        }
    }

    for(auto it = m_chunks.begin(); it != m_chunks.end();)
    {
        const ChunkCoord &coord = it->first;
        if(coord.x < minX || coord.x > maxX || coord.z < minZ || coord.z > maxZ)
        {
            it = m_chunks.erase(it);
        }
        else ++it;
    }
}

void ChunkManager::createChunk(const ChunkCoord &chunkCoord)
{
    // create chunk
    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(chunkCoord.x, chunkCoord.z, m_worldChunkInfo.chunkRez, m_worldChunkInfo.chunkWorldSize);
    chunk->BuildMesh();
    chunk->BuildBound(m_heightMapData, m_heightScale);
    m_chunks.emplace(chunkCoord, std::move(chunk));
}

void ChunkManager::destroyChunk(const ChunkCoord &chunkCoord)
{
    // destroy chunk
    m_chunks.erase(chunkCoord);
}

ChunkCoord ChunkManager::worldToChunk(int worldX, int worldZ) const
{
    // transform coord to chunk from world
    int chunkX = static_cast<int>(std::floor(worldX / m_worldChunkInfo.chunkWorldSize));
    int chunkZ = static_cast<int>(std::floor(worldZ / m_worldChunkInfo.chunkWorldSize));
    return {chunkX, chunkZ};
}