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
    PBRPass();
    ~PBRPass();

    // loop
    void Configure();
    void Render(GBufferTextures& gBufferTextures, ShadowMapTexture shadowMap, SSAOTexture& ssaoTexture, IBLData iblTextures);

private:
    unsigned int m_brdfLut;
    unsigned int m_quadVAO, m_quadVBO;

    void renderQuad();
};