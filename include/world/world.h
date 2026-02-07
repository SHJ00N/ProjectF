#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "world/world_structures.h"
#include "world/chunk_manager.h"
#include "world/terrain.h"

class World
{
public:
    // constructor
    World(const std::string &heightMapPath, float worldScale, float heightScale, int chunkRez, float chunkWorldSize, int viewDistance);
    // sample height and normal in world space
    float GetWorldHeight(float worldX, float worldZ) const;
    glm::vec3 GetWorldNormal(float worldX, float worldZ) const;
    // getter
    ChunkManager* GetChunkManager() const { return m_chunkManager.get(); }
    unsigned int GetHeightMap() const { return m_heightMap; }
    float GetHeightScale() const { return m_worldScaleInfo.heightScale; }
    float GetWorldScale() const { return m_worldScaleInfo.worldScale; }
    int GetHeightMapWidth() const { return m_heightMapData.heightMapWidth; }
    int GetHeightMapHeight() const { return m_heightMapData.heightMapHeight; }
    const std::vector<unsigned short>& GetHeightMapVector() const { return m_heightMapData.heights; }
    HeightMapData& GetHeightMapData() { return m_heightMapData; }

    void Update(const glm::vec3 cameraPos);

private:
    // world data
    WorldScaleInfo m_worldScaleInfo;
    WorldChunkInfo m_worldChunkInfo;
    // height data
    unsigned int m_heightMap;
    HeightMapData m_heightMapData;
    
    std::unique_ptr<Terrain> m_terrain;
    std::unique_ptr<ChunkManager> m_chunkManager;

    // texture loader
    unsigned int loadHeightMapFromFile(const std::string &path);
};