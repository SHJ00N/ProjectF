#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct GBufferTextures
{
    unsigned int depth;
    unsigned int normal;
    unsigned int albedoAO;
    unsigned int roughMetal;
};

class GeometryPass
{
public:
    // constructor(s)
    GeometryPass(unsigned int width, unsigned int height);
    ~GeometryPass();

    // loop
    void Begin();
    void End();

    // getter
    GBufferTextures GetTextaures() { return {m_gDepth, m_gNormal, m_gAlbedoAO, m_gRoughMetal}; }
    unsigned int GetGBuffer() { return m_gBuffer; }
private:
    unsigned int m_gBuffer;
    unsigned int m_gDepth, m_gNormal, m_gAlbedoAO, m_gRoughMetal; // ARM : ambient occlusion, roughness, metallic
};