#pragma once

#include <glad/glad.h>
#include <vector>

struct ShadowMapOffsetTextureData
{
    unsigned int textureID;
    int windowSize;
    int filterSize;
    float radius;
};

class ShadowMapOffsetTexture
{
public:
    ShadowMapOffsetTexture(int windowSize, int filterSize, float radius = 1.0f);
    ~ShadowMapOffsetTexture();

    ShadowMapOffsetTextureData GetData() const { return {m_texture, m_windowSize, m_filterSize, m_radius}; }
private:
    unsigned int m_texture;
    int m_windowSize, m_filterSize;
    float m_radius;

    void createTexture(std::vector<float> &data);
};