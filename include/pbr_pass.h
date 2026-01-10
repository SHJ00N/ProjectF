#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "geometry_pass.h"
#include "ibl_generator.h"

class PBRPass
{
public:
    PBRPass();
    ~PBRPass();

    // loop
    void Begin();
    void Render(GBufferTextures gBufferTextures, IBLData iblTextures);
    void End();

private:
    unsigned int m_brdfLut;
    unsigned int m_quadVAO, m_quadVBO;

    void renderQuad();
};