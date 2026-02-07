#include "world/world.h"
#include "world/terrain.h"
#include "shader.h"
#include "stb_image.h"

#include <iostream>

World::World(const std::string &heightMapPath, float worldScale, float heightScale, int chunkRez, float chunkWorldSize, int viewDistance)
{
    // set world data and chunk data
    m_worldScaleInfo.worldScale = worldScale;
    m_worldScaleInfo.heightScale = heightScale;
    m_worldChunkInfo.chunkRez = chunkRez;
    m_worldChunkInfo.chunkWorldSize = chunkWorldSize;
    m_worldChunkInfo.viewDistance = viewDistance;
    
    // load height
    m_heightMap = loadHeightMapFromFile(heightMapPath);

    // allocate ptr
    m_terrain = std::make_unique<Terrain>(m_worldScaleInfo, m_heightMapData);
    m_chunkManager = std::make_unique<ChunkManager>(m_worldChunkInfo);
}

float World::GetWorldHeight(float worldX, float worldZ) const
{
    return m_terrain.get()->SampleHeight(worldX, worldZ);
}

glm::vec3 World::GetWorldNormal(float worldX, float worldZ) const
{
    return m_terrain.get()->SampleNormal(worldX, worldZ);
}

void World::Update(const glm::vec3 cameraPos)
{
    m_chunkManager.get()->Update(cameraPos);
}

unsigned int World::loadHeightMapFromFile(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned short *data = stbi_load_16(path.c_str(), &width, &height, &nrComponents, 1);
    if(data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_RED, GL_UNSIGNED_SHORT, data);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // set height data
        m_heightMapData.heightMapWidth = width;
        m_heightMapData.heightMapHeight = height;
        m_heightMapData.heights.assign(data, data + width * height);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load height map at path: " << path << std::endl;
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}