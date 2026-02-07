#pragma once

#include <glad/glad.h>

class Scene;

struct GBufferTextures
{
    unsigned int depth;
    unsigned int normal;
    unsigned int albedoAO;
    unsigned int roughMetal;
    unsigned int typeSlope;
};

class GeometryPass
{
public:
    // constructor(s)
    GeometryPass(unsigned int width, unsigned int height);
    ~GeometryPass();

    // loop
    void Configure();
    void Render(Scene *scene);

    // getter
    GBufferTextures& GetTextures() { return m_totalData; }
    unsigned int GetGBuffer() { return m_gBuffer; }
private:
    unsigned int m_gBuffer;
    unsigned int m_gDepth, m_gNormal, m_gAlbedoAO, m_gRoughMetal, m_gTypeSlope; // ARM : ambient occlusion, roughness, metallic
    GBufferTextures m_totalData;
};