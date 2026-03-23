#include "render/pass/post_process_pass.h"
#include "resource_manager.h"
#include "scene/scene.h"

#include <glad/glad.h>

PostProcessPass::PostProcessPass() : m_quadVAO(0), m_quadVBO(0)
{
    ResourceManager::LoadShader("shaders/post_process/post_process.vert", "shaders/post_process/post_process.frag", nullptr, nullptr, nullptr, "postProcessing");
    ResourceManager::LoadTexture("resources/texture/DirtMaskTextureExample.png", true, "dirtMask", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    m_shader = &ResourceManager::GetShader("postProcessing");
    m_shader->Use();
    m_shader->SetInteger("scene", 0);
    m_shader->SetInteger("bloomBlur", 1);
    m_shader->SetInteger("dirtMaskTexture", 2);
}

PostProcessPass::~PostProcessPass()
{
    if(m_quadVBO) glDeleteBuffers(1, &m_quadVBO);
    if(m_quadVAO) glDeleteVertexArrays(1, &m_quadVAO);
}

void PostProcessPass::Render(Scene *scene, unsigned int pbrTexture, unsigned int bloomTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pbrTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);
    glActiveTexture(GL_TEXTURE2);
    ResourceManager::GetTexture("dirtMask").Bind(2);
    m_shader->SetFloat("exposure", 1.0f);

    if(scene->GetSceneState() == SceneState::End)
        m_shader->SetInteger("state", 1);
    else
        m_shader->SetInteger("state", 0);

    renderQuad();
}

void PostProcessPass::renderQuad()
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