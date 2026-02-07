#pragma once

#include <vector>

struct HeightMapData
{
    int heightMapWidth, heightMapHeight;
    std::vector<unsigned short> heights;
};

struct WorldScaleInfo
{
    float worldScale;
    float heightScale;
};

struct WorldChunkInfo
{
    int viewDistance;
    int chunkRez;
    float chunkWorldSize;
};

// chunk struct
struct ChunkCoord
{
    int x, z;
    bool operator==(const ChunkCoord &other) const
    {
        return x == other.x && z == other.z;
    }
};
// chunkCoord hash
struct ChunkCoordHash
{
    size_t operator()(const ChunkCoord& c) const
    {
        return (std::hash<int>()(c.x) << 1) ^ std::hash<int>()(c.z);
    }
};