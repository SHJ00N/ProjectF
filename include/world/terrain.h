#pragma once

#include <glm/glm.hpp>

struct HeightMapData;
struct WorldScaleInfo;

class Terrain
{
public:
    // constructor
    Terrain(WorldScaleInfo &worldScaleInfo, HeightMapData &heightMapData);
    // sampling functions
    float SampleHeight(float worldX, float worldZ) const;
    glm::vec3 SampleNormal(float worldX, float worldZ) const;

private:
    WorldScaleInfo &m_worldScaleInfo;
    HeightMapData &m_heightMapData;
    // sampling assistance
    float bilinearInterpolate(float localX, float localZ) const;
    float getHeightFromHeightData(int localX, int localZ) const;
    glm::vec2 worldToLocal(float worldX, float worldZ) const;
};