#include "pbr_pass.h"
#include "resource_manager.h"

#pragma region lifecycle
PBRPass::PBRPass() : m_quadVBO(0), m_quadVAO(0)
{
    // generate brdfLut map
    m_brdfLut = IBLGenerator::GenerateBRDFLUT();
    // load pbr shader
    ResourceManager::LoadShader("shaders/PBR/pbr.vert", "shaders/PBR/pbr.frag", nullptr, nullptr, nullptr, "PBR");
    // configure PBR shader
    ResourceManager::GetShader("PBR").Use();
    ResourceManager::GetShader("PBR").SetInteger("gDepth",          0);
    ResourceManager::GetShader("PBR").SetInteger("gNormal",         1);
    ResourceManager::GetShader("PBR").SetInteger("gAlbedoAO",       2);
    ResourceManager::GetShader("PBR").SetInteger("gRoughMetal",     3);
    ResourceManager::GetShader("PBR").SetInteger("irradianceMap",   4);
    ResourceManager::GetShader("PBR").SetInteger("prefilterMap",    5);
    ResourceManager::GetShader("PBR").SetInteger("brdfLUT",         6);
    ResourceManager::GetShader("PBR").SetInteger("shadowMap",       7);
    ResourceManager::GetShader("PBR").SetInteger("shadowMapOffset", 8);
}

PBRPass::~PBRPass()
{
    // delete texture
    glDeleteTextures(1, &m_brdfLut);
    // delete buffers
    if(m_quadVBO)glDeleteBuffers(1, &m_quadVBO);
    if(m_quadVAO)glDeleteVertexArrays(1, &m_quadVAO);
    m_quadVBO = m_quadVAO = 0;
}

#pragma endregion

#pragma region loop
void PBRPass::Configure()
{
    // bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PBRPass::Render(GBufferTextures gBufferTextures, ShadowMapTexture shadowMap, IBLData iblTextures)
{
    // set state
    Configure();

    ResourceManager::GetShader("PBR").Use();
    // update sampler
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures.depth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures.normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures.albedoAO);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures.roughMetal);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, iblTextures.irradianceMap);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, iblTextures.prefilterMap);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_brdfLut);
    // update shadow map
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap.shadowMapTexture);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_3D, shadowMap.shadowMapOffsetTexture.textureID);
    ResourceManager::GetShader("PBR").SetInteger("shadowMapOffsetTextureSize", shadowMap.shadowMapOffsetTexture.windowSize);
    ResourceManager::GetShader("PBR").SetInteger("shadowMapOffsetFilterSize", shadowMap.shadowMapOffsetTexture.filterSize);
    ResourceManager::GetShader("PBR").SetFloat("shadowMapOffsetRandomRadius", shadowMap.shadowMapOffsetTexture.radius);
    // draw
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#pragma endregion

#pragma region render draw function
void PBRPass::renderQuad()
{
    if (m_quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);
        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

#pragma endregion