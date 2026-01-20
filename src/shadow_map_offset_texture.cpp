#include "shadow_map_offset_texture.h"

#include <vector>
#include <cassert>
#include <cmath>
#include <random>

// Generate a small random jitter in the range [-0.5, 0.5]
float jitter()
{
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distrib(-0.5f, 0.5f);
    return distrib(generator);
}

// Generate the offset texture data for percentage-closer filtering
static void GenOffsetTextureData(int WindowSize, int FilterSize, std::vector<float>& Data)
{
    int BufferSize = WindowSize * WindowSize * FilterSize * FilterSize * 2;

    Data.resize(BufferSize);

    double pi = acos(-1.0);  // Pi value
    int Index = 0;
    for (int TexY = 0 ; TexY < WindowSize ; TexY++) {
        for (int TexX = 0 ; TexX < WindowSize ; TexX++) {
            for (int v = FilterSize - 1 ; v >= 0 ; v--) {
                for (int u = 0 ; u < FilterSize ; u++) {
                    float x = ((float)u + 0.5f + jitter()) / (float)FilterSize;
                    float y = ((float)v + 0.5f + jitter()) / (float)FilterSize;

                    assert(Index + 1 < Data.size());
                    Data[Index]     = sqrtf(y) * cosf(2 * (float)pi * x);
                    Data[Index + 1] = sqrtf(y) * sinf(2 * (float)pi * x);

                    Index += 2;
                }
            }
        }
    }
}


ShadowMapOffsetTexture::ShadowMapOffsetTexture(int windowSize, int filterSize, float radius) : m_windowSize(windowSize), m_filterSize(filterSize), m_radius(radius)
{
    std::vector<float> Data;

    GenOffsetTextureData(m_windowSize, m_filterSize, Data);
    createTexture(Data);
}

ShadowMapOffsetTexture::~ShadowMapOffsetTexture()
{
    glDeleteTextures(1, &m_texture);
}


void ShadowMapOffsetTexture::createTexture(std::vector<float> &data)
{
    int NumFilterSamples = m_filterSize * m_filterSize;

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_3D, m_texture);

    glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, NumFilterSamples / 2, m_windowSize, m_windowSize);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, NumFilterSamples / 2, m_windowSize, m_windowSize, GL_RGBA, GL_FLOAT, &data[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_3D, 0);
}