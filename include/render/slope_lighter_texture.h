#pragma once

#include <glm/glm.hpp>
#include <vector>

struct HeightMapData;

const float SOFTNESS = 2.0f;

class SlopeLighterTexture
{
public:
    // init value and generate slope lighter texture
    SlopeLighterTexture(HeightMapData &heightMapData, float terrainHeightScale);
    ~SlopeLighterTexture();

    // get lighter map
    unsigned int GetLighterMap() const { return m_lighterMap; }
private:
    // height map
    HeightMapData &m_heightMapData;
    int m_terrainSize;
    float m_terrainHeightScale;
    // lighter map
    unsigned int m_lighterMap;
    float m_softness = 0.0f;

    // calc lighting factor
    void genTextureData(const std::vector<unsigned short> &heightMap, std::vector<float> &data);
    float getLighting(int x, int z, const std::vector<unsigned short> &heightMap);
    float getHeight(int x, int z, const std::vector<unsigned short> &heightMap);
    unsigned int createTexture(std::vector<float> &data) const;
};