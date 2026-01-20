#pragma once

#include <glad/glad.h>

#include "geometry_pass.h"
#include "ibl_generator.h"
#include "cascaded_shadow_pass.h"

class PBRPass
{
public:
    PBRPass();
    ~PBRPass();

    // loop
    void Configure();
    void Render(GBufferTextures gBufferTextures, ShadowMapTexture shadowMap, IBLData iblTextures);

private:
    unsigned int m_brdfLut;
    unsigned int m_quadVAO, m_quadVBO;

    void renderQuad();
};