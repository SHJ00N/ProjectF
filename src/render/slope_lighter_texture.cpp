#include "render/slope_lighter_texture.h"
#include "world/world_structures.h"
#include <glad/glad.h>
#include <iostream>
#include <cmath>

SlopeLighterTexture::~SlopeLighterTexture()
{
    glDeleteTextures(1, &m_lighterMap);
}

SlopeLighterTexture::SlopeLighterTexture(HeightMapData &heightMapData, float terrainHeightScale) : m_heightMapData(heightMapData)
{
    m_terrainSize = heightMapData.heightMapWidth;
    m_terrainHeightScale = terrainHeightScale;
    m_softness = SOFTNESS;

    std::vector<float> data;
    genTextureData(heightMapData.heights, data);
    m_lighterMap = createTexture(data);
}

void SlopeLighterTexture::genTextureData(const std::vector<unsigned short> &heightMap, std::vector<float> &data)
{
    int bufferSize = m_terrainSize * m_terrainSize;
    data.resize(bufferSize);
    // generate data
    for(int z = 0; z < m_terrainSize; ++z)
    {
        for(int x = 0; x < m_terrainSize; ++x)
        {
            float factor = getLighting(x, z, heightMap);
            data[z * m_terrainSize + x] = factor;
        }
    }
}

float SlopeLighterTexture::getHeight(int x, int z, const std::vector<unsigned short> &heightMap)
{
    x = glm::clamp(x, 0, m_terrainSize - 1);
    z = glm::clamp(z, 0, m_terrainSize - 1);
    return heightMap[z * m_terrainSize + x] / 65535.0f * m_terrainHeightScale;
}

float SlopeLighterTexture::getLighting(int x, int z, const std::vector<unsigned short> &heightMap)
{
    // sample heights
    float hL = getHeight(x - 1, z, heightMap);
    float hR = getHeight(x + 1, z, heightMap);
    float hD = getHeight(x, z - 1, heightMap);
    float hU = getHeight(x, z + 1, heightMap);

    float dx = (hR - hL) * m_softness;
    float dz = (hU - hD) * m_softness;

    // calculate normal
    glm::vec3 normal = glm::normalize(glm::vec3(-dx, 2.0f, -dz));

    // slope (0 = 평지, 1 = 급경사)
    float slope = 1.0f - normal.y;

    float min_brightness = 0.4f;

    // lighting factor
    float lighting = 1.0f - slope * (1.0f - min_brightness);

    return glm::clamp(lighting, min_brightness, 1.0f);
}

unsigned int SlopeLighterTexture::createTexture(std::vector<float> &data) const
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, m_terrainSize, m_terrainSize, 0, GL_RED, GL_FLOAT, &data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}