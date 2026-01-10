#include "geometry_pass.h"

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
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
    // set multy rendering targets
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // check complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GeometryPass::~GeometryPass()
{
    // delete textures
    glDeleteTextures(1, &m_gDepth);
    glDeleteTextures(1, &m_gNormal);
    glDeleteTextures(1, &m_gAlbedoAO);
    glDeleteTextures(1, &m_gRoughMetal);
    // delete framebuffer
    glDeleteFramebuffers(1, &m_gBuffer);
}

#pragma endregion

#pragma region loop
void GeometryPass::Begin()
{
    // bind gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    // set opengl state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GeometryPass::End()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#pragma endregion
