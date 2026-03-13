#pragma once

#include <glm/glm.hpp>

class DebugPass
{
public:
    DebugPass();
    ~DebugPass();

    void RenderAABB(class Scene *scene, unsigned int depthTexture);

private:
    unsigned int m_aabbVAO, m_aabbVBO, m_aabbEBO;
    void configureAABBBuffers();

    static void buildAABBVertices(const glm::vec3 &center, const glm::vec3 &extents, glm::vec3 out[8]);
};