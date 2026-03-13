#include "debug/debug_render_pass.h"
#include "resource_manager.h"
#include "scene/scene.h"
#include "object/interface/collidable.h"

#include <glad/glad.h>

DebugPass::DebugPass()
{
    ResourceManager::LoadShader("shaders/debug_shader/debug_aabb.vert", "shaders/debug_shader/debug_aabb.frag", nullptr, nullptr, nullptr, "DebugAABB");
    configureAABBBuffers();
}

DebugPass::~DebugPass()
{
    glDeleteBuffers(1, &m_aabbVBO);
    glDeleteBuffers(1, &m_aabbEBO);
    glDeleteVertexArrays(1, &m_aabbVAO);
}

void DebugPass::RenderAABB(Scene *scene, unsigned int depthTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    
    ResourceManager::GetShader("DebugAABB").Use();

    glBindVertexArray(m_aabbVAO);
    for(auto* collidable : scene->GetCollidables())
    {
        const auto& aabb = collidable->collider.worldAABB;

        glm::vec3 vertices[8];
        buildAABBVertices(aabb.center, aabb.extents, vertices);

        glBindBuffer(GL_ARRAY_BUFFER, m_aabbVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    }

    glEnable(GL_DEPTH_TEST);
}

void DebugPass::configureAABBBuffers()
{
        unsigned int indices[24] =
    {
        0,1, 1,2, 2,3, 3,0,
        4,5, 5,6, 6,7, 7,4,
        0,4, 1,5, 2,6, 3,7
    };

    glGenVertexArrays(1, &m_aabbVAO);
    glGenBuffers(1, &m_aabbVBO);
    glGenBuffers(1, &m_aabbEBO);

    glBindVertexArray(m_aabbVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_aabbVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 8, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_aabbEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
}

void DebugPass::buildAABBVertices(const glm::vec3 &center, const glm::vec3 &extents, glm::vec3 out[8])
{
    glm::vec3 min = center - extents;
    glm::vec3 max = center + extents;

    out[0] = {min.x, min.y, min.z};
    out[1] = {max.x, min.y, min.z};
    out[2] = {max.x, max.y, min.z};
    out[3] = {min.x, max.y, min.z};

    out[4] = {min.x, min.y, max.z};
    out[5] = {max.x, min.y, max.z};
    out[6] = {max.x, max.y, max.z};
    out[7] = {min.x, max.y, max.z};
}