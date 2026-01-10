#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "stb_image.h"
#include "shader.h"

const unsigned int NUM_PATCH_PTS = 4;
const unsigned int NUM_VERTICES_ATTRIBUTE = 5; // x, y, z, u, v

class Terrain
{
public:
    std::vector<float> Vertices;
    unsigned int DiffuseMap, HeightMap, NormalMap, RoughnessMap;
    float HeightScale, WorldScale;

    // constructor
    Terrain();
    Terrain(const char *diffuseMapFile, const char *heightMapFile, const char *normalMapFile, const char *roughMapFile, float heightScale = 64.0f, float worldScale = 1.0f, unsigned int rez = 20);
    void Clear();

    float GetLocalHeight(float localX, float localZ);
    glm::vec3 GetNormal(float localX, float localZ);
    int GetWorldWidth() { return heightMapWidth * WorldScale; }
    int GetWorldHeight() { return heightMapHeight * WorldScale; }

    void Draw(Shader &shader);

private:
    unsigned int VAO, VBO, Rez;

    std::vector<unsigned char> heightData;
    std::vector<unsigned char> normalData;
    int heightMapWidth, heightMapHeight;

    void setUpTerrain();
    unsigned int textureFromFile(const char *path, std::string type, bool gamma = false);
    float bilinearInterpolate(float x, float z);
    float getHeightFromHeightMap(int x, int z);
    glm::vec2 transformLocalCoord(float x, float z);
};