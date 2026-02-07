#pragma once

#include <glm/glm.hpp>
#include <vector>

struct GBufferTextures;

struct SSAOTexture
{
    unsigned int id;
};

const int KERNAL_SIZE = 64;
const float RADIUS = 0.5;
const float BIAS = 0.025;

class SSAOPass
{
public:
    // constructor(s)
    SSAOPass(unsigned int width, unsigned int height);
    ~SSAOPass();
    // functions
    void Render(GBufferTextures& gBufferTextures);
    // getter
    SSAOTexture& GetSSAOTexture() {return m_totalData; }

private:
    // buffers
    unsigned int m_ssaoFBO, m_ssaoBlurFBO, m_quadVAO, m_quadVBO;
    unsigned int m_ssaoTexture, m_ssaoBlurTexture;
    // size
    unsigned int m_width, m_height;
    // noise texture
    unsigned int m_noiseTexture;
    // for external access data
    SSAOTexture m_totalData;

    // private function
    void renderQuad();
    float randomFloat();
    float ourLerp(float a, float b, float f);
    std::vector<glm::vec3> generateSampleKernal();
    std::vector<glm::vec3> generateNoise();
};