#pragma once

#include <glad/glad.h>

#include "ibl_generator.h"
#include "render/pass/cascaded_shadow_pass.h"

struct SSAOTexture;
struct GBufferTextures;
struct SSAOTexture;

class PBRPass
{
public:
    PBRPass(unsigned int width, unsigned int height, unsigned int depthTexture);
    ~PBRPass();

    void Render(GBufferTextures& gBufferTextures, ShadowMapTexture shadowMap, SSAOTexture& ssaoTexture, IBLData iblTextures);
    unsigned int GetColorTexture() const;
    unsigned int GetBrightTexture() const;
    unsigned int GetFrameBuffer() const;

private:
    unsigned int m_brdfLut;
    unsigned int m_quadVAO, m_quadVBO;
    unsigned int m_fbo;
    unsigned int m_colorBuffers[2];

    unsigned int m_width, m_height;

    void renderQuad();
};