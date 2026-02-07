#include "render/pass/geometry_pass.h"
#include "object/interface/renderable.h"
#include "render/renderer/terrain_renderer.h"
#include "scene/scene.h"

#include <iostream>

#pragma region lifecycle
GeometryPass::GeometryPass(unsigned int width, unsigned int height)
{
    // frame buffer
    glGenFramebuffers(1, &m_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    // depth
    glGenTextures(1, &m_gDepth);
    glBindTexture(GL_TEXTURE_2D, m_gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,  m_gDepth, 0);
    // normal
    glGenTextures(1, &m_gNormal);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  m_gNormal, 0);
    // albedo + specular
    glGenTextures(1, &m_gAlbedoAO);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoAO);
    // use a floating point internal format to match GL_FLOAT type and avoid precision/format mismatches
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  m_gAlbedoAO, 0);
    // ambient occlusion + roughness + metallic
    glGenTextures(1, &m_gRoughMetal);
    glBindTexture(GL_TEXTURE_2D, m_gRoughMetal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,  m_gRoughMetal, 0);
    // mesh type + slope lighting factor
    glGenTextures(1, &m_gTypeSlope);
    glBindTexture(GL_TEXTURE_2D, m_gTypeSlope);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D,  m_gTypeSlope, 0);
    // set multy rendering targets
    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);
    // check complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // set total gBuffer data
    m_totalData.depth = m_gDepth;
    m_totalData.normal = m_gNormal;
    m_totalData.albedoAO = m_gAlbedoAO;
    m_totalData.roughMetal = m_gRoughMetal;
    m_totalData.typeSlope = m_gTypeSlope;
}

GeometryPass::~GeometryPass()
{
    // delete textures
    glDeleteTextures(1, &m_gDepth);
    glDeleteTextures(1, &m_gNormal);
    glDeleteTextures(1, &m_gAlbedoAO);
    glDeleteTextures(1, &m_gRoughMetal);
    glDeleteTextures(1, &m_gTypeSlope);
    // delete framebuffer
    glDeleteFramebuffers(1, &m_gBuffer);
}

#pragma endregion

#pragma region loop
void GeometryPass::Configure()
{
    // bind gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    // set opengl state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GeometryPass::Render(Scene *scene)
{
    // start geometry pass
    Configure();
    // render world
    if(scene->GetTerrainRenderer()) scene->GetTerrainRenderer()->Render(); 
    // render all scene meshes
    for(const auto &renderableObj : scene->GetRenderables())
    {
        renderableObj->Render();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#pragma endregion
