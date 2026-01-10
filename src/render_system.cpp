#include "render_system.h"

#include "light.h"
#include "direction_light.h"
#include "resource_manager.h"

#include <iostream>

#pragma region lifecycle
RenderSystem::RenderSystem(unsigned int width, unsigned int height) : m_width(width), m_height(height) 
{
    m_cameraUBO = 0;
    m_dirLightUBO = 0;
    m_cubeVAO = 0;
    m_cubeVBO = 0;
}

RenderSystem::~RenderSystem()
{
    if(m_cameraUBO) glDeleteBuffers(1, &m_cameraUBO);
    if(m_dirLightUBO) glDeleteBuffers(1, &m_dirLightUBO);
    if(m_cubeVBO) glDeleteBuffers(1, &m_cubeVBO);
    if(m_cubeVAO) glDeleteVertexArrays(1, &m_cubeVAO);
    delete m_geometryPass;
    delete m_pbrPass;
}

#pragma endregion

#pragma region loop
void RenderSystem::Init()
{
    configureUBO();
    // create render pass obj
    m_geometryPass = new GeometryPass(m_width, m_height);
    m_pbrPass = new PBRPass();
}

void RenderSystem::BeginFrame(Scene *scene)
{
    if(scene->GetRenderType() == RenderType::Deferred)
    {
        updateUBO(scene);
        // geometry pass
        m_geometryPass->Begin();
    }
}

void RenderSystem::Render(Scene *scene, float dt)
{
    if(scene->GetRenderType() == RenderType::Deferred)
    {
        // draw geometry
        scene->Render(dt);
        m_geometryPass->End();
        // pbr lighting pass
        m_pbrPass->Begin();
        m_pbrPass->Render(m_geometryPass->GetTextaures(), scene->GetIBLData());
        m_pbrPass->End();
        
        // render skybox with forward rendering
        renderSkyBox(scene->GetIBLData().envCubeMap, m_geometryPass->GetGBuffer());
    }
}

void RenderSystem::EndFrame()
{

}

#pragma endregion

#pragma region UBO
void RenderSystem::configureUBO()
{
    // configure uniform buffer object
    // camera uniform block
    glGenBuffers(1, &m_cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // lights uniform block
    glGenBuffers(1, &m_dirLightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_dirLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirLightBlockUBO), nullptr, GL_STATIC_DRAW);  // dirlight variables don't change frequently
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_dirLightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderSystem::updateUBO(Scene* scene)
{
    // null check
    if(!scene) return;

    // update camera UBO
    Camera* camera = scene->GetCamera();
    if(camera && m_cameraUBO != 0)
    {
        CameraUBO data;
        data.projection = camera->GetProjectionMatrix(m_width, m_height);
        data.view = camera->GetViewMatrix();
        data.cameraPos = camera->cameraPos;

        glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUBO), &data);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // update dirlight UBO
    const auto& lights = scene->GetLights();
    for(const auto& light : lights)
    {
        // set light variables
        if(light->GetLightType() == LightType::Direction && m_dirLightUBO != 0)
        {
            DirLightBlockUBO data;
            // lighting variables
            data.light.colorIntensity.x = light->Color.x;
            data.light.colorIntensity.y = light->Color.y;
            data.light.colorIntensity.z = light->Color.z;
            data.light.colorIntensity.w = light->Intensity;
            data.light.direction = static_cast<DirLight*>(light)->Direction;
            // bool variable
            data.hasDirLight = 1;

            // configure uniform buffer data
            glBindBuffer(GL_UNIFORM_BUFFER, m_dirLightUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightBlockUBO), &data);
        }
    }
}

#pragma endregion

#pragma region skybox

void RenderSystem::renderSkyBox(unsigned int envCubeMap, unsigned int gBuffer)
{
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ResourceManager::GetShader("background").Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    renderCube();

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void RenderSystem::renderCube()
{
    // initialize (if necessary)
    if (m_cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &m_cubeVAO);
        glGenBuffers(1, &m_cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(m_cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

#pragma endregion