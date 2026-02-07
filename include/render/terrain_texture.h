#pragma once

#include <string>

class TerrainTexture
{
public:
    unsigned int AlbedoMap = 0, NormalMap = 0, RoughnessMap = 0;
    
    // constructor
    TerrainTexture();
    TerrainTexture(const std::string &albedoPath, const std::string &normalPath, const std::string &roughenssPath, bool gamma = false);
    // delete resource
    void Clear();

private:
    unsigned int loadTextureFromFile(const std::string &path);
    unsigned int loadSRGBTextureFromFile(const std::string &path);
};