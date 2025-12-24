#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <iostream>

#include "stb_image.h"
#include "shader.h"

const unsigned int NUM_PATCH_PTS = 4;
const unsigned int NUM_VERTICES_ATTRIBUTE = 5; // x, y, z, u, v

class Terrain
{
public:
    std::vector<float> Vertices;
    unsigned int DiffuseMap, HeightMap;
    float HeightScale, WorldScale;

    // constructor
    Terrain();
    Terrain(const char *diffuseMapFile, const char *heightMapFile, float heightScale = 64.0f, float worldScale = 1.0f, unsigned int rez = 20);
    void Clear();

    float GetLocalHeight(float localX, float localZ);
    int GetWorldWidth() { return heightMapWidth * WorldScale; }
    int GetWorldHeight() { return heightMapHeight * WorldScale; }

    void Draw(Shader &shader);

private:
    unsigned int VAO, VBO, Rez;

    std::vector<unsigned char> heightData;
    int heightMapWidth, heightMapHeight;

    void setUpTerrain();
    unsigned int textureFromFile(const char *path, bool gamma = false, bool isHeight = false);
    float bilinearInterpolate(float x, float z);
    float getHeightFromHeightMap(int x, int z);
};