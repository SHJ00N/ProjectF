#pragma once

#include "render/pass/bloom/bloom_fbo.h"

class Shader;

class BloomPass
{
public:
    BloomPass(unsigned int width, unsigned int height);
    ~BloomPass();
    void Render(unsigned int srcTexture, float filterRadius);
    unsigned int GetBloomTexture();
    unsigned int GetBloomMip_i(int index);

private:
    BloomFBO m_fbo;
    glm::ivec2 m_srcViewportSize;
    glm::vec2 m_srcViewportSizeFloat;
    Shader *m_downSampleShader;
    Shader *m_upSampleShader;
    unsigned int m_quadVAO, m_quadVBO;
    bool m_KarisAverageOnDownsample = true;

    void renderDownSamples(unsigned int srcTexture);
    void renderUpSamples(float filterRadius);
    void renderQuad();
};